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
#include <debuggingOptions.h>

// Architecture
#define MAX_SLAVE_TYPES 		1	//2 	// The maximum number of slave types
#define MAX_SLAVES 				8	//12	// The maximum number of slaves
#define MAX_SLAVE_NAME_SIZE 	10	// The maximum number of slaves

#define MAPPING_TIME 			500
#define SYNC_TIME 				50

#define MAX_VERTEX_NAME_SIZE 	50
#define MAX_TOOL_NAME			50

#ifdef DSP
	#define MAX_MASTER_ACTORS		100
#else
	#define MAX_MASTER_ACTORS		400
#endif

#define MEMCPY_SPEED 			1.8
#define NB_LOCAL_FUNCTIONS 		20
#define OS_MAX_TASKS_TIME 		350

// Single Rate DAG
#define EDGE_POOL_SIZE			360 // 600
#define NORMAL_POOL_SIZE		140 // 275
#define BROADCAST_POOL_SIZE		75 // 25
#define CONFIG_POOL_SIZE		10 // 20
#define INITEND_POOL_SIZE		10 // 25
#define RB_POOL_SIZE			20 // 35
#define XPLODE_POOL_SIZE		15 // 25

#define MAX_SRDAG_VERTICES 		430 // 400
#define MAX_SRDAG_EDGES 		310 // 600
#define MAX_SRDAG_IO_EDGES		3
#define MAX_SRDAG_XPLODE_EDGES	16
#define MAX_EDGE_ARRAY 			10000

#define MAX_VERTEX_REPETITION 	17	//100 // The maximum number of repetitions for one vertex

#define RB_FUNCT_IX				10
#define BROADCAST_FUNCT_IX		11
#define SWICTH_FUNCT_IX			12
#define XPLODE_FUNCT_IX			13
#define INIT_FUNCT_IX			14
#define END_FUNCT_IX			15

#define MAX_XPL_IN_STEPS		17

// PiSDF
#define MAX_NB_PiSDF_GRAPHS				3	//32
#define MAX_NB_PiSDF_EDGES				16	//32
#define MAX_NB_PiSDF_PARAMS				8	//32
#define MAX_NB_PiSDF_VERTICES			8	//32
#define MAX_NB_PiSDF_CONFIG_VERTICES	2	//32
#define MAX_NB_PiSDF_JOIN_VERTICES		1	//32
#define MAX_NB_PiSDF_INPUT_VERTICES		2	//32
#define MAX_NB_PiSDF_INPUT_EDGES		3	//32
#define MAX_NB_PiSDF_OUTPUT_EDGES		3	//32
#define MAX_NB_PiSDF_BROAD_VERTICES		1	//32
#define MAX_NB_PiSDF_OUTPUT_VERTICES	2	//32
#define MAX_NB_PiSDF_SWITCH_VERTICES	1	//32
#define MAX_NB_PiSDF_SELECT_VERTICES	1	//32
#define MAX_NB_PiSDF_ROUNDB_VERTICES	1	//32
#define MAX_NB_PiSDF_CONFIG_VERTEX_PARAMS	2	//10
#define MAX_NB_PiSDF_VERTEX_NAME	30	//10
#define MAX_NB_PiSDF_PARAM_NAME	30	//10

// XParser
#define VARIABLE_NAME_LEN_MAX 30
#define MAX_VARIABLE_NUMBER 300
#define EXPR_LEN_MAX 128
#define REVERSE_POLISH_STACK_MAX_ELEMENTS 20 // Max number of elements in an expression reverse polish stack

#define MAX_PARAM 4 // Maximum number of parameters for an actor

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
