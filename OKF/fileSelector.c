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
#include <gpmain.h>

#include "global.h"
#include "graphics16.h"
#include "okf.h"

#include "fileSelector.h"

#include "filecache.h"

extern struct { int crc; int size; char *name; } MyDat[];


//*** Private macro definitions


//*** Private prototypes
static void RefreshScreen(FILESELECTOR *pSelector, int topEntry, int selectedEntry);


//*** Public implementations
int FileSelector(FILESELECTOR *pSelector, unsigned char *dest)
{
	int topEntry, selectedEntry;
	char currentPath[FLD_MAXPATH + 1];
	int tick, keyTick=0;
	int pressedKeys, newKeys, oldKeys;
	int result;
	
	// Init structure private members
	pSelector->pathSepLineTop = pSelector->listTop - (pSelector->listTop - (pSelector->pathTop + OkfGetFont(pSelector->pathFont)->pOkf->maxHeight)) / 2 - 1;
	pSelector->totalsSepLineTop = pSelector->totalsTop - (pSelector->totalsTop - (pSelector->listTop + pSelector->listSize * pSelector->selectionHeight)) / 2 - 1;
	pSelector->selectionLeft = pSelector->fileLeft - pSelector->selectionMargin;
	pSelector->selectionWidh = (pSelector->sizeRight - pSelector->fileLeft + 1) + pSelector->selectionMargin * 2;
	pSelector->scrollBarHeight = pSelector->listSize * pSelector->selectionHeight;

	gm_strcpy(currentPath, pSelector->currentPath);
    
	fs(currentPath, dest);

	// Prepare screen
	topEntry = selectedEntry = 0;
	RefreshScreen(pSelector, topEntry, selectedEntry);

	  // Main loop
	while (GpKeyGet());
	while(TRUE)
	{
	    tick = GpTickCountGet();
    
		// Prepare screen
		RefreshScreen(pSelector, topEntry, selectedEntry);

		// Get buttons state
		oldKeys = pressedKeys;
		newKeys = pressedKeys = GpKeyGet();

		// Control first and last item of the list
		if ((selectedEntry == 0) && (oldKeys & GPC_VK_UP))
			newKeys = newKeys & ~GPC_VK_UP;
		if ((selectedEntry == (ini.num_games_in_ini - 1)) && (oldKeys & GPC_VK_DOWN))
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
			if (selectedEntry > 0)
			{
				if (topEntry == selectedEntry)
					topEntry--;
				selectedEntry--;
			}
			else
			{
		        selectedEntry = ini.num_games_in_ini - 1;
				if (ini.num_games_in_ini > pSelector->listSize)
					topEntry = ini.num_games_in_ini - pSelector->listSize;
			}
		}

		//- Move to next file/folder
		else if (newKeys & GPC_VK_DOWN)
		{
			if (selectedEntry < ini.num_games_in_ini - 1)
			{
				selectedEntry++;
				if ((selectedEntry - topEntry) >= pSelector->listSize)
					topEntry = (selectedEntry - pSelector->listSize) + 1;
			}
			else
			{
				selectedEntry = 0;
				topEntry = selectedEntry;
			}
		}

		//- Move to previous N file/folder
		else if (newKeys & GPC_VK_LEFT)
		{
			if (ini.num_games_in_ini > pSelector->listSize)
				if (topEntry > pSelector->listSize)
					topEntry = topEntry  - pSelector->listSize;
				else
					topEntry = 0;

			if (selectedEntry > pSelector->listSize)
				selectedEntry = selectedEntry - pSelector->listSize;
			else
				selectedEntry = topEntry;
		}

		//- Move to next N file/folder
		else if (newKeys & GPC_VK_RIGHT)
		{
			if (ini.num_games_in_ini > pSelector->listSize)
			{
				if (topEntry < ini.num_games_in_ini - pSelector->listSize * 2 - 1)
					topEntry = topEntry + pSelector->listSize;
				else
					topEntry = ini.num_games_in_ini - pSelector->listSize;
			}

			if (topEntry < 0)
				topEntry = 0;

			if (selectedEntry < ini.num_games_in_ini - pSelector->listSize && (ini.num_games_in_ini > pSelector->listSize))
				selectedEntry = selectedEntry + pSelector->listSize;
			else
		        selectedEntry = ini.num_games_in_ini - 1;
	    }

		//- Get selected file/folder
		else if (newKeys & GPC_VK_FA)
		{
			// return the file or load or something :P
			gm_strcpy(pSelector->filename, ini.game[selectedEntry].file);
			result = 1;
			break;
		}
		else if (newKeys & GPC_VK_SELECT)
		{
			char temp[255];
			gm_sprintf(temp,"gp:\\GPMM\\WB32\\WB32.ini");

			GpFileRemove(temp);
       
//			PrintMessage("Rescanning Directory",0);

			fs_scandir(currentPath);
		}

	    while ((GpTickCountGet() - tick) < pSelector->repetitionSpeed);
	}
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
static void RefreshScreen(FILESELECTOR *pSelector, int topEntry, int selectedEntry)
{
	int pos, i, h, last_c, last_o;

	char str[100];

	OkfSetToDefaults();

	// Background
	if (pSelector->pBackground)
		GpBitBlt16(NULL, &okf.pSurfaces[*okf.pCurSurface], 0, 0, GPC_LCD_WIDTH, GPC_LCD_HEIGHT,
					(unsigned char *) pSelector->pBackground, 0, 0, GPC_LCD_WIDTH, GPC_LCD_HEIGHT);
	else
		GpRectFill16(NULL, &okf.pSurfaces[*okf.pCurSurface], 0, 0, GPC_LCD_WIDTH, GPC_LCD_HEIGHT, pSelector->backColor);

	for(pos = topEntry,i = 0;(i < pSelector->listSize) && (pos < ini.num_games_in_ini);pos++,i++)
	{
		if (pos == selectedEntry)
		{
			if (pSelector->selectionIntensity >= 32)
				GpRectFill16(NULL, &okf.pSurfaces[*okf.pCurSurface],
							pSelector->selectionLeft, pSelector->listTop + i * pSelector->selectionHeight,
							pSelector->selectionWidh, pSelector->selectionHeight,
							pSelector->selectionColor);
			else
				OverlayColor16(&okf.pSurfaces[*okf.pCurSurface],
							pSelector->selectionLeft, pSelector->listTop + i * pSelector->selectionHeight,
							pSelector->selectionWidh, pSelector->selectionHeight,
							pSelector->selectionColor, pSelector->selectionIntensity);
		}

		if(ini.game[pos].entry==65535)
			gm_sprintf(str,"%s (CRC unknown)",ini.game[pos].file);
		else
			gm_sprintf(str,"%s",MyDat[ini.game[pos].entry].name); 

		h=gm_lstrlen(str);

		do
		{ 
			if(str[h]==')') 
				last_c=h; 
			if(str[h]=='(') 
				last_o=h; 

			if(str[h]==']') 
				last_c=h; 
			if(str[h]=='[') 
				last_o=h; 
		} while((--h)>4);

		// ok: this remove () but region and version
		str[last_o-1]='\0';

		str[30]='\0'; //cut name

		okf.font = pSelector->fileFont;
		OkfPrintAt(pSelector->fileLeft,
					pSelector->listTop + i * pSelector->selectionHeight + pSelector->fileYDisplacement,
					str);

	}

	// New Scroll bar
	if (pSelector->showScrollBar)
	{
		GpRectFill16(NULL, &okf.pSurfaces[*okf.pCurSurface], pSelector->scrollBarLeft, pSelector->listTop,
					2, pSelector->scrollBarHeight, pSelector->scrollBarColor);
		pos = 0;
		if (ini.num_games_in_ini > 1)
			pos = pSelector->scrollBarHeight * 1000 / (ini.num_games_in_ini - 1);
		pos = pSelector->listTop + selectedEntry * pos / 1000 - 2;
		GpRectFill16(NULL, &okf.pSurfaces[*okf.pCurSurface], pSelector->scrollBarLeft    , pos - 2, 2, 10, pSelector->backColor);
		GpRectFill16(NULL, &okf.pSurfaces[*okf.pCurSurface], pSelector->scrollBarLeft - 2, pos    , 6,  6, pSelector->scrollBarColor);
		GpRectFill16(NULL, &okf.pSurfaces[*okf.pCurSurface], pSelector->scrollBarLeft - 1, pos - 1, 4,  8, pSelector->scrollBarColor);
	}

	// Shows the result
	GpSurfaceFlip(&okf.pSurfaces[*okf.pCurSurface]);
	*okf.pCurSurface ^= 0x01;
}
