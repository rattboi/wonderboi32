#include <gpstdlib.h>
#include <gpstdio.h>
#include <gpgraphic.h>
#include <gpmain.h>
#include <gpfont.h>

#include "misc_str_func.h" // by YoYoFr

#include "menu.h"
#include "setcpuspeed.h"

#include "render.h"

#include "rom.h"
#include "nec/nec.h"
#include "memory.h"
#include "gpu.h"
#include "io.h"
#include "ws.h"

#include "unzip.h"

#include "file_dialog.h"

#include "OKF/global.h"
#include "OKF/clock.h"
#include "OKF/graphics16.h"
#include "OKF/okf.h"

#include "OKF/fonts/verdana10.h"
#include "OKF/fonts/verdana8b_pm.h"
#include "OKF/fonts/comicSansMS12b_pm.h"

#include "OKF/fonts/verdana10.c"
#include "OKF/fonts/verdana8b_pm.c"
#include "OKF/fonts/comicSansMS12b_pm.c"

#include "types.h"

extern unsigned short WBTitle[];
extern unsigned short WSCBmp_horz[];
extern unsigned short WSCBmp_vert[];

#define MAX_ROM_SIZE 4*1024*1024

ubyte *base_rom;
uint32 romsize;

char *long_program_version="WonderBoi32 v0.64 beta";
char *short_program_version="WB32 v.64";

GPDIRENTRY MDL[4096];
ulong DIRCOUNT;
char ws_file[256];

stMenu main_menu;
stMenu statesconfig_menu;
stMenu videoconfig_menu;
stMenu keyconfig_menu;
stMenu horz_keys_menu;
stMenu vert_keys_menu;

int16 screenbuffer[224*144];

int fSkip = 0;

int gp_keys[9] = { 0 };

typedef struct video_mode_tag
{
	void (*video_update)(unsigned char*, unsigned char*);
	int video_x;
	int video_y;
	int scroll_x;
} video_mode;
		
void (*video_update)(unsigned char*, unsigned char*);
int video_x = 0;
int video_y = 0;
int scroll_x = 0;

video_mode video_modes[4] =
{
	horz_render_normal,		48,	41,	0,
	horz_render_hstretch,	0,	41,	6,
	horz_render_hvstretch,	0,	12,	6,
	vert_render_normal,		88,	7,	0
};

int screenmode = 0;

void InitVideo(void)
{
	int i = 0;
	
	video_update = video_modes[screenmode].video_update;
	video_x = video_modes[screenmode].video_x;
	video_y = video_modes[screenmode].video_y;
	scroll_x = video_modes[screenmode].scroll_x;

	switch(screenmode)
	{
	case 0:
		{
			for (i = 0; i < (320*240); i++)
				*((uint16*)gtSurface[0].ptbuffer+i) = WSCBmp_horz[i];
		}
		break;
	case 1:
	case 2:
		{
			for (i=(320*240)-1;i>=0;--i)
				*((uint16*)gtSurface[0].ptbuffer+i) = 0;
		}
		break;
	case 3:
		{
			for (i = 0; i < (320*240); i++)
				*((uint16*)gtSurface[0].ptbuffer+i) = WSCBmp_vert[i];
		}
		break;
	default:
		break;
	}
}

int InitKeys(void)
{
	if (screenmode != 3)
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
	else
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

int DoKeys(void)
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
		if ((ExKey & GPC_VK_FL) && !(ExKey & GPC_VK_FR) && !(LastKey & GPC_VK_FL)) // select + L
		{
			scroll_x = ((scroll_x < 14) ? scroll_x + 2 : 14);
			selectPressed = 1;
		}
		
		if ((ExKey & GPC_VK_FR) && !(ExKey & GPC_VK_FL) && !(LastKey & GPC_VK_FR)) // select + R
		{
			scroll_x = ((scroll_x > 0) ? scroll_x - 2 : 0);
			selectPressed = 1;
		}

		if ((ExKey & GPC_VK_FR) && (ExKey & GPC_VK_FL)) // select + L + R
		{
			ws_reset();
			selectPressed = 1;
		}
		
		if ((ExKey & GPC_VK_UP)  && !(LastKey & GPC_VK_UP)) // select + UP
		{
			screenmode += 1;
			screenmode %= 4;
			InitVideo();
			selectPressed = 1;
		}

		if ((ExKey & GPC_VK_DOWN)  && !(LastKey & GPC_VK_DOWN)) // select + UP
		{

			if (screenmode == 0)
				screenmode = 4;

			screenmode -=1;		
			InitVideo();
			InitKeys();
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
				selectPressed = 0;
		}
	}

	LastKey = ExKey;
	return 0;
}
		
void ws_emulate(void)
{
	int i = 0;
	
	InitVideo();
	InitKeys();
		
	while(1)
	{
		if (DoKeys())
			break;
		
		for (i=0;i<fSkip-1;i++)
		{
			while (!ws_executeLine(screenbuffer,0));

			if (DoKeys())
				return;
		}
			
		while (!ws_executeLine(screenbuffer,1));

		video_update((unsigned char *)screenbuffer+(scroll_x*2), gtSurface[0].ptbuffer+(240*2*video_x)-(video_y*2));
	}
}


void ShowCredits()
{
	char string_version[512];

	GPSTRCPY(string_version,long_program_version);
	GPSTRCAT(string_version,"\nPorted to GP32 by Rattboi\n");
	GPSTRCAT(string_version,"\nBased on OSwan 0.7");
	GPSTRCAT(string_version,"\nUses OKF Font Engine by Oankali");
	PrintMessage(string_version,1);
}


int LoadRom(char *filename)
{
	F_HANDLE F;
	ulong filesize;
	ulong sizeread;

	unzFile zip_file = 0;    
	unz_file_info unzinfo;

	char printString[100];

	char ext[5];
	char *pext;
	char *p;

	ERR_CODE result;

	pext = (char*)yo_strrchr(filename, '.');

	if (pext) gm_strncpy(ext, pext, sizeof(ext));
	yo_strlwr(ext);
	if (gm_compare(ext, ".zip") == 0)  
	{
		zip_file = unzOpen(filename);

		if (zip_file == 0)
		{
			DrawError("Can't Open Zip",1);
			return 0;
		}

		unzGoToFirstFile (zip_file);
		
        for (;;)
        {
        	if (unzGetCurrentFileInfo(zip_file, &unzinfo, ws_file, sizeof(ws_file), NULL, 0, NULL, 0) != UNZ_OK) 
			{
				DrawError("Can't Open File In Zip",1);
				return 0;
			}

			yo_strlwr(ws_file); 

			p = (char *)yo_strrchr(ws_file, '.');

			if (gm_compare(p, ".wsc") == 0)
				break;
			if (gm_compare(p, ".ws") == 0)
				break;
			if (unzGoToNextFile(zip_file) != UNZ_OK)
			{
				DrawError("Can't Go To Next File In Zip",1);
				return 0;
			}
		}         

		unzOpenCurrentFile (zip_file);

		filesize = unzinfo.uncompressed_size;
		
		if (filesize > MAX_ROM_SIZE)
		{
			PrintError("File Too Big!",1);
			return 0;
		}

		if (filesize > GPAVAILABLEMEM())
		{
			PrintError("Not Enough Memory!",1);
			return 0;
		}
		
		romsize = filesize;
		base_rom = (uint8*)GPMALLOC(romsize);

		GPSPRINTF(printString,"Opening File...\nFile Size: %i", filesize);
		PrintMessage(printString,0);

		unzReadCurrentFile(zip_file, base_rom, romsize);
		
		unzCloseCurrentFile (zip_file);
		unzClose (zip_file);
	}
	else
	{	
		result=GpFileGetSize(filename, &filesize);

		if (result==SM_OK) 
		{
			if (filesize > MAX_ROM_SIZE)
			{
				PrintError("File Too Big!",1);
				return 0;
			}

			if (filesize > GPAVAILABLEMEM())
			{
				PrintError("Not Enough Memory!",1);
				return 0;
			}

			romsize = filesize;
			base_rom = (uint8*)GPMALLOC(romsize);
		}
		else
		{
			PrintError("Couldn't Get File Size!", 1);
			return 0;
		}

		result=GpFileOpen(filename,OPEN_R,&F);

		if (result==SM_OK)
		{
			GPSPRINTF(printString,"Opening File...\nFile Size: %i", filesize);
			PrintMessage(printString,0);
		}
		else
		{
			PrintError("Couldn't Open File!", 1);
			return 0;
		}

		GpFileRead(F, base_rom, MAX_ROM_SIZE, &sizeread);

		if (sizeread != filesize)
		{
			GPSPRINTF(printString, "Files Are Different Sizes\n %i & %i", sizeread, filesize);
//			PrintError(printString, 1);
			GpFileClose(F);
			return 0;
		}

		GPSTRCPY(ws_file,filename);
		GpFileClose(F);
	}
	return 1;
}


void ShowTitleScreen()
{
	int i;
	
	for (i = 0; i < (320*240); i++)
		*((uint16*)gtSurface[giSurface].ptbuffer+i) = WBTitle[i];
		
	SurfaceFlip();
	while (!GpKeyGet());  //wait for keypress
	while (GpKeyGet());  // wait for keyrelease			
}

void CPU_alignment_and_cache() {

	asm("	    MRC p15, 0, R5, c1, c0, 0 	\n\
	    		BIC r5,r5,#2			  	\n\
	    		LDR r6,=0x1004				\n\
	    		ORR r5,r5,r6              	\n\
	    		MCR p15, 0, R5, c1, c0, 0 	\n\
	");
}

void Emulate()
{
	int i;
	int forcemode;
	
	setCpuSpeed(cpuSpeeds[main_menu.options[MENU_MAIN_CPUSPEED].selected]);

	forcemode = videoconfig_menu.options[MENU_VIDEOCONFIG_FORCEMODE].selected;
	
	screenmode = videoconfig_menu.options[MENU_VIDEOCONFIG_STRETCH].selected;

	fSkip = (videoconfig_menu.options[MENU_VIDEOCONFIG_FRAMESKIP].selected << 1);

	if (forcemode == FORCEMODE_BW)
			ws_gpu_forceMonoSystem();

	if (forcemode == FORCEMODE_COLOR)
			ws_gpu_forceColorSystem();

	ws_gpu_set_colour_scheme(videoconfig_menu.options[MENU_VIDEOCONFIG_PALETTE].selected);

	GpSurfaceSet(&gtSurface[0]);
	
	ws_emulate();
	
	GpSurfaceSet(&gtSurface[giSurface]);
	
	videoconfig_menu.options[MENU_VIDEOCONFIG_STRETCH].selected = screenmode;
	
	setCpuSpeed(66);
}

int	SaveConfig(char *configPath)
{
	F_HANDLE F;
	uint32 result;
	uint32 i;
	
	result=GpFileCreate(configPath,ALWAYS_CREATE,&F);

	if (result==SM_OK)
	{
		for (i = 0; i < main_menu.num; i++)
			GpFileWrite(F, &main_menu.options[i].selected,sizeof(int));
		GpFileWrite(F, &main_menu.selected,sizeof(int));

		for (i = 0; i < statesconfig_menu.num; i++)
			GpFileWrite(F, &statesconfig_menu.options[i].selected,sizeof(int));
		GpFileWrite(F, &statesconfig_menu.selected,sizeof(int));

		for (i = 0; i < videoconfig_menu.num; i++)
			GpFileWrite(F, &videoconfig_menu.options[i].selected,sizeof(int));
		GpFileWrite(F, &videoconfig_menu.selected,sizeof(int));

		for (i = 0; i < keyconfig_menu.num; i++)
			GpFileWrite(F, &keyconfig_menu.options[i].selected,sizeof(int));
		GpFileWrite(F, &keyconfig_menu.selected,sizeof(int));
		
		for (i = 0; i < horz_keys_menu.num; i++)
			GpFileWrite(F, &horz_keys_menu.options[i].selected,sizeof(int));
		GpFileWrite(F, &horz_keys_menu.selected,sizeof(int));
		
		for (i = 0; i < vert_keys_menu.num; i++)
			GpFileWrite(F, &vert_keys_menu.options[i].selected,sizeof(int));
		GpFileWrite(F, &vert_keys_menu.selected,sizeof(int));

		GpFileClose(F);
	}
	else
	{
		PrintError("Error Saving Config\nIs WB32\\CFG directory created?",1);
	}

	return(1);
}

int	LoadConfig(char *configPath)
{
	F_HANDLE F;
	uint32 result;
	uint32 i;
	ulong sizeread;

	result = GpFileOpen(configPath,OPEN_R, &F);
	
	if (result != SM_OK)
		return(-1);

	for (i = 0; i < main_menu.num; i++)
		GpFileRead(F, &main_menu.options[i].selected,sizeof(int),&sizeread);
	GpFileRead(F, &main_menu.selected,sizeof(int),&sizeread);

	for (i = 0; i < statesconfig_menu.num; i++)
		GpFileRead(F, &statesconfig_menu.options[i].selected,sizeof(int),&sizeread);
	GpFileRead(F, &statesconfig_menu.selected,sizeof(int),&sizeread);

	for (i = 0; i < videoconfig_menu.num; i++)
		GpFileRead(F, &videoconfig_menu.options[i].selected,sizeof(int),&sizeread);
	GpFileRead(F, &videoconfig_menu.selected,sizeof(int),&sizeread);

	for (i = 0; i < keyconfig_menu.num; i++)
		GpFileRead(F, &keyconfig_menu.options[i].selected,sizeof(int),&sizeread);
	GpFileRead(F, &keyconfig_menu.selected,sizeof(int),&sizeread);
	
	for (i = 0; i < horz_keys_menu.num; i++)
		GpFileRead(F, &horz_keys_menu.options[i].selected,sizeof(int),&sizeread);
	GpFileRead(F, &horz_keys_menu.selected,sizeof(int),&sizeread);
	
	for (i = 0; i < vert_keys_menu.num; i++)
		GpFileRead(F, &vert_keys_menu.options[i].selected,sizeof(int),&sizeread);
	GpFileRead(F, &vert_keys_menu.selected,sizeof(int),&sizeread);

	GpFileClose(F);

	return(1);
}

void GpMain(void *arg)
{
	int result;
	int running = 0;							// set to 1 if a rom is loaded
	int forcemode = 0;
	int vert = 0;
	F_HANDLE F;
	
	char *pext;

	char g_string[256];
	
	char fname[256];

	CPU_alignment_and_cache();					// turn off alignment and turn instruction and data cache on

	InitGraphics(16);							// Init Graphics and Fonts
	giSurface = 1;
	OkfInitialize(gtSurface, &giSurface);

	OkfRegister(verdana8b_pm_okf);				// Register some fonts (internal)
	OkfRegister(verdana10_okf);
	OkfRegister(comicSansMS12b_pm_okf);

	okf.font = 1;								// Use font #1
	
	GpFatInit();								//initializes file system
		
	WindowInit();								//initializes windowing system
	
	ShowTitleScreen();
	
	fill_main_menu(&main_menu);					// Init the menus
	fill_statesconfig_menu(&statesconfig_menu);  
	fill_videoconfig_menu(&videoconfig_menu);
	fill_keyconfig_menu(&keyconfig_menu);
	fill_horz_keys_menu(&horz_keys_menu);
	fill_vert_keys_menu(&vert_keys_menu);
	
	GpDirCreate("gp:\\GPMM", NOT_IF_EXIST);		// Init the directory structure
	GpDirCreate("gp:\\GPMM\\WB32", NOT_IF_EXIST);
	GpDirCreate("gp:\\GPMM\\WB32\\SAV",NOT_IF_EXIST);
	GpDirCreate("gp:\\GPMM\\WB32\\CFG",NOT_IF_EXIST);
	GpDirCreate("gp:\\GPMM\\WB32\\SRAM",NOT_IF_EXIST);
	GpDirCreate("gp:\\GPMM\\WB32\\ROM",NOT_IF_EXIST);

	ShowCredits();								// show the opening dialog
	
	result = GpFileOpen("gp:\\GPMM\\WB32\\CFG\\DEFAULT.CFG",OPEN_R, &F);
	
	if (result == SM_OK)
	{
		GpFileClose(F);
		LoadConfig("gp:\\GPMM\\WB32\\CFG\\DEFAULT.CFG");
	}

    result = MENU_MAIN_LOAD_ROM; // start by loading a ROM
    
	while(1)
	{
		switch(result)
		{
		case MENU_MAIN_LOAD_ROM:
			{
				if (DoFileSelector(fname) == 0)
				{
					while (GpKeyGet());  // wait for keyrelease			
					break;
				}
								
				if (running == 1)					// release old memory if a rom was already loaded
				{
					running = 0;
					ws_done();
				}
									
				setCpuSpeed(132);

				if (!LoadRom(fname))
				{
					PrintError("Error Loading File!", 1);
					GpAppExit();
				}

				setCpuSpeed(66);
				
				ws_init(base_rom, romsize, ws_file);	// initialize the emulator
				
				vert = ws_rotated();
				
				if (vert == 1)
					videoconfig_menu.options[MENU_VIDEOCONFIG_STRETCH].selected = 3;
				
				pext = (char*)yo_strrchr(fname, '.'); 	// cut off extension of filename (for use with save states and config files)
				*pext = '\0';
				
				pext = (char*)yo_strrchr(fname, '\\');
				if (pext)
					GPSTRCPY(fname,pext+1);

				GPSPRINTF(g_string, "gp:\\GPMM\\WB32\\SRAM\\%s.srm",fname); // sram save = filename.srm
				
				result = GpFileOpen(g_string,OPEN_R, &F);
				
				if (result == SM_OK)
				{
					GpFileClose(F);
					ws_sram_load(g_string);
					PrintMessage("Loading SRAM...",0);
				}
			
				GPSPRINTF(g_string, "gp:\\GPMM\\WB32\\CFG\\%s.cfg", fname); // config = filename.cfg
				
				result = GpFileOpen(g_string,OPEN_R, &F);
	
				if (result == SM_OK)
				{
					GpFileClose(F);
					LoadConfig(g_string);
				}

				running = 1;							// rom was loaded, so set to 1
				
			} // Fall through and reset, then emulate
		case MENU_MAIN_WS_RESET:
			{
				if (running != 1)
					break;

				if (ws_sram_dirty == 1)
				{
					ws_sram_dirty = 0;
					GPSPRINTF(g_string, "gp:\\GPMM\\WB32\\SRAM\\%s.srm",fname); // sram save = filename.srm
					PrintMessage("Saving SRAM...",0);
					ws_sram_save(g_string);
				}
				
				ws_reset();
			} // Fall through and emulate
		case MENU_CANCEL:
		case MENU_MAIN_PLAY:
			{
				if (running != 1)							// can't play if no rom is loaded
					break;

				Emulate();								// run
			}
			break;
		case MENU_MAIN_REBOOT:
			{
				GpAppExit();									// reboot GP32
			}
			break;
		case MENU_MAIN_CONFIG_STATES:
			{
				result = run_menu(&statesconfig_menu);
				
				switch(result)
				{
				case MENU_STATECONFIG_LOAD_STATE:
					{
						char stateName[30];						// holds savestate filename

						if (running != 1)						// can't load state if no rom is loaded
							break;

						GPSPRINTF(stateName, "gp:\\GPMM\\WB32\\SAV\\%s.sa%d", fname, statesconfig_menu.options[MENU_STATECONFIG_SAVESLOT].selected+1); // savename = filename + saveslot
						ws_loadState(stateName);						// load the state data

						Emulate();										// run
					}
					break;
				case MENU_STATECONFIG_SAVE_STATE:
					{
						char stateName[30];						// holds savestate filename

						if (running != 1)							// can't save state if no rom is loaded
							break;

						GPSPRINTF(stateName, "gp:\\GPMM\\WB32\\SAV\\%s.sa%d", fname, statesconfig_menu.options[MENU_STATECONFIG_SAVESLOT].selected+1); // savename = filename + saveslot
						ws_saveState(stateName);
						
						Emulate();										// run
					}
					break;
				}
			}
			break;			
		case MENU_MAIN_CONFIG_VIDEO:
			{
				result = 0;
				
				while ((result != MENU_VIDEOCONFIG_RETURN) && (result != MENU_CANCEL))
				{
					result = run_menu(&videoconfig_menu);
				}
			}
			break;			
		case MENU_MAIN_CONFIG_KEYS:
			{
				result = 0;

				while ((result != MENU_KEYCONFIG_RETURN) && (result != MENU_CANCEL))
				{
					result = run_menu(&keyconfig_menu);							// run the key config stuff

					switch(result)
					{
					case MENU_KEYCONFIG_CONFIG_HORZ:
						{
							run_menu(&horz_keys_menu);
						}
						break;
					case MENU_KEYCONFIG_CONFIG_VERT:
						{
							run_menu(&vert_keys_menu);
						}
						break;
					case MENU_KEYCONFIG_DEF_HORZ:
						{
							horz_keys_menu.options[MENU_KEYS_UP].selected		= 0;
							horz_keys_menu.options[MENU_KEYS_LEFT].selected		= 2;
							horz_keys_menu.options[MENU_KEYS_RIGHT].selected	= 3;
							horz_keys_menu.options[MENU_KEYS_DOWN].selected		= 1;
							horz_keys_menu.options[MENU_KEYS_A].selected		= 8;
							horz_keys_menu.options[MENU_KEYS_B].selected		= 9;
							horz_keys_menu.options[MENU_KEYS_START].selected	= 10;
							horz_keys_menu.options[MENU_KEYS_L].selected		= 6;
							horz_keys_menu.options[MENU_KEYS_R].selected		= 7;
						}
						break;
					case MENU_KEYCONFIG_DEF_VERT:
						{
							vert_keys_menu.options[MENU_KEYS_UP].selected		= 4;
							vert_keys_menu.options[MENU_KEYS_LEFT].selected		= 6;
							vert_keys_menu.options[MENU_KEYS_RIGHT].selected	= 7;
							vert_keys_menu.options[MENU_KEYS_DOWN].selected		= 5;
							vert_keys_menu.options[MENU_KEYS_A].selected		= 3;
							vert_keys_menu.options[MENU_KEYS_B].selected		= 1;
							vert_keys_menu.options[MENU_KEYS_START].selected	= 10;
							vert_keys_menu.options[MENU_KEYS_L].selected		= 2;
							vert_keys_menu.options[MENU_KEYS_R].selected		= 0;
						}
						break;
					default:
						;
					}
				}
			}
			break;
		case MENU_MAIN_SAVE_DEF_CFG:
			{
				main_menu.selected = MENU_MAIN_CPUSPEED;
				SaveConfig("gp:\\GPMM\\WB32\\CFG\\DEFAULT.CFG");
				main_menu.selected = MENU_MAIN_SAVE_DEF_CFG;
			}
			break;
		case MENU_MAIN_SAVE_GME_CFG:
			{
				if (running == 0)
					break;

				GPSPRINTF(g_string,"gp:\\GPMM\\WB32\\CFG\\%s.cfg",fname);
				
				main_menu.selected = MENU_MAIN_CPUSPEED;
				SaveConfig(g_string);
				main_menu.selected = MENU_MAIN_SAVE_GME_CFG;
			}
			break;
		default:
			;
		}

		result = run_menu(&main_menu);			// run the main menu
	}

	while(1) ;
}
