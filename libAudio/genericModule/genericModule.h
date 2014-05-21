#ifndef __GenericModule_H__
#define __GenericModule_H__

/***************************\
|* ----=== WARNING ===---- *|
|*   This set of classes   *|
|* makes heavy use of the  *|
|*  friend keyword - make  *|
|* sure you do not modify  *|
|*   the relationships!!   *|
\***************************/

class ModuleFile;
class Channel;
class ModuleSample;
class ModulePattern;

#include "../ProTracker.h"
#include "../ScreamTracker.h"
#include "../ArtOfNoise.h"
#include "../FutureComposer.h"
#include "../ImpulseTracker.h"
#include "effects.h"

#define MIXBUFFERSIZE		512

#define MODULE_MOD		1
#define MODULE_S3M		2
#define MODULE_STM		3
#define MODULE_AON		4
#define MODULE_FC1x		5
#define MODULE_IT		6

#define E_BAD_S3M		1
#define E_BAD_STM		2
#define E_BAD_AON		3
#define E_BAD_FC1x		4
#define E_BAD_IT		5

#define FILE_FLAGS_AMIGA_SLIDES		0x01
#define FILE_FLAGS_AMIGA_LIMITS		0x02

#define SAMPLE_FLAGS_LOOP		1
#define SAMPLE_FLAGS_STEREO		2
#define SAMPLE_FLAGS_16BIT		4
#define SAMPLE_FLAGS_LPINGPONG	8

#define ENVELOPE_VOLUME		0
#define ENVELOPE_PANNING	1
#define ENVELOPE_PITCH		2

class ModuleLoaderError
{
private:
	uint32_t Error;

public:
	ModuleLoaderError(uint32_t Error);
	const char *GetError();
};

class ModuleAllocator
{
public:
	void *operator new(size_t s);
	void *operator new[](size_t s);
};

class ModuleHeader : public ModuleAllocator
{
private:
	// Common fields
	char *Name;
	char *Remark;
	uint16_t nOrders;
	uint16_t nSamples;
	uint16_t nInstruments;
	uint16_t nPatterns;
	uint8_t *Orders;
	uint8_t *Panning;
	uint16_t Flags;
	uint16_t CreationVersion;
	uint16_t FormatVersion;
	void *InstrumentPtrs;
	// Slightly badly named
	// SamplePtrs = pointers to where the sample *descriptors* are
	void *SamplePtrs;
	// PatternPtrs = pointers to where the compressed pattern data is
	void *PatternPtrs;

	// Fields specific to certain formats

	// MOD/AON
	uint8_t RestartPos;

	// S3M
	uint8_t Type;
	uint8_t GlobalVolume;
	uint8_t InitialSpeed;
	uint8_t InitialTempo;
	uint8_t MasterVolume;
	uint8_t ChannelSettings[32];

	// AON
	char *Author;
	char ArpTable[16][4];

#ifdef __FC1x_EXPERIMENTAL__
	// FC1x
	uint32_t SeqLength;
	uint32_t PatternOffs;
	uint32_t PatLength;
	uint32_t SampleOffs;
#endif

	// IT
	uint8_t Separation;
	uint32_t MessageOffs;
	uint8_t Volumes[64];

private:
	uint8_t nChannels;
	friend class ModuleFile;
	friend class Channel;

public:
	ModuleHeader(MOD_Intern *p_MF);
	ModuleHeader(S3M_Intern *p_SF);
	ModuleHeader(STM_Intern *p_SF);
	ModuleHeader(AON_Intern *p_AF);
#ifdef __FC1x_EXPERIMENTAL__
	ModuleHeader(FC1x_Intern *p_FF);
#endif
	ModuleHeader(IT_Intern *p_IF);
	~ModuleHeader();
};

class ModuleSample : public ModuleAllocator
{
protected:
	uint8_t Type;

private:
	uint32_t ID;
	friend class ModuleFile;

protected:
	ModuleSample(uint32_t ID, uint8_t Type);

public:
	static ModuleSample *LoadSample(MOD_Intern *p_MF, uint32_t i);
	static ModuleSample *LoadSample(S3M_Intern *p_SF, uint32_t i);
	static ModuleSample *LoadSample(STM_Intern *p_SF, uint32_t i);
	static ModuleSample *LoadSample(AON_Intern *p_AF, uint32_t i, char *Name);
	static ModuleSample *LoadSample(IT_Intern *p_IF, uint32_t i);

	virtual ~ModuleSample();
	uint8_t GetType();
	virtual uint32_t GetLength() = 0;
	virtual uint32_t GetLoopStart() = 0;
	virtual uint32_t GetLoopEnd() = 0;
	virtual uint8_t GetFineTune() = 0;
	virtual uint32_t GetC4Speed() = 0;
	virtual uint8_t GetVolume() = 0;
	virtual uint8_t GetVibratoSpeed() = 0;
	virtual uint8_t GetVibratoDepth() = 0;
	virtual uint8_t GetVibratoType() = 0;
	virtual uint8_t GetVibratoRate() = 0;
	virtual bool Get16Bit() = 0;
	virtual bool GetBidiLoop() = 0;
};

class ModuleSampleNative : public ModuleSample
{
private:
	char *Name;
	uint32_t Length;
	uint8_t FineTune;
	uint8_t Volume;
	uint8_t InstrVol;
	uint32_t LoopStart;
	uint32_t LoopEnd;

private:
	char *FileName;
	uint32_t SamplePos; // actually 24-bit..
	uint8_t Packing;
	uint8_t Flags, SampleFlags;
	uint32_t C4Speed;
	uint8_t DefaultPan;
	uint8_t VibratoSpeed;
	uint8_t VibratoDepth;
	uint8_t VibratoType;
	uint8_t VibratoRate;
	uint32_t SusLoopBegin;
	uint32_t SusLoopEnd;
	friend class ModuleFile;

public:
	ModuleSampleNative(MOD_Intern *p_MF, uint32_t i);
	ModuleSampleNative(S3M_Intern *p_SF, uint32_t i, uint8_t Type);
	ModuleSampleNative(STM_Intern *p_SF, uint32_t i);
	ModuleSampleNative(AON_Intern *p_AF, uint32_t i, char *Name);
	ModuleSampleNative(IT_Intern *p_IF, uint32_t i);
	~ModuleSampleNative();

	uint32_t GetLength();
	uint32_t GetLoopStart();
	uint32_t GetLoopEnd();
	uint8_t GetFineTune();
	uint32_t GetC4Speed();
	uint8_t GetVolume();
	uint8_t GetVibratoSpeed();
	uint8_t GetVibratoDepth();
	uint8_t GetVibratoType();
	uint8_t GetVibratoRate();
	bool Get16Bit();
	bool GetBidiLoop();
};

class ModuleSampleAdlib : public ModuleSample
{
private:
	char *FileName;
	uint8_t D00;
	uint8_t D01;
	uint8_t D02;
	uint8_t D03;
	uint8_t D04;
	uint8_t D05;
	uint8_t D06;
	uint8_t D07;
	uint8_t D08;
	uint8_t D09;
	uint8_t D0A;
	uint8_t D0B;
	uint8_t Volume;
	uint8_t DONTKNOW;
	uint32_t C4Speed;
	char *Name;

public:
	ModuleSampleAdlib(S3M_Intern *p_SF, uint32_t i, uint8_t Type);
	~ModuleSampleAdlib();

	uint32_t GetLength();
	uint32_t GetLoopStart();
	uint32_t GetLoopEnd();
	uint8_t GetFineTune();
	uint32_t GetC4Speed();
	uint8_t GetVolume();
	uint8_t GetVibratoSpeed();
	uint8_t GetVibratoDepth();
	uint8_t GetVibratoType();
	uint8_t GetVibratoRate();
	bool Get16Bit();
	bool GetBidiLoop();
};

class ModuleEnvelope : public ModuleAllocator
{
private:
	uint8_t Type;
	uint8_t Flags;
	uint8_t nNodes;
	uint8_t LoopBegin;
	uint8_t LoopEnd;
	uint8_t SusLoopBegin;
	uint8_t SusLoopEnd;
	uint8_t Nodes[75];

public:
	ModuleEnvelope(IT_Intern *p_IT, uint8_t env);
	uint8_t Apply(uint8_t Tick, uint8_t Value);
};

class ModuleInstrument : public ModuleAllocator
{
private:
	uint32_t ID;

protected:
	ModuleInstrument(uint32_t ID);

public:
	static ModuleInstrument *LoadInstrument(IT_Intern *p_IT, uint32_t i, uint16_t FormatVersion);

	virtual uint8_t Map(uint8_t Note) = 0;
};

class ModuleOldInstrument : public ModuleInstrument
{
public:
	ModuleOldInstrument(IT_Intern *p_IT, uint32_t i);

	uint8_t Map(uint8_t Note);
};

class ModuleNewInstrument : public ModuleInstrument
{
private:
	char *FileName;
	uint8_t NNA;
	uint8_t DCT;
	uint8_t DCA;
	uint16_t FadeOut;
	uint8_t PPS;
	uint8_t PPC;
	uint8_t Volume;
	uint8_t Panning;
	uint8_t RandVolume;
	uint8_t RandPanning;
	uint16_t TrackerVersion;
	uint8_t nSamples;
	char *Name;
	uint8_t SampleMapping[240];
	ModuleEnvelope **Envelopes;

public:
	ModuleNewInstrument(IT_Intern *p_IT, uint32_t i);
	~ModuleNewInstrument();

	uint8_t Map(uint8_t Note);
};

class ModuleCommand : public ModuleAllocator
{
private:
	uint8_t Sample;
	uint8_t Note;
	uint8_t VolEffect;
	uint8_t VolParam;
	uint8_t Effect;
	uint8_t Param;
	uint8_t ArpIndex;

	inline uint8_t MODPeriodToNoteIndex(uint16_t Period);
	void TranslateMODEffect(uint8_t Effect, uint8_t Param);
	friend class ModuleFile;
	friend class Channel;

public:
	void SetSample(uint8_t Sample);
	void SetVolume(uint8_t Volume);
	void SetMODData(uint8_t Data[4]);
	void SetS3MNote(uint8_t Note, uint8_t Sample);
	void SetS3MVolume(uint8_t Volume);
	void SetS3MEffect(uint8_t Effect, uint8_t Param);
	void SetSTMNote(uint8_t Note);
	void SetSTMEffect(uint8_t Effect, uint8_t Param);
	void SetAONNote(uint8_t Note);
	void SetAONArpIndex(uint8_t Index);
	void SetAONEffect(uint8_t Effect, uint8_t Param);
	void SetITRepVal(uint8_t ChannelMask, ModuleCommand &LastCommand);
	void SetITNote(uint8_t Note);
	void SetITVolume(uint8_t Volume);
	void SetITEffect(uint8_t Effect, uint8_t Param);
};

class ModulePattern : public ModuleAllocator
{
private:
	uint32_t Channels;
	ModuleCommand **Commands;

public:
	ModulePattern(MOD_Intern *p_MF, uint32_t nChannels);
	ModulePattern(S3M_Intern *p_SF, uint32_t nChannels);
	ModulePattern(STM_Intern *p_SF);
	ModulePattern(AON_Intern *p_AF, uint32_t nChannels);
	ModulePattern(IT_Intern *p_IF, uint32_t nChannels);
	~ModulePattern();

	ModuleCommand **GetCommands();
};

typedef struct _int16dot16_t
{
	uint16_t Lo;
	int16_t Hi;
} int16dot16_t;

typedef union _int16dot16
{
	int32_t iValue;
	int16dot16_t Value;
} int16dot16;

class Channel : public ModuleAllocator
{
public:
	uint8_t *SampleData, *NewSampleData;
	uint8_t Note, RampLength;
	uint8_t NewNote, NewSample;
	uint32_t LoopStart, LoopEnd, Length;
	uint8_t Volume, VolumeSlide;
	uint8_t ChannelVolume, ChannelVolumeSlide;
	ModuleSample *Sample;
	ModuleInstrument *Instrument;
	uint8_t FineTune, Panning, Arpeggio;
	uint8_t RowNote, RowSample, RowVolEffect;
	uint8_t RowEffect, RowVolParam, RowParam;
	uint16_t PortamentoSlide, Flags;
	uint32_t Period, C4Speed;
	uint32_t Pos, PosLo, StartTick;
	int16dot16 Increment;
	uint32_t PortamentoDest;
	uint8_t Portamento;
	uint8_t Tremor, TremorCount;
	uint8_t LeftVol, RightVol;
	uint8_t NewLeftVol, NewRightVol;
	short LeftRamp, RightRamp;
	int Filter_Y1, Filter_Y2, Filter_Y3, Filter_Y4;
	int Filter_A0, Filter_B0, Filter_B1, Filter_HP;
	uint8_t TremoloDepth, TremoloSpeed, TremoloPos, TremoloType;
	uint8_t VibratoDepth, VibratoSpeed, VibratoPos, VibratoType;
	uint8_t AutoVibratoPos;
	uint8_t PanbrelloDepth, PanbrelloSpeed, PanbrelloPos, PanbrelloType;
	int DCOffsR, DCOffsL;

public:
	Channel();
	void SetData(ModuleCommand *Command, ModuleHeader *p_Header);

	// Channel effects
	void NoteOff();
	void NoteCut(bool Triggered);
	void Vibrato(uint8_t param, uint8_t Multiplier);
	void Panbrello(uint8_t param);
};

class ModuleFile : public ModuleAllocator
{
private:
	uint8_t ModuleType;
	ModuleHeader *p_Header;
	ModuleSample **p_Samples;
	ModulePattern **p_Patterns;
	ModuleInstrument **p_Instruments;
	uint8_t **p_PCM;

	// Mixer info
	uint32_t MixSampleRate, MixChannels, MixBitsPerSample;
	uint32_t TickCount, SamplesToMix, MinPeriod, MaxPeriod;
	uint32_t Row, NextRow;
	uint32_t MusicSpeed, MusicTempo;
	uint32_t Pattern, NewPattern, NextPattern;
	uint32_t RowsPerBeat, SamplesPerTick;
	Channel *Channels;
	uint32_t nMixerChannels, *MixerChannels;

	uint8_t GlobalVolume, GlobalVolSlide;
	uint8_t PatternLoopCount, PatternLoopStart;
	uint8_t PatternDelay, FrameDelay;
	int MixBuffer[MIXBUFFERSIZE * 2];
	int DCOffsR, DCOffsL;

	// Effects functions
	void VolumeSlide(Channel *channel, uint8_t param);
	void ChannelVolumeSlide(Channel *channel, uint8_t param);
	void GlobalVolumeSlide(uint8_t param);
	void PortamentoUp(Channel *channel, uint8_t param);
	void PortamentoDown(Channel *channel, uint8_t param);
	void FinePortamentoUp(Channel *channel, uint8_t param);
	void FinePortamentoDown(Channel *channel, uint8_t param);
	void ExtraFinePortamentoUp(Channel *channel, uint8_t param);
	void ExtraFinePortamentoDown(Channel *channel, uint8_t param);
	void TonePortamento(Channel *channel, uint8_t param);
	int PatternLoop(uint32_t param);
	void ProcessMODExtended(Channel *channel);
	void ProcessS3MExtended(Channel *channel);

	// Processing functions
	bool AdvanceTick();
	bool Tick();
	bool ProcessEffects();
	void ResetChannelPanning();
	void SampleChange(Channel *channel, uint32_t sample);
	void ReloadSample(Channel *channel);
	void NoteChange(Channel * const channel, uint8_t note, uint8_t cmd);
	uint32_t GetPeriodFromNote(uint8_t Note, uint8_t FineTune, uint32_t C4Speed);
	uint32_t GetFreqFromPeriod(uint32_t Period, uint32_t C4Speed, int32_t PeriodFrac);

	// Mixing functions
	inline uint32_t GetSampleCount(Channel *chn, uint32_t Samples);
	inline void FixDCOffset(int *p_DCOffsL, int *p_DCOffsR, int *buff, uint32_t samples);
	void DCFixingFill(uint32_t samples);
	void CreateStereoMix(uint32_t count);
	inline void MonoFromStereo(uint32_t count);

private:
	void MODLoadPCM(FILE *f_MOD);
	void S3MLoadPCM(FILE *f_S3M);
	void STMLoadPCM(FILE *f_STM);
	void AONLoadPCM(FILE *f_AON);
	void ITLoadPCM(FILE *f_IT);
	void DeinitMixer();

public:
	ModuleFile(MOD_Intern *p_MF);
	ModuleFile(S3M_Intern *p_SF);
	ModuleFile(STM_Intern *p_SF);
	ModuleFile(AON_Intern *p_AF);
	ModuleFile(FC1x_Intern *p_FF);
	ModuleFile(IT_Intern *p_IF);
	~ModuleFile();

	const char *GetTitle();
	const char *GetAuthor();
	const char *GetRemark();
	uint8_t GetChannels();
	void InitMixer(FileInfo *p_FI);
	int32_t Mix(uint8_t *Buffer, uint32_t BuffLen);
};

inline uint32_t Swap32(uint32_t i)
{
	return ((i >> 24) & 0xFF) | ((i >> 8) & 0xFF00) |
		((i & 0xFF00) << 8) | ((i & 0xFF) << 24);
}

inline uint16_t Swap16(uint16_t i)
{
	return ((i >> 8) & 0xFF) | ((i & 0xFF) << 8);
}

#endif /*__GenericModule_H__*/
