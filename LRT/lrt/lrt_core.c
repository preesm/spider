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

#include <stdlib.h>
#include <string.h>

#include "lrt_cfg.h"
#include "lrt_monitor.h"
#include "lrt_msgMngr.h"
#include "lrt_taskMngr.h"

#include <platform.h>
#include <platform_queue.h>
#include <platform_data_queue.h>
#include <platform_print.h>
#include <platform_gpio.h>
#include <platform_time.h>

#include <memoryAlloc.h>

#include "lrt_core.h"

/* GLOBAL VARIABLES */
OS_TCB 		*OSTCBCur;               	/* Pointer to currently scheduled TCB. 		*/
OS_TCB 		OSTCBTbl[OS_MAX_TASKS];		/* Table of TCBs							*/
BOOL 		lrt_running; 				/* Flag indicating that kernel is running   */

void mainLoop(){
	wait_ext_msg();

	if (lrt_running && getSw()){
		LRTStartCurrTask();
	}
}

/* Initialize the Runtime */
void LRTInit() {
	OSTCBCur = (OS_TCB *) 0;
	memset(OSTCBTbl, 0, sizeof(OSTCBTbl)); /* Clear all the TCBs */

	Monitor_init();
	OSWorkingMemoryInit();

	lrt_running = FALSE;
}


/*
 * Initialize control queues for communications with the GRT.
 */
void LRTCtrlStart(){
	while (TRUE)
		mainLoop();
}


/*
 * Launch the Runtime.
 */
void LRTStart(){
	while(OSTCBCur)
		LRTStartCurrTask();
}

static UINT8* inputFIFOs[MAX_NB_FIFO];
static UINT8* outputFIFOs[MAX_NB_FIFO];

/* Starts the task pointed by the OSTCBCur. */
void LRTStartCurrTask() {
	// Executes the vertex's code.
#if USE_AM
	OSTCBCur->task_func(0, 0, 0, 0, 0); // Fifos are not required when calling the AM default function.
#else
	int i;
	for(i=0;i<OSTCBCur->actor.nbInputFifos; i++){
		platform_readFifo(OSTCBCur->actor.inputFifo[i].id,
				OSTCBCur->actor.inputFifo[i].add,
				OSTCBCur->actor.inputFifo[i].size,
				0);
		inputFIFOs[i] = (UINT8*)(SHARED_MEM_BASE + OSTCBCur->actor.inputFifo[i].add);
	}
	for(i=0;i<OSTCBCur->actor.nbOutputFifos; i++){
		outputFIFOs[i] = (UINT8*)(SHARED_MEM_BASE + OSTCBCur->actor.outputFifo[i].add);
	}
	Monitor_startTask(OSTCBCur->vertexId);
	OSTCBCur->task_func(inputFIFOs,
						outputFIFOs,
						OSTCBCur->actor.params);
	Monitor_endTask();
	for(i=0;i<OSTCBCur->actor.nbOutputFifos; i++){
		platform_writeFifo(OSTCBCur->actor.outputFifo[i].id,
				OSTCBCur->actor.outputFifo[i].add,
				OSTCBCur->actor.outputFifo[i].size,
				0);
	}
	OSFreeWorkingMemory();
#endif

	if(OSTCBCur->stop)
		LrtTaskDeleteCur();
}

/* Determine the next Task to run */
void LRTSched() {
	/* Round robin Scheduler between Tasks */
	if (OSTCBCur != (OS_TCB*) 0)
		OSTCBCur = OSTCBCur->OSTCBNext;
	else
		/* No Available Tasks : stop the LRT */
		lrt_running = FALSE;
}


OS_TCB* getCurrTask(){
	return OSTCBCur;
}

