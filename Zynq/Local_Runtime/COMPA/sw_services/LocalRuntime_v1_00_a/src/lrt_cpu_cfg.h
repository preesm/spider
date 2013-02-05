

#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT  extern
#endif

//#include "xparameters.h"
//#include <xintc_l.h>


/*
*********************************************************************************************************
*                                    CRITICAL SECTIONS MANAGEMENT
*
* Method #1:  Disable/Enable interrupts using simple instructions.  After critical section, interrupts
*             will be enabled even if they were disabled before entering the critical section.
*
* Method #2:  Disable/Enable interrupts by preserving the state of interrupts.  In other words, if
*             interrupts were disabled before entering the critical section, they will be disabled when
*             leaving the critical section.
*
* Method #3:  Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
*             would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
*             disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
*             disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
*             into the CPU's status register.
*********************************************************************************************************
*/

//#define  OS_CRITICAL_METHOD    3
//
//#define  OS_ENTER_CRITICAL()	cpu_sr = OS_CPU_SR_Save();
//#define  OS_EXIT_CRITICAL()		OS_CPU_SR_Restore(cpu_sr);

/*
*********************************************************************************************************
*                                           Architecture
*********************************************************************************************************
*/
#define PROCESSOR_COMM		0	// Inter-processors communication : 0 - by Mailbox	value - Shared mem.



/*
*********************************************************************************************************
*                                           Xilinx Microblaze
*********************************************************************************************************
*/

//#define  OS_STK_GROWTH        1            /* Stack grows from HIGH to LOW memory on Xilinx Microblaze */

//#define  OS_TASK_SW()         OSCtxSw()    /* See OS_CPU_A.S                                           */

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

//OS_CPU_SR	OS_CPU_SR_Save(void);          /* See OS_CPU_A.S                                           */
//void		OS_CPU_SR_Restore(OS_CPU_SR);     /* See OS_CPU_A.S                                           */
//
//void        OS_CPU_ISR(void);              /* See OS_CPU_A.S                                           */



