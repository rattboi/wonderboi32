#ifndef __GRAPHIC16_H__
#define __GRAPHIC16_H__

#include "gpgraphic.h"

int GpBitBlt16(GPDRAWTAG * gptag,GPDRAWSURFACE * ptgpds,int dx,int dy,int width,int height,unsigned char * src, int sx,int sy,int imgw,int imgh);
int GpTransBlt16(GPDRAWTAG * gptag,GPDRAWSURFACE * ptgpds,int dx,int dy,int width,int height,unsigned char *src,int sx,int sy,int imgw,int imgh,int color);
int GpBitLRBlt16(GPDRAWTAG * gptag,GPDRAWSURFACE * ptgpds,int dx,int dy,int width,int height,unsigned char * src,int sx,int sy,int imgw,int imgh);
int GpTransLRBlt16(GPDRAWTAG * gptag,GPDRAWSURFACE * ptgpds,int dx,int dy,int width,int height,unsigned char* src,int sx,int sy,int imgw,int imgh,int color);
int GpBitUDBlt16(GPDRAWTAG * gptag,GPDRAWSURFACE * ptgpds,int dx,int dy,int width,int height,unsigned char * src,int sx,int sy,int imgw,int imgh);
int GpTransUDBlt16(GPDRAWTAG * gptag,GPDRAWSURFACE * ptgpds,int dx,int dy,int width,int height,unsigned char* src,int sx,int sy,int imgw,int imgh,int color);
int GpRectFill16(GPDRAWTAG * gptag,GPDRAWSURFACE * ptgpds,int dx,int dy,int width,int height,int color);


void GpPointSet16(GPDRAWSURFACE * ptgpds,int x, int y, int color);
void GpLineDraw16(GPDRAWSURFACE * ptgpds,int xS,int yS,int xE,int yE,int color);
void GpRectDraw16(GPDRAWSURFACE * ptgpds,int left,int top,int right,int bottom,int color);
void GpEllipseDraw16(GPDRAWSURFACE * ptgpds,int xS,int yS,int w,int h,int color);


#endif /*__GRPHIC16_H__*/
