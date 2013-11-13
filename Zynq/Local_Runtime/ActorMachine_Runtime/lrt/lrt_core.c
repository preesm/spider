/*
 * lrt_core.c
 *
 *  Created on: 31 oct. 2012
 *      Author: yoliva
 */

#include <stdlib.h>
#include <string.h>

#include "lrt_definitions.h"
#include "lrt_cfg.h"
#include "lrt_prototypes.h"

#include <platform.h>
#include <hwQueues.h>
#include <sharedMem.h>
#include <print.h>
#include <gpio.h>

/* GLOBAL VARIABLES */
OS_TCB *OSTCBCur;               /* Pointer to currently scheduled TCB. 		*/
OS_TCB OSTCBTbl[OS_MAX_TASKS];	/* Table of TCBs							*/
BOOLEAN lrt_running; 			/* Flag indicating that kernel is running   */

inline void mainLoop(){
	switchMonitor(CtrlFifoHandling);
	wait_ext_msg();

	switchMonitor(Default);
	if (lrt_running && getSw()){
#ifdef ARM
//		zynq_puts("vertex ");zynq_putdec(OSTCBCur->current_vertexID);zynq_puts("\n");
#endif

		if(OSTCBCur->current_vertexID == OSTCBCur->nbVertices-1){
//			printResult();
//			resetMonitor();
		}

		OSStartCur();
	}
}

/* Initialize and Launch the Runtime */
void init_lrt() {
	OSTCBCur = (OS_TCB *) 0;
	memset(OSTCBTbl, 0, sizeof(OSTCBTbl)); /* Clear all the TCBs */
	OS_ShMemInit();
	OS_QInit();
	initMonitor();
	OSWorkingMemoryInit();

	lrt_running = FALSE;

	while (TRUE)
		mainLoop();
}



/* Starts the task pointed by the OSTCBCur. */
void OSStartCur() {
	// Executes the vertex's code.

	MonitorAction Act = switchMonitor(AMManagement);
	OSTCBCur->task_func();
//	am_funct[OSTCBCur->am_vertices[OSTCBCur->current_vertexID].type]();

	if(OSTCBCur->stop && (OSTCBCur->stopState == -1 || OSTCBCur->stopState == OSTCBCur->current_vertexID)){
//		int temp = OSTCBCur->current_vertexID;
		LrtTaskDeleteCur();
//		OS_CtrlQPush(&temp, sizeof(int));
	}
	switchMonitor(Act);
}

/* Determine the next Task to run */
void OSSched() {
	/* Round robin Scheduler between Tasks */
	if (OSTCBCur != (OS_TCB*) 0)
		OSTCBCur = OSTCBCur->OSTCBNext;
	else
		/* No Available Tasks : stop the LRT */
		lrt_running = 0;
}

