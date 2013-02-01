/*
 * rt_definitions.h
 *
 *  Created on: 26 oct. 2012
 *      Author: yoliva
 */

#ifndef RT_DEFINITIONS_H_
#define RT_DEFINITIONS_H_

#include "lrt_cfg.h"
//#include "mtapi/mtapi.h"

//extern void print();
//extern void putnum();


/*$PAGE*/
/*
*********************************************************************************************************
*                              TASK STATUS (Bit definition for OSTCBStat)
*********************************************************************************************************
*/
#define  OS_STAT_RDY                 0x00u  /* Ready to run                                            */
//#define  OS_STAT_SEM                 0x01u  /* Pending on semaphore                                    */
//#define  OS_STAT_MBOX                0x02u  /* Pending on mailbox                                      */
//#define  OS_STAT_Q                   0x04u  /* Pending on queue                                        */
//#define  OS_STAT_SUSPEND             0x08u  /* Task is suspended                                       */
//#define  OS_STAT_MUTEX               0x10u  /* Pending on mutual exclusion semaphore                   */
//#define  OS_STAT_FLAG                0x20u  /* Pending on event flag group                             */
//#define  OS_STAT_MULTI               0x80u  /* Pending on multiple events                              */
//
//#define  OS_STAT_PEND_ANY         (OS_STAT_SEM | OS_STAT_MBOX | OS_STAT_Q | OS_STAT_MUTEX | OS_STAT_FLAG)
//
#define  OS_TCB_RESERVED        ((OS_TCB *)1)
/*
*********************************************************************************************************
*                                             ERROR CODES
*********************************************************************************************************
*/
#define OS_ERR_NONE                     0u

//#define OS_ERR_EVENT_TYPE               1u
//#define OS_ERR_PEND_ISR                 2u
//#define OS_ERR_POST_NULL_PTR            3u
//#define OS_ERR_PEVENT_NULL              4u
//#define OS_ERR_POST_ISR                 5u
//#define OS_ERR_QUERY_ISR                6u
//#define OS_ERR_INVALID_OPT              7u
//#define OS_ERR_ID_INVALID               8u
//#define OS_ERR_PDATA_NULL               9u
//
//#define OS_ERR_TIMEOUT                 10u
//#define OS_ERR_EVENT_NAME_TOO_LONG     11u
//#define OS_ERR_PNAME_NULL              12u
//#define OS_ERR_PEND_LOCKED             13u
//#define OS_ERR_PEND_ABORT              14u
//#define OS_ERR_DEL_ISR                 15u
//#define OS_ERR_CREATE_ISR              16u
//#define OS_ERR_NAME_GET_ISR            17u
//#define OS_ERR_NAME_SET_ISR            18u
//#define OS_ERR_ILLEGAL_CREATE_RUN_TIME 19u
//
//#define OS_ERR_MBOX_FULL               20u
//
//#define OS_ERR_Q_FULL                  30u
//#define OS_ERR_Q_EMPTY                 31u
//
#define OS_ERR_PRIO_EXIST              40u
//#define OS_ERR_PRIO                    41u
//#define OS_ERR_PRIO_INVALID            42u
//
//#define OS_ERR_SCHED_LOCKED            50u
//#define OS_ERR_SEM_OVF                 51u

//#define OS_ERR_TASK_CREATE_ISR         60u
//#define OS_ERR_TASK_DEL                61u
//#define OS_ERR_TASK_DEL_IDLE           62u
//#define OS_ERR_TASK_DEL_REQ            63u
//#define OS_ERR_TASK_DEL_ISR            64u
//#define OS_ERR_TASK_NAME_TOO_LONG      65u
#define OS_ERR_TASK_NO_MORE_TCB        66u
//#define OS_ERR_TASK_NOT_EXIST          67u
//#define OS_ERR_TASK_NOT_SUSPENDED      68u
//#define OS_ERR_TASK_OPT                69u
//#define OS_ERR_TASK_RESUME_PRIO        70u
//#define OS_ERR_TASK_SUSPEND_IDLE       71u
//#define OS_ERR_TASK_SUSPEND_PRIO       72u
//#define OS_ERR_TASK_WAITING            73u

//#define OS_ERR_TIME_NOT_DLY            80u
//#define OS_ERR_TIME_INVALID_MINUTES    81u
//#define OS_ERR_TIME_INVALID_SECONDS    82u
//#define OS_ERR_TIME_INVALID_MS         83u
//#define OS_ERR_TIME_ZERO_DLY           84u
//#define OS_ERR_TIME_DLY_ISR            85u

//#define OS_ERR_MEM_INVALID_PART        90u
//#define OS_ERR_MEM_INVALID_BLKS        91u
//#define OS_ERR_MEM_INVALID_SIZE        92u
//#define OS_ERR_MEM_NO_FREE_BLKS        93u
//#define OS_ERR_MEM_FULL                94u
//#define OS_ERR_MEM_INVALID_PBLK        95u
//#define OS_ERR_MEM_INVALID_PMEM        96u
//#define OS_ERR_MEM_INVALID_PDATA       97u
//#define OS_ERR_MEM_INVALID_ADDR        98u
//#define OS_ERR_MEM_NAME_TOO_LONG       99u

//#define OS_ERR_NOT_MUTEX_OWNER        100u
//
//#define OS_ERR_FLAG_INVALID_PGRP      110u
//#define OS_ERR_FLAG_WAIT_TYPE         111u
//#define OS_ERR_FLAG_NOT_RDY           112u
//#define OS_ERR_FLAG_INVALID_OPT       113u
//#define OS_ERR_FLAG_GRP_DEPLETED      114u
//#define OS_ERR_FLAG_NAME_TOO_LONG     115u
//
//#define OS_ERR_PIP_LOWER              120u
//
//#define OS_ERR_TMR_INVALID_DLY        130u
//#define OS_ERR_TMR_INVALID_PERIOD     131u
//#define OS_ERR_TMR_INVALID_OPT        132u
//#define OS_ERR_TMR_INVALID_NAME       133u
//#define OS_ERR_TMR_NON_AVAIL          134u
//#define OS_ERR_TMR_INACTIVE           135u
//#define OS_ERR_TMR_INVALID_DEST       136u
//#define OS_ERR_TMR_INVALID_TYPE       137u
//#define OS_ERR_TMR_INVALID            138u
//#define OS_ERR_TMR_ISR                139u
//#define OS_ERR_TMR_NAME_TOO_LONG      140u
//#define OS_ERR_TMR_INVALID_STATE      141u
//#define OS_ERR_TMR_STOPPED            142u
//#define OS_ERR_TMR_NO_CALLBACK        143u

#define OS_ERR_SH_MEM_NO_ENOUGH_ESPACE	144u
#define OS_ERR_SH_MEM_NO_ENOUGH_DATA	145u




/*
*********************************************************************************************************
*                           TASK PEND STATUS (Status codes for OSTCBStatPend)
*********************************************************************************************************
*/
#define  OS_STAT_PEND_OK                0u  /* Pending status OK, not pending, or pending complete     */
//#define  OS_STAT_PEND_TO                1u  /* Pending timed out                                       */
//#define  OS_STAT_PEND_ABORT             2u  /* Pending aborted                                         */






/*
*********************************************************************************************************
*                           GLOBAL RUNTIME MESSAGE TYPES
*********************************************************************************************************
*/
#define MSG_CREATE_ACTION		1
#define MSG_CREATE_SH_MEM		2
#define MSG_START_TASK			3





#define SH_MEM_DATA_OFFSET		8		/* Shared memory partition controller */




/*$PAGE*/
/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
*/

typedef enum {false, true} 		 BOOLEAN;
typedef unsigned  char           INT8U;        /* Unsigned  8 bit quantity                           */
typedef signed    char           INT8S;        /* Signed    8 bit quantity                           */
typedef unsigned  short          INT16U;       /* Unsigned 16 bit quantity                           */
typedef signed    short          INT16S;       /* Signed   16 bit quantity                           */
typedef unsigned  int            INT32U;       /* Unsigned 32 bit quantity                           */
typedef signed    int            INT32S;       /* Signed   32 bit quantity                           */
typedef float                    FP32;
typedef double                   FP64;
typedef unsigned  int            OS_STK;       /* Each stack entry is 32-bits wide					 */


#if OS_LOWEST_PRIO <= 63u
typedef  INT8U    OS_PRIO;
#else
typedef  INT16U   OS_PRIO;
#endif

//#if     OS_VERSION < 280
//typedef unsigned  short OS_FLAGS;        /* Data type for event flag bits (8, 16 or 32 bits)             */
//#endif







//#if (OS_MEM_EN > 0u) && (OS_MAX_MEM_PART > 0u)
typedef struct os_mem {                   /* MEMORY CONTROL BLOCK                                      */
//    void   *OSMemAddr;                    /* Pointer to beginning of memory partition                  */
    INT32U	MemBaseAddr;
//    void   *OSMemFreeList;                /* Pointer to list of free memory blocks                     */
    INT32U  OSMemBlkSize;                 /* Size (in bytes) of each block of memory                   */
//    INT32U  OSMemNBlks;                   /* Total number of blocks in this partition                  */
//    INT32U  OSMemNFree;                   /* Number of memory blocks remaining in this partition       */
//#if OS_MEM_NAME_EN > 0u
//    INT8U  *OSMemName;                    /* Memory partition name                                     */
//#endif
    INT32U	wr_ix;						  // Write index.
    INT32U	rd_ix;						  // Read index.
} OS_MEM;
//#endif






typedef enum {none, start_task, create_action, get_action} EXT_MSG_TYPE;	// Type of messages from the Global Runtime

typedef void (*FUNCTION_TYPE)(void);										// Function of a task.

typedef struct ext_msg_struct												// External message's structure
{
	INT32U 		msg_type;
	INT32U		task_id;
	INT32U		function_id;
}EXT_MSG_STRUCT;

//typedef struct create_msg_data
//{
//	INT32U funct_id;
//}CREATE_MSG_DATA;










/*$PAGE*/
/*
*********************************************************************************************************
*                                          MISCELLANEOUS
*********************************************************************************************************
*/
#define OS_RDY_TBL_SIZE   ((OS_LOWEST_PRIO) / 8u + 1u)	// Size of ready table.
#define print				zynq_print					// Wrapper for print. Implemented at lrt_debug.c
#define putnum				zynq_putnum					// Wrapper for putnum. Implemented at lrt_debug.c



/*$PAGE*/
/*
*********************************************************************************************************
*                                          TASK CONTROL BLOCK
*********************************************************************************************************
*/

typedef struct os_tcb {
    OS_STK          *OSTCBStkPtr;           /* Pointer to current top of stack                         */

#if OS_TASK_CREATE_EXT_EN > 0u
    void            *OSTCBExtPtr;           /* Pointer to user definable data for TCB extension        */
    OS_STK          *OSTCBStkBottom;        /* Pointer to bottom of stack                              */
    INT32U           OSTCBStkSize;          /* Size of task stack (in number of stack elements)        */
    INT16U           OSTCBOpt;              /* Task options as passed by OSTaskCreateExt()             */
    INT16U           OSTCBId;               /* Task ID (0..65535)                                      */
#endif

    struct os_tcb   *OSTCBNext;             /* Pointer to next     TCB in the TCB list                 */
    struct os_tcb   *OSTCBPrev;             /* Pointer to previous TCB in the TCB list                 */

#if (OS_EVENT_EN)
    OS_EVENT        *OSTCBEventPtr;         /* Pointer to          event control block                 */
#endif

#if (OS_EVENT_EN) && (OS_EVENT_MULTI_EN > 0u)
    OS_EVENT       **OSTCBEventMultiPtr;    /* Pointer to multiple event control blocks                */
#endif

#if ((OS_Q_EN > 0u) && (OS_MAX_QS > 0u)) || (OS_MBOX_EN > 0u)
    void            *OSTCBMsg;              /* Message received from OSMboxPost() or OSQPost()         */
#endif

#if (OS_FLAG_EN > 0u) && (OS_MAX_FLAGS > 0u)
#if OS_TASK_DEL_EN > 0u
    OS_FLAG_NODE    *OSTCBFlagNode;         /* Pointer to event flag node                              */
#endif
    OS_FLAGS         OSTCBFlagsRdy;         /* Event flags that made task ready to run                 */
#endif

    INT32U           OSTCBDly;              /* Nbr ticks to delay task or, timeout waiting for event   */
    INT8U            OSTCBStat;             /* Task      status                                        */
    INT8U            OSTCBStatPend;         /* Task PEND status                                        */
    INT8U            OSTCBPrio;             /* Task priority (0 == highest)                            */

    INT8U            OSTCBX;                /* Bit position in group  corresponding to task priority   */
    INT8U            OSTCBY;                /* Index into ready table corresponding to task priority   */
    OS_PRIO          OSTCBBitX;             /* Bit mask to access bit position in ready table          */
    OS_PRIO          OSTCBBitY;             /* Bit mask to access bit position in ready group          */

#if OS_TASK_DEL_EN > 0u
    INT8U            OSTCBDelReq;           /* Indicates whether a task needs to delete itself         */
#endif

#if OS_TASK_PROFILE_EN > 0u
    INT32U           OSTCBCtxSwCtr;         /* Number of time the task was switched in                 */
    INT32U           OSTCBCyclesTot;        /* Total number of clock cycles the task has been running  */
    INT32U           OSTCBCyclesStart;      /* Snapshot of cycle counter at start of task resumption   */
    OS_STK          *OSTCBStkBase;          /* Pointer to the beginning of the task stack              */
    INT32U           OSTCBStkUsed;          /* Number of bytes used from the stack                     */
#endif

#if OS_TASK_NAME_EN > 0u
    INT8U           *OSTCBTaskName;
#endif

#if OS_TASK_REG_TBL_SIZE > 0u
    INT32U           OSTCBRegTbl[OS_TASK_REG_TBL_SIZE];
#endif

    FUNCTION_TYPE	task_func;				/* The function that is executed by the task.				*/
} OS_TCB;



/*$PAGE*/
/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

extern BOOLEAN          OSRunning;                       			/* Flag indicating that kernel is running   		*/
extern INT8U            OSLockNesting;            					/* Multitasking lock nesting level                 	*/

extern INT8U	        OSRdyGrp;                        			// Ready list group
extern INT8U			OSRdyTbl[OS_RDY_TBL_SIZE];       			// Table of tasks which are ready to run

extern INT8U            OSPrioCur;                					/* Priority of current task                        	*/
extern INT8U            OSPrioHighRdy;            					/* Priority of highest priority task               	*/

extern OS_TCB			*OSTCBCur;                        			/* Pointer to currently running TCB         		*/
extern OS_TCB          	*OSTCBFreeList;                   			/* Pointer to list of free TCBs             		*/
extern OS_TCB          	*OSTCBHighRdy;                    			/* Pointer to highest priority TCB R-to-R   		*/
extern OS_TCB          	*OSTCBList;                       			// Pointer to doubly linked list of TCBs
extern OS_TCB          	*OSTCBPrioTbl[OS_LOWEST_PRIO + 1u];    		// Table of pointers to created TCBs
extern OS_TCB          	OSTCBTbl[OS_MAX_TASKS + OS_N_SYS_TASKS];	// Table of TCBs


extern OS_MEM            OSMemTbl[OS_MAX_MEM_PART];					/* Storage for memory partition manager            */

extern FUNCTION_TYPE 	functions_tbl[];							// Table of local functions.

//extern

#endif /* RT_DEFINITIONS_H_ */
