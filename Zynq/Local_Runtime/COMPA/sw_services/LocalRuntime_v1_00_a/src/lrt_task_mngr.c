/********************************************************************************
 * File    : task_manager.c
 * By      : Yaset Oliva Venegas
 * Version : 1.0
 * Date	   : October 2012
 * Descrip : This file contains the implementation of the RunTime's functions.
 * 			For functions's descriptions refer to the "prototypes.h" file.
*********************************************************************************/

#include "lrt_definitions.h"
#include "lrt_cfg.h"
#include "lrt_prototypes.h"
//#ifdef OS_DEBUG_EN > 0
//	#include <stdio.h>
//#endif

/*
*********************************************************************************************************
*                                         GLOBAL VARIABLES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         LOCAL MACROS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         EXTERNAL FUNCTIONS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         GLOBAL DECLARATIONS
*********************************************************************************************************
*/

INT8U             OSTaskCtr;                      			// Number of tasks created




/*$PAGE*/
/*
*********************************************************************************************************
*                                        INITIALIZE A TASK'S STACK
*
* Description: This function is called by either OSTaskCreate() or OSTaskCreateExt() to initialize the
*              stack frame of the task being created.  This function is highly processor specific.
*
* Arguments  : task          is a pointer to the task code
*
*              p_arg         is a pointer to a user supplied data area that will be passed to the task
*                            when the task first executes.
*
*              ptos          is a pointer to the top of stack.  It is assumed that 'ptos' points to
*                            a 'free' entry on the task stack.  If OS_STK_GROWTH is set to 1 then
*                            'ptos' will contain the HIGHEST valid address of the stack.  Similarly, if
*                            OS_STK_GROWTH is set to 0, the 'ptos' will contains the LOWEST valid address
*                            of the stack.
*
*              opt           specifies options that can be used to alter the behavior of OSTaskStkInit().
*                            (see uCOS_II.H for OS_TASK_OPT_???).
*
* Returns    : Always returns the location of the new top-of-stack' once the processor registers have
*              been placed on the stack in the proper order.
*
* Note(s)    : 1) Interrupts are enabled when your task starts executing.
*
*                 OSTCBHighRdy->OSTCBStkPtr + 0x00     RMSR   (IE=1)       (LOW Memory)
*                                           + 0x04     R2
*                                           + 0x08     R3
*                                           + 0x0C     R4
*                                           + 0x10     R5     (p_arg passed to task)
*                                           + 0x14     R6
*                                           + 0x18     R7
*                                           + 0x1C     R8
*                                           + 0x20     R9
*                                           + 0x24     R10
*                                           + 0x28     R11
*                                           + 0x2C     R12
*                                           + 0x30     R13
*                                           + 0x34     R14
*                                           + 0x38     R15
*                                           + 0x3C     R17
*                                           + 0x40     R18
*                                           + 0x44     R19
*                                           + 0x48     R20
*                                           + 0x4C     R21
*                                           + 0x50     R22
*                                           + 0x54     R23
*                                           + 0x58     R24
*                                           + 0x5C     R25
*                                           + 0x60     R26
*                                           + 0x64     R27
*                                           + 0x68     R28
*                                           + 0x6C     R29
*                                           + 0x70     R30
*                                           + 0x74     R31                (HIGH Memory)
*                                           + 0x78     Empty
*                 ptos --------->           + 0x7C     Empty
*
*              2) R16 is not saved as part of the task context since it is used by the debugger.
*********************************************************************************************************
*/
/*$PAGE*/

OS_STK  *OSTaskStkInit (void (*task)(void *pd), void *p_arg, OS_STK *ptos, INT16U opt)
{
    INT32U  *pstk;


    opt     = opt;                    /* 'opt' is not used, prevent warning                            */
    pstk    = (INT32U *)ptos;         /* Load stack pointer                                            */
    pstk--;                           /* Make sure we point to free entry ...                          */
    pstk--;                           /* ... compiler uses top-of-stack so free an extra one.          */
    *pstk-- = (INT32U)0x31313131;     /* R31                                                           */
    *pstk-- = (INT32U)0x30303030;     /* R30                                                           */
    *pstk-- = (INT32U)0x29292929;     /* R29                                                           */
    *pstk-- = (INT32U)0x28282828;     /* R28                                                           */
    *pstk-- = (INT32U)0x27272727;     /* R27                                                           */
    *pstk-- = (INT32U)0x26262626;     /* R26                                                           */
    *pstk-- = (INT32U)0x25252525;     /* R25                                                           */
    *pstk-- = (INT32U)0x24242424;     /* R24                                                           */
    *pstk-- = (INT32U)0x23232323;     /* R23                                                           */
    *pstk-- = (INT32U)0x22222222;     /* R22                                                           */
    *pstk-- = (INT32U)0x21212121;     /* R21                                                           */
    *pstk-- = (INT32U)0x20202020;     /* R20                                                           */
    *pstk-- = (INT32U)0x19191919;     /* R19                                                           */
    *pstk-- = (INT32U)0x18181818;     /* R18                                                           */
    *pstk-- = (INT32U)0x17171717;     /* R17                                                           */
    *pstk-- = (INT32U)task - 8;       /* R15 = task return address (assuming function call)            */
    *pstk-- = (INT32U)task;           /* R14 = task (Interrupt return address)                         */
//    *pstk-- = (INT32U)&_SDA_BASE_;    /* R13                                                           */
    pstk--;
    *pstk-- = (INT32U)0x12121212;     /* R12                                                           */
    *pstk-- = (INT32U)0x11111111;     /* R11                                                           */
    *pstk-- = (INT32U)0x10101010;     /* R10                                                           */
    *pstk-- = (INT32U)0x09090909;     /* R09                                                           */
    *pstk-- = (INT32U)0x08080808;     /* R08                                                           */
    *pstk-- = (INT32U)0x07070707;     /* R07                                                           */
    *pstk-- = (INT32U)0x06060606;     /* R06                                                           */
    *pstk-- = (INT32U)p_arg;          /* R05                                                           */
    *pstk-- = (INT32U)0x04040404;     /* R04                                                           */
    *pstk-- = (INT32U)0x03030303;     /* R03                                                           */
//    *pstk-- = (INT32U)&_SDA2_BASE_;   /* R02                                                           */
    pstk--;
    *pstk   = (INT32U)0x00000002;     /* MSR with interrupts enabled                                   */

    return ((OS_STK *)pstk);          /* Return new top of stack                                       */
}




/*$PAGE*/
/*
*********************************************************************************************************
*                                            INITIALIZE TCB
*
* Description: This function is internal to uC/OS-II and is used to initialize a Task Control Block when
*              a task is created (see OSTaskCreate() and OSTaskCreateExt()).
*
* Arguments  : prio          is the priority of the task being created
*
*              ptos          is a pointer to the task's top-of-stack assuming that the CPU registers
*                            have been placed on the stack.  Note that the top-of-stack corresponds to a
*                            'high' memory location is OS_STK_GROWTH is set to 1 and a 'low' memory
*                            location if OS_STK_GROWTH is set to 0.  Note that stack growth is CPU
*                            specific.
*
*              pbos          is a pointer to the bottom of stack.  A NULL pointer is passed if called by
*                            'OSTaskCreate()'.
*
*              id            is the task's ID (0..65535)
*
*              stk_size      is the size of the stack (in 'stack units').  If the stack units are INT8Us
*                            then, 'stk_size' contains the number of bytes for the stack.  If the stack
*                            units are INT32Us then, the stack contains '4 * stk_size' bytes.  The stack
*                            units are established by the #define constant OS_STK which is CPU
*                            specific.  'stk_size' is 0 if called by 'OSTaskCreate()'.
*
*              pext          is a pointer to a user supplied memory area that is used to extend the task
*                            control block.  This allows you to store the contents of floating-point
*                            registers, MMU registers or anything else you could find useful during a
*                            context switch.  You can even assign a name to each task and store this name
*                            in this TCB extension.  A NULL pointer is passed if called by OSTaskCreate().
*
*              opt           options as passed to 'OSTaskCreateExt()' or,
*                            0 if called from 'OSTaskCreate()'.
*
* Returns    : OS_ERR_NONE         if the call was successful
*              OS_ERR_TASK_NO_MORE_TCB  if there are no more free TCBs to be allocated and thus, the task cannot
*                                  be created.
*
* Note       : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/

INT8U  OS_TCBInit (INT8U    prio,
                   OS_STK  *ptos,
                   OS_STK  *pbos,
                   INT16U   id,
                   INT32U   stk_size,
                   void    *pext,
                   INT16U   opt)
{
    OS_TCB    *ptcb;
#if OS_CRITICAL_METHOD == 3u                               /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr = 0u;
#endif
#if OS_TASK_REG_TBL_SIZE > 0u
    INT8U      i;
#endif


//    OS_ENTER_CRITICAL();
    ptcb = OSTCBFreeList;                                  /* Get a free TCB from the free TCB list    */
    if (ptcb != (OS_TCB *)0) {
        OSTCBFreeList            = ptcb->OSTCBNext;        /* Update pointer to free TCB list          */
//        OS_EXIT_CRITICAL();
        ptcb->OSTCBStkPtr        = ptos;                   /* Load Stack pointer in TCB                */
        ptcb->OSTCBPrio          = prio;                   /* Load task priority into TCB              */
        ptcb->OSTCBStat          = OS_STAT_RDY;            /* Task is ready to run                     */
        ptcb->OSTCBStatPend      = OS_STAT_PEND_OK;        /* Clear pend status                        */
        ptcb->OSTCBDly           = 0u;                     /* Task is not delayed                      */

#if OS_TASK_CREATE_EXT_EN > 0u
        ptcb->OSTCBExtPtr        = pext;                   /* Store pointer to TCB extension           */
        ptcb->OSTCBStkSize       = stk_size;               /* Store stack size                         */
        ptcb->OSTCBStkBottom     = pbos;                   /* Store pointer to bottom of stack         */
        ptcb->OSTCBOpt           = opt;                    /* Store task options                       */
        ptcb->OSTCBId            = id;                     /* Store task ID                            */
#else
        pext                     = pext;                   /* Prevent compiler warning if not used     */
        stk_size                 = stk_size;
        pbos                     = pbos;
        opt                      = opt;
        id                       = id;
#endif

#if OS_TASK_DEL_EN > 0u
        ptcb->OSTCBDelReq        = OS_ERR_NONE;
#endif

#if OS_LOWEST_PRIO <= 63u                                         /* Pre-compute X, Y                  */
        ptcb->OSTCBY             = (INT8U)(prio >> 3u);
        ptcb->OSTCBX             = (INT8U)(prio & 0x07u);
#else                                                             /* Pre-compute X, Y                  */
        ptcb->OSTCBY             = (INT8U)((INT8U)(prio >> 4u) & 0xFFu);
        ptcb->OSTCBX             = (INT8U) (prio & 0x0Fu);
#endif
                                                                  /* Pre-compute BitX and BitY         */
        ptcb->OSTCBBitY          = (OS_PRIO)(1uL << ptcb->OSTCBY);
        ptcb->OSTCBBitX          = (OS_PRIO)(1uL << ptcb->OSTCBX);

#if (OS_EVENT_EN)
        ptcb->OSTCBEventPtr      = (OS_EVENT  *)0;         /* Task is not pending on an  event         */
#if (OS_EVENT_MULTI_EN > 0u)
        ptcb->OSTCBEventMultiPtr = (OS_EVENT **)0;         /* Task is not pending on any events        */
#endif
#endif

#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u) && (OS_TASK_DEL_EN > 0u)
        ptcb->OSTCBFlagNode  = (OS_FLAG_NODE *)0;          /* Task is not pending on an event flag     */
#endif

#if (OS_MBOX_EN > 0u) || ((OS_Q_EN > 0u) && (OS_MAX_QS > 0u))
        ptcb->OSTCBMsg       = (void *)0;                  /* No message received                      */
#endif

#if OS_TASK_PROFILE_EN > 0u
        ptcb->OSTCBCtxSwCtr    = 0uL;                      /* Initialize profiling variables           */
        ptcb->OSTCBCyclesStart = 0uL;
        ptcb->OSTCBCyclesTot   = 0uL;
        ptcb->OSTCBStkBase     = (OS_STK *)0;
        ptcb->OSTCBStkUsed     = 0uL;
#endif

#if OS_TASK_NAME_EN > 0u
        ptcb->OSTCBTaskName    = (INT8U *)(void *)"?";
#endif

#if OS_TASK_REG_TBL_SIZE > 0u                              /* Initialize the task variables            */
        for (i = 0u; i < OS_TASK_REG_TBL_SIZE; i++) {
            ptcb->OSTCBRegTbl[i] = 0u;
        }
#endif

//        OSTCBInitHook(ptcb);

//        OSTaskCreateHook(ptcb);                            /* Call user defined hook                   */

//        OS_ENTER_CRITICAL();
        OSTCBPrioTbl[prio] = ptcb;
        ptcb->OSTCBNext    = OSTCBList;                    /* Link into TCB chain                      */
        ptcb->OSTCBPrev    = (OS_TCB *)0;
        if (OSTCBList == (OS_TCB *)0)
        {
        	OSTCBFirst 	= ptcb;
        	OSTCBCur	= ptcb;
        }
        else
        {
            OSTCBList->OSTCBPrev = ptcb;
        }
        OSTCBList               = ptcb;
        OSRdyGrp               |= ptcb->OSTCBBitY;         /* Make task ready to run                   */
        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
        OSTaskCtr++;                                       /* Increment the #tasks counter             */
//        OS_EXIT_CRITICAL();
        return (OS_ERR_NONE);
    }
//    OS_EXIT_CRITICAL();
    return (OS_ERR_TASK_NO_MORE_TCB);
}





/*
*********************************************************************************************************
*                                     CREATE A TASK (Extended Version)
*
* Description: This function is used to have uC/OS-II manage the execution of a task.  Tasks can either
*              be created prior to the start of multitasking or by a running task.  A task cannot be
*              created by an ISR.  This function is similar to OSTaskCreate() except that it allows
*              additional information about a task to be specified.
*
* Arguments  : task      is a pointer to the task's code
*
*              p_arg     is a pointer to an optional data area which can be used to pass parameters to
*                        the task when the task first executes.  Where the task is concerned it thinks
*                        it was invoked and passed the argument 'p_arg' as follows:
*
*                            void Task (void *p_arg)
*                            {
*                                for (;;) {
*                                    Task code;
*                                }
*                            }
*
*              ptos      is a pointer to the task's top of stack.  If the configuration constant
*                        OS_STK_GROWTH is set to 1, the stack is assumed to grow downward (i.e. from high
*                        memory to low memory).  'ptos' will thus point to the highest (valid) memory
*                        location of the stack.  If OS_STK_GROWTH is set to 0, 'ptos' will point to the
*                        lowest memory location of the stack and the stack will grow with increasing
*                        memory locations.  'ptos' MUST point to a valid 'free' data item.
*
*              prio      is the task's priority.  A unique priority MUST be assigned to each task and the
*                        lower the number, the higher the priority.
*
*              id        is the task's ID (0..65535)
*
*              pbos      is a pointer to the task's bottom of stack.  If the configuration constant
*                        OS_STK_GROWTH is set to 1, the stack is assumed to grow downward (i.e. from high
*                        memory to low memory).  'pbos' will thus point to the LOWEST (valid) memory
*                        location of the stack.  If OS_STK_GROWTH is set to 0, 'pbos' will point to the
*                        HIGHEST memory location of the stack and the stack will grow with increasing
*                        memory locations.  'pbos' MUST point to a valid 'free' data item.
*
*              stk_size  is the size of the stack in number of elements.  If OS_STK is set to INT8U,
*                        'stk_size' corresponds to the number of bytes available.  If OS_STK is set to
*                        INT16U, 'stk_size' contains the number of 16-bit entries available.  Finally, if
*                        OS_STK is set to INT32U, 'stk_size' contains the number of 32-bit entries
*                        available on the stack.
*
*              fifo_in 	 is the identifier of the input FIFO.
*
*              fifo_out  is the identifier of the input FIFO.
*
*              pext      is a pointer to a user supplied memory location which is used as a TCB extension.
*                        For example, this user memory can hold the contents of floating-point registers
*                        during a context switch, the time each task takes to execute, the number of times
*                        the task has been switched-in, etc.
*
*              opt       contains additional information (or options) about the behavior of the task.  The
*                        LOWER 8-bits are reserved by uC/OS-II while the upper 8 bits can be application
*                        specific.  See OS_TASK_OPT_??? in uCOS-II.H.  Current choices are:
*
*                        OS_TASK_OPT_STK_CHK      Stack checking to be allowed for the task
*                        OS_TASK_OPT_STK_CLR      Clear the stack when the task is created
*                        OS_TASK_OPT_SAVE_FP      If the CPU has floating-point registers, save them
*                                                 during a context switch.
*
* Returns    : OS_ERR_NONE             if the function was successful.
*              OS_PRIO_EXIT            if the task priority already exist
*                                      (each task MUST have a unique priority).
*              OS_ERR_PRIO_INVALID     if the priority you specify is higher that the maximum allowed
*                                      (i.e. > OS_LOWEST_PRIO)
*              OS_ERR_TASK_CREATE_ISR  if you tried to create a task from an ISR.
*********************************************************************************************************
*/

INT8U  OSTaskCreateExt (FUNCTION_TYPE task,
                        void    *p_arg,
                        OS_STK  *ptos,
                        INT8U    prio,
                        INT16U   id,
                        OS_STK  *pbos,
                        INT32U   stk_size,
                        INT16U	nb_fifo_in,
                        INT16U	nb_fifo_out,
                        INT16U	*fifo_in,
                        INT16U	*fifo_out,
                        void    *pext,
                        INT16U   opt)
{
//    OS_STK    *psp;
    INT8U      err;
//#if OS_CRITICAL_METHOD == 3u                 /* Allocate storage for CPU status register               */
//    OS_CPU_SR  cpu_sr = 0u;
//#endif



#if OS_ARG_CHK_EN > 0u
    if (prio > OS_LOWEST_PRIO) {             /* Make sure priority is within allowable range           */
        return (OS_ERR_PRIO_INVALID);
    }
#endif
//    OS_ENTER_CRITICAL();
//    if (OSIntNesting > 0u) {                 /* Make sure we don't create the task from within an ISR  */
//        OS_EXIT_CRITICAL();
//        return (OS_ERR_TASK_CREATE_ISR);
//    }
    if (OSTCBPrioTbl[prio] == (OS_TCB *)0) { /* Make sure task doesn't already exist at this priority  */
        OSTCBPrioTbl[prio] = OS_TCB_RESERVED;/* Reserve the priority to prevent others from doing ...  */
                                             /* ... the same thing until task is created.              */
//        OS_EXIT_CRITICAL();

//#if (OS_TASK_STAT_STK_CHK_EN > 0u)
//        OS_TaskStkClr(pbos, stk_size, opt);                    /* Clear the task stack (if needed)     */
//#endif

//        psp = OSTaskStkInit(task, p_arg, ptos, opt);           /* Initialize the task's stack          */
        err = OS_TCBInit(prio, ptos, pbos, id, stk_size, pext, opt);

//        err = OS_TCBInit(prio, ptos, pbos, id, stk_size, pext, opt);
        if (err == OS_ERR_NONE) {
        	OSTCBPrioTbl[prio]->task_func = (FUNCTION_TYPE)task;

        	if(nb_fifo_in)
        	{
            	INT16U i;
            	for(i=0;i<nb_fifo_in;i++)
            		OSTCBPrioTbl[prio]->fifo_in[i] = get_fifo_hndl(fifo_in[i], &err);
        	}

        	if(nb_fifo_out)
        	{
            	INT16U j;
            	for(j=0;j<nb_fifo_in;j++)
            		OSTCBPrioTbl[prio]->fifo_out[j] = get_fifo_hndl(fifo_in[j], &err);
        	}

//            if (OSRunning == OS_TRUE) {                        /* Find HPT if multitasking has started */
//                OS_Sched();
//            }
        } else {
//            OS_ENTER_CRITICAL();
            OSTCBPrioTbl[prio] = (OS_TCB *)0;                  /* Make this priority avail. to others  */
//            OS_EXIT_CRITICAL();
        }
        return (err);
    }
//    OS_EXIT_CRITICAL();
    return (OS_ERR_PRIO_EXIST);
}











/*
*********************************************************************************************************
*                                            QUERY A TASK
*
* Description: This function is called to obtain a copy of the desired task's TCB.
*
* Arguments  : id         is the id. of the task to obtain information from.
*
*              p_task_data  is a pointer to where the desired task's OS_TCB will be stored.
*
* Returns    : OS_ERR_NONE            if the requested task is suspended
*              OS_ERR_PRIO_INVALID    if the priority you specify is higher that the maximum allowed
*                                     (i.e. > OS_LOWEST_PRIO) or, you have not specified OS_PRIO_SELF.
*              OS_ERR_PRIO            if the desired task has not been created
*              OS_ERR_TASK_NOT_EXIST  if the task is assigned to a Mutex PIP
*              OS_ERR_PDATA_NULL      if 'p_task_data' is a NULL pointer
*********************************************************************************************************
*/

INT8U  OSTaskQuery (INT8U prio, OS_TCB  *p_task_data)
{
    OS_TCB    *ptcb;
#if OS_CRITICAL_METHOD == 3u                     /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0u;
#endif



#if OS_ARG_CHK_EN > 0u
    if (prio > OS_LOWEST_PRIO) {                 /* Task priority valid ?                              */
        if (prio != OS_PRIO_SELF) {
            return (OS_ERR_PRIO_INVALID);
        }
    }
    if (p_task_data == (OS_TCB *)0) {            /* Validate 'p_task_data'                             */
        return (OS_ERR_PDATA_NULL);
    }
#endif

    if (prio == OS_PRIO_SELF) {                  /* See if suspend SELF                                */
        prio = OSTCBCur->OSTCBPrio;
    }
    ptcb = OSTCBPrioTbl[prio];
    if (ptcb == (OS_TCB *)0) {                   /* Task to query must exist                           */
//        OS_EXIT_CRITICAL();
        return (OS_ERR_PRIO);
    }
    if (ptcb == OS_TCB_RESERVED) {               /* Task to query must not be assigned to a Mutex      */
//        OS_EXIT_CRITICAL();
        return (OS_ERR_TASK_NOT_EXIST);
    }
                                                 /* Copy TCB into user storage area                    */
    OS_MemCopy((INT8U *)p_task_data, (INT8U *)ptcb, sizeof(OS_TCB));
//    OS_EXIT_CRITICAL();
    return (OS_ERR_NONE);
}




















/*
*********************************************************************************************************
*                                            DELETE A TASK
*
* Description: This function allows you to delete a task from the list of available tasks...
*
* Arguments  : id    is the identifier of the task to delete.
*
* Returns    : OS_ERR_NONE             if the call is successful
*              OS_ERR_TASK_NOT_EXIST   if the task you want to delete does not exist.
*
*********************************************************************************************************
*/

//#if OS_TASK_DEL_EN > 0u
INT8U  OSTaskDel (INT8U id)
{
    OS_TCB       *ptcb;

//    if (prio == OS_PRIO_SELF) {                         /* See if requesting to delete self            */
//        prio = OSTCBCur->OSTCBPrio;                     /* Set priority to delete to current           */
//    }

    ptcb = OSTCBPrioTbl[id];
    if (ptcb == (OS_TCB *)0) {                          /* Task to delete must exist                   */
        return (OS_ERR_TASK_NOT_EXIST);
    }
    OSTCBPrioTbl[id] = (OS_TCB *)0;                   /* Clear old priority entry                    */



//    OSRdyTbl[ptcb->OSTCBY] &= (OS_PRIO)~ptcb->OSTCBBitX;
//    if (OSRdyTbl[ptcb->OSTCBY] == 0u) {                 /* Make task not ready                         */
//        OSRdyGrp           &= (OS_PRIO)~ptcb->OSTCBBitY;
//    }


//    ptcb->OSTCBDly      = 0u;                           /* Prevent OSTimeTick() from updating          */
//    ptcb->OSTCBStat     = OS_STAT_RDY;                  /* Prevent task from being resumed             */
//    ptcb->OSTCBStatPend = OS_STAT_PEND_OK;
//    if (OSLockNesting < 255u) {                         /* Make sure we don't context switch           */
//        OSLockNesting++;
//    }
    if((ptcb->OSTCBPrev == (OS_TCB *)0) && (ptcb->OSTCBNext == (OS_TCB *)0))	// Removing the sole TCB.
    {
    	OSTCBList 	= (OS_TCB *)0;
    	OSTCBFirst 	= (OS_TCB *)0;
    	OSTCBCur 	= (OS_TCB *)0;

    }
    else if (ptcb->OSTCBNext == (OS_TCB *)0)									// Removing the first created TCB.
    {
    	ptcb->OSTCBPrev->OSTCBNext = (OS_TCB *)0;
    	OSTCBFirst = ptcb->OSTCBPrev;											// The previous TCB becomes the first TCB.
    	if(OSTCBCur == ptcb)													// If removing the current TCB.
    		OSTCBCur = ptcb->OSTCBPrev;											// The previous TCB becomes the current TCB.
    }
    else if (ptcb->OSTCBPrev == (OS_TCB *)0)									// Removing the last created TCB.
    {
    	ptcb->OSTCBNext->OSTCBPrev = (OS_TCB *)0;
    	OSTCBList = ptcb->OSTCBNext;											// The next TCB becomes the last TCB.
    	if(OSTCBCur == ptcb)													// If removing the current TCB.
    		OSTCBCur = OSTCBFirst;												// The first TCB becomes the current TCB.
    }
    else 																		// Removing a TCB from the middle of the list.
    {
        ptcb->OSTCBPrev->OSTCBNext = ptcb->OSTCBNext;
        ptcb->OSTCBNext->OSTCBPrev = ptcb->OSTCBPrev;
        if(OSTCBCur == ptcb)													// If removing the current TCB.
			OSTCBCur = ptcb->OSTCBPrev;											// The previous TCB becomes the current TCB.
    }

    ptcb->OSTCBNext     = OSTCBFreeList;                /* Return TCB to free TCB list                 */
    OSTCBFreeList       = ptcb;
    return (OS_ERR_NONE);
}
//#endif










///*$PAGE*/
///*
//*********************************************************************************************************
//*                                              pend_msg_from_shared_mem
//*
//* Description: This is the function of the idle task. It polls the shared memory until a external message
//* 			is received.
//*
//* Arguments  : addr is the address at the shared memory that will contain a message addressing the
//* 			underlying processor.
//*
//* Returns    : none
//*
//*********************************************************************************************************
//*/
//OS_TCB* find_task(void* funct_addr)
//{
//	OS_TCB* ptcb = OSTCBList;
//
//	while (ptcb != (OS_TCB*)0)
//	{
//		if (ptcb->OSTCBStkPtr[FUNCT_OFFSET_TASK_STK] == (OS_STK) funct_addr)
//		{
//			return ptcb;
//		}
//		ptcb = ptcb->OSTCBNext;
//	}
//	return (OS_TCB*)0;
//}





/*
 * Function : mtapi_task_create
 */
//void mtapi_task_create(void* funct_addr)
//{
//	OSTaskCreateExt(funct_addr,
//					(void*)0,
//					&tasks_stack[OS_DEFAULT_STACK_SIZE],
//					prio,
//					prio,
//					tasks_stack,
//					OS_DEFAULT_STACK_SIZE,
//					(void*)0,
//					0);
//
//
//	if (prio < OS_MAX_PRIO)
//		prio++;
//	else
//		prio = 0;
//}



/*
 * Function : mtapi_task_start
 */
//BOOLEAN mtapi_task_start(void* funct_addr)
//{
//	// TODO: First find the corresponding task.
//	OSTCBHighRdy = find_task(funct_addr);
//	if(OSTCBHighRdy == (OS_TCB*)0)
//		return false;
//	else
//	{
//		OSStartHighRdy();
////		OSTaskDel();
////		lrt_schedule();
//		return true;
//	}
//}

