#include <stdio.h>
#include <malloc.h>
#include <math.h>

#include "libAudio.h"
#include "genericModule/genericModule.h"

modSTM_t::modSTM_t(fd_t &&fd) noexcept : moduleFile_t{audioType_t::moduleSTM, std::move(fd)} { }

modSTM_t *modSTM_t::openR(const char *const fileName) noexcept
{
	auto file = makeUnique<modSTM_t>(fd_t{fileName, O_RDONLY | O_NOCTTY});
	if (!file || !file->valid() || !isSTM(file->_fd))
		return nullptr;
	auto &ctx = *file->context();
	fileInfo_t &info = file->fileInfo();

	info.bitRate = 44100;
	info.bitsPerSample = 16;
	info.channels = 2;
	try { ctx.mod = makeUnique<ModuleFile>(*file); }
	catch (const ModuleLoaderError &e)
	{
		printf("%s\n", e.GetError());
		return nullptr;
	}
	info.title = ctx.mod->title();

	if (ToPlayback)
	{
		if (!ExternalPlayback)
			file->player(makeUnique<playback_t>(file.get(), audioFillBuffer, ctx.playbackBuffer, 8192, info));
		ctx.mod->InitMixer(info);
	}
	return file.release();
}

void *STM_OpenR(const char *fileName) { return modSTM_t::openR(fileName); }
const fileInfo_t *STM_GetFileInfo(void *p_STMFile) { return audioFileInfo(p_STMFile); }
long STM_FillBuffer(void *p_STMFile, uint8_t *OutBuffer, int nOutBufferLen)
	{ return audioFillBuffer(p_STMFile, OutBuffer, nOutBufferLen); }
int STM_CloseFileR(void *p_STMFile) { return audioCloseFile(p_STMFile); }
void STM_Play(void *p_STMFile) { audioPlay(p_STMFile); }
void STM_Pause(void *p_STMFile) { audioPause(p_STMFile); }
void STM_Stop(void *p_STMFile) { audioStop(p_STMFile); }
bool Is_STM(const char *fileName) { return modSTM_t::isSTM(fileName); }

bool modSTM_t::isSTM(const int32_t fd) noexcept
{
	constexpr uint32_t offset = 20;
	char STMMagic[9];
	if (fd == -1 ||
		lseek(fd, offset, SEEK_SET) != offset ||
		read(fd, STMMagic, 9) != 9 ||
		lseek(fd, 0, SEEK_SET) != 0 ||
		memcmp(STMMagic, "!Scream!\x1A", 9) != 0)
		return false;
	else
		return true;
}

bool modSTM_t::isSTM(const char *const fileName) noexcept
{
	fd_t file{fileName, O_RDONLY | O_NOCTTY};
	if (!file.valid())
		return false;
	return isSTM(file);
}

API_Functions STMDecoder =
{
	STM_OpenR,
	nullptr,
	audioFileInfo,
	nullptr,
	audioFillBuffer,
	nullptr,
	audioCloseFile,
	nullptr,
	audioPlay,
	audioPause,
	audioStop
};
