/*************************************************************************

  fileSelector.h

    Configurable file selector for the Game Park GP32.
    Uses the OKF Font Engine to print texts.
    fileSelector.h is part of the Oankali GP32 run-time library.

  Copyright (c) 2004 by
  Guillem Reinoso <oankali@andorra.ad>

  These software is freeware and is provided as is.
  If it doesn't work for you or it makes your PC melt down, it's not my
  responsibility.
  If you use it in your programs, cite me please.

**************************************************************************/


#ifndef __fileSelector_h__
#define __fileSelector_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "okf.h"
#include "folder.h"


// Structures
typedef struct tagFILESELECTOR {

  // public members
  char  currentPath[FLD_MAXPATH + 1];
  char  filename[FLD_MAXFILEPATH + 1];
  char  trackPath;
  char *pFilters;
  short options;

  char *pFileSizeFormat;
  char *pTotalsFormat;

  short pathLeft;
  short pathTop;
  short listTop;
  short listSize;
  short fileLeft;
  short fileYDisplacement;
  short sizeRight;
  short sizeYDisplacement;
  short totalsLeft;
  short totalsTop;
  short scrollBarLeft;
  short selectionHeight;
  short selectionMargin;

  char  showPath;
  char  showExtensions;
  char  showSizes;
  char  showTotals;
  char  showScrollBar;
  char  showSeparationLines;

  int   pathFont;
  int   folderFont;
  int   fileFont;
  int   sizesFont;
  int   totalsFont;

  unsigned short *pBackground;
  int   backColor;
  int   separationLinesColor;
  int   selectionColor;
  int   selectionIntensity;
  int   scrollBarColor;

  unsigned int repetitionDelay;
  unsigned int repetitionSpeed;

  // private members
  short pathSepLineTop;
  short totalsSepLineTop;
  short selectionLeft;
  short selectionWidh;
  short scrollBarHeight;

} FILESELECTOR;


// Prototypes
int  FileSelector(FILESELECTOR *pSelector);
void FileSelectorInit(FILESELECTOR *pSelector);


#ifdef __cplusplus
  }
#endif

#endif /*__fileSelector_h__*/
