//---------------------------------------------------------------------------
#include <gpstdlib.h>
#include <gpstdio.h>
#include <gpgraphic.h>
#include <gpmain.h>
#include <gpfont.h>

#include "WSInput.h"
#include "WSHard.h"

#include "types.h"

//---------------------------------------------------------------------------
#define JOYMAX 2

int KeyStat;
int JoyOn;
uint32 Joy;
uint32 ThRight;
uint32 ThLeft;
uint32 ThUp;
uint32 ThDown;
uint32 NumButton;

char KeyConfig[24][64];
char JoyConfig[24][64];

int KeyMap[256];
int JoyMap[16];

struct CPTR2INT
{
	char *Cptr;
	int Code;
};

const static struct CPTR2INT KeyAddr[] = {
	{ "BS",		0x08},
	{ "RETURN",	0x0D},
	{ "SHIFT",	0x10},
	{ "CONTROL", 0x11},
	{ "SPACE",	0x20},
	{ "LEFT",	0x25},
	{ "UP",		0x26},
	{ "RIGHT",	0x27},
	{ "DOWN",	0x28},
	{ "0",		0x30},
	{ "1",		0x31},
	{ "2",		0x32},
	{ "3",		0x33},
	{ "4",		0x34},
	{ "5",		0x35},
	{ "6",		0x36},
	{ "7",		0x37},
	{ "8",		0x38},
	{ "9",		0x39},
	{ "A",		0x41},
	{ "B",		0x42},
	{ "C",		0x43},
	{ "D",		0x44},
	{ "E",		0x45},
	{ "F",		0x46},
	{ "G",		0x47},
	{ "H",		0x48},
	{ "I",		0x49},
	{ "J",		0x4A},
	{ "K",		0x4B},
	{ "L",		0x4C},
	{ "M",		0x4D},
	{ "N",		0x4E},
	{ "O",		0x4F},
	{ "P",		0x50},
	{ "Q",		0x51},
	{ "R",		0x52},
	{ "S",		0x53},
	{ "T",		0x54},
	{ "U",		0x55},
	{ "V",		0x56},
	{ "W",		0x57},
	{ "X",		0x58},
	{ "Y",		0x59},
	{ "Z",		0x5A},
	{ ":",		0xBA},
	{ ";",		0xBB},
	{ "-",		0xBD},
	{ ". ",		0xBE},
	{ "/",		0xBF},
	{ "@",		0xC0},
	{ "[",		0xDB},
	{ "\\",		0xDC},
	{ "]",		0xDD},
	{ "^",		0xDE},
	{ NULL, NULL	}
};

const static struct CPTR2INT JoyAddr[] = {
	{ "LEFT",	0x01},
	{ "RIGHT",	0x02},
	{ "UP",		0x03},
	{ "DOWN",	0x04},
	{ "1",	0x05},
	{ "2",	0x06},
	{ "3",	0x07},
	{ "4",	0x08},
	{ "5",	0x09},
	{ "6",	0x0A},
	{ "7",	0x0B},
	{ "8",	0x0C},
	{ "9",	0x0D},
	{ "10",	0x0E},
	{ "11",	0x0F},
	{ NULL, NULL	}
};

//---------------------------------------------------------------------------
int WsInputInit(void)
{
	int result;
    uint32 JoyId[1]={0};
    uint32 min, max, value;
    int i;

    KeyStat=0x0000;

    JoyOn=0;
    Joy=NULL;


}

//---------------------------------------------------------------------------
int Joystick(void)
{
	int value;
	/*
	MMRESULT result;
    int value;

    if(JoyOn==0)
    {
        return KeyStat;
    }

	result=joyGetPosEx(Joy,&JIEX);
    if(result!=JOYERR_NOERROR)
    {
        return KeyStat;
    }

    value=0x0000;
    if(JIEX.dwXpos<=ThLeft)
    {
    	value|=JoyMap[0x01];
    }
    if(JIEX.dwXpos>=ThRight)
    {
    	value|=JoyMap[0x02];
    }
    if(JIEX.dwYpos<=ThUp)
    {
    	value|=JoyMap[0x03];
    }
    if(JIEX.dwYpos>=ThDown)
    {
    	value|=JoyMap[0x04];
    }
    if(JIEX.dwButtons&JOY_BUTTON1)
    {
    	value|=JoyMap[0x05];
    }
    if(JIEX.dwButtons&JOY_BUTTON2)
    {
    	value|=JoyMap[0x06];
    }
    if(JIEX.dwButtons&JOY_BUTTON3)
    {
    	value|=JoyMap[0x07];
    }
    if(JIEX.dwButtons&JOY_BUTTON4)
    {
    	value|=JoyMap[0x08];
    }
    if(JIEX.dwButtons&JOY_BUTTON5)
    {
    	value|=JoyMap[0x09];
    }
    if(JIEX.dwButtons&JOY_BUTTON6)
    {
    	value|=JoyMap[0x0A];
    }
    if(JIEX.dwButtons&JOY_BUTTON7)
    {
    	value|=JoyMap[0x0B];
    }
    if(JIEX.dwButtons&JOY_BUTTON8)
    {
    	value|=JoyMap[0x0C];
    }
    if(JIEX.dwButtons&JOY_BUTTON9)
    {
    	value|=JoyMap[0x0D];
    }
    if(JIEX.dwButtons&JOY_BUTTON10)
    {
    	value|=JoyMap[0x0E];
    }
    if(JIEX.dwButtons&JOY_BUTTON11)
    {
    	value|=JoyMap[0x0F];
    }
	*/
	
	//do input shit here

    return KeyStat|value;
}

//---------------------------------------------------------------------------
void WsKeyDown(short int Key)
{
	KeyStat|=KeyMap[Key&0xFF];
}

//---------------------------------------------------------------------------
void WsKeyUp(short int Key)
{
	KeyStat&=~KeyMap[Key&0xFF];
}

//---------------------------------------------------------------------------
void SetKeyMap(int Mode)
{
    char *src, *pos;
    char dest[64];
	int i, j, index;

	index=Mode? 12:0;
    memset(KeyMap, 0, sizeof(KeyMap));

    for(i=0;i<12;i++)
    {
    	src=KeyConfig[i+index];
        while(1)
        {
    		if(src==NULL)
    		{
    			break;
    		}
    		pos=strchr(src, ', ');
    		if(pos)
    		{
    			strncpy(dest, src, pos-src);
                dest[pos-src]='\0';
    			src=pos+1;
    		}
    		else
    		{
    			strcpy(dest, src);
    			src=NULL;
    		}

    		for(j=0;KeyAddr[j]. Cptr;j++)
    		{
        		if(!strcmp(dest, KeyAddr[j]. Cptr))
        		{
        			break;
        		}
    		}
    		if(KeyAddr[j]. Cptr)
    		{
    			KeyMap[KeyAddr[j]. Code]|=0x01<<i;
    		}
        }
    }

	if(!JoyOn)
	{
		return;
	}

    memset(JoyMap,0,sizeof(JoyMap));

    for(i=0;i<12;i++)
    {
    	src=JoyConfig[i+index];
        while(1)
        {
    		if(src==NULL)
    		{
    			break;
    		}
    		pos=strchr(src,',');
    		if(pos)
    		{
    			strncpy(dest,src,pos-src);
                dest[pos-src]='\0';
    			src=pos+1;
    		}
    		else
    		{
    			strcpy(dest,src);
    			src=NULL;
    		}

    		for(j=0;JoyAddr[j].Cptr;j++)
    		{
        		if(!strcmp(dest,JoyAddr[j].Cptr))
        		{
        			break;
       			}
    		}
    		if(JoyAddr[j].Cptr)
    		{
    			JoyMap[JoyAddr[j].Code]|=0x01<<i;
    		}
		}
    }
}

//---------------------------------------------------------------------------

