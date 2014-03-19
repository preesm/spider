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

#include <stdlib.h>
#include "lrt_definitions.h"
#include "lrt_cfg.h"
#include "lrt_taskMngr.h"
#include "lrt_monitor.h"
#include "lrt_fifoMngr.h"
#include "lrt_core.h"
#include "lrt_debug.h"
#include "hwQueues.h"


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



/*
 * Creates an Actor Machine.
 */
void AMCreate(ActorMachine *am){
	UINT8 i;
	UINT8 j;
	AM_VERTEX_STRUCT		*am_vertex_ptr;
	AM_ACTOR_COND_STRUCT	*am_cond_ptr;
	AM_ACTOR_ACTION_STRUCT	*am_action_ptr;

	if(am->nbVertices > AM_MAX_NB_VERTICES)
		exitWithCode(1004);
	if(am->nbConds > AM_MAX_NB_CONDITIONS)
		exitWithCode(1005);
	if(am->nbActions > AM_MAX_NB_ACTIONS)
		exitWithCode(1006);
	if(am->currVertexId > am->nbVertices)
		exitWithCode(1007);


	// Popping vertices.
	for(i=0; i < am->nbVertices; i++){
		am_vertex_ptr = &(am->am_vertices[i]);
		am_vertex_ptr->type 			= RTQueuePop_UINT32(RTCtrlQueue);
		am_vertex_ptr->successor_ix[0] 	= RTQueuePop_UINT32(RTCtrlQueue);
		am_vertex_ptr->successor_ix[1] 	= RTQueuePop_UINT32(RTCtrlQueue);
		am_vertex_ptr->actionID 		= RTQueuePop_UINT32(RTCtrlQueue);
	}

	// Popping conditions.
	for (i = 0; i < am->nbConds; i++) {
		am_cond_ptr = &(am->am_conditions[i]);
		am_cond_ptr->type 		= RTQueuePop_UINT32(RTCtrlQueue);
		am_cond_ptr->fifo.id	= RTQueuePop_UINT32(RTCtrlQueue);
		am_cond_ptr->fifo.size 	= RTQueuePop_UINT32(RTCtrlQueue);
	}

	// Popping actions.
	for (i = 0; i < am->nbActions; i++) {
		am_action_ptr = &(am->am_actions[i]);
		am_action_ptr->functionID	= RTQueuePop_UINT32(RTCtrlQueue);
		am_action_ptr->nb_fifo_in 	= RTQueuePop_UINT32(RTCtrlQueue);
		am_action_ptr->nb_fifo_out	= RTQueuePop_UINT32(RTCtrlQueue);
		am_action_ptr->nb_param		= RTQueuePop_UINT32(RTCtrlQueue);

		// todo verify non null function
		if(am_action_ptr->functionID > NB_LOCAL_FUNCTIONS)
			exitWithCode(1008);
		if(am_action_ptr->nb_fifo_in > MAX_NB_FIFO)
			exitWithCode(1009);
		if(am_action_ptr->nb_fifo_out > MAX_NB_FIFO)
			exitWithCode(1010);
		if(am_action_ptr->nb_param > MAX_NB_PARAMETERS)
			exitWithCode(1011);

		for (j = 0; j < am_action_ptr->nb_fifo_in; j++) {
			am_action_ptr->inputFifoIds[j] = RTQueuePop_UINT32(RTCtrlQueue);
			am_action_ptr->inputFifoDataOffs[j] = RTQueuePop_UINT32(RTCtrlQueue);
			// TODO: get the FIFO' size
		}
		for (j = 0; j < am_action_ptr->nb_fifo_out; j++) {
			am_action_ptr->outputFifoIds[j] = RTQueuePop_UINT32(RTCtrlQueue);
			am_action_ptr->outputFifoDataOffs[j] = RTQueuePop_UINT32(RTCtrlQueue);
			// TODO: get the FIFO' size
		}
		for (j = 0; j < am_action_ptr->nb_param; j++) {
			am_action_ptr->params[j] = RTQueuePop_UINT32(RTCtrlQueue);
		}
	}

}


/* Function corresponding to the Test vertex of a AM Graph */
void am_funct_test(){
	BOOLEAN test=0;
	AM_VERTEX_STRUCT *vertex_ptr = &(OSTCBCur->am.am_vertices[OSTCBCur->am.currVertexId]);
	AM_ACTOR_COND_STRUCT *actor_cond = &OSTCBCur->am.am_conditions[vertex_ptr->condID];

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
	OSTCBCur->am.currVertexId = vertex_ptr->successor_ix[test?1:0];
}

/* Function corresponding to the Execution vertex of a AM Graph */
void am_funct_exec(){
	AM_VERTEX_STRUCT *vertex_ptr = &OSTCBCur->am.am_vertices[OSTCBCur->am.currVertexId];

	/* Execute the corresponding Action */
	functions_tbl[OSTCBCur->am.am_actions[vertex_ptr->actionID].functionID](
			OSTCBCur->am.am_actions[vertex_ptr->actionID].inputFifoIds,
			OSTCBCur->am.am_actions[vertex_ptr->actionID].inputFifoDataOffs,
			OSTCBCur->am.am_actions[vertex_ptr->actionID].outputFifoIds,
			OSTCBCur->am.am_actions[vertex_ptr->actionID].outputFifoDataOffs,
			OSTCBCur->am.am_actions[vertex_ptr->actionID].params);

	/* Step to the next vertex of AM graph */
	OSTCBCur->am.currVertexId = vertex_ptr->successor_ix[0];
}

/* Function corresponding to the State vertex of a AM Graph */
void am_funct_move(){
	/* Step to the next vertex of AM graph */
	OSTCBCur->am.currVertexId = OSTCBCur->am.am_vertices[OSTCBCur->am.currVertexId].successor_ix[0];
}

/* Function corresponding to the Wait vertex of a AM Graph */
void am_funct_wait(){
	MonitorAction Act;
	/* Step to the next vertex of AM graph */
	OSTCBCur->am.currVertexId = OSTCBCur->am.am_vertices[OSTCBCur->am.currVertexId].successor_ix[0];

	/* Re-schedule between each AM Graph */
	Act = switchMonitor(Scheduling);
	LRTSched();
	switchMonitor(Act);
}


/*
 * This is the function of a task that contains an Actor Machine.
 */
void amTaskStart() {
	MonitorAction Act = switchMonitor(AMManagement);
	// Calling the function of the current AM vertex.
	am_funct[OSTCBCur->am.am_vertices[OSTCBCur->am.currVertexId].type]();

//	if(OSTCBCur->stop && (OSTCBCur->stopState == -1 || OSTCBCur->stopState == OSTCBCur->am.currVertexId)){
//		int temp = OSTCBCur->am.currVertexId;
//		LrtTaskDeleteCur();
//		OS_CtrlQPush(&temp, sizeof(int));
//	}
	switchMonitor(Act);
}

