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
//		ws_gpu_spritebuffer_start 	= ws_ioRam[0x06];
//		ws_gpu_spritebuffer_end		= ws_ioRam[0x05];
		// update buffered sprite table
//		GPMEMCPY((uint32*)ws_gpu_spritetable_buffer,(uint32*)(internalRam+(((uint32)ws_ioRam[0x04])<<9)),256*sizeof(uint32));
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

/*
	if (GPSTRLEN(statepath)<4)
		GPSPRINTF(newPath,"%s.wss",statepath);
	else
	{
		int len=GPSTRLEN(statepath);
		if ((statepath[len-1]!='s')&&(statepath[len-1]!='S'))
			GPSPRINTF(newPath,"%s.wss",statepath);
		else
		if ((statepath[len-2]!='s')&&(statepath[len-2]!='S'))
			GPSPRINTF(newPath,"%s.wss",statepath);
		else
		if ((statepath[len-3]!='w')&&(statepath[len-3]!='W'))
			GPSPRINTF(newPath,"%s.wss",statepath);
		else
		if (statepath[len-4]!='.')
			GPSPRINTF(newPath,"%s.wss",statepath);
		else
			GPSPRINTF(newPath,"%s",statepath);
	}
*/

	GPSPRINTF(newPath,"%s",statepath);

	result=GpFileCreate(newPath,ALWAYS_CREATE,&F);

	if (result==SM_OK)
	{
		GpFileWrite(F,&crc,sizeof(crc));
		MacroStoreNecRegisterToFile(F,NEC_IP);
		MacroStoreNecRegisterToFile(F,NEC_AW);
		MacroStoreNecRegisterToFile(F,NEC_BW);
		MacroStoreNecRegisterToFile(F,NEC_CW);
		MacroStoreNecRegisterToFile(F,NEC_DW);
		MacroStoreNecRegisterToFile(F,NEC_CS);
		MacroStoreNecRegisterToFile(F,NEC_DS);
		MacroStoreNecRegisterToFile(F,NEC_ES);
		MacroStoreNecRegisterToFile(F,NEC_SS);
		MacroStoreNecRegisterToFile(F,NEC_IX);
		MacroStoreNecRegisterToFile(F,NEC_IY);
		MacroStoreNecRegisterToFile(F,NEC_BP);
		MacroStoreNecRegisterToFile(F,NEC_SP);
		MacroStoreNecRegisterToFile(F,NEC_FLAGS);
		MacroStoreNecRegisterToFile(F,NEC_VECTOR);
		MacroStoreNecRegisterToFile(F,NEC_PENDING);
		MacroStoreNecRegisterToFile(F,NEC_NMI_STATE);
		MacroStoreNecRegisterToFile(F,NEC_IRQ_STATE);

		GpFileWrite(F,internalRam,65536);
		GpFileWrite(F,ws_staticRam,65536);
		GpFileWrite(F,ws_ioRam,256);
		GpFileWrite(F,ws_paletteColors,8);
		GpFileWrite(F,ws_palette,16*4*2);
		GpFileWrite(F,wsc_palette,16*16*2);
		GpFileWrite(F,&ws_videoMode,1);
		GpFileWrite(F,&ws_gpu_scanline,1);
		GpFileWrite(F,externalEeprom,131072);


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
	uint8	ws_newVideoMode;

	result = GpFileOpen(statepath,OPEN_R, &F);
	
	if (result != SM_OK)
		return(-1);

	GpFileRead(F, &newCrc, sizeof(crc), &sizeread);

	if (newCrc!=crc)
	{
		GpFileClose(F);
		return(-1);
	}

	MacroLoadNecRegisterFromFile(F,NEC_IP);
	MacroLoadNecRegisterFromFile(F,NEC_AW);
	MacroLoadNecRegisterFromFile(F,NEC_BW);
	MacroLoadNecRegisterFromFile(F,NEC_CW);
	MacroLoadNecRegisterFromFile(F,NEC_DW);
	MacroLoadNecRegisterFromFile(F,NEC_CS);
	MacroLoadNecRegisterFromFile(F,NEC_DS);
	MacroLoadNecRegisterFromFile(F,NEC_ES);
	MacroLoadNecRegisterFromFile(F,NEC_SS);
	MacroLoadNecRegisterFromFile(F,NEC_IX);
	MacroLoadNecRegisterFromFile(F,NEC_IY);
	MacroLoadNecRegisterFromFile(F,NEC_BP);
	MacroLoadNecRegisterFromFile(F,NEC_SP);
	MacroLoadNecRegisterFromFile(F,NEC_FLAGS);
	MacroLoadNecRegisterFromFile(F,NEC_VECTOR);
	MacroLoadNecRegisterFromFile(F,NEC_PENDING);
	MacroLoadNecRegisterFromFile(F,NEC_NMI_STATE);
	MacroLoadNecRegisterFromFile(F,NEC_IRQ_STATE);

	GpFileRead(F, internalRam,		65536,	&sizeread);
	GpFileRead(F, ws_staticRam,		65536,	&sizeread);
	GpFileRead(F, ws_ioRam,			256,	&sizeread);
	GpFileRead(F, ws_paletteColors,	8,		&sizeread);
	GpFileRead(F, ws_palette,		16*4*2,	&sizeread);
	GpFileRead(F, wsc_palette,		16*16*2,&sizeread);
	GpFileRead(F, &ws_newVideoMode,	1,		&sizeread);
	GpFileRead(F, &ws_gpu_scanline,	1,		&sizeread);
	GpFileRead(F, externalEeprom,	131072,	&sizeread);

//	ws_audio_readState(fp);
	GpFileClose(F);

	// force a video mode change to make all tiles dirty
	ws_gpu_clearCache();
	return(1);
}
