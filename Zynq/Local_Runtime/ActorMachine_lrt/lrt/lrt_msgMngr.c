
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

#include <string.h>

#include <types.h>
#include <hwQueues.h>
#include <platform.h>
#include <print.h>
#include "lrt_taskMngr.h"
#include "lrt_1W1RfifoMngr.h"
#include "lrt_msgMngr.h"
#include "lrt_core.h"

extern void amTaskStart();

/* Send a Message to the Global Runtime */
void send_ext_msg(UINT32 addr, UINT32 msg_type, void* msg) {
	UINT32 msg_size = 0;

	// Sending the type of message as the first data word.
	RTQueuePush(RTCtrlQueue, &msg_type, sizeof(UINT32));

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
		RTQueuePush(RTCtrlQueue, msg, msg_size);
}

/* Waits for an external message from the Global Runtime and execute the current task */
void wait_ext_msg() {
	UINT32 msg_type;
	UINT32 fifoID;
	OS_TCB *new_tcb;
	UINT32 i;

	/* Popping the first incoming word i.e. the message type. */
	if (RTQueueNonBlockingPop(RTCtrlQueue, &msg_type, sizeof(UINT32)) == sizeof(UINT32)) {
		switch (msg_type) {
		case MSG_CREATE_TASK:
			// TODO: Reuse a previous TCB...
			new_tcb = LrtTaskCreate();

			// Popping the task function id.
			new_tcb->functionId = RTQueuePop_UINT32(RTCtrlQueue);

			// Popping whether the task is stopped after completion.
	//		new_tcb->stop = RTQueuePop_UINT32(RTCtrlQueue);

			// Popping the AM flag.
			new_tcb->isAM = RTQueuePop_UINT32(RTCtrlQueue);

			if(new_tcb->isAM){
				// Creating an actor machine.
				// Popping the actor machine's info.
				new_tcb->am.nbVertices 	= RTQueuePop_UINT32(RTCtrlQueue);
				new_tcb->am.nbConds 	= RTQueuePop_UINT32(RTCtrlQueue);
				new_tcb->am.nbActions	= RTQueuePop_UINT32(RTCtrlQueue);

				// Popping the starting vertex of the AM.
				new_tcb->am.currVertexId = RTQueuePop_UINT32(RTCtrlQueue);
				new_tcb->task_func = amTaskStart; // An AM task's function is predefined.
				new_tcb->stop = FALSE;
				// Creating the AM.
				AMCreate(&(new_tcb->am));
			}
			else
			{
				// Creating a single actor.
				new_tcb->actor = &LRTActorTbl[new_tcb->OSTCBId];

				new_tcb->actor->nbInputFifos = RTQueuePop_UINT32(RTCtrlQueue);
				new_tcb->actor->nbOutputFifos = RTQueuePop_UINT32(RTCtrlQueue);
				new_tcb->actor->nbParams = RTQueuePop_UINT32(RTCtrlQueue);
				for (i = 0; i < new_tcb->actor->nbInputFifos; i++) {
					new_tcb->actor->inputFifoId[i] = RTQueuePop_UINT32(RTCtrlQueue);
					new_tcb->actor->inputFifoDataOff[i] = RTQueuePop_UINT32(RTCtrlQueue);
					// TODO: get the FIFO' size
				}
				for (i = 0; i < new_tcb->actor->nbOutputFifos; i++) {
					new_tcb->actor->outputFifoId[i] = RTQueuePop_UINT32(RTCtrlQueue);
					new_tcb->actor->outputFifoDataOff[i] = RTQueuePop_UINT32(RTCtrlQueue);
					// TODO: get the FIFO' size
				}
				for ( i = 0; i < new_tcb->actor->nbParams; i++) {
					new_tcb->actor->params[i] = RTQueuePop_UINT32(RTCtrlQueue);
				}
				new_tcb->task_func = functions_tbl[new_tcb->functionId];
				new_tcb->stop = TRUE;
			}

//			RTQueuePush(RTCtrlQueue, &msg_type, sizeof(UINT32));
			break;

		case MSG_STOP_TASK:
			OSTaskDel();
			break;

		case MSG_CREATE_FIFO:
//			create_fifo();
//			OS_CtrlQPush(&msg_type, sizeof(UINT32));
			break;

		case MSG_CLEAR_FIFO:
			fifoID = RTQueuePop_UINT32(RTCtrlQueue);
			flushFIFO(fifoID);
//			RTQueuePush_UINT32(RTCtrlQueue, msg_type);
			break;

		case MSG_START_SCHED:
#if PRINT_ACTOR_IN_DOT_FILE == 1
			PrintTasksIntoDot();
#endif
			lrt_running = TRUE;
//			zynq_puts("Start Schedule\n");
//			OS_CtrlQPush(&msg_type, sizeof(UINT32));
			break;

		default:
			zynq_puts("Bad msg Received\n");
			break;
		}
	}
}

