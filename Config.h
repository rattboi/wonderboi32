//---------------------------------------------------------------------------
#ifndef ConfigH
#define ConfigH
//---------------------------------------------------------------------------
#include <stdio.h>

#define FNAME_LENGTH 512

#define LK_TMP 0
#define LK_SIO 1
#define LK_SOUND 2
#define LK_SPRITE 3
extern FILE *Log;
extern int LogKind;

extern int Verbose;

#endif
