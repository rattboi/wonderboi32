/*
	file_dialog.c code by Don_Miguel <gp32@rambler.ru>

*/

#include "types.h"

#include "gpstdlib.h"
#include "gpgraphic.h"
#include "gpfont.h"

#include "OKF/global.h"
#include "OKF/clock.h"
#include "OKF/graphics16.h"
#include "OKF/okf.h"

#include "file_dialog.h"
#include "gpmain.h"

extern char *short_program_version;
extern GPDRAWSURFACE gtSurface[2];
extern int giSurface;
extern unsigned short WBTitle[];

uint16 *halfval;

char g_string[MAX_PATH_NAME_LEN];

void WindowInit()
{
	halfval = (uint16*)GPMALLOC(65536*sizeof(uint16));
	
	int r,v,b;
	
	int i;
	
	for (i=0;i<65536;i++)
	{
		r=((i>>11))>>1;
		v=(((i>>6)&31))>>1;
		b=(((i>>1)&31))>>1;
		halfval[i]=(r<<11)|(v<<6)|(b<<1)|1;
	}
}

void Cls(int giSurface)
{
	int i;
	
	for (i = 0; i < (320*240); i++)
		*((uint16*)gtSurface[giSurface].ptbuffer+i) = WBTitle[i];
	
/*	
	GpRectFill(NULL, &gtSurface[giSurface], 0,0,GPC_LCD_WIDTH,GPC_LCD_HEIGHT, color);

	GpTextOut(NULL, &gtSurface[giSurface], 2, 225, (char*)program_version, 0xf);
	GPSPRINTF(g_string, "%s (%s %s)", (char *)short_program_version, __DATE__, __TIME__);
	GpTextOut(NULL, &gtSurface[giSurface], 2, 225, g_string, 0xf);

	GPSPRINTF(g_string, "Memory Available:%d", gp_mem_func.availablemem());
	GpTextOut(NULL, &gtSurface[giSurface], 2, 213, g_string, 0); 
*/
}


void DrawMessage(char *s, int giSurface)
{
	OKFENGINE oldState;
	int tw,th;

	oldState = okf;

	OkfSetToDefaults();
	okf.font = 1;
	okf.justify = okfCenter;

	tw = OkfGetTextWidth(s);
	th = OkfGetTextHeight(s);
	
	DrawWindow( (GPC_LCD_WIDTH - (tw+12) )/2, (GPC_LCD_HEIGHT - (th+12) )/2, tw+12, th+12, 
				giSurface, COLOR_RED, COLOR_BLUE);

	okf.x = GPC_LCD_WIDTH / 2;
	okf.y = (GPC_LCD_HEIGHT - th) / 2;
	OkfPrint(s);
	
	// Restore state
	okf = oldState;	
}

void DrawError(char *s, int giSurface)
{
	OKFENGINE oldState;
	int tw,th;
	
	oldState = okf;
	
	OkfSetToDefaults();
	okf.font = 1;
	okf.justify = okfCenter;
	
	tw = OkfGetTextWidth(s);
	th = OkfGetTextHeight(s);
	
	DrawWindow( (GPC_LCD_WIDTH - (tw+12) )/2, (GPC_LCD_HEIGHT - (th+12) )/2, tw+6, th+6, 
				giSurface, COLOR_RED, COLOR_BLUE);
	
	okf.x = GPC_LCD_WIDTH / 2;
	okf.y = (GPC_LCD_HEIGHT - th) / 2;
	OkfPrint(s);
	
	// Restore state
	okf = oldState;	
}

void PrintMessage(char* string, int wait)
{
	Cls(giSurface);
	DrawMessage(string,giSurface);
	SurfaceFlip();
	if (wait)
	{
		while (!GpKeyGet());  //wait for keypress
		while (GpKeyGet());  // wait for keyrelease
	}
}

void PrintError(char* string, int wait)
{
	Cls(giSurface);
	DrawError(string,giSurface);
	SurfaceFlip();
	if (wait)
	{
		while (!GpKeyGet());  //wait for keypress
		while (GpKeyGet());  // wait for keyrelease
	}
}

void DrawWindow(int x, int y, int lx, int ly, int giSurface, uint16 color,uint16 col2)
{
	int xx,yy;
	int a,b,c;
	unsigned short* vbuffer;
	
	a = x*240;
	b = 239-y;
	
	vbuffer = (unsigned short*)gtSurface[giSurface].ptbuffer;
	
	for (yy=0;yy<ly;yy++)
	{
		c = a + b + (239-yy);
		
		for (xx=0;xx<lx;xx++)
		{
			vbuffer[xx*240 + c] =halfval[((unsigned short *)vbuffer)[xx*240 + c]];    
		}
		((unsigned short *)vbuffer)[c] = color;
		((unsigned short *)vbuffer)[lx*240 + c] = color;
		((unsigned short *)vbuffer)[c+240-1] = col2;
		((unsigned short *)vbuffer)[lx*240 + c+240-1] = col2;
	} 
	
	for (xx=0;xx<lx;xx++)
	{
		((unsigned short *)vbuffer)[xx*240 + a + b + (239)] =color;
		((unsigned short *)vbuffer)[xx*240 + a + b + (239-ly)] =color;
		((unsigned short *)vbuffer)[xx*240 + a + b + (239)+240-1] =col2;
		((unsigned short *)vbuffer)[xx*240 + a + b + (239-ly)+240-1] =col2;
	}
}
