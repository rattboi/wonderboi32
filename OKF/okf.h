/*************************************************************************

  okf.h

    OKF Font Engine v1.0 public declarations and macro definitions.

  Copyright (c) 2004 by
  Guillem Reinoso <oankali@andorra.ad>

  This file may only be used, modified, and distributed under the terms
  of the license include with this file, OKF_LIC.TXT.
  By continuing to use, modify, or distribute this file you indicate that
  you have read the license and understand and accept it fully.

**************************************************************************/


#ifndef __okf_h__
#define __okf_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <gpgraphic.h>

#include "list.h"


// Enumerations
typedef enum { okfOk, okfNotInitialized, okfMemory, okfNoCurrentFont, okfHandleNotValid,
               okfFontFileNotFound, okfBadFontFile, okfVersionNotSupported, okfSMCError } OKFERROR;

typedef enum { okfJustifyLeft, okfCenter, okfJustifyRight } OKFJUSTIFICATION;

typedef enum { okfNone, okfOverlay, okfAddColor, okfSubtractColor } OKFEFFECTTYPE;


// Constants
#define OKF_FIRSTCHAR     32     // OKF_LASTCHAR - OKF_FIRSTCHAR + 1 has to be
#define OKF_LASTCHAR      255    // less than 255
#define OKF_MAXLINE       1024
#define OKF_MAXNUMBER     32


// Structures
typedef struct tagOKFHEADER {
  unsigned char  signature[4];
  unsigned char  version;

  // to avoid alignment problems, flags is separated in 2 unsigned chars
  union {
    unsigned char flags1;
    struct {
      unsigned char colorDepth : 1;
      unsigned char bitmapRLEEncoded : 1;
      unsigned char alphaMask : 1;
      unsigned char maskRLEEncoded : 1;
      unsigned char shadowMask : 1;
      unsigned char indents : 1;
      unsigned char postProcessed : 1;
      unsigned char proportional : 1;
    };
  };
  union {
    unsigned char flags2;
    struct {
      unsigned char serif : 1;
      unsigned char script : 1;
      unsigned char bold : 1;
      unsigned char italic : 1;
      unsigned char underline : 1;
      unsigned char strikethrough : 1;
    };
  };

  unsigned char  size;
  unsigned char  name[32];
  unsigned long  fileSize;
  unsigned long  bitmapOffset;
  unsigned long  maskOffset;
  unsigned short transColor;
  unsigned short shadowColor;
  unsigned char  reserved1[2];
  unsigned short bitmapWidth;
  unsigned char  bitmapHeight;
  unsigned char  defaultWidth;
  unsigned char  defaultHeight;
  unsigned char  maxWidth;
  unsigned char  maxHeight;
  signed char    maskWidthInc;
  signed char    maskHeightInc;
  signed char    shadowOX;
  signed char    shadowOY;
  signed char    shadowXPos;
  signed char    shadowYPos;
  signed char    lineSeparation;
  signed char    charSeparation;
  unsigned char  nbChars;
  unsigned char  reserved2[2];
} OKFHEADER;

typedef struct tagOKFCHAR {
  unsigned char  index;
  unsigned short position;
  unsigned char  width;
  signed char    leftIndent;
  signed char    rightIndent;
} OKFCHAR;

typedef struct tagOKFFONT {
  OKFHEADER *pOkf;

  unsigned char  embeddedFont;
  unsigned char *pBitmap;
  unsigned short bitmapWidth;
  unsigned short bitmapHeight;
  unsigned char *pMask;
  unsigned short maskWidth;
  unsigned short maskHeight;

  unsigned char *charSet;
  unsigned char *charWidths;
  unsigned char *charLeftIndents;
  unsigned char *charRightIndents;
  OKFCHAR extInfo[OKF_LASTCHAR - OKF_FIRSTCHAR + 1];
} OKFFONT;

typedef struct tagOKFEFFECT {
  OKFEFFECTTYPE type;           // drawing effect type
  int color;                    // color for the drawing effect
  int intensity;                // intensity of the drawing effect
} OKFEFFECT;

typedef struct tagOKFENGINE {
  LIST *pFonts;                 // registered font list
  OKFERROR lastError;           // last error code

  unsigned char initialized;    // engine is initialized?

  GPDRAWSURFACE *pSurfaces;     // Pointer to the GP drawing surfaces
  int *pCurSurface;             // Pointer to the active GP drawing surface

  unsigned char useMask;        // use font mask?
  signed char lineSeparation;   // line separation to add to default font line separation
  signed char charSeparation;   // char separation to add to default font char separation

  struct {
    short x, y;                 // shadow position
    OKFEFFECT effect;           // drawing effect for the shadow
    int opacity;                // opacity of the shadow
  } shadow;                     // shadow properties

  OKFEFFECT effect;             // drawing effect for the characters
  int opacity;                  // opacity of the characters

  unsigned short font;          // current font handle
  int x, y;                     // current cursor position
  OKFJUSTIFICATION justify;     // current text justification mode

  struct {
    short x1, y1, x2, y2;       // clipping coordinate
  } clip;                       // clipping coordinates
} OKFENGINE;


// Global variables
extern OKFENGINE okf;

// Macro definitions
#define OkfGetCurrentFont()   ((OKFFONT *) ListGetObjectByHandle(okf.pFonts, okf.font))
#define OkfGetFont(handle)    ((OKFFONT *) ListGetObjectByHandle(okf.pFonts, (handle)))

// Prototypes
int OkfApplyEffect(int handle, OKFEFFECTTYPE type, int color, int intensity);
int OkfClone(int handle);
int OkfGetEmbeddedFontHandle(unsigned long *pSource);
int OkfGetHandle(OKFFONT *pFont);
int OkfGetTextHeight(unsigned char *pStr);
int OkfGetTextWidth(unsigned char *pStr);
int OkfInitialize(GPDRAWSURFACE *pSurfaces, int *pCurSurface);
int OkfLoadAndRegister(unsigned char *pFilename);
int OkfPrint(unsigned char *pStr);
int OkfPrintSurface(unsigned char *pStr, int surface);
int OkfPrintAt(int x, int y, unsigned char *pStr);
int OkfPrintChar(unsigned char ch);
int OkfPrintCharAt(int x, int y, unsigned char ch);
int OkfPrintLong(long number, unsigned char *pFormat);
int OkfPrintLongAt(int x, int y, long number, unsigned char *pFormat);
int OkfRegister(unsigned long *pSource);
int OkfRelease(int handle);
int OkfSetToDefaults(void);
int OkfSplitLines(unsigned char *pStr, int width, unsigned char *pResultStr);
int OkfTerminate(void);

#ifdef __cplusplus
  }
#endif

#endif /*__okf_h__*/
