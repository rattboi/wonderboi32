#ifndef __FILECACHE_H__
#define __FILECACHE_H__

#define			MAXFS			11

#define			MAX_COUNT_FILE  10000				//maximum number of file entries in one folder
#define			MAX_PATH_LEN	255					//maximum length of full path name

enum
{
	UNK,
	UNP,
	GZ,
	ZIP
};

typedef struct {
	char type;
	unsigned short entry; 
	char file[8+3+1+1];
	unsigned long offset;
	unsigned long filesize;
} ini_game; 

typedef struct {
	int version_ini;
	int num_games_in_ini;
	ini_game game[MAX_COUNT_FILE]; //1000 roms max
} inifile;

extern  inifile			ini;
extern	unsigned long	MyGameSize; 
extern	unsigned char	*MyGame; 
extern	unsigned long	MyGameNo;

void	LoadPacked(int skip, int gz_size, int gz_crc);
void	fs_add(char type,unsigned short entry,char *file,unsigned long offset, unsigned long filesize);
int		fs_loadgame(char *dir, char *name, unsigned long *filesize, unsigned long *CRC32, int entry, int force_type);
void	fs_scanfile(char *dir, char *name);
void	fs_write_ini( );
int		fs_read_ini( );
void	fs_scandir(char *dir);
void	fs(char *dir, unsigned char *dest);

#endif

