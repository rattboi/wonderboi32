#include "types.h"

#include "gpstdlib.h"
#include "gpgraphic.h"
#include "gpgraphic16.h"
#include "gpfont.h"
#include "gpfont16.h"

#include "OKF/global.h"
#include "OKF/clock.h"
#include "OKF/graphics16.h"
#include "OKF/okf.h"

#include "file_dialog.h"

#include "gpmain.h"
#include "menu.h"

extern GPDRAWSURFACE gtSurface[2];
extern int giSurface;

extern unsigned short WBTitle[];

void draw_menu (stMenu *menu,int giSurface) {

	int i,j;

	Cls(giSurface);
	DrawWindow(20, 10, 280, 180, giSurface, COLOR_RED, COLOR_BLUE);
	
	okf.justify = okfJustifyLeft;
	
	for (i=0;i<menu->num;i++){

		if (i==menu->selected) {
			GpRectFill16(NULL, &gtSurface[giSurface], menu->options[i].x-1,
											 menu->options[i].y,
											 menu->options[i].ex-menu->options[i].x+2,
											 menu->options[i].ey-menu->options[i].y+1,
											 RGB(0,31,0));
		}

		OkfPrintAt(menu->options[i].x, menu->options[i].y, menu->options[i].name);

		for (j=menu->options[i].num-1; j>=0 ; j--) {	/* Suboptions from right to left */

			if (j==menu->options[i].selected) {
				GpRectFill16(NULL, &gtSurface[giSurface], menu->options[i].sub_options[j].x-1,
											 menu->options[i].sub_options[j].y,
											 menu->options[i].sub_options[j].ex-menu->options[i].sub_options[j].x+2,
											 menu->options[i].sub_options[j].ey-menu->options[i].sub_options[j].y+1,
											 RGB(0,31,0));
			}


			OkfPrintAt(menu->options[i].sub_options[j].x,
					   menu->options[i].sub_options[j].y,
					   menu->options[i].sub_options[j].name);
			
		}
	}
}

int run_menu(stMenu *menu) {

	int exit_menu=0;
	int menu_cancel=0;
	int keys=0;
	int i;

	do {
		 draw_menu(menu,giSurface);
		 SurfaceFlip();
		 keys=0;
		 while (keys == 0) for (i=0;i<1000;i++) keys|=GpKeyGet();


		 if(keys & GPC_VK_DOWN)  menu->selected=(menu->selected+1)%menu->num;
		 if(keys & GPC_VK_UP)  menu->selected=(menu->selected-1);

		 if (menu->selected<0) menu->selected=menu->num-1;

		 if(keys & (GPC_VK_FA)) {

		 		if (menu->options[menu->selected].num) {
					
    	    	}
    	    	else exit_menu=1;
		 }
		 
		 if(keys & (GPC_VK_FB))
		 {
			 exit_menu=1;
			 menu_cancel=1;
		 }

		 if (menu->options[menu->selected].num) {

    			if(keys & GPC_VK_LEFT)   menu->options[menu->selected].selected=(menu->options[menu->selected].selected-1);
		  else if(keys & GPC_VK_RIGHT)   menu->options[menu->selected].selected=(menu->options[menu->selected].selected+1) %
		 		  										 							menu->options[menu->selected].num;

			 if (menu->options[menu->selected].selected<0) menu->options[menu->selected].selected=
		 		  										 							menu->options[menu->selected].num-1;
		 }

		 while (GpKeyGet());

	} while (exit_menu != 1);

	if (menu_cancel == 1)
		return MENU_CANCEL;
	else
		return menu->selected;
}

void precalc_menu (stMenu *menu,int y_spacing,int x_spacing) {
	int i,xp,yp,lx,ly;
	int j,sxp,syp;

	xp=50,yp=20;
	for (i=0;i<menu->num;i++){

		lx=OkfGetTextWidth(menu->options[i].name);
		ly=OkfGetTextHeight(menu->options[i].name);

		if (menu->options[i].num) {

			menu->options[i].x=xp;
			menu->options[i].y=yp;

			menu->options[i].ex=xp+lx;
			menu->options[i].ey=yp+ly;

			sxp=300-20-x_spacing;
			syp=yp;

			for (j=menu->options[i].num-1; j>=0 ; j--) {	/* Suboptions from right to left */

				menu->options[i].sub_options[j].y=syp;
				menu->options[i].sub_options[j].ex=sxp;

				lx=OkfGetTextWidth(menu->options[i].sub_options[j].name);
				ly=OkfGetTextHeight(menu->options[i].sub_options[j].name);

				sxp-=lx;

				menu->options[i].sub_options[j].x=sxp;
				menu->options[i].sub_options[j].ey=syp+ly;

				sxp-=x_spacing;

			}

		}
		else {
			menu->options[i].x=(320-lx)/2;
			menu->options[i].y=yp;
			menu->options[i].ex=menu->options[i].x+lx;
			menu->options[i].ey=menu->options[i].y+ly;
		}

		yp+=ly+y_spacing;

	}

}

void fill_main_menu (stMenu *menu) {

		GPMEMSET(menu,0,sizeof(stMenu));

		GPSTRCPY (menu->options[MENU_MAIN_CPUSPEED].name,"CPU Mhz");
			GPSTRCPY (menu->options[MENU_MAIN_CPUSPEED].sub_options[0].name,"132");
			GPSTRCPY (menu->options[MENU_MAIN_CPUSPEED].sub_options[1].name,"156");
			GPSTRCPY (menu->options[MENU_MAIN_CPUSPEED].sub_options[2].name,"166");
			GPSTRCPY (menu->options[MENU_MAIN_CPUSPEED].sub_options[3].name,"180");
			GPSTRCPY (menu->options[MENU_MAIN_CPUSPEED].sub_options[4].name,"200");
			GPSTRCPY (menu->options[MENU_MAIN_CPUSPEED].sub_options[5].name,"220");
			menu->options[MENU_MAIN_CPUSPEED].num=6;
			menu->options[MENU_MAIN_CPUSPEED].selected = 0; // Default to 132
			
		GPSTRCPY (menu->options[MENU_MAIN_LOAD_ROM].name,"Load Rom");
		GPSTRCPY (menu->options[MENU_MAIN_PLAY].name,"Continue Playing");
		GPSTRCPY (menu->options[MENU_MAIN_WS_RESET].name,"Reset Game (Saves SRAM)");
		GPSTRCPY (menu->options[MENU_MAIN_LOAD_STATE].name, "Load State");
		GPSTRCPY (menu->options[MENU_MAIN_SAVE_STATE].name, "Save State");
		GPSTRCPY (menu->options[MENU_MAIN_CONFIG].name, "Configuration");
		GPSTRCPY (menu->options[MENU_MAIN_SAVE_DEF_CFG].name, "Save Default Config");
		GPSTRCPY (menu->options[MENU_MAIN_SAVE_GME_CFG].name, "Save Game Config");
		GPSTRCPY (menu->options[MENU_MAIN_REBOOT].name,"Reboot GP32");

		menu->num = 10;

		precalc_menu(menu,2,2);
}

void fill_config_menu (stMenu *menu) {

		GPMEMSET(menu,0,sizeof(stMenu));

		GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_FRAMESKIP].name,"Fskip");
			GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_FRAMESKIP].sub_options[0].name,"0");
			GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_FRAMESKIP].sub_options[1].name,"1");
			GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_FRAMESKIP].sub_options[2].name,"2");
			GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_FRAMESKIP].sub_options[3].name,"3");
			GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_FRAMESKIP].sub_options[4].name,"4");
			GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_FRAMESKIP].sub_options[5].name,"5");
			GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_FRAMESKIP].sub_options[6].name,"6");
			GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_FRAMESKIP].sub_options[7].name,"7");
			GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_FRAMESKIP].sub_options[8].name,"8");
			menu->options[MENU_CONFIG_VIDEO_FRAMESKIP].num=9;
			menu->options[MENU_CONFIG_VIDEO_FRAMESKIP].selected = 4; // Default to FS4

		GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_STRETCH].name,"Video");
			GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_STRETCH].sub_options[0].name,"S");
			GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_STRETCH].sub_options[1].name,"F");
			GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_STRETCH].sub_options[2].name,"W");
			GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_STRETCH].sub_options[3].name,"V");
			menu->options[MENU_CONFIG_VIDEO_STRETCH].num=4;
			menu->options[MENU_CONFIG_VIDEO_STRETCH].selected = 2; // Default to H+V
			
		GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_PALETTE].name,"Palette");
			GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_PALETTE].sub_options[0].name,"Default");
			GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_PALETTE].sub_options[1].name,"Amber");
			GPSTRCPY (menu->options[MENU_CONFIG_VIDEO_PALETTE].sub_options[2].name,"Green");
			menu->options[MENU_CONFIG_VIDEO_PALETTE].num=3;
			
		GPSTRCPY (menu->options[MENU_CONFIG_KEYS_HORZ].name,"Configure Horz Keys");
		GPSTRCPY (menu->options[MENU_CONFIG_KEYS_VERT].name,"Configure Vert Keys");
		GPSTRCPY (menu->options[MENU_CONFIG_KEYS_DEF_HORZ].name,"Load Default Horz Keys");
		GPSTRCPY (menu->options[MENU_CONFIG_KEYS_DEF_VERT].name,"Load Default Vert Keys");
		GPSTRCPY (menu->options[MENU_CONFIG_DEBUG].name,"Debug Settings");
		GPSTRCPY (menu->options[MENU_CONFIG_RETURN].name,"Return To Main Menu");

		menu->num = 9;

		precalc_menu(menu,2,10);
}

void fill_debug_menu (stMenu *menu) {

		GPMEMSET(menu,0,sizeof(stMenu));

		GPSTRCPY (menu->options[MENU_DEBUG_BGLAYER].name,"BG Layer");
			GPSTRCPY (menu->options[MENU_DEBUG_BGLAYER].sub_options[0].name,"Off");
			GPSTRCPY (menu->options[MENU_DEBUG_BGLAYER].sub_options[1].name,"On");
			menu->options[MENU_DEBUG_BGLAYER].num=2;
			menu->options[MENU_DEBUG_BGLAYER].selected = 1;

		GPSTRCPY (menu->options[MENU_DEBUG_FGLAYER].name,"FG Layer");
			GPSTRCPY (menu->options[MENU_DEBUG_FGLAYER].sub_options[0].name,"Off");
			GPSTRCPY (menu->options[MENU_DEBUG_FGLAYER].sub_options[1].name,"On");
			menu->options[MENU_DEBUG_FGLAYER].num=2;
			menu->options[MENU_DEBUG_FGLAYER].selected = 1;

		GPSTRCPY (menu->options[MENU_DEBUG_SPRITELAYER].name,"Sprites");
			GPSTRCPY (menu->options[MENU_DEBUG_SPRITELAYER].sub_options[0].name,"Off");
			GPSTRCPY (menu->options[MENU_DEBUG_SPRITELAYER].sub_options[1].name,"On");
			menu->options[MENU_DEBUG_SPRITELAYER].num=2;
			menu->options[MENU_DEBUG_SPRITELAYER].selected = 1;

		GPSTRCPY (menu->options[MENU_DEBUG_RETURN].name,"Return To Main Menu");

		menu->num = 8;

		precalc_menu(menu,2,10);
}

void fill_horz_keys_menu (stMenu *menu) {

		GPMEMSET(menu,0,sizeof(stMenu));

		GPSTRCPY (menu->options[MENU_KEYS_UP].name,"UP");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[0].name,"U");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[1].name,"D");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[2].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[3].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[4].name,"U2");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[5].name,"D2");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[6].name,"L2");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[7].name,"R2");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[8].name,"1");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[9].name,"2");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[10].name,"St");

			menu->options[MENU_KEYS_UP].num=11;
			menu->options[MENU_KEYS_UP].selected = 0; 

		GPSTRCPY (menu->options[MENU_KEYS_LEFT].name,"LEFT");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[0].name,"U");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[1].name,"D");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[2].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[3].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[4].name,"U2");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[5].name,"D2");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[6].name,"L2");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[7].name,"R2");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[8].name,"1");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[9].name,"2");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[10].name,"St");

			menu->options[MENU_KEYS_LEFT].num=11;
			menu->options[MENU_KEYS_LEFT].selected = 2;

		GPSTRCPY (menu->options[MENU_KEYS_RIGHT].name,"RIGHT");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[0].name,"U");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[1].name,"D");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[2].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[3].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[4].name,"U2");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[5].name,"D2");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[6].name,"L2");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[7].name,"R2");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[8].name,"1");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[9].name,"2");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[10].name,"St");

			menu->options[MENU_KEYS_RIGHT].num=11;
			menu->options[MENU_KEYS_RIGHT].selected = 3;

		GPSTRCPY (menu->options[MENU_KEYS_DOWN].name,"DOWN");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[0].name,"U");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[1].name,"D");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[2].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[3].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[4].name,"U2");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[5].name,"D2");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[6].name,"L2");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[7].name,"R2");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[8].name,"1");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[9].name,"2");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[10].name,"St");

			menu->options[MENU_KEYS_DOWN].num=11;
			menu->options[MENU_KEYS_DOWN].selected = 1;

		GPSTRCPY (menu->options[MENU_KEYS_A].name,"A");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[0].name,"U");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[1].name,"D");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[2].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[3].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[4].name,"U2");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[5].name,"D2");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[6].name,"L2");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[7].name,"R2");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[8].name,"1");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[9].name,"2");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[10].name,"St");

			menu->options[MENU_KEYS_A].num=11;
			menu->options[MENU_KEYS_A].selected = 8;

		GPSTRCPY (menu->options[MENU_KEYS_B].name,"B");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[0].name,"U");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[1].name,"D");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[2].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[3].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[4].name,"U2");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[5].name,"D2");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[6].name,"L2");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[7].name,"R2");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[8].name,"1");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[9].name,"2");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[10].name,"St");

			menu->options[MENU_KEYS_B].num=11;
			menu->options[MENU_KEYS_B].selected = 9;

		GPSTRCPY (menu->options[MENU_KEYS_START].name,"START");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[0].name,"U");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[1].name,"D");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[2].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[3].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[4].name,"U2");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[5].name,"D2");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[6].name,"L2");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[7].name,"R2");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[8].name,"1");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[9].name,"2");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[10].name,"St");

			menu->options[MENU_KEYS_START].num=11;
			menu->options[MENU_KEYS_START].selected = 10;

		GPSTRCPY (menu->options[MENU_KEYS_L].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[0].name,"U");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[1].name,"D");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[2].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[3].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[4].name,"U2");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[5].name,"D2");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[6].name,"L2");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[7].name,"R2");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[8].name,"1");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[9].name,"2");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[10].name,"St");

			menu->options[MENU_KEYS_L].num=11;
			menu->options[MENU_KEYS_L].selected = 6;

		GPSTRCPY (menu->options[MENU_KEYS_R].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[0].name,"U");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[1].name,"D");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[2].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[3].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[4].name,"U2");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[5].name,"D2");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[6].name,"L2");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[7].name,"R2");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[8].name,"1");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[9].name,"2");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[10].name,"St");

			menu->options[MENU_KEYS_R].num=11;
			menu->options[MENU_KEYS_R].selected = 7;

		GPSTRCPY (menu->options[MENU_KEYS_RETURN].name, "Return To Main Menu");
		
		menu->num = 10;

		precalc_menu(menu,2,1);
}

void fill_vert_keys_menu (stMenu *menu) {

		GPMEMSET(menu,0,sizeof(stMenu));

		GPSTRCPY (menu->options[MENU_KEYS_UP].name,"UP");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[0].name,"U");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[1].name,"D");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[2].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[3].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[4].name,"U2");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[5].name,"D2");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[6].name,"L2");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[7].name,"R2");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[8].name,"1");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[9].name,"2");
			GPSTRCPY (menu->options[MENU_KEYS_UP].sub_options[10].name,"St");

			menu->options[MENU_KEYS_UP].num=11;
			menu->options[MENU_KEYS_UP].selected = 4; 

		GPSTRCPY (menu->options[MENU_KEYS_LEFT].name,"LEFT");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[0].name,"U");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[1].name,"D");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[2].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[3].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[4].name,"U2");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[5].name,"D2");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[6].name,"L2");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[7].name,"R2");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[8].name,"1");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[9].name,"2");
			GPSTRCPY (menu->options[MENU_KEYS_LEFT].sub_options[10].name,"St");

			menu->options[MENU_KEYS_LEFT].num=11;
			menu->options[MENU_KEYS_LEFT].selected = 6;

		GPSTRCPY (menu->options[MENU_KEYS_RIGHT].name,"RIGHT");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[0].name,"U");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[1].name,"D");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[2].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[3].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[4].name,"U2");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[5].name,"D2");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[6].name,"L2");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[7].name,"R2");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[8].name,"1");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[9].name,"2");
			GPSTRCPY (menu->options[MENU_KEYS_RIGHT].sub_options[10].name,"St");

			menu->options[MENU_KEYS_RIGHT].num=11;
			menu->options[MENU_KEYS_RIGHT].selected = 7;

		GPSTRCPY (menu->options[MENU_KEYS_DOWN].name,"DOWN");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[0].name,"U");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[1].name,"D");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[2].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[3].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[4].name,"U2");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[5].name,"D2");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[6].name,"L2");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[7].name,"R2");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[8].name,"1");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[9].name,"2");
			GPSTRCPY (menu->options[MENU_KEYS_DOWN].sub_options[10].name,"St");

			menu->options[MENU_KEYS_DOWN].num=11;
			menu->options[MENU_KEYS_DOWN].selected = 5;

		GPSTRCPY (menu->options[MENU_KEYS_A].name,"A");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[0].name,"U");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[1].name,"D");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[2].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[3].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[4].name,"U2");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[5].name,"D2");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[6].name,"L2");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[7].name,"R2");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[8].name,"1");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[9].name,"2");
			GPSTRCPY (menu->options[MENU_KEYS_A].sub_options[10].name,"St");

			menu->options[MENU_KEYS_A].num=11;
			menu->options[MENU_KEYS_A].selected = 3;

		GPSTRCPY (menu->options[MENU_KEYS_B].name,"B");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[0].name,"U");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[1].name,"D");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[2].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[3].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[4].name,"U2");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[5].name,"D2");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[6].name,"L2");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[7].name,"R2");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[8].name,"1");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[9].name,"2");
			GPSTRCPY (menu->options[MENU_KEYS_B].sub_options[10].name,"St");

			menu->options[MENU_KEYS_B].num=11;
			menu->options[MENU_KEYS_B].selected = 1;

		GPSTRCPY (menu->options[MENU_KEYS_START].name,"START");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[0].name,"U");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[1].name,"D");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[2].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[3].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[4].name,"U2");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[5].name,"D2");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[6].name,"L2");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[7].name,"R2");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[8].name,"1");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[9].name,"2");
			GPSTRCPY (menu->options[MENU_KEYS_START].sub_options[10].name,"St");

			menu->options[MENU_KEYS_START].num=11;
			menu->options[MENU_KEYS_START].selected = 10;

		GPSTRCPY (menu->options[MENU_KEYS_L].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[0].name,"U");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[1].name,"D");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[2].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[3].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[4].name,"U2");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[5].name,"D2");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[6].name,"L2");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[7].name,"R2");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[8].name,"1");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[9].name,"2");
			GPSTRCPY (menu->options[MENU_KEYS_L].sub_options[10].name,"St");

			menu->options[MENU_KEYS_L].num=11;
			menu->options[MENU_KEYS_L].selected = 2;

		GPSTRCPY (menu->options[MENU_KEYS_R].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[0].name,"U");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[1].name,"D");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[2].name,"L");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[3].name,"R");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[4].name,"U2");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[5].name,"D2");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[6].name,"L2");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[7].name,"R2");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[8].name,"1");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[9].name,"2");
			GPSTRCPY (menu->options[MENU_KEYS_R].sub_options[10].name,"St");

			menu->options[MENU_KEYS_R].num=11;
			menu->options[MENU_KEYS_R].selected = 0;

		GPSTRCPY (menu->options[MENU_KEYS_RETURN].name, "Return To Main Menu");
		
		menu->num = 10;

		precalc_menu(menu,2,1);
}

