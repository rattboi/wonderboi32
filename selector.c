#include <gpstdlib.h>
#include <gpstdio.h>
#include <gpgraphic.h>
#include <gpgraphic16.h>

#include "OKF/global.h"
#include "OKF/graphics16.h"
#include "OKF/fileSelector.h"

extern unsigned short WBTitle[];

int DoFileSelector(char *filename)
{
	FILESELECTOR fileSelector;
	int result;
	
	FileSelectorInit(&fileSelector);

	gm_strcpy(fileSelector.currentPath, "gp:\\gpmm\\wb32\\rom\\");

	fileSelector.pTotalsFormat = "%s bytes, %s files";
	fileSelector.pFileSizeFormat = " %s";

	fileSelector.pathLeft = 12;
	fileSelector.pathTop  = 6;
	fileSelector.listTop  = 40;
	fileSelector.listSize = 10;
	fileSelector.fileLeft = 20;
	fileSelector.fileYDisplacement = 0;
	fileSelector.sizeRight = 290;
	fileSelector.sizeYDisplacement = 0;
	fileSelector.totalsLeft = 12;
	fileSelector.totalsTop = 220;
	fileSelector.scrollBarLeft = 5;
	fileSelector.selectionHeight = 15;
	fileSelector.selectionMargin = 3;
	
	fileSelector.showExtensions = TRUE;
	fileSelector.showSeparationLines = FALSE;	
	
//	fileSelector.pathFont = 5;
//	fileSelector.folderFont = 4;
//	fileSelector.fileFont = 3;
//	fileSelector.sizesFont = 6;
//	fileSelector.totalsFont = 5;
	
	fileSelector.pBackground = (unsigned short *) WBTitle;
//	fileSelector.backColor = RGB(0, 0, 0);
	fileSelector.selectionColor = RGB(00, 31, 00);
//	fileSelector.selectionIntensity = 16;
	fileSelector.scrollBarColor = RGB(00, 31, 00);
	
	result = FileSelector(&fileSelector);
	
	if (result == 0)
		return 0;
	
	gm_strcpy(filename,fileSelector.filename);
	return 1;
}
