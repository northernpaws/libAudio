#include "libAudio.h"
#include "libAudio_Common.h"

/*!
 * @internal
 * @file loadAudio.cpp
 * @brief The implementation of the master encoder API
 * @author Rachel Mant <dx-mon@users.sourceforge.net>
 * @date 2010-2013
 */

/*!
 * \c ExternalPlayback defaults on library initialisation to 0 and holds whether or not
 * internal playback initialisation is active or not via being a truth value of whether
 * external playback is wanted or not - 0 means internal is active.
 */
uint8_t ExternalPlayback = 0;

/*!
 * This function opens the file given by \c FileName for reading and playback and returns a pointer
 * to the context of the opened file which must be used only by Audio_* functions
 * @param FileName The name of the file to open
 * @return A void pointer to the context of the opened file, or \c NULL if there was an error
 */
void *Audio_OpenR(const char *FileName)
{
	AudioPointer *ret = (AudioPointer *)malloc(sizeof(AudioPointer));
	if (ret == NULL)
		return NULL;
	if (Is_OggVorbis(FileName) == true)
		ret->API = &OggVorbisDecoder;
	else if (Is_FLAC(FileName) == true)
		ret->API = &FLACDecoder;
	else if (Is_WAV(FileName) == true)
		ret->API = &WAVDecoder;
	else if (Is_M4A(FileName) == true)
		ret->API = &M4ADecoder;
	else if (Is_AAC(FileName) == true)
		ret->API = &AACDecoder;
	else if (Is_MP3(FileName) == true)
		ret->API = &MP3Decoder;
	else if (Is_IT(FileName) == true)
		ret->API = &ITDecoder;
	else if (Is_MOD(FileName) == true)
		ret->API = &MODDecoder;
	else if (Is_S3M(FileName) == true)
		ret->API = &S3MDecoder;
	else if (Is_STM(FileName) == true)
		ret->API = &STMDecoder;
	else if (Is_AON(FileName) == true)
		ret->API = &AONDecoder;
#ifdef __FC1x_EXPERIMENTAL__
	else if (Is_FC1x(FileName) == true)
		ret->API = &FC1xDecoder;
#endif
	else if (Is_MPC(FileName) == true)
		ret->API = &MPCDecoder;
	else if (Is_WavPack(FileName) == true)
		ret->API = &WavPackDecoder;
#ifndef __NO_OptimFROG__
	else if (Is_OptimFROG(FileName) == true)
		ret->API = &OptimFROGDecoder;
#endif
	// Add RealAudio call here once decoder is complete
#ifdef __WMA__
	else if (Is_WMA(FileName) == true)
		ret->API = &WMADecoder;
#endif
	else
	{
		free(ret);
		return NULL;
	}

	ret->p_AudioFile = ret->API->OpenR(FileName);
	return ret;
}

/*!
 * This function gets the \c FileInfo structure for an opened file
 * @param p_AudioPtr A pointer to a file opened with \c Audio_OpenR(), or \c NULL for a no-operation
 * @return A \c FileInfo pointer containing various metadata about an opened file or \c NULL
 * @warning This function must be called before using \c Audio_Play() or \c Audio_FillBuffer()
 */
FileInfo *Audio_GetFileInfo(void *p_AudioPtr)
{
	AudioPointer *p_AP = (AudioPointer *)p_AudioPtr;
	if (p_AP == NULL || p_AP->p_AudioFile == NULL)
		return NULL;
	return p_AP->API->GetFileInfo(p_AP->p_AudioFile);
}

/*!
 * If using external playback or not using playback at all but rather wanting
 * to get PCM data, this function will do that by filling a buffer of any given length
 * with audio from an opened file.
 * @param p_AudioPtr A pointer to a file opened with \c Audio_OpenR(), or \c NULL for a no-operation
 * @param OutBuffer A pointer to the buffer to be filled
 * @param nOutBufferLen An integer giving how long the output buffer is as a maximum fill-length
 * @return Either a negative value when an error condition is entered,
 * or the number of bytes written to the buffer
 */
long Audio_FillBuffer(void *p_AudioPtr, uint8_t *OutBuffer, int nOutBufferLen)
{
	AudioPointer *p_AP = (AudioPointer *)p_AudioPtr;
	if (p_AP == NULL || OutBuffer == NULL || p_AP->p_AudioFile == NULL)
		return -3;
	return p_AP->API->FillBuffer(p_AP->p_AudioFile, OutBuffer, nOutBufferLen);
}

/*!
 * Closes an opened audio file
 * @param p_AudioPtr A pointer to a file opened with \c Audio_OpenR(), or \c NULL for a no-operation
 * @return an integer indicating success or failure with the same values as \c fclose()
 * @warning Do not use the pointer given by \p p_AudioPtr after using
 * this function - please either set it to \c NULL or be extra carefull
 * to destroy it via scope
 */
int Audio_CloseFileR(void *p_AudioPtr)
{
	AudioPointer *p_AP = (AudioPointer *)p_AudioPtr;
	API_Functions *API;
	void *p_AudioFile;
	if (p_AP == NULL || p_AP->p_AudioFile == NULL)
		return 0;
	API = p_AP->API;
	p_AudioFile = p_AP->p_AudioFile;
	free(p_AP);
	return API->CloseFileR(p_AudioFile);
}

/*!
 * Plays an opened audio file using OpenAL on the default audio device
 * @param p_AudioPtr A pointer to a file opened with \c Audio_OpenR()
 * @warning If \c ExternalPlayback was a non-zero value for
 * the call to \c Audio_OpenR() used to open the file at \p p_AudioPtr,
 * this function will do nothing.
 */
void Audio_Play(void *p_AudioPtr)
{
	AudioPointer *p_AP = (AudioPointer *)p_AudioPtr;
	if (p_AP != NULL && p_AP->p_AudioFile != NULL)
		p_AP->API->Play(p_AP->p_AudioFile);
}

void Audio_Pause(void *p_AudioPtr)
{
	AudioPointer *p_AP = (AudioPointer *)p_AudioPtr;
	if (p_AP != NULL && p_AP->p_AudioFile != NULL)
		p_AP->API->Pause(p_AP->p_AudioFile);
}

void Audio_Stop(void *p_AudioPtr)
{
	AudioPointer *p_AP = (AudioPointer *)p_AudioPtr;
	if (p_AP != NULL && p_AP->p_AudioFile != NULL)
		p_AP->API->Stop(p_AP->p_AudioFile);
}

/*!
 * Checks the file given by \p FileName for whether it is an audio
 * file recognised by this library or not
 * @param FileName The name of the file to check
 * @return \c true if the file can be utilised by the library,
 * otherwise \c false
 * @note This function does not check the file extension, but rather
 * the file contents to see if it is audio or not
 */
bool Is_Audio(const char *FileName)
{
	if (Is_OggVorbis(FileName) == true)
		return true;
	else if (Is_FLAC(FileName) == true)
		return true;
	else if (Is_WAV(FileName) == true)
		return true;
	else if (Is_M4A(FileName) == true)
		return true;
	else if (Is_AAC(FileName) == true)
		return true;
	else if (Is_MP3(FileName) == true)
		return true;
	else if (Is_IT(FileName) == true)
		return true;
	else if (Is_MOD(FileName) == true)
		return true;
	else if (Is_S3M(FileName) == true)
		return true;
	else if (Is_STM(FileName) == true)
		return true;
	else if (Is_AON(FileName) == true)
		return true;
	else if (Is_FC1x(FileName) == true)
		return true;
#ifndef __NO_MPC__
	else if (Is_MPC(FileName) == true)
		return true;
#endif
	else if (Is_WavPack(FileName) == true)
		return true;
#ifndef __NO_OptimFROG__
	else if (Is_OptimFROG(FileName) == true)
		return true;
#endif
#ifdef __WMA__
	// Add RealAudio call here when decoder is complete
	else if (Is_WMA(FileName) == true)
		return true;
#endif

	return false;
}
