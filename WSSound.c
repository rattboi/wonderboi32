//---------------------------------------------------------------------------
#include <gpstdlib.h>
#include <gpstdio.h>
#include <gpgraphic.h>
#include <gpmain.h>
#include <gpfont.h>

#include "WSSound.h"
//#include "pcm.h"
#include "sound.h"
//#include "gp32_snd.h"

#include "types.h"

#define MIXBUF_SIZE    16384

static signed int *mixbuf = NULL;

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

int ChPerInit;							// 설정의 주기
int SwpTime;							// 스이후˚시간
int SwpStep;							// 스이후˚주기 스텟후˚
int SwpCurPeriod;						// 현재의 스이후˚주기

int MainVol;							// 현재의 메인 호″류무
int Verbose = 0;

int ChCurVol[6]={-1,-1,-1,-1,-1,-1};	// 현재의 채널호″류무
int ChCurPer[6]={-1,-1,-1,-1,-1,-1};	// 현재의 채널 주기
long ChCurPan[6]={-1,-1,-1,-1,-1,-1};	// 현재의 채널하˚

unsigned char PData[4][BUFSIZE];		// Standard small sample buffers
unsigned char PDataP[BUFSIZEP*16];		// PCM Channel Buffer
unsigned char PDataN[8][BUFSIZEN];		// Noise Channel Buffer

int RandData[BUFSIZEN];					// 파형하˚턴, 노이스″

unsigned char ChannelBuf[4][BUFSIZE];
unsigned char ChannelPCMBuf[BUFSIZEP<<4];		// PCM Channel Buffer
unsigned char ChannelNoiseBuf[BUFSIZEN];		// Noise Channel Buffer

int CntSwp=0;							// 스이후˚카운터
int PcmWrPos=0;							// PCM 파형 기입호˚인터

const long TblChVol[16]={				// n/15 n=0~15
	-10000,-2352,-1750,-1398,-1148,-954,-796,-662,
    -546,-444,-352,-269,-194,-124,-60, 0
};

const long TblMainVol[4]={				// 1,1/2,1/4,1/8
	0,-602,-1204,-1806
};

void  SetSoundFreq(int Channel, int Period);
void  SetSoundVol(int Channel, int Vol);
void  SetSoundPData(int Channel, int Index);

#define BIT(n) (1<<n)

typedef struct {
	unsigned int N;
    int InputBit;
    int Mask;
} POLYNOMIAL;

static const POLYNOMIAL TblMask[]=
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

unsigned int Mrand(unsigned int Degree)
{
	static POLYNOMIAL *pTbl=TblMask;
	static int ShiftReg=BIT(2)-1; //TblMask[0]->InputBit-1; // what/s this?
	int XorReg=0;
	int Masked;

    if(pTbl->N!=Degree)
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
		if(!pTbl->N)
		{
			pTbl--;
		}

    	ShiftReg&=pTbl->InputBit-1;
		if(!ShiftReg)
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

struct snd snd;

//---------------------------------------------------------------------------
int WsSoundCreate()
{
//	Debug=(void*) TblChVol;

	//set up sound format

	mixbuf = GPMALLOC(MIXBUF_SIZE);

	snd.ch[0].on = 1;
	snd.ch[1].on = 1;
	snd.ch[2].on = 1;
	snd.ch[3].on = 1;
	snd.ch[4].on = 0; // PCM channel
	snd.ch[5].on = 0; // noise channel

    int i, j;

    for(i=0;i<4;i++)
    {
		// create the sound buffers for 4 channels
		WsSoundClear(i);
    }

	WsSoundClear(4); // PCM Channel
	WsSoundClear(5); // Noise/Sweep Channel ?

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


void pcm_open()
{
	gp_initSound(22050, 0);
}

void pcm_close(void)
{
	PcmStop();
}

//---------------------------------------------------------------------------
void WsSoundRelease(void)
{
	int i;

    for(i=0;i<6;i++)
    {
		//close all 6 sound channels (4 standard + 2 voice?)

		if(snd.ch[i].on)
        {
			snd.ch[i].on = 0; // turn it off
        }

    }
}

//---------------------------------------------------------------------------
int WsSoundPlay(int Channel)
{
	// Play sound

	int result;

    if(snd.ch[Channel].on == 1) // channel is off
    {
    	return -1;
    }

	snd.ch[Channel].on = 1; 

	return 0;
}

//---------------------------------------------------------------------------
int WsSoundStop(int Channel)
{
	// Stop sound

    int result;

    if(snd.ch[Channel].on == 0) // channel is off
    {
    	return -1;
    }

	snd.ch[Channel].on = 0;

    return 0;
}

//---------------------------------------------------------------------------
void WsSoundClear(int Channel)
{
	// Clear Sound

    int		result;
    uint32	len1, len2, size;

    if(snd.ch[Channel].on == 0)
    {
    	return;
    }

    ChCurVol[Channel]=-1;
    ChCurPer[Channel]=-1;
    ChCurPan[Channel]=-1;

	if(Channel==5)
	{
		size=BUFSIZEN;
		GPMEMSET(ChannelNoiseBuf, PL, BUFSIZEN);
	}
	else if(Channel==4)
	{
		size=BUFSIZEP<<4;
        GPMEMSET(PDataP, PL, BUFSIZEP<<4);
        GPMEMSET(ChannelPCMBuf, PL, BUFSIZEP<<4);
	}
	else
	{
		size=BUFSIZE;
        GPMEMSET(PData[Channel], PL, BUFSIZE);
        GPMEMSET(ChannelBuf[Channel], PL, BUFSIZE);
	}
//  GPMEMSET(ptr1, PL, size);
}

//---------------------------------------------------------------------------
void SetSoundFreq(int Channel, int Period)
{
	// Set Sound Frequency

    int		result;
    uint32	Freq;

    if(!snd.ch[Channel].on)
    {
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

	snd.ch[Channel].freq = Freq;
}

//---------------------------------------------------------------------------
void SetSoundVol(int Channel, int Vol)
{
	// Set Sound Volume

    int result;
    long volume;

    if(snd.ch[Channel].on == 0)
    {
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

	snd.ch[Channel].envol = volume;
}

//---------------------------------------------------------------------------
void SetSoundPan(int Channel, int Left, int Right)
{
	// Set Sound Pan

    int result;
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

    if(snd.ch[Channel].on == 0)
    {
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

//    result=snd.ch[Channel]->SetPan(pan);
}

//---------------------------------------------------------------------------
void SetSoundPData(int Channel, int Index)
{
	// Set Sound PData

	int		result;
    int*	ptr1, ptr2;
    uint32	len1, len2, size;
	unsigned char *pData;
	unsigned char *chanData;

    if(snd.ch[Channel].on == 0)
    {
    	return;
    }

	if(Channel==5) //noise
	{
		size=BUFSIZEN;
		pData=PDataN[Index];
		chanData=ChannelNoiseBuf;
	}
	else if(Channel==4) // voice / pcm 
	{
		size=BUFSIZEP<<4;
		pData=PDataP;
		chanData=ChannelPCMBuf;
	}
	else
	{
		size=BUFSIZE;
		pData=PData[Channel];
		chanData=ChannelBuf[Channel];
	}
    GPMEMCPY(chanData, pData, size);
}

//---------------------------------------------------------------------------
void SetSoundPBuf(int Addr, int Data)
{
	// Set Sound PBuf
	
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
void RstSound(int Channel)
{
	//Reset Sound?

    if(Channel==2)
    {
    	SetSoundFreq(2, ChPerInit);
		SwpCurPeriod=ChPerInit;
    }
}

//---------------------------------------------------------------------------
int WsSoundInt(void)
{

	// Sound Interrupt?

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
    		snd.ch[2].freq = value; // set frequency of channel 3?
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

void SetPCM(int Data)
{
	DWORD tick;
	PDataP[PCMPos++]=(unsigned char) Data; // PCM Channel
    tick=GpTickCountGet();
    PcmCount++;
    if(tick>=TickZ)
    {
    	TickZ=tick+125;
        PcmCount<<=3;
        if(PcmCount>=10000)
        {
        	PcmCount=12000;
        }
        snd.ch[4].freq = PcmCount; //set frequency of channel 5?
        PcmCount=0;
    }

	if(PCMPos>=BUFSIZEP)
	{
		FlashPCM();
    }
}

//---------------------------------------------------------------------------
void FlashPCM(void)
{
	// Flash PCM (what is this?)

	const DWORD WrPos[16]={
		BUFSIZEP*0, BUFSIZEP*1, BUFSIZEP*2, BUFSIZEP*3,
		BUFSIZEP*4, BUFSIZEP*5, BUFSIZEP*6, BUFSIZEP*7,
		BUFSIZEP*8, BUFSIZEP*9, BUFSIZEP*10, BUFSIZEP*11,
		BUFSIZEP*12, BUFSIZEP*13, BUFSIZEP*14, BUFSIZEP*15,
    };

    if(!snd.ch[4].on)
	{
		return;
	}

	GPMEMCPY(ChannelPCMBuf+WrPos[PcmWrPos], PDataP, BUFSIZEP);

	PcmWrPos++;
    PcmWrPos&=0xF;
	GPMEMSET(PDataP, PL, sizeof(PDataP)); // Noise Channel
	PCMPos=0;

}

static void clearmixbuffer(signed int * buf, int n)
{
	while((--n)>=0){
		*buf = 0;
		++buf;
	}
}

void updatemixing(signed char *out_l, int todo)
{
	int s, l, r, f, n, i;

	clearmixbuffer(mixbuf, todo);
	
	for( i = 0; i < todo; i++)
	{
		l = r = 0;
		for(i = 0; i < 4; i++)
		{
			if (snd.ch[i].on)
			{
				s = ChannelBuf[i][(snd.ch[i].pos>>16) & 1023];
				if (snd.ch[i].pos & (1<<15)) 
					s &= 15;
				else 
					s >>= 4;
//				s -= 8;

				snd.ch[i].pos += snd.ch[i].freq;

				l += s;
				r += s;
			}
		}

//		l *= (R_NR50 & 0x07);
//		r *= ((R_NR50 & 0x70)>>4);
//		l >>= 4;
//		r >>= 4;
		
		if (l > 127)		l = 127;
		else if (l < -128)	l = -128;
		if (r > 127)		r = 127;
		else if (r < -128)	r = -128;

		mixbuf[i] = ((l+r)>>1)+128;
	}

	for(i = 0; i < todo; i++) 
		out_l[i] = mixbuf[i];
}
