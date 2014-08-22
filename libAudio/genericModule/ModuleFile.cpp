#include "../libAudio.h"
#include "../libAudio_Common.h"
#include "genericModule.h"

#ifndef _WINDOWS
#ifndef max
#define max(a, b) (a > b ? a : b)
#endif
#endif

void *ModuleAllocator::operator new(size_t size)
{
	void *ret = ::operator new(size);
	memset(ret, 0x00, size);
	return ret;
}

void *ModuleAllocator::operator new[](size_t size)
{
	void *ret = ::operator new[](size);
	memset(ret, 0x00, size);
	return ret;
}

ModuleFile::ModuleFile(MOD_Intern *p_MF) : ModuleType(MODULE_MOD), p_Instruments(NULL), Channels(NULL), MixerChannels(NULL)
{
	uint32_t i, maxPattern;
	FILE *f_MOD = p_MF->f_MOD;

	p_Header = new ModuleHeader(p_MF);
	fseek(f_MOD, 20, SEEK_SET);
	p_Samples = new ModuleSample *[p_Header->nSamples];
	for (i = 0; i < p_Header->nSamples; i++)
		p_Samples[i] = ModuleSample::LoadSample(p_MF, i);
	fseek(f_MOD, 130, SEEK_CUR);
	if (p_Header->nSamples != 15)
		fseek(f_MOD, 4, SEEK_CUR);

	// Count the number of patterns present
	for (i = 0, maxPattern = 0; i < 128; i++)
	{
		if (p_Header->Orders[i] < 64)
			maxPattern = max(maxPattern, p_Header->Orders[i]);
	}
	p_Header->nPatterns = maxPattern + 1;
	p_Patterns = new ModulePattern *[p_Header->nPatterns];
	for (i = 0; i < p_Header->nPatterns; i++)
		p_Patterns[i] = new ModulePattern(p_MF, p_Header->nChannels);

	MODLoadPCM(f_MOD);
	MinPeriod = 56;
	MaxPeriod = 7040;
}

ModuleFile::ModuleFile(S3M_Intern *p_SF) : ModuleType(MODULE_S3M), p_Instruments(NULL), Channels(NULL), MixerChannels(NULL)
{
	uint16_t i, *SamplePtrs, *PatternPtrs;
	FILE *f_S3M = p_SF->f_S3M;

	p_Header = new ModuleHeader(p_SF);
	p_Samples = new ModuleSample *[p_Header->nSamples];
	SamplePtrs = (uint16_t *)p_Header->SamplePtrs;
	for (i = 0; i < p_Header->nSamples; i++)
	{
		uint32_t SeekLoc = ((uint32_t)(SamplePtrs[i])) << 4;
		fseek(f_S3M, SeekLoc, SEEK_SET);
		p_Samples[i] = ModuleSample::LoadSample(p_SF, i);
	}

	// Count the number of channels present
	p_Header->nChannels = 32;
	for (i = 0; i < 32; i++)
	{
		if ((p_Header->ChannelSettings[i] & 0x80) != 0)
		{
			p_Header->nChannels = i;
			break;
		}
	}

	p_Patterns = new ModulePattern *[p_Header->nPatterns];
	PatternPtrs = (uint16_t *)p_Header->PatternPtrs;
	for (i = 0; i < p_Header->nPatterns; i++)
	{
		uint32_t SeekLoc = ((uint32_t)PatternPtrs[i]) << 4;
		fseek(f_S3M, SeekLoc, SEEK_SET);
		p_Patterns[i] = new ModulePattern(p_SF, p_Header->nChannels);
	}

	S3MLoadPCM(f_S3M);
	MinPeriod = 64;
	MaxPeriod = 32767;
}

ModuleFile::ModuleFile(STM_Intern *p_SF) : ModuleType(MODULE_STM), p_Instruments(NULL), Channels(NULL), MixerChannels(NULL)
{
	uint32_t i;
	FILE *f_STM = p_SF->f_STM;

	p_Header = new ModuleHeader(p_SF);
	p_Samples = new ModuleSample *[p_Header->nSamples];
	for (i = 0; i < p_Header->nSamples; i++)
		p_Samples[i] = ModuleSample::LoadSample(p_SF, i);
	fseek(f_STM, 128, SEEK_CUR);
	p_Patterns = new ModulePattern *[p_Header->nPatterns];
	for (i = 0; i < p_Header->nPatterns; i++)
		p_Patterns[i] = new ModulePattern(p_SF);
	fseek(f_STM, 1104 + (1024 * p_Header->nPatterns), SEEK_SET);

	STMLoadPCM(f_STM);
	MinPeriod = 64;
	MaxPeriod = 32767;
}

// http://www.tigernt.com/onlineDoc/68000.pdf
// http://eab.abime.net/showthread.php?t=21516
ModuleFile::ModuleFile(AON_Intern *p_AF) : ModuleType(MODULE_AON), p_Instruments(NULL), Channels(NULL), MixerChannels(NULL)
{
	char StrMagic[4];
	uint32_t BlockLen, i, SampleLengths;
	uint8_t ChannelMul;
	FILE *f_AON = p_AF->f_AON;

	p_Header = new ModuleHeader(p_AF);

	fread(StrMagic, 4, 1, f_AON);
	fread(&BlockLen, 4, 1, f_AON);
	BlockLen = Swap32(BlockLen);
	// 2 if 8 voices, 1 otherwise
	ChannelMul = p_Header->nChannels >> 2;
	// Transform that into a shift value to get the number of patterns
	ChannelMul += 9;
	if (strncmp(StrMagic, "PATT", 4) != 0 || (BlockLen % (1 << ChannelMul)) != 0)
		throw new ModuleLoaderError(E_BAD_AON);
	p_Header->nPatterns = BlockLen >> ChannelMul;
	p_Patterns = new ModulePattern *[p_Header->nPatterns];
	for (i = 0; i < p_Header->nPatterns; i++)
		p_Patterns[i] = new ModulePattern(p_AF, p_Header->nChannels);

	fread(StrMagic, 4, 1, f_AON);
	fread(&BlockLen, 4, 1, f_AON);
	BlockLen = Swap32(BlockLen);
	if (strncmp(StrMagic, "INST", 4) != 0 || (BlockLen % 32) != 0)
		throw new ModuleLoaderError(E_BAD_AON);
	p_Header->nInstruments = BlockLen >> 5;
	// TODO: Write instrument stuff up for main mixer to allow this to work
	fseek(f_AON, BlockLen, SEEK_CUR);

	fread(StrMagic, 4, 1, f_AON);
	fread(&BlockLen, 4, 1, f_AON);
	BlockLen = Swap32(BlockLen);
	if (strncmp(StrMagic, "INAM", 4) == 0)
	{
		// We don't care about the instrument names, so skip over them.
		fseek(f_AON, BlockLen, SEEK_CUR);
		fread(StrMagic, 4, 1, f_AON);
		fread(&BlockLen, 4, 1, f_AON);
		BlockLen = Swap32(BlockLen);
	}
	if (strncmp(StrMagic, "WLEN", 4) != 0 || BlockLen != 0x0100)
		throw new ModuleLoaderError(E_BAD_AON);
	p_Header->nSamples = BlockLen >> 2;
	p_Samples = new ModuleSample *[64];
	for (i = 0, SampleLengths = 0; i < 64; i++)
	{
		p_Samples[i] = ModuleSample::LoadSample(p_AF, i, NULL);
		SampleLengths += p_Samples[i]->GetLength();
	}

	fread(StrMagic, 4, 1, f_AON);
	fread(&BlockLen, 4, 1, f_AON);
	BlockLen = Swap32(BlockLen);
	if (strncmp(StrMagic, "WAVE", 4) != 0 || BlockLen != SampleLengths)
		throw new ModuleLoaderError(E_BAD_AON);

	AONLoadPCM(f_AON);
	MinPeriod = 56;
	MaxPeriod = 7040;
}

ModuleFile::ModuleFile(FC1x_Intern *p_FF) : ModuleType(MODULE_FC1x), p_Instruments(NULL), Channels(NULL), MixerChannels(NULL)
{
#ifdef __FC1x_EXPERIMENTAL__
//	FILE *f_FC1x = p_FF->f_FC1x;

	p_Header = new ModuleHeader(p_FF);
#endif
}

ModuleFile::ModuleFile(IT_Intern *p_IF) : ModuleType(MODULE_IT), p_Instruments(NULL), Channels(NULL), MixerChannels(NULL)
{
	uint16_t i;
	uint32_t *SamplePtrs, *PatternPtrs;
	FILE *f_IT = p_IF->f_IT;

	p_Header = new ModuleHeader(p_IF);
	if (p_Header->nInstruments != 0)
	{
		p_Instruments = new ModuleInstrument *[p_Header->nInstruments];
		SamplePtrs = (uint32_t *)p_Header->InstrumentPtrs;
		for (i = 0; i < p_Header->nInstruments; i++)
		{
			fseek(f_IT, SamplePtrs[i], SEEK_SET);
			p_Instruments[i] = ModuleInstrument::LoadInstrument(p_IF, i, p_Header->FormatVersion);
		}
	}
	p_Samples = new ModuleSample *[p_Header->nSamples];
	SamplePtrs = (uint32_t *)p_Header->SamplePtrs;
	for (i = 0; i < p_Header->nSamples; i++)
	{
		fseek(f_IT, SamplePtrs[i], SEEK_SET);
		p_Samples[i] = ModuleSample::LoadSample(p_IF, i);
	}

	p_Header->nChannels = 64;
	for (i = 0; i < 64; i++)
	{
		if (p_Header->Panning[i] > 128)
		{
			p_Header->nChannels = i;
			break;
		}
		else if (p_Header->Panning[i] <= 64)
			p_Header->Panning[i] *= 2;
		else if (p_Header->Panning[i] == 100)
			p_Header->Panning[i] = 64;
	}

	p_Patterns = new ModulePattern *[p_Header->nPatterns];
	PatternPtrs = (uint32_t *)p_Header->PatternPtrs;
	for (i = 0; i < p_Header->nPatterns; i++)
	{
		if (PatternPtrs[i] == 0)
			p_Patterns[i] = NULL;
		else
		{
			fseek(f_IT, PatternPtrs[i], SEEK_SET);
			p_Patterns[i] = new ModulePattern(p_IF, p_Header->nChannels);
		}
	}

	ITLoadPCM(f_IT);
	MinPeriod = 8;
	MaxPeriod = 61440;//32767;
}

ModuleFile::~ModuleFile()
{
	uint32_t i;

	DeinitMixer();

	for (i = 0; i < p_Header->nSamples; i++)
		delete [] p_PCM[i];
	delete [] p_PCM;
	for (i = 0; i < p_Header->nPatterns; i++)
		delete p_Patterns[i];
	delete [] p_Patterns;
	for (i = 0; i < p_Header->nSamples; i++)
		delete p_Samples[i];
	delete [] p_Samples;
	delete p_Header;
}

const char *ModuleFile::GetTitle()
{
	if (p_Header->Name == NULL)
		return NULL;
	return strdup(p_Header->Name);
}

const char *ModuleFile::GetAuthor()
{
	if (p_Header->Author == NULL)
		return NULL;
	return strdup(p_Header->Author);
}

const char *ModuleFile::GetRemark()
{
	if (p_Header->Remark == NULL)
		return NULL;
	return strdup(p_Header->Remark);
}

uint8_t ModuleFile::GetChannels()
{
	return (p_Header->MasterVolume & 0x80) == 0 ? 1 : 2;
}

void ModuleFile::MODLoadPCM(FILE *f_MOD)
{
	uint32_t i;
	p_PCM = new uint8_t *[p_Header->nSamples];
	for (i = 0; i < p_Header->nSamples; i++)
	{
		uint32_t Length = p_Samples[i]->GetLength();
		if (Length != 0)
		{
			p_PCM[i] = new uint8_t[Length];
			fread(p_PCM[i], Length, 1, f_MOD);
			p_PCM[i][0] = p_PCM[i][1] = 0;
			/*if (strncasecmp((char *)p_PCM[i] + 2, "ADPCM", 5) == 0)
			{
				uint32_t j;
				uint8_t *compressionTable = p_PCM[i];
				uint8_t *compBuffer = &p_PCM[i][16];
				uint8_t delta = 0;
				Length -= 16;
				p_Samples[i]->Length = Length;
				Length *= 2;
				p_PCM[i] = (uint8_t *)malloc(Length);
				for (j = 0; j < p_Samples[i]->Length; j++)
				{
					delta += compressionTable[compBuffer[j] & 0x0F];
					p_PCM[i][(j * 2) + 0] = delta;
					delta += compressionTable[(compBuffer[j] >> 4) & 0x0F];
					p_PCM[i][(j * 2) + 1] = delta;
				}
				free(compressionTable);
				compBuffer = compressionTable = NULL;
			}*/
		}
		else
			p_PCM[i] = NULL;
	}
}

void ModuleFile::S3MLoadPCM(FILE *f_S3M)
{
	uint32_t i;
	p_PCM = new uint8_t *[p_Header->nSamples];
	for (i = 0; i < p_Header->nSamples; i++)
	{
		uint32_t SeekLoc, Length = p_Samples[i]->GetLength() << (p_Samples[i]->Get16Bit() ? 1 : 0);
		if (Length != 0 && p_Samples[i]->GetType() == 1)
		{
			SeekLoc = ((ModuleSampleNative *)p_Samples[i])->SamplePos << 4;
			p_PCM[i] = new uint8_t[Length];
			fseek(f_S3M, SeekLoc, SEEK_SET);
			fread(p_PCM[i], Length, 1, f_S3M);
			if (p_Header->FormatVersion == 2)
			{
				uint32_t j;
				if (p_Samples[i]->Get16Bit())
				{
					uint16_t *pcm = (uint16_t *)p_PCM[i];
					for (j = 0; j < (Length >> 1); j++)
						pcm[j] ^= 0x8000;
				}
				else
				{
					uint8_t *pcm = (uint8_t *)p_PCM[i];
					for (j = 0; j < Length; j++)
						pcm[j] ^= 0x80;
				}
			}
		}
		else
			p_PCM[i] = NULL;
	}
}

void ModuleFile::STMLoadPCM(FILE *f_STM)
{
	uint32_t i;
	p_PCM = new uint8_t *[p_Header->nSamples];
	for (i = 0; i < p_Header->nSamples; i++)
	{
		uint32_t Length = p_Samples[i]->GetLength();
		if (Length != 0)
		{
			p_PCM[i] = new uint8_t[Length];
			fread(p_PCM[i], Length, 1, f_STM);
			fseek(f_STM, Length % 16, SEEK_CUR);
		}
		else
			p_PCM[i] = NULL;
	}
}

void ModuleFile::AONLoadPCM(FILE *f_AON)
{
	uint32_t i;
	p_PCM = new uint8_t *[64];
	for (i = 0; i < 64; i++)
	{
		uint32_t Length = p_Samples[i]->GetLength();
		if (Length != 0)
		{
			p_PCM[i] = new uint8_t[Length];
			fread(p_PCM[i], Length, 1, f_AON);
		}
		else
			p_PCM[i] = NULL;
	}
}

uint32_t ITBitstreamRead(uint8_t &buff, uint8_t &buffLen, FILE *f_IT, uint8_t bits)
{
	uint32_t ret = 0;
	uint8_t i = 0;

	if (bits > 0)
	{
		for (i = 0; i < bits; i++)
		{
			if (buffLen == 0)
			{
				fread(&buff, 1, 1, f_IT);
				buffLen = 8;
			}
			ret |= (buff & 1) << i;
			buff >>= 1;
			buffLen--;
		}
	}
	return ret;
}

void ITUnpackPCM8(ModuleSample *sample, uint8_t *PCM, FILE *f_IT, bool deltaComp)
{
	uint8_t buff, buffLen, bitWidth = 9;
	int8_t delta = 0, adjDelta = 0;
	uint32_t blockLen = 0, i = 0, Length;

	Length = sample->GetLength();
	while (Length != 0)
	{
		uint32_t j, offs = 0;
		if (blockLen == 0)
		{
			blockLen = 0x8000;
			buffLen = 0;
			ITBitstreamRead(buff, buffLen, f_IT, 16);
			bitWidth = 9;
			delta = 0;
			adjDelta = 0;
		}
		j = blockLen;
		if (j > Length)
			j = Length;
		do
		{
			uint16_t bits;
			bits = ITBitstreamRead(buff, buffLen, f_IT, bitWidth);
			if (feof(f_IT) == true)
				return;
			if (bitWidth < 7)
			{
				uint16_t special = 1 << (bitWidth - 1);
				if (bits == special)
				{
					uint8_t bits = ITBitstreamRead(buff, buffLen, f_IT, 3) + 1;
					if (bits < bitWidth)
						bitWidth = bits;
					else
						bitWidth = bits + 1;
					continue;
				}
			}
			else if (bitWidth < 9)
			{
				uint16_t special1 = (0xFF >> (9 - bitWidth)) + 4;
				uint16_t special2 = special1 - 8;
				if (bits > special2 && bits <= special1)
				{
					bits -= special2;
					if ((bits & 0xFF) < bitWidth)
						bitWidth = bits;
					else
						bitWidth = bits + 1;
					continue;
				}
			}
			else if (bitWidth > 9)
			{
				offs++;
				continue;
			}
			else if (bits >= 256)
			{
				bitWidth = bits + 1;
				continue;
			}
			if (bitWidth < 8)
			{
				uint8_t shift = 8 - bitWidth;
				bits = ((int8_t)(bits << shift)) >> shift;
			}
			delta += bits;
			adjDelta += delta;
			if (offs >= Length)
				return;
			PCM[i + offs] = deltaComp ? adjDelta : delta;
			offs++;
		}
		while (offs < j);
		i += j;
		Length -= j;
		blockLen -= j;
	}
}

void ITUnpackPCM16(ModuleSample *sample, uint16_t *PCM, FILE *f_IT, bool deltaComp)
{
	uint8_t buff, buffLen, bitWidth = 17;
	int16_t delta = 0, adjDelta = 0;
	uint32_t blockLen = 0, i = 0, Length;

	Length = sample->GetLength() >> 1;
	while (Length != 0)
	{
		uint32_t j, offs = 0;
		if (blockLen == 0)
		{
			blockLen = 0x4000;
			buffLen = 0;
			ITBitstreamRead(buff, buffLen, f_IT, 16);
			bitWidth = 17;
			delta = 0;
			adjDelta = 0;
		}
		j = blockLen;
		if (j > Length)
			j = Length;
		do
		{
			uint32_t bits;
			if (feof(f_IT) == true)
				return;
			bits = ITBitstreamRead(buff, buffLen, f_IT, bitWidth);
			if (bitWidth < 7)
			{
				uint32_t special = 1 << (bitWidth - 1);
				if (bits == special)
				{
					uint8_t bits = ITBitstreamRead(buff, buffLen, f_IT, 4) + 1;
					if (bits < bitWidth)
						bitWidth = bits;
					else
						bitWidth = bits + 1;
					continue;
				}
			}
			else if (bitWidth < 17)
			{
				uint16_t special1 = (0xFFFF >> (17 - bitWidth)) + 8;
				uint16_t special2 = special1 - 16;
				if (bits > special2 && bits <= special1)
				{
					bits -= special2;
					if ((bits & 0xFF) < bitWidth)
						bitWidth = bits;
					else
						bitWidth = bits + 1;
					continue;
				}
			}
			else if (bitWidth > 17)
			{
				offs++;
				continue;
			}
			else if (bits >= 65536)
			{
				bitWidth = bits + 1;
				continue;
			}
			if (bitWidth < 16)
			{
				uint8_t shift = 16 - bitWidth;
				bits = ((int16_t)(bits << shift)) >> shift;
			}
			delta += bits;
			adjDelta += delta;
			if (offs >= Length)
				return;
			PCM[i + offs] = deltaComp ? adjDelta : delta;
			offs++;
		}
		while (offs < j);
		i += j;
		Length -= j;
		blockLen -= j;
	}
}

template<typename T>
void StereoInterleve(T *pcmIn, T *pcmOut, uint32_t Length)
{
	for (uint32_t i = 0; i < Length; i++)
	{
		pcmOut[(i << 1) + 0] = pcmIn[i];
		pcmOut[(i << 1) + 1] = pcmIn[i + Length];
	}
}

void ModuleFile::ITLoadPCM(FILE *f_IT)
{
	uint32_t i;
	p_PCM = new uint8_t *[p_Header->nSamples];
	for (i = 0; i < p_Header->nSamples; i++)
	{
		ModuleSampleNative *Sample = ((ModuleSampleNative *)p_Samples[i]);
		uint32_t Length = p_Samples[i]->GetLength() << ((Sample->Get16Bit() ? 1 : 0) + (Sample->GetStereo() ? 1 : 0));
		if ((Sample->Flags & 0x01) == 0)
		{
			p_PCM[i] = NULL;
			continue;
		}
		p_PCM[i] = new uint8_t[Length];
		fseek(f_IT, Sample->SamplePos, SEEK_SET);
		if ((Sample->Flags & 0x08) != 0)
		{
			if (Sample->Get16Bit())
				ITUnpackPCM16(Sample, (uint16_t *)p_PCM[i], f_IT, p_Header->FormatVersion > 214 && Sample->Packing & 0x04);
			else
				ITUnpackPCM8(Sample, p_PCM[i], f_IT, p_Header->FormatVersion > 214 && Sample->Packing & 0x04);
		}
		else
			fread(p_PCM[i], Length, 1, f_IT);
		if ((Sample->Packing & 0x01) == 0)
		{
			uint32_t j;
			if (Sample->Get16Bit())
			{
				uint16_t *pcm = (uint16_t *)p_PCM[i];
				for (j = 0; j < (Length >> 1); j++)
					pcm[j] ^= 0x8000;
			}
			else
			{
				uint8_t *pcm = (uint8_t *)p_PCM[i];
				for (j = 0; j < Length; j++)
					pcm[j] ^= 0x80;
			}
		}
		if (Sample->GetStereo())
		{
			uint8_t *outBuff = new uint8_t[Length];
			if (Sample->Get16Bit())
				StereoInterleve((uint16_t *)p_PCM[i], (uint16_t *)outBuff, p_Samples[i]->GetLength());
			else
				StereoInterleve(p_PCM[i], outBuff, p_Samples[i]->GetLength());
			delete [] p_PCM[i];
			p_PCM[i] = outBuff;
		}
	}
}

#undef ModuleLoaderError

ModuleLoaderError::ModuleLoaderError(uint32_t error) : Error(error) { }

const char *ModuleLoaderError::GetError()
{
	switch (Error)
	{
		case E_BAD_S3M:
			return "Bad Scream Tracker III Module";
		case E_BAD_STM:
			return "Bad Scream Tracker Module - Maybe just song data?";
		case E_BAD_AON:
			return "Bad Art Of Noise Module";
		case E_BAD_FC1x:
			return "Bad Future Composer Module";
		case E_BAD_IT:
			return "Bad Impulse Tracker Module";
		default:
			return "Unknown error";
	}
}
