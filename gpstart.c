
#include "gpdef.h"
#include "gpstdlib.h"
#include "gpfont.h"
#include "gpfont_port.h"
#include "gpfontres.dat"

#include "initval_port.h"

#ifdef USE_GP_MEM
#include "gpmem.h"
#endif

unsigned int HEAPSTART;
unsigned int HEAPEND;

void InitializeFont (void);
//extern void GpKeyPollingTimeSet (int loop_cnt);

int main (int arg_len, char * arg_v)
   {

   GM_HEAP_DEF gm_heap_def;

#if CHANGE_MCLK
   GpClockSpeedChange (CLK_SPEED, DIV_FACTOR, CLK_MODE);
#endif

   _gp_sdk_init();

   // keyboard polling count setting
   GpKeyPollingTimeSet (KEYPOLLING_NUM);

#ifdef USE_GP_MEM
   gm_heap_def.heapstart = (void*)(HEAPSTART);
   gm_heap_def.heapend = (void *)(HEAPEND & ~3);
   gm_heap_init (&gm_heap_def);

   gp_mem_func.malloc = gm_malloc;
   gp_mem_func.zimalloc = gm_zi_malloc;
   gp_mem_func.calloc = gm_calloc;
   gp_mem_func.free = gm_free;
   gp_mem_func.availablemem = gm_availablesize;
   gp_mem_func.malloc_ex = gm_malloc_ex;
   gp_mem_func.free_ex = gm_free_ex;
   gp_mem_func.make_mem_partition = gm_make_mem_part;

   gp_str_func.memset = gm_memset;
   gp_str_func.memcpy = gm_memcpy;
   gp_str_func.strcpy = gm_strcpy;
   gp_str_func.strncpy = gm_strncpy;
   gp_str_func.strcat = gm_strcat;
   gp_str_func.strncat = gm_strncat;
   gp_str_func.gpstrlen = gm_lstrlen;
   gp_str_func.sprintf = gm_sprintf;
   gp_str_func.uppercase = gm_uppercase;
   gp_str_func.lowercase = gm_lowercase;
   gp_str_func.compare = gm_compare;
   gp_str_func.trim_right = gm_trim_right;
#endif /*USE_GP_MEM*/

	//Font initialize
   InitializeFont ();

   GpKernelInitialize ();
   GpKernelStart ();
   GpAppExit ();
   while (1) {};

   return 1;
}

void InitializeFont (void)
   {
   BGFONTINFO mInfo;
   mInfo.kor_w = KORFONT_W;
   mInfo.kor_h = KORFONT_H;
   mInfo.eng_w = ENGFONT_W;
   mInfo.eng_h = ENGFONT_H;
   mInfo.chargap = FONT_CHARGAP;
   mInfo.linegap = FONT_LINEGAP;
   GpFontInit (&mInfo);

	/* 16bits font init */
   mInfo.kor_w = KORFONT_W;
   mInfo.kor_h = KORFONT_H;
   mInfo.eng_w = ENGFONT_W;
   mInfo.eng_h = ENGFONT_H;
   mInfo.chargap = FONT_CHARGAP;
   mInfo.linegap = FONT_LINEGAP;
//   GpFontInit16 (&mInfo);

   GpFontResSet ((unsigned char*)fontresKor, (unsigned char*)fontresEng);
   }

int GpPredefinedStackGet (H_THREAD th)
   {
   switch (th)
      {
      case H_THREAD_GPMAIN:
         return GPMAIN_STACK_SIZE;
      case H_THREAD_NET:
         return NET_STACK_SIZE;
      case H_THREAD_TMR0:
      case H_THREAD_TMR1:
      case H_THREAD_TMR2:
      case H_THREAD_TMR3:
         return USER_STACK_SIZE;
      default:
         return 0;
      }
   }

