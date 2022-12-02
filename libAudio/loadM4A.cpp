// SPDX-License-Identifier: BSD-3-Clause
#include <cstdio>
#include <algorithm>

#include "m4a.hxx"

/*!
 * @internal
 * @file loadM4A.cpp
 * @brief The implementation of the M4A/MP4 decoder API
 * @author Rachel Mant <dx-mon@users.sourceforge.net>
 * @date 2009-2020
 */

namespace libAudio::loadM4A
{
	/*!
	 * @internal
	 * Internal function used to open the MP4 file for reading
	 * @param fileName The name of the file to open
	 * @param mode The \c MP4FileMode in which to open the file. We ensure this has
	 *    to be FILEMODE_CREATE for our purposes
	 */
	void *openR(const char *fileName, MP4FileMode mode)
	{
		if (mode != FILEMODE_READ)
			return nullptr;
		return fopen(fileName, "rb");
	}

	/*!
	 * @internal
	 * Internal function used to seek in the MP4 file
	 * @param file \c FILE handle for the MP4 file as a void pointer
	 * @param pos Possition into the file to which to seek to
	 */
	int seek(void *filePtr, int64_t pos)
	{
		auto *const file = static_cast<FILE *>(filePtr);
#ifdef _WINDOWS
		return (_fseeki64(file, pos, SEEK_SET) == 0 ? FALSE : TRUE);
#elif defined(__arm__) || defined(__aarch64__)
		return fseeko(file, pos, SEEK_SET) == 0 ? FALSE : TRUE;
#else
		return (fseeko64(file, pos, SEEK_SET) == 0 ? FALSE : TRUE);
#endif
	}

	/*!
	 * @internal
	 * Internal function used to read from the MP4 file
	 * @param file \c FILE handle for the MP4 file as a void pointer
	 * @param buffer A typeless buffer to which the read data should be written
	 * @param bufferLen A 64-bit integer giving how much data should be read from the file
	 * @param read A 64-bit integer count returning how much data was actually read
	 */
	int read(void *filePtr, void *buffer, int64_t bufferLen, int64_t *read, int64_t)
	{
		auto *const file = static_cast<FILE *>(filePtr);
		size_t ret = fread(buffer, 1, size_t(bufferLen), file);
		if (ret == 0 && bufferLen != 0)
			return TRUE;
		*read = ret;
		return FALSE;
	}

	/*!
	 * @internal
	 * Internal function used to write data to the MP4 file
	 * @param file \c FILE handle for the MP4 file as a void pointer
	 * @param buffer A typeless buffer holding the data to be written, which must also not become modified
	 * @param bufferLen A 64-bit integer giving how much data is to be written to the file
	 * @param written A 64-bit integer count returning how much data was actually written
	 */
	int write(void *filePtr, const void *buffer, int64_t bufferLen, int64_t *written, int64_t)
	{
		auto *const file = static_cast<FILE *>(filePtr);
		if (fwrite(buffer, 1, size_t(bufferLen), file) != size_t(bufferLen))
			return TRUE;
		*written = bufferLen;
		return FALSE;
	}

	/*!
	 * @internal
	 * Internal function used to close the MP4 file after I/O is complete
	 * @param file \c FILE handle for the MP4 file as a void pointer
	 */
	int close(void *file) { return fclose(static_cast<FILE *>(file)) != 0; }

	/*!
	 * @internal
	 * Structure holding pointers to the \c MP4Dec* functions given in this file.
	 * Used in the initialising of the MP4v2 file reader as a set of callbacks so
	 * as to prevent run-time issues on Windows.
	 */
	constexpr static MP4FileProvider ioFunctions =
	{
		openR,
		seek,
		read,
		write,
		close
	};
} // namespace libAudio::m4a

using namespace libAudio;

m4a_t::m4a_t(fd_t &&fd, audioModeRead_t) noexcept : audioFile_t(audioType_t::m4a, std::move(fd)),
	decoderCtx{makeUnique<decoderContext_t>()} { }
m4a_t::decoderContext_t::decoderContext_t() : decoder{NeAACDecOpen()}, mp4Stream{nullptr},
	track{MP4_INVALID_TRACK_ID}, frameCount{0}, currentFrame{0}, sampleCount{0}, samplesUsed{0},
	samples{nullptr}, eof{false}, playbackBuffer{} { }

/*!
 * @internal
 * Internal function used to determine the first usable audio track and initialise decoding on it
 * @param fileInfo The \c fileInfo_t structure to fill with the tracks's metadata
 * @return The MP4v2 track ID located for the decoder or -1 on error
 */
void m4a_t::decoderContext_t::aacTrack(fileInfo_t &fileInfo) noexcept
{
	/* find AAC track */
	const uint32_t trackCount = MP4GetNumberOfTracks(mp4Stream, nullptr, 0);

	for (uint32_t i = 0; i < trackCount; ++i)
	{
		const MP4TrackId trackID = MP4FindTrackId(mp4Stream, i, nullptr, 0);
		const char *type = MP4GetTrackType(mp4Stream, trackID);

		if (!MP4_IS_AUDIO_TRACK_TYPE(type))
			continue;
		track = trackID;

		uint8_t *buffer = nullptr;
		uint32_t bufferLen = 0;
		MP4GetTrackESConfiguration(mp4Stream, track, &buffer, &bufferLen);

		unsigned long sampleRate = 0;
		unsigned char channels = 0;
		if (NeAACDecInit2(decoder, buffer, bufferLen, &sampleRate, &channels))
			return finish(); // Return having cleaned up, rather than crash
		fileInfo.bitRate = sampleRate;
		fileInfo.channels = channels;
		MP4Free(buffer);

		NeAACDecConfiguration *const config = NeAACDecGetCurrentConfiguration(decoder);
		config->outputFormat = FAAD_FMT_16BIT;
		NeAACDecSetConfiguration(decoder, config);

		frameCount = MP4GetTrackNumberOfSamples(mp4Stream, track);
	}
	/* can't decode this */
}

void m4a_t::fetchTags() noexcept
{
	fileInfo_t &info = fileInfo();
	const MP4Tags *tags = MP4TagsAlloc();
	auto &ctx = *decoderContext();
	MP4TagsFetch(tags, ctx.mp4Stream);

	info.album = stringDup(tags->album);
	info.artist = stringDup(tags->artist ? tags->artist : tags->albumArtist);
	info.title = stringDup(tags->name);
	if (tags->comments)
		info.other.emplace_back(stringDup(tags->comments));

	info.bitsPerSample = 16;
	const uint32_t timescale = MP4GetTrackTimeScale(ctx.mp4Stream, ctx.track);
	info.totalTime = MP4GetTrackDuration(ctx.mp4Stream, ctx.track) / timescale;
}

/*!
 * Constructs a m4a_t using the file given by \c fileName for reading and playback
 * and returns a pointer to the context of the opened file
 * @param fileName The name of the file to open
 * @return A void pointer to the context of the opened file, or \c nullptr if there was an error
 */
m4a_t *m4a_t::openR(const char *const fileName) noexcept
{
	auto file{makeUnique<m4a_t>(fd_t{fileName, O_RDONLY | O_NOCTTY}, audioModeRead_t{})};
	if (!file || !file->valid() || !isM4A(file->_fd))
		return nullptr;
	auto &ctx = *file->decoderContext();
	fileInfo_t &info = file->fileInfo();

	ctx.mp4Stream = MP4ReadProvider(fileName, 0, &loadM4A::ioFunctions);
	ctx.aacTrack(info);
	if (!ctx.decoder)
		return nullptr;
	file->fetchTags();

	if (!ExternalPlayback)
		file->player(makeUnique<playback_t>(file.get(), audioFillBuffer, ctx.playbackBuffer, 8192, info));
	return file.release();
}

/*!
 * This function opens the file given by \c fileName for reading and playback and returns a pointer
 * to the context of the opened file which must be used only by M4A_* functions
 * @param fileName The name of the file to open
 * @return A void pointer to the context of the opened file, or \c nullptr if there was an error
 */
void *m4aOpenR(const char *fileName) { return m4a_t::openR(fileName); }

void m4a_t::decoderContext_t::finish() noexcept
{
	NeAACDecClose(decoder);
	decoder = nullptr;
	MP4Close(mp4Stream);
	mp4Stream = nullptr;
}

m4a_t::decoderContext_t::~decoderContext_t() noexcept { finish(); }

/*!
 * If using external playback or not using playback at all but rather wanting
 * to get PCM data, this function will do that by filling a buffer of any given length
 * with audio from an opened file.
 * @param bufferPtr A pointer to the buffer to be filled
 * @param length An integer giving how long the output buffer is as a maximum fill-length
 * @return Either a negative value when an error condition is entered,
 * or the number of bytes written to the buffer
 */
int64_t m4a_t::fillBuffer(void *const bufferPtr, const uint32_t length)
{
	auto &ctx = *decoderContext();
	auto buffer = static_cast<uint8_t *>(bufferPtr);
	uint32_t offset = 0;

	while (offset < length && !ctx.eof)
	{
		if (ctx.samplesUsed == ctx.sampleCount)
		{
			if (ctx.currentFrame < ctx.frameCount)
			{
				NeAACDecFrameInfo FI;
				uint8_t *frame = nullptr;
				uint32_t frameLen = 0;
				++ctx.currentFrame;
				if (!MP4ReadSample(ctx.mp4Stream, ctx.track, ctx.currentFrame, &frame, &frameLen))
				{
					ctx.eof = true;
					return -2;
				}
				ctx.samples = (uint8_t *)NeAACDecDecode(ctx.decoder, &FI, frame, frameLen);
				MP4Free(frame);

				ctx.sampleCount = FI.samples * FI.channels;
				ctx.samplesUsed = 0;
				if (FI.error != 0)
				{
					printf("Error: %s\n", NeAACDecGetErrorMessage(FI.error));
					ctx.sampleCount = 0;
					continue;
				}
			}
			else if (ctx.currentFrame == ctx.frameCount)
				return -1;
		}

		const uint32_t sampleCount = std::min(uint32_t(ctx.sampleCount -
			ctx.samplesUsed), length - offset);
		memcpy(buffer + offset, ctx.samples + ctx.samplesUsed, sampleCount);
		offset += sampleCount;
		ctx.samplesUsed += sampleCount;
	}

	return offset;
}

// Standard "ftyp" Atom for a MOV based MP4 AAC file:
// 00 00 00 20 66 74 79 70 4D 34 41 20
// .  .  .     f  t  y  p  M  4  A

/*!
 * Checks the file given by \p fileName for whether it is an MP4/M4A
 * file recognised by this library or not
 * @param fileName The name of the file to check
 * @return \c true if the file can be utilised by the library,
 * otherwise \c false
 * @note This function does not check the file extension, but rather
 * the file contents to see if it is an MP4/M4A file or not
 */
bool isM4A(const char *fileName) { return m4a_t::isM4A(fileName); }

/*!
 * Checks the file descriptor given by \p fd for whether it represents a MP4/M4A
 * file recognised by this library or not
 * @param fd The descriptor of the file to check
 * @return \c true if the file can be utilised by the library,
 * otherwise \c false
 * @note This function does not check the file extension, but rather
 * the file contents to see if it is a MP4/M4A file or not
 */
bool m4a_t::isM4A(const int32_t fd) noexcept
{
	char length[4], typeSig[4], fileType[4];
	if (fd == -1 ||
		read(fd, length, 4) != 4 ||
		read(fd, typeSig, 4) != 4 ||
		read(fd, fileType, 4) != 4 ||
		memcmp(typeSig, "ftyp", 4) != 0 ||
		(memcmp(fileType, "M4A ", 4) != 0 &&
		memcmp(fileType, "mp42", 4) != 0))
		return false;
	return true;
}

/*!
 * Checks the file given by \p fileName for whether it is a MP4/M4A
 * file recognised by this library or not
 * @param fileName The name of the file to check
 * @return \c true if the file can be utilised by the library,
 * otherwise \c false
 * @note This function does not check the file extension, but rather
 * the file contents to see if it is a MP4/M4A file or not
 */
bool m4a_t::isM4A(const char *const fileName) noexcept
{
	fd_t file(fileName, O_RDONLY | O_NOCTTY);
	if (!file.valid())
		return false;
	return isM4A(file);
}
