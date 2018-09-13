#include "../libAudio.h"
#include "../libAudio_Common.h"
#include "genericModule.h"

// Default initalise key fields
ModuleHeader::ModuleHeader() : RestartPos(255), GlobalVolume(64), InitialSpeed(6), InitialTempo(125), MasterVolume(64), Separation(128),
	Volumes{64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64}
{
}

ModuleHeader::ModuleHeader(const modMOD_t &file) : ModuleHeader{}
{
	constexpr const uint32_t seekOffset = (30 * 31) + 130;
	std::array<char, 4> magic;
	const fd_t &fd = file.fd();

	Name = makeUnique<char []>(21);
	Orders = makeUnique<uint8_t []>(128);
	nOrders = 0;

	if (!Name || !Orders ||
		!fd.read(Name, 20) ||
		fd.seek(seekOffset, SEEK_CUR) != (seekOffset + 20) ||
		!fd.read(magic) ||
		fd.seek(-134, SEEK_CUR) != (seekOffset - 130 + 20) ||
		!fd.read(&nOrders, 1) ||
		!fd.read(&RestartPos, 1) ||
		!fd.read(Orders.get(), 128))
		throw ModuleLoaderError(E_BAD_MOD);

	// Defaults
	nSamples = 31;
	nChannels = 4;
	// Now find out the actual values
	if (strncmp(magic.data(), "M.K.", 4) == 0 || strncmp(magic.data(), "M!K!", 4) == 0 ||
		strncmp(magic.data(), "M&K!", 4) == 0 || strncmp(magic.data(), "N.T.", 4) == 0)
		nChannels = 4;
	else if (strncmp(magic.data(), "CD81", 4) == 0 || strncmp(magic.data(), "OKTA", 4) == 0)
		nChannels = 8;
	else if (strncmp(magic.data(), "FLT", 3) == 0 && magic[3] >= '4' && magic[3] <= '9')
		nChannels = magic[3] - '0';
	else if (strncmp(magic.data() + 1, "CHN", 3) == 0 && magic[0] >= '4' && magic[0] <= '9')
		nChannels = magic[0] - '0';
	else if (strncmp(magic.data() + 2, "CH", 2) == 0 && magic[0] == '1' && magic[1] >= '0' && magic[1] <= '9')
		nChannels = magic[1] - '0' + 10;
	else if (strncmp(magic.data() + 2, "CH", 2) == 0 && magic[0] == '2' && magic[1] >= '0' && magic[1] <= '9')
		nChannels = magic[1] - '0' + 20;
	else if (strncmp(magic.data() + 2, "CH", 2) == 0 && magic[0] == '3' && magic[1] >= '0' && magic[1] <= '9')
		nChannels = magic[1] - '0' + 30;
	else if (strncmp(magic.data(), "TDZ", 3) == 0 && magic[3] >= '4' && magic[3] <= '9')
		nChannels = magic[3] - '0';
	else if (strncmp(magic.data(), "16CN", 4) == 0)
		nChannels = 16;
	else if (strncmp(magic.data(), "32CN", 4) == 0)
		nChannels = 32;
	else
		nSamples = 15;

	if (Name[19] != 0)
		Name[20] = 0;
	if (nOrders > 128)
		nOrders = 128;
	if (RestartPos > 127)
		RestartPos = 127;

	/********************************************\
	|* The following block just initialises the *|
	|* unused fields to harmless values.        *|
	\********************************************/
	Type = 0;
	Flags = 0;
	nInstruments = 0;
	CreationVersion = FormatVersion = 0;
	Author = nullptr;
	Remark = nullptr;
}

ModuleHeader::ModuleHeader(const modS3M_t &file) : ModuleHeader{}
{
	std::array<char, 4> magic;
	std::array<uint8_t, 10> dontCare;
	uint8_t Const;
	uint16_t Special, RawFlags;
	const fd_t &fd = file.fd();

	Name = makeUnique<char []>(29);
	if (!Name ||
		!fd.read(Name, 28) ||
		!fd.read(Const) ||
		!fd.read(Type) ||
		!fd.read<2>(dontCare) ||
		!fd.read(nOrders) ||
		!fd.read(nSamples) ||
		!fd.read(nPatterns) ||
		!fd.read(RawFlags) ||
		!fd.read(CreationVersion) ||
		!fd.read(FormatVersion) ||
		!fd.read(magic) ||
		!fd.read(GlobalVolume) ||
		!fd.read(InitialSpeed) ||
		!fd.read(InitialTempo) ||
		!fd.read(MasterVolume) ||
		!fd.read(dontCare) ||
		!fd.read(Special) ||
		!fd.read(ChannelSettings, 32))
		throw ModuleLoaderError(E_BAD_S3M);

	if (Name[27] != 0)
		Name[28] = 0;
	if ((RawFlags & 0x04) != 0)
		Flags |= FILE_FLAGS_AMIGA_SLIDES;
	if ((RawFlags & 0x10) != 0)
		Flags |= FILE_FLAGS_AMIGA_LIMITS;
	if (CreationVersion < 0x1320 && (RawFlags & 0x40) != 0)
		Flags |= FILE_FLAGS_FAST_SLIDES;

	if (Const != 0x1A || Type != 16 || FormatVersion > 2 || FormatVersion == 0 ||
		memcmp(magic.data(), "SCRM", 4) != 0)
		throw ModuleLoaderError(E_BAD_S3M);

	Orders = makeUnique<uint8_t []>(nOrders);
	SamplePtrs = new uint16_t[nSamples];
	PatternPtrs = new uint16_t[nPatterns];
	if (!Orders || !SamplePtrs || !PatternPtrs ||
		!fd.read(Orders.get(), nOrders) ||
		!fd.read(SamplePtrs.get(), nSamples * 2) ||
		!fd.read(PatternPtrs.get(), nPatterns * 2))
		throw ModuleLoaderError(E_BAD_S3M);

	// Panning?
	if (dontCare[1] == 0xFC)
	{
		uint8_t i;
		Panning = makeUnique<uint16_t []>(32);
		if (!Panning)
			throw ModuleLoaderError(E_BAD_S3M);
		for (i = 0; i < 32; i++)
		{
			uint8_t value;
			if (!fd.read(value))
				throw ModuleLoaderError(E_BAD_S3M);
			else if (value & 0x20)
				Panning[i] = ((value & 0x0F) << 4) | (value & 0x0F);
			else
				Panning[i] = 128;
		}
	}

	/********************************************\
	|* The following block just initialises the *|
	|* unused fields to harmless values.        *|
	\********************************************/
	nInstruments = 0;
	Author = nullptr;
	Remark = nullptr;
}

ModuleHeader::ModuleHeader(const modSTM_t &file) : ModuleHeader{}
{
	std::array<char, 9> magic;
	std::array<char, 13> reserved;
	uint8_t patternCount_;
	const fd_t &fd = file.fd();

	Name = makeUnique<char []>(21);
	Orders = makeUnique<uint8_t []>(128);
	if (!Name || !Orders ||
		!fd.read(Name, 20) ||
		!fd.read(magic) ||
		!fd.read(Type) ||
		!fd.read(FormatVersion) ||
		!fd.read(InitialSpeed) ||
		!fd.read(patternCount_) ||
		!fd.read(GlobalVolume) ||
		!fd.read(reserved))
		throw ModuleLoaderError(E_BAD_STM);

	InitialSpeed >>= 4;
	nPatterns = patternCount_;
	if (Name[19] != 0)
		Name[20] = 0;

	if (strncmp(magic.data(), "!Scream!\x1A", 9) != 0 || Type != 2)
		throw ModuleLoaderError(E_BAD_STM);

	nOrders = 128;
	if (fd.seek(1040, SEEK_SET) != 1040 ||
		!fd.read(Orders, 128) ||
		fd.seek(48, SEEK_SET) != 48)
		throw ModuleLoaderError(E_BAD_STM);

	for (uint8_t i = 0; i < nOrders; ++i)
	{
		if (Orders[i] >= 99)
			Orders[i] = 255;
	}
	nSamples = 31;
	nChannels = 4;

	/********************************************\
	|* The following block just initialises the *|
	|* unused fields to harmless values.        *|
	\********************************************/
	Flags = 0;
	nInstruments = 0;
	Author = nullptr;
	Remark = nullptr;
}

ModuleHeader::ModuleHeader(AON_Intern *p_AF) : ModuleHeader()
{
	char Magic1[4], Magic2[42];
	char StrMagic[4];
	uint32_t BlockLen;
	uint8_t Const, i;
	FILE *f_AON = p_AF->f_Module;

	fread(Magic1, 4, 1, f_AON);
	fread(Magic2, 42, 1, f_AON);
	fread(StrMagic, 4, 1, f_AON);
	fread(&BlockLen, 4, 1, f_AON);
	BlockLen = Swap32(BlockLen);

	if (strncmp(Magic1, "AON4", 4) != 0 && strncmp(Magic1, "AON8", 4) != 0 &&
		strncmp(Magic2, "artofnoise by bastian spiegel (twice/lego)", 42) != 0 &&
		strncmp(StrMagic, "NAME", 4) != 0)
		throw new ModuleLoaderError(E_BAD_AON);

	nChannels = Magic1[3] - '0';

	Name = makeUnique<char []>(BlockLen + 1);
	if (!Name)
		throw new ModuleLoaderError(E_BAD_AON);
	fread(Name.get(), BlockLen, 1, f_AON);
	Name[BlockLen] = 0;

	fread(StrMagic, 4, 1, f_AON);
	if (strncmp(StrMagic, "AUTH", 4) != 0)
		throw new ModuleLoaderError(E_BAD_AON);
	fread(&BlockLen, 4, 1, f_AON);
	BlockLen = Swap32(BlockLen);
	Author = new char [BlockLen + 1];
	fread(Author, BlockLen, 1, f_AON);
	Author[BlockLen] = 0;

	fread(StrMagic, 4, 1, f_AON);
	if (strncmp(StrMagic, "DATE", 4) != 0)
		throw new ModuleLoaderError(E_BAD_AON);
	fread(&BlockLen, 4, 1, f_AON);
	BlockLen = Swap32(BlockLen);
	fseek(f_AON, BlockLen, SEEK_CUR);

	fread(StrMagic, 4, 1, f_AON);
	if (strncmp(StrMagic, "RMRK", 4) != 0)
		throw new ModuleLoaderError(E_BAD_AON);
	fread(&BlockLen, 4, 1, f_AON);
	if (BlockLen != 0)
	{
		BlockLen = Swap32(BlockLen);
		Remark = new char[BlockLen + 1];
		fread(Remark, BlockLen, 1, f_AON);
		Remark[BlockLen] = 0;
	}
	else
		Remark = nullptr;

	fread(StrMagic, 4, 1, f_AON);
	fread(&BlockLen, 4, 1, f_AON);
	fread(&Const, 1, 1, f_AON);
	if (strncmp(StrMagic, "INFO", 4) != 0 || BlockLen != Swap32(4) ||
		Const != 0x34)
		throw new ModuleLoaderError(E_BAD_AON);
	fread(&Const, 1, 1, f_AON);
	nOrders = Const;
	fread(&Const, 1, 1, f_AON);
	RestartPos = Const;
	fread(&Const, 1, 1, f_AON);

	// Skip over the arpeggio table..
	fread(StrMagic, 4, 1, f_AON);
	fread(&BlockLen, 4, 1, f_AON);
	if (strncmp(StrMagic, "ARPG", 4) != 0 || BlockLen != Swap32(64))
		throw new ModuleLoaderError(E_BAD_AON);
	for (i = 0; i < 16; i++)
	{
		for (int j = 0; j < 4; j++)
			fread(&ArpTable[i][j], 1, 1, f_AON);
	}
	if (ArpTable[0][0] != 0 || ArpTable[0][1] != 0 ||
		ArpTable[0][2] != 0 || ArpTable[0][3] != 0)
		throw new ModuleLoaderError(E_BAD_AON);

	fread(StrMagic, 4, 1, f_AON);
	fread(&BlockLen, 4, 1, f_AON);
	BlockLen = Swap32(BlockLen);
	// If odd number of orders
	if ((nOrders & 1) != 0)
		// Get rid of fill byte from count
		BlockLen--;
	if (strncmp(StrMagic, "PLST", 4) != 0 || BlockLen != nOrders)
		throw new ModuleLoaderError(E_BAD_AON);
	Orders = makeUnique<uint8_t []>(nOrders);
	for (i = 0; i < nOrders; i++)
		fread(&Orders[i], 1, 1, f_AON);
	// If odd read length
	if ((BlockLen & 1) != 0)
		// Read the fill-byte
		fread(&Const, 1, 1, f_AON);

	/********************************************\
	|* The following block just initialises the *|
	|* unused fields to harmless values.        *|
	\********************************************/
	Type = 0;
	Flags = 0;
	CreationVersion = FormatVersion = 0;
	nInstruments = 0;
}

#ifdef __FC1x_EXPERIMENTAL__
ModuleHeader::ModuleHeader(FC1x_Intern *p_FF) : ModuleHeader()
{
	char Magic[4];
	uint32_t IDK1, IDK2;
	uint32_t Special;
	FILE *f_FC1x = p_FF->f_Module;

	fread(Magic, 4, 1, f_FC1x);
	fread(&SeqLength, 4, 1, f_FC1x);
	fread(&PatternOffs, 4, 1, f_FC1x);
	fread(&PatLength, 4, 1, f_FC1x);
	fread(&IDK1, 4, 1, f_FC1x);
	fread(&IDK2, 4, 1, f_FC1x);
	fread(&IDK1, 4, 1, f_FC1x);
	fread(&IDK2, 4, 1, f_FC1x);
	fread(&SampleOffs, 4, 1, f_FC1x);
	fread(&Special, 4, 1, f_FC1x);

	if (strncmp(Magic, "SMOD", 4) != 0 && strncmp(Magic, "FC14", 4) != 0)
		throw new ModuleLoaderError(E_BAD_FC1x);

	//

	/********************************************\
	|* The following block just initialises the *|
	|* unused fields to harmless values.        *|
	\********************************************/
	nInstruments = 0;
	Author = nullptr;
	Remark = nullptr;
}
#endif

ModuleHeader::ModuleHeader(const modIT_t &file) : ModuleHeader{}
{
	std::array<char, 4> magic;
	char DontCare[4];
	uint16_t MsgLength, SongFlags;
	uint8_t Const;
	const fd_t &fd = file.fd();

	if (!fd.read(magic) ||
		strncmp(magic.data(), "IMPM", 4) != 0)
		throw ModuleLoaderError(E_BAD_IT);

	Name = makeUnique<char []>(27);
	Panning = makeUnique<uint16_t []>(64);

	if (!Name || !Panning ||
		!fd.read(Name, 26) ||
		!fd.read(DontCare, 2) ||
		!fd.read(&nOrders, 2) ||
		!fd.read(&nInstruments, 2) ||
		!fd.read(&nSamples, 2) ||
		!fd.read(&nPatterns, 2) ||
		!fd.read(&CreationVersion, 2) ||
		!fd.read(&FormatVersion, 2) ||
		!fd.read(&SongFlags, 2) ||
		// TODO: Handle special.
		!fd.read(DontCare, 2) ||
		!fd.read(&GlobalVolume, 1) ||
		!fd.read(&MasterVolume, 1) ||
		!fd.read(&InitialSpeed, 1) ||
		!fd.read(&InitialTempo, 1) ||
		!fd.read(&Separation, 1) ||
		!fd.read(&Const, 1) ||
		!fd.read(&MsgLength, 2) ||
		!fd.read(&MessageOffs, 4) ||
		!fd.read(&DontCare, 4))
		throw ModuleLoaderError(E_BAD_IT);

	if (Name[25] != 0)
		Name[26] = 0;

	// This loop is unfortunately necessary to perform a width conversion
	// from the read value to our internal panning value.
	for (uint8_t i = 0; i < 64; i++)
	{
		uint8_t value;
		if (!fd.read(value))
			throw ModuleLoaderError(E_BAD_IT);
		Panning[i] = value;
	}

	Orders = makeUnique<uint8_t []>(nOrders);
	InstrumentPtrs = new uint32_t[nInstruments];
	// TODO: Implement managedPtr_t<> to handle type elision of uint32_t to void
	// for storage and retrieval + proper deletion of these.
	SamplePtrs = new uint32_t[nSamples];
	PatternPtrs = new uint32_t[nPatterns];

	if (!Orders || !InstrumentPtrs || !SamplePtrs || !PatternPtrs ||
		!fd.read(Volumes) ||
		!fd.read(Orders, nOrders) ||
		!fd.read(InstrumentPtrs, nInstruments * 4) ||
		!fd.read(SamplePtrs, nSamples * 4) ||
		!fd.read(PatternPtrs, nPatterns * 4))
		throw ModuleLoaderError(E_BAD_IT);

	Flags = 0;
	Flags |= (SongFlags & 0x0010) == 0 ? FILE_FLAGS_AMIGA_SLIDES : FILE_FLAGS_LINEAR_SLIDES;
	if ((SongFlags & 0x0004) == 0)
		nInstruments = 0;

	if (MessageOffs != 0)
	{
		Remark = new char[MsgLength + 1];
		if (fd.seek(MessageOffs, SEEK_SET) != MessageOffs ||
			!fd.read(Remark, MsgLength))
			throw ModuleLoaderError(E_BAD_IT);
		Remark[MsgLength] = 0;
	}

	/********************************************\
	|* The following block just initialises the *|
	|* unused fields to harmless values.        *|
	\********************************************/
	Author = nullptr;
	Remark = nullptr;
}

ModuleHeader::~ModuleHeader()
{
	delete [] Author;
	delete [] Remark;
}
