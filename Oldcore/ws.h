//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
// Copyright(C)2000-2001 by david Raingeard
// cinati@caramail.com or david.raingeard@etu.univ-tours.fr
// For more information please read the readme.txt file
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __WS_H__
#define __WS_H__

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
	uint8	internalRam[65536];
	uint8	ws_staticRam[65536];
	uint8	ws_ioRam[256];
	uint8	ws_paletteColors[8];
	uint8	ws_palette[16*4*2];
	uint8	wsc_palette[16*16*2];
	uint8	ws_videoMode;
	uint8	ws_gpu_scanline;
	uint8	externalEeprom[131072];
} gameState;

int 	ws_init(uint8 *rom, uint32 romSize, char *filename);
void	ws_reset(void);
int		ws_executeLine(int16 *framebuffer, int renderLine);
void	ws_patchRom(void);
void	ws_done(void);
int 	ws_rotated(void);
int		ws_saveState(char *statepath);
int		ws_loadState(char *statepath);
void	saveStateToMem(gameState *tempState);
void	loadStateFromMem(gameState *tempState);

#endif
