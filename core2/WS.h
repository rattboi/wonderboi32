//---------------------------------------------------------------------------
#ifndef WSH
#define WSH
#include "WSHard.h"
//---------------------------------------------------------------------------

extern int RunFlag;					// GB실행 프랙션″
extern int RunCount;
extern int SoundOn[7];
extern int FrameSkip;


typedef struct WsRomHeader_tag
{
	uint8	developerId;
	uint8	minimumSupportSystem;
	uint8	cartId;
	uint8	unused;
	uint8	romSize;
	uint8	ramSize;
	uint8	additionalCapabilities;
	uint8	realtimeClock;
	uint16	checksum;
} WsRomHeader;

typedef struct gameState_tag
{
	uint32	NEC_IP;
	uint32	NEC_AW;
	uint32	NEC_BW;
	uint32	NEC_CW;
	uint32	NEC_DW;
	uint32	NEC_CS;
	uint32	NEC_DS;
	uint32	NEC_ES;
	uint32	NEC_SS;
	uint32	NEC_IX;
	uint32	NEC_IY;
	uint32	NEC_BP;
	uint32	NEC_SP;
	uint32	NEC_FLAGS;
	uint32	NEC_VECTOR;
	uint32	NEC_PENDING;
	uint32	NEC_NMI_STATE;
	uint32	NEC_IRQ_STATE;
	byte 	IRAM[0x10000];			//internal ram 	(64k)
	byte	SRAM[0x10000];			//save ram		(64k) could be smaller, just use 64k
	byte	IORam[0x100];				//port IO ram	(256 bytes)
	uint8	MonoColor[8];
	uint8 	ColTbl[0x210];
	uint8	Palette[16+1][16][4];
	uint8	RSTRL;
} gameState;

extern int WsCreate(BYTE *RomBase, uint32 romSize);
extern int WsRun(void);
extern void WsRelease(void);

#endif
