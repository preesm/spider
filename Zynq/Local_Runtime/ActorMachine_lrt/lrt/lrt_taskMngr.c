/*
 * lrt_taskMngr.c
 *
 *  Created on: Jul 25, 2013
 *      Author: yoliva jheulot
 */

#include "lrt_definitions.h"
#include "lrt_cfg.h"
#include "lrt_prototypes.h"
#include "string.h"
#include <hwQueues.h>
#include <print.h>
#include <platform.h>

/*
*********************************************************************************************************
*                                         GLOBAL DECLARATIONS
*********************************************************************************************************
*/

static UINT8 OSTaskCtr = 0;                     			// Tasks' counter.
static UINT8 OSTaskIndex = 0;                     			// Tasks' index.

//static UINT8* workingMemory;
static UINT8 workingMemory[WORKING_MEMORY_SIZE];
static UINT8* freeWorkingMemoryPtr;

extern void amTaskStart();

/*
 * Creates an Actor Machine.
 */
void LrtAMCreate(OS_TCB *new_tcb){
	if(new_tcb->nbVertices > AM_MAX_NB_VERTICES)
		exitWithCode(1004);
	if(new_tcb->nbConds > AM_MAX_NB_CONDITIONS)
		exitWithCode(1005);
	if(new_tcb->nbActions > AM_MAX_NB_ACTIONS)
		exitWithCode(1006);
	if(new_tcb->current_vertexID > new_tcb->nbVertices)
		exitWithCode(1007);


	// Popping vertices.
	UINT8 i;
	UINT8 j;
	AM_VERTEX_STRUCT	*am_vertex_ptr;
	for(i=0; i < new_tcb->nbVertices; i++){
		am_vertex_ptr = &(new_tcb->am_vertices[i]);
		am_vertex_ptr->type 			= OS_CtrlQPop_UINT32();
		am_vertex_ptr->successor_ix[0] 	= OS_CtrlQPop_UINT32();
		am_vertex_ptr->successor_ix[1] 	= OS_CtrlQPop_UINT32();
		am_vertex_ptr->actionID 		= OS_CtrlQPop_UINT32();
	}

	// Popping conditions.
	AM_ACTOR_COND_STRUCT	*am_cond_ptr;
	for (i = 0; i < new_tcb->nbConds; i++) {
		am_cond_ptr = &(new_tcb->am_conditions[i]);
		am_cond_ptr->type 		= OS_CtrlQPop_UINT32();
		am_cond_ptr->fifo.id	= OS_CtrlQPop_UINT32();
		am_cond_ptr->fifo.size 	= OS_CtrlQPop_UINT32();
	}

	// Popping actions.
	AM_ACTOR_ACTION_STRUCT	*am_action_ptr;
	for (i = 0; i < new_tcb->nbActions; i++) {
		am_action_ptr = &(new_tcb->am_actions[i]);
		am_action_ptr->functionID	= OS_CtrlQPop_UINT32();
		am_action_ptr->nb_fifo_in 	= OS_CtrlQPop_UINT32();
		am_action_ptr->nb_fifo_out	= OS_CtrlQPop_UINT32();
		am_action_ptr->nb_param		= OS_CtrlQPop_UINT32();

		// todo verify non null function
		if(am_action_ptr->functionID > NB_LOCAL_FUNCTIONS)
			exitWithCode(1008);
		if(am_action_ptr->nb_fifo_in > MAX_NB_FIFO)
			exitWithCode(1009);
		if(am_action_ptr->nb_fifo_out > MAX_NB_FIFO)
			exitWithCode(1010);
		if(am_action_ptr->nb_param > MAX_NB_ARGS)
			exitWithCode(1011);

		for (j = 0; j < am_action_ptr->nb_fifo_in; j++) {
			am_action_ptr->fifo_in_id[j] = OS_CtrlQPop_UINT32();
		}
		for (j = 0; j < am_action_ptr->nb_fifo_out; j++) {
			am_action_ptr->fifo_out_id[j] = OS_CtrlQPop_UINT32();
		}
		for (j = 0; j < am_action_ptr->nb_param; j++) {
			am_action_ptr->param_value[j] = OS_CtrlQPop_UINT32();
		}
	}

}

/*
*********************************************************************************************************
*                                     CREATE A LRT's TASK
*
* Description: Creates a Lrt's task.
*
*********************************************************************************************************
*/

void  LrtTaskCreate (){
	// Popping second incoming word, the task Id.
//	UINT8 id = OS_CtrlQPop_UINT32();

	if(OSTaskIndex >= OS_MAX_TASKS){
		zynq_puts("Create Task ");zynq_putdec(OSTaskIndex);zynq_puts("\n");
		exitWithCode(1003);
	}
	OS_TCB *new_tcb = &OSTCBTbl[OSTaskIndex];

	if (new_tcb->OSTCBState == OS_STAT_UNINITIALIZED) { /* Make sure task doesn't already exist at this id  */
		new_tcb->OSTCBState = OS_STAT_READY;/* Reserve the priority to prevent others from doing ...  */

		/* Store task ID */
		new_tcb->OSTCBId = OSTaskIndex++; OSTaskCtr++;

		/* Update current running Task List */
		if(OSTCBCur == (OS_TCB*)0){
			/* If no running Task */
			OSTCBCur = new_tcb;
			new_tcb->OSTCBNext = new_tcb;
		}else{
			new_tcb->OSTCBNext = OSTCBCur;
		}

		// Popping the actor machine's info.
		new_tcb->nbVertices = OS_CtrlQPop_UINT32();
		new_tcb->nbConds 	 = OS_CtrlQPop_UINT32();
		new_tcb->nbActions	 = OS_CtrlQPop_UINT32();

		// Popping the task function id.
		UINT32 taskFunctId = OS_CtrlQPop_UINT32();

		// Popping whether the task is stopped after completion.
		new_tcb->stop = OS_CtrlQPop_UINT32();

		// Popping the starting vertex of the AM.
		new_tcb->current_vertexID 	= OS_CtrlQPop_UINT32();


		// Filling in the actor machine if needed.
		if(new_tcb->nbVertices > 0) // The task contains an AM.
		{
			new_tcb->task_func = amTaskStart; // An AM task's function is predefined.
			// Creating the AM.
			LrtAMCreate(new_tcb);
		}
		else// The task has no AM.
		{
			new_tcb->task_func = functions_tbl[taskFunctId];
		}

		// Popping
	    zynq_puts("Create Task ID"); zynq_putdec(new_tcb->OSTCBId);
	    zynq_puts(" @");  zynq_putdec(new_tcb->current_vertexID);
	    zynq_puts("\n");

#if defined ARM || defined DESKTOP
		char s[8] = "tX_X.gv";
		s[1] = new_tcb->OSTCBId + '0';
		s[3] = cpuId + '0';
		dotWriter(new_tcb, s);
#endif
	}else
		exitWithCode(1012);
}


AM_ACTOR_ACTION_STRUCT* OSCurActionQuery(){
	return &OSTCBCur->am_actions[OSTCBCur->am_vertices[OSTCBCur->current_vertexID].actionID];
}


/*
*********************************************************************************************************
*                                            DELETE A TASK
*
* Description: This function allows you to delete a task from the list of available tasks...
*
* Arguments  : id    is the identifier of the task to delete.
* 			   curr_vertex_id is will contain the id. of the current vertex of the deleted task.
*
* Returns    : OS_ERR_NONE             if the call is successful
*              OS_ERR_TASK_NOT_EXIST   if the task you want to delete does not exist.
*
*********************************************************************************************************
*/

void  LrtTaskDeleteCur(){
	UINT8	id = OSTCBCur->OSTCBId;
    OS_TCB    *del_tcb = &OSTCBTbl[id];
    if (del_tcb->OSTCBState == OS_STAT_READY) { /* Make sure task doesn't already exist at this id  */
    	del_tcb->OSTCBState = OS_STAT_UNINITIALIZED;/* Reserve the priority to prevent others from doing ...  */

    	/* Update current running Task List */
		if(del_tcb->OSTCBNext == del_tcb){
			OSTCBCur = (OS_TCB*)0;
			lrt_running = FALSE;
		}else{
			OSTCBCur = del_tcb->OSTCBNext;
		}

		/* Decrement the tasks counter */
		OSTaskCtr--;
    }else
    	exitWithCode(1013);
}

void  OSTaskDel (){
	UINT8 taskId   = OS_CtrlQPop_UINT32();
	UINT8 vectorId = OS_CtrlQPop_UINT32();
    OS_TCB       *ptcb = &OSTCBTbl[taskId];
    ptcb->stop = TRUE;
    ptcb->stopState = vectorId;
    zynq_puts("Stop Task ID"); zynq_putdec(taskId);
    zynq_puts(" at Vector ID");  zynq_putdec(vectorId);
    zynq_puts("\n");
}


void OSWorkingMemoryInit(){
	freeWorkingMemoryPtr = workingMemory;
}

void* OSAllocWorkingMemory(int size){
	if(freeWorkingMemoryPtr-workingMemory+size > WORKING_MEMORY_SIZE){
		zynq_puts("Asked ");zynq_putdec(size);zynq_puts(" bytes, but ");zynq_putdec(WORKING_MEMORY_SIZE-(int)freeWorkingMemoryPtr+(int)workingMemory);zynq_puts(" bytes available\n");
		exitWithCode(1015);
	}
	void* mem = freeWorkingMemoryPtr;
	freeWorkingMemoryPtr += size;
	return mem;
}

void OSFreeWorkingMemory(){
	freeWorkingMemoryPtr = workingMemory;
}
