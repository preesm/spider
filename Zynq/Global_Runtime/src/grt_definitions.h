/*
 * rt_definitions.h
 *
 *  Created on: 26 oct. 2012
 *      Author: yoliva
 */

#ifndef GRT_DEFINITIONS_H_
#define GRT_DEFINITIONS_H_


#define DEBUG_EN	1


/**************** Message types ******************/
#define MSG_CREATE_TASK			1
#define MSG_CREATE_FIFO			2
#define MSG_START_SCHED			3
#define MSG_STOP_TASK			4
#define MSG_CLEAR_FIFO			5
#define MSG_CURR_VERTEX_ID		6


/**************** FIFO's******************************/
#define FIFO_IN_DIR		0
#define FIFO_OUT_DIR	1
#define MAX_NB_FIFO		4

/**************** TASK's******************************/
#define MAX_NB_ARGS		8


/**************** Actor machine's*********************/
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


/**************** MISCELLANEOUS ************************/
#define false				0
#define true				1

/**************** Data types ******************/
typedef unsigned int 		INT32U;

typedef int					INT32S;

typedef unsigned char		INT8U;

typedef char				INT8S;

typedef unsigned char		BOOLEAN;


typedef void (*FUNCTION_TYPE)(void);						// Function of a task/vertex.


typedef struct am_edge_struct	// Structure of an actor machine's edge.
{
	INT32U	am_edge_src;			// Id. of the source vertex.
	INT32U	am_edge_sink;			// Id. of the sink vertex.
}AM_EDGE_STRUCT;


typedef unsigned char 		AM_CONDITION_TYPE;				// Type of AM conditions.


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


typedef unsigned char 		AM_VERTEX_TYPE;


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



/*$PAGE*/
/*
*********************************************************************************************************
*                                            EXTERN DECLARATIONS
*********************************************************************************************************
*/
extern void print(char*);


#endif
