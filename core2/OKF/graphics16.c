/*************************************************************************

  graphics16.c

    Graphics functions to work in 16 bits for the Game Park GP32.
    graphics16.c is part of the Oankali GP32 run-time library.

  Copyright (c) 2004 by
  Guillem Reinoso <oankali@andorra.ad>

  These software is freeware and is provided as is.
  If it doesn't work for you or it makes your PC melt down, it's not my
  responsibility.
  If you use it in your programs, cite me please.

**************************************************************************/


#include <gpgraphic.h>

#include "graphics16.h"


// Implementacions
int BitBlt8To16(GPDRAWTAG * gptag, GPDRAWSURFACE * ptgpds, int dx, int dy, int width, int height,
                unsigned char * bitmap, int bitmapx, int bitmapy, int bitmapWidth, int bitmapHeight)
{
  GPDRAWTAG limits;
  int x, y, i, j;
  unsigned char intensity;

  if (gptag)
    limits = *gptag;
  else
  {
    limits.clip_x = 0;
    limits.clip_y = 0;
    limits.clip_w = 320;
    limits.clip_h = 240;
  }

  // Clip bitmap
  //- If completly out of limits, return
  if ((dx >= limits.clip_x + limits.clip_w) || (dy >= limits.clip_y + limits.clip_h))
    return 0;
  if ((dx + width - 1 < limits.clip_x) || (dy + height - 1 < limits.clip_y))
    return 0;

  //- Clip X and Width
  if (dx < limits.clip_x)
  {
    width -= limits.clip_x - dx;
    bitmapx += limits.clip_x - dx;
    dx = limits.clip_x;
  }
  if (dx + width > limits.clip_x + limits.clip_w)
    width = limits.clip_x + limits.clip_w - dx;

  //- Clip Y and Height
  if (dy < limits.clip_y)
  {
    height -= limits.clip_y - dy;
    bitmapy += limits.clip_y - dy;
    dy = limits.clip_y;
  }
  if (dy + height> limits.clip_y + limits.clip_h)
    height = limits.clip_y + limits.clip_h - dy;


  // Blit bitmap
  for (y=bitmapHeight - (bitmapy + height), j=0; y < bitmapHeight - bitmapy; y++, j++)
    for (x=bitmapx, i=0; x < bitmapx + width; x++, i++) {
      intensity = bitmap[y + x * bitmapHeight];
      PutPixel(ptgpds, dx + i, dy + height - j - 1, RGB(intensity, intensity, intensity));
    }

  return 0;
}

void OverlayColor16(GPDRAWSURFACE *ptgpds, int dx, int dy, int width, int height,
                    int color, int intensity)
{
  int x, y;
  unsigned short pixColor;
  int red, green, blue;

  for (y=dy; y < dy + height; y++)
    for (x=dx; x < dx + width; x++)
    {
      pixColor = GetPixel(ptgpds, x, y);

      red   = ((GetRGBRed  (color) * intensity) >> 5) + ((GetRGBRed(pixColor)   * (32 - intensity)) >> 5);
      green = ((GetRGBGreen(color) * intensity) >> 5) + ((GetRGBGreen(pixColor) * (32 - intensity)) >> 5);
      blue  = ((GetRGBBlue (color) * intensity) >> 5) + ((GetRGBBlue(pixColor)  * (32 - intensity)) >> 5);

      PutPixel(ptgpds, x, y, RGB(red, green, blue));
    }
}
