//---------------------------------------------------------------------------
#include "types.h"

#include <gpstdlib.h>
#include <gpstdio.h>
#include <gpgraphic.h>
#include <gpmain.h>
#include <gpfont.h>

#include "OKF/global.h"
#include "OKF/clock.h"
#include "OKF/graphics16.h"
#include "OKF/okf.h"

#include "file_dialog.h"

#include "WSDraw.h"
#include "WS.h"

#include "render.h"

//#define DRAW_DEBUG

#define RGB555(R,G,B) ((((int)(R))<<11)|(((int)(G))<<6)|(((int)(B))<<1))

int DrawMode=-1;
int BGLayer;
int FGLayer;
int SpLayer;


byte ColTbl[0x210];
uint32 Palette[256];

uint8 screenbuffer[240*240];

uint8	*ws_tile_cache;
uint8	*wsc_tile_cache;

uint8	*ws_hflipped_tile_cache;
uint8	*wsc_hflipped_tile_cache;
uint8	*ws_modified_tile;
uint8	*wsc_modified_tile;

#define MAP_TILE 0x01FF
#define MAP_PAL  0x1E00
#define MAP_BANK 0x2000
#define MAP_HREV 0x4000
#define MAP_VREV 0x8000
byte *Scr1TMap;							// 스크린 1의 타이르맛후˚호˚인터
byte *Scr2TMap;							// 스크린 2의 타이르맛후˚호˚인터

#define SPR_TILE 0x01FF
#define SPR_PAL  0x0E00
#define SPR_CLIP 0x1000
#define SPR_LAYR 0x2000
#define SPR_HREV 0x4000
#define SPR_VREV 0x8000
byte *SprTTMap;							// 스후˚라이트의 선두 타이르맛후˚
byte *SprETMap;							// 스후˚라이트의 말미 타이르맛후˚
byte SprTMap[512];						// 스후˚라이트의 타이르맛후˚

byte *BGndTPal=NULL;					// 하″크크″라운트″의 타일하˚렛호˚인터
byte *WndTPal=NULL;						// 윈트″우의 타일하˚렛호˚인터

extern int vert;

typedef struct video_mode_tag
{
	void (*video_update)(unsigned char*, unsigned char*);
	int video_x;
	int video_y;
	int scroll_x;
} video_mode;

void (*video_update)(unsigned char*, unsigned char*);
int video_x = 0;
int video_y = 0;
int scroll_x = 0;

video_mode video_modes[5] =
{
	horz_render_square,			48,	12,	0,
	horz_render_widescreen,		0,	48,	4,
	horz_render_fullscreen,		0,	12,	4,
	NULL,						48,	48,	0,
	NULL,						88,	0,	0
};

float gammaC = 1.0f;

uint8 *WsGetTileRow(uint32 TMap, uint32 line, uint32 sprite);

#define VERT_BUFFER_MODE 1
#define HORZ_BUFFER_MODE 240

#undef  BUFFER_MODE
#define BUFFER_MODE HORZ_BUFFER_MODE
#include "newgfxcore.h"

#undef  BUFFER_MODE
#define BUFFER_MODE VERT_BUFFER_MODE
#include "newgfxcore.h"

void (*renderLine[]) (int Line, void *buffer) = {
	RefreshLineColorHorz,
	RefreshLineMonoHorz,
	RefreshLineColorVert,
	RefreshLineMonoVert,
};

void  (*RefreshLine)(int Line, void* lpSurface);

void  SetHWPalette();
void  RebuildPalette();

//---------------------------------------------------------------------------
int  WsDrawCreate()
{
    int result;

	video_update = video_modes[DrawMode].video_update;
	video_x = video_modes[DrawMode].video_x;
	video_y = video_modes[DrawMode].video_y;
	scroll_x = video_modes[DrawMode].scroll_x;

	if (ws_tile_cache == NULL)
	{
		ws_tile_cache				= (uint8*)GPMALLOC(512*8*8);
		wsc_tile_cache				= (uint8*)GPMALLOC(1024*8*8);

		ws_hflipped_tile_cache		= (uint8*)GPMALLOC(512*8*8);
		wsc_hflipped_tile_cache		= (uint8*)GPMALLOC(1024*8*8);

		ws_modified_tile			= (uint8*)GPMALLOC(512);
		wsc_modified_tile			= (uint8*)GPMALLOC(1024);
	}

	GPMEMSET(ws_tile_cache,0x00,512*8*8);
	GPMEMSET(wsc_tile_cache,0x00,1024*8*8);

	GPMEMSET(ws_hflipped_tile_cache,0x00,512*8*8);
	GPMEMSET(wsc_hflipped_tile_cache,0x00,1024*8*8);

	GPMEMSET(ws_modified_tile,0x01,512);
	GPMEMSET(wsc_modified_tile,0x01,1024);

    WsDrawClear();
	WriteIO(0x60, IO[0x60]);
	RebuildPalette();
    return 0;
}

//---------------------------------------------------------------------------
int  WsDrawLine(int Line)
{
	if (video_update != NULL)
	{
		if (vert == 0)
			RefreshLine(Line, screenbuffer+(144-Line)); // works for horz
		else
			RefreshLine(Line, screenbuffer+(144-Line)); // works for vert
	}
	else
	{
		if (vert == 0)	// horz
			RefreshLine(Line, gtSurface[giSurface].ptbuffer+(240*video_x)-(video_y)+(240-(Line)));
		else		// vert
			RefreshLine(Line, gtSurface[giSurface].ptbuffer+(240*video_x)-(video_y)+(Line*240));
	}

	return 0;
}

//---------------------------------------------------------------------------
int  WsDrawFlip(void)
{
	int i,j;

	if (video_update != NULL)
	{
		video_update((uint8*)screenbuffer+(8*240)+(scroll_x*240), gtSurface[giSurface].ptbuffer+(240*video_x)+(video_y));
	}
	else
	{
		if (vert == 0) // horz mode (no screen stretch)
		{
			for (i = 0; i < 8; i++)
			{
				for (j = 0; j < 144; j++)
				{
					*(gtSurface[giSurface].ptbuffer+(240*video_x)-(video_y)+(i*240)+(240-(j))) = 0xFF;
					*(gtSurface[giSurface].ptbuffer+(240*video_x)+(240*(224+8))-(video_y)+(i*240)+(240-(j))) = 0xFF;
				}
			}
		}
		else // vert mode (no stretch)
		{
			for (i = 0; i < 144; i++)
			{
				for (j = 0; j < 8; j++)
				{
					*(gtSurface[giSurface].ptbuffer+(240*video_x)-(video_y)+(i*240)+j) = 0xFF;
					*(gtSurface[giSurface].ptbuffer+(240*video_x)-(video_y)+(i*240)+j+224) = 0xFF;
				}
			}
		}
	}

	SurfaceFlip();
	SetHWPalette();
	return 0;
}

//---------------------------------------------------------------------------
void  WsDrawClear(void)
{
	int i;
	unsigned long *HWPALETTE=(unsigned long *)0x14A00400;
	
	HWPALETTE[0xFF] = RGB555(0,0,0);

	for (i=(320*240)-1;i>=0;--i)
	{
		*((uint8*)gtSurface[0].ptbuffer+i) = 0xFF;
		*((uint8*)gtSurface[1].ptbuffer+i) = 0xFF;
	}
}

//---------------------------------------------------------------------------
inline void  SetPalette(int Index, byte PalData)
{
	uint16 Pal;
	int i;
	int r,g,b;

	ColTbl[Index]=PalData;
	i=Index&0x3FE;
	Pal=(ColTbl[i+1]<<8) |ColTbl[i];

	r = (int)(((Pal>>8)&0x0f)*gammaC)<<1;
	g = (int)(((Pal>>4)&0x0f)*gammaC)<<1;
	b = (int)((Pal&0x0f)*gammaC)<<1;

	if (r > 31) r = 31;
	if (g > 31) g = 31;
	if (b > 31) b = 31;

	if ((Index >> 1) < 255)	
		Palette[Index >> 1] = RGB555(r,g,b);

	return;
}

void	RebuildPalette()
{
	uint16 Pal;
	int i;
	int r,g,b;

	for (i = 0; i < 512; i+=2)
	{
		Pal=(ColTbl[i+1]<<8) |ColTbl[i];

		r = (int)(((Pal>>8)&0x0f)*gammaC)<<1;
		g = (int)(((Pal>>4)&0x0f)*gammaC)<<1;
		b = (int)((Pal&0x0f)*gammaC)<<1;

		if (r > 31) r = 31;
		if (g > 31) g = 31;
		if (b > 31) b = 31;

		if ((i >> 1) < 255)	
			Palette[i >> 1] = RGB555(r,g,b);
	}
}
//---------------------------------------------------------------------------
void	SetHWPalette()
{
	uint32 *HWPALETTE=(uint32 *)0x14A00400;
	int i;

	for (i = 0; i < 256; i++)
		HWPALETTE[i] =	Palette[i];

	return;
}

//---------------------------------------------------------------------------
void SetDrawMode(int Mode)
{
	if ((vert == 1) && (Mode == 3))
		Mode = 4;

	DrawMode=Mode;
	WsDrawCreate();
    return;
}

int  GetDrawMode()
{
	if ((vert == 1) && (DrawMode == 4))
		return 3;

	return DrawMode;
}

void WsClearGpuCache(void)
{
	if (ws_modified_tile != NULL)
		GPMEMSET(ws_modified_tile,0x01,512);

	if (wsc_modified_tile != NULL)
		GPMEMSET(wsc_modified_tile,0x01,1024);
}

uint8 *WsGetTileRow(uint32 TMap, uint32 line, uint32 sprite)
{
	int forline;
	int vFlip;
	int hFlip;
	int bank;
	int tileIndex;
	
	if (sprite == 0)
	{
		tileIndex	= TMap&MAP_TILE;
		vFlip		= TMap&MAP_VREV;
		hFlip		= TMap&MAP_HREV;
		bank		= TMap&MAP_BANK;
	}
	else
	{
		tileIndex	= TMap&SPR_TILE;
		vFlip		= TMap&SPR_VREV;
		hFlip		= TMap&SPR_HREV;
		bank		= 0;
	}

	if (bank)
		tileIndex+=512;

	switch(COLCTL & 0xE0)
	{
	case 0xE0:		// 16 color packed (so easy!)
		{
			if ((wsc_modified_tile[tileIndex] == 1))
			{

				uint8	*tileInCachePtr = &wsc_tile_cache[tileIndex<<6];
				uint8	*hflippedTileInCachePtr = &wsc_hflipped_tile_cache[tileIndex<<6];
				uint32	*tileInRamPtr   = (uint32*)&IRAM[0x4000+(tileIndex<<5)];
				uint32	tileLine;

				for (forline=0;forline<8;forline++)
				{
					tileLine=*tileInRamPtr++;
					
					tileInCachePtr[0]=(tileLine>>4)&0x0f;
					tileInCachePtr[1]=(tileLine>>0)&0x0f;
					tileInCachePtr[2]=(tileLine>>12)&0x0f;
					tileInCachePtr[3]=(tileLine>>8)&0x0f;
					tileInCachePtr[4]=(tileLine>>20)&0x0f;
					tileInCachePtr[5]=(tileLine>>16)&0x0f;
					tileInCachePtr[6]=(tileLine>>28)&0x0f;
					tileInCachePtr[7]=(tileLine>>24)&0x0f;

					hflippedTileInCachePtr[7]=tileInCachePtr[0];
					hflippedTileInCachePtr[6]=tileInCachePtr[1];
					hflippedTileInCachePtr[5]=tileInCachePtr[2];
					hflippedTileInCachePtr[4]=tileInCachePtr[3];
					hflippedTileInCachePtr[3]=tileInCachePtr[4];
					hflippedTileInCachePtr[2]=tileInCachePtr[5];
					hflippedTileInCachePtr[1]=tileInCachePtr[6];
					hflippedTileInCachePtr[0]=tileInCachePtr[7];

					tileInCachePtr+=8;
					hflippedTileInCachePtr+=8;				
				}
				wsc_modified_tile[tileIndex]=0;
			}
			if (vFlip)
				line=7-line;
			if (hFlip)
				return(&wsc_hflipped_tile_cache[(tileIndex<<6)+(line<<3)]);
			else
				return(&wsc_tile_cache[(tileIndex<<6)+(line<<3)]);
		}
		break;
	case 0xC0:		// 16 color layered (not so easy!)
		{
			if ((wsc_modified_tile[tileIndex] == 1))
			{
				uint8	*tileInCachePtr			= &wsc_tile_cache[tileIndex<<6];
				uint8	*hflippedTileInCachePtr = &wsc_hflipped_tile_cache[tileIndex<<6];
				uint32	*tileInRamPtr			= (uint32*)&IRAM[0x4000+(tileIndex<<5)];
				uint32	tileLine;
				uint32	j;

				for (forline=0;forline<8;forline++)
				{
					tileLine=*tileInRamPtr++;
					
					tileInCachePtr[0] = ((((j=tileLine&0x80808080)>>28)|(j>>21)|(j>>14)|(j>>7))&0x0F);
					tileInCachePtr[1] = ((((j=tileLine&0x40404040)>>27)|(j>>20)|(j>>13)|(j>>6))&0x0F);
					tileInCachePtr[2] = ((((j=tileLine&0x20202020)>>26)|(j>>19)|(j>>12)|(j>>5))&0x0F);
					tileInCachePtr[3] = ((((j=tileLine&0x10101010)>>25)|(j>>18)|(j>>11)|(j>>4))&0x0F);
					tileInCachePtr[4] = ((((j=tileLine&0x08080808)>>24)|(j>>17)|(j>>10)|(j>>3))&0x0F);
					tileInCachePtr[5] = ((((j=tileLine&0x04040404)>>23)|(j>>16)|(j>>9)|(j>>2))&0x0F);
					tileInCachePtr[6] = ((((j=tileLine&0x02020202)>>22)|(j>>15)|(j>>8)|(j>>1))&0x0F);
					tileInCachePtr[7] = ((((j=tileLine&0x01010101)>>21)|(j>>14)|(j>>7)|(j))&0x0F);

					hflippedTileInCachePtr[7]=tileInCachePtr[0];
					hflippedTileInCachePtr[6]=tileInCachePtr[1];
					hflippedTileInCachePtr[5]=tileInCachePtr[2];
					hflippedTileInCachePtr[4]=tileInCachePtr[3];
					hflippedTileInCachePtr[3]=tileInCachePtr[4];
					hflippedTileInCachePtr[2]=tileInCachePtr[5];
					hflippedTileInCachePtr[1]=tileInCachePtr[6];
					hflippedTileInCachePtr[0]=tileInCachePtr[7];

					tileInCachePtr+=8;
					hflippedTileInCachePtr+=8;
				}
				wsc_modified_tile[tileIndex]=0;
			}
			if (vFlip)
				line=7-line;
			if (hFlip)
				return(&wsc_hflipped_tile_cache[(tileIndex<<6)+(line<<3)]);
			else
				return(&wsc_tile_cache[(tileIndex<<6)+(line<<3)]);
		}
		break;
	case 0x60:		// 4 color packed (so easy!)
		{
			if ((ws_modified_tile[tileIndex] == 1))
			{
				uint8	*tileInCachePtr			= &wsc_tile_cache[tileIndex<<6];
				uint8	*hflippedTileInCachePtr	= &wsc_hflipped_tile_cache[(tileIndex<<6)];
				uint16	*tileInRamPtr			= (uint16*)&IRAM[0x2000+(tileIndex<<4)];
				uint16	tileLine;
				uint32	j;

				for (forline=0;forline<8;forline++)
				{
					tileLine=*tileInRamPtr++;
					
					tileInCachePtr[0]=(tileLine>>6)&0x03;
					tileInCachePtr[1]=(tileLine>>4)&0x03;
					tileInCachePtr[2]=(tileLine>>2)&0x03;
					tileInCachePtr[3]=(tileLine>>0)&0x03;
					tileInCachePtr[4]=(tileLine>>14)&0x03;
					tileInCachePtr[5]=(tileLine>>12)&0x03;
					tileInCachePtr[6]=(tileLine>>10)&0x03;
					tileInCachePtr[7]=(tileLine>>8)&0x03;

					hflippedTileInCachePtr[7]=tileInCachePtr[0];
					hflippedTileInCachePtr[6]=tileInCachePtr[1];
					hflippedTileInCachePtr[5]=tileInCachePtr[2];
					hflippedTileInCachePtr[4]=tileInCachePtr[3];
					hflippedTileInCachePtr[3]=tileInCachePtr[4];
					hflippedTileInCachePtr[2]=tileInCachePtr[5];
					hflippedTileInCachePtr[1]=tileInCachePtr[6];
					hflippedTileInCachePtr[0]=tileInCachePtr[7];

					tileInCachePtr+=8;
					hflippedTileInCachePtr+=8;				
				}
				ws_modified_tile[tileIndex]=0;
			}
			if (vFlip)
				line=7-line;
			if (hFlip)
				return(&wsc_hflipped_tile_cache[(tileIndex<<6)+(line<<3)]);
			else
				return(&wsc_tile_cache[(tileIndex<<6)+(line<<3)]);
		}
		break;
	case 0x40:		// 4 color layered (not so easy!)
		{
			if ((ws_modified_tile[tileIndex] == 1))
			{
				uint8	*tileInCachePtr			= &wsc_tile_cache[tileIndex<<6];
				uint8	*hflippedTileInCachePtr	= &wsc_hflipped_tile_cache[(tileIndex<<6)];
				uint16	*tileInRamPtr			= (uint16*)&IRAM[0x2000+(tileIndex<<4)];
				uint16	tileLine;
				uint32	j;

				for (forline=0;forline<8;forline++)
				{
					tileLine=*tileInRamPtr++;
					
					tileInCachePtr[0] = ((((j=tileLine&0x8080)>>14)|(j>>7))&0x03);
					tileInCachePtr[1] = ((((j=tileLine&0x4040)>>13)|(j>>6))&0x03);
					tileInCachePtr[2] = ((((j=tileLine&0x2020)>>12)|(j>>5))&0x03);
					tileInCachePtr[3] = ((((j=tileLine&0x1010)>>11)|(j>>4))&0x03);
					tileInCachePtr[4] = ((((j=tileLine&0x0808)>>10)|(j>>3))&0x03);
					tileInCachePtr[5] = ((((j=tileLine&0x0404)>>9)|(j>>2))&0x03);
					tileInCachePtr[6] = ((((j=tileLine&0x0202)>>8)|(j>>1))&0x03);
					tileInCachePtr[7] = ((((j=tileLine&0x0101)>>7)|(j>>0))&0x03);

					hflippedTileInCachePtr[7]=tileInCachePtr[0];
					hflippedTileInCachePtr[6]=tileInCachePtr[1];
					hflippedTileInCachePtr[5]=tileInCachePtr[2];
					hflippedTileInCachePtr[4]=tileInCachePtr[3];
					hflippedTileInCachePtr[3]=tileInCachePtr[4];
					hflippedTileInCachePtr[2]=tileInCachePtr[5];
					hflippedTileInCachePtr[1]=tileInCachePtr[6];
					hflippedTileInCachePtr[0]=tileInCachePtr[7];

					tileInCachePtr+=8;
					hflippedTileInCachePtr+=8;
				}
				ws_modified_tile[tileIndex]=0;
			}
			if (vFlip)
				line=7-line;
			if (hFlip)
				return(&wsc_hflipped_tile_cache[(tileIndex<<6)+(line<<3)]);
			else
				return(&wsc_tile_cache[(tileIndex<<6)+(line<<3)]);
		}
		break;
	case 0x20:		// 4 mono packed (so easy!)
		{
			if ((ws_modified_tile[tileIndex] == 1))
			{

				uint8	*tileInCachePtr			= &ws_tile_cache[tileIndex<<6];
				uint8	*hflippedTileInCachePtr = &ws_hflipped_tile_cache[tileIndex<<6];
				uint32	*tileInRamPtr			= (uint32*)&IRAM[0x2000+(tileIndex<<4)];
				uint16	tileLine;

				for (forline=0;forline<8;forline++)
				{
					tileLine=*tileInRamPtr++;
					
					tileInCachePtr[0]=(tileLine>>6)&0x03;
					tileInCachePtr[1]=(tileLine>>4)&0x03;
					tileInCachePtr[2]=(tileLine>>2)&0x03;
					tileInCachePtr[3]=(tileLine>>0)&0x03;
					tileInCachePtr[4]=(tileLine>>14)&0x03;
					tileInCachePtr[5]=(tileLine>>12)&0x03;
					tileInCachePtr[6]=(tileLine>>10)&0x03;
					tileInCachePtr[7]=(tileLine>>8)&0x03;

					hflippedTileInCachePtr[7]=tileInCachePtr[0];
					hflippedTileInCachePtr[6]=tileInCachePtr[1];
					hflippedTileInCachePtr[5]=tileInCachePtr[2];
					hflippedTileInCachePtr[4]=tileInCachePtr[3];
					hflippedTileInCachePtr[3]=tileInCachePtr[4];
					hflippedTileInCachePtr[2]=tileInCachePtr[5];
					hflippedTileInCachePtr[1]=tileInCachePtr[6];
					hflippedTileInCachePtr[0]=tileInCachePtr[7];

					tileInCachePtr+=8;
					hflippedTileInCachePtr+=8;				
				}
				ws_modified_tile[tileIndex]=0;
			}
			if (vFlip)
				line=7-line;
			if (hFlip)
				return(&ws_hflipped_tile_cache[(tileIndex<<6)+(line<<3)]);
			else
				return(&ws_tile_cache[(tileIndex<<6)+(line<<3)]);
		}
		break;
	case 0x00:		// 4 mono layered (not so easy!)
		{
			if ((ws_modified_tile[tileIndex] == 1))
			{
				uint8	*tileInCachePtr			 = &ws_tile_cache[tileIndex<<6];
				uint8	*hflippedTileInCachePtr	 = &ws_hflipped_tile_cache[(tileIndex<<6)+7];
				uint32	*tileInRamPtr			 = (uint32*)&IRAM[0x2000+(tileIndex<<4)];
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
		break;
	default:
		break;
	}
	return(NULL);
}
