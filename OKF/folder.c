/*************************************************************************

  folder.c

    To retrieve a list of files and/or folders from your SMC card.
    folder.c is part of the Oankali GP32 run-time library.

  Copyright (c) 2004 by
  Guillem Reinoso <oankali@andorra.ad>

  These software is freeware and is provided as is.
  If it doesn't work for you or it makes your PC melt down, it's not my
  responsibility.
  If you use it in your programs, cite me please.

**************************************************************************/


#include <string.h>

#include <gpstdlib.h>
#include <gpstdio.h>
#include <gpmem.h>

#include "folder.h"


//*** Private prototypes
static int FilterString(char *str, char *filter);
static int SortFiles(LISTITEM *pItem1, LISTITEM *pItem2);


//*** Public implementations

void FolderDrop(FOLDER *pFolder)
{
  ListDestroy(pFolder->pEntries);
  pFolder->pEntries = NULL;
}


FLDERROR FolderGet(char *path, char *filters, short options, FOLDER *pFolder)
{
  ERR_CODE result;
  GPDIRENTRY *pDirEntries;
  GPFILEATTR attr;
  LISTITEM *pItem;
  FOLDERENTRY *pEntry;
  char filename[FLD_MAXPATH + FLD_MAXFILENAME + FLD_MAXEXTENSION + 1];
  char filter[FLD_MAXFILENAME + FLD_MAXEXTENSION + 1];
  char str[FLD_MAXFILENAME + 1], flt[FLD_MAXFILENAME + 1];
  ulong i, nbEntries, nbRead;
  int bFirstLoop;
  char *pFilterPos, *pPos;

  // complete parameters if needed
  if (path == NULL)
    path = "gp:\\";
  if (filters == NULL)
    filters = "*.*";

  // init folder object
  strlwr(gm_strcpy(pFolder->path, path));
  pPos = pFolder->path + gm_lstrlen(pFolder->path) - 1;
  if (*pPos != '\\')
  {
    *(++pPos) = '\\';
    *(++pPos) = '\0';
  }
  if ((pFolder->pEntries = ListCreate()) == NULL)
    return fldMemory;
  pFolder->pTop = NULL;
  pFolder->pCurrent = NULL;
  pFolder->totalFiles = 0;
  pFolder->totalSize = 0;

  // read all folder entries
  result = GpDirEnumNum(pFolder->path, &nbEntries);
  if (result != SM_OK)
    return fldBadFolder;
  if (nbEntries == 0)
    return fldOk;

  pDirEntries = gm_malloc(sizeof(*pDirEntries) * nbEntries);
  if (!pDirEntries)
    return fldMemory;

  result = GpDirEnumList(pFolder->path, 0, nbEntries, pDirEntries, &nbRead);
  if (result != SM_OK)
  {
    gm_free(pDirEntries);
    return fldBadFolder;
  }
  if (nbRead == 0)
  {
    gm_free(pDirEntries);
    return fldOk;
  }

  // loop on filters
  bFirstLoop = TRUE;
  pFilterPos = filters;
  while (*pFilterPos)
  {
    // get next filter
    gm_memcpy(filter, pFilterPos, sizeof(filter));
    if ((pPos = strchr(filter, ';')) != NULL)
    {
      *pPos = '\0';
      pFilterPos++;
    }
    pFilterPos += gm_lstrlen(filter);
    if (strchr(filter, '.') == NULL)
      gm_strcat(filter, ".*");
    strlwr(filter);

    // if blank filter, continue
    if (!filter[0])
      continue;

    // add filtered files/folders to list
    for (i=0; i < nbRead; i++)
    {
      // if entry has already been managed, continue
      if (!pDirEntries[i].name[0])
        continue;

      // folder . is always invalid
      if ((pDirEntries[i].name[0] == '.') &&
          (pDirEntries[i].name[1] == '\0'))
      {
        pDirEntries[i].name[0] = '\0';
        continue;
      }

      // folder .. can be valid
      if ((pDirEntries[i].name[0] == '.') &&
          (pDirEntries[i].name[1] == '.') &&
          (pDirEntries[i].name[2] == '\0'))
      {
        // add folder to list, if allowed
        if (options & FLD_EXPLORE)
        {
          pItem = ListAddItem(pFolder->pEntries, NULL, sizeof(FOLDERENTRY));
          if (pItem == NULL)
          {
            gm_free(pDirEntries);
            return fldMemory;
          }
          // set attributes manually
          pEntry = (FOLDERENTRY *) pItem->pObject;
          gm_strcpy(pEntry->filename, pDirEntries[i].name);
          pEntry->attr.attr = 0x10;  // directory
          pEntry->attr.size = 0;
        }
        pDirEntries[i].name[0] = '\0';
        continue;
      }

      // we look for folders only in first loop
      if (bFirstLoop)
      {
        strlwr(pDirEntries[i].name);
        gm_strcat(gm_strcpy(filename, pFolder->path), pDirEntries[i].name);
        result = GpFileAttr(filename, &attr);
        if (result != SM_OK)
        {
          gm_free(pDirEntries);
          return fldAttributes;
        }

        // it is a folder
        if (attr.attr & 0x10)
        {
          // add folder to list, if allowed
          if (options & (FLD_FOLDERS | FLD_EXPLORE))
          {
            // add folder to list
            if ((pItem = ListAddItem(pFolder->pEntries, NULL, sizeof(FOLDERENTRY))) == NULL)
            {
              gm_free(pDirEntries);
              return fldMemory;
            }

            // save attributes
            pEntry = (FOLDERENTRY *) pItem->pObject;
            gm_strcpy(pEntry->filename, pDirEntries[i].name);
            pEntry->attr = attr;
          }
          pDirEntries[i].name[0] = '\0';
          continue;
        }
        // it is a file
        else
          // if files are not allowed, invalidate
          if (!(options & FLD_FILES))
          {
            pDirEntries[i].name[0] = '\0';
            continue;
          }
      }

      // get name side of entry and filter
      if ((pPos = strchr(pDirEntries[i].name, '.')) != NULL)
      {
        *pPos = '\0';
        gm_strcpy(str, pDirEntries[i].name);
        *pPos = '.';
      }
      else
        gm_strcpy(str, pDirEntries[i].name);
      pPos = strchr(filter, '.');
      *pPos = '\0';
      gm_strcpy(flt, filter);
      *pPos = '.';

      // filter name
      if (FilterString(str, flt))
      {
        // get extension side of entry and filter
        if ((pPos = strchr(pDirEntries[i].name, '.')) != NULL)
          gm_strcpy(str, pPos + 1);
        else
          str[0] = '\0';
        pPos = strchr(filter, '.');
        gm_strcpy(flt, pPos + 1);

        // filter extension
        if (FilterString(str, flt))
        {
          // get file attributes, if needed
          if (!bFirstLoop)
          {
            gm_strcat(gm_strcpy(filename, pFolder->path), pDirEntries[i].name);
            result = GpFileAttr(filename, &attr);
            if (result != SM_OK)
            {
              gm_free(pDirEntries);
              return fldAttributes;
            }
          }
          // add file to list
          if ((pItem = ListAddItem(pFolder->pEntries, NULL, sizeof(FOLDERENTRY))) == NULL)
          {
            gm_free(pDirEntries);
            return fldMemory;
          }
          pFolder->totalFiles++;
          pFolder->totalSize += attr.size;

          // save attributes
          pEntry = (FOLDERENTRY *) pItem->pObject;
          gm_strcpy(pEntry->filename, pDirEntries[i].name);
          pEntry->attr = attr;

          pDirEntries[i].name[0] = '\0';
        }
      }
    }

    bFirstLoop = FALSE;
  }

  gm_free(pDirEntries);
  ListSort(pFolder->pEntries, &SortFiles);
  return fldOk;
}


//*** Private implementations

static int FilterString(char *string, char *filter)
{
  while (*string && *filter)
  {
    if (*filter == '?')
      *string++;

    else if (*filter == '*')
      do
      {
        string++;
        if (*string && (*string == *(filter + 1)))
          if (FilterString(string, filter + 1))
            return TRUE;
      }
      while (*string);

    else if (*filter == *string)
      string++;

    else
      return FALSE;

    filter++;
  }

  while (*filter == '*')
    filter++;

  return (!*string && !*filter);
}

static int SortFiles(LISTITEM *pItem1, LISTITEM *pItem2)
{
  FOLDERENTRY *pEntry1, *pEntry2;
  char *pPos1, *pPos2;
  int result;

  pEntry1 = (FOLDERENTRY *) pItem1->pObject;
  pEntry2 = (FOLDERENTRY *) pItem2->pObject;

  // Folder .. always first
  if ((pEntry1->filename[0] == '.') && 
      (pEntry1->filename[1] == '.') && 
      (pEntry1->filename[2] == '\0'))
    return -1;
  if ((pEntry2->filename[0] == '.') && 
      (pEntry2->filename[1] == '.') && 
      (pEntry2->filename[2] == '\0'))
    return +1;

  // Compare file type (folder goes before files)
  result = (pEntry2->attr.attr & 0x10) - (pEntry1->attr.attr & 0x10);

  // If same type
  if (!result)
  {
    if (pPos1 = strchr(pEntry1->filename, '.')) *pPos1 = '\0';
    if (pPos2 = strchr(pEntry2->filename, '.')) *pPos2 = '\0';

    // Compare file names
    result = strcmp(pEntry1->filename, pEntry2->filename);

    // If same file name, compare extensions
    if (!result && pPos1)
      result = strcmp(pPos1 + 1, pPos2 + 1);

    if (pPos1) *pPos1 = '.';
    if (pPos2) *pPos2 = '.';
  }

  return result;
}

