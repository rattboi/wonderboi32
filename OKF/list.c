/*************************************************************************

  list.c

    To maintain a chained list of dynamically allocated items.
    list.c is part of the Oankali GP32 run-time library.

  Copyright (c) 2004 by
  Guillem Reinoso <oankali@andorra.ad>

  These software is freeware and is provided as is.
  If it doesn't work for you or it makes your PC melt down, it's not my
  responsibility.
  If you use it in your programs, cite me please.

**************************************************************************/


#include <gpdef.h>
#include <gpstdlib.h>

#include "list.h"


// Implementacions
LISTITEM *ListAddItem(LIST *pList, void *pObject, unsigned long size)
{
  LISTITEM *pNew;

  if (!pList)
    return NULL;

  // allocate memory for the item
  pNew = gm_malloc(sizeof(LISTITEM));
  if (!pNew)
    return NULL;

  // allocate memory for the object if needed
  if (pObject == NULL)
  {
    pObject = gm_malloc(size);
    if (!pObject)
    {
      gm_free(pNew);
      return NULL;
    }
    pNew->autoAllocated = TRUE;
  }
  else
    pNew->autoAllocated = FALSE;

  // initialization
  pNew->pObject = pObject;
  pNew->handle = ++pList->lastHandle;

  // update pointers
  pNew->pList = pList;
  pNew->pPrevious = pList->pLast;
  pNew->pNext = NULL;
  if (pNew->pPrevious)
    pNew->pPrevious->pNext = pNew;

  if (!pList->pFirst)
    pList->pFirst = pNew;
  pList->pLast = pNew;
  pList->count++;

  return pNew;
}

LIST *ListCreate(void)
{
  LIST *pNew;

  // allocate memory
  pNew = gm_malloc(sizeof(LIST));
  if (!pNew)
    return NULL;

  // initialization
  pNew->pFirst = NULL;
  pNew->pLast = NULL;
  pNew->count = 0;
  pNew->lastHandle = 0;

  return pNew;
}

void ListDestroy(LIST *pList)
{
  LISTITEM *pItem;

  if (!pList)
    return;

  // destroy items in the list
  pItem = pList->pFirst;
  while (pItem)
  {
    pList->pFirst = pItem->pNext;

    // destroy object if needed
    if (pItem->autoAllocated)
      gm_free(pItem->pObject);

    gm_free(pItem);
    pList->count--;
    pItem = pList->pFirst;
  }

  // destroy list
  gm_free(pList);
}

int ListGetIndex(LISTITEM *pItem)
{
  int iCount=-1;

  if (pItem)
  {
    // count positions to first item
    do
    {
      iCount++;
      pItem = pItem->pPrevious;
    }
    while (pItem);
  }

  return iCount;
}

int ListGetIndexByHandle(LIST *pList, int handle)
{
  LISTITEM *pItem;
  int iCount=0;

  if (!pList)
    return -1;
  if ((handle <= 0) || (handle > pList->lastHandle))
    return -1;

  // look for the item in the list
  pItem = pList->pFirst;
  while (pItem)
  {
    if (pItem->handle == handle)
      return iCount;
    pItem = pItem->pNext;
    iCount++;
  }

  // handle not found
  return -1;
}

LISTITEM *ListGetItemByHandle(LIST *pList, int handle)
{
  LISTITEM *pItem;

  if (!pList)
    return NULL;
  if ((handle <= 0) || (handle > pList->lastHandle))
    return NULL;

  // look for the item in the list
  pItem = pList->pFirst;
  while (pItem)
  {
    if (pItem->handle == handle)
      return pItem;
    pItem = pItem->pNext;
  }

  // handle not found
  return NULL;
}

LISTITEM *ListGetItemByIndex(LIST *pList, int index)
{
  LISTITEM *pItem;
  int iCount=0;

  if (!pList)
    return NULL;
  if ((index < 0) || (index >= pList->count))
    return NULL;

  // look for the item in the list
  pItem = pList->pFirst;
  while (pItem)
  {
    if (iCount == index)
      return pItem;
    pItem = pItem->pNext;
    iCount++;
  }

  // index not found
  return NULL;
}

LISTITEM *ListGetItemByObject(LIST *pList, void *pObject)
{
  LISTITEM *pItem;

  if (!pList)
    return NULL;

  // search the object in the list
  pItem = pList->pFirst;
  while (pItem)
  {
    if (pItem->pObject == pObject)
      return pItem;
    pItem = pItem->pNext;
  }

  // object not found
  return NULL;
}

void *ListGetObjectByHandle(LIST *pList, int handle)
{
  LISTITEM *pItem;

  if ((pItem = ListGetItemByHandle(pList, handle)) == NULL)
    return NULL;

  return pItem->pObject;
}

void *ListGetObjectByIndex(LIST *pList, int index)
{
  LISTITEM *pItem;

  if ((pItem = ListGetItemByIndex(pList, index)) == NULL)
    return NULL;

  return pItem->pObject;
}

LISTITEM *ListGetRelativeItem(LISTITEM *pItem, int relIndex)
{
  if (!pItem)
    return NULL;
  if (!relIndex)
    return pItem;

  // backwards
  if (relIndex < 0)
    while (relIndex++)
      if (pItem->pPrevious)
        pItem = pItem->pPrevious;
      else
        break;

  // forward
  else
    while (relIndex--)
      if (pItem->pNext)
        pItem = pItem->pNext;
      else
        break;

  return pItem;
}

int ListGetRelativePosition(LISTITEM *pItem1, LISTITEM *pItem2)
{
  LISTITEM *pItem;
  int iCount;

  // diferent lists
  if (pItem1->pList != pItem2->pList)
    return 9999999;

  // same item
  if (pItem1 == pItem2)
    return 0;

  // item1 is next to item2
  iCount = 1;
  pItem = pItem2->pNext; 
  while (pItem)
  {
    if (pItem == pItem1)
      return iCount;
    pItem = pItem->pNext;
    iCount++;
  }

  // item1 is previous to item2
  iCount = -1;
  pItem = pItem2->pPrevious; 
  while (pItem != pItem1)
  {
    pItem = pItem->pPrevious;
    iCount--;
  }

  return iCount;
}

void ListRemoveItem(LISTITEM *pItem)
{
  LIST *pList;

  if (!pItem)
    return;

  pList = pItem->pList;

  // update pointers
  if (pItem->pPrevious)
    pItem->pPrevious->pNext = pItem->pNext;
  if (pItem->pNext)
    pItem->pNext->pPrevious = pItem->pPrevious;

  if (pList->pFirst == pItem)
    pList->pFirst = pItem->pNext;
  if (pList->pLast == pItem)
    pList->pLast = pItem->pPrevious;

  pList->count--;

  // destroy object if needed
  if (pItem->autoAllocated)
    gm_free(pItem->pObject);

  // destroy item
  gm_free(pItem);
}

void ListRemoveItemByHandle(LIST *pList, int handle)
{
  LISTITEM *pItem;

  if ((pItem = ListGetItemByHandle(pList, handle)) != NULL)
    ListRemoveItem(pItem);
}

void ListRemoveItemByIndex(LIST *pList, int index)
{
  LISTITEM *pItem;

  if ((pItem = ListGetItemByIndex(pList, index)) != NULL)
    ListRemoveItem(pItem);
}

void ListRemoveItemByObject(LIST *pList, void *pObject)
{
  LISTITEM *pItem;

  if ((pItem = ListGetItemByObject(pList, pObject)) != NULL)
    ListRemoveItem(pItem);
}

void ListSort(LIST *pList, int (*pCompare)(LISTITEM *pItem1, LISTITEM *pItem2))
{
  LISTITEM *pLast, *pItem;
  void *pSwap;
  int result;

  if (!pList || pList->count < 2)
    return;

  // Bubble sort
  pLast = pList->pLast->pPrevious;
  while (pLast)
  {
    pItem = pList->pFirst;
    while (pItem != pLast->pNext)
    {
      result = pCompare(pItem, pItem->pNext);
      if (result > 0)
      {
        pSwap = pItem->pObject;
        pItem->pObject = pItem->pNext->pObject;
        pItem->pNext->pObject = pSwap;
      }

      pItem = pItem->pNext;
    }
    pLast = pLast->pPrevious;
  }
}
