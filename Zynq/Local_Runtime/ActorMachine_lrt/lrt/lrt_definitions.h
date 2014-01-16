
/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet				*
 * 																			*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr					*
 * 																			*
 * This software is a computer program whose purpose is to execute			*
 * parallel applications.													*
 * 																			*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 												*
 * 																			*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 																*
 * 																			*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 									*
 * 																			*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.			*
 ****************************************************************************/

#ifndef RT_DEFINITIONS_H_
#define RT_DEFINITIONS_H_

#include "lrt_cfg.h"



#define TRUE 			1
#define FALSE 			0

#define  OS_TCB_RESERVED        ((OS_TCB *)1)

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
*                                             FIFOs
*********************************************************************************************************
*/
#define FIFO_RD_IX_OFFSET		0		// Offsets in number of 32-bits words.
#define FIFO_WR_IX_OFFSET		4
#define FIFO_DATA_OFFSET		8

#define FIFO_MUTEX_SIZE			1
#define MAX_NB_FIFOs			200

//************************ Status codes
#define FIFO_STAT_INIT		1
#define FIFO_STAT_USED		2

/*
*********************************************************************************************************
*                                          MISCELLANEOUS
*********************************************************************************************************
*/

#define print				zynq_print						// Wrapper for print. Implemented at lrt_debug.c
#define putnum				zynq_putnum						// Wrapper for putnum. Implemented at lrt_debug.c
#define putnum_dec			zynq_putnum_dec					// Wrapper for putnum in decimal.


#define PRINT_ACTOR_IN_DOT_FILE		1


/*
 * Data types.
 */
typedef unsigned char 	BOOLEAN;

typedef char			INT8_S;
typedef short			INT16_S;
typedef int			 	INT32_S;
typedef long long	 	INT64_S;

typedef unsigned char 	UINT8;
typedef unsigned short	UINT16;
typedef unsigned int	UINT32;
typedef long long unsigned	UINT64;

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



typedef void (*FUNCTION_TYPE)(UINT32 inputFIFOIds[],
							  UINT32 inputFIFOAddrs[],
							  UINT32 outputFIFOIds[],
							  UINT32 outputFIFOAddrs[],
							  UINT32 params[]);// Function of a task/vertex.

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
			INT8_S 	id;
			INT32_S size;
		} fifo;
	};
}AM_ACTOR_COND_STRUCT;

typedef struct {
	UINT8 	functionID;
	UINT8	nb_fifo_in;
	UINT8	nb_fifo_out;
	UINT8	nb_param;
	UINT32	inputFifoIds[MAX_NB_FIFO];
	UINT32	outputFifoIds[MAX_NB_FIFO];
	UINT32 	inputFifoDataOffs[MAX_NB_FIFO];
	UINT32 	outputFifoDataOffs[MAX_NB_FIFO];
	UINT32	params[MAX_NB_PARAMETERS];

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
 * Actor Machine' structure
 */
typedef struct actorMachine{
    UINT8 nbVertices;
    UINT8 nbConds;
    UINT8 nbActions;
	AM_VERTEX_STRUCT		am_vertices[AM_MAX_NB_VERTICES];
	AM_ACTOR_COND_STRUCT	am_conditions[AM_MAX_NB_CONDITIONS];
	AM_ACTOR_ACTION_STRUCT	am_actions[AM_MAX_NB_ACTIONS];

	UINT8	currVertexId;	// Active vertex' index.
	UINT8 	stopVertexId;	// Stop vertex' index.
} ActorMachine;



/*
 * Job' structure.
 */
typedef struct lrtActor{
	UINT32	nbInputFifos;
	UINT32	nbOutputFifos;
	UINT32	nbParams;
	UINT32	inputFifoId[MAX_NB_FIFO];
	UINT32	outputFifoId[MAX_NB_FIFO];
	UINT32	readDataSize[MAX_NB_FIFO];
	UINT32	writeDataSize[MAX_NB_FIFO];
	UINT32 	inputFifoDataOff[MAX_NB_FIFO];
	UINT32 	outputFifoDataOff[MAX_NB_FIFO];
	UINT32	params[MAX_NB_PARAMETERS];
}LRTActor;



/*
*********************************************************************************************************
*                                          TASK CONTROL BLOCK
*********************************************************************************************************
*/

typedef struct os_tcb {
    UINT16			OSTCBId;
    UINT8           OSTCBState;
    struct os_tcb   *OSTCBNext;
    UINT32			functionId;
    FUNCTION_TYPE	task_func;
    BOOLEAN			isAM;
	BOOLEAN 		stop;		// Whether the task must stop running.
	LRTActor*		actor;
	ActorMachine	am;			// TODO: ..define it as a pointer to save memory footprint.
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

extern LRTActor			LRTActorTbl[OS_MAX_TASKS];
// Table of functions.

extern FUNCTION_TYPE 		functions_tbl[];						// Table of local functions.

// Miscellaneous.
extern BOOLEAN          lrt_running;                       			/* Flag indicating that kernel is running   		*/

// AM's functions.
extern void (*am_funct[5]) (void);

extern void init_lrt();

#endif /* RT_DEFINITIONS_H_ */
