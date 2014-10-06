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

// File names for graph's printing
#ifdef RELEASE

#define EXEC							1
#define PRINT_GRAPH						0
#define	STAT							0
#define PRINT_REAL_GANTT				1
#define DEBUG							0
#define SEE_PARAM_VAL					0

#else

#define EXEC							1
#define PRINT_GRAPH						1
#define	STAT							1
#define PRINT_REAL_GANTT				1
#define DEBUG							1
#define SEE_PARAM_VAL					1

#endif

#define USE_AM							0

#define ITER_MAX 1
#define PERIOD 600000

#define PiSDF_FILE_PATH					"pisdf"
#define SUB_SRDAG_FIFO_ID_FILE_PATH		"subSrDagFifoId"
#define SUB_SDF_FILE_0_PATH				"subSDF"
#define SRDAG_FILE_PATH					"srDag"
#define PRE_SRDAG_FILE_NAME				"preSrDag"
#define SUB_SRDAG_FILE_NAME				"subSrDag"
#define POST_SRDAG_FILE_NAME			"postSrDag"
#define SRDAG_FIFO_ID_FILE_PATH			"srDagFifoId"
#define SCHED_FILE_NAME					"test"

// Architecture
#define MAX_SLAVE_TYPES 		1	//2 	// The maximum number of slave types
#define MAX_SLAVES 				9	//12	// The maximum number of slaves
#define MAX_SLAVE_NAME_SIZE 	10	// The maximum number of slaves

#define MAPPING_TIME 			500
#define SYNC_TIME 				50

#define MAX_VERTEX_NAME_SIZE 	50
#define MAX_TOOL_NAME			50

#ifdef DSP
	#define MAX_MASTER_ACTORS		100
#else
	#define MAX_MASTER_ACTORS		1000
#endif

#define MEMCPY_SPEED 			1.8
#define NB_LOCAL_FUNCTIONS 		30

// Single Rate DAG

#define MAX_SRDAG_VERTICES 		10000 // 400
#define MAX_SRDAG_EDGES 		10000 // 600
#define MAX_SRDAG_BROADCASTS	10000 // 25
#define MAX_SRDAG_RBS			10000 // 35
#define MAX_SRDAG_IMPLODES		10000 // 25

#define MAX_SRDAG_IO_EDGES		1000
#define MAX_SRDAG_XPLODE_EDGES	1000
#define MAX_EDGE_ARRAY 			1000000
#define MAX_PARAM_ARRAY 		100

#define MAX_VERTEX_REPETITION 	800	//100 // The maximum number of repetitions for one vertex
#define MAX_XPL_IN_STEPS		100

#define RB_FUNCT_IX				20
#define BROADCAST_FUNCT_IX		21
#define SWICTH_FUNCT_IX			22
#define XPLODE_FUNCT_IX			23
#define INIT_FUNCT_IX			24
#define END_FUNCT_IX			25


// PiSDF
#define MAX_NB_PiSDF_GRAPHS				4	//32
#define MAX_NB_PiSDF_EDGES				100	//32
#define MAX_NB_PiSDF_PARAMS				20	//32
#define MAX_NB_PiSDF_VERTICES			40	//32
#define MAX_NB_PiSDF_CONFIG_VERTICES	2	//32
#define MAX_NB_PiSDF_JOIN_VERTICES		10	//32
#define MAX_NB_PiSDF_INPUT_VERTICES		10	//32
#define MAX_NB_PiSDF_INPUT_EDGES		10	//32
#define MAX_NB_PiSDF_OUTPUT_EDGES		10	//32
#define MAX_NB_PiSDF_BROAD_VERTICES		10	//32
#define MAX_NB_PiSDF_OUTPUT_VERTICES	10	//32
#define MAX_NB_PiSDF_SWITCH_VERTICES	10	//32
#define MAX_NB_PiSDF_SELECT_VERTICES	10	//32
#define MAX_NB_PiSDF_ROUNDB_VERTICES	10	//32
#define MAX_NB_PiSDF_CONFIG_VERTEX_PARAMS	10	//10
#define MAX_NB_PiSDF_VERTEX_NAME		50	//10
#define MAX_NB_PiSDF_PARAM_NAME			50	//10

// XParser
#define VARIABLE_NAME_LEN_MAX 100
#define MAX_VARIABLE_NUMBER 300
#define EXPR_LEN_MAX 128
#define REVERSE_POLISH_STACK_MAX_ELEMENTS 20 // Max number of elements in an expression reverse polish stack

#define MAX_PARAM 10 // Maximum number of parameters for an actor

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
#define MAX_NB_ARGS		100


/**************** MISCELLANEOUS ************************/

typedef void (*FUNCTION_TYPE)(UINT8* inputFIFOs[],
							  UINT8* outputFIFOs[],
							  UINT32 params[]);// Function of a task/vertex.


#endif
