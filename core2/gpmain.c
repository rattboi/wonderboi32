#include <gpstdlib.h>
#include <gpstdio.h>
#include <gpgraphic.h>
#include <gpmain.h>
#include <gpfont.h>

#include "../OKF/global.h"
#include "../OKF/clock.h"
#include "../OKF/graphics16.h"
#include "../OKF/okf.h"

#include "../OKF/fonts/verdana8b_pm.h"
#include "../OKF/fonts/verdana8b_pm.c"

#include "types.h"

char *long_program_version="WonderBoi32 v0.7 (new core)";
char *short_program_version="WB32 v.7";

extern unsigned short WBTitle[];

void ShowCredits()
{
	char string_version[512];

	GPSTRCPY(string_version,long_program_version);
	GPSTRCAT(string_version,"\nPorted to GP32 by Rattboi\n");
	GPSTRCAT(string_version,"\nBased on WSCamp");
	GPSTRCAT(string_version,"\nUses OKF Font Engine by Oankali");
	PrintMessage(string_version,1);
}

void CPU_alignment_and_cache() {

	asm("	    MRC p15, 0, R5, c1, c0, 0 	\n\
	    		BIC r5,r5,#2			  	\n\
	    		LDR r6,=0x1004				\n\
	    		ORR r5,r5,r6              	\n\
	    		MCR p15, 0, R5, c1, c0, 0 	\n\
	");
}

void GpMain(void *arg)
{
	char debugstring[512];

	CPU_alignment_and_cache();		// turn off alignment and turn instruction and data cache on

	setCpuSpeed(132);

	InitGraphics(16);							// Init Graphics and Fonts

	giSurface = 1;
	OkfInitialize(gtSurface, &giSurface);

	OkfRegister(verdana8b_pm_okf);

	okf.font = 1;								// Use font #1
	
	WindowInit();								//initializes windowing system

	GpFatInit();								//initializes file system

	ShowCredits();								// show the opening dialog

	WsCreate("gp:\\gpmm\\wb32\\rom\\mrdrill.ws");

	WsDrawCreate();

	GpSurfaceSet(&gtSurface[0]);

	while(1)
	{
//		GPSPRINTF(debugstring,"before WsRun");
//		PrintMessage(debugstring,0);

		WsRun();

//		GPSPRINTF(debugstring,"after WsRun");
//		PrintMessage(debugstring,0);
	}

	return;
}
