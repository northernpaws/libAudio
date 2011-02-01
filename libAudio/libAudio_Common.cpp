#include <stdio.h>
#ifdef _WINDOWS
#include <windows.h>
#define _usleep _sleep
#else
#include <ctype.h>
#include <time.h>
#define MSECS_IN_SEC 1000
#define NSECS_IN_MSEC 1000000
#define _usleep(milisec) \
	{\
		struct timespec req = {milisec / MSECS_IN_SEC, (milisec % MSECS_IN_SEC) * NSECS_IN_MSEC}; \
		nanosleep(&req, NULL); \
	}
#endif
#include "libAudio.h"
#include "libAudio_Common.h"

int fseek_wrapper(void *p_file, __int64 offset, int origin)
{
	if (p_file == NULL)
		return -1;
	return fseek((FILE *)p_file, (long)offset, origin);
}

__int64 ftell_wrapper(void *p_file)
{
	return ftell((FILE *)p_file);
}

UINT Initialize_OpenAL(ALCdevice **pp_Dev, ALCcontext **pp_Ctx)
{
	float orient[6] = {0, 0, -1, 0, 1, 0};
	UINT ret;

	*pp_Dev = alcOpenDevice(NULL);
	*pp_Ctx = alcCreateContext(*pp_Dev, NULL);
	alcMakeContextCurrent(*pp_Ctx);

	alGenSources(1, &ret);

	alSourcef(ret, AL_GAIN, 1);
	alSourcef(ret, AL_PITCH, 1);
	alSource3f(ret, AL_POSITION, 0, 0, 0);
	alSource3f(ret, AL_VELOCITY, 0, 0, 0);
	alSource3f(ret, AL_DIRECTION, 0, 0, 0);
	alSourcef(ret, AL_ROLLOFF_FACTOR, 0);

	return ret;
}

UINT *CreateBuffers(UINT n, UINT nSize, UINT nChannels)
{
	UINT bufNum = 0;
	UINT *ret = (UINT *)malloc(sizeof(UINT) * n);
	alGenBuffers(n, ret);

	for (bufNum = 0; bufNum < n; bufNum++)
	{
		alBufferi(ret[bufNum], AL_SIZE, nSize);
		alBufferi(ret[bufNum], AL_CHANNELS, nChannels);
	}

	return ret;
}

void DestroyBuffers(UINT **buffs, UINT n)
{
	alDeleteBuffers(n, *buffs);
	*buffs = NULL;
}

void Deinitialize_OpenAL(ALCdevice **pp_Dev, ALCcontext **pp_Ctx, UINT Source)
{
	alDeleteSources(1, &Source);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(*pp_Ctx);
	alcCloseDevice(*pp_Dev);
	*pp_Ctx = NULL;
	*pp_Dev = NULL;
}

void QueueBuffer(UINT Source, UINT *p_BuffNum, int format, BYTE *Buffer, int nBuffSize, int BitRate)
{
	alBufferData(*p_BuffNum, format, Buffer, nBuffSize, BitRate);
	alSourceQueueBuffers(Source, 1, p_BuffNum);
}

void UnqueueBuffer(UINT Source, UINT *p_BuffNum)
{
	alSourceUnqueueBuffers(Source, 1, p_BuffNum);
}

int GetBuffFmt(int BPS, int Channels)
{
	if (Channels == 2)
		return AL_FORMAT_STEREO16;
	else if (Channels == 1)
		return AL_FORMAT_MONO16;
	/*else if (BPS == 8 && Channels == 2)
		return AL_FORMAT_STEREO8;
	else if (BPS == 8 && Channels == 1)
		return AL_FORMAT_MONO8;*/
	else
		return AL_FORMAT_STEREO16;
}

Playback::Playback(FileInfo *p_FI, FB_Func DataCallback, BYTE *BuffPtr, int nBuffLen, void *p_AudioPtr)
{
	float orient[6] = {0, 0, -1, 0, 1, 0};

	if (p_FI == NULL)
		return;
	if (DataCallback == NULL)
		return;
	if (BuffPtr == NULL)
		return;
	if (p_AudioPtr == NULL)
		return;

	this->p_FI = p_FI;
	FillBuffer = DataCallback;
	buffer = BuffPtr;
	nBufferLen = nBuffLen;
	this->p_AudioPtr = p_AudioPtr;

	// Initialize OpenAL ready
	device = alcOpenDevice(NULL);
	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);

	alGenSources(1, &sourceNum);

	alSourcef(sourceNum, AL_GAIN, 1);
	alSourcef(sourceNum, AL_PITCH, 1);
	alSource3f(sourceNum, AL_POSITION, 0, 0, 0);
	alSource3f(sourceNum, AL_VELOCITY, 0, 0, 0);
	alSource3f(sourceNum, AL_DIRECTION, 0, 0, 0);
	alSourcef(sourceNum, AL_ROLLOFF_FACTOR, 0);

	buffers = (UINT *)malloc(sizeof(UINT) * 4);
	alGenBuffers(4, buffers);

	for (UINT i = 0; i < 4; i++)
	{
		alBufferi(buffers[i], AL_SIZE, nBufferLen);
		alBufferi(buffers[i], AL_CHANNELS, p_FI->Channels);
	}
}

#ifdef __NICE_OUTPUT__
void DoDisplay(int *p_CN, int *p_P, char *Chars)
{
	int lenChars = strlen(Chars);
	if (((*p_P) % lenChars) == 0)
	{
		fprintf(stdout, "%c\b", Chars[(*p_CN)]);
		fflush(stdout);
		(*p_CN)++;
		if (*p_CN == lenChars)
			*p_CN = 0;
	}
	(*p_P)++;
	if (*p_P == lenChars)
		*p_P = 0;
}
#endif

void Playback::Play()
{
	long bufret = 1;
	int nBuffs = 0, Playing;
	int Fmt = GetBuffFmt(p_FI->BitsPerSample, p_FI->Channels);
#ifdef __NICE_OUTPUT__
	static char *ProgressChars = "\xB3/\-\\";
	int CharNum = 0, Proc = 0;
#endif

#ifdef __NICE_OUTPUT__
	fprintf(stdout, "Playing: *\b");
	fflush(stdout);
#endif
	bufret = FillBuffer(p_AudioPtr, buffer, nBufferLen);
	if (bufret > 0)
	{
		alBufferData(buffers[0], Fmt, buffer, nBufferLen, p_FI->BitRate);
		alSourceQueueBuffers(sourceNum, 1, &buffers[0]);
		nBuffs++;
	}
#ifdef __NICE_OUTPUT__
	DoDisplay(&CharNum, &Proc, ProgressChars);
#endif

	bufret = FillBuffer(p_AudioPtr, buffer, nBufferLen);
	if (bufret > 0)
	{
		alBufferData(buffers[1], Fmt, buffer, nBufferLen, p_FI->BitRate);
		alSourceQueueBuffers(sourceNum, 1, &buffers[1]);
		nBuffs++;
	}
#ifdef __NICE_OUTPUT__
	DoDisplay(&CharNum, &Proc, ProgressChars);
#endif

	bufret = FillBuffer(p_AudioPtr, buffer, nBufferLen);
	if (bufret > 0)
	{
		alBufferData(buffers[2], Fmt, buffer, nBufferLen, p_FI->BitRate);
		alSourceQueueBuffers(sourceNum, 1, &buffers[2]);
		nBuffs++;
	}
#ifdef __NICE_OUTPUT__
	DoDisplay(&CharNum, &Proc, ProgressChars);
#endif

	bufret = FillBuffer(p_AudioPtr, buffer, nBufferLen);
	if (bufret > 0)
	{
		alBufferData(buffers[3], Fmt, buffer, nBufferLen, p_FI->BitRate);
		alSourceQueueBuffers(sourceNum, 1, &buffers[3]);
		nBuffs++;
	}
#ifdef __NICE_OUTPUT__
	DoDisplay(&CharNum, &Proc, ProgressChars);
#endif

	alSourcePlay(sourceNum);

	while (bufret > 0)
	{
		int Processed;
		alGetSourcei(sourceNum, AL_BUFFERS_PROCESSED, &Processed);
		alGetSourcei(sourceNum, AL_SOURCE_STATE, &Playing);

		while (Processed--)
		{
			UINT buffer;

			alSourceUnqueueBuffers(sourceNum, 1, &buffer);
			bufret = FillBuffer(p_AudioPtr, this->buffer, nBufferLen);
			if (bufret <= 0)
			{
				nBuffs -= (Processed + 1);
				goto finish;
			}
#ifdef __NICE_OUTPUT__
			DoDisplay(&CharNum, &Proc, ProgressChars);
#endif
			alBufferData(buffer, Fmt, this->buffer, nBufferLen, p_FI->BitRate);
			alSourceQueueBuffers(sourceNum, 1, &buffer);

		}

		if (Playing != AL_PLAYING)
			alSourcePlay(sourceNum);
		_usleep(40);
	}

finish:
#ifdef __NICE_OUTPUT__
	fprintf(stdout, "*\n");
	fflush(stdout);
#endif
	alGetSourcei(sourceNum, AL_SOURCE_STATE, &Playing);
	while (nBuffs > 0)
	{
		int Processed;
		alGetSourcei(sourceNum, AL_BUFFERS_PROCESSED, &Processed);

		while (Processed--)
		{
			UINT buffer;
			alSourceUnqueueBuffers(sourceNum, 1, &buffer);
			nBuffs--;
		}

		if (Playing != AL_PLAYING)
			alSourcePlay(sourceNum);
		_usleep(40);
	}
}

Playback::~Playback()
{
	alDeleteBuffers(4, buffers);

	alDeleteSources(1, &sourceNum);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);

	free(p_FI);
}
