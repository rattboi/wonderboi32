//---------------------------------------------------------------------------
#include "types.h"


#include <gpstdlib.h>
#include <gpstdio.h>
#include <gpgraphic.h>
#include <gpmain.h>
#include <gpfont.h>

#include "WSHard.h"
#include "WS.h"
#include "WSDraw.h"
#include "WSInput.h"
#include "WSSound.h"

#include "necintrf.h"

//#define SOUND_DEBUG
//#define PCM_DEBUG
//#define SOUND_SPRITE	

//---------------------------------------------------------------------------
int SoundOn[7]={1,1,1,1,1,1,1};
int FrameSkip=4;
int SkipCnt=0;
int TblSkip[11][10]={
    {1,1,1,1,1,1,1,1,1,1},
    {0,1,1,1,1,1,1,1,1,1},
    {0,1,1,1,1,0,1,1,1,1},
    {0,1,1,0,1,1,1,0,1,1},
    {0,1,1,0,1,1,0,1,1,0},
    {0,1,0,1,0,1,0,1,0,1},
    {0,1,0,0,1,0,1,0,1,0},
    {1,0,0,1,0,0,1,0,0,0},
    {0,0,0,0,1,0,0,0,0,1},
    {0,0,0,0,1,0,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0},
};

int vert;
uint16 RomCRC;
extern uint8 *base_rom;

void ErrorMsg(char *Msg)
{
	char str[128];
    int i=nec_get_reg(NEC_CS)<<4;
    int j=nec_get_reg(NEC_IP);

	GPSPRINTF(str,"%s %06X", Msg, i+j);
    PrintMessage(str,1);
    return;
}

struct BYTE2INT
{
	byte Code;
	int Value;
};

struct WORD2CPTR
{
	word Code;
	char *Cptr;
};

uint8 *Page[16];				//리트″아트″Respond 공간 0 x10000 단위
uint8 IRAM[0x10000];			// 내장 RAM 공간의 64kB
uint8 MemDummy[0x10000];		// 더미 공간의 64kB
uint8 IO[0x100];
WsRomHeader *WsromHeader;

#define CK_EEP 0x00000001
int CartKind;

uint8 *ROMMap[0x100];		// table of ROM banks
int ROMBanks;				// # of banks of ROM (up to 256)
const static struct BYTE2INT ROMBanksTable[] = {
	{0x01, 8},				// [header 3] identifies the size of the ROM (# of banks)
	{0x01, 8},				
	{0x02, 8},
	{0x03, 16},
	{0x04, 32},
	{0x06, 64},
	{0x08, 128},
	{0x09, 256},
	{NULL, NULL}
};

uint8 SRAM[0x10000];		// Holds map of all SAVE RAM banks (only one entry ever)
int RAMSize;				// Size of Save Ram
int RAMEnable;

/*
const static struct BYTE2INT RAMBanksTable[] = {
	{0x01, 1},				// [Header 4] identifies RAM type and size
	{0x02, 1},
	{0x10, 1},
	{0x20, 1},
	{NULL, NULL}
};
*/

const static struct BYTE2INT RAMSizeTable[] = {
	{0x01, 0x2000},			// [Header 4] identifies RAM type and size
	{0x02, 0x8000},
	{0x10, 0x80},
	{0x20, 0x800},
	{NULL, NULL}
};

const static struct BYTE2INT RAMKindTable[] = {
	{0x10, CK_EEP},
	{0x20, CK_EEP},
	{NULL, NULL}
};

int IPeriod=32;				// HBlank/8 주기 [클락](256/8)

char SaveName[512];			// 테″레크트리+파일명+". sav"

uint8 DefColor[]={	0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
					0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00};
uint8 MonoColor[8];

uint16 JoyState = 0x0000;		// Joystick state: B.A.START.OPTION.X4.X3.X2.X1.Y4.Y3.Y2.Y1

int WaveMap;

char str[128];

struct EEPROM
{
	byte *pData;
	int WrEnable;
};

struct EEPROM sIEep;
struct EEPROM sCEep;

uint8 IEep[0x800];

void ComEEP(struct EEPROM *Eep, byte *Cmd, byte *Data)
{
	int i, j, cmd, op, addr;
	const int tblmask[16][5]=
	{
		{0x0000, 0,0x0000, 0,0x0000},
		{0x0000, 0,0x0000, 0,0x0000},
		{0x0000, 0,0x0000, 0,0x0000},
		{0x0000, 0,0x0000, 0,0x0000},
		{0x000C, 2,0x0003, 0,0x0003},
		{0x0018, 3,0x0006, 1,0x0007},
		{0x0030, 4,0x000C, 2,0x000F},
		{0x0060, 5,0x0018, 3,0x001F},
		{0x00C0, 6,0x0030, 4,0x003F},
		{0x0180, 7,0x0060, 5,0x007F},
		{0x0300, 8,0x00C0, 6,0x00FF},
		{0x0600, 9,0x0180, 7,0x01FF},
		{0x0C00, 10,0x0300, 8,0x03FF},
		{0x1800, 11,0x0600, 9,0x07FF},
		{0x3000, 12,0x0C00, 10,0x0FFF},
		{0x6000, 13,0x1800, 11,0x1FFF},
	};

	if(Eep->pData==NULL)
	{
		return;
	}

	cmd=(Cmd[1]<<8) |Cmd[0];
	for(i=15, j=0x8000;i>=0;i--, j>>=1)
	{
		if(cmd&j)
		{
			break;
		}
	}

	op=(cmd&tblmask[i][0])>>tblmask[i][1];
	switch(op)
	{
		case 0:
			addr=(cmd&tblmask[i][2])>>tblmask[i][3];
			switch(addr)
			{
				case 0:
					Eep->WrEnable=0;
					break;
				case 1:
					if(Eep->WrEnable)
					{
						for(j=tblmask[i][4];j>=0;j--)
						{
							Eep->pData[j<<1]=Data[0];
							Eep->pData[(j<<1)+1]=Data[1];
						}
					}
					break;
				case 2:
					if(Eep->WrEnable)
					{
						GPMEMSET(Eep->pData,-1, (tblmask[i][4]+1)<<1);
					}
					break;
				case 3:
					Eep->WrEnable=1;
					break;
			}
			Data[0]=0;
			Data[1]=0;
			break;
		case 1:
        	if(Eep->WrEnable)
            {
				addr=cmd&tblmask[i][4];
				Eep->pData[addr<<1]=Data[0];
				Eep->pData[(addr<<1)+1]=Data[1];
            }
			Data[0]=0;
			Data[1]=0;
			break;
		case 2:
			addr=cmd&tblmask[i][4];
			Data[0]=Eep->pData[addr<<1];
			Data[1]=Eep->pData[(addr<<1)+1];
			break;
		case 3:
        	if(Eep->WrEnable)
            {
				addr=cmd&tblmask[i][4];
				Eep->pData[addr<<1]=-1;
				Eep->pData[(addr<<1)+1]=-1;
            }
			Data[0]=0;
			Data[1]=0;
			break;
		default: break;
	}
}

byte  ReadMem(unsigned int A)
{
	return Page[((A)>>16) &0xF][(A) &0xFFFF];
}

void  WriteMem(unsigned int A, byte V)
{
	(*WriteMemFnTable[(A>>16) &0x0F])(A, (byte) V);
}

static void WriteRom(unsigned A, byte V)
{

}

static void WriteIRam(unsigned A, byte V)
{
	// wsc palette
	if((A&0xFE00)==0xFE00)
	{
    	SetPalette(A&0x1FF, V);
	}
	else
	if (((A&0xFFFF)>=0x2000)&&((A&0xFFFF)<0x4000))
	{
		if (IRAM[A&0xFFFF]!=V)
			ws_modified_tile[(A&0x1fff)>>4]=1;
	}
	else
	// wsc 16 color tiles bank 0
	if (((A&0xFFFF)>=0x4000)&&((A&0xFFFF)<0x8000))
	{
		if (IRAM[A&0xFFFF]!=V)
			wsc_modified_tile[(A&0x3fff)>>5]=1;
	}
	else
	// wsc 16 color tiles bank 1
	if (((A&0xFFFF)>=0x8000)&&((A&0xFFFF)<0xC000))
	{
		if (IRAM[(A&0xFFFF)]!=V)
			wsc_modified_tile[((A&0x3fff)>>5)+512]=1;
	}
/*
	if(!((A-WaveMap) &0xFFC0))
	{
	#ifdef SOUND_DEBUG
	GPSPRINTF(str,"%05d-\t\t\t\t\t(%02X) %02X\n", GetTickCount() &0xFFFF, A&0x003F, V);
	LogFile(LK_SOUND, str);
	#endif
		SetSoundPBuf(A&0x003F, V);
	}
*/
	IRAM[A&0xFFFF]=V;
}

static void  WriteCRam(unsigned A, byte V)
{
	Page[(A>>16) &0x0F][A&0xFFFF]=V;
}

void  WriteIO(unsigned A, byte V)
{
	int i, j, k, n;
    byte b;

	switch(A)
	{
		case 0x00:break;
		case 0x01:
#ifdef SOUND_SPRITE
GPSPRINTF(str,"(%02X) %02X\n", A, V);
LogFile(LK_SPRITE, str);
#endif
					break;
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
		case 0x06:break;
		case 0x07:
					Scr1TMap=IRAM+((V&0x0F)<<11);
					Scr2TMap=IRAM+((V&0xF0)<<7);
					break;
		case 0x08:
		case 0x09:
		case 0x0A:
		case 0x0B:
		case 0x0C:
		case 0x0D:
		case 0x0E:
		case 0x0F:
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
		case 0x14:
		case 0x15:break;
		case 0x16:break;
		case 0x17:break;

		case 0x1C:
		case 0x1D:
		case 0x1E:
		case 0x1F:
        			if(COLCTL&0x80) break;
					i=((A-0x1C)<<2) +0x200;
					j=DefColor[V&0x0F];
					SetPalette(i++, (byte) j);
					SetPalette(i++, (byte) j);
					j=DefColor[(V>>4) &0x0F];
					SetPalette(i++, (byte) j);
					SetPalette(i++, (byte) j);

					i=(A-0x1C)<<1;
					MonoColor[i]=DefColor[V&0x0F];
					MonoColor[i+1]=DefColor[(V&0xF0)>>4];
                    for(k=0x20;k<0x40;k++)
                    {
						i=(k&0x1E)<<4;
						if(k&0x01) i+=4;
						j=MonoColor[IO[k]&0x07];
						SetPalette(i++, (byte) j);
						SetPalette(i++, (byte) j);
						j=MonoColor[(IO[k]>>4) &0x07];
						SetPalette(i++, (byte) j);
						SetPalette(i++, (byte) j);
                    }
#ifdef SOUND_SPRITE
GPSPRINTF(str,"\t(%02X) %02X\n", A, V);
LogFile(LK_SPRITE, str);
#endif
					break;

		case 0x20:
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
		case 0x28:
		case 0x29:
		case 0x2A:
		case 0x2B:
		case 0x2C:
		case 0x2D:
		case 0x2E:
		case 0x2F:
		case 0x30:
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x38:
		case 0x39:
		case 0x3A:
		case 0x3B:
		case 0x3C:
		case 0x3D:
		case 0x3E:
		case 0x3F:
        			if(COLCTL&0x80) break;
					i=(A&0x1E)<<4;
					if(A&0x01) i+=4;
					j=MonoColor[V&0x07];
					SetPalette(i++, (byte) j);
					SetPalette(i++, (byte) j);
					j=MonoColor[(V>>4) &0x07];
					SetPalette(i++, (byte) j);
					SetPalette(i++, (byte) j);
#ifdef SOUND_SPRITE
GPSPRINTF(str,"\t\t(%02X) %02X\n", A, V);
LogFile(LK_SPRITE, str);
#endif
					break;

		case 0x40:
		case 0x41:
		case 0x42:
		case 0x43:
		case 0x44:
		case 0x45:
		case 0x46:
		case 0x47:break;
		case 0x48:
					if(V&0x80)
					{
						n=(DMACH<<8) |DMACL;
						i=(DMASB<<16)|(DMASH<<8) |DMASL;
						j=(DMADH<<8) |DMADL;
						for(k=0;k<n;k++)
						{
							b=ReadMem(i);
							WriteMem(j,b);
							i++;
							j++;
						}
                        n=0;
						DMASB=(byte)((i>>16) &0xFF);
						DMASH=(byte)((i>>8) &0xFF);
						DMASL=(byte)(i&0xFF);
						DMADB=(byte)((j>>16) &0xFF);
						DMADH=(byte)((j>>8) &0xFF);
						DMADL=(byte)(j&0xFF);
						DMACH=(byte)((n>>8) &0xFF);
						DMACL=(byte)(n&0xFF);
						V&=0x7F;
					}
					break;
		case 0x49:break;

		case 0x4A:
		case 0x4B:
		case 0x4C:
		case 0x4D:
		case 0x4E:
		case 0x4F:
		case 0x50:
		case 0x51:break;
		case 0x52:
		case 0x53:break;

		case 0x60:
			{
				WsClearGpuCache();
				switch((V&0xE0))
				{
/*				case 0xE0:
					RefreshLine = renderLine[0];
					break;
				case 0xC0:
					RefreshLine = renderLine[0];
					break;
				case 0x60:
					RefreshLine = renderLine[0];
					break;
				case 0x40:
					RefreshLine = renderLine[0];
					break;
*/
				default:
					RefreshLine = renderLine[1];
					break;
				}
			}
			break;

		case 0x62:break;

		case 0x64:
		case 0x65:
		case 0x66:
		case 0x67:
		case 0x68:
		case 0x69:
		case 0x6A:
		case 0x6B:break;

		case 0x80:
#ifdef SOUND_DEBUG
GPSPRINTF(str,"%05d-\t(p) %02X\n", GetTickCount() &0xFFFF, V);
LogFile(LK_SOUND, str);
#endif
					break;
		case 0x81:
					i=((V&0x07)<<8) +IO[0x80];
//					SetSoundFreq(0, i);
#ifdef SOUND_DEBUG
GPSPRINTF(str,"%05d-\t(P) %02X\n", GetTickCount() &0xFFFF, V);
LogFile(LK_SOUND, str);
#endif
					break;
		case 0x82:
#ifdef SOUND_DEBUG
GPSPRINTF(str,"%05d-\t\t(p) %02X\n", GetTickCount() &0xFFFF, V);
LogFile(LK_SOUND, str);
#endif
					break;
		case 0x83:
					i=((V&0x07)<<8) +IO[0x82];
//					SetSoundFreq(1, i);
#ifdef SOUND_DEBUG
GPSPRINTF(str,"%05d-\t\t(P) %02X\n", GetTickCount() &0xFFFF, V);
LogFile(LK_SOUND, str);
#endif
					break;
		case 0x84:
#ifdef SOUND_DEBUG
GPSPRINTF(str,"%05d-\t\t\t(p) %02X\n", GetTickCount() &0xFFFF, V);
LogFile(LK_SOUND, str);
#endif
					break;
		case 0x85:
					i=((V&0x07)<<8) +IO[0x84];
//					SetSoundFreq(2, i);
#ifdef SOUND_DEBUG
GPSPRINTF(str,"%05d-\t\t\t(P) %02X\n", GetTickCount() &0xFFFF, V);
LogFile(LK_SOUND, str);
#endif
					break;
		case 0x86:
#ifdef SOUND_DEBUG
GPSPRINTF(str,"%05d-\t\t\t\t(p) %02X\n", GetTickCount() &0xFFFF, V);
LogFile(LK_SOUND, str);
#endif
					break;
		case 0x87:
					i=((V&0x07)<<8) +IO[0x86];
//                    SetSoundFreq(5, i);
//                    SetSoundFreq(3, i);
#ifdef SOUND_DEBUG
GPSPRINTF(str,"%05d-\t\t\t\t(P) %02X\n", GetTickCount() &0xFFFF, V);
LogFile(LK_SOUND, str);
#endif
					break;
		case 0x88:
//					SetSoundPan(0, (V&0xF0)>>4, V&0x0F);
#ifdef SOUND_DEBUG
GPSPRINTF(str,"%05d-\t(v) %02X\n", GetTickCount() &0xFFFF, V);
LogFile(LK_SOUND, str);
#endif
					break;
		case 0x89:
        			if(!(SNDMOD&0x20))
                    {
//						SetSoundPan(1, (V&0xF0)>>4, V&0x0F);
                    }
#ifdef SOUND_DEBUG
GPSPRINTF(str,"%05d-\t\t(v) %02X\n", GetTickCount() &0xFFFF, V);
LogFile(LK_SOUND, str);
#endif
					break;
		case 0x8A:
//					SetSoundPan(2, (V&0xF0)>>4, V&0x0F);
#ifdef SOUND_DEBUG
GPSPRINTF(str,"%05d-\t\t\t(v) %02X\n", GetTickCount() &0xFFFF, V);
LogFile(LK_SOUND, str);
#endif
					break;
		case 0x8B:
//                    SetSoundPan(5, (V&0xF0)>>4, V&0x0F);
//                    SetSoundPan(3, (V&0xF0)>>4, V&0x0F);
#ifdef SOUND_DEBUG
GPSPRINTF(str,"%05d-\t\t\t\t(v) %02X\n", GetTickCount() &0xFFFF, V);
LogFile(LK_SOUND, str);
#endif
					break;
		case 0x8C:
					SwpStep=V;
#ifdef SOUND_DEBUG
GPSPRINTF(str,"%05d-\t\t\t(s) %02X\n", GetTickCount() &0xFFFF, V);
LogFile(LK_SOUND, str);
#endif
					break;
		case 0x8D:
					SwpTime=(V+1)<<5;
#ifdef SOUND_DEBUG
GPSPRINTF(str,"%05d-\t\t\t(t) %02X\n", GetTickCount() &0xFFFF, V);
LogFile(LK_SOUND, str);
#endif
					break;
		case 0x8E:
//					SetSoundPData(5, V&0x07);
#ifdef SOUND_DEBUG
GPSPRINTF(str,"%05d-\t\t\t\t(n) %02X\n", GetTickCount() &0xFFFF, V);
LogFile(LK_SOUND, str);
#endif
					break;
		case 0x8F:
					WaveMap=V<<6;
#ifdef SOUND_DEBUG
GPSPRINTF(str,"%05d-(m) %02X\n", GetTickCount() &0xFFFF, V);
LogFile(LK_SOUND, str);
#endif
					break;
		case 0x90:
					if((V&0x01)&&!(SNDMOD&0x01))
					{
//                        if(SoundOn[0])
//						WsSoundPlay(0);
					}
					else if(!(V&0x01)&&(SNDMOD&0x01))
					{
//						WsSoundStop(0);
					}

					if(((V&0x22)==0x02)&&((SNDMOD&0x22)!=0x02))
					{
//                       if(SoundOn[1])
//						WsSoundPlay(1);
					}
					else if(((V&0x22)!=0x02)&&((SNDMOD&0x22) ==0x02))
					{
//						WsSoundStop(1);
					}

					if((V&0x04)&&!(SNDMOD&0x04))
					{
//                       if(SoundOn[2])
//						WsSoundPlay(2);
					}
					else if(!(V&0x04)&&(SNDMOD&0x04))
					{
//						WsSoundStop(2);
					}

					if(((V&0x88)==0x08)&&((SNDMOD&0x88)!=0x08))
					{
//                       if(SoundOn[3])
//						WsSoundPlay(3);
					}
					else if(((V&0x88)!=0x08)&&((SNDMOD&0x88)==0x08))
					{
//						WsSoundStop(3);
					}

					if(((V&0x22)==0x22)&&((SNDMOD&0x22)!=0x22))
					{
						;
					}
					else if(((V&0x22)!=0x22)&&((SNDMOD&0x22)==0x22))
					{
						;
					}

					if(((V&0x88)==0x88)&&((SNDMOD&0x88)!=0x88))
					{
//                       if(SoundOn[4])
//						WsSoundPlay(5);
					}
					else if(((V&0x88)!=0x88)&&((SNDMOD&0x88)==0x88))
					{
//						WsSoundStop(5);
					}

#ifdef SOUND_DEBUG
GPSPRINTF(str,"%05d-(c) %02X\n", GetTickCount() &0xFFFF, V);
LogFile(LK_SOUND, str);
#endif
					break;
		case 0x91:
					MainVol=0;
					V|=0x80;
					break;
		case 0x92:
		case 0x93:break;
		case 0x94:
//					SetSoundPan(4, V&0x0F, V&0x0F);
#ifdef SOUND_DEBUG
GPSPRINTF(str,"%05d-(? ) %02X\n", GetTickCount() &0xFFFF, V);
LogFile(LK_SOUND, str);
#endif
					break;

		case 0xA0:V=0x02;break;

		case 0xA2:
					if(V&0x01)
					{
						HCNTL=HPREL;
						HCNTH=HPREH;
					}
					else
					{
						HCNTL=0;
						HCNTH=0;
					}
					if(V&0x04)
					{
						VCNTL=VPREL;
						VCNTH=VPREH;
					}
					else
					{
						VCNTL=0;
						VCNTH=0;
					}
					break;
		case 0xA4:
		case 0xA5:
		case 0xA6:
		case 0xA7:
        			IO[A+4]=V;
                    break;
		case 0xA8:
		case 0xA9:
		case 0xAA:
		case 0xAB:break;

		case 0xB0:break;
		case 0xB1:break;
		case 0xB2:break;
		case 0xB3:
        			if(V&0x20)
                    {
                    	V&=0xDF;
                    }
                        V|=0x04;
                    break;

		case 0xB4:break;
		case 0xB5:
					KEYCTL=(byte)(V&0xF0);
					if(KEYCTL&0x40) KEYCTL|=(byte)((JoyState>>8) &0x0F);
					if(KEYCTL&0x20) KEYCTL|=(byte)((JoyState>>4) &0x0F);
					if(KEYCTL&0x10) KEYCTL|=(byte)(JoyState&0x0F);
					return;
		case 0xB6:
					IRQACK&=(byte) ~V;
					return;
		case 0xB7:break;

		case 0xBA:
		case 0xBB:
		case 0xBC:
		case 0xBD:break;
		case 0xBE:
					ComEEP(&sIEep, IO+0xBC, IO+0xBA);
        			V>>=4;
        			break;

		case 0xC0:
                    if(nec_get_reg(NEC_CS)>=0x4000)
                    {
//					    nec_prefetch();		//명 탐정 코난서쪽의 이름 탐정 최대의 위기!?
                    }
					j=(V<<4) &0xF0;
    				Page[0x4]=ROMMap[0x4|j];
    				Page[0x5]=ROMMap[0x5|j];
    				Page[0x6]=ROMMap[0x6|j];
    				Page[0x7]=ROMMap[0x7|j];
    				Page[0x8]=ROMMap[0x8|j];
    				Page[0x9]=ROMMap[0x9|j];
    				Page[0xA]=ROMMap[0xA|j];
    				Page[0xB]=ROMMap[0xB|j];
    				Page[0xC]=ROMMap[0xC|j];
    				Page[0xD]=ROMMap[0xD|j];
    				Page[0xE]=ROMMap[0xE|j];
    				Page[0xF]=ROMMap[0xF|j];
					break;
		case 0xC1:
//					if(V>=RAMBanks) RAMEnable=0;
//					else Page[1]=SRAM;
					if(RAMEnable)
						Page[1]=SRAM;
					break;
		case 0xC2:
					Page[2]=ROMMap[V];
					break;
		case 0xC3:
					Page[3]=ROMMap[V];
					break;

		case 0xC4:
		case 0xC5:
		case 0xC6:
		case 0xC7:break;
		case 0xC8:
					ComEEP(&sCEep, IO+0xC6, IO+0xC4);
					if(V&0x10)
					{
						V>>=4;
					}
					if(V&0x20)
					{
						V>>=4;
					}
					if(V&0x40)
					{
						V>>=5;
					}
        			break;

		case 0xCA:V|=0x80;break;
		case 0xCB:
		case 0xCC:
		case 0xCD:
		case 0xCE:
		case 0xCF:break;
        
		case 0xD1:
		case 0xD3:
		case 0xD5:break;

		default:
    		//if(Verbose)
				//ErrorMsg("불명 레시″스튜디오의 기입");
     		break;
	}
    if(A>=0x100)
    {
    	return;
    }
	IO[A]=V;
}

WriteMemFn WriteMemFnTable[16]= {
	WriteIRam,
	WriteCRam,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
    WriteRom,
};

void WsReset (void)
{
	int i, j;

    Page[0x0]=IRAM;

	if(CartKind&CK_EEP)
	{
    	Page[0x1]=MemDummy;
		sCEep.pData=SRAM;
		sCEep.WrEnable=0;
	}
	else
	{
    	Page[0x1]=SRAM;
		sCEep.pData=NULL;
		sCEep.WrEnable=0;
	}

    Page[0xF]=ROMMap[0xFF];
    
	i=(SPRTAB&0x1F)<<9;
    i+=SPRBGN<<2;
    j=SPRCNT<<2;
    GPMEMCPY(SprTMap, IRAM+i, j);
    SprTTMap=SprTMap;
    SprETMap=SprTMap+j-4;
    WriteIO(0x07, 0x00);
    WriteIO(0x14, 0x01);
    for(i=0;i<0x210;i++)
    {
        SetPalette(i, 0);
    }

    WriteIO(0x1C, 0x99);
    WriteIO(0x1D, 0xFD);
    WriteIO(0x1E, 0xB7);
    WriteIO(0x1F, 0xDF);
    WriteIO(0x20, 0x30);
    WriteIO(0x21, 0x57);
    WriteIO(0x22, 0x75);
    WriteIO(0x23, 0x76);
    WriteIO(0x24, 0x15);
    WriteIO(0x25, 0x73);
    WriteIO(0x26, 0x77);
    WriteIO(0x27, 0x77);
    WriteIO(0x28, 0x20);
    WriteIO(0x29, 0x75);
    WriteIO(0x2A, 0x50);
    WriteIO(0x2B, 0x36);
    WriteIO(0x2C, 0x70);
    WriteIO(0x2D, 0x67);
    WriteIO(0x2E, 0x50);
    WriteIO(0x2F, 0x77);
    WriteIO(0x30, 0x57);
    WriteIO(0x31, 0x54);
    WriteIO(0x32, 0x75);
    WriteIO(0x33, 0x77);
    WriteIO(0x34, 0x75);
    WriteIO(0x35, 0x17);
    WriteIO(0x36, 0x37);
    WriteIO(0x37, 0x73);
    WriteIO(0x38, 0x50);
    WriteIO(0x39, 0x57);
    WriteIO(0x3A, 0x60);
    WriteIO(0x3B, 0x77);
    WriteIO(0x3C, 0x70);
    WriteIO(0x3D, 0x77);
    WriteIO(0x3E, 0x10);
    WriteIO(0x3F, 0x73);
    WriteIO(0x01, 0x00);

    WriteIO(0x8F, 0x03);
    WriteIO(0x91, 0x80);
    WriteIO(0xA0, 0x02);
    WriteIO(0xB3, 0x04);

    IO[0xC0]=0x0F;
    j=0xF0;
    Page[0x4]=ROMMap[0x4|j];
    Page[0x5]=ROMMap[0x5|j];
    Page[0x6]=ROMMap[0x6|j];
    Page[0x7]=ROMMap[0x7|j];
    Page[0x8]=ROMMap[0x8|j];
    Page[0x9]=ROMMap[0x9|j];
    Page[0xA]=ROMMap[0xA|j];
    Page[0xB]=ROMMap[0xB|j];
    Page[0xC]=ROMMap[0xC|j];
    Page[0xD]=ROMMap[0xD|j];
    Page[0xE]=ROMMap[0xE|j];
    Page[0xF]=ROMMap[0xF|j];
    WriteIO(0xC2, 0xFF);
    WriteIO(0xC3, 0xFF);
//  SetSoundFreq(4,0);
//  SetSoundFreq(4,1792);
	WaveMap=-1;
//	WsSoundClear(0);
//	WsSoundClear(1);
//	WsSoundClear(2);
//	WsSoundClear(3);
//	WsSoundClear(4);
//	WsSoundClear(5);

	JoyState=0x0000;

	nec_reset(NULL);
	nec_set_reg(NEC_SP, 0x2000);

// Some games require initialized RAM 
    IRAM[0x75AC]=0x41;
    IRAM[0x75AD]=0x5F;
    IRAM[0x75AE]=0x43;
    IRAM[0x75AF]=0x31;
    IRAM[0x75B0]=0x6E;
    IRAM[0x75B1]=0x5F;
    IRAM[0x75B2]=0x63;
    IRAM[0x75B3]=0x31;
}

WsRomHeader	*WsRomGetHeader(uint8 *wsrom, uint32 wsromSize)
{
	WsRomHeader *wsromHeader=(WsRomHeader *)GPMALLOC(sizeof(WsRomHeader));

	GPMEMCPY(wsromHeader,wsrom+(wsromSize-10),10);
	return(wsromHeader);
}

int WsCreate(uint8 *RomBase, uint32 romSize)
{
	int Checksum, i, j;
    uint8 b;

	char debugstring[512];


	for(i=0;i<0x100;i++)
	{
		ROMMap[i]=MemDummy;
//		RAMMap[i]=MemDummy;
	}

	GPMEMSET(IRAM, 0, sizeof(IRAM));
	GPMEMSET(MemDummy, 0xFF, sizeof(MemDummy));
	GPMEMSET(IO, 0, sizeof(IO));

	if(RomBase == NULL)
	{
		GPSPRINTF(debugstring, "RomBase = NULL");
		PrintMessage(debugstring,1);
		return -1;
	}

	WsromHeader = WsRomGetHeader(RomBase,romSize);

	CartKind=0;

	b = WsromHeader->romSize;
	
	ROMBanks=0;
	
	for(i=0;!ROMBanks && ROMBanksTable[i].Value;i++)
	{
		if(b==ROMBanksTable[i].Code)
		{
			ROMBanks=ROMBanksTable[i].Value;
		}
	}
	if(ROMBanks>64)
	{
		GPSPRINTF(debugstring,"Sorry, Games over 4mb not supported yet");
		PrintMessage(debugstring,1);
		return -1;
	}

	b = WsromHeader->ramSize;
	RAMSize=0;

	for(i=0;!RAMSize && RAMSizeTable[i].Value;i++)
	{
		if(b==RAMSizeTable[i].Code)
		{
			RAMSize=RAMSizeTable[i].Value;
			RAMEnable = 1;
		}
	}

	for(i=0;!(CartKind&CK_EEP) && RAMKindTable[i]. Value;i++)
	{
		if(b==RAMKindTable[i].Code)
		{
			CartKind=RAMKindTable[i].Value;
		}
	}

    if((WsromHeader->developerId==0x01)&&(WsromHeader->cartId==0x16)) //STAR HEARTS ∼별과 다이치의 사자∼
    {
        RAMSize=0x8000;
        CartKind=NULL;
    }

	for(i=ROMBanks-1;i>=0;i--)
	{
		ROMMap[0x100-ROMBanks+i] = &RomBase[0x10000 * i];
	}

	GPMEMSET(SRAM,0x00,0x10000);
	
	WsReset ();

	vert = WsromHeader->additionalCapabilities & 1; // set if the game is vertical

	RomCRC = WsromHeader->checksum;

	return 0;
}

#define MacroStoreNecRegisterToMem(M,R)        \
		M->R = nec_get_reg(R);

void saveStateToMem(gameState *tempState)
{
	int		i;
	uint32	PageSave[16];

	MacroStoreNecRegisterToMem(tempState,NEC_IP);
	MacroStoreNecRegisterToMem(tempState,NEC_AW);
	MacroStoreNecRegisterToMem(tempState,NEC_BW);
	MacroStoreNecRegisterToMem(tempState,NEC_CW);
	MacroStoreNecRegisterToMem(tempState,NEC_DW);
	MacroStoreNecRegisterToMem(tempState,NEC_CS);
	MacroStoreNecRegisterToMem(tempState,NEC_DS);
	MacroStoreNecRegisterToMem(tempState,NEC_ES);
	MacroStoreNecRegisterToMem(tempState,NEC_SS);
	MacroStoreNecRegisterToMem(tempState,NEC_IX);
	MacroStoreNecRegisterToMem(tempState,NEC_IY);
	MacroStoreNecRegisterToMem(tempState,NEC_BP);
	MacroStoreNecRegisterToMem(tempState,NEC_SP);
	MacroStoreNecRegisterToMem(tempState,NEC_FLAGS);
	MacroStoreNecRegisterToMem(tempState,NEC_VECTOR);
	MacroStoreNecRegisterToMem(tempState,NEC_PENDING);
	MacroStoreNecRegisterToMem(tempState,NEC_NMI_STATE);
	MacroStoreNecRegisterToMem(tempState,NEC_IRQ_STATE);

	GPMEMCPY(tempState->IRAM,		IRAM,		0x10000);
	GPMEMCPY(tempState->SRAM,		SRAM,		0x10000);
	GPMEMCPY(tempState->IORam,		IO,			0x00100);
	GPMEMCPY(tempState->MonoColor,	MonoColor,	8);
	GPMEMCPY(tempState->ColTbl,		ColTbl,		0x210);
	GPMEMCPY(tempState->Palette,	Palette,	(16+1)*16*sizeof(uint16));

	for (i = 0; i < 16; i++)
		PageSave[i] = (uint32)(Page[i] - base_rom);

	GPMEMCPY(tempState->Page,		PageSave,	16*sizeof(uint32));

	tempState->RSTRL = RSTRL;
}

#define MacroLoadNecRegisterFromMem(M,R)        \
	    nec_set_reg(R,M->R);

void loadStateFromMem(gameState *tempState)
{
	int		i;

	uint32	PageSave[16];

	char	debugstring[512];

	MacroLoadNecRegisterFromMem(tempState,NEC_IP);
	MacroLoadNecRegisterFromMem(tempState,NEC_AW);
	MacroLoadNecRegisterFromMem(tempState,NEC_BW);
	MacroLoadNecRegisterFromMem(tempState,NEC_CW);
	MacroLoadNecRegisterFromMem(tempState,NEC_DW);
	MacroLoadNecRegisterFromMem(tempState,NEC_CS);
	MacroLoadNecRegisterFromMem(tempState,NEC_DS);
	MacroLoadNecRegisterFromMem(tempState,NEC_ES);
	MacroLoadNecRegisterFromMem(tempState,NEC_SS);
	MacroLoadNecRegisterFromMem(tempState,NEC_IX);
	MacroLoadNecRegisterFromMem(tempState,NEC_IY);
	MacroLoadNecRegisterFromMem(tempState,NEC_BP);
	MacroLoadNecRegisterFromMem(tempState,NEC_SP);
	MacroLoadNecRegisterFromMem(tempState,NEC_FLAGS);
	MacroLoadNecRegisterFromMem(tempState,NEC_VECTOR);
	MacroLoadNecRegisterFromMem(tempState,NEC_PENDING);
	MacroLoadNecRegisterFromMem(tempState,NEC_NMI_STATE);
	MacroLoadNecRegisterFromMem(tempState,NEC_IRQ_STATE);

	GPMEMCPY(IRAM,		tempState->IRAM,		0x10000);
	GPMEMCPY(SRAM,		tempState->SRAM,		0x10000);
	GPMEMCPY(IO,		tempState->IORam,		0x00100);
	GPMEMCPY(MonoColor,	tempState->MonoColor,	8);
	GPMEMCPY(ColTbl,	tempState->ColTbl,		0x210);
	GPMEMCPY(Palette,	tempState->Palette,		(16+1)*16*sizeof(uint16));
	GPMEMCPY(PageSave,	tempState->Page,		16*sizeof(uint32));

	for (i = 2; i < 16; i++)
		Page[i] = (uint8*)(base_rom + PageSave[i]);

    Page[0x0]=IRAM;

	if(CartKind&CK_EEP)
	{
    	Page[0x1]=MemDummy;
		sCEep.pData=SRAM;
		sCEep.WrEnable=0;
	}
	else
	{
    	Page[0x1]=SRAM;
		sCEep.pData=NULL;
		sCEep.WrEnable=0;
	}

	RSTRL = tempState->RSTRL;
}

int	WsSaveState(char *statepath)
{
	F_HANDLE	F;
	uint32		result;
	uint16		crc	=	RomCRC;
	unsigned	value;
	char		newPath[1024];
	gameState	*saveState;
	
	GPSPRINTF(newPath,"%s",statepath);

	result=GpFileCreate(newPath,ALWAYS_CREATE,&F);

	if (result != SM_OK)
		return (-1);

	saveState = GPMALLOC(sizeof(gameState));

	if (saveState == NULL)
	{
		GpFileClose(F);
		return (-1);
	}

	saveStateToMem(saveState);
	GpFileWrite(F,&crc,sizeof(crc));
	GpFileWrite(F,saveState,sizeof(gameState));
	GPFREE(saveState);
	GpFileClose(F);
	return (1);
}

int	WsLoadState(char *statepath)
{
	F_HANDLE	F;
	uint32		result;
	ulong		sizeread;

	uint16		crc		=	RomCRC;
	uint16		newCrc;

	gameState	*saveState;

	result = GpFileOpen(statepath,OPEN_R, &F);
	
	if (result != SM_OK)
		return(-1);

	GpFileRead(F, &newCrc, sizeof(newCrc), &sizeread);

	if (newCrc!=crc)
	{
		GpFileClose(F);
		return(-1);
	}

	saveState	=	GPMALLOC(sizeof(gameState));

	if (saveState == NULL)
	{
		GpFileClose(F);
		return(-1);
	}

	GpFileRead(F, saveState, sizeof(gameState), &sizeread);
	GpFileClose(F);

	loadStateFromMem(saveState);

	GPFREE(saveState);

	// force a video mode change to make all tiles dirty
	//	ws_gpu_clearCache();
	return(1);
}

void WsSaveRam(char *SaveName)
{
	ERR_CODE 	result;
	F_HANDLE	F;
	int 		i;

	if(SaveName[0]!='\0' && RAMSize)
	{
		result=GpFileCreate(SaveName,ALWAYS_CREATE,&F);
		
		if (result==SM_OK)
		{
//			for(i=0;i<RAMBanks;i++)
//			{
				if(RAMSize<0x10000)
				{	
	          		GpFileWrite(F, SRAM,RAMSize);
				}
				else
				{
	          		GpFileWrite(F, SRAM,0x10000);
				}
//			}
			GpFileClose(F);
		}
	}
}

int WsLoadRam(char *SaveName)
{
	ERR_CODE 	result;
	F_HANDLE	F;
	ulong 		sizeread;
	int 		i;
	char 		debugstring[512];
	
    result = GpFileOpen(SaveName,OPEN_R, &F);

	if(result == SM_OK)
    {
//		for(i=0;i<RAMBanks;i++)
//		{
			if(RAMSize<0x10000)
			{
				GpFileRead(F, SRAM,RAMSize,&sizeread);
				
        		if(sizeread != RAMSize)
        		{
					GpFileClose(F);
					PrintMessage("ERROR: sizeread != RAMSIZE (< 0x10000)",1);
					// MessageBox(NULL,"세이프″파일이 이상","에러",MB_ICONEXCLAMATION | MB_OK);
					return -1;
        		}
			}
			else
			{
				GpFileRead(F, SRAM,0x10000,&sizeread);
				
        		if(sizeread != 0x10000)
        		{
					GpFileClose(F);
					PrintMessage("ERROR: sizeread != RAMSIZE (0x10000)",1);
					// MessageBox(NULL,"세이프″파일이 이상","에러",MB_ICONEXCLAMATION | MB_OK);
					return -1;
        		}
			}
//		}
		GpFileClose(F);
	}
}

void WsRelease(void)
{
	F_HANDLE *F;
	int i;

//    WsSoundStop(4);

//	for(i=0;i<RAMBanks;i++)
//	if (RAMEnable)
//		GPFREE(SRAM);

	for(i=0xFF;i>=0;i--)
	{
    	if(ROMMap[i]==MemDummy)
        {
        	break;
        }
		ROMMap[i]=MemDummy;
	}
}

int selectPressed = 0;
int savestateMode = 0;

int Interrupt(void)
{
	static int LCount=0, Joyz=0x0000;
	int i, j;
    byte b;

	char debugstring[512];

	if(++LCount>=8)
	{
		LCount=0;
	}

	switch(LCount)
	{
		case 0:
			if(RSTRL==144)
			{
				selectPressed=DoKeys(&JoyState);
 			
				KEYCTL&=(byte) 0xF0;
				if(KEYCTL&0x40) KEYCTL|=(byte)((JoyState>>8) &0x0F);
				if(KEYCTL&0x20) KEYCTL|=(byte)((JoyState>>4) &0x0F);
				if(KEYCTL&0x10) KEYCTL|=(byte)(JoyState&0x0F);
				if((JoyState^Joyz) &Joyz)
				{
					if(IRQENA&KEY_IFLAG)
					{
	                	IRQACK|=KEY_IFLAG;
					}
				}
				Joyz=JoyState;
			}

			break;
		case 2:
			/* Skip Sound Interrupts for now
			i=WsSoundInt();
            PCSRL=(byte)(i&0xFF);
            PCSRH=(byte)((i>>8) &0xFF);
            if((SDMACTL&0x88) ==0x80) //STAR HEARTS 금지 패치
            {
                i=(SDMACH<<8) |SDMACL;
                j=(SDMASB<<16)|(SDMASH<<8) |SDMASL;
                b=cpu_readmem20(j);
                if(!SoundOn[6])
                    b=0x80;
                IO[0x89]=b;
                i--;
                j++;
                if(i<32) //HUNTER×HUNTER ∼각각의 결의∼
                {
                    i=0;
                    SDMACTL&=0x7F;
                }
                SDMASB=(byte)((j>>16) &0xFF);
                SDMASH=(byte)((j>>8) &0xFF);
                SDMASL=(byte)(j&0xFF);
                SDMACH=(byte)((i>>8) &0xFF);
                SDMACL=(byte)(i&0xFF);
            }
            else if((SNDMOD&0x22) ==0x22)
            {
                b=IO[0x89];
                if(!SoundOn[5])
                    b=0x80;
            }
            else
            {
                b=0x80;
            }
#ifdef PCM_DEBUG
GPSPRINTF(str,"%d\n", b);
LogFile(LK_SOUND, str);
#endif
            b>>=1;
            b+=0x40;
            if(b>0xAA)
            {
                b=0xAA;
            }
            else if(b<0x55)
            {
                b=0x55;
            }
		       SetPCM(b);
		*/
            break;
		case 4:
            if(RSTRL==140)
            {
				i=(SPRTAB&0x1F)<<9;
                i+=SPRBGN<<2;
                j=SPRCNT<<2;
                GPMEMCPY(SprTMap, IRAM+i, j);
                SprTTMap=SprTMap;
                SprETMap=SprTMap+j-4;
            }

        	if(LCDSLP&0x01)
            {
                if(RSTRL==0)
                {
                    SkipCnt--;

					if (SkipCnt < 0)
	                    SkipCnt=FrameSkip;
                }

				if(SkipCnt == 0)
                {
					if(RSTRL<144)
					{
	                	WsDrawLine((int) RSTRL);
					}
					if(RSTRL==144)
					{
	                	WsDrawFlip();
					}
                }
			}
            break;
		case 6:
			i=(HCNTH<<8) +HCNTL;
            j=(HPREH<<8) +HPREL;
			if((TIMCTL&0x01) &&i)
			{
				i--;
				if(!i)
				{
					if(TIMCTL&0x02)
					{
						i=j;
					}
					if(IRQENA&HTM_IFLAG)
					{
						IRQACK|=HTM_IFLAG;
					}
				}
				HCNTH=(byte)(i>>8);
				HCNTL=(byte)(i&0xFF);
			}
            else if(j==1)
            {
                if(IRQENA&HTM_IFLAG)
                {
                    IRQACK|=HTM_IFLAG;
                }
            }

			if((IRQENA&VBB_IFLAG)&&(RSTRL==144))
			{
				IRQACK|=VBB_IFLAG;
			}

			i=(VCNTH<<8) +VCNTL;
            j=(VPREH<<8) +VPREL;
			if((TIMCTL&0x04)&&(RSTRL==144) &&i)
			{
				i--;
				if(!i)
				{
					if(TIMCTL&0x08)
					{
						i=j;
					}
					if(IRQENA&VTM_IFLAG)
					{
						IRQACK|=VTM_IFLAG;
					}
				}
				VCNTH=(byte)(i>>8);
				VCNTL=(byte)(i&0xFF);
			}

			if((IRQENA&RST_IFLAG)&&(RSTRL==RSTRLC))
			{
				IRQACK|=RST_IFLAG;
			}

			break;
		case 7: // Increase scanline
            RSTRL++;
			if(RSTRL>=159)
			{
				RSTRL=0;
			}

            break;
		default:
        	break;
	}

	return IRQACK;
}

int WsRun(void)
{
	static int period=32; //=IPeriod;
	int i, j, n, m;
	selectPressed = 0;

	for(i=0;i<480;i++) //5ms
//	for(i=0;i<1280;i++) // 1/75s
//	for(i=0;i<1440;i++) // 15ms
	{
		j=nec_execute(period);
        period+=IPeriod-j;
 		if(Interrupt())
		{
			n=IRQACK;
			for(m=7;m>=0;m--)
			{
				if(n&0x80)
				{
					break;
				}
			 	n<<=1;
			}
			m+=IRQBSE;
			nec_int(m<<2);
		}

		if (selectPressed > 0)
		{
			return selectPressed;
		}
	}
	return 0;
}
