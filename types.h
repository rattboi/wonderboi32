//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
// Copyright(C)2000-2001 by david Raingeard
// cinati@caramail.com or david.raingeard@etu.univ-tours.fr
// For more information please read the readme.txt file
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __TYPES_H__
#define __TYPES_H__

#ifndef NULL
#define NULL	0
#endif

#ifndef BYTE
#define BYTE	unsigned char
#endif

#ifndef WORD
#define WORD	unsigned short
#endif

#ifndef DWORD
#define DWORD	unsigned long
#endif

#define UINT8	BYTE
#define UINT16	WORD
#define INT8	signed char
#define INT16	signed short
#define INT32	signed long
#define UINT32	unsigned long

#define uint8	UINT8
#define uint16	UINT16
#define uint32	UINT32
#define int8	INT8
#define int16	INT16
#define int32	INT32

#define GPMEMSET		(gp_str_func.memset)
#define GPMEMCPY		(gp_str_func.memcpy)
#define GPSTRCPY		(gp_str_func.strcpy)
#define GPSTRNCPY		(gp_str_func.strncpy)
#define GPSTRCAT		(gp_str_func.strcat)
#define GPSTRNCAT		(gp_str_func.strncat)
#define GPSTRLEN		(gp_str_func.gpstrlen)

#define	GPSPRINTF		(gp_str_func.sprintf)
#define	GPTOUPPER		(gp_str_func.uppercase)
#define GPTOLOWER		(gp_str_func.lowercase)
#define GPSTRCMP		(gp_str_func.compare)
#define GPTRIMRIGHT		(gp_str_func.trim_right)

#define GPMALLOC		(gp_mem_func.malloc)
#define GPZIMALLOC		(gp_mem_func.zimalloc)
#define GPCALLOC		(gp_mem_func.calloc)
#define GPFREE			(gp_mem_func.free)
#define GPAVAILABLEMEM	(gp_mem_func.availablemem)
#define GPMALLOXEX		(gp_mem_func.malloc_ex)
#define GPFREEEX		(gp_mem_func.free_ex) 
#define GPMAKEMEMPART	(gp_mem_func.make_mem_partition)

#define	LCD_WIDTH	320	/*width  of LCD*/
#define	LCD_HEIGHT	240	/*height of LCD*/

#endif
