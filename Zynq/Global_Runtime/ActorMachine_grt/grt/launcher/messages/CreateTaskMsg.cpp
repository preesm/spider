/*
 * TaskMsg.cpp
 *
 *  Created on: Jun 26, 2013
 *      Author: jheulot
 */

#include "CreateTaskMsg.h"
#include <hwQueues.h>

CreateTaskMsg::CreateTaskMsg(SRDAGGraph* graph, Schedule* schedule, int slave) {
	taskID = 0;
	functID = 0; // unused
	nbFifoIn = 0;
	nbFifoOut = 0;

	/* Actor Machine */
	initStateAM = 0;
	AM = AMGraph(schedule, slave);
}

CreateTaskMsg::CreateTaskMsg(SRDAGGraph* graph, SRDAGVertex* vertex) {
	taskID = graph->getVertexIndex(vertex);
	functID = vertex->getCsDagReference()->getFunctionIndex();

	nbFifoIn = vertex->getNbInputEdge();
	for(int i=0; i<nbFifoIn; i++)
		FifosInID[i] = graph->getEdgeIndex(vertex->getInputEdge(i));

	nbFifoOut = vertex->getNbOutputEdge();
	for(int i=0; i<nbFifoOut; i++)
		FifosOutID[i] = graph->getEdgeIndex(vertex->getOutputEdge(i));

	/* Actor Machine */
	initStateAM = 0;
	AM = AMGraph(vertex);
}

void CreateTaskMsg::send(int LRTID){
	UINT32 msg[MAX_MSG_LENGTH];
	int k=0, i;

	msg[k++] = MSG_CREATE_TASK;
	msg[k++] = taskID;

	msg[k++] = AM.getNbVertices();
	msg[k++] = AM.getNbConds();
	msg[k++] = AM.getNbActions();
	msg[k++] = initStateAM;

	/* Send vertices */
	for(i=0; i<AM.getNbVertices(); i++){
		msg[k++] = AM.getVertex(i)->getType();
		msg[k++] = AM.getVertex(i)->getSucID(0);
		msg[k++] = AM.getVertex(i)->getSucID(1);
		switch(AM.getVertex(i)->getType()){
		case EXEC:
			msg[k++] = AM.getVertex(i)->getAction();
			break;
		case TEST:
			msg[k++] = AM.getVertex(i)->getCondID();
			break;
		case WAIT:
		case STATE:
			msg[k++] = 0;
			break;
		default:
			//todo error
			break;
		}
	}

	/* Send Conditions */
	for(i=0; i<AM.getNbConds(); i++){
		msg[k++] = AM.getCond(i)->type;
		msg[k++] = AM.getCond(i)->fifo.id;
		msg[k++] = AM.getCond(i)->fifo.size;
	}

	// new
	for(i=0; i<AM.getNbActions(); i++){
		AMAction* action = AM.getAction(i);
		msg[k++] = action->getFunctionId();
		msg[k++] = action->getNbFifoIn();
		msg[k++] = action->getNbFifoOut();
		msg[k++] = action->getNbArgs();

		for(int j=0; j<action->getNbFifoIn(); j++)
			msg[k++] = action->getFifoIn(j);
		for(int j=0; j<action->getNbFifoOut(); j++)
			msg[k++] = action->getFifoOut(j);
		for(int j=0; j<action->getNbArgs(); j++)
			msg[k++] = action->getArg(j);
	}



	OS_CtrlQPush(LRTID, msg, k*sizeof(UINT32));
}

AMGraph* CreateTaskMsg::getAm(){
	return &AM;
}

void CreateTaskMsg::toDot(const char* path){
	AM.toDot(path);
}

int CreateTaskMsg::prepare(int* data, int offset){
	int k=0, i;

	data[offset + k++] = MSG_CREATE_TASK;
	data[offset + k++] = taskID;

	data[offset + k++] = AM.getNbVertices();
	data[offset + k++] = AM.getNbConds();
	data[offset + k++] = AM.getNbActions();
	data[offset + k++] = initStateAM;

	/* Send vertices */
	for(i=0; i<AM.getNbVertices(); i++){
		data[offset + k++] = AM.getVertex(i)->getType();
		data[offset + k++] = AM.getVertex(i)->getSucID(0);
		data[offset + k++] = AM.getVertex(i)->getSucID(1);
		switch(AM.getVertex(i)->getType()){
		case EXEC:
			data[offset + k++] = AM.getVertex(i)->getAction();
			break;
		case TEST:
			data[offset + k++] = AM.getVertex(i)->getCondID();
			break;
		case WAIT:
		case STATE:
			data[offset + k++] = 0;
			break;
		default:
			//todo error
			break;
		}
	}

	/* Send Conditions */
	for(i=0; i<AM.getNbConds(); i++){
		data[offset + k++] = AM.getCond(i)->type;
		data[offset + k++] = AM.getCond(i)->fifo.id;
		data[offset + k++] = AM.getCond(i)->fifo.size;
	}

	/* Send Actions */
	for(i=0; i<AM.getNbActions(); i++){
		AMAction* action = AM.getAction(i);
		data[offset + k++] = action->getFunctionId();
		data[offset + k++] = action->getNbFifoIn();
		data[offset + k++] = action->getNbFifoOut();
		data[offset + k++] = action->getNbArgs();

		for(int j=0; j<action->getNbFifoIn(); j++)
			data[offset + k++] = action->getFifoIn(j);
		for(int j=0; j<action->getNbFifoOut(); j++)
			data[offset + k++] = action->getFifoOut(j);
		for(int j=0; j<action->getNbArgs(); j++)
			data[offset + k++] = action->getArg(j);
	}



	return k;
}

void CreateTaskMsg::prepare(int slave, launcher* launch){
	launch->addUINT32ToSend(slave, MSG_CREATE_TASK);
	launch->addUINT32ToSend(slave, taskID);

	launch->addUINT32ToSend(slave, AM.getNbVertices());
	launch->addUINT32ToSend(slave, AM.getNbConds());
	launch->addUINT32ToSend(slave, AM.getNbActions());
	launch->addUINT32ToSend(slave, initStateAM);

	/* Send vertices */
	for(int i=0; i<AM.getNbVertices(); i++){
		launch->addUINT32ToSend(slave, AM.getVertex(i)->getType());
		launch->addUINT32ToSend(slave, AM.getVertex(i)->getSucID(0));
		launch->addUINT32ToSend(slave, AM.getVertex(i)->getSucID(1));

		switch(AM.getVertex(i)->getType()){
		case EXEC:
			launch->addUINT32ToSend(slave, AM.getVertex(i)->getAction());
			break;
		case TEST:
			launch->addUINT32ToSend(slave, AM.getVertex(i)->getCondID());
			break;
		case WAIT:
		case STATE:
			launch->addUINT32ToSend(slave, 0);
			break;
		default:
			//todo error
			break;
		}
	}

	/* Send Conditions */
	for(int i=0; i<AM.getNbConds(); i++){
		launch->addUINT32ToSend(slave, AM.getCond(i)->type);
		launch->addUINT32ToSend(slave, AM.getCond(i)->fifo.id);
		launch->addUINT32ToSend(slave, AM.getCond(i)->fifo.size);
	}

	/* Send Actions */
	for(int i=0; i<AM.getNbActions(); i++){
		AMAction* action = AM.getAction(i);
		launch->addUINT32ToSend(slave, action->getFunctionId());
		launch->addUINT32ToSend(slave, action->getNbFifoIn());
		launch->addUINT32ToSend(slave, action->getNbFifoOut());
		launch->addUINT32ToSend(slave, action->getNbArgs());

		for(int j=0; j<action->getNbFifoIn(); j++)
			launch->addUINT32ToSend(slave, action->getFifoIn(j));
		for(int j=0; j<action->getNbFifoOut(); j++)
			launch->addUINT32ToSend(slave, action->getFifoOut(j));
		for(int j=0; j<action->getNbArgs(); j++)
			launch->addUINT32ToSend(slave, action->getArg(j));
	}

	launch->addUINT32ToReceive(slave, MSG_CREATE_TASK);
}
