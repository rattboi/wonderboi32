/*************************************************************************

  fileSelector.c

    Configurable file selector for the Game Park GP32.
    Uses the OKF Font Engine to print texts.
    fileSelector.c is part of the Oankali GP32 run-time library.

  Copyright (c) 2004 by
  Guillem Reinoso <oankali@andorra.ad>

  These software is freeware and is provided as is.
  If it doesn't work for you or it makes your PC melt down, it's not my
  responsibility.
  If you use it in your programs, cite me please.

**************************************************************************/


#include <string.h>

#include <gpdef.h>
#include <gpstdio.h>
#include <gpstdlib.h>
#include <gpmem.h>
#include <gpgraphic.h>
#include <gpgraphic16.h>

#include "global.h"
#include "graphics16.h"
#include "okf.h"

#include "fileSelector.h"


//*** Private macro definitions


//*** Private prototypes
static void RefreshScreen(FILESELECTOR *pSelector, FOLDER *pFolder);


//*** Public implementations
int FileSelector(FILESELECTOR *pSelector)
{
  FOLDER folder;
  FOLDERENTRY *pEntry;
  FLDERROR error;
  char currentPath[FLD_MAXPATH + 1];
  char *pCurrPath;
  int tick, keyTick=0;
  int pressedKeys, newKeys, oldKeys;
  int result;

  // Init structure private members
  pSelector->pathSepLineTop = pSelector->listTop - (pSelector->listTop - (pSelector->pathTop + OkfGetFont(pSelector->pathFont)->pOkf->maxHeight)) / 2 - 1;
  pSelector->totalsSepLineTop = pSelector->totalsTop - (pSelector->totalsTop - (pSelector->listTop + pSelector->listSize * pSelector->selectionHeight)) / 2 - 1;
  pSelector->selectionLeft = pSelector->fileLeft - pSelector->selectionMargin;
  pSelector->selectionWidh = (pSelector->sizeRight - pSelector->fileLeft + 1) + pSelector->selectionMargin * 2;
  pSelector->scrollBarHeight = pSelector->listSize * pSelector->selectionHeight;

  // Init current path
  if (pSelector->trackPath)
  {
    pCurrPath = pSelector->currentPath;
  }
  else
  {
    gm_strcpy(currentPath, pSelector->currentPath);
    pCurrPath = currentPath;
  }

  // Prepare screen
  folder.pEntries = NULL;
  RefreshScreen(pSelector, &folder);

  // Get folder files & folders
  error = FolderGet(pCurrPath, pSelector->pFilters, pSelector->options, &folder);
  if (error != fldOk)
  {
    FolderDrop(&folder);
    return -1;
  }
  folder.pTop = folder.pCurrent = folder.pEntries->pFirst;

  // Main loop
  while (GpKeyGet());
  while(TRUE)
  {
    tick = GpTickCountGet();
    
    // Prepare screen
    RefreshScreen(pSelector, &folder);

    // Get buttons state
    oldKeys = pressedKeys;
    newKeys = pressedKeys = GpKeyGet();

    // Filter buttons
    if (!folder.pCurrent)
      newKeys = newKeys & GPC_VK_FB;
    else
    {
      // In some cases A same as SELECT
      if (newKeys & GPC_VK_FA)
        if (!(pSelector->options & FLD_EXPLORE))   // If not explore, A same as SELECT
          newKeys = (newKeys & ~GPC_VK_FA) | GPC_VK_SELECT;
        else                                      // If explore, A same as SELECT for files only
          if (!(((FOLDERENTRY *) folder.pCurrent->pObject)->attr.attr & 0x10))
            newKeys = (newKeys & ~GPC_VK_FA) | GPC_VK_SELECT;

      // Control first an last item of the list
      if (!folder.pCurrent->pPrevious && (oldKeys & GPC_VK_UP))
        newKeys = newKeys & ~GPC_VK_UP;
      if (!folder.pCurrent->pNext && (oldKeys & GPC_VK_DOWN))
        newKeys = newKeys & ~GPC_VK_DOWN;

      // Special key delay for first displacements
      if (!(newKeys & (GPC_VK_UP | GPC_VK_DOWN | GPC_VK_LEFT | GPC_VK_RIGHT)))
        keyTick = 0;
      else
        if (keyTick == 0)
          keyTick = GpTickCountGet();
        else
          if (keyTick != -1)
            if (GpTickCountGet() - keyTick < pSelector->repetitionDelay)
              newKeys = newKeys & ~(GPC_VK_UP | GPC_VK_DOWN | GPC_VK_LEFT | GPC_VK_RIGHT);
            else
              keyTick = -1;
    }

    // Manage buttons
    //- Cancel file/folder selection
    if (newKeys & GPC_VK_FB)
    {
      result = 0;
      break;
    }
    //- Move to previous file/folder
    else if (newKeys & GPC_VK_UP)
    {
      if (folder.pCurrent->pPrevious)
      {
        if (folder.pTop == folder.pCurrent)
          folder.pTop = folder.pTop->pPrevious;
        folder.pCurrent = folder.pCurrent->pPrevious;
      }
      else
      {
        folder.pCurrent = folder.pEntries->pLast;
        if (folder.pEntries->count > pSelector->listSize)
          folder.pTop = ListGetItemByIndex(folder.pEntries,
                                           folder.pEntries->count - pSelector->listSize);
      }
    }
    //- Move to next file/folder
    else if (newKeys & GPC_VK_DOWN)
    {
      if (folder.pCurrent->pNext)
      {
        folder.pCurrent = folder.pCurrent->pNext;
        if (ListGetRelativePosition(folder.pCurrent, folder.pTop) >= pSelector->listSize)
          folder.pTop = ListGetItemByIndex(folder.pEntries,
                                           ListGetIndex(folder.pCurrent) - pSelector->listSize + 1);
      }
      else
      {
        folder.pCurrent = folder.pEntries->pFirst;
        folder.pTop = folder.pCurrent;
      }
    }
    //- Move to previous N file/folder
    else if (newKeys & GPC_VK_LEFT)
    {
      if (folder.pEntries->count > pSelector->listSize)
        if (ListGetIndex(folder.pTop) > pSelector->listSize)
          folder.pTop = ListGetRelativeItem(folder.pTop, -pSelector->listSize);
        else
          folder.pTop = folder.pEntries->pFirst;

      if (ListGetIndex(folder.pCurrent) > pSelector->listSize)
        folder.pCurrent = ListGetRelativeItem(folder.pCurrent, -pSelector->listSize);
      else
        folder.pCurrent = folder.pTop;
    }
    //- Move to next N file/folder
    else if (newKeys & GPC_VK_RIGHT)
    {
      if (folder.pEntries->count > pSelector->listSize)
        if (ListGetIndex(folder.pTop) < folder.pEntries->count - pSelector->listSize * 2 - 1)
          folder.pTop = ListGetRelativeItem(folder.pTop, pSelector->listSize);
        else
          folder.pTop = ListGetRelativeItem(folder.pEntries->pLast, -pSelector->listSize + 1);

      if (ListGetIndex(folder.pCurrent) < folder.pEntries->count - pSelector->listSize)
        folder.pCurrent = ListGetRelativeItem(folder.pCurrent, pSelector->listSize);
      else
        folder.pCurrent = folder.pEntries->pLast;
    }
    //- Get selected file/folder
    else if (newKeys & GPC_VK_SELECT)
    {
      pEntry = (FOLDERENTRY *) folder.pCurrent->pObject;

      // folder .. can't be selected
      if (!((pEntry->filename[0] == '.') && 
            (pEntry->filename[1] == '.') && 
            (pEntry->filename[2] == '\0')))
        if ((pSelector->options & FLD_FOLDERS) &&  (pEntry->attr.attr & 0x10) || 
            (pSelector->options & FLD_FILES)   && !(pEntry->attr.attr & 0x10))
          {
            gm_strcat(gm_strcpy(pSelector->filename, folder.path), pEntry->filename);
            result = 1;
            break;
          }
    }
    //- Explore new folder
    else if (newKeys & GPC_VK_FA)
    {
      pEntry = (FOLDERENTRY *) folder.pCurrent->pObject;

      if (pEntry->attr.attr & 0x10)
      {
        // Folder up
        if ((pEntry->filename[0] == '.') && 
            (pEntry->filename[1] == '.') && 
            (pEntry->filename[2] == '\0'))
        {
          gm_strcpy(pCurrPath, folder.path);
          pCurrPath[gm_lstrlen(pCurrPath) - 1] = '\0';
          *strrchr(pCurrPath, '\\') = '\0';
        }
        // Folder down
        else
          gm_strcat(gm_strcpy(pCurrPath, folder.path), pEntry->filename); 

        // Move to folder
        FolderDrop(&folder);
        error = FolderGet(pCurrPath, pSelector->pFilters, pSelector->options, &folder);
        if (error != fldOk)
        {
          // show message
          result = -1;
          break;
        }
        folder.pTop = folder.pCurrent = folder.pEntries->pFirst;
        while (GpKeyGet());
      }
    }

    while ((GpTickCountGet() - tick) < pSelector->repetitionSpeed);
  }

  // Release memory
  FolderDrop(&folder);

  return result;
}

void FileSelectorInit(FILESELECTOR *pSelector)
{
  gm_strcpy(pSelector->currentPath, "gp:\\");
  pSelector->filename[0] =     '\0';
  pSelector->trackPath =       TRUE;
  pSelector->pFilters =        "*.*";
  pSelector->options =         FLD_ALL;

  pSelector->pFileSizeFormat = "%s byte(s)";
  pSelector->pTotalsFormat =   "%s byte(s) in %s files(s)";

  pSelector->pathLeft =        9;
  pSelector->pathTop =         7;
  pSelector->listTop =         35;
  pSelector->listSize =        10;
  pSelector->fileLeft =        30;
  pSelector->fileYDisplacement = 1;
  pSelector->sizeRight =       282;
  pSelector->sizeYDisplacement = 1;
  pSelector->totalsLeft =      9;
  pSelector->totalsTop =       220;
  pSelector->scrollBarLeft =   311;
  pSelector->selectionHeight = 17;
  pSelector->selectionMargin = 20;

  pSelector->showPath =        TRUE;
  pSelector->showExtensions =  TRUE;
  pSelector->showSizes =       TRUE;
  pSelector->showTotals =      TRUE;
  pSelector->showScrollBar =   TRUE;
  pSelector->showSeparationLines = TRUE;

  pSelector->pathFont =        1;
  pSelector->folderFont =      1;
  pSelector->fileFont =        1;
  pSelector->sizesFont =       1;
  pSelector->totalsFont =      1;

  pSelector->pBackground =     NULL;
  pSelector->backColor =       RGB(7, 22, 22);
  pSelector->separationLinesColor = RGB(0, 10, 10);
  pSelector->selectionColor =  RGB(31, 31, 31);
  pSelector->selectionIntensity = 32;
  pSelector->scrollBarColor =  RGB(31, 31, 31);

  pSelector->repetitionDelay = 300;
  pSelector->repetitionSpeed = 30;
}


//*** Private implementations
static void RefreshScreen(FILESELECTOR *pSelector, FOLDER *pFolder)
{
  LISTITEM *pItem;
  FOLDERENTRY *pEntry;
  int pos;
  char str[40], str1[10], str2[10], *pPos;

  OkfSetToDefaults();

  // Background
  if (pSelector->pBackground)
    GpBitBlt16(NULL, &okf.pSurfaces[*okf.pCurSurface], 0, 0, GPC_LCD_WIDTH, GPC_LCD_HEIGHT,
               (unsigned char *) pSelector->pBackground, 0, 0, GPC_LCD_WIDTH, GPC_LCD_HEIGHT);
  else
    GpRectFill16(NULL, &okf.pSurfaces[*okf.pCurSurface], 0, 0, GPC_LCD_WIDTH, GPC_LCD_HEIGHT, pSelector->backColor);

  // Current path
  if (pSelector->showPath)
  {
    if (pFolder->pEntries)
    {
      okf.font = pSelector->pathFont;
      OkfPrintAt(pSelector->pathLeft, pSelector->pathTop, pFolder->path);
    }

    if (pSelector->showSeparationLines)
      GpRectFill16(NULL, &okf.pSurfaces[*okf.pCurSurface], 0, pSelector->pathSepLineTop,
                   GPC_LCD_WIDTH, 2, pSelector->separationLinesColor);
  }

  // Folders and files
  if (pFolder->pEntries)
  {
    pos = 0;
    pItem = pFolder->pTop;
    while (pItem && (pos < pSelector->listSize))
    {
      pEntry = (FOLDERENTRY *) pItem->pObject;

      // if file/folder is selected
      if (pItem == pFolder->pCurrent)
        if (pSelector->selectionIntensity >= 32)
          GpRectFill16(NULL, &okf.pSurfaces[*okf.pCurSurface],
                       pSelector->selectionLeft, pSelector->listTop + pos * pSelector->selectionHeight,
                       pSelector->selectionWidh, pSelector->selectionHeight,
                       pSelector->selectionColor);
        else
          OverlayColor16(&okf.pSurfaces[*okf.pCurSurface],
                         pSelector->selectionLeft, pSelector->listTop + pos * pSelector->selectionHeight,
                         pSelector->selectionWidh, pSelector->selectionHeight,
                         pSelector->selectionColor, pSelector->selectionIntensity);

      // remove file extension, if required
      gm_strcpy(str, pEntry->filename);
      if (!pSelector->showExtensions && !(pEntry->attr.attr & 0x10))
        if (pPos = strchr(str, '.'))
          *pPos = '\0';

      // file/folder name
      okf.font = (pEntry->attr.attr & 0x10) ? pSelector->folderFont : pSelector->fileFont;
      OkfPrintAt(pSelector->fileLeft,
                 pSelector->listTop + pos * pSelector->selectionHeight + pSelector->fileYDisplacement,
                 str);

      // file size, if needed
      if (pSelector->showSizes && !(pEntry->attr.attr & 0x10))
      {
        gm_sprintf(str1, "%ld", pEntry->attr.size);
        gm_sprintf(str, pSelector->pFileSizeFormat, Thousands(str1));
        okf.justify = okfJustifyRight;
        okf.font = pSelector->sizesFont;
        OkfPrintAt(pSelector->sizeRight,
                   pSelector->listTop + pos * pSelector->selectionHeight + pSelector->sizeYDisplacement,
                   str);
        okf.justify = okfJustifyLeft;
      }

      pItem = pItem->pNext;
      pos++;
    }
  }

  // Scroll bar
  if (pSelector->showScrollBar && pFolder->pEntries)
  {
    GpRectFill16(NULL, &okf.pSurfaces[*okf.pCurSurface], pSelector->scrollBarLeft, pSelector->listTop,
                 2, pSelector->scrollBarHeight, pSelector->scrollBarColor);
    if (pFolder->pCurrent)
    {
      pos = 0;
      if (pFolder->pEntries->count > 1)
        pos = pSelector->scrollBarHeight * 1000 / (pFolder->pEntries->count - 1);
      pos = pSelector->listTop + ListGetIndex(pFolder->pCurrent) * pos / 1000 - 2;
      GpRectFill16(NULL, &okf.pSurfaces[*okf.pCurSurface], pSelector->scrollBarLeft    , pos - 2, 2, 10, pSelector->backColor);
      GpRectFill16(NULL, &okf.pSurfaces[*okf.pCurSurface], pSelector->scrollBarLeft - 2, pos    , 6,  6, pSelector->scrollBarColor);
      GpRectFill16(NULL, &okf.pSurfaces[*okf.pCurSurface], pSelector->scrollBarLeft - 1, pos - 1, 4,  8, pSelector->scrollBarColor);
    }
  }

  // Totals
  if (pSelector->showTotals)
  {
    if (pSelector->showSeparationLines)
      GpRectFill16(NULL, &okf.pSurfaces[*okf.pCurSurface], 0, pSelector->totalsSepLineTop,
                   GPC_LCD_WIDTH, 2, pSelector->separationLinesColor);

    if (pFolder->pEntries)
    {
      gm_sprintf(str1, "%ld", pFolder->totalSize);
      gm_sprintf(str2, "%ld", pFolder->totalFiles);
      gm_sprintf(str, pSelector->pTotalsFormat, Thousands(str1), Thousands(str2));
      okf.font = pSelector->totalsFont;
      OkfPrintAt(pSelector->totalsLeft, pSelector->totalsTop, str);
    }
  }

  // Shows the result
  GpSurfaceFlip(&okf.pSurfaces[*okf.pCurSurface]);
  *okf.pCurSurface ^= 0x01;
}

