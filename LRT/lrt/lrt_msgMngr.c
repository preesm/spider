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
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 									*
 * 																			*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.			*
 ****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <platform_types.h>
#include <platform_data_queue.h>
#include <platform_queue.h>
#include <platform_time.h>
#include <platform.h>

#include "lrt_taskMngr.h"
#include "lrt_msgMngr.h"
#include "lrt_core.h"

extern void amTaskStart();

/* Waits for an external message from the Global Runtime and execute the current task */
void wait_ext_msg() {
	UINT32 msg_type;
	OS_TCB *new_tcb;

	/* Popping the first incoming word i.e. the message type. */
	if (platform_queue_NBPop_UINT32(PlatformCtrlQueue, &msg_type)) {
		switch (msg_type) {
		case MSG_CREATE_TASK:
			// TODO: Reuse a previous TCB...
			new_tcb = LrtTaskCreate();

			// Popping the task function id.
			new_tcb->functionId = platform_queue_pop_UINT32(PlatformCtrlQueue);

			// Popping the id of the vx in the current SrDAG on the GRT.
			new_tcb->vertexId = platform_queue_pop_UINT32(PlatformCtrlQueue);

			// Popping whether the task is stopped after completion.
	//		new_tcb->stop = RTQueuePop_UINT32(RTCtrlQueue);

			// Popping the AM flag.
			new_tcb->isAM = platform_queue_pop_UINT32(PlatformCtrlQueue);

			if(new_tcb->isAM){
				// Creating an actor machine.
				// Popping the actor machine's info.
				new_tcb->am.nbVertices 	= platform_queue_pop_UINT32(PlatformCtrlQueue);
				new_tcb->am.nbConds 	= platform_queue_pop_UINT32(PlatformCtrlQueue);
				new_tcb->am.nbActions	= platform_queue_pop_UINT32(PlatformCtrlQueue);

				// Popping the starting vertex of the AM.
				new_tcb->am.currVertexId = platform_queue_pop_UINT32(PlatformCtrlQueue);
				new_tcb->task_func = amTaskStart; // An AM task's function is predefined.
				new_tcb->stop = FALSE;
				// Creating the AM.
				AMCreate(&(new_tcb->am));
			}
			else
			{
				// Creating a single actor.
				new_tcb->actor = &LRTActorTbl[new_tcb->OSTCBId];

				new_tcb->actor->nbInputFifos = platform_queue_pop_UINT32(PlatformCtrlQueue);
				new_tcb->actor->nbOutputFifos = platform_queue_pop_UINT32(PlatformCtrlQueue);
				new_tcb->actor->nbParams = platform_queue_pop_UINT32(PlatformCtrlQueue);

//				for (i = 0; i < new_tcb->actor->nbInputFifos; i++)
//					new_tcb->actor->inputFifoId[i] = platform_queue_pop_UINT32(PlatformCtrlQueue);
				platform_queue_pop(PlatformCtrlQueue, new_tcb->actor->inputFifoId, new_tcb->actor->nbInputFifos*sizeof(UINT32));

//				for (i = 0; i < new_tcb->actor->nbInputFifos; i++)
//					new_tcb->actor->inputFifoDataOff[i] = platform_queue_pop_UINT32(PlatformCtrlQueue);
				platform_queue_pop(PlatformCtrlQueue, new_tcb->actor->inputFifoDataOff, new_tcb->actor->nbInputFifos*sizeof(UINT32));

				platform_queue_pop(PlatformCtrlQueue, new_tcb->actor->inputFifoDataSize, new_tcb->actor->nbInputFifos*sizeof(UINT32));


//				for (i = 0; i < new_tcb->actor->nbOutputFifos; i++)
//					new_tcb->actor->outputFifoId[i] = platform_queue_pop_UINT32(PlatformCtrlQueue);
				platform_queue_pop(PlatformCtrlQueue, new_tcb->actor->outputFifoId, new_tcb->actor->nbOutputFifos*sizeof(UINT32));

//				for (i = 0; i < new_tcb->actor->nbOutputFifos; i++)
//					new_tcb->actor->outputFifoDataOff[i] = platform_queue_pop_UINT32(PlatformCtrlQueue);
				platform_queue_pop(PlatformCtrlQueue, new_tcb->actor->outputFifoDataOff, new_tcb->actor->nbOutputFifos*sizeof(UINT32));
				platform_queue_pop(PlatformCtrlQueue, new_tcb->actor->outputFifoDataSize, new_tcb->actor->nbOutputFifos*sizeof(UINT32));



				platform_queue_pop(PlatformCtrlQueue, new_tcb->actor->params, new_tcb->actor->nbParams*sizeof(UINT32));

				new_tcb->task_func = functions_tbl[new_tcb->functionId];
				new_tcb->stop = TRUE;

//				printf("New Job:\n");
//				printf("\t id: %d\n", new_tcb->functionId);
//				printf("\t nbFifo in: %d\n", new_tcb->actor->nbInputFifos);
//				printf("\t nbFifo out: %d\n", new_tcb->actor->nbOutputFifos);
//				printf("\t nbParams: %d\n", new_tcb->actor->nbParams);
			}

//			RTQueuePush(RTCtrlQueue, &msg_type, sizeof(UINT32));
			lrt_running = TRUE;
			break;

		case MSG_CLEAR_TIME:
			platform_time_reset();
			break;

		case MSG_SEND_INFO_DATA:
			sendExecData();
			break;

		default:
			platform_puts("Bad msg Received\n");
			break;
		}
	}
}

