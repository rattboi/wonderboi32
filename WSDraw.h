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

extern byte		ColTbl[0x210];
extern uint32	Palette[256];

extern byte 	*BGndTPal;
extern byte 	*WndTPal;
extern int 		MaxLine;
extern int 		DrawLine;

extern float	gammaC;

extern uint8	*ws_modified_tile;
extern uint8	*wsc_modified_tile;

extern int		WsDrawCreate();

extern  void  	WsDrawRelease(void);

extern  void  	SetDrawMode(int Mode);
extern  int  	GetDrawMode(void);

extern  int  	WsDrawLine(int Line);
extern  int  	WsDrawFlip(void);
extern  void  	WsDrawClear(void);
extern  void	WsClearGpuCache(void);

extern  void  	SetPalette(int Index, byte PalData);

extern void (*RefreshLine)			(int Line, void *lpSurface);
extern void (*renderLine[])			(int Line, void *lpSurface);

#endif
