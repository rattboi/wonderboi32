/*************************************************************************

  folder.h

    To retrieve a list of files and/or folders from your SMC card.
    folder.h is part of the Oankali GP32 run-time library.

  Copyright (c) 2004 by
  Guillem Reinoso <oankali@andorra.ad>

  These software is freeware and is provided as is.
  If it doesn't work for you or it makes your PC melt down, it's not my
  responsibility.
  If you use it in your programs, cite me please.

**************************************************************************/


#ifndef __folder_h__
#define __folder_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <gpstdio.h>

#include "list.h"


// Enumerations
typedef enum { fldOk, fldMemory, fldBadFolder, fldAttributes } FLDERROR;


// Constants
#define FLD_FILES         0x01
#define FLD_FOLDERS       0x02
#define FLD_EXPLORE       0x04
#define FLD_ALL           0xFF

#define FLD_MAXFILENAME   8
#define FLD_MAXEXTENSION  (1+3)
#define FLD_MAXPATH       64
#define FLD_MAXFILEPATH   (FLD_MAXPATH+FLD_MAXFILENAME+FLD_MAXEXTENSION)


// Structures
//- A folder entry
typedef struct tagFOLDERENTRY {
  unsigned char filename[FLD_MAXFILENAME + FLD_MAXEXTENSION + 1];
  GPFILEATTR attr;
} FOLDERENTRY;

//- A folder
typedef struct tagFOLDER {
  unsigned char path[FLD_MAXPATH + 1];
  LIST *pEntries;
  LISTITEM *pTop;
  LISTITEM *pCurrent;
  int totalFiles;
  int totalSize;
} FOLDER;


// Prototypes
FLDERROR FolderGet(char *path, char *filters, short options, FOLDER *pFolder);
void     FolderDrop(FOLDER *pFolder);


#ifdef __cplusplus
  }
#endif

#endif /*__folder_h__*/
