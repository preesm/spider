/*
 * rt_definitions.h
 *
 *  Created on: 26 oct. 2012
 *      Author: yoliva
 */

#ifndef RT_DEFINITIONS_H_
#define RT_DEFINITIONS_H_

#include "lrt_cfg.h"
#include <types.h>

/*
*********************************************************************************************************
*                              TASK STATUS (Bit definition for OSTCBStat)
*********************************************************************************************************
*/
typedef enum os_stat{
	OS_STAT_UNINITIALIZED,
	OS_STAT_READY,
	OS_STAT_RUNNING
} OS_STAT;

#define  OS_TCB_RESERVED        ((OS_TCB *)1)

typedef void (*FUNCTION_TYPE)(void);										// Function of a task/vertex.
/*
*********************************************************************************************************
*                                             ERROR CODES
*********************************************************************************************************
*/
#define OS_ERR_NONE                     0u

#define OS_ERR_TASK_NOT_EXIST          	67u
#define OS_ERR_TASK_ID_ALREADY_EXIST	74u

#define OS_ERR_FIFO_NO_ENOUGH_ESPACE	144u
#define OS_ERR_FIFO_NO_ENOUGH_DATA		145u
#define OS_ERR_FIFO_INVALID_ID			146u
#define OS_ERR_FIFO_NOT_INITIALIZED		147u

/*
*********************************************************************************************************
*                           GLOBAL RUNTIME MESSAGE TYPES
*********************************************************************************************************
*/
// GRT_MESSAGE_TYPE
typedef enum{
	MSG_CREATE_TASK=1,
	MSG_CREATE_FIFO=2,
	MSG_START_SCHED=3,
	MSG_STOP_TASK=4,
	MSG_CLEAR_FIFO=5,
	MSG_CURR_VERTEX_ID=6
}GRT_MSG_TYPE;


/*
*********************************************************************************************************
*                           FIFO's
*********************************************************************************************************
*/
#define FIFO_RD_IX_OFFSET		0		// Offsets in number of 32-bits words.
#define FIFO_WR_IX_OFFSET		4
#define FIFO_DATA_OFFSET		8

//************************ Status codes
#define FIFO_STAT_INIT		1
#define FIFO_STAT_USED		2

// FIFO handle block
typedef struct lrt_fifo_hndle{
    UINT32	address;				// FIFO's base address.
    UINT32  Size;                 		// FIFO's depth.
    UINT8	Status;						// One of the defined FIFO states.
} LRT_FIFO_HNDLE;

/*
*********************************************************************************************************
*                           Actor machine's
*********************************************************************************************************
*/

// AM_CONDITION_TYPE
typedef enum{
	COND_UNINITIALIZED=0,
	FIFO_IN=1,
	FIFO_OUT=2
}COND_TYPE;

// AM_VERTEX_TYPES
typedef enum{
	VERTEX_UNINITIALIZED=0,
	WAIT,
	STATE,
	TEST,
	EXEC
} AM_VERTEX_TYPE ;

/*
*********************************************************************************************************
*                                          MISCELLANEOUS
*********************************************************************************************************
*/

#define print				zynq_print						// Wrapper for print. Implemented at lrt_debug.c
#define putnum				zynq_putnum						// Wrapper for putnum. Implemented at lrt_debug.c
#define putnum_dec			zynq_putnum_dec					// Wrapper for putnum in decimal.

typedef enum{
	Default		  = 0,
	AMManagement  = 1,
	Scheduling	  = 2,
	FifoCheck	  = 3,
	DataTransfert = 4,
	CtrlFifoHandling = 5,
	Action		  = 6,
	MaxMonitor	  = Action+6,
} MonitorAction;


/*
*********************************************************************************************************
*                                          ACTOR MACHINE STRUCT
*********************************************************************************************************
*/

typedef struct {		//
	unsigned  char	type;
	union{
		/* FIFO Condition specific attributes */
		struct {
			INT8 	id;
			INT32 size;
		} fifo;
	};
}AM_ACTOR_COND_STRUCT;

typedef struct {
	UINT8 	functionID;
	UINT8	nb_fifo_in;
	UINT8	nb_fifo_out;
	UINT8	nb_param;
	UINT8	fifo_in_id[MAX_NB_FIFO];
	UINT8	fifo_out_id[MAX_NB_FIFO];
	UINT32	param_value[MAX_NB_ARGS];

}AM_ACTOR_ACTION_STRUCT;

typedef struct // Structure of an actor machine's vertex.
{
	AM_VERTEX_TYPE			type;								// Type of vertex.
	UINT8					successor_ix[AM_MAX_NB_SUCCESSORS];	// Indices of the vertex' successors.
	union{
		UINT8			condID;								// Index, of the condition to be tested(Test vertex), in the actor's array of conditions.
		UINT8			actionID;							// Index of the vertex's action function.
	};
}AM_VERTEX_STRUCT;



/*
*********************************************************************************************************
*                                          TASK CONTROL BLOCK
*********************************************************************************************************
*/

typedef struct os_tcb {
    UINT8           OSTCBId;               /* Task ID (0..65535)                                      */
    UINT8           OSTCBState;

    struct os_tcb   *OSTCBNext;             /* Pointer to next     TCB in the TCB list                 */

    FUNCTION_TYPE		task_func;							// The function that is executed by the task.

    UINT8 nbVertices;
    UINT8 nbConds;
    UINT8 nbActions;
	AM_VERTEX_STRUCT		am_vertices[AM_MAX_NB_VERTICES];		// Table of vertices of the actor machine.
	AM_ACTOR_COND_STRUCT	am_conditions[AM_MAX_NB_CONDITIONS];	// Table of conditions of the actor machine.
	AM_ACTOR_ACTION_STRUCT	am_actions[AM_MAX_NB_ACTIONS];

	UINT8	current_vertexID;					// Active vertex.

	BOOLEAN stop;
	UINT8 stopState;
} OS_TCB;



/*$PAGE*/
/*
*********************************************************************************************************
*                                            EXTERN DECLARATIONS
*********************************************************************************************************
*/

// OS's global variables.
extern OS_TCB			*OSTCBCur;                        			/* Pointer to currently running TCB */
extern OS_TCB          	OSTCBTbl[OS_MAX_TASKS];						// Table of TCBs

// AM's functions.
extern void (*am_funct[5]) (void);

// Table of functions.

extern FUNCTION_TYPE 		functions_tbl[];						// Table of local functions.

// Miscellaneous.
extern BOOLEAN          lrt_running;                       			/* Flag indicating that kernel is running   		*/

#endif /* RT_DEFINITIONS_H_ */
