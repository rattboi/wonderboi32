#include <stdlib.h>
#include <gpgraphic.h>
#include <gpgraphic16.h>
#include "graphics16.h"

// gpgraphic16.h
/*int MyGpBitBlt16(GPDRAWTAG * gptag, GPDRAWSURFACE * ptgpds, int dx, int dy, int width, int height, 
                 unsigned char * src, int sx, int sy, int imgw, int imgh)
{
  register int x, y;
  int c;
  
  if ((imgw <= 0) || (sx + imgw > width))
    imgw = width - sx;
  if ((imgh <= 0) || (sy + imgh > height))
    imgh = height - sy;

  for (y=0; y < imgh; y++)
    for (x=0; x < imgw; x++) {
      c = ((unsigned short *) src)[((sx + x) * height) + (height - (sy + y) - 1)];
      PutPixel(ptgpds, dx + x, dy + y, ,c);
    }

  return 0;
}*/

/*int MyTransBlt16(GPDRAWTAG * gptag, GPDRAWSURFACE * ptgpds, int dx, int dy, int width, int height,
                 unsigned char *src, int sx, int sy, int imgw, int imgh, int color)
{
  register int x, y;
  int c;
  
  if ((imgw <= 0) || (sx + imgw > width))
    imgw = width - sx;
  if ((imgh <= 0) || (sy + imgh > height))
    imgh = height - sy;

  for (y=0; y < imgh; y++)
    for (x=0; x < imgw; x++) {
      c = ((unsigned short *) src)[((sx + x) * height) + (height - (sy + y) - 1)];
      if (c != color)
        PutPixel(ptgpds, dx + x, dy + y, c);
    }

  return 0;
}*/


// gpstdlib.h
void MyGpAppExit(void)
{
  exit(0);
}

int MyGpClockSpeedChange(int master_speed, int div_factor, int clk_mode)
{
  return 0;
}

void MyGpSrand(int seed)
{
  srand(seed);
}

unsigned int MyGpRand(void)
{
  return (unsigned int) rand(); 
}

unsigned int MyGpRandN(unsigned n)
{
  return (rand() % n);
}



// gpmodplay.h
#ifdef libmodplay

#include "gpmodplay.h"

void MODPlay_Init ( MODPlay *modPlayer ) { } ;
int  MODPlay_AllocSFXChannels ( MODPlay *modPlayer, int channels ) { return 0; } ;
int  MODPlay_Load ( MODPlay *modPlayer, char * fname ) { return 0; } ;
void MODPlay_Unload ( MODPlay *modPlayer ) { } ;
int  MODPlay_Start ( MODPlay *modPlayer ) { return 0; } ;
int  MODPlay_Stop ( MODPlay *modPlayer ) { return 0; } ;
int  MODPlay_SetMOD ( MODPlay *modPlayer, u8 *modSource ) { return 0; } ;
int  MODPlay_Pause ( MODPlay *modPlayer, BOOL activate ) { return 0; } ;
void MODPlay_SetStereo ( MODPlay *modPlayer, BOOL activate ) { } ;
int  MODPlay_SetPlayFreq ( MODPlay *modPlayer, int freq ) { return 0; } ;
int  MODPlay_TriggerNote ( MODPlay *modPlayer, int channel, u8 instnum, u16 freq, u8 vol ) { return 0; } ;

#endif
