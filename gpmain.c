#include <gpstdlib.h>
#include <gpstdio.h>
#include <gpgraphic.h>
#include <gpmain.h>
#include <gpfont.h>

#include "misc_str_func.h" // by YoYoFr

#include "OKF/global.h"
#include "OKF/clock.h"
#include "OKF/graphics16.h"
#include "OKF/okf.h"

#include "OKF/fonts/verdana8b_pm.h"
#include "OKF/fonts/verdana8b_pm.c"

#include "menu.h"
#include "setcpuspeed.h"
#include "file_dialog.h"


#include "unzip.h"

#include "WSDraw.h"
#include "WS.h"

#include "types.h"

#define MAX_ROM_SIZE 4*1024*1024

char *long_program_version="WonderBoi32 v0.7 (new core)";
char *short_program_version="WB32 v.7";

extern unsigned short WBTitle[];
extern int vert;

extern int BGLayer;
extern int FGLayer;
extern int SpLayer;

ubyte *base_rom;
uint32 romSize;
char ws_file[256];
char basename[256];
char SaveName[256];

stMenu main_menu;
stMenu config_menu;
stMenu debug_menu;
stMenu horz_keys_menu;
stMenu vert_keys_menu;

int LoadState(void );
int SaveState(void );

void ShowCredits()
{
	char string_version[512];

	GPSTRCPY(string_version,long_program_version);
	GPSTRCAT(string_version,"\nPorted to GP32 by Rattboi\n");
	GPSTRCAT(string_version,"\nBased on WSCamp");
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

	if (pext) GPSTRNCPY(ext, pext, sizeof(ext));
	yo_strlwr(ext);
	if (GPSTRCMP(ext, ".zip") == 0)  
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

			if (GPSTRCMP(p, ".wsc") == 0)
				break;
			if (GPSTRCMP(p, ".ws") == 0)
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
		
		romSize = filesize;

		GPSPRINTF(printString,"Opening File...\nFile Size: %i", filesize);
		PrintMessage(printString,0);

		unzReadCurrentFile(zip_file, base_rom, romSize);
		
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

			romSize = filesize;
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
			GpFileClose(F);
			return 0;
		}

		GPSTRCPY(ws_file,filename);
		GpFileClose(F);
	}
	return 1;
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
	setCpuSpeed(cpuSpeeds[main_menu.options[MENU_MAIN_CPUSPEED].selected]);
	InitGraphics(8);

	SetDrawMode(config_menu.options[MENU_CONFIG_VIDEO_STRETCH].selected);
	FrameSkip	=	(config_menu.options[MENU_CONFIG_VIDEO_FRAMESKIP].selected);
	BGLayer		=	(debug_menu.options[MENU_DEBUG_BGLAYER].selected);
	FGLayer		=	(debug_menu.options[MENU_DEBUG_FGLAYER].selected);
	SpLayer		=	(debug_menu.options[MENU_DEBUG_SPRITELAYER].selected);

	WsInputInit(vert && (GetDrawMode() == 3));

//	PROFILER_START();
	while (WsRun() ==  0)	{ }
//	PROFILER_STOP();
//	PROFILER_DUMP();

	config_menu.options[MENU_CONFIG_VIDEO_FRAMESKIP].selected = FrameSkip;
	config_menu.options[MENU_CONFIG_VIDEO_STRETCH].selected = GetDrawMode();

	setCpuSpeed(66);
	InitGraphics(16);
}


int SaveStatesEmulate(char *savestate)
{
	int				i;
	int				StateDrawMode;
	char			stateName[30];						// holds savestate filename
	char			printstring[30];
	int				SaveStateSelected;
	static uint16	selectedState 		= 0;
	int				ExKey				= 0;
	int				LastKey				= -1;
	int				loadOK				= 0;

	asm volatile(
		"b afterpool\n" 
		".pool \n" 
		"afterpool:\n"
	);

	setCpuSpeed(cpuSpeeds[main_menu.options[MENU_MAIN_CPUSPEED].selected]);

	InitGraphics(8);

/*
	StateDrawMode = GetDrawMode();
	if (StateDrawMode == 3)
		SetDrawMode(5);
	else
		SetDrawMode(4);
*/

	StateInputInit();
//	GpSurfaceSet(&gtSurface[0]);
	
	while (1)
	{
		if (loadOK == 1)
			WsRun();

		GpKeyGetEx(&ExKey);

		if ((ExKey & GPC_VK_UP)  && !(LastKey & GPC_VK_UP)) // UP (previous state)
		{
			if (selectedState == 0)
				selectedState = 9;

			selectedState -=1;
		}
	
		if ((ExKey & GPC_VK_DOWN)  && !(LastKey & GPC_VK_DOWN)) // DOWN (next state)
		{
			selectedState += 1;
			selectedState %= 9;
		}

		if ((ExKey & GPC_VK_FB) && !(LastKey & GPC_VK_FB)) // B Pressed (cancel)
		{
			SaveStateSelected = 0;
			break;
		}

		if ((ExKey & GPC_VK_FA) && !(LastKey & GPC_VK_FA)) // A Pressed (selected)
		{
			GPSTRCPY(savestate, stateName);

			if (loadOK != -1)
				SaveStateSelected = 1;
			else
				SaveStateSelected = 2;

			break;
		}

		if (ExKey != LastKey)
		{
			GPSPRINTF(stateName, "gp:\\GPMM\\WB32\\SAV\\%s.sa%d", basename, selectedState + 1); // savename = filename + saveslot
			loadOK = WsLoadState(stateName);				// load the state data

			if (loadOK == -1)
			{
				if (GetDrawMode() == 4)
				{
					okf.x = 12;
					okf.y = 220;
				}
				else
				{
					okf.x = 12;
					okf.y = 225;
				}
				GPSPRINTF(printstring, "Slot %d: Empty Slot", selectedState + 1);
			}
			else
			{
				if (GetDrawMode() == 4)  // horizontal
				{
					okf.x = 12;
					okf.y = 220;
				}
				else
				{
					okf.x = 12;
					okf.y = 225;
				}

				GPSPRINTF(printstring, "Slot %d: %s.sa%d", selectedState + 1, basename, selectedState + 1);
				WsClearGpuCache();
//				RebuildPalette();
			}
//			OkfPrintSurface(printstring,giSurface);
		}
		LastKey = ExKey;
	}

//	SetDrawMode(StateDrawMode);
	setCpuSpeed(66);

	InitGraphics(16);

	return SaveStateSelected;
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

		for (i = 0; i < config_menu.num; i++)
			GpFileWrite(F, &config_menu.options[i].selected,sizeof(int));
		GpFileWrite(F, &config_menu.selected,sizeof(int));
		
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

	for (i = 0; i < config_menu.num; i++)
		GpFileRead(F, &config_menu.options[i].selected,sizeof(int),&sizeread);
	GpFileRead(F, &config_menu.selected,sizeof(int),&sizeread);
	
	for (i = 0; i < horz_keys_menu.num; i++)
		GpFileRead(F, &horz_keys_menu.options[i].selected,sizeof(int),&sizeread);
	GpFileRead(F, &horz_keys_menu.selected,sizeof(int),&sizeread);
	
	for (i = 0; i < vert_keys_menu.num; i++)
		GpFileRead(F, &vert_keys_menu.options[i].selected,sizeof(int),&sizeread);
	GpFileRead(F, &vert_keys_menu.selected,sizeof(int),&sizeread);

	GpFileClose(F);

	return(1);
}
void GetBaseName(char *fname, char *basename)
{
	char *pext;

	GPSTRCPY(basename,fname);

	pext = (char*)yo_strrchr(basename, '.'); 	// cut off extension of filename (for use with save states and config files)
	*pext = '\0';
	
	pext = (char*)yo_strrchr(basename, '\\');
	
	if (pext)
		GPSTRCPY(basename,pext+1);

	return;
}
void GpMain(void *arg)
{
	int i;
	char g_string[256];
	int result;
	F_HANDLE F;
	int running = 0;				// set to 1 if a rom is loaded
	int first_run = 1;				// used to see if Reset should save ram
	int horz_mode = 0;

	CPU_alignment_and_cache();		// turn off alignment and turn instruction and data cache on

	InitGraphics(16);							// Init Graphics and Fonts

	giSurface = 1;
	OkfInitialize(gtSurface, &giSurface);

	OkfRegister(verdana8b_pm_okf);

	okf.font = 1;								// Use font #1
	
	WindowInit();								//initializes windowing system

	GpFatInit();								//initializes file system

	GpDirCreate("gp:\\GPMM", NOT_IF_EXIST);		// Init the directory structure
	GpDirCreate("gp:\\GPMM\\WB32", NOT_IF_EXIST);
	GpDirCreate("gp:\\GPMM\\WB32\\SAV",NOT_IF_EXIST);
	GpDirCreate("gp:\\GPMM\\WB32\\CFG",NOT_IF_EXIST);
	GpDirCreate("gp:\\GPMM\\WB32\\RAM",NOT_IF_EXIST);
	GpDirCreate("gp:\\GPMM\\WB32\\ROM",NOT_IF_EXIST);

	base_rom = NULL;

    if(base_rom==NULL) base_rom=(byte *)gm_zi_malloc(MAX_ROM_SIZE);

	ShowCredits();								// show the opening dialog

	fill_main_menu(&main_menu);					// Init the menus
	fill_config_menu(&config_menu);  
	fill_debug_menu(&debug_menu);
	fill_horz_keys_menu(&horz_keys_menu);
	fill_vert_keys_menu(&vert_keys_menu);

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
				if (DoFileSelector(g_string, base_rom) == 0)
				{
					while (GpKeyGet());  // wait for keyrelease			
					break;
				}
								
				if (running == 1)					// release old memory if a rom was already loaded
				{
					running = 0;
					WsRelease();
				}

				setCpuSpeed(132);

				if (!LoadRom(g_string))
				{
					PrintError("Error Loading File!", 1);
					break;
				}

				setCpuSpeed(66);

				WsCreate(base_rom, romSize);
				
				if (vert == 1)
				{
					//if (config_menu.options[MENU_CONFIG_VIDEO_STRETCH].selected != 3)
					horz_mode = config_menu.options[MENU_CONFIG_VIDEO_STRETCH].selected;

					config_menu.options[MENU_CONFIG_VIDEO_STRETCH].selected = 3;
				}
				else
				{
					if (config_menu.options[MENU_CONFIG_VIDEO_STRETCH].selected == 3)
						config_menu.options[MENU_CONFIG_VIDEO_STRETCH].selected = horz_mode;
				}
				
				GetBaseName(g_string, basename);

				GPSPRINTF(SaveName, "gp:\\GPMM\\WB32\\RAM\\%s.ram",basename); // ram save = filename.ram

				PrintMessage(SaveName, 0);
				
				result = GpFileOpen(SaveName,OPEN_R, &F);
	
				if (result == SM_OK)
				{
					GpFileClose(F);
					PrintMessage("Loading RAM...",0);
					WsLoadRam(SaveName);
				}

				GPSPRINTF(g_string, "gp:\\GPMM\\WB32\\CFG\\%s.cfg", basename); // config = filename.cfg
				
				result = GpFileOpen(g_string,OPEN_R, &F);
	
				if (result == SM_OK)
				{
					GpFileClose(F);
					LoadConfig(g_string);
				}

				first_run = 1;
				running = 1;							// rom was loaded, so set to 1
				
			} // Fall through and reset, then emulate
		case MENU_MAIN_WS_RESET:
			{
				if (running != 1)
					break;
					
				if (first_run == 0)
				{
					GPSPRINTF(SaveName, "gp:\\GPMM\\WB32\\RAM\\%s.ram", basename); // ram save = filename.ram
					PrintMessage("Saving RAM...",0);
					WsSaveRam(SaveName);
				}

				first_run = 0;
				
				WsReset();
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
		case MENU_MAIN_LOAD_STATE:
			{
				if (running != 1)				// can't load state if no rom is loaded
					break;

				if (LoadState())
					Emulate();						// run
			}
			break;
		case MENU_MAIN_SAVE_STATE:
			{
				if (running != 1)				// can't save state if no rom is loaded
					break;

				if (SaveState())
					Emulate();						// run
			}
			break;
		case MENU_MAIN_CONFIG:
			{
				result = 0;
				
				while ((result != MENU_CONFIG_RETURN) && (result != MENU_CANCEL))
				{
					result = run_menu(&config_menu);

					switch(result)
					{
					case MENU_CONFIG_DEBUG:
						{
							run_menu(&debug_menu);
						}
						break;
					case MENU_CONFIG_KEYS_HORZ:
						{
							run_menu(&horz_keys_menu);
						}
						break;
					case MENU_CONFIG_KEYS_VERT:
						{
							run_menu(&vert_keys_menu);
						}
						break;
					case MENU_CONFIG_KEYS_DEF_HORZ:
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
					case MENU_CONFIG_KEYS_DEF_VERT:
						{
							vert_keys_menu.options[MENU_KEYS_UP].selected		= 4;
							vert_keys_menu.options[MENU_KEYS_LEFT].selected		= 6;
							vert_keys_menu.options[MENU_KEYS_RIGHT].selected	= 7;
							vert_keys_menu.options[MENU_KEYS_DOWN].selected		= 5;
							vert_keys_menu.options[MENU_KEYS_A].selected		= 1;
							vert_keys_menu.options[MENU_KEYS_B].selected		= 3;
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

				GPSPRINTF(g_string,"gp:\\GPMM\\WB32\\CFG\\%s.cfg",basename);
				
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


int LoadState(void)
{
	int			selection = 0;
	char		stateName[30];				// holds savestate filename
	gameState	*saveState;

	saveState = GPMALLOC(sizeof(gameState));

	if (saveState != NULL)
	{
		saveStateToMem(saveState);
		
		do
		{
			selection = SaveStatesEmulate(stateName);

			if (selection == 0)
			{
				loadStateFromMem(saveState);
			}
			else if (selection == 1)
			{
				WsLoadState(stateName);
			}
		}
		while(selection == 2);

		GPFREE(saveState);
	}
	return (selection);
}

int SaveState(void )
{
	int			selection = 0;
	char		stateName[30];				// holds savestate filename
	gameState	*saveState;

	saveState = GPMALLOC(sizeof(gameState));

	if (saveState != NULL)
	{
		saveStateToMem(saveState);
		selection = SaveStatesEmulate(stateName);

		loadStateFromMem(saveState);

		if (selection != 0)
		{
			WsSaveState(stateName);
		}

		GPFREE(saveState);
	}
	return (selection);
}
