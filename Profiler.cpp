#ifdef __cplusplus
extern "C" {
#endif

#include <gpdef.h>
#include <gpmain.h>
#include <gpos_def.h>
#include <gpstdio.h>
#include <gpgraphic.h>
#include <gpFont.h>
#include <gpstdlib.h>
#include <string.h>
//***************************************************************************//
#ifdef DEBUGGER_PRESENT
extern void DEBUG_Print(char *pFormat, ...);
#endif
//***************************************************************************//

#define PROF_SEC	600
#define PROF_FREQ	200
typedef struct {
	unsigned int	PC;
	unsigned int	LR;
} PCSample;
//***************************************************************************//
// Global variables.
PCSample		*g_pSamples = NULL;
unsigned int	g_uiTicks = 0;
unsigned int	g_uiNextSample;
bool			g_bProfilerRunning = false;
unsigned int	g_uiPC;
unsigned int	g_uiLR;
unsigned int	g_uiGPISR;
//***************************************************************************//
void PROFILER_START();
void PROFILER_STOP();
void PROFILER_DUMP();


//void ProfilerISR() __attribute__ ((naked));
void ProfilerISR() 
{
	g_uiNextSample++;
	if (g_uiNextSample >= PROF_SEC * PROF_FREQ) PROFILER_STOP();
}


void ProfilerISR3()
{
	asm volatile(
			"@ switch to supervisor where the code is executet most of the time \n"
			"mrs		r1, CPSR \n"
			"mov		r2, r1 \n"


			"@// Let us set the mode to supervisor so we can get r13 and r14 \n"
			"bic		r1, r1, #0x1f \n"
			"orr		r1, r1, #0x13 \n"
			"msr		CPSR_c, r1 \n"
			"mov		r3, #1 \n"
			"ldr		r2, [r1] \n"
			

			"@// Just remember that we're in the supervisor stack aswell now \n"
			"str		r13, [r4,#0x34] \n"
			"str		r14, [r4,#0x38] \n"
			"@// Lets move back to whatever mode we was in. \n"
			"@// Make sure that IRQ's are turned on \n"
			"bic		r2, r2, #0x80 \n"
			"msr		CPSR_fsxc, r2 \n"
		:
		:
		:"r0", "r1", "r2", "r3");

}






void ProfilerISR2() __attribute__ ((naked));
void ProfilerISR2()
{
	
	asm volatile(
			"sub		sp, sp, #0x04 \n"
			"stmdb	sp!, {r0, r1, r2, r3, ip} \n"
		:
		:
		:"r0");

	asm volatile(
	     	"str		%0, [sp, #20] \n"
			"str		r14, [%1] \n"
			"ldmia	sp!, {r0, r1, r2, r3, ip, pc} \n"
		:
		: "r" (g_uiGPISR), "r" (&g_pSamples[g_uiNextSample])
		:"r0");
}
//***************************************************************************//
//***************************************************************************//
/*
Dump of assembler code for function _GpProcTmr:
0xc021fb4 <_GpProcTmr>: ldr     r3, [pc, #108]  ; 0xc022028 <_GpProcTmr+116>
0xc021fb8 <_GpProcTmr+4>:       add     r3, r3, r2, lsl #4
0xc021fbc <_GpProcTmr+8>:       ldr     r1, [r3, #8]
0xc021fc0 <_GpProcTmr+12>:      mov     r0, #125        ; 0x7d
0xc021fc4 <_GpProcTmr+16>:      mov     r0, r0, lsl #3
0xc021fc8 <_GpProcTmr+20>:      bl      0xc021bb0 <ACHI_P_DIVIDE>
0xc021fcc <_GpProcTmr+24>:      mov     r0, r0, lsr #1
0xc021fd0 <_GpProcTmr+28>:      mov     r1, r3
0xc021fd4 <_GpProcTmr+32>:      ldr     r3, [r1, #12]
0xc021fd8 <_GpProcTmr+36>:      cmp     r3, #0  ; 0x0
0xc021fdc <_GpProcTmr+40>:      beq     0xc021ffc <_GpProcTmr+72>
0xc021fe0 <_GpProcTmr+44>:      stmdb   sp!, {r0, r1, r2, lr}
0xc021fe4 <_GpProcTmr+48>:      mov     lr, pc
0xc021fe8 <_GpProcTmr+52>:      mov     pc, r3
0xc021fec <_GpProcTmr+56>:      ldr     r0, [sp]
0xc021ff0 <_GpProcTmr+60>:      bl      0xc021c9c <GpThreadSleep>
0xc021ff4 <_GpProcTmr+64>:      ldmia   sp!, {r0, r1, r2, lr}
0xc021ff8 <_GpProcTmr+68>:      b       0xc021fd4 <_GpProcTmr+32>
0xc021ffc <_GpProcTmr+72>:      mrs     r5, CPSR
0xc022000 <_GpProcTmr+76>:      orr     r5, r5, #192    ; 0xc0
0xc022004 <_GpProcTmr+80>:      msr     CPSR_fsxc, r5
0xc022008 <_GpProcTmr+84>:      stmdb       sp!, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12}
0xc02200c <_GpProcTmr+88>:      bl      0xc0231ec <GpTimerKill>
0xc022010 <_GpProcTmr+92>:      b       0xc022010 <_GpProcTmr+92>
0xc022014 <_GpProcTmr+96>:      stceq   12, cr6, [sp], -#1008
0xc022018 <_GpProcTmr+100>:     stceq   13, cr6, [sp]
0xc02201c <_GpProcTmr+104>:     stceq   13, cr6, [sp], -#48
0xc022020 <_GpProcTmr+108>:     stceq   13, cr6, [sp], -#16
0xc022024 <_GpProcTmr+112>:     stceq   6, cr7, [sp], -#768
0xc022028 <_GpProcTmr+116>:     stceq   8, cr7, [sp], -#384
*/

/*******************************************************************************************
 *                                                                                         *
 * GpTimerOptSet(int idx,int tmr_tps,int max_exec_tick, void (*irq_tmrfunc(void))          *
 *                                                                                         *
 *  1. idx : timer index for interrupt handler                                             *
 *  2. tmr_tps : tick per second (frequency of interrupt)                                  *
 *  3. max_exec_tick                                                                       *
 *  4. irq_tmrfunc : timer interrupt handler (MUST NOT be NULL)                            *
 *                                                                                         *
 *******************************************************************************************/
void PROFILER_START()
{
	unsigned int	*pISRAddress = (unsigned int *)(0x0c7ac028);
	void (*pISR)();


	// Set up timer interrupt.
	g_uiNextSample = 0;
	if (g_pSamples  == NULL) {
		g_pSamples = (PCSample *)gm_malloc(PROF_SEC * sizeof(PCSample) * PROF_FREQ);

	}

	if (GPOS_ERR_ALREADY_USED == GpTimerOptSet(0, PROF_FREQ, 0, ProfilerISR))    GpTimerKill(0);
	GpTimerSet(0);
	g_bProfilerRunning = true;

	// Get GP's damn ISR address, we need it to tap into the timer interrupts.
	pISR = ProfilerISR2;
	g_uiGPISR = *pISRAddress;
	*pISRAddress = (unsigned int)pISR;



}

//***************************************************************************//
//***************************************************************************//

void PROFILER_STOP()
{
	if (g_bProfilerRunning) {
		g_bProfilerRunning = false;
		GpTimerKill(0);
	}
}

//***************************************************************************//
//***************************************************************************//

void PROFILER_DUMP()
{
	F_HANDLE 			h_file;
	ERR_CODE 			err = SM_OK;
	unsigned long		iFileSize;
	unsigned char		*pFileBuffer;
	char				*pSubString;
	unsigned long		iBytesRead;
	
#ifdef DEBUGGER_PRESENT
	DEBUG_Print("PROFILER DUMP\n");
	DEBUG_Print("Collected: [%d] samples\n", g_uiNextSample);
	for (int i = 0; i < g_uiNextSample; i++) {
		DEBUG_Print("AP!Sample:[%d] - [0x%x] <= [0x%x]\n", i, g_pSamples[i].PC, g_pSamples[i].LR);
	}
#endif



	err = GpRelativePathSet("gp:\\GPMM\\");
	if (!err) err = GpFileCreate("Prof.dat", ALWAYS_CREATE, &h_file);
	if (!err) {
		GpFileWrite(h_file, &g_uiNextSample, 4);
		GpFileWrite(h_file, g_pSamples, g_uiNextSample * sizeof(PCSample));

	}
	GpFileClose(h_file);

}


#ifdef __cplusplus
} // extern "C"
#endif
