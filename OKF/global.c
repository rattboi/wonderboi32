/*************************************************************************

  global.c

    Some global functions and variables.
    global.c is part of the Oankali GP32 run-time library.

  Copyright (c) 2004 by
  Guillem Reinoso <oankali@andorra.ad>

  These software is freeware and is provided as is.
  If it doesn't work for you or it makes your PC melt down, it's not my
  responsibility.
  If you use it in your programs, cite me please.

**************************************************************************/


#include <gpdef.h>
#include <gpstdlib.h>
#include <gpmem.h>
#include <gpgraphic.h>

#include "global.h"


GPDRAWSURFACE gtSurface[2];
int giSurface;


// Implementacions
void InitGraphics(int depth)
{
  // Inicialitza el mode gràfic
  GpGraphicModeSet(depth, NULL);

  // Crea les superficies
  GpLcdSurfaceGet(&gtSurface[0], 0);
  GpLcdSurfaceGet(&gtSurface[1], 1);

  // Registra la superficie primària
  giSurface = 0;
  GpSurfaceSet(&gtSurface[giSurface]);

  // Activa el LCD si no ho estava
  if (!(GpLcdStatusGet() & GPC_LCD_ON_BIT))
    GpLcdEnable();

  giSurface = 1;
}

void ReleaseScreen(unsigned short *screen)
{
  // allibera la memoria
  if (screen != NULL)
    gm_free(screen);
}

unsigned short *SaveBackScreen(void)
{
  unsigned short *screen;
  
  //-- guarda l'estat actual de la pantalla
  screen = (unsigned short *) gm_malloc(320 * 240 * sizeof(unsigned short));
  if (screen != NULL)
    gm_memcpy(screen, gtSurface[giSurface].ptbuffer, 320 * 240 * sizeof(unsigned short));

  return screen;
}

unsigned short *SaveScreen(void)
{
  unsigned short *screen;
  
  //-- guarda l'estat actual de la pantalla
  screen = (unsigned short *) gm_malloc(320 * 240 * sizeof(unsigned short));
  if (screen != NULL)
    gm_memcpy(screen, gtSurface[(giSurface+1) & 0x01].ptbuffer, 320 * 240 * sizeof(unsigned short));

  return screen;
}

char *Thousands(char *number)
{
  int i, p, d;

  p = gm_lstrlen(number);
  if (p <= 3)
    return number;

  i = 0;
  d = (p - 1) / 3;
  number[p + d] = number[p--];

  while (d)
  {
    number[p + d] = number[p--];
    if (++i % 3 == 0)
    {
      number[p + d] = '.';
      d--;
    }
  }

  return number;
}

void Wait(unsigned int ticks)
{
  unsigned int tick;

  tick = GpTickCountGet();
  while ((GpTickCountGet() - tick) < ticks) ;
}

int WaitKey(unsigned int ticks)
{
  unsigned int tick;

  tick = GpTickCountGet();
  while ((GpTickCountGet() - tick) < ticks)
    if (GpKeyGet())
      return TRUE;

  return FALSE;
}
