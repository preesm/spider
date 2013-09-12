/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
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

// Coomon to all types of graphs
#define MAX_VERTEX_NAME_SIZE 32

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
#define MAX_SRDAG_VERTICES 512
#define MAX_SRDAG_EDGES 512
#define MAX_SRDAG_INPUT_EDGES 64
#define MAX_SRDAG_OUTPUT_EDGES 100

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
#define NB_MAX_CTRLQ 8
#define MAX_NB_FIFO 300

#endif
