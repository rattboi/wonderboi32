//---------------------------------------------------------------------------
#ifndef WSDrawH
#define WSDrawH
#include "WSHard.h"
//---------------------------------------------------------------------------
extern byte 	*Scr1TMap;
extern byte 	*Scr2TMap;
extern byte 	*SprTTMap;
extern byte 	*SprETMap;
extern byte 	SprTMap[512];

extern byte ColTbl[0x210];
extern byte Palette[16+1][16][4];

extern byte 	*BGndTPal;
extern byte 	*WndTPal;
extern int 		MaxLine;
extern int 		DrawLine;

extern int 		DispHeight;
extern int 		DispWidth;
extern int 		PixelDepth;

extern int		WsDrawCreate();

extern  void  	WsDrawRelease(void);

extern  int  	SetDrawMode(int Mode);
extern  int  	GetDrawMode(void);

extern  int  	WsDrawLine(int Line);
extern  int  	WsDrawFlip(void);
extern  void  	WsDrawClear(void);

extern  void  	SetPalette(int Index, byte PalData);

#endif
