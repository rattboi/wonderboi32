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
	RefreshLine(Line, gtSurface[0].ptbuffer+(240*2*video_x)-(video_y*2)-(8*2)+(480-(Line*2)));

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

void  RefreshLineOld(int Line, void* buffer)
{
	uint16	*pSBuf;			// 서페스하″파의 (0, Y) 좌표호˚인터(8艱″실 포함하지 않고)
	uint16	*pSWrBuf16;

	int *pZ;				// ZBuf의 라이트호˚인터
	int ZBuf[0x100];		// Z絿스크
	int *pW;				// WBuf의 라이트호˚인터
	int WBuf[0x100];		// 윈트″우마스크

	int OffsetX;			// X좌표 오프셋(0∼7)
	int OffsetY;			// Y좌표 오프셋(0∼E)

	byte *pbTMap;			// 독 붐비어 타이르맛후˚의 호˚인터
	int TMap;				// 타이르맛후˚
	int TMapX;				// 타이르맛후˚의 메쉬 X값
	int TMapXEnd;			// 타이르맛후˚의 하″크크″라운트″영역 종료의 메쉬 X값

	byte *pbTData;			// 독 붐비어 타이르테″타의 호˚인터
	int TData;				// 타이르테″타

    int i, j, k, index[8];

	uint16 *pbPal;			// 하˚렛호˚인터
	uint16 defpal = 0;

	pSBuf=(uint16 *) buffer+8;
	pSWrBuf16=pSBuf;

	if(LCDSLP&0x01)
	{
		if(COLCTL&0xE0) // Any color mode
		{
			pbPal=&Palette[(BORDER&0xF0)>>4][BORDER&0x0F];
		}
		else // mono mode
		{
			pbPal=&Palette[16][BORDER&0x07];
		}
	}
	else
	{
		pbPal=&defpal;
	}
	for(i=0;i<224;i++)
	{
		*pSWrBuf16=*pbPal; pSWrBuf16 += 240;
	}

	if(!(LCDSLP&0x01)) return;

 	if(DSPCTL&0x01) // BG Layer on
	{
		OffsetX=SCR1X&0x07;						// X좌표 오프셋을 설정
		pSWrBuf16=pSBuf-(OffsetX * 240); //PixelDepth;		// 서페스하″파의 기입호˚인터를 X됫후셋트
		i=Line+SCR1Y;
		OffsetY=(i&0x07);						// Y좌표 오프셋을 설정

		pbTMap=Scr1TMap+((i&0xF8)<<3);			// 타이르맛후˚의 (0, Y) 좌표호˚인터
		TMapX=(SCR1X&0xF8)>>2;					// 타이르맛후˚의 X값을 설정
		TMapXEnd=((SCR1X+224+7)>>2) &0xFFE;		// 하″크크″라운트″영역 종료의 X값을 설정

		for(;TMapX<TMapXEnd;)					// 하″크크″라운트″영역을 묘화
		{
			TMap=*(pbTMap+(TMapX++&0x3F));		// 타이르맛후˚를 읽어들인다
			TMap|=*(pbTMap+(TMapX++&0x3F))<<8;	// 타이르맛후˚를 읽어들인다

			if(COLCTL&0x40) // Color Mode
			{
            	if(TMap&MAP_BANK)
            	{
            		pbTData=IRAM+0x8000;
            	}
            	else
            	{
            		pbTData=IRAM+0x4000;
            	}
				pbTData+=(TMap&MAP_TILE)<<5;
				if(TMap&MAP_VREV)
       			{
            		pbTData+=(7-OffsetY)<<2;// 타이르테″타의 호˚인터를 설정
       			}
				else
				{
					pbTData+=OffsetY<<2;// 타이르테″타의 호˚인터를 설정
				}
			}
			else	// Mono Mode
			{
            	pbTData=IRAM+0x2000;
            	pbTData+=(TMap&MAP_TILE)<<4;
				if(TMap&MAP_VREV)
       			{
            		pbTData+=(7-OffsetY)<<1;// 타이르테″타의 호˚인터를 설정
       			}
				else
				{
					pbTData+=OffsetY<<1;// 타이르테″타의 호˚인터를 설정
				}
			}

			if(COLCTL&0x20)						// Packed Mode
			{
				if(COLCTL&0x40)					// 16 Color
				{
					index[0]=(pbTData[0]&0xF0)>>4;
					index[1]=pbTData[0]&0x0F;
					index[2]=(pbTData[1]&0xF0)>>4;
					index[3]=pbTData[1]&0x0F;
					index[4]=(pbTData[2]&0xF0)>>4;
					index[5]=pbTData[2]&0x0F;
					index[6]=(pbTData[3]&0xF0)>>4;
					index[7]=pbTData[3]&0x0F;
				}
				else							// 4 Color
				{
					index[0]=(pbTData[0]&0xC0)>>6;
					index[1]=(pbTData[0]&0x30)>>4;
					index[2]=(pbTData[0]&0x0C)>>2;
					index[3]=pbTData[0]&0x03;
					index[4]=(pbTData[1]&0xC0)>>6;
					index[5]=(pbTData[1]&0x30)>>4;
					index[6]=(pbTData[1]&0x0C)>>2;
					index[7]=pbTData[1]&0x03;
				}
			}
			else
			{
				if(COLCTL&0x40)					// 16 Color (unpacked)
				{
					index[0] =(pbTData[0]&0x80)? 0x1:0;
					index[0]|=(pbTData[1]&0x80)? 0x2:0;
					index[0]|=(pbTData[2]&0x80)? 0x4:0;
					index[0]|=(pbTData[3]&0x80)? 0x8:0;
					index[1] =(pbTData[0]&0x40)? 0x1:0;
					index[1]|=(pbTData[1]&0x40)? 0x2:0;
					index[1]|=(pbTData[2]&0x40)? 0x4:0;
					index[1]|=(pbTData[3]&0x40)? 0x8:0;
					index[2] =(pbTData[0]&0x20)? 0x1:0;
					index[2]|=(pbTData[1]&0x20)? 0x2:0;
					index[2]|=(pbTData[2]&0x20)? 0x4:0;
					index[2]|=(pbTData[3]&0x20)? 0x8:0;
					index[3] =(pbTData[0]&0x10)? 0x1:0;
					index[3]|=(pbTData[1]&0x10)? 0x2:0;
					index[3]|=(pbTData[2]&0x10)? 0x4:0;
					index[3]|=(pbTData[3]&0x10)? 0x8:0;
					index[4] =(pbTData[0]&0x08)? 0x1:0;
					index[4]|=(pbTData[1]&0x08)? 0x2:0;
					index[4]|=(pbTData[2]&0x08)? 0x4:0;
					index[4]|=(pbTData[3]&0x08)? 0x8:0;
					index[5] =(pbTData[0]&0x04)? 0x1:0;
					index[5]|=(pbTData[1]&0x04)? 0x2:0;
					index[5]|=(pbTData[2]&0x04)? 0x4:0;
					index[5]|=(pbTData[3]&0x04)? 0x8:0;
					index[6] =(pbTData[0]&0x02)? 0x1:0;
					index[6]|=(pbTData[1]&0x02)? 0x2:0;
					index[6]|=(pbTData[2]&0x02)? 0x4:0;
					index[6]|=(pbTData[3]&0x02)? 0x8:0;
					index[7] =(pbTData[0]&0x01)? 0x1:0;
					index[7]|=(pbTData[1]&0x01)? 0x2:0;
					index[7]|=(pbTData[2]&0x01)? 0x4:0;
					index[7]|=(pbTData[3]&0x01)? 0x8:0;
				}
				else							// 4 Color (unpacked)
				{
					index[0] =(pbTData[0]&0x80)? 0x1:0;
					index[0]|=(pbTData[1]&0x80)? 0x2:0;
					index[1] =(pbTData[0]&0x40)? 0x1:0;
					index[1]|=(pbTData[1]&0x40)? 0x2:0;
					index[2] =(pbTData[0]&0x20)? 0x1:0;
					index[2]|=(pbTData[1]&0x20)? 0x2:0;
					index[3] =(pbTData[0]&0x10)? 0x1:0;
					index[3]|=(pbTData[1]&0x10)? 0x2:0;
					index[4] =(pbTData[0]&0x08)? 0x1:0;
					index[4]|=(pbTData[1]&0x08)? 0x2:0;
					index[5] =(pbTData[0]&0x04)? 0x1:0;
					index[5]|=(pbTData[1]&0x04)? 0x2:0;
					index[6] =(pbTData[0]&0x02)? 0x1:0;
					index[6]|=(pbTData[1]&0x02)? 0x2:0;
					index[7] =(pbTData[0]&0x01)? 0x1:0;
					index[7]|=(pbTData[1]&0x01)? 0x2:0;
				}
			}

            if(TMap&MAP_HREV)
           	{
				j=index[0];
				index[0]=index[7];
				index[7]=j;
				j=index[1];
				index[1]=index[6];
				index[6]=j;
				j=index[2];
				index[2]=index[5];
				index[5]=j;
				j=index[3];
				index[3]=index[4];
				index[4]=j;
			}

			if((!index[0])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf16+=240;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[0]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += 240;
			}
			if((!index[1])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))  pSWrBuf16 += 240;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[1]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += 240; 
			}
			if((!index[2])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))  pSWrBuf16 += 240;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[2]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += 240; 
			}
			if((!index[3])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))  pSWrBuf16 += 240;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[3]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += 240; 
			}
			if((!index[4])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))  pSWrBuf16 += 240;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[4]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += 240; 
			}
			if((!index[5])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))  pSWrBuf16 += 240;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[5]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += 240; 
			}
			if((!index[6])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))  pSWrBuf16 += 240;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[6]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += 240; 
			}
			if((!index[7])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))  pSWrBuf16 += 240;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[7]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += 240; 
			}
		}
	}

	GPMEMSET(&ZBuf, 0, sizeof(ZBuf));

	if(DSPCTL&0x02)			// FG Layer on
	{
		if((DSPCTL&0x30) ==0x20) // FG Layer displayed only inside window
		{
            for(i=0, pW=WBuf+8;i<224;i++)
			{
        		*pW++=1;
			}
			if((Line>=SCR2WT)&&(Line<=SCR2WB))
            {
				for(i=SCR2WL, pW=WBuf+8+i;i<=SCR2WR;i++)
            	{
            		*pW++=0;
            	}
            }
		}
		else if((DSPCTL&0x30) ==0x30) // FG Layer displayed only outside window
		{
            for(i=0, pW=WBuf+8;i<224;i++)
			{
        		*pW++=0;
			}
			if((Line>=SCR2WT)&&(Line<=SCR2WB))
            {
				for(i=SCR2WL, pW=WBuf+8+i;i<=SCR2WR;i++)
            	{
            		*pW++=1;
            	}
            }
		}
		else  // no FG window
		{
			for(i=0, pW=WBuf+8;i<0x100;i++)
			{
				*pW++=0;
			}
		}

		OffsetX=SCR2X&0x07;						// X좌표 오프셋을 설정
		pSWrBuf16=pSBuf-(OffsetX * 240);		// 서페스하″파의 기입호˚인터를 X됫후셋트
		i=Line+SCR2Y;
		OffsetY=(i&0x07);						// Y좌표 오프셋을 설정

		pbTMap=Scr2TMap+((i&0xF8)<<3);			// 타이르맛후˚의 (0, Y) 좌표호˚인터
		TMapX=(SCR2X&0xF8)>>2;					// 타이르맛후˚의 X값을 설정
		TMapXEnd=((SCR2X+224+7)>>2) &0xFFE;				// 하″크크″라운트″영역 종료의 X값을 설정

		pW=WBuf+8-OffsetX;
		pZ=ZBuf+8-OffsetX;
        
		for(;TMapX<TMapXEnd;)					// 하″크크″라운트″영역을 묘화
		{
			TMap=*(pbTMap+(TMapX++&0x3F));		// 타이르맛후˚를 읽어들인다
			TMap|=*(pbTMap+(TMapX++&0x3F))<<8;	// 타이르맛후˚를 읽어들인다

			if(COLCTL&0x40)
			{
            	if(TMap&MAP_BANK)
            	{
            		pbTData=IRAM+0x8000;
            	}
            	else
            	{
            		pbTData=IRAM+0x4000;
            	}
				pbTData+=(TMap&MAP_TILE)<<5;
				if(TMap&MAP_VREV)
       			{
            		pbTData+=(7-OffsetY)<<2;// 타이르테″타의 호˚인터를 설정
       			}
				else
				{
					pbTData+=OffsetY<<2;// 타이르테″타의 호˚인터를 설정
				}
			}
			else
			{
            	pbTData=IRAM+0x2000;
            	pbTData+=(TMap&MAP_TILE)<<4;
				if(TMap&MAP_VREV)
       			{
            		pbTData+=(7-OffsetY)<<1;// 타이르테″타의 호˚인터를 설정
       			}
				else
				{
					pbTData+=OffsetY<<1;// 타이르테″타의 호˚인터를 설정
				}
			}

			if(COLCTL&0x20)						// Packed Mode
			{
				if(COLCTL&0x40)					// 16 Color
				{
					index[0]=(pbTData[0]&0xF0)>>4;
					index[1]=pbTData[0]&0x0F;
					index[2]=(pbTData[1]&0xF0)>>4;
					index[3]=pbTData[1]&0x0F;
					index[4]=(pbTData[2]&0xF0)>>4;
					index[5]=pbTData[2]&0x0F;
					index[6]=(pbTData[3]&0xF0)>>4;
					index[7]=pbTData[3]&0x0F;
				}
				else							// 4 Color
				{
					index[0]=(pbTData[0]&0xC0)>>6;
					index[1]=(pbTData[0]&0x30)>>4;
					index[2]=(pbTData[0]&0x0C)>>2;
					index[3]=pbTData[0]&0x03;
					index[4]=(pbTData[1]&0xC0)>>6;
					index[5]=(pbTData[1]&0x30)>>4;
					index[6]=(pbTData[1]&0x0C)>>2;
					index[7]=pbTData[1]&0x03;
				}
			}
			else
			{
				if(COLCTL&0x40)					// 16 Color
				{
					index[0] =(pbTData[0]&0x80)? 0x1:0;
					index[0]|=(pbTData[1]&0x80)? 0x2:0;
					index[0]|=(pbTData[2]&0x80)? 0x4:0;
					index[0]|=(pbTData[3]&0x80)? 0x8:0;
					index[1] =(pbTData[0]&0x40)? 0x1:0;
					index[1]|=(pbTData[1]&0x40)? 0x2:0;
					index[1]|=(pbTData[2]&0x40)? 0x4:0;
					index[1]|=(pbTData[3]&0x40)? 0x8:0;
					index[2] =(pbTData[0]&0x20)? 0x1:0;
					index[2]|=(pbTData[1]&0x20)? 0x2:0;
					index[2]|=(pbTData[2]&0x20)? 0x4:0;
					index[2]|=(pbTData[3]&0x20)? 0x8:0;
					index[3] =(pbTData[0]&0x10)? 0x1:0;
					index[3]|=(pbTData[1]&0x10)? 0x2:0;
					index[3]|=(pbTData[2]&0x10)? 0x4:0;
					index[3]|=(pbTData[3]&0x10)? 0x8:0;
					index[4] =(pbTData[0]&0x08)? 0x1:0;
					index[4]|=(pbTData[1]&0x08)? 0x2:0;
					index[4]|=(pbTData[2]&0x08)? 0x4:0;
					index[4]|=(pbTData[3]&0x08)? 0x8:0;
					index[5] =(pbTData[0]&0x04)? 0x1:0;
					index[5]|=(pbTData[1]&0x04)? 0x2:0;
					index[5]|=(pbTData[2]&0x04)? 0x4:0;
					index[5]|=(pbTData[3]&0x04)? 0x8:0;
					index[6] =(pbTData[0]&0x02)? 0x1:0;
					index[6]|=(pbTData[1]&0x02)? 0x2:0;
					index[6]|=(pbTData[2]&0x02)? 0x4:0;
					index[6]|=(pbTData[3]&0x02)? 0x8:0;
					index[7] =(pbTData[0]&0x01)? 0x1:0;
					index[7]|=(pbTData[1]&0x01)? 0x2:0;
					index[7]|=(pbTData[2]&0x01)? 0x4:0;
					index[7]|=(pbTData[3]&0x01)? 0x8:0;
				}
				else							// 4 Color
				{
					index[0] =(pbTData[0]&0x80)? 0x1:0;
					index[0]|=(pbTData[1]&0x80)? 0x2:0;
					index[1] =(pbTData[0]&0x40)? 0x1:0;
					index[1]|=(pbTData[1]&0x40)? 0x2:0;
					index[2] =(pbTData[0]&0x20)? 0x1:0;
					index[2]|=(pbTData[1]&0x20)? 0x2:0;
					index[3] =(pbTData[0]&0x10)? 0x1:0;
					index[3]|=(pbTData[1]&0x10)? 0x2:0;
					index[4] =(pbTData[0]&0x08)? 0x1:0;
					index[4]|=(pbTData[1]&0x08)? 0x2:0;
					index[5] =(pbTData[0]&0x04)? 0x1:0;
					index[5]|=(pbTData[1]&0x04)? 0x2:0;
					index[6] =(pbTData[0]&0x02)? 0x1:0;
					index[6]|=(pbTData[1]&0x02)? 0x2:0;
					index[7] =(pbTData[0]&0x01)? 0x1:0;
					index[7]|=(pbTData[1]&0x01)? 0x2:0;
				}
			}

            if(TMap&MAP_HREV)
           	{
				j=index[0];
				index[0]=index[7];
				index[7]=j;
				j=index[1];
				index[1]=index[6];
				index[6]=j;
				j=index[2];
				index[2]=index[5];
				index[5]=j;
				j=index[3];
				index[3]=index[4];
				index[4]=j;
			}

			if(((!index[0])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW))  pSWrBuf16 += 240;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[0]];
				*pSWrBuf16=*pbPal++;  pSWrBuf16 += 240; 
				*pZ=1;
			}
			pW++;pZ++;
			if(((!index[1])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW))  pSWrBuf16 += 240;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[1]];
				*pSWrBuf16=*pbPal++;  pSWrBuf16 += 240; 
				*pZ=1;
			}
			pW++;pZ++;
			if(((!index[2])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW))  pSWrBuf16 += 240;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[2]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += 240; 
				*pZ=1;
			}
			pW++;pZ++;
			if(((!index[3])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW))  pSWrBuf16 += 240;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[3]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += 240; 
				*pZ=1;
			}
			pW++;pZ++;
			if(((!index[4])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW))  pSWrBuf16 += 240;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[4]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += 240; 
				*pZ=1;
			}
			pW++;pZ++;
			if(((!index[5])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW))  pSWrBuf16 += 240;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[5]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += 240; 
				*pZ=1;
			}
			pW++;pZ++;
			if(((!index[6])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW))  pSWrBuf16 += 240;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[6]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += 240; 
				*pZ=1;
			}
			pW++;pZ++;
			if(((!index[7])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW))  pSWrBuf16 += 240;
			else
			{
				pbPal=&Palette[(TMap&MAP_PAL)>>9][index[7]];
				*pSWrBuf16=*pbPal++; pSWrBuf16 += 240; 
				*pZ=1;
			}
			pW++;pZ++;
		}
	}

	if(DSPCTL&0x04) // sprites on
	{
		if(DSPCTL&0x08) //sprite window on
        {
            for(i=0, pW=WBuf+8;i<224;i++)
			{
        		*pW++=1;
			}
			if((Line>=SPRWT)&&(Line<=SPRWB))
            {
				for(i=SPRWL, pW=WBuf+8+i;i<=SPRWR;i++)
            	{
            		*pW++=0;
            	}
            }
        }

		for(pbTMap=SprETMap;pbTMap>=SprTTMap;pbTMap-=4) // 스후˚라이트의 묘화
		{
			TMap=pbTMap[0];						// 타이르맛후˚를 읽어들인다
			TMap|=pbTMap[1]<<8;					// 타이르맛후˚를 읽어들인다

			if(pbTMap[2]>0xF8)
            {
            	j=pbTMap[2]-0x100;
            }
            else
            {
            	j=pbTMap[2];
            }
			if(pbTMap[3]>0xF8)
            {
            	k=pbTMap[3]-0x100;
            }
            else
            {
            	k=pbTMap[3];
            }

			if(Line<j)
				continue;
			if(Line>=j+8)
				continue;
			if(224<=k)
				continue;

			i=k;
			pSWrBuf16=pSBuf+(i * 240);

			if(COLCTL&0x40)		// 16 color
			{
            	pbTData=IRAM+0x4000;
            	pbTData+=(TMap&SPR_TILE)<<5;
				if(TMap&SPR_VREV)
       			{
            		pbTData+=(7-Line+j)<<2;// 타이르테″타의 호˚인터를 설정
       			}
				else
				{
					pbTData+=(Line-j)<<2;// 타이르테″타의 호˚인터를 설정
				}
			}
			else			// 4 color
			{
            	pbTData=IRAM+0x2000;
            	pbTData+=(TMap&SPR_TILE)<<4;
				if(TMap&SPR_VREV)
       			{
            		pbTData+=(7-Line+j)<<1;// 타이르테″타의 호˚인터를 설정
       			}
				else
				{
					pbTData+=(Line-j)<<1;// 타이르테″타의 호˚인터를 설정
				}
			}

			if(COLCTL&0x20)						// Packed Mode
			{
				if(COLCTL&0x40)					// 16 Color
				{
					index[0]=(pbTData[0]&0xF0)>>4;
					index[1]=pbTData[0]&0x0F;
					index[2]=(pbTData[1]&0xF0)>>4;
					index[3]=pbTData[1]&0x0F;
					index[4]=(pbTData[2]&0xF0)>>4;
					index[5]=pbTData[2]&0x0F;
					index[6]=(pbTData[3]&0xF0)>>4;
					index[7]=pbTData[3]&0x0F;
				}
				else							// 4 Color
				{
					index[0]=(pbTData[0]&0xC0)>>6;
					index[1]=(pbTData[0]&0x30)>>4;
					index[2]=(pbTData[0]&0x0C)>>2;
					index[3]=pbTData[0]&0x03;
					index[4]=(pbTData[1]&0xC0)>>6;
					index[5]=(pbTData[1]&0x30)>>4;
					index[6]=(pbTData[1]&0x0C)>>2;
					index[7]=pbTData[1]&0x03;
				}
			}
			else
			{
				if(COLCTL&0x40)					// 16 Color
				{
					index[0] =(pbTData[0]&0x80)? 0x1:0;
					index[0]|=(pbTData[1]&0x80)? 0x2:0;
					index[0]|=(pbTData[2]&0x80)? 0x4:0;
					index[0]|=(pbTData[3]&0x80)? 0x8:0;
					index[1] =(pbTData[0]&0x40)? 0x1:0;
					index[1]|=(pbTData[1]&0x40)? 0x2:0;
					index[1]|=(pbTData[2]&0x40)? 0x4:0;
					index[1]|=(pbTData[3]&0x40)? 0x8:0;
					index[2] =(pbTData[0]&0x20)? 0x1:0;
					index[2]|=(pbTData[1]&0x20)? 0x2:0;
					index[2]|=(pbTData[2]&0x20)? 0x4:0;
					index[2]|=(pbTData[3]&0x20)? 0x8:0;
					index[3] =(pbTData[0]&0x10)? 0x1:0;
					index[3]|=(pbTData[1]&0x10)? 0x2:0;
					index[3]|=(pbTData[2]&0x10)? 0x4:0;
					index[3]|=(pbTData[3]&0x10)? 0x8:0;
					index[4] =(pbTData[0]&0x08)? 0x1:0;
					index[4]|=(pbTData[1]&0x08)? 0x2:0;
					index[4]|=(pbTData[2]&0x08)? 0x4:0;
					index[4]|=(pbTData[3]&0x08)? 0x8:0;
					index[5] =(pbTData[0]&0x04)? 0x1:0;
					index[5]|=(pbTData[1]&0x04)? 0x2:0;
					index[5]|=(pbTData[2]&0x04)? 0x4:0;
					index[5]|=(pbTData[3]&0x04)? 0x8:0;
					index[6] =(pbTData[0]&0x02)? 0x1:0;
					index[6]|=(pbTData[1]&0x02)? 0x2:0;
					index[6]|=(pbTData[2]&0x02)? 0x4:0;
					index[6]|=(pbTData[3]&0x02)? 0x8:0;
					index[7] =(pbTData[0]&0x01)? 0x1:0;
					index[7]|=(pbTData[1]&0x01)? 0x2:0;
					index[7]|=(pbTData[2]&0x01)? 0x4:0;
					index[7]|=(pbTData[3]&0x01)? 0x8:0;
				}
				else							// 4 Color
				{
					index[0] =(pbTData[0]&0x80)? 0x1:0;
					index[0]|=(pbTData[1]&0x80)? 0x2:0;
					index[1] =(pbTData[0]&0x40)? 0x1:0;
					index[1]|=(pbTData[1]&0x40)? 0x2:0;
					index[2] =(pbTData[0]&0x20)? 0x1:0;
					index[2]|=(pbTData[1]&0x20)? 0x2:0;
					index[3] =(pbTData[0]&0x10)? 0x1:0;
					index[3]|=(pbTData[1]&0x10)? 0x2:0;
					index[4] =(pbTData[0]&0x08)? 0x1:0;
					index[4]|=(pbTData[1]&0x08)? 0x2:0;
					index[5] =(pbTData[0]&0x04)? 0x1:0;
					index[5]|=(pbTData[1]&0x04)? 0x2:0;
					index[6] =(pbTData[0]&0x02)? 0x1:0;
					index[6]|=(pbTData[1]&0x02)? 0x2:0;
					index[7] =(pbTData[0]&0x01)? 0x1:0;
					index[7]|=(pbTData[1]&0x01)? 0x2:0;
				}
			}

            if(TMap&SPR_HREV)
           	{
				j=index[0];
				index[0]=index[7];
				index[7]=j;
				j=index[1];
				index[1]=index[6];
				index[6]=j;
				j=index[2];
				index[2]=index[5];
				index[5]=j;
				j=index[3];
				index[3]=index[4];
				index[4]=j;
			}

			pW=WBuf+8+k;
			pZ=ZBuf+k+8;
            for(i=0;i<8;i++, pZ++, pW++)
            {
            	if(DSPCTL&0x08)			// sprite window on
            	{
            		if(TMap&SPR_CLIP)
            		{
                		if(!*pW)
                		{
                			 pSWrBuf16 += 240;
							continue;
                		}
            		}
            		else
					{
						if(*pW)
						{
							 pSWrBuf16 += 240;
                   			continue;
						}
            		}
                }

				if((!index[i])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))
                {
                	 pSWrBuf16 += 240;
                    continue;
                }

				if((*pZ)&&(!(TMap&SPR_LAYR)))
                {
                	 pSWrBuf16 += 240;
                    continue;
                }

                pbPal=&Palette[((TMap&SPR_PAL)>>9)+8][index[i]];
#ifdef DRAW_DEBUG
				if(!(COLCTL&0x80))
                {
					pbPal=&Palette[((TMap&SPR_PAL)>>9)+8][index[i]+12];
                }
#endif
				*pSWrBuf16=*pbPal++; pSWrBuf16 += 240; 
			}
		}
	}
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
				*((uint16*)gtSurface[0].ptbuffer+i) = WSCBmp_horz[i];
		}
			break;
		case 1:
		case 2:
		{
			for (i=(320*240)-1;i>=0;--i)
				*((uint16*)gtSurface[0].ptbuffer+i) = 0;
		}
			break;
		case 3:
		{
			for (i = 0; i < (320*240); i++)
				*((uint16*)gtSurface[0].ptbuffer+i) = WSCBmp_vert[i];
		}
			break;
		case 4:
		{
			Cls(0);
			DrawWindow(44, 37, 228, 148, 0, COLOR_RED, COLOR_BLUE); // file selector window
			DrawWindow(5, 219,  305, 15,  0, COLOR_RED, COLOR_BLUE); // totals window
		}
			break;
		case 5:
		{
			Cls(0);
			DrawWindow(85, 0,  148, 228, 0, COLOR_RED, COLOR_BLUE); // file selector window
			DrawWindow(5, 225,  305, 12,  0, COLOR_RED, COLOR_BLUE); // totals window
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



