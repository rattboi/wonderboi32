//---------------------------------------------------------------------------
#include "types.h"

#include <gpstdlib.h>
#include <gpstdio.h>
#include <gpgraphic.h>
#include <gpmain.h>
#include <gpfont.h>

#include "WSInput.h"
#include "WS.h"
#include "WSDraw.h"
#include "WSHard.h"

#include "menu.h"


//---------------------------------------------------------------------------

int 	gp_keys[9] = { 0 };
uint8   ws_keys[11];

extern stMenu horz_keys_menu;
extern stMenu vert_keys_menu;
extern int vert;
extern int scroll_x;

#define WS_KEY_H_UP		0
#define WS_KEY_H_DOWN	1
#define WS_KEY_H_LEFT	2
#define WS_KEY_H_RIGHT	3

#define WS_KEY_V_UP		4
#define WS_KEY_V_DOWN	5
#define WS_KEY_V_LEFT	6
#define WS_KEY_V_RIGHT	7

#define WS_KEY_1		8
#define WS_KEY_2		9
#define WS_KEY_START	10

//---------------------------------------------------------------------------
int (*DoKeys)(uint16 *state);
int WsKeys(uint16 *state);
int StateKeys(uint16 *state);

int WsInputInit(int isVertical)
{
		DoKeys = WsKeys;

		if (isVertical == 0)
		{
			gp_keys[KEYS_LEFT]	= horz_keys_menu.options[MENU_KEYS_LEFT].selected;
			gp_keys[KEYS_RIGHT]	= horz_keys_menu.options[MENU_KEYS_RIGHT].selected;
			gp_keys[KEYS_UP]	= horz_keys_menu.options[MENU_KEYS_UP].selected;
			gp_keys[KEYS_DOWN]	= horz_keys_menu.options[MENU_KEYS_DOWN].selected;
			gp_keys[KEYS_A]		= horz_keys_menu.options[MENU_KEYS_A].selected;
			gp_keys[KEYS_B]		= horz_keys_menu.options[MENU_KEYS_B].selected;
			gp_keys[KEYS_L]		= horz_keys_menu.options[MENU_KEYS_L].selected;
			gp_keys[KEYS_R]		= horz_keys_menu.options[MENU_KEYS_R].selected;
			gp_keys[KEYS_START]	= horz_keys_menu.options[MENU_KEYS_START].selected;
		}
		else // horizontal
		{
			gp_keys[KEYS_LEFT]	= vert_keys_menu.options[MENU_KEYS_LEFT].selected;
			gp_keys[KEYS_RIGHT]	= vert_keys_menu.options[MENU_KEYS_RIGHT].selected;
			gp_keys[KEYS_UP]	= vert_keys_menu.options[MENU_KEYS_UP].selected;
			gp_keys[KEYS_DOWN]	= vert_keys_menu.options[MENU_KEYS_DOWN].selected;
			gp_keys[KEYS_A]		= vert_keys_menu.options[MENU_KEYS_A].selected;
			gp_keys[KEYS_B]		= vert_keys_menu.options[MENU_KEYS_B].selected;
			gp_keys[KEYS_L]		= vert_keys_menu.options[MENU_KEYS_L].selected;
			gp_keys[KEYS_R]		= vert_keys_menu.options[MENU_KEYS_R].selected;
			gp_keys[KEYS_START]	= vert_keys_menu.options[MENU_KEYS_START].selected;
		}
}

int StateInputInit(int isVertical)
{
		DoKeys = StateKeys;
}

//---------------------------------------------------------------------------

int StateKeys(uint16 *state)
{

}

int WsKeys(uint16 *state)
{
	int i 						= 0;
	static int selectPressed 	= 0;
	int ExKey					= 0;
	static int LastKey			= 0;
	
	for (i = 0; i < 11; i++)
		ws_keys[i] = 0;

	GpKeyGetEx(&ExKey);

	if (ExKey & GPC_VK_SELECT)
	{
		int DrawMode = GetDrawMode();
		
		if ((ExKey & GPC_VK_FL) && !(ExKey & GPC_VK_FR) && !(LastKey & GPC_VK_FL)) // select + L
		{
			if (DrawMode != 0 && DrawMode != 3)
			{
				scroll_x = ((scroll_x < 10) ? scroll_x + 2 : 10);
			}
			selectPressed = 1;
		}
		
		if ((ExKey & GPC_VK_FR) && !(ExKey & GPC_VK_FL) && !(LastKey & GPC_VK_FR)) // select + R
		{
			if (DrawMode != 0 && DrawMode != 3)
			{
				scroll_x = ((scroll_x > 0) ? scroll_x - 2 : 0);
			}
			selectPressed = 1;
		}

		if ((ExKey & GPC_VK_FR) && (ExKey & GPC_VK_FL)) // select + L + R
		{
			WsReset();
			selectPressed = 1;
		}
		
		if ((ExKey & GPC_VK_UP)  && !(LastKey & GPC_VK_UP)) // select + UP
		{
			DrawMode += 1;
			DrawMode %= 4;
			SetDrawMode(DrawMode);
			WsInputInit(DrawMode == 3);
			selectPressed = 1;
		}

		if ((ExKey & GPC_VK_DOWN)  && !(LastKey & GPC_VK_DOWN)) // select + DOWN
		{
			if (DrawMode == 0)
				DrawMode = 4;

			DrawMode -=1;		
			SetDrawMode(DrawMode);
			WsInputInit(DrawMode == 3);
			selectPressed = 1;
		}

		if ((ExKey & GPC_VK_LEFT)  && !(LastKey & GPC_VK_LEFT)) // select + LEFT
		{
			if (FrameSkip == 0)
				FrameSkip = 9;
			FrameSkip -= 1;

			selectPressed = 1;
		}
				
		if ((ExKey & GPC_VK_RIGHT)  && !(LastKey & GPC_VK_RIGHT)) // select + RIGHT
		{
			FrameSkip += 1;
			FrameSkip %= 9;

			selectPressed = 1;
		}

		if (ExKey & GPC_VK_FB) // select + B
		{
			gammaC -= 0.03f;

			if (gammaC < 1.0f)
				gammaC = 1.0f;

			RebuildPalette();

			selectPressed = 1;
		}
				
		if (ExKey & GPC_VK_FA)// select + A
		{
			gammaC += 0.03f;

			if (gammaC > 3.0f)
				gammaC = 3.0f;

			RebuildPalette();

			selectPressed = 1;
		}

		if (!(ExKey & GPC_VK_START) && (LastKey & GPC_VK_START)) // select + start (stopped pressing)
		{
			GpAppExit();									// reboot GP32
		}
	}
	else
	{
		if (ExKey & GPC_VK_LEFT)
			ws_keys[gp_keys[KEYS_LEFT]] = 1;
		if (ExKey & GPC_VK_RIGHT)
			ws_keys[gp_keys[KEYS_RIGHT]] = 1;
		if (ExKey & GPC_VK_UP)
			ws_keys[gp_keys[KEYS_UP]] = 1;
		if (ExKey & GPC_VK_DOWN)
			ws_keys[gp_keys[KEYS_DOWN]] = 1;
		if (ExKey & GPC_VK_FA)
			ws_keys[gp_keys[KEYS_A]] = 1;
		if (ExKey & GPC_VK_FB)
			ws_keys[gp_keys[KEYS_B]] = 1;
		if (ExKey & GPC_VK_FL)
			ws_keys[gp_keys[KEYS_L]] = 1;
		if (ExKey & GPC_VK_FR)
			ws_keys[gp_keys[KEYS_R]] = 1;
		if (ExKey & GPC_VK_START)
			ws_keys[gp_keys[KEYS_START]] = 1;

		if (LastKey & GPC_VK_SELECT)
		{
			if (selectPressed == 0)
			{
				selectPressed = 0;
				LastKey = 0;
				return 1;
			}
			else
			{
				selectPressed = 0;
			}
		}	
	}

	LastKey = ExKey;
		
	*state = (ws_keys[WS_KEY_START]	<<9)|
			(ws_keys[WS_KEY_1]		<<10)|
			(ws_keys[WS_KEY_2]		<<11)|
			(ws_keys[WS_KEY_H_UP]	<<4)|
			(ws_keys[WS_KEY_H_RIGHT]<<5)|
			(ws_keys[WS_KEY_H_DOWN]	<<6)|
			(ws_keys[WS_KEY_H_LEFT]	<<7)|
			(ws_keys[WS_KEY_V_UP]	<<1)|
			(ws_keys[WS_KEY_V_RIGHT]<<2)|
			(ws_keys[WS_KEY_V_DOWN]	<<3)|
			(ws_keys[WS_KEY_V_LEFT]	<<0);

    return 0;
}
