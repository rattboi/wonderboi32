//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "WSSound.h"
#include "Config.h"
//---------------------------------------------------------------------------
// DirectX 하″시″3 이상으로 확인 이 끝난 상태 하″시″2 에서도 동작 가능
//---------------------------------------------------------------------------
#define BPS 22050
#define BPSMAX 100000
#define BPSMIN 100
#define BUFSIZE 1024
#define BUFSIZEN 0x10000
#define BUFSIZEP 159
#define PCMNUM 100
#define POFF 128
#define PDIV 3
#define PH POFF+PDIV*8
#define PL POFF-PDIV*7

LPDIRECTSOUND lpDS=NULL;				// DirectSound됫후″시″크트
LPDIRECTSOUNDBUFFER lpDSB[6];			// 하″크하″파 0 됫후″시″크트

int ChPerInit;							// 설정의 주기
int SwpTime;							// 스이후˚시간
int SwpStep;							// 스이후˚주기 스텟후˚
int SwpCurPeriod;						// 현재의 스이후˚주기

int MainVol;							// 현재의 메인 호″류무

int ChCurVol[6]={-1,-1,-1,-1,-1,-1};	// 현재의 채널호″류무
int ChCurPer[6]={-1,-1,-1,-1,-1,-1};	// 현재의 채널 주기
long ChCurPan[6]={-1,-1,-1,-1,-1,-1};	// 현재의 채널하˚

unsigned char PData[4][BUFSIZE];		// 파형하˚턴,1/8,1/4,1/2,3/4, 임의
unsigned char PDataP[BUFSIZEP<<4];		// PCM 파형하˚턴
unsigned char PDataN[8][BUFSIZEN];		// 파형하˚턴, 노이스″

int RandData[BUFSIZEN];					// 파형하˚턴, 노이스″

int CntSwp=0;							// 스이후˚카운터
int PcmWrPos=0;							// PCM 파형 기입호˚인터

const long TblChVol[16]={				// n/15 n=0~15
	-10000,-2352,-1750,-1398,-1148,-954,-796,-662,
    -546,-444,-352,-269,-194,-124,-60, 0
};

const long TblMainVol[4]={				// 1,1/2,1/4,1/8
	0,-602,-1204,-1806
};

void __fastcall SetSoundFreq(int Channel, int Period);
void __fastcall SetSoundVol(int Channel, int Vol);
void __fastcall SetSoundPData(int Channel, int Index);
void __fastcall SoundErr(LPCSTR Msg);

//---------------------------------------------------------------------------
/***************************************/
/* M계열 신호 발생기                     */
/* 불려 갈 때에 M계열 신호를 돌려줍니다 */
/***************************************/
unsigned int __fastcall Mrand(unsigned int Degree)
{
#define BIT(n) (1<<n)
	typedef struct {
		unsigned int N;
    	int InputBit;
    	int Mask;
	} POLYNOMIAL;

	static POLYNOMIAL TblMask[]=
	{
		{ 2,BIT(2) ,BIT(0)|BIT(1)},
		{ 3,BIT(3) ,BIT(0)|BIT(1)},
		{ 4,BIT(4) ,BIT(0)|BIT(1)},
		{ 5,BIT(5) ,BIT(0)|BIT(2)},
		{ 6,BIT(6) ,BIT(0)|BIT(1)},
		{ 7,BIT(7) ,BIT(0)|BIT(1)},
		{ 8,BIT(8) ,BIT(0)|BIT(2)|BIT(3)|BIT(4)},
		{ 9,BIT(9) ,BIT(0)|BIT(4)},
		{10,BIT(10),BIT(0)|BIT(3)},
		{11,BIT(11),BIT(0)|BIT(2)},
		{12,BIT(12),BIT(0)|BIT(1)|BIT(4)|BIT(6)},
		{13,BIT(13),BIT(0)|BIT(1)|BIT(3)|BIT(4)},
		{14,BIT(14),BIT(0)|BIT(1)|BIT(4)|BIT(5)},
		{15,BIT(15),BIT(0)|BIT(1)},
		{NULL,NULL,NULL},
	};

	static POLYNOMIAL *pTbl=TblMask;
	static int ShiftReg=pTbl->InputBit-1;
	int XorReg=0;
	int Masked;

    if(pTbl->N! =Degree)
    {
    	pTbl=TblMask;
		while(pTbl->N)
		{
			if(pTbl->N==Degree)
			{
				break;
			}
        	pTbl++;
		}
		if(! pTbl->N)
		{
			pTbl--;
		}

    	ShiftReg&=pTbl->InputBit-1;
		if(! ShiftReg)
		{
			ShiftReg=pTbl->InputBit-1;
		}
    }

	Masked=ShiftReg&pTbl->Mask;
	while(Masked)
	{
		XorReg^=Masked&0x01;
		Masked>>=1;
	}

	if(XorReg)
	{
		ShiftReg|=pTbl->InputBit;
	}
	else
	{
		ShiftReg&=~pTbl->InputBit;
	}
	ShiftReg>>=1;

	return ShiftReg;
}
//---------------------------------------------------------------------------
int __fastcall WsSoundCreate(HWND hWnd)
{
    HRESULT result;
    PCMWAVEFORMAT PCMWF;
    DSBUFFERDESC DSBD;

Debug=(void*) TblChVol;
// DSound의 생성
	result=DirectSoundCreate(NULL, &lpDS, NULL);
    if(result! =DS_OK)
    {
		SoundErr("DSound를 작성 할 수 없습니다");
    	WsSoundRelease();
    	return -1;
    }

// 협조 레헤″르의 설정
	result=lpDS->SetCooperativeLevel(hWnd, DSSCL_NORMAL);
    if(result! =DS_OK)
    {
    	SoundErr("협조 레헤″르 설정을 할 수 없습니다");
        WsSoundRelease();
        return -1;
    }

// 하″크하″파의 생성
    memset(&PCMWF, 0, sizeof(PCMWAVEFORMAT));
    PCMWF.wf.wFormatTag=WAVE_FORMAT_PCM;
    PCMWF.wf.nChannels=1;
    PCMWF.wf.nSamplesPerSec=BPS;
    PCMWF.wf.nBlockAlign=1;
    PCMWF.wf.nAvgBytesPerSec=PCMWF.wf.nSamplesPerSec*PCMWF.wf.nBlockAlign;
    PCMWF.wBitsPerSample = 8;
    memset(&DSBD, 0, sizeof(DSBD));
	DSBD.dwSize=sizeof(DSBD);
   	DSBD.dwFlags=DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
    DSBD.dwBufferBytes=BUFSIZE;
    DSBD.lpwfxFormat=(LPWAVEFORMATEX) &PCMWF;

    int i, j;

    for(i=0;i<4;i++)
    {
		result=lpDS->CreateSoundBuffer(&DSBD, &lpDSB[i], NULL);
    	if(result! =DS_OK)
    	{
			SoundErr("하″크하″파를 작성 할 수 없습니다");
    		WsSoundRelease();
    		return -1;
    	}
		WsSoundClear(i);
    }

	DSBD.dwBufferBytes=BUFSIZEN;
	result=lpDS->CreateSoundBuffer(&DSBD, &lpDSB[5], NULL);
	if(result! =DS_OK)
	{
		SoundErr("하″크하″파를 작성 할 수 없습니다");
		WsSoundRelease();
		return -1;
	}
	WsSoundClear(5);

    PCMWF.wf.nSamplesPerSec=BPS;
    PCMWF.wf.nAvgBytesPerSec=PCMWF.wf.nSamplesPerSec*PCMWF.wf.nBlockAlign;
   	DSBD.dwFlags=DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
    DSBD.dwBufferBytes=(BUFSIZEP<<4) *PCMWF.wf.nBlockAlign;
    result=lpDS->CreateSoundBuffer(&DSBD, &lpDSB[4], NULL);
    if(result! =DS_OK)
    {
    	SoundErr("하″크하″파를 작성 할 수 없습니다");
        WsSoundRelease();
        return -1;
    }
	WsSoundClear(4);

    int rand;
	for(i=0;i<8;i++)
	{
    	for(j=0;j<BUFSIZEN;j++)
    	{
    		rand=Mrand(15-i)&1;
            if(rand)
            {
    			PDataN[i][j]=PH;
            }
            else
            {
    			PDataN[i][j]=PL;
            }
    	}
	}
    for(j=0;j<BUFSIZEN;j++)
    {
    	RandData[j]=Mrand(15);
    }

    return 0;
}

//---------------------------------------------------------------------------
void __fastcall WsSoundRelease(void)
{
	int i;

	if(lpDS!=NULL)
    {
    	for(i=0;i<6;i++)
        {
        	if(lpDSB[i]!=NULL)
        	{
				lpDSB[i]->Stop();
            	lpDSB[i]->Release();
            	lpDSB[i]=NULL;
        	}
        }
        lpDS->Release();
        lpDS=NULL;
    }
}

//---------------------------------------------------------------------------
int __fastcall WsSoundPlay(int Channel)
{
    HRESULT result;

    if(lpDSB[Channel]==NULL)
    {
    	if(Verbose)
		SoundErr("하″크하″파가 없습니다");
    	return -1;
    }

	result=lpDSB[Channel]->Play(0,0, DSBPLAY_LOOPING);
    if(result! =DS_OK)
    {
    	SoundErr("하″크하″파를 재생 할 수 없습니다");
    	return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
int __fastcall WsSoundStop(int Channel)
{
    HRESULT result;

    if(lpDSB[Channel]==NULL)
    {
    	if(Verbose)
		SoundErr("하″크하″파가 없습니다");
    	return -1;
    }

	result=lpDSB[Channel]->Stop();
    if(result! =DS_OK)
    {
    	SoundErr("하″크하″파를 정지 할 수 없습니다");
    	return -1;
    }

    return 0;
}

//---------------------------------------------------------------------------
void __fastcall WsSoundClear(int Channel)
{
    HRESULT result;
    LPVOID ptr1, ptr2;
    DWORD len1, len2, size;

    if(lpDSB[Channel]==NULL)
    {
    	if(Verbose)
		SoundErr("하″크하″파가 없습니다");
    	return;
    }

    ChCurVol[Channel]=-1;
    ChCurPer[Channel]=-1;
    ChCurPan[Channel]=-1;

	if(Channel==5)
	{
		size=BUFSIZEN;
	}
	else if(Channel==4)
	{
		size=BUFSIZEP<<4;
        memset(PDataP, PL, BUFSIZEP<<4);
	}
	else
	{
		size=BUFSIZE;
        memset(PData[Channel], PL, BUFSIZE);
	}

	result=lpDSB[Channel]->Lock(0, size, &ptr1, &len1, &ptr2, &len2, 0);
    if(result! =DS_OK)
    {
    	SoundErr("하″크하″파를 락 할 수 없지 않습니다");
    	return;
    }

    memset(ptr1, PL, size);

    result=lpDSB[Channel]->Unlock(ptr1, len1, ptr2, len2);
    if(result! =DS_OK)
    {
    	SoundErr("하″크하″파를 언로크 할 수 없지 않습니다");
        return;
    }
}

//---------------------------------------------------------------------------
void __fastcall SetSoundFreq(int Channel, int Period)
{
    HRESULT result;
    DWORD Freq;

    if(lpDSB[Channel]==NULL)
    {
    	if(Verbose)
		SoundErr("하″크하″파가 없습니다");
    	return;
    }

    if(ChCurPer[Channel]==Period)
    {
    	return;
    }
    ChCurPer[Channel]=Period;

    Freq=3072000/(2048-Period);
	if(Channel==2)
	{
		ChPerInit=Period;
		SwpCurPeriod=Period;
	}
    if(Freq>BPSMAX)
	{
		Freq=BPSMAX;
	}
	else if(Freq<BPSMIN)
	{
		Freq=BPSMIN;
	}

	result=lpDSB[Channel]->SetFrequency(Freq);
	if(result! =DS_OK)
	{
    	if(Verbose)
		SoundErr("하″크하″파의 주파수를 설정 할 수 없습니다");
		return;
	}
}

//---------------------------------------------------------------------------
void __fastcall SetSoundVol(int Channel, int Vol)
{
    HRESULT result;
    long volume;

    if(lpDSB[Channel]==NULL)
    {
    	if(Verbose)
		SoundErr("하″크하″파가 없습니다");
    	return;
    }

    if(ChCurVol[Channel]==Vol)
    {
    	return;
    }
    ChCurVol[Channel]=Vol;

    volume=TblChVol[ChCurVol[Channel]]+TblMainVol[MainVol];
    if(volume<-10000)
    {
    	volume=-10000;
    }

    result=lpDSB[Channel]->SetVolume(volume);
    if(result! =DS_OK)
    {
    	SoundErr("하″크하″파의 호″류무를 설정 할 수 없습니다");
    	return;
    }
}

//---------------------------------------------------------------------------
void __fastcall SetSoundPan(int Channel, int Left, int Right)
{
    HRESULT result;
    long pan;

    const long TblPan[16][16]={
	{     0, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000},
	{-10000,     0,   602,   954,  1204,  1398,  1556,  1690,  1806,  1908,  2000,  2082,  2158,  2228,  2292,  2352},
	{-10000,  -602,     0,   352,   602,   796,   954,  1088,  1204,  1306,  1398,  1481,  1556,  1626,  1690,  1750},
	{-10000,  -954,  -352,     0,   250,   444,   602,   736,   852,   954,  1046,  1129,  1204,  1274,  1338,  1398},
	{-10000, -1204,  -602,  -250,     0,   194,   352,   486,   602,   704,   796,   879,   954,  1024,  1088,  1148},
	{-10000, -1398,  -796,  -444,  -194,     0,   158,   292,   408,   511,   602,   685,   760,   830,   894,   954},
	{-10000, -1556,  -954,  -602,  -352,  -158,     0,   134,   250,   352,   444,   526,   602,   672,   736,   796},
	{-10000, -1690, -1088,  -736,  -486,  -292,  -134,     0,   116,   218,   310,   393,   468,   538,   602,   662},
	{-10000, -1806, -1204,  -852,  -602,  -408,  -250,  -116,     0,   102,   194,   277,   352,   422,   486,   546},
	{-10000, -1908, -1306,  -954,  -704,  -511,  -352,  -218,  -102,     0,    92,   174,   250,   319,   384,   444},
	{-10000, -2000, -1398, -1046,  -796,  -602,  -444,  -310,  -194,   -92,     0,    83,   158,   228,   292,   352},
	{-10000, -2082, -1481, -1129,  -879,  -685,  -526,  -393,  -277,  -174,   -83,     0,    76,   145,   209,   269},
	{-10000, -2158, -1556, -1204,  -954,  -760,  -602,  -468,  -352,  -250,  -158,   -76,     0,    70,   134,   194},
	{-10000, -2228, -1626, -1274, -1024,  -830,  -672,  -538,  -422,  -319,  -228,  -145,   -70,     0,    64,   124},
	{-10000, -2292, -1690, -1338, -1088,  -894,  -736,  -602,  -486,  -384,  -292,  -209,  -134,   -64,     0,    60},
	{-10000, -2352, -1750, -1398, -1148,  -954,  -796,  -662,  -546,  -444,  -352,  -269,  -194,  -124,   -60,     0},
    };

    if(lpDSB[Channel]==NULL)
    {
    	if(Verbose)
		SoundErr("하″크하″파가 없습니다");
    	return;
    }

    if(Left>Right)
    {
    	SetSoundVol(Channel, Left);
    }
    else
    {
    	SetSoundVol(Channel, Right);
    }

    pan=TblPan[Left][Right];

    if(ChCurPan[Channel]==pan)
    {
    	return;
    }
    ChCurPan[Channel]=pan;

    result=lpDSB[Channel]->SetPan(pan);
    if(result! =DS_OK)
    {
    	SoundErr("하″크하″파의 하˚를 설정 할 수 없습니다");
        return;
    }
}

//---------------------------------------------------------------------------
void __fastcall SetSoundPData(int Channel, int Index)
{
    HRESULT result;
    LPVOID ptr1, ptr2;
    DWORD len1, len2, size;
	unsigned char *pData;

    if(lpDSB[Channel]==NULL)
    {
    	if(Verbose)
    	SoundErr("하″크하″파가 없습니다");
    	return;
    }

	if(Channel==5)
	{
		size=BUFSIZEN;
		pData=PDataN[Index];
	}
	else if(Channel==4)
	{
		size=BUFSIZEP<<4;
		pData=PDataP;
	}
	else
	{
		size=BUFSIZE;
		pData=PData[Channel];
	}

	result=lpDSB[Channel]->Lock(0, size, &ptr1, &len1, &ptr2, &len2, 0);
    if(result! =DS_OK)
    {
    	SoundErr("하″크하″파를 락 할 수 없지 않습니다");
    	return;
    }

    memcpy(ptr1, pData, size);

    result=lpDSB[Channel]->Unlock(ptr1, len1, ptr2, len2);
    if(result! =DS_OK)
    {
    	SoundErr("하″크하″파를 언로크 할 수 없지 않습니다");
        return;
    }

}

//---------------------------------------------------------------------------
void __fastcall SetSoundPBuf(int Addr, int Data)
{
	int i, j;

	i=(Addr&0x30)>>4;
    for(j=(Addr&0x0F)<<1;j<BUFSIZE;j+=32)
    {
    	PData[i][j]=(unsigned char)(POFF+PDIV*((Data&0x0F)-7));
    	PData[i][j+1]=(unsigned char)(POFF+PDIV*(((Data&0xF0)>>4)-7));
    }
    if((Addr&0x0F) ==0x0F)
    {
    	SetSoundPData(i, 0);
    }

}

//---------------------------------------------------------------------------
void __fastcall RstSound(int Channel)
{
    if(Channel==2)
    {
    	SetSoundFreq(2, ChPerInit);
		SwpCurPeriod=ChPerInit;
    }
}

//---------------------------------------------------------------------------
int __fastcall WsSoundInt(void)
{
	int value;
    static int i;

    if((SwpStep)&&(SNDMOD&0x40))
    {
    	if(CntSwp<0)
        {
        	CntSwp=SwpTime;
			SwpCurPeriod+=SwpStep;
            SwpCurPeriod&=0x7FF;
			value=3072000/(2048-SwpCurPeriod);
	        if(value>100000)
	        {
        		value=100000;
        		SetSoundVol(2,0);
	        }
	        if(value<100)
	        {
        		value=100;
        	}
    		lpDSB[2]->SetFrequency(value);
        }
        CntSwp--;
    }
    i++;
    if(i>=BUFSIZEN)
    {
    	i=0;
    }
    return RandData[i];
}

//---------------------------------------------------------------------------
static DWORD PCMPos=0;
DWORD TickZ=0, PcmCount;

void __fastcall SetPCM(int Data)
{
	DWORD tick;
	PDataP[PCMPos++]=(unsigned char) Data;
    tick=GetTickCount();
    PcmCount++;
    if(tick>=TickZ)
    {
    	TickZ=tick+125;
        PcmCount<<=3;
        if(PcmCount>=10000)
        {
        	PcmCount=12000;
        }
        lpDSB[4]->SetFrequency(PcmCount);
        PcmCount=0;
    }

	if(PCMPos>=BUFSIZEP)
	{
		FlashPCM();
    }
}

//---------------------------------------------------------------------------
void __fastcall FlashPCM(void)
{
    HRESULT result;
    LPVOID ptr1, ptr2;
    DWORD len1, len2;
    const DWORD WrPos[16]={
		BUFSIZEP*0, BUFSIZEP*1, BUFSIZEP*2, BUFSIZEP*3,
		BUFSIZEP*4, BUFSIZEP*5, BUFSIZEP*6, BUFSIZEP*7,
		BUFSIZEP*8, BUFSIZEP*9, BUFSIZEP*10, BUFSIZEP*11,
		BUFSIZEP*12, BUFSIZEP*13, BUFSIZEP*14, BUFSIZEP*15,
    };

    if(lpDSB[4]==NULL)
	{
    	if(Verbose)
		SoundErr("하″크하″파가 없습니다");
		return;
	}

	result=lpDSB[4]->Lock(WrPos[PcmWrPos], BUFSIZEP, &ptr1, &len1, &ptr2, &len2, 0);
	if(result! =DS_OK)
	{
		SoundErr("하″크하″파를 락 할 수 없지 않습니다");
		return;
	}

 	memcpy(ptr1, PDataP, len1);
	if(ptr2! =NULL)
	{
		memcpy(ptr2, PDataP+len1, len2);
	}

	result=lpDSB[4]->Unlock(ptr1, len1, ptr2, len2);
	if(result! =DS_OK)
	{
		SoundErr("하″크하″파를 언로크 할 수 없지 않습니다");
		return;
	}

    PcmWrPos++;
    PcmWrPos&=0xF;
	memset(PDataP, PL, sizeof(PDataP));
	PCMPos=0;

}

//---------------------------------------------------------------------------
void __fastcall SoundErr(LPCSTR Msg)
{
	MessageBox(NULL, Msg,"WsSound닝라", MB_ICONEXCLAMATION|MB_OK);
}

//---------------------------------------------------------------------------
