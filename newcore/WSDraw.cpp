//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "WSDraw.h"
#include "Config.h"
#include "WS.h"

//#define DRAW_DEBUG

//---------------------------------------------------------------------------
// DirectX 하″시″3 이상으로 확인 이 끝난 상태 하″시″2 에서도 동작 가능
//---------------------------------------------------------------------------
LPDIRECTDRAW lpDD=NULL;					// DirectDraw됫후″시″크트
LPDIRECTDRAWSURFACE lpDDSPrimary=NULL;	// 후˚라이마리서페스오후″시″크트
LPDIRECTDRAWSURFACE lpDDSBack0=NULL;	// 하″크서페스 0됫후″시″크트
LPDIRECTDRAWCLIPPER lpDDC=NULL;			// 크리하˚오프″시″크트
LPDIRECTDRAWPALETTE lpDDP=NULL;			// 하˚렛 오프″시″크트

HWND hGbWnd;							// 윈트″우한트″르
int DrawSize=DS_1;						// 묘화 사이스″
int DrawMode=0;							// 묘화 수평 수직 모트″

RECT GbMainRect;						// 메인윈트″우의 묘화 영역
POINT PosOrig;							// 윈트″우의 묘화 영역 원점
DWORD DispHeight, DispWidth;				// 테″스후˚레이 높이, 폭
int PixelDepth;							// 히˚쿠셀 당의 하″실수
DWORD RBitMask, GBitMask, BBitMask;		// RGB拒″트마스크
int SftR, SftG, SftB;						// 32 bitRGB로부터의 오른쪽 쉬프트수
long Pitch;								// 1旒인당의 히˚치
DWORD VBFreq;							// 수직 동기 주파수

byte ColTbl[0x210];						// 하˚렛트테″타(16+1후″락, 16쒸트, 2艱″실)
byte Palette[16+1][16][4];				// 하˚렛(16+1후″락, 16쒸트, 4艱″실)

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

//---------------------------------------------------------------------------
// RefreshLine 1旒인 묘화
// 인수 Line		:묘화 라인의 Y좌표(0∼143)
// 인수 lpSurface	:하″크서페스의 메모리호˚인터
// 반치없음
//
// GB의 HBlank시에 1旒인을 하″크서페스에 묘화 한다.
//---------------------------------------------------------------------------
void __fastcall RefreshLine(int Line, LPVOID lpSurface);
void __fastcall DrawErr(LPCSTR Msg);

//---------------------------------------------------------------------------
int __fastcall WsDrawCreate(HWND hWnd)
{
    HRESULT result;
	DDSURFACEDESC DDSD;

    hGbWnd=hWnd;

// DDraw의 생성
	result=DirectDrawCreate(NULL, &lpDD, NULL);
    if(result! =DD_OK)
    {
		DrawErr("DDraw를 작성 할 수 없습니다");
    	WsDrawRelease();
    	return -1;
    }

// 히˚쿠셀당의 하″실수를 설정(하˚렛의 생성과 할당)
    memset(&DDSD, 0, sizeof(DDSD));
	DDSD.dwSize=sizeof(DDSD);
	DDSD.dwFlags=DDSD_ALL;
	result=lpDD->GetDisplayMode(&DDSD);
    if (result! =DD_OK)
    {
		DrawErr("테″스후˚레이모트″를 취득할 수 없습니다");
    	WsDrawRelease();
    	return -1;
    }
	DispHeight=DDSD.dwHeight;
    DispWidth=DDSD.dwWidth;
    PixelDepth=DDSD.ddpfPixelFormat.dwRGBBitCount;
    RBitMask=DDSD.ddpfPixelFormat.dwRBitMask;
    GBitMask=DDSD.ddpfPixelFormat.dwGBitMask;
    BBitMask=DDSD.ddpfPixelFormat.dwBBitMask;
    DWORD tmp;
    int sft;
    for(tmp=RBitMask, sft=0;sft<32;sft++)
    {
    	if(tmp&0x80000000)
        {
        	SftR=sft;
            break;
        }
        tmp<<=1;
    }
    for(tmp=GBitMask, sft=0;sft<32;sft++)
    {
    	if(tmp&0x80000000)
        {
        	SftG=sft;
            break;
        }
        tmp<<=1;
    }
    for(tmp=BBitMask, sft=0;sft<32;sft++)
    {
    	if(tmp&0x80000000)
        {
        	SftB=sft;
            break;
        }
        tmp<<=1;
    }

// 협조 레헤″르의 설정
    if(DrawSize==DS_FULL)
    {
		result=lpDD->SetCooperativeLevel(hGbWnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN);
    	if(result! =DD_OK)
    	{
			DrawErr("협조 레헤″르 설정을 할 수 없습니다");
    		WsDrawRelease();
    		return -1;
    	}
        result=lpDD->SetDisplayMode(640,480, PixelDepth);
    	if(result! =DD_OK)
    	{
			DrawErr("테″스후˚레이모트″설정을 할 수 없습니다");
    		WsDrawRelease();
    		return -1;
    	}
    }
    else
    {
		result=lpDD->SetCooperativeLevel(hGbWnd, DDSCL_NORMAL);
    	if(result! =DD_OK)
    	{
			DrawErr("협조 레헤″르 설정을 할 수 없습니다");
    		WsDrawRelease();
    		return -1;
    	}
    }
	lpDD->GetMonitorFrequency(&VBFreq);

// 후˚라이마리서페스의 생성
    memset(&DDSD, 0, sizeof(DDSD));
	DDSD.dwSize=sizeof(DDSD);
	DDSD.dwFlags=DDSD_CAPS;
	DDSD.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE;
	result=lpDD->CreateSurface(&DDSD, &lpDDSPrimary, NULL);
    if (result! =DD_OK)
    {
		DrawErr("후˚라이마리서페스를 작성 할 수 없습니다");
    	WsDrawRelease();
    	return -1;
    }

// 하″크서페스의 생성
    memset(&DDSD, 0, sizeof(DDSD));
	DDSD.dwSize=sizeof(DDSD);
	DDSD.dwFlags=DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH;
	DDSD.ddsCaps.dwCaps=DDSCAPS_OFFSCREENPLAIN|DDSCAPS_SYSTEMMEMORY;
	if(DrawMode)
	{
		DDSD.dwWidth=144;
		DDSD.dwHeight=224;
	}
	else
	{
		DDSD.dwWidth=224;
		DDSD.dwHeight=144;
	}
	result=lpDD->CreateSurface(&DDSD, &lpDDSBack0, NULL);
    if(result! =DD_OK)
    {
		DrawErr("메인 영역하″크서페스를 작성 할 수 없습니다");
    	WsDrawRelease();
    	return -1;
    }
    memset(&DDSD, 0, sizeof(DDSD));
	DDSD.dwSize=sizeof(DDSD);
	DDSD.dwFlags=DDSD_ALL;
	result=lpDDSBack0->GetSurfaceDesc(&DDSD);
    if (result! =DD_OK)
    {
		DrawErr("테″스후˚레이모트″를 취득할 수 없습니다");
    	WsDrawRelease();
    	return -1;
    }
    Pitch=DDSD.lPitch;

// 크리하˚-의 생성과 할당
	if(! (DrawSize==DS_FULL))
    {
		result=lpDD->CreateClipper(0, &lpDDC, NULL);
    	if(result! =DD_OK)
    	{
			DrawErr("크리하˚-를 작성 할 수 없습니다");
    		WsDrawRelease();
    		return -1;
    	}
		result=lpDDC->SetHWnd(0, hGbWnd);
    	if(result! =DD_OK)
    	{
			DrawErr("크리하˚-를 윈트″우에 설정 할 수 없습니다");
    		WsDrawRelease();
    		return -1;
    	}
		result=lpDDSPrimary->SetClipper(lpDDC);
    	if(result! =DD_OK)
    	{
			DrawErr("크리하˚-를 후˚라이마리서페스에 설정 할 수 없습니다");
    		WsDrawRelease();
    		return -1;
    	}
    }

    if(PixelDepth==16)
    {
         PixelDepth=2;
    }
    else if(PixelDepth<=24)
    {
         PixelDepth=3;
    }
    else if(PixelDepth<=32)
    {
         PixelDepth=4;
    }
    else
    {
		DrawErr("16拒″트색이상 32拒″트색이하로 해 주세요");
    	WsDrawRelease();
    	return -1;
    }

// 묘화 영역을 설정
	HDC hdc;
    RECT rect;

    hdc=GetDC(hGbWnd);
    GetDCOrgEx(hdc, &PosOrig);
    ReleaseDC(hGbWnd, hdc);
    GetClientRect(hGbWnd, &rect);

    GbMainRect.left=PosOrig.x+rect.left;
    GbMainRect.top=PosOrig.y+rect.top;
    if(DrawSize==DS_FULL)
    {
		if(DrawMode)
		{
    		GbMainRect.left=(640-144*2)/2;
    		GbMainRect.top=(480-224*2)/2;
        	GbMainRect.right=GbMainRect.left+144*2;
        	GbMainRect.bottom=GbMainRect.top+224*2;
		}
		else
		{
    		GbMainRect.left=(640-224*2)/2;
    		GbMainRect.top=(480-144*2)/2;
        	GbMainRect.right=GbMainRect.left+224*2;
        	GbMainRect.bottom=GbMainRect.top+144*2;
		}
    }
    else
    {
		if(DrawMode)
		{
        	GbMainRect.right=GbMainRect.left+144*DrawSize;
        	GbMainRect.bottom=GbMainRect.top+224*DrawSize;
		}
		else
		{
        	GbMainRect.right=GbMainRect.left+224*DrawSize;
        	GbMainRect.bottom=GbMainRect.top+144*DrawSize;
		}
    }

    WsDrawClear();
    return 0;
}

//---------------------------------------------------------------------------
void __fastcall WsDrawRelease(void)
{
	if(lpDD! =NULL)
    {
        if(lpDDSPrimary! =NULL)
        {
	        if(lpDDP! =NULL)
	        {
            	lpDDP->Release();
            	lpDDP=NULL;
            }
	        if(lpDDC! =NULL)
	        {
            	lpDDC->Release();
            	lpDDC=NULL;
            }
            lpDDSPrimary->Release();
            lpDDSPrimary=NULL;
        }
        if(lpDDSBack0! =NULL)
        {
            lpDDSBack0->Release();
            lpDDSBack0=NULL;
        }
    	if(DrawSize==DS_FULL)
    	{
        	lpDD->RestoreDisplayMode();
        }
        lpDD->Release();
        lpDD=NULL;
    }
}

//---------------------------------------------------------------------------
int __fastcall WsDrawLine(int Line)
{
    HRESULT result;
	DDSURFACEDESC DDSD;
    RECT rect;
    byte buf[(224+16)*4];
    int i, j, pitch;
	byte *pdest, *psrc;

    if(lpDDSBack0==NULL)
    {
    	if(Verbose)
		DrawErr("서페스가 없습니다");
    	return -1;
    }

    RefreshLine(Line, buf);

    memset(&DDSD, 0, sizeof(DDSD));
	DDSD.dwSize=sizeof(DDSD);

	if(DrawMode)
	{
	    rect.left=Line;
	    rect.top=0;
	    rect.right=Line;
	    rect.bottom=224;

    	result=lpDDSBack0->Lock(&rect, &DDSD, DDLOCK_WAIT, NULL);
    	if(result! =DD_OK)
    	{
    		if(Verbose)
    		DrawErr("하″크서페스를 락 할 수 없지 않습니다");
        	return -1;
    	}

		psrc=buf+(PixelDepth<<3);
		pitch=DDSD.lPitch+PixelDepth;
		pdest=(byte*) DDSD.lpSurface+223*DDSD.lPitch;
        for(i=0;i<224;i++)
		{
			for(j=0;j<PixelDepth;j++)
			{
				*pdest++=*psrc++;
			}
			pdest-=pitch;
		}

    	result=lpDDSBack0->Unlock(DDSD.lpSurface);
    	if(result! =DD_OK)
    	{
    		if(Verbose)
    		DrawErr("하″크서페스를 언로크 할 수 없지 않습니다");
        	return -1;
    	}
	}
	else
	{
	    rect.left=0;
	    rect.top=Line;
	    rect.right=224;
	    rect.bottom=Line;

    	result=lpDDSBack0->Lock(&rect, &DDSD, DDLOCK_WAIT, NULL);
    	if(result! =DD_OK)
    	{
    		if(Verbose)
    		DrawErr("하″크서페스를 락 할 수 없지 않습니다");
        	return -1;
    	}

    	memcpy(DDSD.lpSurface, buf+(8*PixelDepth), 224*PixelDepth);

    	result=lpDDSBack0->Unlock(DDSD.lpSurface);
    	if(result! =DD_OK)
    	{
    		if(Verbose)
    		DrawErr("하″크서페스를 언로크 할 수 없지 않습니다");
        	return -1;
    	}
	}

    return 0;
}

//---------------------------------------------------------------------------
int __fastcall WsDrawFlip(void)
{
    HRESULT result;
    RECT rect, GbRect;

    if(lpDDSBack0==NULL)
    {
    	if(Verbose)
		DrawErr("하″크서페스가 없습니다");
    	return -1;
    }

    if(lpDDSPrimary==NULL)
    {
    	if(Verbose)
		DrawErr("후˚라이마리서페스가 없습니다");
    	return -1;
    }

	HDC hdc;
    POINT dwPos;

	hdc=GetDC(hGbWnd);
	GetDCOrgEx(hdc, &dwPos);
	ReleaseDC(hGbWnd, hdc);

    if(dwPos.x! =PosOrig.x)
    {
		GbMainRect.left+=dwPos.x-PosOrig.x;
		GbMainRect.right+=dwPos.x-PosOrig.x;
        PosOrig.x=dwPos.x;
    }
    if(dwPos.y! =PosOrig.y)
    {
		GbMainRect.top+=dwPos.y-PosOrig.y;
		GbMainRect.bottom+=dwPos.y-PosOrig.y;
        PosOrig.y=dwPos.y;
    }

    GbRect.left=GbMainRect.left;
    GbRect.top=GbMainRect.top;
    GbRect.right=GbMainRect.right;
    GbRect.bottom=GbMainRect.bottom;

	if(DrawMode)
	{
		rect.left=0;
		rect.top=0;
		rect.right=144;
		rect.bottom=224;
	}
	else
	{
		rect.left=0;
		rect.top=0;
		rect.right=224;
		rect.bottom=144;
	}

    result=lpDDSPrimary->Blt(&GbRect, lpDDSBack0, &rect, DDBLT_WAIT, NULL);
    if(result! =DD_OK)
    {
    	if(Verbose)
    	DrawErr("후˚라이마리서페스에 히″르트 할 수 없지 않습니다");
    	return -1;
    }
    return 0;
}

//---------------------------------------------------------------------------
void __fastcall SetPalette(int Index, byte PalData)
{
	int Pal, i, j, k;
    DWORD R, G, B, PF;

    if(lpDDSBack0==NULL)
    {
    	if(Verbose)
		DrawErr("서페스가 없습니다");
    	return;
    }

	ColTbl[Index]=PalData;
    i=Index&0x3FE;
	Pal=(ColTbl[i+1]<<8) |ColTbl[i];

	R=(Pal&0x00000F00)<<20;
	G=(Pal&0x000000F0)<<24;
	B=(Pal&0x0000000F)<<28;

	R|=R>>4;
	G|=G>>4;
	B|=B>>4;

	R>>=SftR;
	G>>=SftG;
	B>>=SftB;

	PF=(R&RBitMask)|(G&GBitMask)|(B&BBitMask);

	i=Index>>5;
	j=(Index>>1) &0x0F;
	for(k=0;k<PixelDepth;k++)
 	{
		Palette[i][j][k]=(byte)(PF&0xFF);
		PF>>=8;
	}

#ifdef DRAW_DEBUG
    if((Index<0x200)&&! (COLCTL&0x80))
    {
		PF=(R&RBitMask);
        j+=4;
		for(k=0;k<PixelDepth;k++)
 		{
			Palette[i][j][k]=(byte)(PF&0xFF);
			PF>>=8;
		}

		PF=(G&GBitMask);
        j+=4;
		for(k=0;k<PixelDepth;k++)
 		{
			Palette[i][j][k]=(byte)(PF&0xFF);
			PF>>=8;
		}

		PF=(B&BBitMask);
        j+=4;
		for(k=0;k<PixelDepth;k++)
 		{
			Palette[i][j][k]=(byte)(PF&0xFF);
			PF>>=8;
		}
    }
#endif
    return;
}

void __fastcall RefreshLine(int Line, LPVOID lpSurface)
{
	byte *pSBuf;			// 서페스하″파의 (0, Y) 좌표호˚인터(8艱″실 포함하지 않고)
	byte *pSWrBuf;			// 서페스하″파의 라이트호˚인터

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

	byte *pbPal;			// 하˚렛호˚인터
	byte *pbRdPal;			// 하˚렛호˚인터
	byte defpal[]={0,0,0,0};

	pSBuf=(byte *) lpSurface+(PixelDepth<<3);
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
		if(PixelDepth==2)
		{
			*pSWrBuf++=pbPal[0];
			*pSWrBuf++=pbPal[1];
		}
		else if(PixelDepth==3)
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
	}

	if(! (LCDSLP&0x01)) return;

 	if(DSPCTL&0x01)
	{
		OffsetX=SCR1X&0x07;						// X좌표 오프셋을 설정
		pSWrBuf=pSBuf-OffsetX*PixelDepth;		// 서페스하″파의 기입호˚인터를 X됫후셋트
		i=Line+SCR1Y;
		OffsetY=(i&0x07);						// Y좌표 오프셋을 설정

		pbTMap=Scr1TMap+((i&0xF8)<<3);			// 타이르맛후˚의 (0, Y) 좌표호˚인터
		TMapX=(SCR1X&0xF8)>>2;					// 타이르맛후˚의 X값을 설정
		TMapXEnd=((SCR1X+224+7)>>2) &0xFFE;		// 하″크크″라운트″영역 종료의 X값을 설정

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

			if(PixelDepth==2)
			{
				if((! index[0])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[0]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[1])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[1]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[2])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[2]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[3])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[3]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[4])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[4]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[5])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[5]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[6])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[6]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[7])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[7]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
			}
			else if(PixelDepth==3)
			{
				if((! index[0])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[0]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[1])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[1]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[2])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[2]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[3])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[3]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[4])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[4]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[5])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[5]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[6])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[6]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[7])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=3;
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
				if((! index[0])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[0]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[1])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[1]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[2])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[2]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[3])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[3]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[4])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[4]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[5])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[5]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[6])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[6]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				if((! index[7])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800))))) pSWrBuf+=4;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[7]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
			}
		}
	}

	memset(&ZBuf, 0, sizeof(ZBuf));

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

		OffsetX=SCR2X&0x07;						// X좌표 오프셋을 설정
		pSWrBuf=pSBuf-OffsetX*PixelDepth;		// 서페스하″파의 기입호˚인터를 X됫후셋트
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

			if(PixelDepth==2)
			{
				if(((! index[0])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[0]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((! index[1])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[1]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((! index[2])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[2]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((! index[3])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[3]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((! index[4])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[4]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((! index[5])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[5]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((! index[6])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[6]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((! index[7])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=2;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[7]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
			}
			else if(PixelDepth==3)
			{
				if(((! index[0])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[0]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((! index[1])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[1]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((! index[2])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[2]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((! index[3])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[3]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((! index[4])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[4]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((! index[5])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[5]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((! index[6])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=3;
				else
				{
					pbPal=&Palette[(TMap&MAP_PAL)>>9][index[6]][0];
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
					*pZ=1;
				}
				pW++;pZ++;
				if(((! index[7])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=3;
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
				if(((! index[0])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=4;
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
				if(((! index[1])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=4;
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
				if(((! index[2])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=4;
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
				if(((! index[3])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=4;
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
				if(((! index[4])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=4;
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
				if(((! index[5])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=4;
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
				if(((! index[6])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=4;
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
				if(((! index[7])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))||(*pW)) pSWrBuf+=4;
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
		}
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
			pSWrBuf=pSBuf+i*PixelDepth;

			if(COLCTL&0x40)
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
			else
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
            	if(DSPCTL&0x08)
            	{
            		if(TMap&SPR_CLIP)
            		{
                		if(! *pW)
                		{
                			pSWrBuf+=PixelDepth;
							continue;
                		}
            		}
            		else
					{
						if(*pW)
						{
							pSWrBuf+=PixelDepth;
                   			continue;
						}
            		}
                }

				if((! index[i])&&(! (! (COLCTL&0x40)&&(! (TMap&0x0800)))))
                {
                	pSWrBuf+=PixelDepth;
                    continue;
                }

				if((*pZ)&&(! (TMap&SPR_LAYR)))
                {
                	pSWrBuf+=PixelDepth;
                    continue;
                }

                pbPal=&Palette[((TMap&SPR_PAL)>>9)+8][index[i]][0];
#ifdef DRAW_DEBUG
				if(! (COLCTL&0x80))
                {
					pbPal=&Palette[((TMap&SPR_PAL)>>9)+8][index[i]+12][0];
                }
#endif
				if(PixelDepth==2)
				{
					*pSWrBuf++=*pbPal++;
					*pSWrBuf++=*pbPal++;
				}
				else if(PixelDepth==3)
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

			}
		}
	}

}

//---------------------------------------------------------------------------
void __fastcall WsDrawClear(void)
{
    HRESULT result;
    DDBLTFX DDBFX;

    memset(&DDBFX, 0, sizeof(DDBFX));
	DDBFX.dwSize=sizeof(DDBFX);
    DDBFX.dwFillColor=0;
    if(lpDDSBack0==NULL)
    {
    	return;
    }

	result=lpDDSBack0->Blt(NULL, NULL, NULL, DDBLT_WAIT|DDBLT_COLORFILL, &DDBFX);
    if(result! =DD_OK)
    {
    	if(Verbose)
    	DrawErr("하″크서페스에 히″르트 할 수 없지 않습니다");
    	return;
    }

	result=lpDDSPrimary->Blt(NULL, NULL, NULL, DDBLT_WAIT|DDBLT_COLORFILL, &DDBFX);
    if(result! =DD_OK)
    {
    	if(Verbose)
    	DrawErr("후˚라이마리서페스에 히″르트 할 수 없지 않습니다");
    	return;
    }

}

//---------------------------------------------------------------------------
int __fastcall SetDrawMode(int Size, int Mode)
{
	if((DrawSize! =Size)||(DrawMode! =Mode))
    {
	WsDrawRelease();
    DrawSize=Size;
    DrawMode=Mode;
    WsDrawCreate(hGbWnd);
    }
    return 0;
}

//---------------------------------------------------------------------------
void __fastcall DrawErr(LPCSTR Msg)
{
	MessageBox(NULL, Msg,"WsDraw닝라", MB_ICONEXCLAMATION|MB_OK);
}

//---------------------------------------------------------------------------
int MaxLine=0;
int DrawLine=0;
int VSW=0;

int __fastcall GetLine(void)
{
	DWORD i;
	static int j;

	if(lpDD->GetScanLine(&i) ==DD_OK)
    {
		DrawLine=(int) i;
    }
    if(DrawLine<j)
    {
    	j-=MaxLine;
    }
    VSW+=DrawLine-j;
    j=DrawLine;

    if(DrawLine>MaxLine)
    {
    	MaxLine=DrawLine;
    }
    return DrawLine;
}

//---------------------------------------------------------------------------
extern "C" int __fastcall VSWStart(void)
{
	int i;
	GetLine();
    i=VSW;
	VSW=0;
    return i;
}

//---------------------------------------------------------------------------








