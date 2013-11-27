/*
 * lrt_am.c
 *
 *  Created on: Jul 25, 2013
 *      Author: yoliva jheulot
 */


#include <stdlib.h>
#include "lrt_definitions.h"
#include "lrt_cfg.h"
#include "lrt_prototypes.h"


void am_funct_test();
void am_funct_exec();
void am_funct_wait();
void am_funct_move();

/* Function table corresponding to each AM vertex types */
void (*am_funct[5]) (void) = {
		NULL,
		&am_funct_wait,
		&am_funct_move,
		&am_funct_test,
		&am_funct_exec
};


/* Function corresponding to the Test vertex of a AM Graph */
void am_funct_test(){
	BOOLEAN test=0;
	AM_VERTEX_STRUCT *vertex_ptr = &(OSTCBCur->am_vertices[OSTCBCur->current_vertexID]);
	AM_ACTOR_COND_STRUCT *actor_cond = &OSTCBCur->am_conditions[vertex_ptr->condID];

	/* Determine the condition value */
	MonitorAction Act;
	switch (actor_cond->type) {
		case FIFO_OUT:
			Act = switchMonitor(FifoCheck);
			test = check_output_fifo(actor_cond->fifo.id, actor_cond->fifo.size);
			switchMonitor(Act);
			break;
		case FIFO_IN:
			Act = switchMonitor(FifoCheck);
			test = check_input_fifo(actor_cond->fifo.id, actor_cond->fifo.size);
			switchMonitor(Act);
			break;
		default:
			break;
	}

	/* Step to the next vertex of AM graph */
	OSTCBCur->current_vertexID = vertex_ptr->successor_ix[test?1:0];
}

/* Function corresponding to the Execution vertex of a AM Graph */
void am_funct_exec(){
	AM_VERTEX_STRUCT *vertex_ptr = &OSTCBCur->am_vertices[OSTCBCur->current_vertexID];

	/* Execute the corresponding Action */
	functions_tbl[OSTCBCur->am_actions[vertex_ptr->actionID].functionID]();

	/* Step to the next vertex of AM graph */
	OSTCBCur->current_vertexID = vertex_ptr->successor_ix[0];
}

/* Function corresponding to the State vertex of a AM Graph */
void am_funct_move(){
	/* Step to the next vertex of AM graph */
	OSTCBCur->current_vertexID = OSTCBCur->am_vertices[OSTCBCur->current_vertexID].successor_ix[0];
}

/* Function corresponding to the Wait vertex of a AM Graph */
void am_funct_wait(){
	MonitorAction Act;
	/* Step to the next vertex of AM graph */
	OSTCBCur->current_vertexID = OSTCBCur->am_vertices[OSTCBCur->current_vertexID].successor_ix[0];

	/* Re-schedule between each AM Graph */
	Act = switchMonitor(Scheduling);
	OSSched();
	switchMonitor(Act);
}


/*
 * This is the function of a task that contains an Actor Machine.
 */
void amTaskStart() {
	MonitorAction Act = switchMonitor(AMManagement);
	// Calling the function of the current AM vertex.
	am_funct[OSTCBCur->am_vertices[OSTCBCur->current_vertexID].type]();

//	if(OSTCBCur->stop && (OSTCBCur->stopState == -1 || OSTCBCur->stopState == OSTCBCur->current_vertexID)){
//		int temp = OSTCBCur->current_vertexID;
//		LrtTaskDeleteCur();
//		OS_CtrlQPush(&temp, sizeof(int));
//	}
	switchMonitor(Act);
}

