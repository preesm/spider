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


/**************** FIFO's******************************/
#define FIFO_IN_DIR		0
#define FIFO_OUT_DIR	1
#define MAX_NB_FIFO		2


/**************** Data types ******************/

//typedef enum {create_action = 1, create_sh_mem = 2, start_task = 3} EXT_MSG_TYPE;

//typedef void (*FUNCTION_TYPE)(void);

typedef struct create_task_msg			// Structure of a create task message.
{
	unsigned int	msg_size;
	unsigned int 	msg_type;
	unsigned int 	task_id;
	//	FUNCTION_TYPE	funct_addr;
	unsigned int	function_id;
}CREATE_TASK_STRUCT;

typedef struct create_fifo_msg			// Structure of a create FIFO message.
{
	unsigned int	msg_size;
	unsigned int 	msg_type;
	unsigned int 	fifo_id;
	unsigned char 	direction;			// Input : 0, Output : 1.
}CREATE_FIFO_STRUCT;

typedef struct start_sched_msg			// Structure of a start scheduling message.
{
	unsigned int	msg_size;
	unsigned int 	msg_type;
}START_SCHED_MSG;


typedef struct lrt_msg					// Structure for any kind of message.
{
	unsigned int 	msg_type;
	unsigned int 	task_id;
	//	FUNCTION_TYPE	funct_addr;
	unsigned int	function_id;
	unsigned int 	fifo_id;
	unsigned int 	direction;				// Input : 0, Output : 1.
	unsigned int 	nb_fifo_in;				// Number of input FIFOs.
	unsigned int 	nb_fifo_out;			// Number of output FIFOs.
	unsigned int 	fifo_in[MAX_NB_FIFO];	// Array of input FIFO ids.
	unsigned int 	fifo_out[MAX_NB_FIFO];	// Array of output FIFO ids.
}LRT_MSG;





extern void print(char*);


#endif
