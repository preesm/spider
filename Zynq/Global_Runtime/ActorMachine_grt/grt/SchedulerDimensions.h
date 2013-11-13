
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

#ifndef SCHEDULER_DIMENSIONS
#define SCHEDULER_DIMENSIONS

/**
 * Defines used to statically allocate the scheduler objects
 * 
 * @author mpelcat
 */

// Architecture
#define MAX_SLAVE_TYPES 2 // The maximum number of slave types
#define MAX_SLAVES 12// The maximum number of slaves
#define MAX_SLAVE_NAME_SIZE 10// The maximum number of slaves

// Common to all types of graphs
#define MAX_VERTEX_NAME_SIZE 32
#define MAX_PARAM_NAME_SIZE 32
#define MAX_NB_VERTICES			512
#define MAX_NB_EDGES			512
#define MAX_NB_OUTPUT_EDGES		512
#define MAX_NB_INPUT_EDGES		512

// CSDAG
#define MAX_ARG_EXPRESSION 64
#define MAX_CSDAG_ARGS 64
#define MAX_CSDAG_VERTICES 32
#define MAX_CSDAG_EDGES 32
#define MAX_CSDAG_INPUT_EDGES 32 // Maximum of input edges for a given DAG vertex

#define MAX_CSDAG_VERTEX_REPETITION 100 // The maximum number of repetitions for one vertex
#define MAX_CSDAG_PATTERN_TABLE_SIZE 2100 // Maximum size of the whole table containing the patterns of one CSDAG graph
#define MAX_CSDAG_PATTERN_SIZE 100 // Maximum size of one integer pattern (in number of integers)

// Single Rate DAG
#define MAX_SRDAG_VERTICES 		512
#define MAX_SRDAG_EDGES 		512
#define MAX_SRDAG_INPUT_EDGES 	64
#define MAX_SRDAG_OUTPUT_EDGES 	100
#define MAX_VERTEX_REPETITION 	100 // The maximum number of repetitions for one vertex

// PICSDF
//#define MAX_PISDF_CONFIG_PORTS		512
//#define MAX_NB_CONFIG_PARAMS		10	// Maximum of parameters a configuration actor can set.


// PiSDF
#define MAX_NB_PiSDF_SUB_GRAPHS			32
#define MAX_NB_PiSDF_EDGES				32
#define MAX_NB_PiSDF_PARAMS				32
#define MAX_NB_PiSDF_VERTICES			32
#define MAX_NB_PiSDF_CONFIG_VERTICES	32
#define MAX_NB_PiSDF_JOIN_VERTICES		32
#define MAX_NB_PiSDF_INPUT_VERTICES		32
#define MAX_NB_PiSDF_BROAD_VERTICES		32
#define MAX_NB_PiSDF_OUTPUT_VERTICES	32
#define MAX_NB_PiSDF_SWITCH_VERTICES	32
#define MAX_NB_PiSDF_SELECT_VERTICES	32
#define MAX_NB_PiSDF_CONFIG_VERTEX_PARAMS	10

// DAG
#define MAX_DAG_VERTEX_REPETITION 100 // The maximum number of repetitions for one vertex

// XParser
#define VARIABLE_NAME_LEN_MAX 30
#define MAX_VARIABLE_NUMBER 300
#define EXPR_LEN_MAX 128
#define REVERSE_POLISH_STACK_MAX_ELEMENTS 20 // Max number of elements in an expression reverse polish stack

#define MAX_PARAM 4 // Maximum number of parameters for an actor

// AM

#define AM_VERTEX_MAX_SUCCESSOR 2 //10 //2
#define AM_GRAPH_MAX_VERTEX		300
#define AM_GRAPH_MAX_COND		300
#define AM_GRAPH_MAX_ACTIONS	100
#define AM_ACTION_NAME 100

// Platform
#define NB_MAX_CTRLQ 			8
#define MAX_NB_FIFO 			300
//#define DEFAULT_FIFO_SIZE		1024
#define DEFAULT_FIFO_SIZE		2057478
#define FILE_PATH_LENGTH		50
#define MAX_NB_AM				10

#endif
