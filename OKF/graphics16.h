/*************************************************************************

  graphics16.h

    Graphics functions to work in 16 bits for the Game Park GP32.
    graphics16.h is part of the Oankali GP32 run-time library.

  Copyright (c) 2004 by
  Guillem Reinoso <oankali@andorra.ad>

  These software is freeware and is provided as is.
  If it doesn't work for you or it makes your PC melt down, it's not my
  responsibility.
  If you use it in your programs, cite me please.

**************************************************************************/


#ifndef __graphics16_h__
#define __graphics16_h__

#ifdef __cplusplus

extern "C" {

#endif


// Macros
#define RGB(Red, Green, Blue) (((unsigned int) ((Red) << 11)) + \
                               ((unsigned int) ((Green) << 6)) + \
                               ((unsigned int) ((Blue) << 1)))

#define GetRGBRed(color)   (((color) & 0xF800) >> 11)
#define GetRGBGreen(color) (((color) & 0x7C0)  >>  6)
#define GetRGBBlue(color)  (((color) & 0x3E)   >>  1)

#define GetPixel(ptgpds, x,  y) (*((unsigned short *)((ptgpds)->ptbuffer) + (x) * 240 + (239 - (y))))

#define PutPixel(ptgpds, x,  y,  c)\
{\
	*((unsigned short *)((ptgpds)->ptbuffer) + (x) * 240 + (239 - (y))) = (c);\
}


// Prototipus
int  BitBlt8To16(GPDRAWTAG * gptag, GPDRAWSURFACE * ptgpds, int dx, int dy, int width, int height, unsigned char * bitmap, int bitmapx, int bitmapy, int bitmapWidth, int bitmapHeight);
void OverlayColor16(GPDRAWSURFACE *ptgpds, int dx, int dy, int width, int height, int color, int intensity);

#ifdef __cplusplus

}

#endif


#endif /*__graphics16_h__*/
