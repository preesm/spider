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
#include "xmbox.h"

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
#define OS_ERR_PRIO                    41u
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
#define OS_ERR_TASK_NOT_EXIST          67u
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

#define OS_ERR_FIFO_NO_ENOUGH_ESPACE	144u
#define OS_ERR_FIFO_NO_ENOUGH_DATA		145u
#define OS_ERR_FIFO_INVALID_ID			146u
#define OS_ERR_FIFO_NOT_INITIALIZED		147u



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
#define MSG_CREATE_TASK			1
#define MSG_CREATE_FIFO			2
#define MSG_START_SCHED			3
#define MSG_STOP_TASK			4
#define MSG_CLEAR_FIFO			5
#define MSG_CURR_VERTEX_ID		6



/*
*********************************************************************************************************
*                           TASK's
*********************************************************************************************************
*/
#define MAX_NB_ARGS		8


/*
*********************************************************************************************************
*                           FIFO's
*********************************************************************************************************
*/
#define FIFO_WR_IX_OFFSET		0		// Offsets in number of 32-bits words.
#define FIFO_RD_IX_OFFSET		4
#define FIFO_DATA_OFFSET		8


#define FIFO_SIZE		30*1024
#define FIFO_IN_ADDR	XPAR_BRAM_0_BASEADDR
#define FIFO_OUT_ADDR	XPAR_BRAM_0_BASEADDR
#define FIFO_IN_DIR		0
#define FIFO_OUT_DIR	1
#define MAX_NB_FIFO		4


//************************ Status codes
#define FIFO_STAT_INIT		1
#define FIFO_STAT_USED		2
#define FIFO_STAT_FULL		3
#define FIFO_STAT_EMPTY		4

/*
*********************************************************************************************************
*                           Actor machine's
*********************************************************************************************************
*/
#define AM_STATE_MAX_CONDITIONS	10
#define AM_MAX_NB_VERTICES		10
#define AM_MAX_NB_EDGES			10
#define AM_MAX_NB_CONDITIONS	10
#define AM_MAX_NB_SUCCESSORS	5

// AM_CONDITION_TYPE
#define cond_check_out_fifo		0
#define cond_check_in_fifo		1

// AM_VERTEX_TYPES
#define vertex_state			0
#define vertex_exec 			1
#define vertex_wait				2
#define vertex_test				3



/*
*********************************************************************************************************
*                                          MISCELLANEOUS
*********************************************************************************************************
*/
#define false				0
#define true				1

#define OS_PRIO_SELF      	0xFFu              				// Indicate SELF priority
#define OS_RDY_TBL_SIZE   	((OS_LOWEST_PRIO) / 8u + 1u)	// Size of ready table.


#define print				zynq_print						// Wrapper for print. Implemented at lrt_debug.c
#define putnum				zynq_putnum						// Wrapper for putnum. Implemented at lrt_debug.c
#define putnum_dec			zynq_putnum_dec					// Wrapper for putnum in decimal.




/*$PAGE*/
/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
*/

typedef unsigned  char	 		 BOOLEAN;
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


typedef void (*FUNCTION_TYPE)(void);										// Function of a task/vertex.


typedef BOOLEAN (*TEST_FUNCTION_TYPE)(INT32U);									// Test condition function type.


typedef struct am_edge_struct	// Structure of an actor machine's edge.
{
	INT32U	am_edge_src;			// Id. of the source vertex.
	INT32U	am_edge_sink;			// Id. of the sink vertex.
}AM_EDGE_STRUCT;


typedef unsigned  char			AM_CONDITION_TYPE;			// Type of AM conditions.


typedef struct am_actor_cond_struct		//
{
	INT32U				id;
	AM_CONDITION_TYPE	type;
	INT32U				data_size;
	INT32U				fifo_id;
}AM_ACTOR_COND_STRUCT;


typedef struct am_vertex_cond_struct		//
{
	INT32U				ix;					// Index in the actor's array of conditions.
	BOOLEAN				value;
}AM_VERTEX_COND_STRUCT;


typedef unsigned  char			AM_VERTEX_TYPE;


typedef struct am_vertex_struct	// Structure of an actor machine's vertex.
{
	AM_VERTEX_TYPE			type;									// Type of vertex.
	INT32U					id;										// Id. of the vertex.
	INT32U					successor_ix[AM_MAX_NB_SUCCESSORS];		// Indices of the vertex' successors.
	INT8U					nb_conditions;							// Number of conditions(State vertex).
	AM_VERTEX_COND_STRUCT	conditions[AM_STATE_MAX_CONDITIONS];	// Conditions of a state(State vertex).
	INT32U					cond_ix;								// Index, of the condition to be tested(Test vertex), in the actor's array of conditions.
	INT32U					action_funct_ix;						// Index of the vertex's action function.
	FUNCTION_TYPE			funct_ptr;								// Pointer to the vertex's function.
}AM_VERTEX_STRUCT;


typedef struct msg_create_task_struct 	// Structure of a create task message.
{
	INT32U 					task_id;
	INT32U					function_id;
	INT32U					fifo_direction;							// Input : 0, Output : 1.
	INT32U					nb_fifo_in;								// Number of input FIFOs.
	INT32U					nb_fifo_out;							// Number of output FIFOs.
	INT32U					fifo_in_id[MAX_NB_FIFO];				// Array of input FIFO ids.
	INT32U					fifo_out_id[MAX_NB_FIFO];				// Array of output FIFO ids.
	INT32U					start_vextex_ix;						// Index of the starting vertex.
	INT32U					nb_am_vertices;							// Number of vertices in the AM.
	AM_VERTEX_STRUCT		am_vertices[AM_MAX_NB_VERTICES];		// Array of AM's vertices.
	INT32U					nb_am_conditions;						// Number of conditions in the AM.
	AM_ACTOR_COND_STRUCT	am_conditions[AM_MAX_NB_CONDITIONS];	// Array of the AM's conditions.
}MSG_CREATE_TASK_STRUCT;


typedef struct msg_create_fifo_struct	// Structure of a create FIFO message.
{
	INT32U 	id;
	INT32U	size;
	INT32U	mem_block;				// Memory block where the FIFO will be created.
	INT32U	block_ix;				// Index of each FIFO in a single memory block.
	INT32U	direction;				// Input : 0, Output : 1.
	BOOLEAN init;					// If true, the FIFO's indices are cleared, if false they are updated from the FIFO's registers.
}MSG_CREATE_FIFO_STRUCT;


typedef struct generic_msg_struct
{
	INT32U msg_type;
	void*  msg_type_struct;
}GENERIC_MSG_STRUC;


typedef struct lrt_msg														// Message's structure
{
	INT32U	msg_type;
	INT32U 	task_id;
	INT32U	function_id;
	INT32U	nb_args;				// Number of arguments to be passed to the function.
	INT32U	args[MAX_NB_ARGS];		// Array of arguments to be passed to the function.
	INT32U	fifo_id;
	INT32U	fifo_size;
	INT32U	fifo_direction;			// Input : 0, Output : 1.
	INT32U	nb_fifo_in;				// Number of input FIFOs.
	INT32U	nb_fifo_out;			// Number of output FIFOs.
	INT32U	fifo_in[MAX_NB_FIFO];	// Array of input FIFO ids.
	INT32U	fifo_out[MAX_NB_FIFO];	// Array of output FIFO ids.
}LRT_MSG;


typedef struct lrt_fifo_hndle{			// FIFO handle block
//    void   *OSMemAddr;                    /* Pointer to beginning of memory partition                  */
    INT32U	FifoBaseAddr;				// FIFO's base address.
    INT32U	DataBaseAddr;				// Data's base address.
//    void   *OSMemFreeList;                /* Pointer to list of free memory blocks                     */
    INT32U  Size;                 		// FIFO's depth.
    INT8U	Direction;					// Input : 1 , Output : 0.
    INT8U	Status;						// One of the defined FIFO states.
//    INT32U  OSMemNBlks;                   /* Total number of blocks in this partition                  */
//    INT32U  OSMemNFree;                   /* Number of memory blocks remaining in this partition       */
//#if OS_MEM_NAME_EN > 0u
//    INT8U  *OSMemName;                    /* Memory partition name                                     */
//#endif
    INT32U	wr_ix;						// Write index.
    INT32U	rd_ix;						// Read index.
    void	*next_hndl;					// Pointer to the next LRT_FIFO_HNDLE object.
} LRT_FIFO_HNDLE;


/*$PAGE*/
/*
*********************************************************************************************************
*                                          TASK CONTROL BLOCK
*********************************************************************************************************
*/

typedef struct os_tcb {
    OS_STK          *OSTCBStkPtr;           /* Pointer to current top of stack                         */
    OS_STK          *OSTCBStkBottom;        /* Pointer to bottom of stack                              */
    INT32U           OSTCBStkSize;          /* Size of task stack (in number of stack elements)        */
//    INT16U           OSTCBOpt;              /* Task options as passed by OSTaskCreateExt()             */
    INT16U           OSTCBId;               /* Task ID (0..65535)                                      */

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

    FUNCTION_TYPE		task_func;							// The function that is executed by the task.
    INT32U          	args[MAX_NB_ARGS];					// Array of arguments to be passed to the function.
    LRT_FIFO_HNDLE		*fifo_in[MAX_NB_FIFO];				// Pointer to the list of input FIFOs.
    LRT_FIFO_HNDLE		*fifo_out[MAX_NB_FIFO];				// Pointer to the list of output FIFOs.

	AM_VERTEX_STRUCT		am_vertices[AM_MAX_NB_VERTICES];		// Table of vertices of the actor machine.
	AM_ACTOR_COND_STRUCT	am_conditions[AM_MAX_NB_CONDITIONS];	// Table of conditions of the actor machine.

	AM_VERTEX_STRUCT	*current_vertex;					// Active vertex.
} OS_TCB;



/*$PAGE*/
/*
*********************************************************************************************************
*                                            EXTERN DECLARATIONS
*********************************************************************************************************
*/

// OS's global variables.

extern INT8U	        OSRdyGrp;                        			// Ready list group
extern INT8U			OSRdyTbl[OS_RDY_TBL_SIZE];       			// Table of tasks which are ready to run

extern INT8U            OSPrioCur;                					/* Priority of current task                        	*/
extern INT8U            OSPrioHighRdy;            					/* Priority of highest priority task               	*/

extern OS_TCB			*OSTCBCur;                        			/* Pointer to currently running TCB         		*/
extern OS_TCB          	*OSTCBFirst;								// Pointer to first created TCB.
extern OS_TCB          	*OSTCBFreeList;                   			/* Pointer to list of free TCBs             		*/
extern OS_TCB          	*OSTCBHighRdy;                    			/* Pointer to highest priority TCB R-to-R   		*/
extern OS_TCB          	*OSTCBList;                       			// Pointer to doubly linked list of TCBs
extern OS_TCB          	*OSTCBPrioTbl[OS_LOWEST_PRIO + 1u];    		// Table of pointers to created TCBs
extern OS_TCB          	OSTCBTbl[OS_MAX_TASKS + OS_N_SYS_TASKS];	// Table of TCBs


// Tables of FIFOs.
extern LRT_FIFO_HNDLE   InputFIFOs[OS_NB_IN_FIFO];					// Table of input FIFO handles.
extern LRT_FIFO_HNDLE   OutputFIFOs[OS_NB_OUT_FIFO];				// Table of output FIFO handles.


#if CONTROL_COMM == 1
extern LRT_FIFO_HNDLE 	*cntrl_fifo;								// Pointer to an input FIFO for control messages.
#endif


// AM's functions.
extern void am_funct_test();


extern void am_funct_exec();


extern void am_funct_wait();


extern void am_funct_move();


// Table of functions.
extern FUNCTION_TYPE 		functions_tbl[];						// Table of local functions.


//extern TEST_FUNCTION_TYPE	test_functions_tbl[];					// Table of test condition functions.

// Miscellaneous.
extern BOOLEAN			sched_locked;								// Flag indicating whether scheduling is enabled.
extern BOOLEAN          lrt_running;                       			/* Flag indicating that kernel is running   		*/
extern INT32U			control_addr;								// Address for receiving external messages.

#endif /* RT_DEFINITIONS_H_ */
