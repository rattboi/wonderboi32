////////////////////////////////////////////////////////////////////////////////
// Wonderswan emulator
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////

#include "types.h"
#include "gpstdlib.h"
#include "gpstdio.h"

#include <stdlib.h>

#include "rom.h"
#include "./nec/nec.h"
#include "./nec/necintrf.h"
#include "memory.h"
#include "gpu.h"
#include "io.h"
#include "ws.h"

//#include "file_dialog.h"
//extern int nFlip;


////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////

uint32	ws_cycles;
uint32	ws_skip;
uint32	ws_cyclesByLine=677;

uint32	vblank_count=0;

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
void ws_patchRom(void)
{
	uint8	*rom=memory_getRom();
	uint32	romSize=memory_getRomSize();

	if((rom[romSize-10]==0x01)&&(rom[romSize-8]==0x27)) /* Detective Conan */
	{
		/* WS cpu is using cache/pipeline or
		   there's protected ROM bank where
		   pointing CS
		*/
		rom[0xfffe8]=0xea;
		rom[0xfffe9]=0x00;
		rom[0xfffea]=0x00;
		rom[0xfffeb]=0x00;
		rom[0xfffec]=0x20;

	}

	ws_cyclesByLine=256; //677;

/*
	if((( rom[romSize-10]==0x01)&&(rom[romSize-8]==0x23))|| //Final Lap special
		((rom[romSize-10]==0x00)&&(rom[romSize-8]==0x17))|| //turn tablis
		((rom[romSize-10]==0x01)&&(rom[romSize-8]==0x08))|| //klonoa
		((rom[romSize-10]==0x26)&&(rom[romSize-8]==0x01))|| //ring infinity
		((rom[romSize-10]==0x01)&&(rom[romSize-8]==0x04))|| //puyo puyo 2
		((rom[romSize-10]==0x1b)&&(rom[romSize-8]==0x03))|| //rainbow islands
		((rom[romSize-10]==0x28)&&(rom[romSize-8]==0x01))|| //FF1
		((rom[romSize-10]==0x28)&&(rom[romSize-8]==0x02)))  //FF2
			ws_cyclesByLine=837;


	if(((rom[romSize-10]==0x01)&&(rom[romSize-8]==0x3)))//digimon tamers
		ws_cyclesByLine=574;
*/
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
int ws_init(uint8 *rom, uint32 romSize, char *filename)
{
	int len=GPSTRLEN(filename);

	if ((filename[len-1]=='c')||(filename[len-1]=='C'))
		ws_gpu_operatingInColor=1;
	else
		ws_gpu_operatingInColor=0;

	ws_memory_init(rom,romSize);
	ws_patchRom();
	ws_io_init();
	ws_gpu_init();

	return(1);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
void ws_reset(void)
{
	ws_memory_reset();
	ws_io_reset();
	ws_gpu_reset();
	nec_reset(0);
	nec_set_reg(NEC_SP, 0x2000);
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////


int ws_executeLine(int16 *framebuffer, int renderLine)
{
	int drawWholeScreen=0;
	uint32 uI;

	// update scanline register
	ws_ioRam[2]=ws_gpu_scanline;
	
	ws_cycles=nec_execute((ws_cyclesByLine>>1)+(rand()&7));
	ws_cycles+=nec_execute((ws_cyclesByLine>>1)+(rand()&7));


	if(ws_cycles>=ws_cyclesByLine+ws_cyclesByLine)
		ws_skip=ws_cycles/ws_cyclesByLine;
	else
		ws_skip=1;
	ws_cycles%=ws_cyclesByLine;

	for(uI=0;uI<ws_skip;uI++)
	{
	   if (renderLine)
		   ws_gpu_renderScanline(framebuffer);

	   ws_gpu_scanline++;
	   if(ws_gpu_scanline==144)
		   drawWholeScreen=1;

	}

	if(ws_gpu_scanline>158)
	{
		ws_gpu_scanline=0;
		{
			if((ws_ioRam[0xb2]&32))/*VBLANK END INT*/
			{
				if(ws_ioRam[0xa7]!=0x35)/*Beatmania Fix*/
				{
					ws_ioRam[0xb6]&=~32;
					nec_int((ws_ioRam[0xb0]+5)*4);
				}
			}
		}
	}
	ws_ioRam[2]=ws_gpu_scanline;
	if(drawWholeScreen)
	{

		if(ws_ioRam[0xb2]&64) /*VBLANK INT*/
		{
			ws_ioRam[0xb6]&=~64;
			nec_int((ws_ioRam[0xb0]+6)*4);
		}
		vblank_count++;
	}
   if(ws_ioRam[0xa4]&&(ws_ioRam[0xb2]&128)) /*HBLANK INT*/
   {

	  if(!ws_ioRam[0xa5])
		  ws_ioRam[0xa5]=ws_ioRam[0xa4];
	  if(ws_ioRam[0xa5])
		  ws_ioRam[0xa5]--;
	  if((!ws_ioRam[0xa5])&&(ws_ioRam[0xb2]&128))
	  {

		  ws_ioRam[0xb6]&=~128;
		  nec_int((ws_ioRam[0xb0]+7)*4);

	  }
  }
  if((ws_ioRam[0x2]==ws_ioRam[0x3])&&(ws_ioRam[0xb2]&16)) /*SCANLINE INT*/
  {
		ws_ioRam[0xb6]&=~16;
		nec_int((ws_ioRam[0xb0]+4)*4);
  }
  return(drawWholeScreen);
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
void ws_done(void)
{
	ws_memory_done();
	ws_io_done();
	ws_gpu_done();
}

int ws_rotated(void)
{
	uint8	*rom=memory_getRom();
	uint32	romSize=memory_getRomSize();

	return(rom[romSize-4]&1);
}
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////

#define MacroStoreNecRegisterToFile(F,R)        \
	    value=nec_get_reg(R); \
		GpFileWrite(F,&value,sizeof(value));

int	ws_saveState(char *statepath)
{
	F_HANDLE F;
	uint32 result;
	uint16	crc=memory_getRomCrc();
	unsigned	value;
	char		newPath[1024];
	gameState	*saveState;
	
	GPSPRINTF(newPath,"%s",statepath);

	result=GpFileCreate(newPath,ALWAYS_CREATE,&F);

	if (result==SM_OK)
	{
		saveState = GPMALLOC(sizeof(gameState));

		if (saveState != NULL)
		{
			saveStateToMem(saveState);
			GpFileWrite(F,&crc,sizeof(crc));
			GpFileWrite(F,saveState,sizeof(gameState));
		}
		GpFileClose(F);
	}
	else
	{
//		PrintError("Error Saving\nIs WB32\\SAV directory created?",0);
//		Delay(1000);
	}

	return(1);
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
#define MacroLoadNecRegisterFromFile(F,R)        \
		GpFileRead(F,&value,sizeof(value),&sizeread);		\
	    nec_set_reg(R,value);

int	ws_loadState(char *statepath)
{
	F_HANDLE F;
	uint32 result;
	ulong sizeread;

	uint16	crc=memory_getRomCrc();
	uint16	newCrc;
	unsigned	value;

	gameState	*saveState;

	result = GpFileOpen(statepath,OPEN_R, &F);
	
	if (result != SM_OK)
		return(-1);

	GpFileRead(F, &newCrc, sizeof(crc), &sizeread);

	if (newCrc!=crc)
	{
		GpFileClose(F);
		return(-1);
	}

	saveState	=	GPMALLOC(sizeof(gameState));

	if (saveState == NULL)
	{
		GpFileClose(F);
		return(-1);
	}

	GpFileRead(F, saveState, sizeof(gameState), &sizeread);
	GpFileClose(F);

	loadStateFromMem(saveState);

	// force a video mode change to make all tiles dirty
	ws_gpu_clearCache();
	return(1);
}

#define MacroStoreNecRegisterToMem(M,R)        \
		M->R = nec_get_reg(R);

void saveStateToMem(gameState *tempState)
{
	MacroStoreNecRegisterToMem(tempState,NEC_IP);
	MacroStoreNecRegisterToMem(tempState,NEC_AW);
	MacroStoreNecRegisterToMem(tempState,NEC_BW);
	MacroStoreNecRegisterToMem(tempState,NEC_CW);
	MacroStoreNecRegisterToMem(tempState,NEC_DW);
	MacroStoreNecRegisterToMem(tempState,NEC_CS);
	MacroStoreNecRegisterToMem(tempState,NEC_DS);
	MacroStoreNecRegisterToMem(tempState,NEC_ES);
	MacroStoreNecRegisterToMem(tempState,NEC_SS);
	MacroStoreNecRegisterToMem(tempState,NEC_IX);
	MacroStoreNecRegisterToMem(tempState,NEC_IY);
	MacroStoreNecRegisterToMem(tempState,NEC_BP);
	MacroStoreNecRegisterToMem(tempState,NEC_SP);
	MacroStoreNecRegisterToMem(tempState,NEC_FLAGS);
	MacroStoreNecRegisterToMem(tempState,NEC_VECTOR);
	MacroStoreNecRegisterToMem(tempState,NEC_PENDING);
	MacroStoreNecRegisterToMem(tempState,NEC_NMI_STATE);
	MacroStoreNecRegisterToMem(tempState,NEC_IRQ_STATE);

	GPMEMCPY(tempState->internalRam,		internalRam,		65536);
	GPMEMCPY(tempState->ws_staticRam,		ws_staticRam,		65536);
	GPMEMCPY(tempState->ws_ioRam,			ws_ioRam,			256);
	GPMEMCPY(tempState->ws_paletteColors,	ws_paletteColors,	8);
	GPMEMCPY(tempState->ws_palette,			ws_palette,			16*4*2);
	GPMEMCPY(tempState->wsc_palette,		wsc_palette,		16*16*2);
	GPMEMCPY(tempState->externalEeprom,		externalEeprom,		131072);

	tempState->ws_videoMode		= ws_videoMode;
	tempState->ws_gpu_scanline	= ws_gpu_scanline;
}

#define MacroLoadNecRegisterFromMem(M,R)        \
	    nec_set_reg(R,M->R);

void loadStateFromMem(gameState *tempState)
{
	uint8	ws_newVideoMode;

	MacroLoadNecRegisterFromMem(tempState,NEC_IP);
	MacroLoadNecRegisterFromMem(tempState,NEC_AW);
	MacroLoadNecRegisterFromMem(tempState,NEC_BW);
	MacroLoadNecRegisterFromMem(tempState,NEC_CW);
	MacroLoadNecRegisterFromMem(tempState,NEC_DW);
	MacroLoadNecRegisterFromMem(tempState,NEC_CS);
	MacroLoadNecRegisterFromMem(tempState,NEC_DS);
	MacroLoadNecRegisterFromMem(tempState,NEC_ES);
	MacroLoadNecRegisterFromMem(tempState,NEC_SS);
	MacroLoadNecRegisterFromMem(tempState,NEC_IX);
	MacroLoadNecRegisterFromMem(tempState,NEC_IY);
	MacroLoadNecRegisterFromMem(tempState,NEC_BP);
	MacroLoadNecRegisterFromMem(tempState,NEC_SP);
	MacroLoadNecRegisterFromMem(tempState,NEC_FLAGS);
	MacroLoadNecRegisterFromMem(tempState,NEC_VECTOR);
	MacroLoadNecRegisterFromMem(tempState,NEC_PENDING);
	MacroLoadNecRegisterFromMem(tempState,NEC_NMI_STATE);
	MacroLoadNecRegisterFromMem(tempState,NEC_IRQ_STATE);

	GPMEMCPY(internalRam,		tempState->internalRam,		65536);
	GPMEMCPY(ws_staticRam,		tempState->ws_staticRam,	65536);
	GPMEMCPY(ws_ioRam,			tempState->ws_ioRam,		256);
	GPMEMCPY(ws_paletteColors,	tempState->ws_paletteColors,8);
	GPMEMCPY(ws_palette,		tempState->ws_palette,		16*4*2);
	GPMEMCPY(wsc_palette,		tempState->wsc_palette,		16*16*2);
	GPMEMCPY(externalEeprom,	tempState->externalEeprom,	131072);

	ws_newVideoMode = tempState->ws_videoMode;
	ws_gpu_scanline = tempState->ws_gpu_scanline;
}
