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

byte ColTbl[0x210];
uint16 Palette[16+1][16];

uint16 screenbuffer[(224+16)*144];

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

extern unsigned short WSCBmp_horz[];
extern unsigned short WSCBmp_vert[];

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

video_mode video_modes[6] =
{
	horz_render_normal,		48,	41,	0,
	horz_render_hstretch,	0,	41,	6,
	horz_render_hvstretch,	0,	12,	6,
	vert_render_normal,		88,	7,	0,
	horz_render_normal,		48,	41,	0,			//for savestates
	vert_render_normal,		88,	0,	0			//for savestates
};

#define VERT_BUFFER_MODE 1
#define HORZ_BUFFER_MODE 240

//#define BUFFER_MODE HORZ_BUFFER_MODE
#define BUFFER_MODE VERT_BUFFER_MODE

#include "newgfxcore.h"

void  (*RefreshLine)(int Line, void* lpSurface);

//---------------------------------------------------------------------------
int  WsDrawCreate()
{
    int result;

	video_update = video_modes[DrawMode].video_update;
	video_x = video_modes[DrawMode].video_x;
	video_y = video_modes[DrawMode].video_y;
	scroll_x = video_modes[DrawMode].scroll_x;

	RefreshLine = RefreshLineOld;

    WsDrawClear();
    return 0;
}

//---------------------------------------------------------------------------
int  WsDrawLine(int Line)
{
    int result;
	char debug[100];

//	RefreshLine(Line, &screenbuffer[(224+16)*Line]);

#if BUFFER_MODE == HORZ_BUFFER_MODE
	RefreshLine(Line, gtSurface[giSurface].ptbuffer+(240*2*video_x)-(video_y*2)-(8*2)+(480-(Line*2))); // works for horz
#else
	RefreshLine(Line, gtSurface[giSurface].ptbuffer+(240*2*video_x)-(video_y*2)-(8*2)+(Line*2*240)); // works for vert
#endif

/*
	GPSPRINTF(debug, "videomode = %x", IO[0x60]);

	okf.x = 12;
	okf.y = 225;

	OkfPrintSurface(debug,0);
*/
    return 0;
}

//---------------------------------------------------------------------------
int  WsDrawFlip(void)
{
//	video_update((byte*)screenbuffer+(scroll_x*2)+16, gtSurface[0].ptbuffer+(240*2*video_x)-(video_y*2));
	SurfaceFlip();
	return 0;
}

//---------------------------------------------------------------------------
void  SetPalette(int Index, byte PalData)
{
	uint16 Pal;
    uint16 PF;
	int i,j;

	ColTbl[Index]=PalData;
    i=Index&0x3FE;
	Pal=(ColTbl[i+1]<<8) |ColTbl[i];

	PF = RGB555(((Pal>>8)&0x0f)<<1,((Pal>>4)&0x0f)<<1,(Pal&0x0f)<<1);

	i=Index>>5;
	j=(Index>>1)&0x0F;

	Palette[i][j]=PF;

	return;
}


//---------------------------------------------------------------------------
void  WsDrawClear(void)
{
	int i;
	
	switch(DrawMode)
	{
		case 0:
		{
			for (i = 0; i < (320*240); i++)
			{
				*((uint16*)gtSurface[0].ptbuffer+i) = WSCBmp_horz[i];
				*((uint16*)gtSurface[1].ptbuffer+i) = WSCBmp_horz[i];
			}
		}
			break;
		case 1:
		case 2:
		{
			for (i=(320*240)-1;i>=0;--i)
			{
				*((uint16*)gtSurface[0].ptbuffer+i) = 0;
				*((uint16*)gtSurface[1].ptbuffer+i) = 0;
			}
		}
			break;
		case 3:
		{
			for (i = 0; i < (320*240); i++)
			{
				*((uint16*)gtSurface[0].ptbuffer+i) = WSCBmp_vert[i];
				*((uint16*)gtSurface[1].ptbuffer+i) = WSCBmp_vert[i];
			}
		}
			break;
		case 4:
		{
			Cls(0);
			Cls(1);
			DrawWindow(44, 37, 228, 148, 0, COLOR_RED, COLOR_BLUE); // file selector window
			DrawWindow(5, 219,  305, 15,  0, COLOR_RED, COLOR_BLUE); // totals window
			DrawWindow(44, 37, 228, 148, 1, COLOR_RED, COLOR_BLUE); // file selector window
			DrawWindow(5, 219,  305, 15,  1, COLOR_RED, COLOR_BLUE); // totals window

		}
			break;
		case 5:
		{
			Cls(0);
			Cls(1);
			DrawWindow(85, 0,  148, 228, 0, COLOR_RED, COLOR_BLUE); // file selector window
			DrawWindow(5, 225,  305, 12,  0, COLOR_RED, COLOR_BLUE); // totals window
			DrawWindow(85, 0,  148, 228, 1, COLOR_RED, COLOR_BLUE); // file selector window
			DrawWindow(5, 225,  305, 12,  1, COLOR_RED, COLOR_BLUE); // totals window
		}
			break;
		default:
			break;
	}	
}

//---------------------------------------------------------------------------
int  SetDrawMode(int Mode)
{
	DrawMode=Mode;
	WsDrawCreate();
    return 0;
}

int  GetDrawMode()
{
	return DrawMode;
}



