#ifndef __file_dialog_h__
#define __file_dialog_h__

#include "types.h"

void WindowInit();
void DrawWindow(int x, int y, int lx, int ly, int giSurface, uint16 color, uint16 col2);
void Cls(int giSurface);
void DrawMessage(char *s, int nflip);
void PrintMessage(char* string, int wait);
void DrawError(char *s, int nflip);
void PrintError(char* string, int wait);

#define COLOR_WHITE			0xffff
#define COLOR_GRAY			((15<<11)|(15<<6)|(15<<1)|1)
#define COLOR_BLACK			0x0000
#define COLOR_RED			0xf800
#define COLOR_DARKRED		((15<<11)|1)
#define COLOR_GREEN			0x07e0
#define COLOR_BLUE			0x003e
#define COLOR_MAGENTA		0xf83e
#define COLOR_CYAN			0x07fe
#define COLOR_YELLOW		0xffc0
#define COLOR_ORANGE		0xfc00
#define COLOR_VIOLET		COLOR_MAGENTA

#endif /*__file_dialog_h__*/
