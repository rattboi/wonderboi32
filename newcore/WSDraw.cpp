//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "WSDraw.h"
#include "Config.h"
#include "WS.h"

//#define DRAW_DEBUG

//---------------------------------------------------------------------------
// DirectX �ϡȽá�3 �̻����� Ȯ�� �� ���� ���� �ϡȽá�2 ������ ���� ����
//---------------------------------------------------------------------------
LPDIRECTDRAW lpDD=NULL;					// DirectDraw���ġȽá�ũƮ
LPDIRECTDRAWSURFACE lpDDSPrimary=NULL;	// �Ģ����̸������佺���ġȽá�ũƮ
LPDIRECTDRAWSURFACE lpDDSBack0=NULL;	// �ϡ�ũ���佺 0���ġȽá�ũƮ
LPDIRECTDRAWCLIPPER lpDDC=NULL;			// ũ���Ϣ������Ƚá�ũƮ
LPDIRECTDRAWPALETTE lpDDP=NULL;			// �Ϣ��� �����Ƚá�ũƮ

HWND hGbWnd;							// ��Ʈ�ȿ���Ʈ�ȸ�
int DrawSize=DS_1;						// ��ȭ ���̽���
int DrawMode=0;							// ��ȭ ���� ���� ��Ʈ��

RECT GbMainRect;						// ������Ʈ�ȿ��� ��ȭ ����
POINT PosOrig;							// ��Ʈ�ȿ��� ��ȭ ���� ����
DWORD DispHeight, DispWidth;				// �סȽ��Ģ����� ����, ��
int PixelDepth;							// ������ ���� �ϡȽǼ�
DWORD RBitMask, GBitMask, BBitMask;		// RGB�ޡ�Ʈ����ũ
int SftR, SftG, SftB;						// 32 bitRGB�κ����� ������ ����Ʈ��
long Pitch;								// 1ײ�δ��� ����ġ
DWORD VBFreq;							// ���� ���� ���ļ�

byte ColTbl[0x210];						// �Ϣ���Ʈ�ס�Ÿ(16+1�ġȶ�, 16��Ʈ, 2�ޡȽ�)
byte Palette[16+1][16][4];				// �Ϣ���(16+1�ġȶ�, 16��Ʈ, 4�ޡȽ�)

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

//---------------------------------------------------------------------------
// RefreshLine 1ײ�� ��ȭ
// �μ� Line		:��ȭ ������ Y��ǥ(0��143)
// �μ� lpSurface	:�ϡ�ũ���佺�� �޸�ȣ������
// ��ġ����
//
// GB�� HBlank�ÿ� 1ײ���� �ϡ�ũ���佺�� ��ȭ �Ѵ�.
//---------------------------------------------------------------------------
void __fastcall RefreshLine(int Line, LPVOID lpSurface);
void __fastcall DrawErr(LPCSTR Msg);

//---------------------------------------------------------------------------
int __fastcall WsDrawCreate(HWND hWnd)
{
    HRESULT result;
	DDSURFACEDESC DDSD;

    hGbWnd=hWnd;

// DDraw�� ����
	result=DirectDrawCreate(NULL, &lpDD, NULL);
    if(result! =DD_OK)
    {
		DrawErr("DDraw�� �ۼ� �� �� �����ϴ�");
    	WsDrawRelease();
    	return -1;
    }

// ���������� �ϡȽǼ��� ����(�Ϣ����� ������ �Ҵ�)
    memset(&DDSD, 0, sizeof(DDSD));
	DDSD.dwSize=sizeof(DDSD);
	DDSD.dwFlags=DDSD_ALL;
	result=lpDD->GetDisplayMode(&DDSD);
    if (result! =DD_OK)
    {
		DrawErr("�סȽ��Ģ����̸�Ʈ�ȸ� ����� �� �����ϴ�");
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

// ���� ����ȸ��� ����
    if(DrawSize==DS_FULL)
    {
		result=lpDD->SetCooperativeLevel(hGbWnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN);
    	if(result! =DD_OK)
    	{
			DrawErr("���� ����ȸ� ������ �� �� �����ϴ�");
    		WsDrawRelease();
    		return -1;
    	}
        result=lpDD->SetDisplayMode(640,480, PixelDepth);
    	if(result! =DD_OK)
    	{
			DrawErr("�סȽ��Ģ����̸�Ʈ�ȼ����� �� �� �����ϴ�");
    		WsDrawRelease();
    		return -1;
    	}
    }
    else
    {
		result=lpDD->SetCooperativeLevel(hGbWnd, DDSCL_NORMAL);
    	if(result! =DD_OK)
    	{
			DrawErr("���� ����ȸ� ������ �� �� �����ϴ�");
    		WsDrawRelease();
    		return -1;
    	}
    }
	lpDD->GetMonitorFrequency(&VBFreq);

// �Ģ����̸������佺�� ����
    memset(&DDSD, 0, sizeof(DDSD));
	DDSD.dwSize=sizeof(DDSD);
	DDSD.dwFlags=DDSD_CAPS;
	DDSD.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE;
	result=lpDD->CreateSurface(&DDSD, &lpDDSPrimary, NULL);
    if (result! =DD_OK)
    {
		DrawErr("�Ģ����̸������佺�� �ۼ� �� �� �����ϴ�");
    	WsDrawRelease();
    	return -1;
    }

// �ϡ�ũ���佺�� ����
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
		DrawErr("���� �����ϡ�ũ���佺�� �ۼ� �� �� �����ϴ�");
    	WsDrawRelease();
    	return -1;
    }
    memset(&DDSD, 0, sizeof(DDSD));
	DDSD.dwSize=sizeof(DDSD);
	DDSD.dwFlags=DDSD_ALL;
	result=lpDDSBack0->GetSurfaceDesc(&DDSD);
    if (result! =DD_OK)
    {
		DrawErr("�סȽ��Ģ����̸�Ʈ�ȸ� ����� �� �����ϴ�");
    	WsDrawRelease();
    	return -1;
    }
    Pitch=DDSD.lPitch;

// ũ���Ϣ�-�� ������ �Ҵ�
	if(! (DrawSize==DS_FULL))
    {
		result=lpDD->CreateClipper(0, &lpDDC, NULL);
    	if(result! =DD_OK)
    	{
			DrawErr("ũ���Ϣ�-�� �ۼ� �� �� �����ϴ�");
    		WsDrawRelease();
    		return -1;
    	}
		result=lpDDC->SetHWnd(0, hGbWnd);
    	if(result! =DD_OK)
    	{
			DrawErr("ũ���Ϣ�-�� ��Ʈ�ȿ쿡 ���� �� �� �����ϴ�");
    		WsDrawRelease();
    		return -1;
    	}
		result=lpDDSPrimary->SetClipper(lpDDC);
    	if(result! =DD_OK)
    	{
			DrawErr("ũ���Ϣ�-�� �Ģ����̸������佺�� ���� �� �� �����ϴ�");
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
		DrawErr("16�ޡ�Ʈ���̻� 32�ޡ�Ʈ�����Ϸ� �� �ּ���");
    	WsDrawRelease();
    	return -1;
    }

// ��ȭ ������ ����
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
		DrawErr("���佺�� �����ϴ�");
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
    		DrawErr("�ϡ�ũ���佺�� �� �� �� ���� �ʽ��ϴ�");
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
    		DrawErr("�ϡ�ũ���佺�� ���ũ �� �� ���� �ʽ��ϴ�");
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
    		DrawErr("�ϡ�ũ���佺�� �� �� �� ���� �ʽ��ϴ�");
        	return -1;
    	}

    	memcpy(DDSD.lpSurface, buf+(8*PixelDepth), 224*PixelDepth);

    	result=lpDDSBack0->Unlock(DDSD.lpSurface);
    	if(result! =DD_OK)
    	{
    		if(Verbose)
    		DrawErr("�ϡ�ũ���佺�� ���ũ �� �� ���� �ʽ��ϴ�");
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
		DrawErr("�ϡ�ũ���佺�� �����ϴ�");
    	return -1;
    }

    if(lpDDSPrimary==NULL)
    {
    	if(Verbose)
		DrawErr("�Ģ����̸������佺�� �����ϴ�");
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
    	DrawErr("�Ģ����̸������佺�� ���ȸ�Ʈ �� �� ���� �ʽ��ϴ�");
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
		DrawErr("���佺�� �����ϴ�");
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
		OffsetX=SCR1X&0x07;						// X��ǥ �������� ����
		pSWrBuf=pSBuf-OffsetX*PixelDepth;		// ���佺�ϡ����� ����ȣ�����͸� X���ļ�Ʈ
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

		OffsetX=SCR2X&0x07;						// X��ǥ �������� ����
		pSWrBuf=pSBuf-OffsetX*PixelDepth;		// ���佺�ϡ����� ����ȣ�����͸� X���ļ�Ʈ
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
			pSWrBuf=pSBuf+i*PixelDepth;

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
    	DrawErr("�ϡ�ũ���佺�� ���ȸ�Ʈ �� �� ���� �ʽ��ϴ�");
    	return;
    }

	result=lpDDSPrimary->Blt(NULL, NULL, NULL, DDBLT_WAIT|DDBLT_COLORFILL, &DDBFX);
    if(result! =DD_OK)
    {
    	if(Verbose)
    	DrawErr("�Ģ����̸������佺�� ���ȸ�Ʈ �� �� ���� �ʽ��ϴ�");
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
	MessageBox(NULL, Msg,"WsDraw�׶�", MB_ICONEXCLAMATION|MB_OK);
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








