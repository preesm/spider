/*
 * lrt_core.c
 *
 *  Created on: 31 oct. 2012
 *      Author: yoliva
 */

#include "lrt_definitions.h"
#include "lrt_cfg.h"
#include "xparameters.h"
#include "lrt_prototypes.h"





/*$PAGE*/
/*
*********************************************************************************************************
*                                         DEFINITIONS
*********************************************************************************************************
*/




/*$PAGE*/
/*
*********************************************************************************************************
*                                         GLOBAL VARIABLES
*********************************************************************************************************
*/
BOOLEAN 		OSRunning;                      			/* Flag indicating that kernel is running   */
INT8U  			OSLockNesting;								/* Multitasking lock nesting level          */

INT8U           OSPrioCur;                					// Priority of current task
INT8U           OSPrioHighRdy;            					// Priority of highest priority task

INT8U	        OSRdyGrp;                        			// Ready list group
INT8U			OSRdyTbl[OS_RDY_TBL_SIZE];       			// Table of tasks which are ready to run

OS_TCB          *OSTCBCur;                        			// Pointer to currently running TCB
OS_TCB          *OSTCBFreeList;                   			// Pointer to list of free TCBs
OS_TCB          *OSTCBHighRdy;                    			// Pointer to highest priority TCB R-to-R
OS_TCB          *OSTCBList;                       			// Pointer to doubly linked list of TCBs
OS_TCB          *OSTCBPrioTbl[OS_LOWEST_PRIO + 1u];    		// Table of pointers to created TCBs
OS_TCB          OSTCBTbl[OS_MAX_TASKS + OS_N_SYS_TASKS];	// Table of TCBs

/*
*********************************************************************************************************
*                                       PRIORITY RESOLUTION TABLE
*
* Note: Index into table is bit pattern to resolve highest priority
*       Indexed value corresponds to highest priority bit position (i.e. 0..7)
*********************************************************************************************************
*/

INT8U  const  OSUnMapTbl[256] = {
    0u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x00 to 0x0F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x10 to 0x1F                   */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x20 to 0x2F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x30 to 0x3F                   */
    6u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x40 to 0x4F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x50 to 0x5F                   */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x60 to 0x6F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x70 to 0x7F                   */
    7u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x80 to 0x8F                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0x90 to 0x9F                   */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xA0 to 0xAF                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xB0 to 0xBF                   */
    6u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xC0 to 0xCF                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xD0 to 0xDF                   */
    5u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, /* 0xE0 to 0xEF                   */
    4u, 0u, 1u, 0u, 2u, 0u, 1u, 0u, 3u, 0u, 1u, 0u, 2u, 0u, 1u, 0u  /* 0xF0 to 0xFF                   */
};


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CLEAR A SECTION OF MEMORY
*
* Description: This function is called by other uC/OS-II services to clear a contiguous block of RAM.
*
* Arguments  : pdest    is the start of the RAM to clear (i.e. write 0x00 to)
*
*              size     is the number of bytes to clear.
*
* Returns    : none
*
* Notes      : 1) This function is INTERNAL to uC/OS-II and your application should not call it.
*              2) Note that we can only clear up to 64K bytes of RAM.  This is not an issue because none
*                 of the uses of this function gets close to this limit.
*              3) The clear is done one byte at a time since this will work on any processor irrespective
*                 of the alignment of the destination.
*********************************************************************************************************
*/

void  OS_MemClr (INT8U  *pdest, INT16U  size)
{
    while (size > 0u) {
        *pdest++ = (INT8U)0;
        size--;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                                    INITIALIZE MISCELLANEOUS VARIABLES
*
* Description: This function is called by OSInit() to initialize miscellaneous variables.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

static  void  OS_InitMisc (void)
{
#if OS_TIME_GET_SET_EN > 0u
    OSTime                    = 0uL;                       /* Clear the 32-bit system clock            */
#endif

//    OSIntNesting              = 0u;                        /* Clear the interrupt nesting counter      */
    OSLockNesting             = 0u;                        /* Clear the scheduling lock counter        */

//    OSTaskCtr                 = 0u;                        /* Clear the number of tasks                */

    OSRunning                 = false;                  /* Indicate that multitasking not started   */

//    OSCtxSwCtr                = 0u;                        /* Clear the context switch counter         */
//    OSIdleCtr                 = 0uL;                       /* Clear the 32-bit idle counter            */

#if OS_TASK_STAT_EN > 0u
    OSIdleCtrRun              = 0uL;
    OSIdleCtrMax              = 0uL;
    OSStatRdy                 = OS_FALSE;                  /* Statistic task is not ready              */
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508
    OSSafetyCriticalStartFlag = OS_FALSE;                  /* Still allow creation of objects          */
#endif
}








/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                                       INITIALIZE THE READY LIST
*
* Description: This function is called by OSInit() to initialize the Ready List.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

static  void  OS_InitRdyList (void)
{
    INT8U  i;

    OSRdyGrp      = 0u;                                    /* Clear the ready list                     */
    for (i = 0u; i < OS_RDY_TBL_SIZE; i++) {
        OSRdyTbl[i] = 0u;
    }

    OSPrioCur     = 0u;
    OSPrioHighRdy = 0u;

    OSTCBHighRdy  = (OS_TCB *)0;
    OSTCBCur      = (OS_TCB *)0;
}




/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*                            INITIALIZE THE FREE LIST OF TASK CONTROL BLOCKS
*
* Description: This function is called by OSInit() to initialize the free list of OS_TCBs.
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/

void  OS_InitTCBList (void)
{
    INT8U    ix;
    INT8U    ix_next;
    OS_TCB  *ptcb1;
    OS_TCB  *ptcb2;

    OS_MemClr((INT8U *)&OSTCBTbl[0],     sizeof(OSTCBTbl));      /* Clear all the TCBs                 */
    OS_MemClr((INT8U *)&OSTCBPrioTbl[0], sizeof(OSTCBPrioTbl));  /* Clear the priority table           */
    for (ix = 0u; ix < (OS_MAX_TASKS + OS_N_SYS_TASKS - 1u); ix++) {    /* Init. list of free TCBs     */
        ix_next =  ix + 1u;
        ptcb1   = &OSTCBTbl[ix];
        ptcb2   = &OSTCBTbl[ix_next];
        ptcb1->OSTCBNext = ptcb2;
#if OS_TASK_NAME_EN > 0u
        ptcb1->OSTCBTaskName = (INT8U *)(void *)"?";             /* Unknown name                       */
#endif
    }
    ptcb1                   = &OSTCBTbl[ix];
    ptcb1->OSTCBNext        = (OS_TCB *)0;                       /* Last OS_TCB                        */
#if OS_TASK_NAME_EN > 0u
    ptcb1->OSTCBTaskName    = (INT8U *)(void *)"?";              /* Unknown name                       */
#endif
    OSTCBList               = (OS_TCB *)0;                       /* TCB lists initializations          */
    OSTCBFreeList           = &OSTCBTbl[0];
}









/*$PAGE*/
/*
*********************************************************************************************************
*                                    INITIALIZE MEMORY PARTITION MANAGER
*
* Description : This function is called by uC/OS-II to initialize the memory partition manager.  Your
*               application MUST NOT call this function.
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)    : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/

void  OS_MemInit (void)
{
#if OS_MAX_MEM_PART == 1u
    OS_MemClr((INT8U *)&OSMemTbl[0], sizeof(OSMemTbl));   /* Clear the memory partition table          */
//    OSMemFreeList               = (OS_MEM *)&OSMemTbl[0]; /* Point to beginning of free list           */
#if OS_MEM_NAME_EN > 0u
    OSMemFreeList->OSMemName    = (INT8U *)"?";           /* Unknown name                              */
#endif
#endif

#if OS_MAX_MEM_PART >= 2u
    OS_MEM  *pmem;
    INT16U   i;


    OS_MemClr((INT8U *)&OSMemTbl[0], sizeof(OSMemTbl));   /* Clear the memory partition table          */
    for (i = 0u; i < (OS_MAX_MEM_PART - 1u); i++) {       /* Init. list of free memory partitions      */
        pmem                = &OSMemTbl[i];               /* Point to memory control block (MCB)       */
        pmem->OSMemFreeList = (void *)&OSMemTbl[i + 1u];  /* Chain list of free partitions             */
#if OS_MEM_NAME_EN > 0u
        pmem->OSMemName  = (INT8U *)(void *)"?";
#endif
    }
    pmem                = &OSMemTbl[i];
    pmem->OSMemFreeList = (void *)0;                      /* Initialize last node                      */
#if OS_MEM_NAME_EN > 0u
    pmem->OSMemName = (INT8U *)(void *)"?";
#endif

    OSMemFreeList   = &OSMemTbl[0];                       /* Point to beginning of free list           */
#endif
    OSMemTbl[0].MemBaseAddr = (void* )XPAR_MB_SHARED_BRAM_CTRL_0_S_AXI_BASEADDR;
    OSMemTbl[0].DataBaseAddr = (INT32U* )(XPAR_MB_SHARED_BRAM_CTRL_0_S_AXI_BASEADDR) + SH_MEM_DATA_OFFSET;
    OSMemTbl[0].OSMemBlkSize = XPAR_MB_SHARED_BRAM_CTRL_0_S_AXI_HIGHADDR - (INT32U)OSMemTbl[0].DataBaseAddr;

    *((INT32U*)OSMemTbl[0].MemBaseAddr + SH_MEM_WR_IX_OFFSET) = 0;
    *((INT32U*)OSMemTbl[0].MemBaseAddr + SH_MEM_RD_IX_OFFSET) = 0;
}






/*$PAGE*/
/*
*********************************************************************************************************
*                                             INITIALIZATION
*
* Description: This function is used to initialize the internals of uC/OS-II and MUST be called prior to
*              creating any uC/OS-II object and, prior to calling OSStart().
*
* Arguments  : none
*
* Returns    : none
*********************************************************************************************************
*/
void  OSInit (void)
{
//    OSInitHookBegin();                                           /* Call port specific initialization code   */

    OS_InitMisc();                                               /* Initialize miscellaneous variables       */

    OS_InitRdyList();                                            /* Initialize the Ready List                */

    OS_InitTCBList();                                            /* Initialize the free list of OS_TCBs      */

//    OS_InitEventList();                                          /* Initialize the free list of OS_EVENTs    */

#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
    OS_FlagInit();                                               /* Initialize the event flag structures     */
#endif

//#if (OS_MEM_EN > 0u) && (OS_MAX_MEM_PART > 0u)
#if (OS_MAX_MEM_PART > 0u)
    OS_MemInit();                                                /* Initialize the memory manager            */
#endif

#if (OS_Q_EN > 0u) && (OS_MAX_QS > 0u)
    OS_QInit();                                                  /* Initialize the message queue structures  */
#endif

//    OS_InitTaskIdle();                                           /* Create the Idle Task                     */
#if OS_TASK_STAT_EN > 0u
    OS_InitTaskStat();                                           /* Create the Statistic Task                */
#endif

#if OS_TMR_EN > 0u
    OSTmr_Init();                                                /* Initialize the Timer Manager             */
#endif

//    OSInitHookEnd();                                             /* Call port specific init. code            */

#if OS_DEBUG_EN > 0u
//    OSDebugInit();
#endif


}







void init_lrt()
{
#if OS_DEBUG_EN > 0
	print("Initializing microblaze ");
	putnum(XPAR_CPU_ID);
	print("\n");
#endif

    OSInit();

    while(true)
    	wait_for_ext_msg();
}












/*
*********************************************************************************************************
*                              FIND HIGHEST PRIORITY TASK READY TO RUN
*
* Description: This function determines the highest priority task that is ready to run.
* The global variable 'OSPrioHighRdy' is changed accordingly.
*
* Arguments  : none
*
* Returns    : none
*
* Notes      : 1) This function is INTERNAL to uC/OS-II and your application should not call it.
*              2) Interrupts are assumed to be disabled when this function is called.
*********************************************************************************************************
*/

void  OS_SchedNew (void)
{
#if OS_LOWEST_PRIO <= 63u                        /* See if we support up to 64 tasks                   */
    INT8U   y;


    y             = OSUnMapTbl[OSRdyGrp];
    OSPrioHighRdy = (INT8U)((y << 3u) + OSUnMapTbl[OSRdyTbl[y]]);
#else                                            /* We support up to 256 tasks                         */
    INT8U     y;
    OS_PRIO  *ptbl;


    if ((OSRdyGrp & 0xFFu) != 0u) {
        y = OSUnMapTbl[OSRdyGrp & 0xFFu];
    } else {
        y = OSUnMapTbl[(OS_PRIO)(OSRdyGrp >> 8u) & 0xFFu] + 8u;
    }
    ptbl = &OSRdyTbl[y];
    if ((*ptbl & 0xFFu) != 0u) {
        OSPrioHighRdy = (INT8U)((y << 4u) + OSUnMapTbl[(*ptbl & 0xFFu)]);
    } else {
        OSPrioHighRdy = (INT8U)((y << 4u) + OSUnMapTbl[(OS_PRIO)(*ptbl >> 8u) & 0xFFu] + 8u);
    }
#endif
}










/*$PAGE*/
/*
*********************************************************************************************************
*                                            OSStartHighRdy()
*
* Description: Starts the highest priority task that is available to run.  OSStartHighRdy() MUST:
*
*********************************************************************************************************
*/

void OSStartHighRdy()
{
//	if (check_fifos)
//	{
//	read_inputs
	OSTCBHighRdy->task_func();
//	write_outputs
//	}
}





/*
 * Function : schedule
 */
void OS_Sched()
{
    if (OSLockNesting == 0u) {                     /* ... scheduler is not locked                  */
        OS_SchedNew();
        OSTCBHighRdy = OSTCBPrioTbl[OSPrioHighRdy];
//        if (OSPrioHighRdy != OSPrioCur) {          /* No Ctx Sw if current task is highest rdy     */
//            OSCtxSwCtr++;                          /* Increment context switch counter             */
//            OS_TASK_SW();                          /* Perform a context switch                     */
//        }
        OSStartHighRdy();
    }
}


