//---------------------------------------------------------------------------
#include "types.h"

#include <gpstdlib.h>
#include <gpstdio.h>
#include <gpgraphic.h>
#include <gpmain.h>
#include <gpfont.h>

#include "../OKF/global.h"
#include "../OKF/clock.h"
#include "../OKF/graphics16.h"
#include "../OKF/okf.h"

#include "WSDraw.h"
#include "WS.h"

#include "../render.h"

//#define DRAW_DEBUG

#define RGB555(R,G,B) ((((int)(R))<<11)|(((int)(G))<<6)|(((int)(B))<<1))

int DrawMode=-1;
int PixelDepth;

byte ColTbl[0x210];
byte Palette[16+1][16][4];

uint16 screenbuffer[(224+16)*144];

#define MAP_TILE 0x01FF
#define MAP_PAL  0x1E00
#define MAP_BANK 0x2000
#define MAP_HREV 0x4000
#define MAP_VREV 0x8000
byte *Scr1TMap;							// ��ũ�� 1�� Ÿ�̸����Ģ�ȣ������
byte *Scr2TMap;							// ��ũ�� 2�� Ÿ�̸����Ģ�ȣ������

#define SPR_TILE 0x01FF
#define SPR_PAL  0x0E00
#define SPR_CLIP 0x1000
#define SPR_LAYR 0x2000
#define SPR_HREV 0x4000
#define SPR_VREV 0x8000
byte *SprTTMap;							// ���Ģ�����Ʈ�� ���� Ÿ�̸����Ģ�
byte *SprETMap;							// ���Ģ�����Ʈ�� ���� Ÿ�̸����Ģ�
byte SprTMap[512];						// ���Ģ�����Ʈ�� Ÿ�̸����Ģ�

byte *BGndTPal=NULL;					// �ϡ�ũũ�ȶ��Ʈ���� Ÿ���Ϣ���ȣ������
byte *WndTPal=NULL;						// ��Ʈ�ȿ��� Ÿ���Ϣ���ȣ������

void  RefreshLine(int Line, void* lpSurface);
void  DrawErr(char* Msg);

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

video_mode video_modes[4] =
{
	horz_render_normal,		48,	41,	0,
	horz_render_hstretch,	0,	41,	6,
	horz_render_hvstretch,	0,	12,	6,
	vert_render_normal,		88,	7,	0
};

//---------------------------------------------------------------------------
int  WsDrawCreate()
{
    int result;

	video_update = video_modes[DrawMode].video_update;
	video_x = video_modes[DrawMode].video_x;
	video_y = video_modes[DrawMode].video_y;
	scroll_x = video_modes[DrawMode].scroll_x;
		
    PixelDepth=2;

    WsDrawClear();
    return 0;
}

//---------------------------------------------------------------------------
int  WsDrawLine(int Line)
{
    int result;

    RefreshLine(Line, &screenbuffer[(224+16)*Line]);

    return 0;
}

//---------------------------------------------------------------------------
int  WsDrawFlip(void)
{
	video_update((byte*)screenbuffer+(scroll_x*2), gtSurface[0].ptbuffer+(240*2*video_x)-(video_y*2));

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

	Palette[i][j][0]=(byte)(PF&0xFF);
	PF = PF >> 8;
	Palette[i][j][1]=(byte)(PF&0xFF);

    return;
}

void  RefreshLine(int Line, void* buffer)
{
	byte *pSBuf;			// ���佺�ϡ����� (0, Y) ��ǥȣ������(8�ޡȽ� �������� �ʰ�)
	byte *pSWrBuf;			// ���佺�ϡ����� ����Ʈȣ������

	int *pZ;				// ZBuf�� ����Ʈȣ������
	int ZBuf[0x100];		// ZϽ��ũ
	int *pW;				// WBuf�� ����Ʈȣ������
	int WBuf[0x100];		// ��Ʈ�ȿ츶��ũ

	int OffsetX;			// X��ǥ ������(0��7)
	int OffsetY;			// Y��ǥ ������(0��E)

	byte *pbTMap;			// �� �պ�� Ÿ�̸����Ģ��� ȣ������
	int TMap;				// Ÿ�̸����Ģ�
	int TMapX;				// Ÿ�̸����Ģ��� �޽� X��
	int TMapXEnd;			// Ÿ�̸����Ģ��� �ϡ�ũũ�ȶ��Ʈ�ȿ��� ������ �޽� X��

	byte *pbTData;			// �� �պ�� Ÿ�̸��ס�Ÿ�� ȣ������
	int TData;				// Ÿ�̸��ס�Ÿ

    int i, j, k, index[8];

	byte *pbPal;			// �Ϣ���ȣ������
	byte *pbRdPal;			// �Ϣ���ȣ������
	byte defpal[]={0,0,0,0};

	pSBuf=(byte *) buffer;
	pSWrBuf=pSBuf;
	if(LCDSLP&0x01)
	{
		if(COLCTL&0xE0)
		{
			pbPal=&Palette[(BORDER&0xF0)>>4][BORDER&0x0F][0];
		}
		else
		{
			pbPal=&Palette[16][BORDER&0x07][0];
		}
	}
	else
	{
		pbPal=defpal;
	}
	for(i=0;i<224;i++)
	{
//		if(PixelDepth==2)
		{
			*pSWrBuf++=pbPal[0];
			*pSWrBuf++=pbPal[1];
		}
/*		else if(PixelDepth==3)
		{
			*pSWrBuf++=pbPal[0];
			*pSWrBuf++=pbPal[1];
			*pSWrBuf++=pbPal[2];
		}
		else if(PixelDepth==4)
		{
			*pSWrBuf++=pbPal[0];
			*pSWrBuf++=pbPal[1];
			*pSWrBuf++=pbPal[2];
			*pSWrBuf++=pbPal[3];
		}
*/	}

	if(!(LCDSLP&0x01)) return;

 	if(DSPCTL&0x01)
	{
		OffsetX=SCR1X&0x07;						// X��ǥ �������� ����
		pSWrBuf=pSBuf-OffsetX*2; //PixelDepth;		// ���佺�ϡ����� ����ȣ�����͸� X���ļ�Ʈ
		i=Line+SCR1Y;
		OffsetY=(i&0x07);						// Y��ǥ �������� ����

		pbTMap=Scr1TMap+((i&0xF8)<<3);			// Ÿ�̸����Ģ��� (0, Y) ��ǥȣ������
		TMapX=(SCR1X&0xF8)>>2;					// Ÿ�̸����Ģ��� X���� ����
		TMapXEnd=((SCR1X+224+7)>>2) &0xFFE;		// �ϡ�ũũ�ȶ��Ʈ�ȿ��� ������ X���� ����

		for(;TMapX<TMapXEnd;)					// �ϡ�ũũ�ȶ��Ʈ�ȿ����� ��ȭ
		{
			TMap=*(pbTMap+(TMapX++&0x3F));		// Ÿ�̸����Ģ��� �о���δ�
			TMap|=*(pbTMap+(TMapX++&0x3F))<<8;	// Ÿ�̸����Ģ��� �о���δ�

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
            		pbTData+=(7-OffsetY)<<2;// Ÿ�̸��ס�Ÿ�� ȣ�����͸� ����
       			}
				else
				{
					pbTData+=OffsetY<<2;// Ÿ�̸��ס�Ÿ�� ȣ�����͸� ����
				}
			}
			else
			{
            	pbTData=IRAM+0x2000;
            	pbTData+=(TMap&MAP_TILE)<<4;
				if(TMap&MAP_VREV)
       			{
            		pbTData+=(7-OffsetY)<<1;// Ÿ�̸��ס�Ÿ�� ȣ�����͸� ����
       			}
				else
				{
					pbTData+=OffsetY<<1;// Ÿ�̸��ס�Ÿ�� ȣ�����͸� ����
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

//			if(PixelDepth==2)
			{
				if((!index[0])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[0]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[1])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[1]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[2])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[2]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[3])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[3]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[4])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[4]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[5])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[5]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[6])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[6]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[7])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[7]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
			}
/*			else if(PixelDepth==3)
			{
				if((!index[0])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[0]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[1])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[1]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[2])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[2]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[3])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[3]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[4])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[4]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[5])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[5]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[6])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[6]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[7])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[7]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
			}
			else if(PixelDepth==4)
			{
				if((!index[0])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[0]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[1])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[1]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[2])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[2]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[3])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[3]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[4])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[4]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[5])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[5]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[6])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[6]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((!index[7])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800))))) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[7]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
			}
	*/	}
	}

	GPMEMSET(&ZBuf, 0, sizeof(ZBuf));

	if(DSPCTL&0x02)
	{
		if((DSPCTL&0x30) ==0x20)
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
		else if((DSPCTL&0x30) ==0x30)
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
		else
		{
			for(i=0, pW=WBuf+8;i<0x100;i++)
			{
				*pW++=0;
			}
		}

		OffsetX=SCR2X&0x07;						// X��ǥ �������� ����
		pSWrBuf=pSBuf-OffsetX*2; //PixelDepth;		// ���佺�ϡ����� ����ȣ�����͸� X���ļ�Ʈ
		i=Line+SCR2Y;
		OffsetY=(i&0x07);						// Y��ǥ �������� ����

		pbTMap=Scr2TMap+((i&0xF8)<<3);			// Ÿ�̸����Ģ��� (0, Y) ��ǥȣ������
		TMapX=(SCR2X&0xF8)>>2;					// Ÿ�̸����Ģ��� X���� ����
		TMapXEnd=((SCR2X+224+7)>>2) &0xFFE;				// �ϡ�ũũ�ȶ��Ʈ�ȿ��� ������ X���� ����

		pW=WBuf+8-OffsetX;
		pZ=ZBuf+8-OffsetX;
        
		for(;TMapX<TMapXEnd;)					// �ϡ�ũũ�ȶ��Ʈ�ȿ����� ��ȭ
		{
			TMap=*(pbTMap+(TMapX++&0x3F));		// Ÿ�̸����Ģ��� �о���δ�
			TMap|=*(pbTMap+(TMapX++&0x3F))<<8;	// Ÿ�̸����Ģ��� �о���δ�

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
            		pbTData+=(7-OffsetY)<<2;// Ÿ�̸��ס�Ÿ�� ȣ�����͸� ����
       			}
				else
				{
					pbTData+=OffsetY<<2;// Ÿ�̸��ס�Ÿ�� ȣ�����͸� ����
				}
			}
			else
			{
            	pbTData=IRAM+0x2000;
            	pbTData+=(TMap&MAP_TILE)<<4;
				if(TMap&MAP_VREV)
       			{
            		pbTData+=(7-OffsetY)<<1;// Ÿ�̸��ס�Ÿ�� ȣ�����͸� ����
       			}
				else
				{
					pbTData+=OffsetY<<1;// Ÿ�̸��ס�Ÿ�� ȣ�����͸� ����
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

//			if(PixelDepth==2)
			{
				if(((!index[0])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[0]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[1])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[1]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[2])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[2]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[3])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[3]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[4])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[4]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[5])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[5]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[6])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[6]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[7])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[7]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
			}
/*			else if(PixelDepth==3)
			{
				if(((!index[0])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[0]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[1])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[1]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[2])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[2]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[3])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[3]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[4])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[4]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[5])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[5]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[6])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[6]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[7])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[7]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
			}
			else if(PixelDepth==4)
			{
				if(((!index[0])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[0]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[1])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[1]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[2])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[2]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[3])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[3]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[4])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[4]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[5])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[5]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[6])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[6]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((!index[7])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))||(*pW)) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[7]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
			}
	*/	}
	}

	if(DSPCTL&0x04)
	{
		if(DSPCTL&0x08)
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

		for(pbTMap=SprETMap;pbTMap>=SprTTMap;pbTMap-=4) // ���Ģ�����Ʈ�� ��ȭ
		{
			TMap=pbTMap[0];						// Ÿ�̸����Ģ��� �о���δ�
			TMap|=pbTMap[1]<<8;					// Ÿ�̸����Ģ��� �о���δ�

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
			pSWrBuf=pSBuf+i*2; //PixelDepth;

			if(COLCTL&0x40)
			{
            	pbTData=IRAM+0x4000;
            	pbTData+=(TMap&SPR_TILE)<<5;
				if(TMap&SPR_VREV)
       			{
            		pbTData+=(7-Line+j)<<2;// Ÿ�̸��ס�Ÿ�� ȣ�����͸� ����
       			}
				else
				{
					pbTData+=(Line-j)<<2;// Ÿ�̸��ס�Ÿ�� ȣ�����͸� ����
				}
			}
			else
			{
            	pbTData=IRAM+0x2000;
            	pbTData+=(TMap&SPR_TILE)<<4;
				if(TMap&SPR_VREV)
       			{
            		pbTData+=(7-Line+j)<<1;// Ÿ�̸��ס�Ÿ�� ȣ�����͸� ����
       			}
				else
				{
					pbTData+=(Line-j)<<1;// Ÿ�̸��ס�Ÿ�� ȣ�����͸� ����
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
            	if(DSPCTL&0x08)
            	{
            		if(TMap&SPR_CLIP)
            		{
                		if(!*pW)
                		{
                			pSWrBuf+=2; //PixelDepth;
							continue;
                		}
            		}
            		else
					{
						if(*pW)
						{
							pSWrBuf+=2; //PixelDepth;
                   			continue;
						}
            		}
                }

				if((!index[i])&&(!(!(COLCTL&0x40)&&(!(TMap&0x0800)))))
                {
                	pSWrBuf+=2; //PixelDepth;
                    continue;
                }

				if((*pZ)&&(!(TMap&SPR_LAYR)))
                {
                	pSWrBuf+=2; //PixelDepth;
                    continue;
                }

                pbPal=&Palette[((TMap&SPR_PAL)>>9)+8][index[i]][0];
#ifdef DRAW_DEBUG
				if(!(COLCTL&0x80))
                {
					pbPal=&Palette[((TMap&SPR_PAL)>>9)+8][index[i]+12][0];
                }
#endif
//				if(PixelDepth==2)
				{
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
/*				else if(PixelDepth==3)
				{
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				else if(PixelDepth==4)
				{
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
*/
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

//---------------------------------------------------------------------------
void  DrawErr(char* Msg)
{
//	MessageBox(NULL, Msg,"WsDraw�׶�", MB_ICONEXCLAMATION|MB_OK);
}




