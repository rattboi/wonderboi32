////////////////////////////////////////////////////////////////////////////////
// GPU
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

#include "rom.h"
#include "./nec/nec.h"
#include "io.h"
#include "gpu.h"

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
extern uint8	*internalRam;

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
#define RGB555(R,G,B) ((((int)(R))<<11)|(((int)(G))<<6)|(((int)(B))<<1))

uint8	ws_gpu_operatingInColor;
uint8	ws_gpu_forceColorSystemBool=0;
uint8	ws_gpu_forceMonoSystemBool=0;

uint32 ws_gpu_spritebuffer_start;
uint32 ws_gpu_spritebuffer_end;
uint32 ws_gpu_spritetable_buffer[256];

uint8	ws_videoMode;
uint8	ws_gpu_scanline=0;
int16	ws_palette[16*4];
int8	ws_paletteColors[8];
int16	wsc_palette[16*16];
int16	ws_shades[16]={	
						RGB555(30,30,30),
						RGB555(28,28,28),
						RGB555(26,26,26),
						RGB555(24,24,24),
						RGB555(22,22,22),
						RGB555(20,20,20),
						RGB555(18,18,18),
						RGB555(16,16,16),
						RGB555(14,14,14),
						RGB555(12,12,12),
						RGB555(10,10,10),
						RGB555(8,8,8),
						RGB555(6,6,6),
						RGB555(4,4,4),
						RGB555(2,2,2),
						RGB555(0,0,0)
					};
					
// white
#define SHADE_COLOR_RED		1.00
#define SHADE_COLOR_GREEN	1.00
#define SHADE_COLOR_BLUE	1.00

int16	ws_colour_scheme_default[16]={	
						RGB555(SHADE_COLOR_RED*30,SHADE_COLOR_GREEN*30,SHADE_COLOR_BLUE*30),
						RGB555(SHADE_COLOR_RED*28,SHADE_COLOR_GREEN*28,SHADE_COLOR_BLUE*28),
						RGB555(SHADE_COLOR_RED*26,SHADE_COLOR_GREEN*26,SHADE_COLOR_BLUE*26),
						RGB555(SHADE_COLOR_RED*24,SHADE_COLOR_GREEN*24,SHADE_COLOR_BLUE*24),
						RGB555(SHADE_COLOR_RED*22,SHADE_COLOR_GREEN*22,SHADE_COLOR_BLUE*22),
						RGB555(SHADE_COLOR_RED*20,SHADE_COLOR_GREEN*20,SHADE_COLOR_BLUE*20),
						RGB555(SHADE_COLOR_RED*18,SHADE_COLOR_GREEN*18,SHADE_COLOR_BLUE*18),
						RGB555(SHADE_COLOR_RED*16,SHADE_COLOR_GREEN*16,SHADE_COLOR_BLUE*16),
						RGB555(SHADE_COLOR_RED*14,SHADE_COLOR_GREEN*14,SHADE_COLOR_BLUE*14),
						RGB555(SHADE_COLOR_RED*12,SHADE_COLOR_GREEN*12,SHADE_COLOR_BLUE*12),
						RGB555(SHADE_COLOR_RED*10,SHADE_COLOR_GREEN*10,SHADE_COLOR_BLUE*10),
						RGB555(SHADE_COLOR_RED*8,SHADE_COLOR_GREEN*8,SHADE_COLOR_BLUE*8),
						RGB555(SHADE_COLOR_RED*6,SHADE_COLOR_GREEN*6,SHADE_COLOR_BLUE*6),
						RGB555(SHADE_COLOR_RED*4,SHADE_COLOR_GREEN*4,SHADE_COLOR_BLUE*4),
						RGB555(SHADE_COLOR_RED*2,SHADE_COLOR_GREEN*2,SHADE_COLOR_BLUE*2),
						RGB555(SHADE_COLOR_RED*0,SHADE_COLOR_GREEN*0,SHADE_COLOR_BLUE*0)
					};
// green
#undef  SHADE_COLOR_RED
#undef  SHADE_COLOR_GREEN
#undef  SHADE_COLOR_BLUE
#define SHADE_COLOR_RED		0.20
#define SHADE_COLOR_GREEN	0.90
#define SHADE_COLOR_BLUE	0.20

int16	ws_colour_scheme_green[16]={	
						RGB555(SHADE_COLOR_RED*30,SHADE_COLOR_GREEN*30,SHADE_COLOR_BLUE*30),
						RGB555(SHADE_COLOR_RED*28,SHADE_COLOR_GREEN*28,SHADE_COLOR_BLUE*28),
						RGB555(SHADE_COLOR_RED*26,SHADE_COLOR_GREEN*26,SHADE_COLOR_BLUE*26),
						RGB555(SHADE_COLOR_RED*24,SHADE_COLOR_GREEN*24,SHADE_COLOR_BLUE*24),
						RGB555(SHADE_COLOR_RED*22,SHADE_COLOR_GREEN*22,SHADE_COLOR_BLUE*22),
						RGB555(SHADE_COLOR_RED*20,SHADE_COLOR_GREEN*20,SHADE_COLOR_BLUE*20),
						RGB555(SHADE_COLOR_RED*18,SHADE_COLOR_GREEN*18,SHADE_COLOR_BLUE*18),
						RGB555(SHADE_COLOR_RED*16,SHADE_COLOR_GREEN*16,SHADE_COLOR_BLUE*16),
						RGB555(SHADE_COLOR_RED*14,SHADE_COLOR_GREEN*14,SHADE_COLOR_BLUE*14),
						RGB555(SHADE_COLOR_RED*12,SHADE_COLOR_GREEN*12,SHADE_COLOR_BLUE*12),
						RGB555(SHADE_COLOR_RED*10,SHADE_COLOR_GREEN*10,SHADE_COLOR_BLUE*10),
						RGB555(SHADE_COLOR_RED*8,SHADE_COLOR_GREEN*8,SHADE_COLOR_BLUE*8),
						RGB555(SHADE_COLOR_RED*6,SHADE_COLOR_GREEN*6,SHADE_COLOR_BLUE*6),
						RGB555(SHADE_COLOR_RED*4,SHADE_COLOR_GREEN*4,SHADE_COLOR_BLUE*4),
						RGB555(SHADE_COLOR_RED*2,SHADE_COLOR_GREEN*2,SHADE_COLOR_BLUE*2),
						RGB555(SHADE_COLOR_RED*0,SHADE_COLOR_GREEN*0,SHADE_COLOR_BLUE*0)
					};
// amber
#undef  SHADE_COLOR_RED
#undef  SHADE_COLOR_GREEN
#undef  SHADE_COLOR_BLUE
#define SHADE_COLOR_RED		1.00
#define SHADE_COLOR_GREEN	0.61
#define SHADE_COLOR_BLUE	0.00

int16	ws_colour_scheme_amber[16]={	
						RGB555(SHADE_COLOR_RED*30,SHADE_COLOR_GREEN*30,SHADE_COLOR_BLUE*30),
						RGB555(SHADE_COLOR_RED*28,SHADE_COLOR_GREEN*28,SHADE_COLOR_BLUE*28),
						RGB555(SHADE_COLOR_RED*26,SHADE_COLOR_GREEN*26,SHADE_COLOR_BLUE*26),
						RGB555(SHADE_COLOR_RED*24,SHADE_COLOR_GREEN*24,SHADE_COLOR_BLUE*24),
						RGB555(SHADE_COLOR_RED*22,SHADE_COLOR_GREEN*22,SHADE_COLOR_BLUE*22),
						RGB555(SHADE_COLOR_RED*20,SHADE_COLOR_GREEN*20,SHADE_COLOR_BLUE*20),
						RGB555(SHADE_COLOR_RED*18,SHADE_COLOR_GREEN*18,SHADE_COLOR_BLUE*18),
						RGB555(SHADE_COLOR_RED*16,SHADE_COLOR_GREEN*16,SHADE_COLOR_BLUE*16),
						RGB555(SHADE_COLOR_RED*14,SHADE_COLOR_GREEN*14,SHADE_COLOR_BLUE*14),
						RGB555(SHADE_COLOR_RED*12,SHADE_COLOR_GREEN*12,SHADE_COLOR_BLUE*12),
						RGB555(SHADE_COLOR_RED*10,SHADE_COLOR_GREEN*10,SHADE_COLOR_BLUE*10),
						RGB555(SHADE_COLOR_RED*8,SHADE_COLOR_GREEN*8,SHADE_COLOR_BLUE*8),
						RGB555(SHADE_COLOR_RED*6,SHADE_COLOR_GREEN*6,SHADE_COLOR_BLUE*6),
						RGB555(SHADE_COLOR_RED*4,SHADE_COLOR_GREEN*4,SHADE_COLOR_BLUE*4),
						RGB555(SHADE_COLOR_RED*2,SHADE_COLOR_GREEN*2,SHADE_COLOR_BLUE*2),
						RGB555(SHADE_COLOR_RED*0,SHADE_COLOR_GREEN*0,SHADE_COLOR_BLUE*0)
					};
					
					
uint8	*ws_tile_cache;
uint8	*wsc_tile_cache;

uint8	*ws_hflipped_tile_cache;
uint8	*wsc_hflipped_tile_cache;
uint8	*ws_modified_tile;
uint8	*wsc_modified_tile;

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
void ws_gpu_set_colour_scheme(int scheme)
{
	switch (scheme)
	{
	case COLOUR_SCHEME_DEFAULT: GPMEMCPY(ws_shades,ws_colour_scheme_default,16*sizeof(int16)); break;
	case COLOUR_SCHEME_AMBER  : GPMEMCPY(ws_shades,ws_colour_scheme_amber,16*sizeof(int16)); break;
	case COLOUR_SCHEME_GREEN  : GPMEMCPY(ws_shades,ws_colour_scheme_green,16*sizeof(int16)); break;
	}
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
void ws_gpu_forceColorSystem(void)
{
	ws_gpu_forceColorSystemBool=1;
	ws_gpu_forceMonoSystemBool=0;
	ws_gpu_operatingInColor=1;
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
void ws_gpu_forceMonoSystem(void)
{
	ws_gpu_forceColorSystemBool=0;
	ws_gpu_forceMonoSystemBool=1;
	ws_gpu_operatingInColor=0;
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
void ws_gpu_init(void)
{
	ws_tile_cache				= (uint8*)GPMALLOC(512*8*8);
	wsc_tile_cache				= (uint8*)GPMALLOC(1024*8*8);

	ws_hflipped_tile_cache		= (uint8*)GPMALLOC(512*8*8);
	wsc_hflipped_tile_cache		= (uint8*)GPMALLOC(1024*8*8);

	ws_modified_tile			= (uint8*)GPMALLOC(512);
	wsc_modified_tile			= (uint8*)GPMALLOC(1024);

	GPMEMSET(ws_tile_cache,0x00,512*8*8);
	GPMEMSET(wsc_tile_cache,0x00,1024*8*8);

	GPMEMSET(ws_hflipped_tile_cache,0x00,512*8*8);
	GPMEMSET(wsc_hflipped_tile_cache,0x00,1024*8*8);

	GPMEMSET(ws_modified_tile,0x01,512);
	GPMEMSET(wsc_modified_tile,0x01,1024);
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
void ws_gpu_done(void)
{
	GPFREE(ws_tile_cache);
	GPFREE(wsc_tile_cache);
	GPFREE(ws_hflipped_tile_cache);
	GPFREE(wsc_hflipped_tile_cache);
	GPFREE(ws_modified_tile);
	GPFREE(wsc_modified_tile);

	ws_tile_cache			= NULL;
	wsc_tile_cache			= NULL;
	ws_hflipped_tile_cache	= NULL;
	wsc_hflipped_tile_cache	= NULL;
	ws_modified_tile		= NULL;
	wsc_modified_tile		= NULL;
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
void ws_gpu_changeVideoMode(uint8 value)
{
	if (ws_videoMode!=(value>>5))
	{
		ws_videoMode=value>>5;
		if (ws_videoMode==4)
			ws_videoMode=2;

		// mark all tiles dirty
		GPMEMSET(wsc_modified_tile,0x01,1024);
		GPMEMSET(ws_modified_tile,0x01,512);
	}
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
void ws_gpu_reset(void)
{
	GPMEMSET(wsc_modified_tile,0x01,1024);
	GPMEMSET(ws_modified_tile,0x01,512);
	ws_gpu_scanline=0;
	ws_gpu_changeVideoMode(2);
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
void ws_gpu_clearCache(void)
{
	GPMEMSET(wsc_modified_tile,0x01,1024);
	GPMEMSET(ws_modified_tile,0x01,512);
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
uint8 *ws_tileCache_getTileRow(uint32 tileIndex, uint32 line, 
							   uint32 vFlip, uint32 hFlip, uint32 bank)
{
	int forline;
	if (ws_gpu_operatingInColor)
	{
		if (bank)
			tileIndex+=512;
		
		// need to update tile cache ?
		// 4 colors tiles
		if ((ws_videoMode==2)&&(ws_modified_tile[tileIndex]))
		{
			uint8	*tileInCachePtr = &wsc_tile_cache[tileIndex<<6];
			uint8	*hflippedTileInCachePtr = &wsc_hflipped_tile_cache[tileIndex<<6];
			uint16	*tileInRamPtr   = (uint16*)&internalRam[0x2000+(tileIndex<<4)];
			uint16	tileLine;

			for (forline=0;forline<8;forline++)
			{
				tileLine=*tileInRamPtr++;

				tileInCachePtr[0]=((tileLine&0x80)>>7)|((tileLine&0x8000)>>14);
				hflippedTileInCachePtr[7]=tileInCachePtr[0];
				tileInCachePtr[1]=((tileLine&0x40)>>6)|((tileLine&0x4000)>>13);
				hflippedTileInCachePtr[6]=tileInCachePtr[1];
				tileInCachePtr[2]=((tileLine&0x20)>>5)|((tileLine&0x2000)>>12);
				hflippedTileInCachePtr[5]=tileInCachePtr[2];
				tileInCachePtr[3]=((tileLine&0x10)>>4)|((tileLine&0x1000)>>11);
				hflippedTileInCachePtr[4]=tileInCachePtr[3];
				tileInCachePtr[4]=((tileLine&0x08)>>3)|((tileLine&0x0800)>>10);
				hflippedTileInCachePtr[3]=tileInCachePtr[4];
				tileInCachePtr[5]=((tileLine&0x04)>>2)|((tileLine&0x0400)>>9);
				hflippedTileInCachePtr[2]=tileInCachePtr[5];
				tileInCachePtr[6]=((tileLine&0x02)>>1)|((tileLine&0x0200)>>8);
				hflippedTileInCachePtr[1]=tileInCachePtr[6];
				tileInCachePtr[7]=((tileLine&0x01)>>0)|((tileLine&0x0100)>>7);
				hflippedTileInCachePtr[0]=tileInCachePtr[7];

				tileInCachePtr+=8;
				hflippedTileInCachePtr+=8;
			}				
			ws_modified_tile[tileIndex]=0;
		}
		else
		if (wsc_modified_tile[tileIndex])
		{
			// 16 colors by tile layered mode
			if (ws_videoMode==6)
			{
				uint8	*tileInCachePtr			= &wsc_tile_cache[tileIndex<<6];
				uint8	*hflippedTileInCachePtr = &wsc_hflipped_tile_cache[tileIndex<<6];
				uint32	*tileInRamPtr			= (uint32*)&internalRam[0x4000+(tileIndex<<5)];
				uint32	tileLine;

				for (forline=0;forline<8;forline++)
				{
					tileLine=*tileInRamPtr++;
					
					tileInCachePtr[0]=((tileLine&0x00000080)>>7)|((tileLine&0x00008000)>>14)|
									  ((tileLine&0x00800000)>>21)|((tileLine&0x80000000)>>28);
					hflippedTileInCachePtr[7]=tileInCachePtr[0];

					tileInCachePtr[1]=((tileLine&0x00000040)>>6)|((tileLine&0x00004000)>>13)|
									  ((tileLine&0x00400000)>>20)|((tileLine&0x40000000)>>27);
					hflippedTileInCachePtr[6]=tileInCachePtr[1];
					
					tileInCachePtr[2]=((tileLine&0x00000020)>>5)|((tileLine&0x00002000)>>12)|
									  ((tileLine&0x00200000)>>19)|((tileLine&0x20000000)>>26);
					hflippedTileInCachePtr[5]=tileInCachePtr[2];
					
					tileInCachePtr[3]=((tileLine&0x00000010)>>4)|((tileLine&0x00001000)>>11)|
									  ((tileLine&0x00100000)>>18)|((tileLine&0x10000000)>>25);
					hflippedTileInCachePtr[4]=tileInCachePtr[3];
					
					tileInCachePtr[4]=((tileLine&0x00000008)>>3)|((tileLine&0x00000800)>>10)|
									  ((tileLine&0x00080000)>>17)|((tileLine&0x08000000)>>24);
					hflippedTileInCachePtr[3]=tileInCachePtr[4];
					
					tileInCachePtr[5]=((tileLine&0x00000004)>>2)|((tileLine&0x00000400)>>9)|
									  ((tileLine&0x00040000)>>16)|((tileLine&0x04000000)>>23);
					hflippedTileInCachePtr[2]=tileInCachePtr[5];
					
					tileInCachePtr[6]=((tileLine&0x00000002)>>1)|((tileLine&0x00000200)>>8)|
									  ((tileLine&0x00020000)>>15)|((tileLine&0x02000000)>>22);
					hflippedTileInCachePtr[1]=tileInCachePtr[6];
					
					tileInCachePtr[7]=((tileLine&0x00000001)>>0)|((tileLine&0x00000100)>>7)|
									  ((tileLine&0x00010000)>>14)|((tileLine&0x01000000)>>21);
					hflippedTileInCachePtr[0]=tileInCachePtr[7];

					tileInCachePtr+=8;
					hflippedTileInCachePtr+=8;
				}				
			}
			else
			// 16 colors by tile packed mode
			if (ws_videoMode==7)
			{
				uint8	*tileInCachePtr = &wsc_tile_cache[tileIndex<<6];
				uint8	*hflippedTileInCachePtr = &wsc_hflipped_tile_cache[tileIndex<<6];
				uint32	*tileInRamPtr   = (uint32*)&internalRam[0x4000+(tileIndex<<5)];
				uint32	tileLine;

				for (forline=0;forline<8;forline++)
				{
					tileLine=*tileInRamPtr++;
					
					tileInCachePtr[0]=(tileLine>>4)&0x0f;
					hflippedTileInCachePtr[7]=tileInCachePtr[0];
					tileInCachePtr[1]=(tileLine>>0)&0x0f;
					hflippedTileInCachePtr[6]=tileInCachePtr[1];
					tileInCachePtr[2]=(tileLine>>12)&0x0f;
					hflippedTileInCachePtr[5]=tileInCachePtr[2];
					tileInCachePtr[3]=(tileLine>>8)&0x0f;
					hflippedTileInCachePtr[4]=tileInCachePtr[3];
					tileInCachePtr[4]=(tileLine>>20)&0x0f;
					hflippedTileInCachePtr[3]=tileInCachePtr[4];
					tileInCachePtr[5]=(tileLine>>16)&0x0f;
					hflippedTileInCachePtr[2]=tileInCachePtr[5];
					tileInCachePtr[6]=(tileLine>>28)&0x0f;
					hflippedTileInCachePtr[1]=tileInCachePtr[6];
					tileInCachePtr[7]=(tileLine>>24)&0x0f;
					hflippedTileInCachePtr[0]=tileInCachePtr[7];

					tileInCachePtr+=8;
					hflippedTileInCachePtr+=8;
					
				}				
			}
			else
			{
				// unknown mode 
			}
			// tile cache updated
			wsc_modified_tile[tileIndex]=0;
		}	
		if (vFlip)
			line=7-line;
		if (hFlip)
			return(&wsc_hflipped_tile_cache[(tileIndex<<6)+(line<<3)]);
		else
			return(&wsc_tile_cache[(tileIndex<<6)+(line<<3)]);
	
	}
	else
	{
		// need to update tile cache ?
		/*
		if (ws_modified_tile[tileIndex])
		{
			uint8	*tileInCachePtr			 = &ws_tile_cache[tileIndex<<6];
			uint8	*hflippedTileInCachePtr	 = &ws_hflipped_tile_cache[tileIndex<<6];
			uint16	*tileInRamPtr			 = (uint16*)&internalRam[0x2000+(tileIndex<<4)];
			uint16	tileLine;

			for (forline=0;forline<8;forline++)
			{
				tileLine=*tileInRamPtr++;

				tileInCachePtr[0]=((tileLine&0x80)>>7)|((tileLine&0x8000)>>14);
				hflippedTileInCachePtr[7]=tileInCachePtr[0];
				tileInCachePtr[1]=((tileLine&0x40)>>6)|((tileLine&0x4000)>>13);
				hflippedTileInCachePtr[6]=tileInCachePtr[1];
				tileInCachePtr[2]=((tileLine&0x20)>>5)|((tileLine&0x2000)>>12);
				hflippedTileInCachePtr[5]=tileInCachePtr[2];
				tileInCachePtr[3]=((tileLine&0x10)>>4)|((tileLine&0x1000)>>11);
				hflippedTileInCachePtr[4]=tileInCachePtr[3];
				tileInCachePtr[4]=((tileLine&0x08)>>3)|((tileLine&0x0800)>>10);
				hflippedTileInCachePtr[3]=tileInCachePtr[4];
				tileInCachePtr[5]=((tileLine&0x04)>>2)|((tileLine&0x0400)>>9);
				hflippedTileInCachePtr[2]=tileInCachePtr[5];
				tileInCachePtr[6]=((tileLine&0x02)>>1)|((tileLine&0x0200)>>8);
				hflippedTileInCachePtr[1]=tileInCachePtr[6];
				tileInCachePtr[7]=((tileLine&0x01)>>0)|((tileLine&0x0100)>>7);
				hflippedTileInCachePtr[0]=tileInCachePtr[7];

				tileInCachePtr+=8;
				hflippedTileInCachePtr+=8;
			}				
			// tile cache updated
			ws_modified_tile[tileIndex]=0;
		}
		*/
		if (ws_modified_tile[tileIndex])
		{
			uint8	*tileInCachePtr			 = &ws_tile_cache[tileIndex<<6];
			uint8	*hflippedTileInCachePtr	 = &ws_hflipped_tile_cache[(tileIndex<<6)+7];
			uint32	*tileInRamPtr			 = (uint32*)&internalRam[0x2000+(tileIndex<<4)];
			uint32	tileLine;

			for (forline=0;forline<4;forline++)
			{
				tileLine=*tileInRamPtr++;
								
				*hflippedTileInCachePtr--=*tileInCachePtr++=((tileLine&0x80)>>7)|((tileLine&0x8000)>>14);
				*hflippedTileInCachePtr--=*tileInCachePtr++=((tileLine&0x40)>>6)|((tileLine&0x4000)>>13);
				*hflippedTileInCachePtr--=*tileInCachePtr++=((tileLine&0x20)>>5)|((tileLine&0x2000)>>12);
				*hflippedTileInCachePtr--=*tileInCachePtr++=((tileLine&0x10)>>4)|((tileLine&0x1000)>>11);
				*hflippedTileInCachePtr--=*tileInCachePtr++=((tileLine&0x08)>>3)|((tileLine&0x0800)>>10);
				*hflippedTileInCachePtr--=*tileInCachePtr++=((tileLine&0x04)>>2)|((tileLine&0x0400)>>9);
				*hflippedTileInCachePtr--=*tileInCachePtr++=((tileLine&0x02)>>1)|((tileLine&0x0200)>>8);
				*hflippedTileInCachePtr--=*tileInCachePtr++=((tileLine&0x01)>>0)|((tileLine&0x0100)>>7);
				hflippedTileInCachePtr+=16;
				tileLine>>=16;
				*hflippedTileInCachePtr--=*tileInCachePtr++=((tileLine&0x80)>>7)|((tileLine&0x8000)>>14);
				*hflippedTileInCachePtr--=*tileInCachePtr++=((tileLine&0x40)>>6)|((tileLine&0x4000)>>13);
				*hflippedTileInCachePtr--=*tileInCachePtr++=((tileLine&0x20)>>5)|((tileLine&0x2000)>>12);
				*hflippedTileInCachePtr--=*tileInCachePtr++=((tileLine&0x10)>>4)|((tileLine&0x1000)>>11);
				*hflippedTileInCachePtr--=*tileInCachePtr++=((tileLine&0x08)>>3)|((tileLine&0x0800)>>10);
				*hflippedTileInCachePtr--=*tileInCachePtr++=((tileLine&0x04)>>2)|((tileLine&0x0400)>>9);
				*hflippedTileInCachePtr--=*tileInCachePtr++=((tileLine&0x02)>>1)|((tileLine&0x0200)>>8);
				*hflippedTileInCachePtr--=*tileInCachePtr++=((tileLine&0x01)>>0)|((tileLine&0x0100)>>7);
				hflippedTileInCachePtr+=16;
			}				
			// tile cache updated
			ws_modified_tile[tileIndex]=0;
		}
		
		if (vFlip)
			line=7-line;
		if (hFlip)
			return(&ws_hflipped_tile_cache[(tileIndex<<6)+(line<<3)]);
		else
			return(&ws_tile_cache[(tileIndex<<6)+(line<<3)]);
	}
	return(NULL);
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
void ws_drawClippedSpriteLine(int16 *framebuffer, uint16 scanline,
							  uint32 x, uint32 y, uint32 tileIndex, uint32 paletteIndex,
							  uint32 vFlip, uint32 hFlip,
							  uint32 clip_x0, uint32 clip_y0, uint32 clip_x1, uint32 clip_y1)
{
	uint8	*ws_tileRow;
	uint16	nbPixels;

	if ((scanline<y)||(scanline>(y+7)))
		return;
	if ((x+7<clip_x0)||(x>=clip_x1))
		return;
	if ((y+7<clip_y0)||(y>=clip_y1))
		return;
//	if(scanline>clip_y0 && scanline<clip_y1)
//		return;

	
	ws_tileRow=ws_tileCache_getTileRow(tileIndex,(scanline-y)&0x07,hFlip,vFlip, 0);
	nbPixels=8;
	if (x<clip_x0)
	{
		ws_tileRow+=clip_x0-x;
		nbPixels-=clip_x0-x;
		x=clip_x0;
	}
	if (x+nbPixels>clip_x1)
		nbPixels=(clip_x1-x);
	framebuffer+=x;

	if (ws_gpu_operatingInColor)
	{
		int16	*wsc_paletteAlias=&wsc_palette[paletteIndex<<4];
		while (nbPixels)
		{
			if (*ws_tileRow) *framebuffer=wsc_paletteAlias[*ws_tileRow];
			framebuffer++;	ws_tileRow++;
			nbPixels--;
		}
	}
	else
	{
		int16	*ws_paletteAlias=&ws_palette[paletteIndex<<2];
		if (paletteIndex&0x04)
		{
			while (nbPixels)
			{
				if (*ws_tileRow) *framebuffer=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; 
				framebuffer++;	ws_tileRow++;
				nbPixels--;
			}
		}
		else
		{
			while (nbPixels)
			{
				*framebuffer=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; 
				framebuffer++;	ws_tileRow++;
				nbPixels--;
			}
		}
	}
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
void ws_gpu_renderScanline(int16 *framebuffer)
{
	int16 backgroundColor;
	int i;
	uint16	*ws_bgScrollRamBase;
	int	ws_currentTile;
	int	lineInTile;   
	int columnInTile; 

	int16 *scanlinePtr;

	if (ws_gpu_scanline>143)	
		return;

	framebuffer+=(224*ws_gpu_scanline);

	// fill with background color
	if (ws_gpu_operatingInColor)
		backgroundColor=wsc_palette[ws_ioRam[0x01]];
	else
		backgroundColor=ws_shades[ws_paletteColors[ws_palette[((ws_ioRam[0x01]&0xf0)>>2)+(ws_ioRam[0x01]&0x03)]]];

	for (i=0;i<224;i++)
		framebuffer[i]=backgroundColor;

	// render background layer
	if (ws_ioRam[0x00]&0x01)
	{
		int ws_bgScroll_x=ws_ioRam[0x10];
		int ws_bgScroll_y=ws_ioRam[0x11];
		
		// seek to the first tile
		ws_bgScroll_y=(ws_bgScroll_y+ws_gpu_scanline)&0xff;

		// note: byte ordering assumptions!
		ws_currentTile=(ws_bgScroll_x>>3);
		ws_bgScrollRamBase=(uint16*)(internalRam+(((uint32)ws_ioRam[0x07]&0x0f)<<11)+
									((ws_bgScroll_y&0xfff8)<<3));
		
		lineInTile   = ws_bgScroll_y&0x07;
		columnInTile = ws_bgScroll_x&0x07;
	
		scanlinePtr=framebuffer;

		if (ws_gpu_operatingInColor)
		{
			int nbTiles;
			
			// render the first clipped tile
			if (columnInTile)
			{
				uint16	tileInfo=ws_bgScrollRamBase[ws_currentTile&0x1f];
				uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile, tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
				int16	*wsc_paletteAlias=&wsc_palette[((tileInfo>>9)&0x0f)<<4];
				ws_currentTile++;
				ws_tileRow+=columnInTile;
				for (i=columnInTile;i<8;i++)
				{
					if (*ws_tileRow)	
						*scanlinePtr=wsc_paletteAlias[*ws_tileRow]; 
					scanlinePtr++; 
					ws_tileRow++;
				}
			}

			// render the tiles between them
			nbTiles=28;
			if (columnInTile)
				nbTiles=27;
			
			for (i=0;i<nbTiles;i++)
			{
				uint16	tileInfo=ws_bgScrollRamBase[ws_currentTile&0x1f];
				uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile, tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
				int16	*wsc_paletteAlias=&wsc_palette[((tileInfo>>9)&0x0f)<<4];
				ws_currentTile++;

				if (*ws_tileRow) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++;
				if (*ws_tileRow) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++;
				if (*ws_tileRow) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++;
				if (*ws_tileRow) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++;
				if (*ws_tileRow) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++;
				if (*ws_tileRow) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++;
				if (*ws_tileRow) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++;
				if (*ws_tileRow) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++;
			}

			// render the last clipped tile
			if (columnInTile)
			{
				uint16	tileInfo=ws_bgScrollRamBase[ws_currentTile&0x1f];
				uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
													tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
				int16	*wsc_paletteAlias=&wsc_palette[((tileInfo>>9)&0x0f)<<4];
				ws_currentTile++;

				for (i=0;i<columnInTile;i++)
				{
					if (*ws_tileRow)	
						*scanlinePtr=wsc_paletteAlias[*ws_tileRow]; 
					scanlinePtr++; 
					ws_tileRow++;
				}
			}
		}
		else
		{
			int nbTiles;
			// render the first clipped tile
			if (columnInTile)
			{
				uint16	tileInfo=ws_bgScrollRamBase[ws_currentTile&0x1f];
				uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
													tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
				
				int16	*ws_paletteAlias=&ws_palette[((tileInfo>>9)&0x0f)<<2];
				ws_currentTile++;
				ws_tileRow+=columnInTile;

				if ((tileInfo>>9)&0x04)
				{
					for (i=columnInTile;i<8;i++)
					{
						if (*ws_tileRow)	
							*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; 
						scanlinePtr++; 
						ws_tileRow++;
					}
				}
				else
				{
					for (i=columnInTile;i<8;i++)
					{
						*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; 
						scanlinePtr++; 
						ws_tileRow++;
					}
				}
			}
			nbTiles=28;
			if (columnInTile)
				nbTiles=27;
			
			for (i=0;i<nbTiles;i++)
			{
				uint16	tileInfo=ws_bgScrollRamBase[ws_currentTile&0x1f];
				uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
													tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
				int16	*ws_paletteAlias=&ws_palette[((tileInfo>>9)&0x0f)<<2];
				ws_currentTile++;
				
				if ((tileInfo>>9)&0x04)
				{
					if (*ws_tileRow)	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
					if (*ws_tileRow)	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
					if (*ws_tileRow)	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
					if (*ws_tileRow)	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
					if (*ws_tileRow)	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
					if (*ws_tileRow)	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
					if (*ws_tileRow)	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
					if (*ws_tileRow)	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
				}
				else
				{
					*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
					*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
					*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
					*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
					*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
					*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
					*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
					*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
				}

			}

			// render the last clipped tile
			if (columnInTile)
			{
				uint16	tileInfo=ws_bgScrollRamBase[ws_currentTile&0x1f];
				uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
													tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
				int16	*ws_paletteAlias=&ws_palette[((tileInfo>>9)&0x0f)<<2];
				ws_currentTile++;

				if ((tileInfo>>9)&0x04)
				{
					for (i=0;i<columnInTile;i++)
					{
						if (*ws_tileRow)	
							*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; 
						scanlinePtr++; 
						ws_tileRow++;
					}
				}
				else
				{
					for (i=0;i<columnInTile;i++)
					{
						*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; 
						scanlinePtr++; 
						ws_tileRow++;
					}
				}
			}
		}
	}
	// render sprites which are between both layers
	if (ws_ioRam[0x00]&0x04)
	{
		int ws_sprWindow_x0=ws_ioRam[0x0c];
		int ws_sprWindow_y0=ws_ioRam[0x0d];
		int ws_sprWindow_x1=ws_ioRam[0x0e];
		int ws_sprWindow_y1=ws_ioRam[0x0f];
		uint32	*ws_sprRamBase=(uint32*)(internalRam+(((uint32)ws_ioRam[0x04])<<9));
//		uint32	*ws_sprRamBase=(uint32*)ws_gpu_spritetable_buffer;
		
		// seek to first sprite
		ws_sprRamBase+=ws_ioRam[0x06]-1;
//		ws_sprRamBase+=ws_gpu_spritebuffer_start-1;

		for (i=ws_ioRam[0x06];i>ws_ioRam[0x05];i--)
//		for (i=ws_gpu_spritebuffer_start;i>ws_gpu_spritebuffer_end;i--)
		{
			uint32 spr=*ws_sprRamBase--;

			if (!(spr&0x2000))
			{
				// sprite window on ?
				if ((ws_ioRam[0x00]&0x08)&&(spr&0x1000)&&(ws_sprWindow_x0!=ws_sprWindow_x1))
				{
					ws_drawClippedSpriteLine(framebuffer,ws_gpu_scanline,(spr&0xff000000)>>24, (spr&0x00ff0000)>>16,
										 spr&0x1ff,8+((spr&0xe00)>>9),spr&0x4000,spr&0x8000,
										 ws_sprWindow_x0,ws_sprWindow_y0,ws_sprWindow_x1,ws_sprWindow_y1);
				}
				else
				{
					ws_drawClippedSpriteLine(framebuffer,ws_gpu_scanline,(spr&0xff000000)>>24, (spr&0x00ff0000)>>16,
										 spr&0x1ff,8+((spr&0xe00)>>9),spr&0x4000,spr&0x8000,
										 0,0,224,144);
				}
			}
		}
	}

	// render foreground layer
	if (ws_ioRam[0x00]&0x02)
	{
		int ws_currentTile;
		uint16 *ws_fgScrollRamBase;
		int	lineInTile;
		int columnInTile;
		int16 *scanlinePtr;

		int ws_fgWindow_x0=ws_ioRam[0x08];
		int ws_fgWindow_y0=ws_ioRam[0x09];
		int ws_fgWindow_x1=ws_ioRam[0x0a];
		int ws_fgWindow_y1=ws_ioRam[0x0b];
		int ws_fgScroll_x=ws_ioRam[0x12];
		int ws_fgScroll_y=ws_ioRam[0x13];

		int windowMode=ws_ioRam[0x00]&0x30;
				
		// seek to the first tile
		ws_fgScroll_y=(ws_fgScroll_y+ws_gpu_scanline)&0xff;

		// note: byte ordering assumptions!
		ws_currentTile=(ws_fgScroll_x>>3);
		ws_fgScrollRamBase=(uint16*)(internalRam+(((uint32)ws_ioRam[0x07]&0xf0)<<7)+
									((ws_fgScroll_y&0xfff8)<<3));
		
		lineInTile   = ws_fgScroll_y&0x07;
		columnInTile = ws_fgScroll_x&0x07;
	
		scanlinePtr=framebuffer;
		
		// window disabled
		if ((!windowMode) || 
		   ((windowMode == 0x03) && 
		   ((ws_gpu_scanline < ws_fgWindow_y0) || (ws_gpu_scanline > ws_fgWindow_y1))))
		{
			if (ws_gpu_operatingInColor)
			{
				int nbTiles;
				
				// render the first clipped tile
				if (columnInTile)
				{
					uint16	tileInfo=ws_fgScrollRamBase[ws_currentTile&0x1f];
					uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
														tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
					int16	*wsc_paletteAlias=&wsc_palette[((tileInfo>>9)&0x0f)<<4];
					ws_currentTile++;
					
					ws_tileRow+=columnInTile;

					for (i=columnInTile;i<8;i++)
					{
						if (*ws_tileRow)	
							*scanlinePtr=wsc_paletteAlias[*ws_tileRow]; 
						scanlinePtr++; 
						ws_tileRow++;
					}
				}

				// render the tiles between them
				nbTiles=28;
				if (columnInTile)
					nbTiles=27;
				
				for ( i=0;i<nbTiles;i++)
				{
					uint16	tileInfo=ws_fgScrollRamBase[ws_currentTile&0x1f];
					uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
														tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
					int16	*wsc_paletteAlias=&wsc_palette[((tileInfo>>9)&0x0f)<<4];
					ws_currentTile++;

					if (*ws_tileRow) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++;
					if (*ws_tileRow) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++;
					if (*ws_tileRow) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++;
					if (*ws_tileRow) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++;
					if (*ws_tileRow) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++;
					if (*ws_tileRow) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++;
					if (*ws_tileRow) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++;
					if (*ws_tileRow) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++;
				}

				// render the last clipped tile
				if (columnInTile)
				{
					uint16	tileInfo=ws_fgScrollRamBase[ws_currentTile&0x1f];
					uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
														tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
					int16	*wsc_paletteAlias=&wsc_palette[((tileInfo>>9)&0x0f)<<4];
					ws_currentTile++;

					for (i=0;i<columnInTile;i++)
					{
						if (*ws_tileRow)	
							*scanlinePtr=wsc_paletteAlias[*ws_tileRow]; 
						scanlinePtr++; 
						ws_tileRow++;
					}
				}
			}
			else
			{
				int nbTiles;
				// render the first clipped tile
				if (columnInTile)
				{
					uint16	tileInfo=ws_fgScrollRamBase[ws_currentTile&0x1f];
					uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
														tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
					
					int16	*ws_paletteAlias=&ws_palette[((tileInfo>>9)&0x0f)<<2];
					ws_currentTile++;
					ws_tileRow+=columnInTile;
					if ((tileInfo>>9)&0x04)
					{
						for (i=columnInTile;i<8;i++)
						{
							if (*ws_tileRow)	
								*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; 
							scanlinePtr++; 
							ws_tileRow++;
						}
					}
					else
					{
						for (i=columnInTile;i<8;i++)
						{
							*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; 
							scanlinePtr++; 
							ws_tileRow++;
						}
					}
				}
				nbTiles=28;
				if (columnInTile)
					nbTiles=27;
				
				for (i=0;i<nbTiles;i++)
				{
					uint16	tileInfo=ws_fgScrollRamBase[ws_currentTile&0x1f];
					uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
														tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
					int16	*ws_paletteAlias=&ws_palette[((tileInfo>>9)&0x0f)<<2];
					ws_currentTile++;
					
					if ((tileInfo>>9)&0x04)
					{
						if (*ws_tileRow)	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
						if (*ws_tileRow)	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
						if (*ws_tileRow)	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
						if (*ws_tileRow)	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
						if (*ws_tileRow)	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
						if (*ws_tileRow)	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
						if (*ws_tileRow)	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
						if (*ws_tileRow)	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
					}
					else
					{
						*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
						*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
						*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
						*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
						*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
						*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
						*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
						*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++;
					}
				}

				// render the last clipped tile
				if (columnInTile)
				{
					uint16	tileInfo=ws_fgScrollRamBase[ws_currentTile&0x1f];
					uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
														tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
					int16	*ws_paletteAlias=&ws_palette[((tileInfo>>9)&0x0f)<<2];
					ws_currentTile++;
					
					if ((tileInfo>>9)&0x04)
					{
						for (i=0;i<columnInTile;i++)
						{
							if (*ws_tileRow)	
								*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; 
							scanlinePtr++; 
							ws_tileRow++;
						}
					}
					else
					{
						for (i=0;i<columnInTile;i++)
						{
							*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; 
							scanlinePtr++; 
							ws_tileRow++;
						}
					}
				}
			}
		}
		else
		// foreground layer displayed only inside the window
		if (windowMode==0x20 && (ws_gpu_scanline >= ws_fgWindow_y0) && (ws_gpu_scanline <= ws_fgWindow_y1))
		{
			int column=0;
			if (ws_gpu_operatingInColor)
			{
				int nbTiles;
				// render the first clipped tile
				if (columnInTile)
				{
					uint16	tileInfo=ws_fgScrollRamBase[ws_currentTile&0x1f];
					uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
														tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
					int16	*wsc_paletteAlias=&wsc_palette[((tileInfo>>9)&0x0f)<<4];
					ws_currentTile++;
					
					ws_tileRow+=columnInTile;

					for (i=columnInTile;i<8;i++)
					{
						if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	
							*scanlinePtr=wsc_paletteAlias[*ws_tileRow]; 
						column++;
						scanlinePtr++; 
						ws_tileRow++;
					}
				}

				// render the tiles between them
				nbTiles=28;
				if (columnInTile)
					nbTiles=27;
				
				for (i=0;i<nbTiles;i++)
				{
					uint16	tileInfo=ws_fgScrollRamBase[ws_currentTile&0x1f];
					uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
														tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
					int16	*wsc_paletteAlias=&wsc_palette[((tileInfo>>9)&0x0f)<<4];
					ws_currentTile++;
					
					if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1)) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++; column++;
					if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1)) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++; column++;
					if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1)) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++; column++;
					if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1)) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++; column++;
					if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1)) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++; column++;
					if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1)) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++; column++;
					if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1)) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++; column++;
					if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1)) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++; column++;
				}

				// render the last clipped tile
				if (columnInTile)
				{
					uint16	tileInfo=ws_fgScrollRamBase[ws_currentTile&0x1f];
					uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
														tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
					int16	*wsc_paletteAlias=&wsc_palette[((tileInfo>>9)&0x0f)<<4];
					ws_currentTile++;
					
					for (i=0;i<columnInTile;i++)
					{
						if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	
							*scanlinePtr=wsc_paletteAlias[*ws_tileRow]; 
						column++;
						scanlinePtr++; 
						ws_tileRow++;
					}
				}
			}
			else
			{
				int nbTiles;
				// render the first clipped tile
				if (columnInTile)
				{
					uint16	tileInfo=ws_fgScrollRamBase[ws_currentTile&0x1f];
					uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
														tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
					
					int16	*ws_paletteAlias=&ws_palette[((tileInfo>>9)&0x0f)<<2];
					ws_currentTile++;
					ws_tileRow+=columnInTile;

					if ((tileInfo>>9)&0x04)
					{
						for (i=columnInTile;i<8;i++)
						{
							if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	
								*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; 
							column++;
							scanlinePtr++; 
							ws_tileRow++;
						}
					}
					else
					{
						for (i=columnInTile;i<8;i++)
						{
							if ((column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))
								*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]];
							column++;
							scanlinePtr++; 
							ws_tileRow++;
						}
					}
				}

				nbTiles=28;
				if (columnInTile)
					nbTiles=27;
				
				for (i=0;i<nbTiles;i++)
				{
					uint16	tileInfo=ws_fgScrollRamBase[ws_currentTile&0x1f];
					uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
														tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
					int16	*ws_paletteAlias=&ws_palette[((tileInfo>>9)&0x0f)<<2];
					ws_currentTile++;

					if ((tileInfo>>9)&0x04)
					{
						if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
					}
					else
					{
						if ((column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
					}
				}

				// render the last clipped tile
				if (columnInTile)
				{
					uint16	tileInfo=ws_fgScrollRamBase[ws_currentTile&0x1f];
					uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
														tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
					int16	*ws_paletteAlias=&ws_palette[((tileInfo>>9)&0x0f)<<2];
					ws_currentTile++;

					if ((tileInfo>>9)&0x04)
					{
						for (i=0;i<columnInTile;i++)
						{
							if ((*ws_tileRow)&&(column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))
								*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]];
							column++;
							scanlinePtr++; 
							ws_tileRow++;
						}
					}
					else
					{
						for (i=0;i<columnInTile;i++)
						{
							if ((column>=ws_fgWindow_x0)&&(column<=ws_fgWindow_x1))
								*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]];
							column++;
							scanlinePtr++; 
							ws_tileRow++;
						}
					}
				}
			}
		}
		else
		// foreground layer displayed only outside the window
		if (windowMode==0x30)
		{
			int column=0;
			if (ws_gpu_operatingInColor)
			{
				int nbTiles;
				// render the first clipped tile
				if (columnInTile)
				{
					uint16	tileInfo=ws_fgScrollRamBase[ws_currentTile&0x1f];
					uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
														tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
					int16	*wsc_paletteAlias=&wsc_palette[((tileInfo>>9)&0x0f)<<4];
					ws_currentTile++;
					
					ws_tileRow+=columnInTile;

					for (i=columnInTile;i<8;i++)
					{
						if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1)))	
							*scanlinePtr=wsc_paletteAlias[*ws_tileRow]; 
						column++;
						scanlinePtr++; 
						ws_tileRow++;
					}
				}

				// render the tiles between them
				nbTiles=28;
				if (columnInTile)
					nbTiles=27;
				
				for (i=0;i<nbTiles;i++)
				{
					uint16	tileInfo=ws_fgScrollRamBase[ws_currentTile&0x1f];
					uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
														tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
					int16	*wsc_paletteAlias=&wsc_palette[((tileInfo>>9)&0x0f)<<4];
					ws_currentTile++;
					
					if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1))) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++; column++;
					if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1))) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++; column++;
					if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1))) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++; column++;
					if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1))) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++; column++;
					if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1))) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++; column++;
					if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1))) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++; column++;
					if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1))) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++; column++;
					if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1))) *scanlinePtr=wsc_paletteAlias[*ws_tileRow]; scanlinePtr++;	ws_tileRow++; column++;
				}

				// render the last clipped tile
				if (columnInTile)
				{
					uint16	tileInfo=ws_fgScrollRamBase[ws_currentTile&0x1f];
					uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
														tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
					int16	*wsc_paletteAlias=&wsc_palette[((tileInfo>>9)&0x0f)<<4];
					ws_currentTile++;
					
					for (i=0;i<columnInTile;i++)
					{
						if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1)))	
							*scanlinePtr=wsc_paletteAlias[*ws_tileRow]; 
						column++;
						scanlinePtr++; 
						ws_tileRow++;
					}
				}
			}
			else
			{
				int nbTiles;
				// render the first clipped tile
				if (columnInTile)
				{
					uint16	tileInfo=ws_fgScrollRamBase[ws_currentTile&0x1f];
					uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
														tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
					
					int16	*ws_paletteAlias=&ws_palette[((tileInfo>>9)&0x0f)<<2];
					ws_currentTile++;
					ws_tileRow+=columnInTile;

					if ((tileInfo>>9)&0x04)
					{
						for (i=columnInTile;i<8;i++)
						{
							if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1)))	
								*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; 
							column++;
							scanlinePtr++; 
							ws_tileRow++;
						}
					}
					else
					{
						for (i=columnInTile;i<8;i++)
						{
						if ((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1))
								*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]];
							column++;
							scanlinePtr++; 
							ws_tileRow++;
						}
					}
				}
				
				nbTiles = 28;
				if (columnInTile)
					nbTiles=27;
				
				for (i=0;i<nbTiles;i++)
				{
					uint16	tileInfo=ws_fgScrollRamBase[ws_currentTile&0x1f];
					uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile, tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
					int16	*ws_paletteAlias=&ws_palette[((tileInfo>>9)&0x0f)<<2];
					ws_currentTile++;

					if ((tileInfo>>9)&0x04)
					{
						if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1)))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1)))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1)))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1)))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1)))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1)))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1)))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1)))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
					}
					else
					{
						if ((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
						if ((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1))	*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]]; scanlinePtr++; ws_tileRow++; column++;
					}
				}

				// render the last clipped tile
				if (columnInTile)
				{
					uint16	tileInfo=ws_fgScrollRamBase[ws_currentTile&0x1f];
					uint8	*ws_tileRow=ws_tileCache_getTileRow(	tileInfo&0x1ff, lineInTile,
														tileInfo&0x8000, tileInfo&0x4000, tileInfo&0x2000);
					int16	*ws_paletteAlias=&ws_palette[((tileInfo>>9)&0x0f)<<2];
					ws_currentTile++;
					
					if ((tileInfo>>9)&0x04)
					{
						for (i=0;i<columnInTile;i++)
						{
							if ((*ws_tileRow)&&((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1)))	
								*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]];
							column++;
							scanlinePtr++; 
							ws_tileRow++;
						}
					}
					else
					{
						for (i=0;i<columnInTile;i++)
						{
							if ((column<ws_fgWindow_x0)||(column>ws_fgWindow_x1))
								*scanlinePtr=ws_shades[ws_paletteColors[ws_paletteAlias[*ws_tileRow]]];
							column++;
							scanlinePtr++; 
							ws_tileRow++;
						}
					}
				}
			}
		}
		else
		{
			// unknown
		}
	}

	// render sprites
	if (ws_ioRam[0x00]&0x04)
	{
		int ws_sprWindow_x0=ws_ioRam[0x0c];
		int ws_sprWindow_y0=ws_ioRam[0x0d];
		int ws_sprWindow_x1=ws_ioRam[0x0e];
		int ws_sprWindow_y1=ws_ioRam[0x0f];
		uint32	*ws_sprRamBase=(uint32*)(internalRam+(((uint32)ws_ioRam[0x04])<<9));
//		uint32	*ws_sprRamBase=(uint32*)ws_gpu_spritetable_buffer;

		// seek to first sprite
		ws_sprRamBase+=ws_ioRam[0x06]-1;
//		ws_sprRamBase+=ws_gpu_spritebuffer_start-1;

		for (i=ws_ioRam[0x06];i>ws_ioRam[0x05];i--)
//		for (i=ws_gpu_spritebuffer_start;i>ws_gpu_spritebuffer_end;i--)
		{
			uint32 spr=*ws_sprRamBase--;
			
			if (spr&0x2000)
			{
				// sprite window on ?
				if ((ws_ioRam[0x00]&0x08)&&(spr&0x1000)&&(ws_sprWindow_x0!=ws_sprWindow_x1))
				{
					ws_drawClippedSpriteLine(framebuffer,ws_gpu_scanline,(spr&0xff000000)>>24, (spr&0x00ff0000)>>16,
										 spr&0x1ff,8+((spr&0xe00)>>9),spr&0x4000,spr&0x8000,
										 ws_sprWindow_x0,ws_sprWindow_y0,ws_sprWindow_x1,ws_sprWindow_y1);
				}
				else
				{
					ws_drawClippedSpriteLine(framebuffer,ws_gpu_scanline,(spr&0xff000000)>>24, (spr&0x00ff0000)>>16,
										 spr&0x1ff,8+((spr&0xe00)>>9),spr&0x4000,spr&0x8000,
										 0,0,224,144);
				}
			}
		}
	}

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
void ws_gpu_write_byte(DWORD offset, BYTE value)
{
	// ws 4 color tiles
	if ((offset>=0x2000)&&(offset<0x4000))
	{
		if (internalRam[offset]!=value)
			ws_modified_tile[(offset&0x1fff)>>4]=1;
		// update the ram
		internalRam[offset]=value;
	}
	if (ws_gpu_operatingInColor)
	{
		// wsc 16 color tiles bank 0
		if ((offset>=0x4000)&&(offset<0x8000))
		{
			if (internalRam[offset]!=value)
				wsc_modified_tile[(offset&0x3fff)>>5]=1;
		}
		else
		// wsc 16 color tiles bank 1
		if ((offset>=0x8000)&&(offset<0xC000))
		{
			if (internalRam[offset]!=value)
				wsc_modified_tile[512+((offset&0x3fff)>>5)]=1;
		}

		// update the ram
		internalRam[offset]=value;
		// palette ram
		if (offset>=0xfe00)
		{
			// RGB444 format
			uint16	color=(internalRam[(offset&0xfffe)+1]);
			color<<=8;
			color|=(internalRam[(offset&0xfffe)]);

			wsc_palette[(offset&0x1ff)>>1]=RGB555(((color>>8)&0x0f)<<1,((color>>4)&0x0f)<<1,(color&0x0f)<<1);				
		}
	}
	else
	if (offset<0x4000)
		internalRam[offset]=value;	
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
void ws_gpu_port_write(DWORD port,BYTE value)
{
	int paletteIndex;
	
	switch (port)
	{
	case 0x60:	ws_gpu_changeVideoMode(value);
				return;
	case 0x1C:	ws_paletteColors[0]=value&0xf;
				ws_paletteColors[1]=(value>>4)&0xf;
				return;
	case 0x1D:	ws_paletteColors[2]=value&0xf;
				ws_paletteColors[3]=(value>>4)&0xf;
				return;
	case 0x1E:	ws_paletteColors[4]=value&0xf;
				ws_paletteColors[5]=(value>>4)&0xf;
				return;
	case 0x1F:	ws_paletteColors[6]=value&0xf;
				ws_paletteColors[7]=(value>>4)&0xf;
				return;
	}
	if ((port>=0x20)&&(port<=0x3f))
	{
		port-=0x20;
		paletteIndex=port>>1;
		if (port&0x01)
		{
			ws_palette[(paletteIndex<<2)+2]=value&0x7; 
			ws_palette[(paletteIndex<<2)+3]=(value>>4)&0x7;
		}
		else
		{
			ws_palette[(paletteIndex<<2)+0]=value&0x7; 
			ws_palette[(paletteIndex<<2)+1]=(value>>4)&0x7;
		}
	}
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
BYTE ws_gpu_port_read(BYTE port)
{
	switch(port)
	{
	case 0xa0:
		if (ws_gpu_forceColorSystemBool)
			return ws_ioRam[0xa0]|2;
		else
		if (ws_gpu_forceMonoSystemBool)
			return ws_ioRam[0xa0]&(~0x02);
		else
		{
			if (ws_gpu_operatingInColor) 
				return ws_ioRam[0xa0]|2; 
			else 
				return ws_ioRam[0xa0]&(~0x02);
		}
			break;
	case 0xaa:
			return vblank_count&0xff;
	case 0xab:
			return (vblank_count>>8)&0xff;
	case 0xac:
			return (vblank_count>>16)&0xff;
	case 0xad:
			return (vblank_count>>24)&0xff;
	}
	return(ws_ioRam[port]);
}
