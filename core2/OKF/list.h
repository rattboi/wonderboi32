/*************************************************************************

  list.h

    To maintain a chained list of dynamically allocated items.
    list.h is part of the Oankali GP32 run-time library.

  Copyright (c) 2004 by
  Guillem Reinoso <oankali@andorra.ad>

  These software is freeware and is provided as is.
  If it doesn't work for you or it makes your PC melt down, it's not my
  responsibility.
  If you use it in your programs, cite me please.

**************************************************************************/


#ifndef __list_h__
#define __list_h__

#ifdef __cplusplus
extern "C" {
#endif


// Structures
//- An item of the list
typedef struct tagLISTITEM {
  struct tagLIST     *pList;       // a pointer to the header of the list
  struct tagLISTITEM *pPrevious;   // a pointer to the previous item in the list
  struct tagLISTITEM *pNext;       // a pointer to the next item in the list

  void *pObject;                   // a pointer to the object data
  unsigned char autoAllocated;     // TRUE if object has been created automatically
  unsigned short handle;           // unique handle of the item
} LISTITEM;

//- A list of objects
typedef struct tagLIST {
  LISTITEM *pFirst;                // a pointer to the first item in the list
  LISTITEM *pLast;                 // a pointer to the last item in the list
  unsigned short count;            // number of items in the list
  unsigned short lastHandle;       // last given unique item handle
} LIST;


// Prototypes
LISTITEM *ListAddItem(LIST *pList, void *pObject, unsigned long size);
LIST     *ListCreate(void);
void      ListDestroy(LIST *pList);
int       ListGetIndex(LISTITEM *pItem);
int       ListGetIndexByHandle(LIST *pList, int handle);
LISTITEM *ListGetItemByHandle(LIST *pList, int handle);
LISTITEM *ListGetItemByIndex(LIST *pList, int index);
LISTITEM *ListGetItemByObject(LIST *pList, void *pObject);
void     *ListGetObjectByHandle(LIST *pList, int handle);
void     *ListGetObjectByIndex(LIST *pList, int index);
LISTITEM *ListGetRelativeItem(LISTITEM *pItem, int relIndex);
int       ListGetRelativePosition(LISTITEM *pItem1, LISTITEM *pItem2);
void      ListRemoveItem(LISTITEM *pItem);
void      ListRemoveItemByHandle(LIST *pList, int handle);
void      ListRemoveItemByIndex(LIST *pList, int index);
void      ListRemoveItemByObject(LIST *pList, void *pObject);
void      ListSort(LIST *pList, int (*pCompare)(LISTITEM *pItem1, LISTITEM *pItem2));


#ifdef __cplusplus
  }
#endif

#endif /*__list_h__*/
