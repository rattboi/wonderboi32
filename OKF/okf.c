/*************************************************************************

  okf.c

    OKF Font Engine v1.0 implementations.

  Copyright (c) 2004 by
  Guillem Reinoso <oankali@andorra.ad>

  This file may only be used, modified, and distributed under the terms
  of the license include with this file, OKF_LIC.TXT.
  By continuing to use, modify, or distribute this file you indicate that
  you have read the license and understand and accept it fully.

**************************************************************************/


#include <gpdef.h>
#include <gpstdlib.h>
#include <gpstdio.h>
#include <gpgraphic.h>
#include <gpgraphic16.h>

#include "global.h"
#include "graphics16.h"
#include "list.h"
#include "okf.h"


//*** Public global variables
OKFENGINE okf;


//*** Private macro definitions
#define OkfOverlayColor(red, green, blue, color, intensity) \
{ \
  if ((intensity) == 31) \
  { \
    red   = GetRGBRed  (color); \
    green = GetRGBGreen(color); \
    blue  = GetRGBBlue (color); \
  } \
  else if ((intensity) > 0) \
  { \
    red   = (GetRGBRed  (color) * (intensity)) / 31 + ((red)   * (31 - (intensity))) / 31; \
    green = (GetRGBGreen(color) * (intensity)) / 31 + ((green) * (31 - (intensity))) / 31; \
    blue  = (GetRGBBlue (color) * (intensity)) / 31 + ((blue)  * (31 - (intensity))) / 31; \
  } \
}

#define OkfUnderlayColor(red, green, blue, color, intensity) \
{ \
  if ((intensity) == 0) \
  { \
    red   = GetRGBRed  (color); \
    green = GetRGBGreen(color); \
    blue  = GetRGBBlue (color); \
  } \
  else if ((intensity) < 31) \
  { \
    red   = ((red)   * (intensity)) / 31 + (GetRGBRed  (color) * (31 - (intensity))) / 31; \
    green = ((green) * (intensity)) / 31 + (GetRGBGreen(color) * (31 - (intensity))) / 31; \
    blue  = ((blue)  * (intensity)) / 31 + (GetRGBBlue (color) * (31 - (intensity))) / 31; \
  } \
}

#define OkfAddColor(red, green, blue, color) \
{ \
  red   += GetRGBRed(color); \
  green += GetRGBGreen(color); \
  blue  += GetRGBBlue(color); \
}

#define OkfSubtractColor(red, green, blue, color) \
{ \
  red   -= GetRGBRed(color); \
  green -= GetRGBGreen(color); \
  blue  -= GetRGBBlue(color); \
}

#define OkfAdjustColorComponents(red, green, blue) \
{ \
  if ((red) < 0)    red = 0; \
  if ((red) > 31)   red = 31; \
  if ((green) < 0)  green = 0; \
  if ((green) > 31) green = 31; \
  if ((blue) < 0)   blue = 0; \
  if ((blue) > 31)  blue = 31; \
}


//*** Private prototypes
static OKFFONT *OkfAllocateFontMemory(int embeddedFont, OKFHEADER *pOkfHeader);
static void OkfDecodeBitmap16(OKFFONT *pFont, unsigned char *pRLEData, unsigned short *pBitmap);
static void OkfDecodeMask(OKFFONT *pFont, unsigned char *pRLEData, unsigned char *pMask);
static int OkfDoGetTextHeight(OKFFONT *pFont, unsigned char *pStr);
static int OkfDoGetTextWidth(OKFFONT *pFont, unsigned char *pStr);
static int OkfDoPrint(OKFFONT *pFont, unsigned char *pStr);
static int OkfDoPrintSurface(OKFFONT *pFont, unsigned char *pStr, int surface);
static int OkfDoPrintJustified(OKFFONT *pFont, unsigned char *pStr);
static int OkfDoPrintJustifiedSurface(OKFFONT *pFont, unsigned char *pStr, int surface);
static void OkfFreeFontMemory(OKFFONT *pFont);
static void OkfGPRotateBitmap16(OKFFONT *pFont, unsigned char *pRAWData, unsigned short *pBitmap);
static void OkfGPRotateMask(OKFFONT *pFont, unsigned char *pRAWData, unsigned char *pMask);
static void OkfPrepareBitmaps(OKFFONT *pFont, unsigned char *pFontData, unsigned char *pMaskData);
static void OkfPrepareCharTables(OKFFONT *pFont);
static int OkfRender16(int dx, int dy, int width, int height, unsigned char *bitmap, int bitmapx, int bitmapy, int bitmapWidth, int bitmapHeight, unsigned short transColor);
static int OkfRenderMask16(int dx, int dy, int width, int height, unsigned char *bitmap, int bitmapx, int bitmapy, int bitmapWidth, int bitmapHeight, unsigned char *mask);
static int OkfRenderShadow16(int dx, int dy, int width, int height, unsigned char *mask, int maskx, int masky, int maskWidth, int maskHeight, unsigned short shadowColor);
static int OkfRenderSurface16(int dx, int dy, int width, int height, unsigned char *bitmap, int bitmapx, int bitmapy, int bitmapWidth, int bitmapHeight, unsigned short transColor, int surface);
static int OkfRenderMaskSurface16(int dx, int dy, int width, int height, unsigned char *bitmap, int bitmapx, int bitmapy, int bitmapWidth, int bitmapHeight, unsigned char *mask, int surface);
static int OkfRenderShadowSurface16(int dx, int dy, int width, int height, unsigned char *mask, int maskx, int masky, int maskWidth, int maskHeight, unsigned short shadowColor, int surface);

//*** Public implementations
int OkfApplyEffect(int handle, OKFEFFECTTYPE type, int color, int intensity)
{
  OKFFONT *pFont;
  int x, y;
  unsigned short *bmp;
  unsigned short pixColor;
  int red, green, blue;

  if (!okf.initialized)
  {
    okf.lastError = okfNotInitialized;
    return -1;
  }

  // Get font
  pFont = OkfGetFont(handle);
  if (!pFont)
  {
    okf.lastError = okfHandleNotValid;
    return -1;
  }

  okf.lastError = okfOk;

  // If no effect, return
  if (type == okfNone)
    return 0;

  bmp = (unsigned short *) pFont->pBitmap;
  for (x=0; x < pFont->pOkf->bitmapWidth; x++)
    for (y=0; y < pFont->pOkf->bitmapHeight; y++)
    {
      pixColor = bmp[x * pFont->bitmapHeight + (pFont->bitmapHeight - 1 - y)];

      if (!pFont->pOkf->alphaMask || pFont->pOkf->shadowMask)
        if (pixColor == pFont->pOkf->transColor)
          continue;

      // Separate color components
      red   = GetRGBRed(pixColor);
      green = GetRGBGreen(pixColor);
      blue  = GetRGBBlue(pixColor);

      // Modify color
      switch (type)
      {
        case okfOverlay:
          OkfOverlayColor(red, green, blue, color, intensity);
          break;
        case okfAddColor:
          OkfAddColor(red, green, blue, color);
          OkfAdjustColorComponents(red, green, blue);
          break;
        case okfSubtractColor:
          OkfSubtractColor(red, green, blue, color);
          OkfAdjustColorComponents(red, green, blue);
          break;
      }

      // Update píxel
      bmp[x * pFont->bitmapHeight + (pFont->bitmapHeight - 1 - y)] = RGB(red, green, blue);
    }

  return 0;
}

int OkfClone(int handle)
{
  OKFFONT *pFont, *pNewFont;
  int size;
  
  if (!okf.initialized)
  {
    okf.lastError = okfNotInitialized;
    return -1;
  }

  // Get font
  pFont = OkfGetFont(handle);
  if (!pFont)
  {
    okf.lastError = okfHandleNotValid;
    return -1;
  }

  // Allocate memory for the new font
  if ((pNewFont = OkfAllocateFontMemory(FALSE, pFont->pOkf)) == NULL)
  {
    okf.lastError = okfMemory;
    return -1;
  }

  // Copy original header and other properties
  gm_memcpy(pNewFont->pOkf, pFont->pOkf, pFont->pOkf->bitmapOffset);
  OkfPrepareCharTables(pNewFont);

  // Copy decoded font bitmap
  size = pNewFont->bitmapWidth * pNewFont->bitmapHeight *
             (pNewFont->pOkf->colorDepth == 1 ? sizeof(short) : sizeof(char));
  gm_memcpy(pNewFont->pBitmap, pFont->pBitmap, size);

  // Copy decoded mask bitmap, if needed
  if (pNewFont->pOkf->alphaMask)
  {
    size = pNewFont->maskWidth * pNewFont->maskHeight * sizeof(char);
    gm_memcpy(pNewFont->pMask, pFont->pMask, size);
  }

  // Add font to list of fonts
  if (ListAddItem(okf.pFonts, pNewFont, 0) == NULL)
  {
    // Release allocated resources
    OkfFreeFontMemory(pNewFont);
    okf.lastError = okfMemory;
    return -1;
  }
  else
    if (!okf.font)
      okf.font = okf.pFonts->lastHandle;

  okf.lastError = okfOk;
  return okf.pFonts->lastHandle;
}

int OkfGetEmbeddedFontHandle(unsigned long *pSource)
{
  LISTITEM *pItem;

  if (!okf.initialized)
  {
    okf.lastError = okfNotInitialized;
    return -1;
  }

  okf.lastError = okfOk;

  // Search the font source in the list
  pItem = okf.pFonts->pFirst;
  while (pItem)
  {
    if (((OKFFONT *) pItem->pObject)->embeddedFont)
      if (((OKFFONT *) pItem->pObject)->pOkf == (OKFHEADER *) pSource)
        return pItem->handle;
    pItem = pItem->pNext;
  }

  // Font source not found
  return 0;
}

int OkfGetHandle(OKFFONT *pFont)
{
  LISTITEM *pItem;

  if (!okf.initialized)
  {
    okf.lastError = okfNotInitialized;
    return -1;
  }

  okf.lastError = okfOk;
  if ((pItem = ListGetItemByObject(okf.pFonts, pFont)) != NULL)
    return pItem->handle;
  else
    return 0;
}

int OkfGetTextHeight(unsigned char *pStr)
{
  OKFFONT *pFont;

  if (!okf.initialized)
  {
    okf.lastError = okfNotInitialized;
    return -1;
  }

  // Get current font
  pFont = OkfGetCurrentFont();
  if (!pFont)
  {
    okf.lastError = okfNoCurrentFont;
    return -1;
  }

  return OkfDoGetTextHeight(pFont, pStr);
}

int OkfGetTextWidth(unsigned char *pStr)
{
  OKFFONT *pFont;
  
  if (!okf.initialized)
  {
    okf.lastError = okfNotInitialized;
    return -1;
  }

  // Get current font
  pFont = OkfGetCurrentFont();
  if (!pFont)
  {
    okf.lastError = okfNoCurrentFont;
    return -1;
  }

  return OkfDoGetTextWidth(pFont, pStr);
}

int OkfInitialize(GPDRAWSURFACE *pSurfaces, int *pCurSurface)
{
  // Init engine
  if (!okf.initialized)
    if ((okf.pFonts = ListCreate()) == NULL)
    {
      okf.initialized = FALSE;
      okf.lastError = okfMemory;
      return -1;
    }

  // Default values
  OkfSetToDefaults();

  okf.pSurfaces = pSurfaces;
  okf.pCurSurface = pCurSurface;

  okf.initialized = TRUE;
  okf.lastError = okfOk;
  return 0;
}

int OkfLoadAndRegister(unsigned char *pFilename)
{
  F_HANDLE handle=0;
  OKFHEADER okfHeader;
  OKFFONT *pFont=NULL;
  ulong bytesRead, oldOffset;
  unsigned char *pFontData=NULL, *pMaskData=NULL;
  unsigned long fontDataSize, maskDataSize;

  okf.lastError = okfOk;

  if (!okf.initialized)
  {
    okf.lastError = okfNotInitialized;
    return -1;
  }

  // Open OKF file
  if (GpFileOpen(pFilename, OPEN_R, &handle) != SM_OK)
  {
    okf.lastError = okfFontFileNotFound;
    return -1;
  }

  // Read OKF main header
  if (okf.lastError == okfOk)
  {
    GpFileRead(handle, &okfHeader, sizeof(OKFHEADER), &bytesRead);
    if (bytesRead != sizeof(OKFHEADER))
      okf.lastError = okfBadFontFile;
  }

  // Check file signature
  if (okf.lastError == okfOk)
  {
    if (okfHeader.signature[0] != 'O' ||
        okfHeader.signature[1] != 'K' ||
        okfHeader.signature[2] != 'F' ||
        okfHeader.signature[3] != '\0')
      okf.lastError = okfBadFontFile;
  }

  // Check font version
  if (okf.lastError == okfOk)
  {
    if (okfHeader.version != 1)
      okf.lastError = okfVersionNotSupported;
  }

  // Allocate memory for the new font
  if (okf.lastError == okfOk)
  {
    if ((pFont = OkfAllocateFontMemory(FALSE, &okfHeader)) == NULL)
      okf.lastError = okfMemory;
  }

  // Calculate size of data (font & mask) to read from the SMC
  if (okf.lastError == okfOk)
  {
    GpFileSeek(handle, FROM_END, 0L, &oldOffset);
    GpFileSeek(handle, FROM_BEGIN, 0L, &oldOffset);
    if (okfHeader.alphaMask)
      maskDataSize = oldOffset - okfHeader.maskOffset;
    else
      maskDataSize = 0;
    fontDataSize = oldOffset - okfHeader.bitmapOffset - maskDataSize;
  }

  // Allocate temporal memory for font bitmap data
  if (okf.lastError == okfOk)
  {
    if ((pFontData = gm_malloc(fontDataSize)) == NULL)
      okf.lastError = okfMemory;
  }

  // Allocate temporal memory for mask bitmap data, if needed
  if (okf.lastError == okfOk)
  {
    if (okfHeader.alphaMask)
      if ((pMaskData = gm_malloc(maskDataSize)) == NULL)
        okf.lastError = okfMemory;
  }

  // Read complete header and all data from SMC card
  if (okf.lastError == okfOk)
  {
    // File position is at the beginning, see previous GpFileSeek()
    GpFileRead(handle, pFont->pOkf, okfHeader.bitmapOffset, &bytesRead);
    if (bytesRead != okfHeader.bitmapOffset)
      okf.lastError = okfBadFontFile;

    if (okf.lastError == okfOk)
    {
      GpFileRead(handle, pFontData, fontDataSize, &bytesRead);
      if (bytesRead != fontDataSize)
        okf.lastError = okfBadFontFile;
    }

    if (okf.lastError == okfOk)
      if (okfHeader.alphaMask)
      {
        GpFileRead(handle, pMaskData, maskDataSize, &bytesRead);
        if (bytesRead != maskDataSize)
          okf.lastError = okfBadFontFile;
      }

    GpFileClose(handle);
    handle = 0;
  }

  // Decode bitmaps
  if (okf.lastError == okfOk)
  {
    // Decode and/or rotate bitmaps
    OkfPrepareBitmaps(pFont, pFontData, pMaskData);

    // Free temporal allocated memory
    if (pMaskData)
      gm_free(pMaskData);
    if (pFontData)
      gm_free(pFontData);
    pMaskData = NULL;
    pFontData = NULL;
  }

  // Final initializations
  if (okf.lastError == okfOk)
  {
    OkfPrepareCharTables(pFont);
  }

  // Add font to list of fonts
  if (okf.lastError == okfOk)
  {
    if (ListAddItem(okf.pFonts, pFont, 0) == NULL)
      okf.lastError = okfMemory;
    else
      if (!okf.font)
        okf.font = okf.pFonts->lastHandle;
  }

  // If some error raised
  if (okf.lastError != okfOk)
  {
    // Release allocated resources
    if (handle)
      GpFileClose(handle);
    if (pMaskData)
      gm_free(pMaskData);
    if (pFontData)
      gm_free(pFontData);
    OkfFreeFontMemory(pFont);
    return -1;
  }
  
  return okf.pFonts->lastHandle;
}

int OkfPrint(unsigned char *pStr)
{
  OKFFONT *pFont;

  if (!okf.initialized)
  {
    okf.lastError = okfNotInitialized;
    return -1;
  }

  // Get current font
  pFont = OkfGetCurrentFont();
  if (!pFont)
  {
    okf.lastError = okfNoCurrentFont;
    return -1;
  }

  if (okf.justify == okfJustifyLeft)
    return OkfDoPrint(pFont, pStr);
  else
    return OkfDoPrintJustified(pFont, pStr);
}

int OkfPrintAt(int x, int y, unsigned char *pStr)
{
  okf.x = x;
  okf.y = y;

  return OkfPrint(pStr);
}

int OkfPrintChar(unsigned char ch)
{
  unsigned char str[2];
  
  str[0] = ch;
  str[1] = '\0';
  return OkfPrint(str);
}

int OkfPrintCharAt(int x, int y, unsigned char ch)
{
  unsigned char str[2];
  
  str[0] = ch;
  str[1] = '\0';
  return OkfPrintAt(x, y, str);
}

int OkfPrintLong(long number, unsigned char *pFormat)
{
	 unsigned char str[OKF_MAXNUMBER];

  gm_sprintf(str, pFormat, number);

  return OkfPrint(str);
}

int OkfPrintLongAt(int x, int y, long number, unsigned char *pFormat)
{
	 unsigned char str[OKF_MAXNUMBER];

  gm_sprintf(str, pFormat, number);

  return OkfPrintAt(x, y, str);
}

// Return handle of new font
int OkfRegister(unsigned long *pSource)
{
  OKFFONT *pFont;
  OKFHEADER *pOkfHeader;

  okf.lastError = okfOk;
  pOkfHeader = (OKFHEADER *) pSource;

  if (!okf.initialized)
  {
    okf.lastError = okfNotInitialized;
    return -1;
  }

  // Check file signature
  if (okf.lastError == okfOk)
  {
    if (pOkfHeader->signature[0] != 'O' ||
        pOkfHeader->signature[1] != 'K' ||
        pOkfHeader->signature[2] != 'F' ||
        pOkfHeader->signature[3] != '\0')
      okf.lastError = okfBadFontFile;
  }

  // Check font version
  if (okf.lastError == okfOk)
  {
    if (pOkfHeader->version != 1)
      okf.lastError = okfVersionNotSupported;
  }

  // Allocate memory for the new font
  if (okf.lastError == okfOk)
  {
    if ((pFont = OkfAllocateFontMemory(TRUE, pOkfHeader)) == NULL)
      okf.lastError = okfMemory;
  }

  // Decode bitmaps
  if (okf.lastError == okfOk)
  {
    // Decode and/or rotate bitmaps
    OkfPrepareBitmaps(pFont, (unsigned char *) pOkfHeader + pOkfHeader->bitmapOffset,
                             (unsigned char *) pOkfHeader + pOkfHeader->maskOffset);
  }

  // Final initializations
  if (okf.lastError == okfOk)
  {
    OkfPrepareCharTables(pFont);
  }

  // Add font to list of fonts
  if (okf.lastError == okfOk)
  {
    if (ListAddItem(okf.pFonts, pFont, 0) == NULL)
      okf.lastError = okfMemory;
    else
      if (!okf.font)
        okf.font = okf.pFonts->lastHandle;
  }

  // If some error raised
  if (okf.lastError != okfOk)
  {
    // Release allocated resources
    OkfFreeFontMemory(pFont);
    return -1;
  }
  
  return okf.pFonts->lastHandle;
}

// Return new current font
int OkfRelease(int handle)
{
  OKFFONT *pFont;

  if (!okf.initialized)
  {
    okf.lastError = okfNotInitialized;
    return -1;
  }

  // Get font
  pFont = OkfGetFont(handle);
  if (!pFont)
  {
    okf.lastError = okfHandleNotValid;
    return -1;
  }

  // Release allocated resources
  OkfFreeFontMemory(pFont);

  // Drop the font from the list
  ListRemoveItemByHandle(okf.pFonts, handle);

  // Update current font if needed
  if (okf.font == handle)
    if (okf.pFonts->pFirst)
      okf.font = okf.pFonts->pFirst->handle;
    else
      okf.font = 0;

  okf.lastError = okfOk;
  return okf.font;
}

int OkfSetToDefaults(void)
{
  okf.useMask = TRUE;
  okf.lineSeparation = 0;
  okf.charSeparation = 0;

  okf.shadow.x = 0;
  okf.shadow.y = 0;
  okf.shadow.effect.type = okfNone;
  okf.shadow.effect.color = 0;
  okf.shadow.effect.intensity = 31;
  okf.shadow.opacity = 31;

  okf.effect.type = okfNone;
  okf.effect.color = 0;
  okf.effect.intensity = 31;
  okf.opacity = 31;

  if (okf.initialized && okf.pFonts->pFirst)
    okf.font = okf.pFonts->pFirst->handle;
  else
    okf.font = 0;
  okf.x = 0;
  okf.y = 0;
  okf.justify = okfJustifyLeft;

  okf.clip.x1 = 0;
  okf.clip.y1 = 0;
  okf.clip.x2 = GPC_LCD_WIDTH - 1;
  okf.clip.y2 = GPC_LCD_HEIGHT -1;

  okf.lastError = okfOk;
  return 0;
}

int OkfSplitLines(unsigned char *pStr, int width, unsigned char *pResultStr)
{
  OKFFONT *pFont;
  unsigned char *ptr, *pBegin, *pEnd, *pCut;
  int w;

  if (!okf.initialized)
  {
    okf.lastError = okfNotInitialized;
    return -1;
  }

  // Get current font
  pFont = OkfGetCurrentFont();
  if (!pFont)
  {
    okf.lastError = okfNoCurrentFont;
    return -1;
  }

  // Separate string in multiple lines, separation is made between words when posible
  ptr = pStr;
  pBegin = pEnd = pResultStr;
  while (*ptr)
  {
    switch (*ptr)
    {
    case '\n':
      *pEnd = '\n';
      *(++pEnd) = '\0';
      pBegin = pEnd;
      break;

    default:
      *pEnd = *ptr;
      *(++pEnd) = '\0';
      break;
    }

    ptr++;

    // If width of line is to big, split line to end of previous word if possible
    w = OkfDoGetTextWidth(pFont, pBegin);
    if (w > width)
    {
      pCut = pEnd - 1;

      // If current line is one character long, split after current char
      if (pCut == pBegin)
      {
        *pEnd = '\n';
        *(++pEnd) = '\0';
      }

      // If current line is more than one character long, split before current char
      else
      {
        // Look for a previous end of word (' ', '-')
        while (pCut > pBegin)
        {
          if ((*pCut == ' ') || (*pCut == '-'))
          {
            ptr -= pEnd - pCut - 1;
            pEnd = pCut + (*pCut == '-' ? 1 : 0);
            *pEnd = '\n';
            *(++pEnd) = '\0';
            pCut = NULL;
            break;
          }

          pCut--;
        }

        // If couldn't split line to the end of a whole word
        if (pCut)
        {
          ptr--;
          *(pEnd - 1) = '\n';
        }
      }

      pBegin = pEnd;
    }
  }

  okf.lastError = okfOk;
  return 0;
}

int OkfTerminate(void)
{
  if (!okf.initialized)
  {
    okf.lastError = okfOk;
    return 0;
  }

  // Release all fonts
  while (okf.pFonts->pLast)
    OkfRelease(okf.pFonts->pLast->handle);

  // Destroy main list
  ListDestroy(okf.pFonts);
  okf.pFonts = NULL;

  okf.initialized = FALSE;
  okf.lastError = okfOk;
  return 0;
}


//*** Private implementations
static OKFFONT *OkfAllocateFontMemory(int embeddedFont, OKFHEADER *pOkfHeader)
{
  int size;
  OKFFONT *pFont=NULL;
  int ok=TRUE;

  // Allocate memory for main font structure
  if (ok)
  {
    pFont = gm_malloc(sizeof(OKFFONT));
    if (!pFont)
      ok = FALSE;
    else
    {
      pFont->embeddedFont = embeddedFont;
      pFont->pOkf = NULL;
      pFont->pBitmap = NULL;
      pFont->pMask = NULL;
    }
  }

  // Allocate memory for complete header (main file header + char tables), if needed
  if (ok)
  {
    if (!embeddedFont)
    {
      if ((pFont->pOkf = gm_malloc(pOkfHeader->bitmapOffset)) == NULL)
        ok = FALSE;
    }
    else
      pFont->pOkf = pOkfHeader;
  }

  // Calculate sizes
  if (ok)
  {
    // Calculate GP32 bitmap sizes
    pFont->bitmapWidth = ((pOkfHeader->bitmapWidth + 7) / 8) * 8;
    pFont->bitmapHeight = ((pOkfHeader->bitmapHeight + 7) / 8) * 8;
  
    // Calculate GP32 mask sizes
    pFont->maskWidth = pOkfHeader->bitmapWidth + pOkfHeader->maskWidthInc * pOkfHeader->nbChars;
    pFont->maskHeight = pOkfHeader->bitmapHeight + pOkfHeader->maskHeightInc;
    pFont->maskWidth = ((pFont->maskWidth + 7) / 8) * 8;
    pFont->maskHeight = ((pFont->maskHeight + 7) / 8) * 8;
  }

  // Allocate memory for decoded font bitmap
  if (ok)
  {
    size = pFont->bitmapWidth * pFont->bitmapHeight *
               (pOkfHeader->colorDepth == 1 ? sizeof(short) : sizeof(char));
    if ((pFont->pBitmap = gm_malloc(size)) == NULL)
      ok = FALSE;
  }

  // Allocate memory for decoded mask bitmap, if needed
  if (ok)
  {
    if (pOkfHeader->alphaMask)
    {
      size = pFont->maskWidth * pFont->maskHeight * sizeof(char);
      if ((pFont->pMask = gm_malloc(size)) == NULL)
        ok = FALSE;
    }
  }

  // If some error raised
  if (!ok)
  {
    // Release allocated resources
    if (pFont)
    {
      if (pFont->pMask)
        gm_free(pFont->pMask);
      if (pFont->pBitmap)
        gm_free(pFont->pBitmap);
      if (pFont->pOkf && !pFont->embeddedFont)
        gm_free(pFont->pOkf);
      gm_free(pFont);
    }
    pFont = NULL;
  }
  
  return pFont;
}

static void OkfDecodeBitmap16(OKFFONT *pFont, unsigned char *pRLEData, unsigned short *pBitmap)
{
  int x, y, i, plane;
  unsigned char count;
  unsigned short color;

  // Some initializations
  // By default, black píxels with highlight ON
  for (i=0; i < pFont->bitmapWidth * pFont->bitmapHeight; i++)
    pBitmap[i] = 0x0001;

  // Decode the data
  pRLEData--;
  for (y=0; y < pFont->pOkf->bitmapHeight; y++)
    for (plane=0; plane < 3; plane++)
      for (x=0; x < pFont->pOkf->bitmapWidth; )
      {
        // Get color & count
        color = *(++pRLEData) & 0x1F;
        if (*pRLEData & 0xE0)
          count = (*pRLEData & 0xE0) >> 5;  
        else
          count = *(++pRLEData);

        // Add color component to GP rotated/sized bitmap
        for (i=0; i < count; i++, x++)
          if (color != 0)
            pBitmap[x * pFont->bitmapHeight + (pFont->bitmapHeight - 1 - y)] |= 
                  color << (11 - plane * 5);
      }
}

static void OkfDecodeMask(OKFFONT *pFont, unsigned char *pRLEData, unsigned char *pMask)
{
  int x, y, i;
  unsigned char count, opacity;
  int maskWidth, maskHeight;

  // Some initializations
  gm_memset(pMask, 0, pFont->maskWidth * pFont->maskHeight * sizeof(unsigned char));
  maskWidth = pFont->pOkf->bitmapWidth + pFont->pOkf->maskWidthInc * pFont->pOkf->nbChars;
  maskHeight = pFont->pOkf->bitmapHeight + pFont->pOkf->maskHeightInc;

  // Decode the data
  pRLEData--;
  for (y=0; y < maskHeight; y++)
    for (x=0; x < maskWidth; )
    {
      // Get opacity & count
      opacity = *(++pRLEData) & 0x1F;
      if (*pRLEData & 0xE0)
        count = (*pRLEData & 0xE0) >> 5;  
      else
        count = *(++pRLEData);

      // Add opacity component to GP rotated/sized mask
      for (i=0; i < count; i++, x++)
        if (opacity != 0)
          pMask[x * pFont->maskHeight + (pFont->maskHeight - 1 - y)] |= opacity;
    }
}

static int OkfDoPrint(OKFFONT *pFont, unsigned char *pStr)
{
  OKFHEADER *pFOkf;
  OKFCHAR *pCh;
  int charSep, totalLineHeight, shadowXPos, shadowYPos, shadowYEndPos;
  int startX, startY;
  int pass, before, after, newLine;
  unsigned char *ptr;
  GPDRAWTAG drawTag;

  // Initialize clipping in case of using standard SDK functions
  drawTag.clip_x = okf.clip.x1;
  drawTag.clip_y = okf.clip.y1;
  drawTag.clip_w = okf.clip.x2 - okf.clip.x1 + 1;
  drawTag.clip_h = okf.clip.y2 - okf.clip.y1 + 1;
  
  // If font has a shadow mask, the rendering will be
  // executed in 2 passes, otherwise in 1
  pFOkf = pFont->pOkf;
  if (pFOkf->alphaMask && pFOkf->shadowMask && okf.useMask && okf.shadow.opacity)
    pass = 2;
  else
    pass = 1;

  // Other initializations to speed up things
  charSep = pFOkf->charSeparation + okf.charSeparation;
  totalLineHeight = pFOkf->defaultHeight + pFOkf->lineSeparation + okf.lineSeparation;
  if (pass == 2)
  {
    shadowXPos = okf.shadow.x + pFOkf->shadowOX + pFOkf->shadowXPos;
    shadowYPos = okf.shadow.y + pFOkf->shadowOY + pFOkf->shadowYPos;
    shadowYEndPos = shadowYPos + pFOkf->defaultHeight + pFOkf->maskHeightInc;
  }
  
  // Loop on different passes
  startX = okf.x;
  startY = okf.y;
  for (; pass >= 1; pass--)
  {
    // Main loop on string
    okf.x = startX;
    okf.y = startY;
    before = after = FALSE;
    newLine = TRUE;
    for (ptr=pStr; *ptr; ptr++)
    {
      // If new line, control its visibility
      if (newLine && !after)
        if (pass == 1)
        {
          // Control text vertical position
          before = (okf.y + pFOkf->defaultHeight - 1 < okf.clip.y1);
          if (!before)
            after = (okf.y > okf.clip.y2);
          else
            after = FALSE;
        }
        else 
        {
          // Control mask vertical position
          before = (okf.y + shadowYEndPos - 1 < okf.clip.y1);
          if (!before)
            after = (okf.y + shadowYPos > okf.clip.y2);
          else
            after = FALSE;
        }

      // If line is not visible, jump to next line
      if (before || after)
      {
        for (; *ptr; ptr++)
          if (*ptr == '\n')
          {
            okf.y += totalLineHeight;
            newLine = TRUE;
            break;
          }
          
        // If it was the last line of the text, simulate the printing
        // to have an accurate final okf.x position, even if it's not visible
        // (Note: calculation is made from end to begin of line)
        if (!*ptr)
        {
          // Only for text, not for shadow
          if (pass == 1)
          {
            newLine = TRUE;
            while ((--ptr >= pStr) && (*ptr != '\n'))
            {
              pCh = &pFont->extInfo[*ptr - OKF_FIRSTCHAR];
              if (pCh->index != 0xFF)
              {
                if (!newLine)
                  okf.x += pCh->rightIndent;
                else
                  newLine = FALSE;
                okf.x += charSep + pCh->leftIndent + pCh->width;
              }
              else
              {
                if (newLine)
                  newLine = FALSE;
                okf.x += charSep + pFOkf->defaultWidth;
              }
            }
          }
          
          // We jump outside the main loop
          break;
        }
      } 
        
      // If line is visible, print char
      else
        if (*ptr == '\n')
        {
          okf.x = startX;
          okf.y += totalLineHeight;
          newLine = TRUE;
        }
        else
        {
          pCh = &pFont->extInfo[*ptr - OKF_FIRSTCHAR];
          if (pCh->index != 0xFF)
          {
            if (!newLine)
              okf.x += pCh->leftIndent;
            else
              newLine = FALSE;
  
            // Render shadow, visibility has been controled before 
            if (pass == 2)
                OkfRenderShadow16(okf.x + shadowXPos,
                                  okf.y + shadowYPos,
                                  pCh->width + pFOkf->maskWidthInc,
                                  pFOkf->defaultHeight + pFOkf->maskHeightInc,
                                  pFont->pMask,
                                  pCh->position + pCh->index * pFOkf->maskWidthInc, 0,
                                  pFont->maskWidth, pFont->maskHeight,
                                  pFOkf->shadowColor);
  
            // Render character, if its visible
            else if (okf.opacity > 0)
              if (pFOkf->alphaMask && !pFOkf->shadowMask && okf.useMask)
                OkfRenderMask16(okf.x, okf.y, pCh->width, pFOkf->defaultHeight,
                                pFont->pBitmap, pCh->position, 0,
                                pFont->bitmapWidth, pFont->bitmapHeight,
                                pFont->pMask);
              else
                if ((okf.effect.type != okfNone) || (okf.opacity < 31))
                  OkfRender16(okf.x, okf.y, pCh->width, pFOkf->defaultHeight, 
                              pFont->pBitmap, pCh->position, 0, 
                              pFont->bitmapWidth, pFont->bitmapHeight,
                              pFOkf->transColor);
                else
                  // To speed up rendering we use SDK when possible 
                  GpTransBlt16(&drawTag, &okf.pSurfaces[*okf.pCurSurface], 
                               okf.x, okf.y, pCh->width, pFOkf->defaultHeight, 
                               pFont->pBitmap, pCh->position, 0, 
                               pFont->bitmapWidth, pFont->bitmapHeight,
                               pFOkf->transColor);
  
            okf.x += pCh->width + pCh->rightIndent + charSep;
          }
          else
          {
            if (newLine)
              newLine = FALSE;
          
            okf.x += pFOkf->defaultWidth + charSep;
          }
        }
    }
  }

  okf.lastError = okfOk;
  return 0;
}

static int OkfDoPrintJustified(OKFFONT *pFont, unsigned char *pStr)
{
  unsigned char str[OKF_MAXLINE];
  unsigned char *pBegin, *pEnd;
  unsigned char ch;
  int left;

  // Get each text line
  gm_strcpy(str, pStr);
  left = okf.x;
  pBegin = pEnd = str;
  do
  {
    if ((*pEnd == '\n') || (*pEnd == '\0'))
    {
      // Forces end of string
      ch = *pEnd;
      *pEnd = '\0';

      // Calculate starting x position
      switch (okf.justify)
      {
        case okfJustifyLeft:  okf.x = left; break;
        case okfCenter:       okf.x = left - (OkfDoGetTextWidth(pFont, pBegin) / 2); break;
        case okfJustifyRight: okf.x = left - OkfDoGetTextWidth(pFont, pBegin); break;
      }

      // Print line
      if (OkfDoPrint(pFont, pBegin) == -1)
        return -1;

      // Recover end of line and jump line if needed
      if ((*pEnd = ch) == '\n')
        okf.y += pFont->pOkf->defaultHeight + pFont->pOkf->lineSeparation + okf.lineSeparation;

      // Next line
      pBegin = pEnd + 1;
    }
  } while (*(pEnd++) != '\0');

  return 0;
}

static int OkfDoGetTextHeight(OKFFONT *pFont, unsigned char *pStr)
{
  int lines;
  int i;

  if (pStr == NULL || !(*pStr))
    return 0;
  
  // Get number of lines
  for (i=0, lines=1; i < gm_lstrlen(pStr); i++)
    if (pStr[i] == '\n')
      lines++;

  return (lines * pFont->pOkf->defaultHeight +
          (lines - 1) * (pFont->pOkf->lineSeparation + okf.lineSeparation));
}

static int OkfDoGetTextWidth(OKFFONT *pFont, unsigned char *pStr)
{
  OKFCHAR *pCh;
  int lineWidth, spaceWidth, maxWidth;
  int li, ri, w;
  int first;
  int i;
  
  if (pStr == NULL || !(*pStr))
    return 0;
  
  // Get widest line
  lineWidth = spaceWidth = maxWidth = 0;
  first = TRUE;
  for (i=0; i < gm_lstrlen(pStr); i++)
  {
    if (pStr[i] == '\n')
    {
      if (lineWidth > maxWidth)
        maxWidth = lineWidth;
      lineWidth = 0;
      spaceWidth = 0;
      first = TRUE;
    }
    else {
      pCh = &pFont->extInfo[pStr[i] - OKF_FIRSTCHAR];
      if (pCh->index != 0xFF)
      {
        w = pCh->width;
        li = pCh->leftIndent;
        ri = pCh->rightIndent;
      }
      else
      {
        w = pFont->pOkf->defaultWidth;
        li = 0;
        ri = 0;
      }
      
      if (first)
        first = FALSE;
      else
        lineWidth += pFont->pOkf->charSeparation + okf.charSeparation + li;

      lineWidth += spaceWidth + w;
      spaceWidth = ri;
    }
  }

  return (lineWidth > maxWidth) ? lineWidth : maxWidth;
}

static void OkfFreeFontMemory(OKFFONT *pFont)
{
  // Release allocated resources
  if (pFont)
  {
    if (pFont->pMask)
      gm_free(pFont->pMask);
    if (pFont->pBitmap)
      gm_free(pFont->pBitmap);
    if (pFont->pOkf && !pFont->embeddedFont)
      gm_free(pFont->pOkf);
    gm_free(pFont);
  }
}

static void OkfGPRotateBitmap16(OKFFONT *pFont, unsigned char *pRAWData, unsigned short *pBitmap)
{
  int x, y;

  // Rotate bitmap
  for (x=0; x < pFont->pOkf->bitmapWidth; x++)
    for (y=0; y < pFont->pOkf->bitmapHeight; y++)
      pBitmap[x * pFont->bitmapHeight + (pFont->bitmapHeight - 1 - y)] = 
          ((unsigned short *) pRAWData)[x + y * pFont->pOkf->bitmapWidth];
}

static void OkfGPRotateMask(OKFFONT *pFont, unsigned char *pRAWData, unsigned char *pMask)
{
  int maskWidth, maskHeight;
  int x, y;

  // Some initializations
  maskWidth = pFont->pOkf->bitmapWidth + pFont->pOkf->maskWidthInc * pFont->pOkf->nbChars;
  maskHeight = pFont->pOkf->bitmapHeight + pFont->pOkf->maskHeightInc;

  // Rotate bitmap
  for (x=0; x < maskWidth; x++)
    for (y=0; y < maskHeight; y++)
      pMask[x * pFont->maskHeight + (pFont->maskHeight - 1 - y)] = 
          pRAWData[x + y * maskWidth];
}

static void OkfPrepareBitmaps(OKFFONT *pFont, unsigned char *pFontData, unsigned char *pMaskData)
{
  // Decode and/or rotate font bitmap
  if (pFont->pOkf->colorDepth == 1)   // 16 bits bitmaps
    if (pFont->pOkf->bitmapRLEEncoded)
      OkfDecodeBitmap16(pFont, pFontData, (unsigned short *) pFont->pBitmap);
    else
      OkfGPRotateBitmap16(pFont, pFontData, (unsigned short *) pFont->pBitmap);

  // Decode and/or rotate mask bitmap, if needed
  if (pFont->pOkf->alphaMask)
    if (pFont->pOkf->maskRLEEncoded)
      OkfDecodeMask(pFont, pMaskData, pFont->pMask);
    else
      OkfGPRotateMask(pFont, pMaskData, pFont->pMask);
}

static void OkfPrepareCharTables(OKFFONT *pFont)
{
  unsigned char *ptr=NULL;
  unsigned int i;
  int pos;

  // Initialize table pointers
  //- ptr = first byte after end of structure pHeader
  ptr = (unsigned char *) (pFont->pOkf + 1);
  pFont->charSet = ptr;
  ptr += pFont->pOkf->nbChars;
  pFont->charWidths = ptr;
  if (pFont->pOkf->indents)
  {
    ptr += pFont->pOkf->nbChars;
    pFont->charLeftIndents = ptr;
    ptr += pFont->pOkf->nbChars;
    pFont->charRightIndents = ptr;
  }

  // Initialize extended info
  for (i=0; i <= (OKF_LASTCHAR - OKF_FIRSTCHAR); i++)
  {
    pFont->extInfo[i].index = 0xFF;
    pFont->extInfo[i].position = 0;
    pFont->extInfo[i].width = 0;
    pFont->extInfo[i].leftIndent = 0;
    pFont->extInfo[i].rightIndent = 0;
  }
  
  // Calculate characters extended info to optimize speed
  for (i=0, pos=0; i < pFont->pOkf->nbChars; pos+=pFont->charWidths[i], i++)
  {
    pFont->extInfo[pFont->charSet[i] - OKF_FIRSTCHAR].index = i;
    pFont->extInfo[pFont->charSet[i] - OKF_FIRSTCHAR].position = pos;
    pFont->extInfo[pFont->charSet[i] - OKF_FIRSTCHAR].width = pFont->charWidths[i];
    if (pFont->pOkf->indents)
    {
      pFont->extInfo[pFont->charSet[i] - OKF_FIRSTCHAR].leftIndent = pFont->charLeftIndents[i];
      pFont->extInfo[pFont->charSet[i] - OKF_FIRSTCHAR].rightIndent = pFont->charRightIndents[i];
    }
  }

  // Other initializations
  pFont->pOkf->transColor |= 0x0001; // This library works with highlight ON
  pFont->pOkf->shadowColor |= 0x0001; // This library works with highlight ON
}

static int OkfRender16(int dx, int dy, int width, int height,
                       unsigned char *bitmap, int bitmapx, int bitmapy,
                       int bitmapWidth, int bitmapHeight, unsigned short transColor)
{
  int x, y, i, j;
  unsigned short *bmp;
  unsigned short color, bgColor;
  int red, green, blue;

  // Clip bitmap
  //- If completly out of limits, return
  if ((dx > okf.clip.x2) || (dy > okf.clip.y2))
    return 0;
  if ((dx + width - 1 < okf.clip.x1) || (dy + height - 1 < okf.clip.y1))
    return 0;

  //- Clip X and Width
  if (dx < okf.clip.x1)
  {
    width -= okf.clip.x1 - dx;
    bitmapx += okf.clip.x1 - dx;
    dx = okf.clip.x1;
  }
  if (dx + width - 1 > okf.clip.x2)
    width = okf.clip.x2 - dx + 1;

  //- Clip Y and Height
  if (dy < okf.clip.y1)
  {
    height -= okf.clip.y1 - dy;
    bitmapy += okf.clip.y1 - dy;
    dy = okf.clip.y1;
  }
  if (dy + height - 1 > okf.clip.y2)
    height = okf.clip.y2 - dy + 1;

  // Blit bitmap
  bmp = (unsigned short *) bitmap;
  for (y=bitmapHeight - (bitmapy + height), j=0; y < bitmapHeight - bitmapy; y++, j++)
    for (x=bitmapx, i=0; x < bitmapx + width; x++, i++)
    {
      color = bmp[y + x * bitmapHeight];

      if (color != transColor) {

        // Separate color components
        red   = GetRGBRed(color);
        green = GetRGBGreen(color);
        blue  = GetRGBBlue(color);

        // Modify color
        switch (okf.effect.type)
        {
          case okfOverlay:
            OkfOverlayColor(red, green, blue, okf.effect.color, okf.effect.intensity);
            break;
          case okfAddColor:
            OkfAddColor(red, green, blue, okf.effect.color);
            OkfAdjustColorComponents(red, green, blue);
            break;
          case okfSubtractColor:
            OkfSubtractColor(red, green, blue, okf.effect.color);
            OkfAdjustColorComponents(red, green, blue);
            break;
        }

        // Modify opacity, if required
        if (okf.opacity < 31)
        {
          // Merge color with background
          bgColor = GetPixel(&okf.pSurfaces[*okf.pCurSurface], dx + i, dy + height - j - 1);
          OkfUnderlayColor(red, green, blue, bgColor, okf.opacity);
        }

        // Draw píxel
        PutPixel(&okf.pSurfaces[*okf.pCurSurface], dx + i, dy + height - j - 1, RGB(red, green, blue));
      }
    }

  return 0;
}

static int OkfRenderMask16(int dx, int dy, int width, int height,
                           unsigned char *bitmap, int bitmapx,
                           int bitmapy, int bitmapWidth, int bitmapHeight,
                           unsigned char *mask)
{
  int x, y, i, j;
  unsigned short *bmp;
  unsigned short color, bgColor;
  unsigned char opacity;
  int red, green, blue;
  
  // Clip bitmap
  //- If completly out of limits, return
  if ((dx > okf.clip.x2) || (dy > okf.clip.y2))
    return 0;
  if ((dx + width - 1 < okf.clip.x1) || (dy + height - 1 < okf.clip.y1))
    return 0;

  //- Clip X and Width
  if (dx < okf.clip.x1)
  {
    width -= okf.clip.x1 - dx;
    bitmapx += okf.clip.x1 - dx;
    dx = okf.clip.x1;
  }
  if (dx + width - 1 > okf.clip.x2)
    width = okf.clip.x2 - dx + 1;

  //- Clip Y and Height
  if (dy < okf.clip.y1)
  {
    height -= okf.clip.y1 - dy;
    bitmapy += okf.clip.y1 - dy;
    dy = okf.clip.y1;
  }
  if (dy + height - 1 > okf.clip.y2)
    height = okf.clip.y2 - dy + 1;

  // Blit bitmap
  bmp = (unsigned short *) bitmap;
  for (y=bitmapHeight - (bitmapy + height), j=0; y < bitmapHeight - bitmapy; y++, j++)
    for (x=bitmapx, i=0; x < bitmapx + width; x++, i++)
    {
      opacity = mask[y + x * bitmapHeight];

      if (opacity > 0)
      {
        // Modify opacity, if required
        if (okf.opacity < 31)
          opacity = (opacity * okf.opacity) / 31;

        if (opacity > 0)
        {
          color = bmp[y + x * bitmapHeight];

          // Separate color components
          red   = GetRGBRed(color);
          green = GetRGBGreen(color);
          blue  = GetRGBBlue(color);

          // Modify color
          switch (okf.effect.type)
          {
            case okfOverlay:
              OkfOverlayColor(red, green, blue, okf.effect.color, okf.effect.intensity);
              break;
            case okfAddColor:
              OkfAddColor(red, green, blue, okf.effect.color);
              OkfAdjustColorComponents(red, green, blue);
              break;
            case okfSubtractColor:
              OkfSubtractColor(red, green, blue, okf.effect.color);
              OkfAdjustColorComponents(red, green, blue);
              break;
          }

          // Merge color with background
          bgColor = GetPixel(&okf.pSurfaces[*okf.pCurSurface], dx + i, dy + height - j - 1);
          OkfUnderlayColor(red, green, blue, bgColor, opacity);

          // Draw píxel
          PutPixel(&okf.pSurfaces[*okf.pCurSurface], dx + i, dy + height - j - 1, RGB(red, green, blue));
        }
      }
    }

  return 0;
}

static int OkfRenderShadow16(int dx, int dy, int width, int height,
                             unsigned char *mask, int maskx, int masky,
                             int maskWidth, int maskHeight,
                             unsigned short shadowColor)
{
  int x, y, i, j;
  unsigned short bgColor;
  unsigned char opacity;
  int red, green, blue;
  int shadowRed, shadowGreen, shadowBlue;
  
  // Clip mask
  //- If completly out of limits, return
  if ((dx > okf.clip.x2) || (dy > okf.clip.y2))
    return 0;
  if ((dx + width - 1 < okf.clip.x1) || (dy + height - 1 < okf.clip.y1))
    return 0;

  //- Clip X and Width
  if (dx < okf.clip.x1)
  {
    width -= okf.clip.x1 - dx;
    maskx += okf.clip.x1 - dx;
    dx = okf.clip.x1;
  }
  if (dx + width - 1 > okf.clip.x2)
    width = okf.clip.x2 - dx + 1;

  //- Clip Y and Height
  if (dy < okf.clip.y1)
  {
    height -= okf.clip.y1 - dy;
    masky += okf.clip.y1 - dy;
    dy = okf.clip.y1;
  }
  if (dy + height - 1 > okf.clip.y2)
    height = okf.clip.y2 - dy + 1;

  // Here the only color to manage it's the font shadow color
  shadowRed   = GetRGBRed(shadowColor);
  shadowGreen = GetRGBGreen(shadowColor);
  shadowBlue  = GetRGBBlue(shadowColor);

  //- Modify color
  switch (okf.shadow.effect.type)
  {
    case okfOverlay:
      OkfOverlayColor(shadowRed, shadowGreen, shadowBlue,
                      okf.shadow.effect.color, okf.shadow.effect.intensity);
      break;
    case okfAddColor:
      OkfAddColor(shadowRed, shadowGreen, shadowBlue, okf.shadow.effect.color);
      OkfAdjustColorComponents(shadowRed, shadowGreen, shadowBlue);
      break;
    case okfSubtractColor:
      OkfSubtractColor(shadowRed, shadowGreen, shadowBlue, okf.shadow.effect.color);
      OkfAdjustColorComponents(shadowRed, shadowGreen, shadowBlue);
      break;
  }

  // Blit mask
  for (y=maskHeight - (masky + height), j=0; y < maskHeight - masky; y++, j++)
    for (x=maskx, i=0; x < maskx + width; x++, i++)
    {
      opacity = mask[y + x * maskHeight];

      if (opacity > 0)
      {
        // Modify opacity, if required
        if (okf.shadow.opacity < 31)
          opacity = (opacity * okf.shadow.opacity) / 31;

        if (opacity > 0)
        {
          // Merge color with background
          bgColor = GetPixel(&okf.pSurfaces[*okf.pCurSurface], dx + i, dy + height - j - 1);

          // We want to preserve the shadow color so we Overlay manually
          red   = (shadowRed   * opacity) / 31 + (GetRGBRed(bgColor)   * (31 - opacity)) / 31;
          green = (shadowGreen * opacity) / 31 + (GetRGBGreen(bgColor) * (31 - opacity)) / 31;
          blue  = (shadowBlue  * opacity) / 31 + (GetRGBBlue(bgColor)  * (31 - opacity)) / 31;

          // Draw píxel
          PutPixel(&okf.pSurfaces[*okf.pCurSurface], dx + i, dy + height - j - 1, RGB(red, green, blue));
        }
      }
    }

  return 0;
}

/*****************************************************************/

int OkfPrintSurface(unsigned char *pStr, int surface)
{
  OKFFONT *pFont;

  if (!okf.initialized)
  {
    okf.lastError = okfNotInitialized;
    return -1;
  }

  // Get current font
  pFont = OkfGetCurrentFont();
  if (!pFont)
  {
    okf.lastError = okfNoCurrentFont;
    return -1;
  }

  if (okf.justify == okfJustifyLeft)
    return OkfDoPrintSurface(pFont, pStr, surface);
  else
    return OkfDoPrintJustifiedSurface(pFont, pStr, surface);
}

static int OkfDoPrintSurface(OKFFONT *pFont, unsigned char *pStr, int surface)
{
  OKFHEADER *pFOkf;
  OKFCHAR *pCh;
  int charSep, totalLineHeight, shadowXPos, shadowYPos, shadowYEndPos;
  int startX, startY;
  int pass, before, after, newLine;
  unsigned char *ptr;
  GPDRAWTAG drawTag;

  // Initialize clipping in case of using standard SDK functions
  drawTag.clip_x = okf.clip.x1;
  drawTag.clip_y = okf.clip.y1;
  drawTag.clip_w = okf.clip.x2 - okf.clip.x1 + 1;
  drawTag.clip_h = okf.clip.y2 - okf.clip.y1 + 1;
  
  // If font has a shadow mask, the rendering will be
  // executed in 2 passes, otherwise in 1
  pFOkf = pFont->pOkf;
  if (pFOkf->alphaMask && pFOkf->shadowMask && okf.useMask && okf.shadow.opacity)
    pass = 2;
  else
    pass = 1;

  // Other initializations to speed up things
  charSep = pFOkf->charSeparation + okf.charSeparation;
  totalLineHeight = pFOkf->defaultHeight + pFOkf->lineSeparation + okf.lineSeparation;
  if (pass == 2)
  {
    shadowXPos = okf.shadow.x + pFOkf->shadowOX + pFOkf->shadowXPos;
    shadowYPos = okf.shadow.y + pFOkf->shadowOY + pFOkf->shadowYPos;
    shadowYEndPos = shadowYPos + pFOkf->defaultHeight + pFOkf->maskHeightInc;
  }
  
  // Loop on different passes
  startX = okf.x;
  startY = okf.y;
  for (; pass >= 1; pass--)
  {
    // Main loop on string
    okf.x = startX;
    okf.y = startY;
    before = after = FALSE;
    newLine = TRUE;
    for (ptr=pStr; *ptr; ptr++)
    {
      // If new line, control its visibility
      if (newLine && !after)
        if (pass == 1)
        {
          // Control text vertical position
          before = (okf.y + pFOkf->defaultHeight - 1 < okf.clip.y1);
          if (!before)
            after = (okf.y > okf.clip.y2);
          else
            after = FALSE;
        }
        else 
        {
          // Control mask vertical position
          before = (okf.y + shadowYEndPos - 1 < okf.clip.y1);
          if (!before)
            after = (okf.y + shadowYPos > okf.clip.y2);
          else
            after = FALSE;
        }

      // If line is not visible, jump to next line
      if (before || after)
      {
        for (; *ptr; ptr++)
          if (*ptr == '\n')
          {
            okf.y += totalLineHeight;
            newLine = TRUE;
            break;
          }
          
        // If it was the last line of the text, simulate the printing
        // to have an accurate final okf.x position, even if it's not visible
        // (Note: calculation is made from end to begin of line)
        if (!*ptr)
        {
          // Only for text, not for shadow
          if (pass == 1)
          {
            newLine = TRUE;
            while ((--ptr >= pStr) && (*ptr != '\n'))
            {
              pCh = &pFont->extInfo[*ptr - OKF_FIRSTCHAR];
              if (pCh->index != 0xFF)
              {
                if (!newLine)
                  okf.x += pCh->rightIndent;
                else
                  newLine = FALSE;
                okf.x += charSep + pCh->leftIndent + pCh->width;
              }
              else
              {
                if (newLine)
                  newLine = FALSE;
                okf.x += charSep + pFOkf->defaultWidth;
              }
            }
          }
          
          // We jump outside the main loop
          break;
        }
      } 
        
      // If line is visible, print char
      else
        if (*ptr == '\n')
        {
          okf.x = startX;
          okf.y += totalLineHeight;
          newLine = TRUE;
        }
        else
        {
          pCh = &pFont->extInfo[*ptr - OKF_FIRSTCHAR];
          if (pCh->index != 0xFF)
          {
            if (!newLine)
              okf.x += pCh->leftIndent;
            else
              newLine = FALSE;
  
            // Render shadow, visibility has been controled before 
            if (pass == 2)
                OkfRenderShadowSurface16(okf.x + shadowXPos,
                                  okf.y + shadowYPos,
                                  pCh->width + pFOkf->maskWidthInc,
                                  pFOkf->defaultHeight + pFOkf->maskHeightInc,
                                  pFont->pMask,
                                  pCh->position + pCh->index * pFOkf->maskWidthInc, 0,
                                  pFont->maskWidth, pFont->maskHeight,
                                  pFOkf->shadowColor,
								  surface);
  
            // Render character, if its visible
            else if (okf.opacity > 0)
              if (pFOkf->alphaMask && !pFOkf->shadowMask && okf.useMask)
                OkfRenderMaskSurface16(okf.x, okf.y, pCh->width, pFOkf->defaultHeight,
                                pFont->pBitmap, pCh->position, 0,
                                pFont->bitmapWidth, pFont->bitmapHeight,
                                pFont->pMask,
								surface);
              else
                if ((okf.effect.type != okfNone) || (okf.opacity < 31))
                  OkfRenderSurface16(okf.x, okf.y, pCh->width, pFOkf->defaultHeight, 
                              pFont->pBitmap, pCh->position, 0, 
                              pFont->bitmapWidth, pFont->bitmapHeight,
                              pFOkf->transColor,
							  surface);
                else
                  // To speed up rendering we use SDK when possible 
                  GpTransBlt16(&drawTag, &okf.pSurfaces[surface], 
                               okf.x, okf.y, pCh->width, pFOkf->defaultHeight, 
                               pFont->pBitmap, pCh->position, 0, 
                               pFont->bitmapWidth, pFont->bitmapHeight,
                               pFOkf->transColor);
  
            okf.x += pCh->width + pCh->rightIndent + charSep;
          }
          else
          {
            if (newLine)
              newLine = FALSE;
          
            okf.x += pFOkf->defaultWidth + charSep;
          }
        }
    }
  }

  okf.lastError = okfOk;
  return 0;
}

static int OkfDoPrintJustifiedSurface(OKFFONT *pFont, unsigned char *pStr, int surface)
{
  unsigned char str[OKF_MAXLINE];
  unsigned char *pBegin, *pEnd;
  unsigned char ch;
  int left;

  // Get each text line
  gm_strcpy(str, pStr);
  left = okf.x;
  pBegin = pEnd = str;
  do
  {
    if ((*pEnd == '\n') || (*pEnd == '\0'))
    {
      // Forces end of string
      ch = *pEnd;
      *pEnd = '\0';

      // Calculate starting x position
      switch (okf.justify)
      {
        case okfJustifyLeft:  okf.x = left; break;
        case okfCenter:       okf.x = left - (OkfDoGetTextWidth(pFont, pBegin) / 2); break;
        case okfJustifyRight: okf.x = left - OkfDoGetTextWidth(pFont, pBegin); break;
      }

      // Print line
      if (OkfDoPrintSurface(pFont, pBegin, surface) == -1)
        return -1;

      // Recover end of line and jump line if needed
      if ((*pEnd = ch) == '\n')
        okf.y += pFont->pOkf->defaultHeight + pFont->pOkf->lineSeparation + okf.lineSeparation;

      // Next line
      pBegin = pEnd + 1;
    }
  } while (*(pEnd++) != '\0');

  return 0;
}

static int OkfRenderSurface16(int dx, int dy, int width, int height,
                       unsigned char *bitmap, int bitmapx, int bitmapy,
                       int bitmapWidth, int bitmapHeight, unsigned short transColor,
					   int surface)
{
  int x, y, i, j;
  unsigned short *bmp;
  unsigned short color, bgColor;
  int red, green, blue;

  // Clip bitmap
  //- If completly out of limits, return
  if ((dx > okf.clip.x2) || (dy > okf.clip.y2))
    return 0;
  if ((dx + width - 1 < okf.clip.x1) || (dy + height - 1 < okf.clip.y1))
    return 0;

  //- Clip X and Width
  if (dx < okf.clip.x1)
  {
    width -= okf.clip.x1 - dx;
    bitmapx += okf.clip.x1 - dx;
    dx = okf.clip.x1;
  }
  if (dx + width - 1 > okf.clip.x2)
    width = okf.clip.x2 - dx + 1;

  //- Clip Y and Height
  if (dy < okf.clip.y1)
  {
    height -= okf.clip.y1 - dy;
    bitmapy += okf.clip.y1 - dy;
    dy = okf.clip.y1;
  }
  if (dy + height - 1 > okf.clip.y2)
    height = okf.clip.y2 - dy + 1;

  // Blit bitmap
  bmp = (unsigned short *) bitmap;
  for (y=bitmapHeight - (bitmapy + height), j=0; y < bitmapHeight - bitmapy; y++, j++)
    for (x=bitmapx, i=0; x < bitmapx + width; x++, i++)
    {
      color = bmp[y + x * bitmapHeight];

      if (color != transColor) {

        // Separate color components
        red   = GetRGBRed(color);
        green = GetRGBGreen(color);
        blue  = GetRGBBlue(color);

        // Modify color
        switch (okf.effect.type)
        {
          case okfOverlay:
            OkfOverlayColor(red, green, blue, okf.effect.color, okf.effect.intensity);
            break;
          case okfAddColor:
            OkfAddColor(red, green, blue, okf.effect.color);
            OkfAdjustColorComponents(red, green, blue);
            break;
          case okfSubtractColor:
            OkfSubtractColor(red, green, blue, okf.effect.color);
            OkfAdjustColorComponents(red, green, blue);
            break;
        }

        // Modify opacity, if required
        if (okf.opacity < 31)
        {
          // Merge color with background
          bgColor = GetPixel(&okf.pSurfaces[surface], dx + i, dy + height - j - 1);
          OkfUnderlayColor(red, green, blue, bgColor, okf.opacity);
        }

        // Draw píxel
        PutPixel(&okf.pSurfaces[surface], dx + i, dy + height - j - 1, RGB(red, green, blue));
      }
    }

  return 0;
}

static int OkfRenderMaskSurface16(int dx, int dy, int width, int height,
                           unsigned char *bitmap, int bitmapx,
                           int bitmapy, int bitmapWidth, int bitmapHeight,
                           unsigned char *mask, 
						   int surface)
{
  int x, y, i, j;
  unsigned short *bmp;
  unsigned short color, bgColor;
  unsigned char opacity;
  int red, green, blue;
  
  // Clip bitmap
  //- If completly out of limits, return
  if ((dx > okf.clip.x2) || (dy > okf.clip.y2))
    return 0;
  if ((dx + width - 1 < okf.clip.x1) || (dy + height - 1 < okf.clip.y1))
    return 0;

  //- Clip X and Width
  if (dx < okf.clip.x1)
  {
    width -= okf.clip.x1 - dx;
    bitmapx += okf.clip.x1 - dx;
    dx = okf.clip.x1;
  }
  if (dx + width - 1 > okf.clip.x2)
    width = okf.clip.x2 - dx + 1;

  //- Clip Y and Height
  if (dy < okf.clip.y1)
  {
    height -= okf.clip.y1 - dy;
    bitmapy += okf.clip.y1 - dy;
    dy = okf.clip.y1;
  }
  if (dy + height - 1 > okf.clip.y2)
    height = okf.clip.y2 - dy + 1;

  // Blit bitmap
  bmp = (unsigned short *) bitmap;
  for (y=bitmapHeight - (bitmapy + height), j=0; y < bitmapHeight - bitmapy; y++, j++)
    for (x=bitmapx, i=0; x < bitmapx + width; x++, i++)
    {
      opacity = mask[y + x * bitmapHeight];

      if (opacity > 0)
      {
        // Modify opacity, if required
        if (okf.opacity < 31)
          opacity = (opacity * okf.opacity) / 31;

        if (opacity > 0)
        {
          color = bmp[y + x * bitmapHeight];

          // Separate color components
          red   = GetRGBRed(color);
          green = GetRGBGreen(color);
          blue  = GetRGBBlue(color);

          // Modify color
          switch (okf.effect.type)
          {
            case okfOverlay:
              OkfOverlayColor(red, green, blue, okf.effect.color, okf.effect.intensity);
              break;
            case okfAddColor:
              OkfAddColor(red, green, blue, okf.effect.color);
              OkfAdjustColorComponents(red, green, blue);
              break;
            case okfSubtractColor:
              OkfSubtractColor(red, green, blue, okf.effect.color);
              OkfAdjustColorComponents(red, green, blue);
              break;
          }

          // Merge color with background
          bgColor = GetPixel(&okf.pSurfaces[surface], dx + i, dy + height - j - 1);
          OkfUnderlayColor(red, green, blue, bgColor, opacity);

          // Draw píxel
          PutPixel(&okf.pSurfaces[surface], dx + i, dy + height - j - 1, RGB(red, green, blue));
        }
      }
    }

  return 0;
}

static int OkfRenderShadowSurface16(int dx, int dy, int width, int height,
                             unsigned char *mask, int maskx, int masky,
                             int maskWidth, int maskHeight,
                             unsigned short shadowColor, int surface)
{
  int x, y, i, j;
  unsigned short bgColor;
  unsigned char opacity;
  int red, green, blue;
  int shadowRed, shadowGreen, shadowBlue;
  
  // Clip mask
  //- If completly out of limits, return
  if ((dx > okf.clip.x2) || (dy > okf.clip.y2))
    return 0;
  if ((dx + width - 1 < okf.clip.x1) || (dy + height - 1 < okf.clip.y1))
    return 0;

  //- Clip X and Width
  if (dx < okf.clip.x1)
  {
    width -= okf.clip.x1 - dx;
    maskx += okf.clip.x1 - dx;
    dx = okf.clip.x1;
  }
  if (dx + width - 1 > okf.clip.x2)
    width = okf.clip.x2 - dx + 1;

  //- Clip Y and Height
  if (dy < okf.clip.y1)
  {
    height -= okf.clip.y1 - dy;
    masky += okf.clip.y1 - dy;
    dy = okf.clip.y1;
  }
  if (dy + height - 1 > okf.clip.y2)
    height = okf.clip.y2 - dy + 1;

  // Here the only color to manage it's the font shadow color
  shadowRed   = GetRGBRed(shadowColor);
  shadowGreen = GetRGBGreen(shadowColor);
  shadowBlue  = GetRGBBlue(shadowColor);

  //- Modify color
  switch (okf.shadow.effect.type)
  {
    case okfOverlay:
      OkfOverlayColor(shadowRed, shadowGreen, shadowBlue,
                      okf.shadow.effect.color, okf.shadow.effect.intensity);
      break;
    case okfAddColor:
      OkfAddColor(shadowRed, shadowGreen, shadowBlue, okf.shadow.effect.color);
      OkfAdjustColorComponents(shadowRed, shadowGreen, shadowBlue);
      break;
    case okfSubtractColor:
      OkfSubtractColor(shadowRed, shadowGreen, shadowBlue, okf.shadow.effect.color);
      OkfAdjustColorComponents(shadowRed, shadowGreen, shadowBlue);
      break;
  }

  // Blit mask
  for (y=maskHeight - (masky + height), j=0; y < maskHeight - masky; y++, j++)
    for (x=maskx, i=0; x < maskx + width; x++, i++)
    {
      opacity = mask[y + x * maskHeight];

      if (opacity > 0)
      {
        // Modify opacity, if required
        if (okf.shadow.opacity < 31)
          opacity = (opacity * okf.shadow.opacity) / 31;

        if (opacity > 0)
        {
          // Merge color with background
          bgColor = GetPixel(&okf.pSurfaces[surface], dx + i, dy + height - j - 1);

          // We want to preserve the shadow color so we Overlay manually
          red   = (shadowRed   * opacity) / 31 + (GetRGBRed(bgColor)   * (31 - opacity)) / 31;
          green = (shadowGreen * opacity) / 31 + (GetRGBGreen(bgColor) * (31 - opacity)) / 31;
          blue  = (shadowBlue  * opacity) / 31 + (GetRGBBlue(bgColor)  * (31 - opacity)) / 31;

          // Draw píxel
          PutPixel(&okf.pSurfaces[surface], dx + i, dy + height - j - 1, RGB(red, green, blue));
        }
      }
    }

  return 0;
}

