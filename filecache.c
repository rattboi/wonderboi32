#include <gpstdlib.h>
#include <gpstdio.h>
#include <gpgraphic.h>
#include <gpmain.h>
#include <gpfont.h>

#include "unzip.h"

#include "filecache.h"

#include "crc.h"
#include "goodwsx.h"

#include "types.h"

unsigned long	MyGameSize; 
unsigned char	*MyGame; 
unsigned long	MyGameNo;

ERR_CODE		err;
char			g_path_curr[MAX_PATH_LEN + 1];		//current path (in full path format)
GPDIRENTRY		g_list_file[MAX_COUNT_FILE];		//list of files in current path
unsigned long	g_cnt_file = 0;						//number of files in current path

F_HANDLE		g__file;

inifile			ini;

char szRomName[255];

void LoadPacked(int skip, int gz_size, int gz_crc)
{
	z_stream d_stream; /* decompression stream */
	void *temp; int i; 

    {
		unsigned long tt; 
		int lsize=((int)(GPAVAILABLEMEM()/65536))*65536;

		lsize=8192; //patch for low mem (<512k), used by fGB

		temp=(void *)GPMALLOC(lsize+256);  //extra bytes

		d_stream.zalloc = (alloc_func)0;
		d_stream.zfree = (free_func)0;
		d_stream.opaque = (voidpf)0;

		d_stream.next_in   = (Bytef *)temp; //skip
		d_stream.avail_in  = (uLongf)lsize; //-skip-8 //solo con MyGameSize quedaria ok
		d_stream.next_out  = (Bytef *)&MyGame[0];
		d_stream.avail_out = (uLongf)gz_size;

		i=inflateInit2(&d_stream, -MAX_WBITS);

		GpFileSeek(g__file,FROM_BEGIN,skip,(long *)&tt);

		do
		{
			GpFileRead(g__file,temp,lsize,&tt); 

			d_stream.next_in   = (Bytef *)temp; 
			d_stream.avail_in  = (uLongf)lsize;
		}
		while(inflate(&d_stream, Z_SYNC_FLUSH)!=Z_STREAM_END);

		i=inflateEnd(&d_stream);

		MyGameSize=gz_size;

		GPFREE(temp);
    }
    
    for(CRC=i=0;i<MyGameSize;i++) GetCRC(MyGame[i]);
}

//ok

void fs_add(char type,unsigned short entry,char *file,unsigned long offset)
{
	ini.game[ini.num_games_in_ini].type=type;
	ini.game[ini.num_games_in_ini].entry=entry;
	ini.game[ini.num_games_in_ini].offset=offset;
	GPSPRINTF(ini.game[ini.num_games_in_ini].file,"%s",file);
	ini.game[ini.num_games_in_ini].file[12]='\0'; //force string

	ini.num_games_in_ini++;
}

int fs_loadgame(char *dir, char *name, unsigned long *CRC32, int entry, int force_type) 
{
	char filename[255];
	F_HANDLE file;
	int i;
	int test=1;


	if(force_type==UNK)
	{
		test=0;
		force_type=ini.game[entry].type;
		GPSPRINTF(filename,"%s\\%s",dir,ini.game[entry].file);
		PrintMessage(filename,1);
	}
	else
		GPSPRINTF(filename,"%s\\%s",dir,name);  

	switch(force_type)
	{
	case GZ:  
		{
			int skip; uLongf gz_crc,gz_size; 

			GpFileOpen(filename, OPEN_R, &file);
			GpFileRead(file, (void *)&MyGame[0], 1024, &MyGameSize);
			GpFileClose(file);

			GpFileGetSize(filename, &MyGameSize);

			//gzip
			//ID1,ID2,CM,FLG,MTIME-MTIME-MTIME-MTIME,XFL,OS
			skip=10;          //1000
			if(MyGame[3]&4)  { skip+=4; skip+=(MyGame[12]<<8)|MyGame[13]; }	//FLG.FEXTRA (4 o 2?)
            if(MyGame[3]&8)  { /*int l=0;*/ while(MyGame[skip++]) ; }		//FLG.FNAME
			if(MyGame[3]&16) { while(MyGame[skip++]); }						//FLG.FCOMMENT
			if(MyGame[3]&2)  { skip+=2; }									//FLG.FHCRC

			gz_size=(MyGame[MyGameSize-1]<<24)|(MyGame[MyGameSize-2]<<16)|(MyGame[MyGameSize-3]<<8)|(MyGame[MyGameSize-4]);
			gz_crc =(MyGame[MyGameSize-5]<<24)|(MyGame[MyGameSize-6]<<16)|(MyGame[MyGameSize-7]<<8)|(MyGame[MyGameSize-8]);

            if(!test) 
			{ 
				GpFileOpen(filename, OPEN_R, &g__file);
                LoadPacked(skip,gz_size,gz_crc);
                GpFileClose(g__file);
			}

			*CRC32=gz_crc;

  	  		return 1;
		}
		break;

	case UNP: 
		{
			GpFileGetSize(filename, &MyGameSize);

			GpFileOpen(filename, OPEN_R, &file);
			GpFileRead(file, (void *)&MyGame[0], MyGameSize, &MyGameSize);
			GpFileClose(file);

			for(CRC=i=0;i<MyGameSize;i++)
				GetCRC(MyGame[i]);

			*CRC32=CRC;

  	 		return 1;
		}
		break;
	
	case ZIP:
		{
			int skip;
			uLongf gz_crc, gz_size;

			GpFileOpen(filename, OPEN_R, &file);

			GpFileRead(file, (void *)&MyGame[0], 1024, &MyGameSize);

			GpFileClose(file);

			GpFileGetSize(filename, &MyGameSize); //nuevo!

			skip=30;
			skip+=(MyGame[27]<<8)|MyGame[26]; //filename length
			skip+=(MyGame[29]<<8)|MyGame[28]; //extra fields length

			gz_size=(MyGame[25]<<24)|(MyGame[24]<<16)|(MyGame[23]<<8)|(MyGame[22]);
			gz_crc =(MyGame[17]<<24)|(MyGame[16]<<16)|(MyGame[15]<<8)|(MyGame[14]);

			if(!test) 
			{ 
				GpFileOpen(filename, OPEN_R, &g__file);
				LoadPacked(skip,gz_size,gz_crc);
				GpFileClose(g__file);
			}

			*CRC32=gz_crc;
			
			return 1;
		}
		break;
	}
	return 0;
}

//ok
void fs_scanfile(char *dir, char *name)
{
	char filename[255]; 
	char head[]="abcdef"; 
	F_HANDLE file; 
	unsigned long crc=0; 
	int i;

	MyGameSize=0;

	GPSPRINTF(filename,"%s\\%s",dir,name);

	if(GpFileOpen(filename, OPEN_R, &file)!=SM_OK) return;
	GpFileRead(file, (void *)&head[0], 4, &crc); //crc unused
	GpFileClose(file);
	if(crc==0) return;

	if(head[0]=='P'&&head[1]=='K') //zip
	{

		fs_loadgame(dir,name,&crc,0,ZIP);
 

		i=DAT_LookFor(crc);

		if(i!=-1) 
		{
			fs_add(ZIP,i,name,0);
		}
		else 
		{
			fs_add(ZIP,65535,name,0); //deberia almacenar "real_filename"
		}
	}
	else
	if(head[0]==0x1f&&head[1]==0x8b) //gzip
	{
		fs_loadgame(dir,name,&crc,0,GZ);
 
		i=DAT_LookFor(crc);

		if(i!=-1)
		{
			fs_add(GZ,i,name,0);
		}
		else 
		{
			fs_add(GZ,65535,name,0); //deberia almacenar "real_filename"
		}
	}
	else
	{
		{
			fs_loadgame(dir,name,&crc,0,UNP);

			i=DAT_LookFor(crc);

			if(i!=-1)
			{
				fs_add(UNP,i,name,0);
			}
			else
			{
				fs_add(UNP,65535,name,0); //deberia almacenar "name sin ."
			}
		}
	}
}





//ok
void fs_write_ini(char *dir, char *name)
{
	char filename[255]; F_HANDLE file; unsigned long size;
	int i,j;

	GPSPRINTF(filename,"%s\\%s",dir,name);

	if(GpFileCreate(filename, ALWAYS_CREATE, &file)!=SM_OK) 
		return;

	GpFileClose(file);

	if(GpFileOpen(filename, OPEN_W, &file)!=SM_OK) 
		return;

	//sort
	for(i=0;i<ini.num_games_in_ini-1;i++)
	{
		for(j=i+1;j<ini.num_games_in_ini;j++)
		{
			char str1[255], str2[255]; ini_game k;

			if(ini.game[i].entry==65535) 
			{
				GPSPRINTF(str1,ini.game[i].file);
			}
			else
			{
				GPSPRINTF(str1,"%s",MyDat[ini.game[i].entry].name);
			}

			if(ini.game[j].entry==65535)
			{
				GPSPRINTF(str2,ini.game[j].file);
			}
			else
			{
				GPSPRINTF(str2,"%s",MyDat[ini.game[j].entry].name);
			}

			if(strcmp(str1,str2)>0)
			{
				GPMEMCPY(&k,&ini.game[i],sizeof(ini_game));
				GPMEMCPY(&ini.game[i],&ini.game[j],sizeof(ini_game));
				GPMEMCPY(&ini.game[j],&k,sizeof(ini_game));
			}
		}
	}

	ini.version_ini=1;//version_emu;
	size=sizeof(int)*2+sizeof(ini.game[0])*ini.num_games_in_ini;
	GpFileWrite(file, (void *)&size, sizeof(unsigned long));
	GpFileWrite(file, (void *)&ini, size);
	GpFileClose(file);
}

//ok
int fs_read_ini(char *dir, char *name)
{
	char filename[255]; 
	F_HANDLE file; 
	unsigned long size,temp;

	GPMEMSET(&ini,0,sizeof(ini));

	GPSPRINTF(filename,"%s\\%s",dir,name);

	if(GpFileOpen(filename, OPEN_R, &file)!=SM_OK) 
		return 0;

	GpFileRead(file, (void *)&size, sizeof(unsigned long), &temp);
	GpFileRead(file, (void *)&ini, size, &temp);
	GpFileClose(file);

	while(ini.game[ini.num_games_in_ini].type!=UNK) ini.num_games_in_ini++;

	return 1;
}


//ok
void fs_scandir(char *dir, char *name)
{
	int i;
	char temp[255];

	GPSPRINTF(g_path_curr,"%s\\",dir);

	// GpFatInit();

	if(!fs_read_ini(dir,name))
	{
		if(ini.version_ini!=1)//version_emu)
		{
			if(ini.version_ini==0)
				GPSPRINTF(temp,"Welcome!\nPress A for a new scanning");
			else
				GPSPRINTF(temp,"Another emulator version detected\nPress A to force a new scanning");

//			PrintMessage(temp,1);
		}

		PrintMessage("Reading dir, please wait",0);

		GpDirEnumList(g_path_curr, 0, MAX_COUNT_FILE, (GPDIRENTRY*)&g_list_file, &g_cnt_file);

		for(i=0;i<g_cnt_file;i++)
		{
			GPSPRINTF(temp," Scanning file %04d/%04d (%03d%%) \n%s",i+1,g_cnt_file,(i+1)*100/g_cnt_file,g_list_file[i].name);
			PrintMessage(temp,0);
//			DrawPercentageBar(110, 130, i, 0, g_cnt_file-1);
//			FlipScreen(1,0);

			fs_scanfile(dir,g_list_file[i].name);
		}

		if(ini.num_games_in_ini) fs_write_ini(dir,name); 
	}
}



void fs(char *dir, char *name, unsigned char *dest)
{

	MyGame=(unsigned char*)dest;

	fs_scandir(dir,name);

//	fs_loadgame(dir,"",&crc,choose,UNK);

//	MyGameNo=choose;
}
