/*************************************************************************

  global.h

    Some global functions and variables.
    global.h is part of the Oankali GP32 run-time library.

  Copyright (c) 2004 by
  Guillem Reinoso <oankali@andorra.ad>

  These software is freeware and is provided as is.
  If it doesn't work for you or it makes your PC melt down, it's not my
  responsibility.
  If you use it in your programs, cite me please.

**************************************************************************/


#ifndef __global_h__
#define __global_h__

#ifdef __cplusplus
extern "C" {
#endif


extern GPDRAWSURFACE gtSurface[2];
extern int giSurface;


// Macros
#define CopyBackScreen() \
{\
  gm_memcpy(gtSurface[giSurface].ptbuffer, gtSurface[(giSurface+1) & 0x01].ptbuffer, 320 * 240 * sizeof(unsigned short));\
}

#define SurfaceFlip() \
{\
  GpSurfaceFlip(&gtSurface[giSurface]);\
  giSurface ^= 0x01;\
}


// Prototipus
void InitGraphics(int depth);
void ReleaseScreen(unsigned short *screen);
unsigned short *SaveBackScreen(void);
unsigned short *SaveScreen(void);
char *Thousands(char *number);
void Wait(unsigned int ticks);
int WaitKey(unsigned int ticks);


#ifdef __cplusplus
  }
#endif

#endif /*__global_h__*/
