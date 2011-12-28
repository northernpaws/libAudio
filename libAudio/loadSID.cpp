#include "libAudio.h"
#include "libAudio_Common.h"

bool Is_SID(const char *FileName)
{
	FILE *f_SID = fopen(FileName, "rb");
	char SIDMagic[4];
	if (f_SID == NULL)
		return false;

	fread(&SIDMagic, 1, 4, f_SID);
	fclose(f_SID);

	if (strncmp(SIDMagic, "PSID", 4) == 0)
		return true;
	else
		return false;
}
