#ifndef __GPFONT16_H__
#define	__GPFONT16_H__

#include "gpdef.h" 
#include "gpgraphic.h"
#include "gpfont.h"

void GpFontInit16(BGFONTINFO * ptr);
void GpTextOut16(GPDRAWTAG * gptag, GPDRAWSURFACE * ptgpds, int x, int y, char * sour, int color);
void GpCharOut16(GPDRAWTAG * gptag, GPDRAWSURFACE * ptgpds, int x, int y, char * sour, int color);
void GpTextNOut16(GPDRAWTAG * gptag, GPDRAWSURFACE * ptgpds, int x, int y, char * sour, int nStart, int nString, int color);
void GpSysFontGet16(BGFONTINFO *pFinfo);

#endif
