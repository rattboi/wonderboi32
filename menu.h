#ifndef __menu_h__
#define __menu_h__

#include "types.h"

#define MAX_MENU_OPTIONS 16

typedef struct
{
	int x,y;
	int ex,ey;
	char name[8];
} menuSubOption;

typedef struct
{
    int x,y;							/* Start X,Y */
    int ex,ey;							/* End X,Y need to be calculated... */
    int selected;						/* Selected sub_option */
    int num;
   	char name[16];
   	menuSubOption sub_options[16];

} menuOption ;

typedef struct
{
	menuOption options[MAX_MENU_OPTIONS];
	int num;
	int selected;
} stMenu;

void draw_menu (stMenu *menu,int nflip);
int run_menu(stMenu *menu);
void precalc_menu (stMenu *menu,int y_spacing,int x_spacing);
void fill_main_menu (stMenu *menu);
void fill_config_menu(stMenu *menu);
void fill_horz_keys_menu (stMenu *menu);
void fill_vert_keys_menu (stMenu *menu);

#define MENU_CANCEL				-1

#define MENU_MAIN_CPUSPEED      0
#define MENU_MAIN_LOAD_ROM		1
#define MENU_MAIN_PLAY			2
#define MENU_MAIN_WS_RESET		3
#define MENU_MAIN_LOAD_STATE	4
#define MENU_MAIN_SAVE_STATE	5
#define MENU_MAIN_CONFIG		6
#define MENU_MAIN_SAVE_DEF_CFG	7
#define MENU_MAIN_SAVE_GME_CFG	8
#define MENU_MAIN_REBOOT		9

#define MENU_CONFIG_VIDEO_FRAMESKIP	0
#define MENU_CONFIG_VIDEO_STRETCH	1
#define MENU_CONFIG_VIDEO_PALETTE	2
#define MENU_CONFIG_KEYS_HORZ		3
#define MENU_CONFIG_KEYS_VERT		4
#define MENU_CONFIG_KEYS_DEF_HORZ	5
#define MENU_CONFIG_KEYS_DEF_VERT	6
#define MENU_CONFIG_RETURN			7

#define MENU_KEYS_UP		0
#define MENU_KEYS_LEFT		1
#define MENU_KEYS_RIGHT		2
#define MENU_KEYS_DOWN		3
#define MENU_KEYS_A			4
#define MENU_KEYS_B			5
#define MENU_KEYS_START		6
#define MENU_KEYS_L			7
#define MENU_KEYS_R			8
#define MENU_KEYS_RETURN	9

#define KEYS_UP			0
#define KEYS_LEFT		1
#define KEYS_RIGHT		2
#define KEYS_DOWN		3
#define KEYS_A			4
#define KEYS_B			5
#define KEYS_START		6
#define KEYS_L			7
#define KEYS_R			8

#define STRETCH_NONE	0
#define STRETCH_HORZ	1
#define STRETCH_BOTH	2

#endif /*__menu_h__*/
