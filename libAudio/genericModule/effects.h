#ifndef __effects_H__
#define __effects_H__

#define CMD_NONE			0x00
#define CMD_ARPEGGIO		0x01
#define CMD_PORTAMENTOUP	0x02
#define CMD_PORTAMENTODOWN	0x03
#define CMD_TONEPORTAMENTO	0x04
#define CMD_VIBRATO			0x05
#define CMD_TONEPORTAVOL	0x06
#define CMD_VIBRATOVOL		0x07
#define CMD_TREMOLO			0x08
#define CMD_OFFSET			0x09
#define CMD_VOLUMESLIDE		0x0A
#define CMD_POSITIONJUMP	0x0B
#define CMD_PATTERNBREAK	0x0C
#define CMD_SPEED			0x0D
#define CMD_MOD_EXTENDED	0x0E
#define CMD_S3M_EXTENDED	0x0F
#define CMD_RETRIGER		0x10
#define CMD_TEMPO			0x11
#define CMD_VOLUME			0x12
#define CMD_PANNING			0x13

#define VOLCMD_NONE			0x00
#define VOLCMD_VOLUME		0x01
#define VOLCMD_PANNING		0x02

#define CMD_MODEX_FILTER		0x00
#define CMD_MODEX_FINEPORTAUP	0x01
#define CMD_MODEX_FINEPORTADOWN	0x02
#define CMD_MODEX_GLISSANDO		0x03
#define CMD_MODEX_VIBRATOWAVE	0x04
#define CMD_MODEX_FINETUNE		0x05
#define CMD_MODEX_LOOP			0x06
#define CMD_MODEX_TREMOLOWAVE	0x07
#define CMD_MODEX_UNUSED		0x08
#define CMD_MODEX_RETRIGER		0x09
#define CMD_MODEX_FINEVOLUP		0x0A
#define CMD_MODEX_FINEVOLDOWN	0x0B
#define CMD_MODEX_CUT			0x0C
#define CMD_MODEX_DELAYSAMP		0x0D
#define CMD_MODEX_DELAYPAT		0x0E
#define CMD_MODEX_INVERTLOOP	0x0F

#define CMD_S3MEX_FILTER		0x00
#define CMD_S3MEX_GLISSANDO		0x01
#define CMD_S3MEX_FINETUNE		0x02
#define CMD_S3MEX_VIBRATOWAVE	0x03
#define CMD_S3MEX_TREMOLOWAVE	0x04
#define CMD_S3MEX_PANWAVE		0x05
#define CMD_S3MEX_FRAMEDELAY	0x06
#define CMD_S3MEX_ENVELOPE		0x07
#define CMD_S3MEX_PANNING		0x08
#define CMD_S3MEX_SURROUND		0x09
#define CMD_S3MEX_OFFSET		0x0A
#define CMD_S3MEX_LOOP			0x0B
#define CMD_S3MEX_NOTECUT		0x0C
#define CMD_S3MEX_DELAYSAMP		0x0D
#define CMD_S3MEX_DELAYPAT		0x0E

#endif /*__effects_H__*/

