#include <gpstdlib.h>
#include <gpstdio.h>
#include <gpgraphic.h>
#include <gpgraphic16.h>

#include "OKF/global.h"
#include "OKF/graphics16.h"
#include "OKF/fileSelector.h"

#include "file_dialog.h"

#include "types.h"

extern GPDRAWSURFACE gtSurface[2];
extern int giSurface;

uint16 FileBG[320*240];						// holds the file manager background;

int DoFileSelector(char *filename, unsigned char *dest)
{
	FILESELECTOR fileSelector;
	int result;
	
	FileSelectorInit(&fileSelector);

	Cls(giSurface);

//	DrawWindow(5, 9,  305, 15,  giSurface, COLOR_RED, COLOR_BLUE); // path window
	DrawWindow(5, 6, 305, 181, giSurface, COLOR_RED, COLOR_BLUE); // file selector window
	DrawWindow(5, 219,  305, 15,  giSurface, COLOR_RED, COLOR_BLUE); // totals window

	SaveBackground(giSurface, (uint16 *)FileBG);

	gm_strcpy(fileSelector.currentPath, "gp:\\gpmm\\wb32\\rom");

	fileSelector.pTotalsFormat = "Total Files: %s, File Size: %s";
	fileSelector.pFileSizeFormat = " %s";

	fileSelector.pathLeft = 12;
	fileSelector.pathTop  = 10;
	fileSelector.listTop  = 14; //37; 
	fileSelector.listSize = 11;
	fileSelector.fileLeft = 25;
	fileSelector.fileYDisplacement = 0;
	fileSelector.sizeRight = 290;
	fileSelector.sizeYDisplacement = 0;
	fileSelector.totalsLeft = 12;
	fileSelector.totalsTop = 220;
	fileSelector.scrollBarLeft = 13;
	fileSelector.selectionHeight = 15;
	fileSelector.selectionMargin = 3;
	
	fileSelector.showExtensions = TRUE;
	fileSelector.showSeparationLines = FALSE;

	fileSelector.options = FLD_FILES;
	
//	fileSelector.pathFont = 5;
//	fileSelector.folderFont = 4;
//	fileSelector.fileFont = 3;
//	fileSelector.sizesFont = 6;
//	fileSelector.totalsFont = 5;
//	fileSelector.backColor = RGB(0, 0, 0);
//	fileSelector.selectionIntensity = 16;

	fileSelector.pBackground = (unsigned short *) FileBG;
	fileSelector.selectionColor = RGB(00, 31, 00);
	fileSelector.scrollBarColor = RGB(00, 31, 00);
	
	result = FileSelector(&fileSelector, dest);
	
	if (result == 0)
		return 0;
	
	gm_sprintf(filename,"%s\\%s", fileSelector.currentPath, fileSelector.filename);

//	PrintMessage(filename, 1);

	return 1;
}
