/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

#ifndef GRT_DEFINITIONS_H_
#define GRT_DEFINITIONS_H_

#include <platform_types.h>

// Architecture
#define MAX_SLAVE_TYPES 		1	//2 	// The maximum number of slave types
#define MAX_SLAVES 				8	//12	// The maximum number of slaves
#define MAX_SLAVE_NAME_SIZE 	10	// The maximum number of slaves

#define MAX_VERTEX_NAME_SIZE 50

#define MEMCPY_SPEED 1.8

// Single Rate DAG
#define MAX_SRDAG_VERTICES 		360	//512
#define MAX_SRDAG_EDGES 		450	//512
#define MAX_SRDAG_INPUT_EDGES 	12	//64
#define MAX_SRDAG_OUTPUT_EDGES 	12	//100
#define MAX_VERTEX_REPETITION 	50	//100 // The maximum number of repetitions for one vertex
#define MAX_CHILD 	150

#define BROADCAST_FUNCT_IX			11
#define SWICTH_FUNCT_IX			12
#define XPLODE_FUNCT_IX			13
#define INIT_FUNCT_IX			14
#define END_FUNCT_IX			15

// PICSDF
//#define MAX_PISDF_CONFIG_PORTS		512
//#define MAX_NB_CONFIG_PARAMS		10	// Maximum of parameters a configuration actor can set.


// PiSDF
#define MAX_NB_PiSDF_GRAPHS				3	//32
#define MAX_NB_PiSDF_EDGES				16	//32
#define MAX_NB_PiSDF_PARAMS				8	//32
#define MAX_NB_PiSDF_VERTICES			8	//32
#define MAX_NB_PiSDF_CONFIG_VERTICES	8	//32
#define MAX_NB_PiSDF_JOIN_VERTICES		8	//32
#define MAX_NB_PiSDF_INPUT_VERTICES		8	//32
#define MAX_NB_PiSDF_INPUT_EDGES		3	//32
#define MAX_NB_PiSDF_OUTPUT_EDGES		3	//32
#define MAX_NB_PiSDF_BROAD_VERTICES		8	//32
#define MAX_NB_PiSDF_OUTPUT_VERTICES	8	//32
#define MAX_NB_PiSDF_SWITCH_VERTICES	1	//32
#define MAX_NB_PiSDF_SELECT_VERTICES	1	//32
#define MAX_NB_PiSDF_ROUNDB_VERTICES	8	//32
#define MAX_NB_PiSDF_CONFIG_VERTEX_PARAMS	2	//10
#define MAX_NB_PiSDF_VERTEX_NAME	30	//10
#define MAX_NB_PiSDF_PARAM_NAME	30	//10

// XParser
#define VARIABLE_NAME_LEN_MAX 30
#define MAX_VARIABLE_NUMBER 300
#define EXPR_LEN_MAX 128
#define REVERSE_POLISH_STACK_MAX_ELEMENTS 20 // Max number of elements in an expression reverse polish stack

#define MAX_PARAM 4 // Maximum number of parameters for an actor

// AM
#define AM_VERTEX_MAX_SUCCESSOR 0	//2 //10 //2
#define AM_GRAPH_MAX_VERTEX		0	//300
#define AM_GRAPH_MAX_COND		0	//300
#define AM_GRAPH_MAX_ACTIONS	0	//100
#define AM_ACTION_NAME 			0	//100

// Platform
#define NB_MAX_CTRLQ 			8
#define MAX_NB_HW_FIFO 			300
#define DEFAULT_FIFO_SIZE		1024
//#define DEFAULT_FIFO_SIZE		2057478
#define FILE_PATH_LENGTH		50
#define MAX_NB_AM				10
#define MAX_FILE_NAME_SIZE		30
#define MAX_PISDF_STEPS 		5*ITER_MAX



#define DEBUG_EN				1

/**************** Message types ******************/
#define MSG_CREATE_TASK			1
#define MSG_EXEC_TIMES			2
#define MSG_PARAM_VALUE			3
#define MSG_CLEAR_TIME			4
#define MSG_SEND_INFO_DATA		5


/**************** FIFO's******************************/
#define FIFO_IN_DIR		0
#define FIFO_OUT_DIR	1
#define MAX_NB_FIFO		100

/**************** TASK's******************************/
#define MAX_NB_ARGS		8


/**************** Actor machine's*********************/
#define AM_STATE_MAX_CONDITIONS	0//10
#define AM_MAX_NB_VERTICES		0//10
#define AM_MAX_NB_EDGES			0//10
#define AM_MAX_NB_CONDITIONS	0//10
#define AM_MAX_NB_SUCCESSORS	0//5

// AM_CONDITION_TYPE
#define cond_check_out_fifo		0
#define cond_check_in_fifo		1

// AM_VERTEX_TYPES
#define vertex_state			0
#define vertex_exec 			1
#define vertex_wait				2
#define vertex_test				3


/**************** MISCELLANEOUS ************************/

typedef void (*FUNCTION_TYPE)(void);						// Function of a task/vertex.


typedef struct am_edge_struct	// Structure of an actor machine's edge.
{
	UINT32	am_edge_src;			// Id. of the source vertex.
	UINT32	am_edge_sink;			// Id. of the sink vertex.
}AM_EDGE_STRUCT;


typedef unsigned char 		AM_CONDITION_TYPE;				// Type of AM conditions.


typedef struct am_actor_cond_struct		//
{
	UINT32				id;
	AM_CONDITION_TYPE	type;
	UINT32				data_size;
	UINT32				fifo_id;
}AM_ACTOR_COND_STRUCT;


typedef struct am_vertex_cond_struct		//
{
	UINT32				ix;					// Index in the actor's array of conditions.
	BOOL				value;
}AM_VERTEX_COND_STRUCT;


typedef unsigned char 		AM_VERTEX_TYPE;


typedef struct am_vertex_struct	// Structure of an actor machine's vertex.
{
	AM_VERTEX_TYPE			type;									// Type of vertex.
	UINT32					id;										// Id. of the vertex.
	UINT32					successor_ix[AM_MAX_NB_SUCCESSORS];		// Indices of the vertex' successors.
	UINT8					nb_conditions;							// Number of conditions(State vertex).
	AM_VERTEX_COND_STRUCT	conditions[AM_STATE_MAX_CONDITIONS];	// Conditions of a state(State vertex).
	UINT32					cond_ix;								// Index, of the condition to be tested(Test vertex), in the actor's array of conditions.
	UINT32					action_funct_ix;						// Index of the vertex's action function.
	FUNCTION_TYPE			funct_ptr;								// Pointer to the vertex's function.
}AM_VERTEX_STRUCT;


typedef struct msg_create_task_struct 	// Structure of a create task message.
{
	UINT32 					task_id;
	UINT32					function_id;
	UINT32					fifo_direction;							// Input : 0, Output : 1.
	UINT32					nb_fifo_in;								// Number of input FIFOs.
	UINT32					nb_fifo_out;							// Number of output FIFOs.
	UINT32					fifo_in_id[MAX_NB_FIFO];				// Array of input FIFO ids.
	UINT32					fifo_out_id[MAX_NB_FIFO];				// Array of output FIFO ids.
	UINT32					start_vextex_ix;						// Index of the starting vertex.
	UINT32					nb_am_vertices;							// Number of vertices in the AM.
	AM_VERTEX_STRUCT		am_vertices[AM_MAX_NB_VERTICES];		// Array of AM's vertices.
	UINT32					nb_am_conditions;						// Number of conditions in the AM.
	AM_ACTOR_COND_STRUCT	am_conditions[AM_MAX_NB_CONDITIONS];	// Array of the AM's conditions.
}MSG_CREATE_TASK_STRUCT;


typedef struct msg_create_fifo_struct	// Structure of a create FIFO message.
{
	UINT32 	id;
	UINT32	size;
	UINT32	mem_block;				// Memory block where the FIFO will be created.
	UINT32	block_ix;				// Index of each FIFO in a single memory block.
	UINT32	direction;				// Input : 0, Output : 1.
	BOOL	init;					// If TRUE, the FIFO's indices are cleared, if false they are updated from the FIFO's registers.
}MSG_CREATE_FIFO_STRUCT;


typedef struct lrt_msg														// Message's structure
{
	UINT32	msg_type;
	UINT32 	task_id;
	UINT32	function_id;
	UINT32	nb_args;				// Number of arguments to be passed to the function.
	UINT32	args[MAX_NB_ARGS];		// Array of arguments to be passed to the function.
	UINT32	fifo_id;
	UINT32	fifo_size;
	UINT32	fifo_direction;			// Input : 0, Output : 1.
	UINT32	nb_fifo_in;				// Number of input FIFOs.
	UINT32	nb_fifo_out;			// Number of output FIFOs.
	UINT32	fifo_in[MAX_NB_FIFO];	// Array of input FIFO ids.
	UINT32	fifo_out[MAX_NB_FIFO];	// Array of output FIFO ids.
}LRT_MSG;

/*$PAGE*/
/*
*********************************************************************************************************
*                                            EXTERN DECLARATIONS
*********************************************************************************************************
*/
//extern void print(char*);


#endif
