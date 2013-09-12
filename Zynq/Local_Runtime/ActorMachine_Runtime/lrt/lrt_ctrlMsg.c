/*
 * lrt_ctrlMsg.c
 *
 *  Created on: 26 oct. 2012
 *      Author: yoliva jheulot
 */

#include <string.h>

#include "lrt_prototypes.h"
#include <types.h>
#include <hwQueues.h>
#include <platform.h>
#include <print.h>

/* GLOBAL VARIABLES */
FUNCTION_TYPE functions_tbl[NB_LOCAL_FUNCTIONS]; /* Table of Action Fcts */

/* Send a Message to the Global Runtime */
void send_ext_msg(UINT32 addr, UINT32 msg_type, void* msg) {
	UINT32 msg_size = 0;

	// Sending the type of message as the first data word.
	OS_CtrlQPush(&msg_type, sizeof(UINT32));

	// Determining the size of the rest of data.
	switch (msg_type) {
	case MSG_CURR_VERTEX_ID:
		msg_size = sizeof(UINT32);
		break;
	default:
		break;
	}

	// Sending the rest of data.
	if (msg_size > 0)
		OS_CtrlQPush(msg, msg_size);
}

/* Waits for an external message from the Global Runtime and execute the current task */
void wait_ext_msg() {
	UINT32 msg_type;

	/* Reads the first incoming word i.e. the message type. */
	if (OS_CtrlQPop_nonBlocking(&msg_type, sizeof(UINT32)) == sizeof(UINT32)) {
		switch (msg_type) {
		case MSG_CREATE_TASK:
			LrtTaskCreate();
			OS_CtrlQPush(&msg_type, sizeof(UINT32));
			break;

		case MSG_STOP_TASK:
			OSTaskDel();
			break;

		case MSG_CREATE_FIFO:
			create_fifo();
//			OS_CtrlQPush(&msg_type, sizeof(UINT32));
			break;

		case MSG_CLEAR_FIFO:
			flush_fifo();
//			OS_CtrlQPush(&msg_type, sizeof(UINT32));
			break;

		case MSG_START_SCHED:
			lrt_running = TRUE;
			zynq_puts("Start Schedule\n");
//			OS_CtrlQPush(&msg_type, sizeof(UINT32));
			break;

		default:
			zynq_puts("Bad msg Received\n");
			break;
		}
	}
}

