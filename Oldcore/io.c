////////////////////////////////////////////////////////////////////////////////
// I/O ports
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////

#include "types.h"
#include "gpstdlib.h"

#include "rom.h"
#include "./nec/nec.h"
#include "initialio.h"
#include "ieeprom.h"
#include "gpu.h"

extern	uint8	*externalEeprom;
extern	uint32	externalEepromAddressMask;
extern	uint32	romAddressMask;

uint8	*ws_ioRam=NULL;

/*
uint8	ws_key_start;
uint8	ws_key_left;
uint8	ws_key_right;
uint8	ws_key_up;
uint8	ws_key_down;
uint8	ws_key_button_1;
uint8	ws_key_button_2;
*/

uint8	ws_key_flipped;


uint8 ws_keys[11];

int		rtcDataRegisterReadCount=0;

#define WS_KEY_H_UP		0
#define WS_KEY_H_DOWN	1
#define WS_KEY_H_LEFT	2
#define WS_KEY_H_RIGHT	3

#define WS_KEY_V_UP		4
#define WS_KEY_V_DOWN	5
#define WS_KEY_V_LEFT	6
#define WS_KEY_V_RIGHT	7

#define WS_KEY_1		8
#define WS_KEY_2		9
#define WS_KEY_START	10

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
void ws_io_reset(void)
{
	int i;

	for (i = 0; i < 11; i++)
		ws_keys[i] = 0;

	for (i=0;i<0x100;i++)
		ws_ioRam[i]=initialIoValue[i];
	for (i=0;i<0xc9;i++)
		cpu_writeport(i,initialIoValue[i]);

	rtcDataRegisterReadCount=0;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
void ws_io_init(void)
{
	if (ws_ioRam==NULL)
		ws_ioRam=(uint8*)GPMALLOC(0x100);

	ws_io_reset();
	ws_key_flipped=0;
}

void ws_io_flipControls(void)
{
	ws_key_flipped=!ws_key_flipped;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
void ws_io_done(void)
{
	if (ws_ioRam!=NULL)
	{
		GPFREE(ws_ioRam);
		ws_ioRam = NULL;
	}
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
BYTE cpu_readport(BYTE port)
{
	int w1,w2;

	switch (port)
	{
	case 0x4e:
	case 0x4f:
	case 0x50:
	case 0x51:
	case 0x80:
	case 0x81:
	case 0x82:
	case 0x83:
	case 0x84:
	case 0x85:
	case 0x86:
	case 0x87:
	case 0x88:
	case 0x89:
	case 0x8a:
	case 0x8b:
	case 0x8c:
	case 0x8d:
	case 0x8e:
	case 0x8f:
	case 0x90:
	case 0x91:
	case 0x92:
	case 0x93:
	case 0x94:  return ws_ioRam[port];
	case 0xaa:	return 0xff;
	case 0xb3:	// ???
				if (ws_ioRam[0xb3]<0x80)
					return 0;

				if (ws_ioRam[0xb3]<0xc0)
					return 0x84;

				return 0xc4;
	case 0xb5:	w1=ws_ioRam[0xb5];
				if(w1&0x40)
				{
					w2=0x00;
/*					if (ws_key_flipped)
						w2=(ws_key_start<<1);
					else
						w2=(ws_key_start<<1)|(ws_key_button_1<<2)|(ws_key_button_2<<3);
*/
					w2 = (ws_keys[WS_KEY_START]	<<1)|
						 (ws_keys[WS_KEY_1]		<<2)|
						 (ws_keys[WS_KEY_2]		<<3);
					return (uint8)((w1&0xf0)|w2);
				}
				if(w1&0x20)
				{
					w2=0x00;
/*					if (ws_key_flipped)
						w2=(ws_key_button_1)|(ws_key_button_2<<2);
					else
						w2=(ws_key_up<<0)|(ws_key_right<<1)|(ws_key_down<<2)|(ws_key_left<<3);
*/
					w2 = (ws_keys[WS_KEY_H_UP]		<<0)|
						 (ws_keys[WS_KEY_H_RIGHT]	<<1)|
						 (ws_keys[WS_KEY_H_DOWN]	<<2)|
						 (ws_keys[WS_KEY_H_LEFT]	<<3);
					return (uint8)((w1&0xf0)|w2);
				}
				if(w1&0x10)
				{
					w2=0x00;
/*					if (ws_key_flipped)
						w2=(ws_key_up<<1)|(ws_key_right<<2)|(ws_key_down<<3)|(ws_key_left);
*/
					w2 = (ws_keys[WS_KEY_V_UP]		<<1)|
						 (ws_keys[WS_KEY_V_RIGHT]	<<2)|
						 (ws_keys[WS_KEY_V_DOWN]	<<3)|
						 (ws_keys[WS_KEY_V_LEFT]	<<0);
					
					return (uint8)((w1&0xf0)|w2);
				}
			 break;
	case 0xbe:	// internal eeprom status/command register
				// ack eeprom write
				 if(ws_ioRam[0xbe]&0x20)
					 return ws_ioRam[0xbe]|2;

				 // ack eeprom read
				 if(ws_ioRam[0xbe]&0x10)
					 return ws_ioRam[0xbe]|1;

				 // else ack both
				 return ws_ioRam[0xbe]|3;
	case 0xba:	// eeprom even byte read
				w1=((((uint16)ws_ioRam[0xbd])<<8)|((uint16)ws_ioRam[0xbc]));
				w1=(w1<<1)&0x3ff;
				return internalEeprom[w1];
	case 0xbb:	// eeprom odd byte read
				w1=((((uint16)ws_ioRam[0xbd])<<8)|((uint16)ws_ioRam[0xbc]));
				w1=((w1<<1)+1)&0x3ff;
				return internalEeprom[w1];
	case 0xc0 : // ???
				return ((ws_ioRam[0xc0]&0xf)|0x20);

	case 0xc4:	// external eeprom even byte read
				w1=(((((WORD)ws_ioRam[0xc7])<<8)|((WORD)ws_ioRam[0xc6]))<<1)&(externalEepromAddressMask);
				return externalEeprom[w1];

	case 0xc5:	// external eeprom odd byte read
				w1=(((((WORD)ws_ioRam[0xc7])<<8)|((WORD)ws_ioRam[0xc6]))<<1)&(externalEepromAddressMask);
				return externalEeprom[w1+1];

	case 0xc8:	// external eeprom status/command register
				// ack eeprom write
				 if(ws_ioRam[0xc8]&0x20)
					 return ws_ioRam[0xc8]|2;

				 // ack eeprom read
				 if(ws_ioRam[0xc8]&0x10)
					 return ws_ioRam[0xc8]|1;

				 // else ack both
				 return ws_ioRam[0xc8]|3;

	case 0xca : // RTC Command and status register
				// set ack to always 1
				return (ws_ioRam[0xca]|0x80);
	case 0xcb : // RTC data register



		  if(ws_ioRam[0xca]==0x15)	// get time command
				{
					//struct tm *newtime;
					//time_t long_time;
					//time( &long_time );
					//newtime = localtime( &long_time );

					#define  BCD(value) ((value/10)<<4)|(value%10)

					switch(rtcDataRegisterReadCount)
					{
					  case 0:	rtcDataRegisterReadCount++;
								return BCD(3);//newtime->tm_year-100);
					  case 1:	rtcDataRegisterReadCount++;
								return BCD(3);//newtime->tm_mon);
					  case 2:	rtcDataRegisterReadCount++;
								return BCD(3);//newtime->tm_mday);
					  case 3:	rtcDataRegisterReadCount++;
								return BCD(3);//newtime->tm_wday);
					  case 4:	rtcDataRegisterReadCount++;
								return BCD(3);//newtime->tm_hour);
					  case 5:	rtcDataRegisterReadCount++;
								return BCD(3);//newtime->tm_min);
					  case 6:	rtcDataRegisterReadCount=0;
								return BCD(3);//newtime->tm_sec);
					}
					return 0;
				}
				else
				{
					// set ack
					return (ws_ioRam[0xcb]|0x80);
				}
	}
	return(ws_gpu_port_read(port));
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////
void cpu_writeport(DWORD port,BYTE value)
{
	int w1;

	if (ws_ioRam[port]==value)
		return;

	ws_ioRam[port]=value;

	switch (port)
	{
	case 0x48:	// DMA

				// bit 7 set to start dma transfer
				if(value&0x80)
				{
					int dma_start = (((DWORD)ws_ioRam[0x41])<<8)|(((DWORD)ws_ioRam[0x40]))|(((DWORD)ws_ioRam[0x42])<<16);
					int dma_end   = (((DWORD)ws_ioRam[0x45])<<8)|(((DWORD)ws_ioRam[0x44]))|(((DWORD)ws_ioRam[0x43])<<16);
					int dma_size  = (((DWORD)ws_ioRam[0x47])<<8)|(((DWORD)ws_ioRam[0x46]));
					int ix;

					for(ix=0;ix<dma_size;ix++)
						cpu_writemem20(dma_end++,cpu_readmem20(dma_start++));

					ws_ioRam[0x47]=0;
					ws_ioRam[0x46]=0;
					ws_ioRam[0x41]=(BYTE)(dma_start>>8);
					ws_ioRam[0x40]=(BYTE)(dma_start&0xff);
					ws_ioRam[0x45]=(BYTE)(dma_end>>8);
					ws_ioRam[0x44]=(BYTE)(dma_end&0xff);
					ws_ioRam[0x48]=0;
				}
				break;

   case 0xba: w1=(((((WORD)ws_ioRam[0xbd])<<8)|((WORD)ws_ioRam[0xbc])));
			  w1=(w1<<1)&0x3ff;
			  internalEeprom[w1]=value;
			  return;

	case 0xbb: w1=(((((WORD)ws_ioRam[0xbd])<<8)|((WORD)ws_ioRam[0xbc])));
			   w1=((w1<<1)+1)&0x3ff;
			   internalEeprom[w1]=value;
			   return;

	case 0xc4: w1=(((((WORD)ws_ioRam[0xc7])<<8)|((WORD)ws_ioRam[0xc6]))<<1)&externalEepromAddressMask;
			   externalEeprom[w1]=value;
			   return;

	case 0xc5: w1=(((((WORD)ws_ioRam[0xc7])<<8)|((WORD)ws_ioRam[0xc6]))<<1)&externalEepromAddressMask;
			   externalEeprom[w1+1]=value;
			   return;

	case 0xca:	if(value==0x15)
					rtcDataRegisterReadCount=0;
				break;
	}
	ws_gpu_port_write(port,value);
}
