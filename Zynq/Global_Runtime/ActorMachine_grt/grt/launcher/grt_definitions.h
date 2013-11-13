
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/

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
#define TRUE				1

/**************** Data types ******************/
typedef unsigned int 		UINT32;

typedef int					INT32;

typedef unsigned char		UINT8;

typedef char				INT8S;

typedef unsigned char		BOOLEAN;


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
	BOOLEAN				value;
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
	BOOLEAN init;					// If TRUE, the FIFO's indices are cleared, if false they are updated from the FIFO's registers.
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
extern void print(char*);


#endif
