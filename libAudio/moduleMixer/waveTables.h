#ifndef __waveTables_H__
#define __waveTables_H__

const int8_t SinusTable[64] =
{
	0, 12, 25, 37, 49, 60, 71, 81, 90, 98, 106, 112, 117, 122, 125, 126,
	127, 126, 125, 122, 117, 112, 106, 98, 90, 81, 71, 60, 49, 37, 25, 12,
	0, -12, -25, -37, -49, -60, -71, -81, -90, -98, -106, -112, -117, -122, -125, -126,
	-127, -126, -125, -122, -117, -112, -106, -98, -90, -81, -71, -60, -49, -37, -25, -12
};

const int8_t RampDownTable[64] =
{
	124, 120, 116, 112, 108, 104, 100, 96, 92, 88, 84, 80, 76, 72, 68, 64,
	60, 56, 52, 48, 44, 40, 36, 32, 28, 24, 20, 16, 12, 8, 4, 0,
	-4, -8, -12, -16, -20, -24, -28, -32, -36, -40, -44, -48, -52, -56, -60, -64,
	-68, -72, -76, -80, -84, -88, -92, -96, -100, -104, -108, -112, -116, -120, -124, -128
};

const int8_t SquareTable[64] =
{
	127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
	-127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127,
	-127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127
};

// Temp. table. Will rewrite this via using a randomisation
// function to generate 64 signed char datapoints. Table will
// have it's pointer stored inside the MixerState
const int8_t RandomTable[64] =
{
	98, -127, -43, 88, 102, 41, -65, -94, 125, 20, -71, -86, -70, -32, -16, -96,
	17, 72, 107, -5, 116, -69, -62, -40, 10, -61, 65, 109, -18, -38, -13, -76,
	-23, 88, 21, -94, 8, 106, 21, -112, 6, 109, 20, -88, -30, 9, -127, 118,
	42, -34, 89, -4, -51, -72, 21, -29, 112, 123, 84, -101, -92, 98, -54, -95
};

const int8_t FT2VibratoTable[256] =
{
	0, -2, -3, -5, -6, -8, -9, -11, -12, -14, -16, -17, -19, -20, -22, -23,
	-24, -26, -27, -29, -30, -32, -33, -34, -36, -37, -38, -39, -41, -42,
	-43, -44, -45, -46, -47, -48, -49, -50, -51, -52, -53, -54, -55, -56,
	-56, -57, -58, -59, -59, -60, -60, -61, -61, -62, -62, -62, -63, -63,
	-63, -64, -64, -64, -64, -64, -64, -64, -64, -64, -64, -64, -63, -63,
	-63, -62, -62, -62, -61, -61, -60, -60, -59, -59, -58, -57, -56, -56,
	-55, -54, -53, -52, -51, -50, -49, -48, -47, -46, -45, -44, -43, -42,
	-41, -39, -38, -37, -36, -34, -33, -32, -30, -29, -27, -26, -24, -23,
	-22, -20, -19, -17, -16, -14, -12, -11, -9, -8, -6, -5, -3, -2, 0,
	2, 3, 5, 6, 8, 9, 11, 12, 14, 16, 17, 19, 20, 22, 23, 24, 26, 27, 29, 30,
	32, 33, 34, 36, 37, 38, 39, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
	52, 53, 54, 55, 56, 56, 57, 58, 59, 59, 60, 60, 61, 61, 62, 62, 62, 63,
	63, 63, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 63, 63, 63, 62, 62,
	62, 61, 61, 60, 60, 59, 59, 58, 57, 56, 56, 55, 54, 53, 52, 51, 50, 49,
	48, 47, 46, 45, 44, 43, 42, 41, 39, 38, 37, 36, 34, 33, 32, 30, 29, 27,
	26, 24, 23, 22, 20, 19, 17, 16, 14, 12, 11, 9, 8, 6, 5, 3, 2
};

#endif /*__waveTables_H__*/
