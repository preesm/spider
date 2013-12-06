
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/

#include "CreateTaskMsg.h"
#include <hwQueues.h>

AMGraph AMGraphTbl[MAX_NB_AM];
UINT32 nbAM 		= 0;
LRTActor LRTActorTbl[MAX_SRDAG_VERTICES];
UINT32 nbLRTActors 	= 0;


CreateTaskMsg::CreateTaskMsg(SRDAGGraph* graph, Schedule* schedule, int slave, AMGraph* am) {
	taskID = 0;
	functID = 0; // In case of it is a simple task without AM.
//	nbFifoIn = 0;
//	nbFifoOut = 0;
//	this->am = am;

	/* Actor Machine */
//	initStateAM = 0;
//	am->generate(schedule, slave);
}

CreateTaskMsg::CreateTaskMsg(SRDAGGraph* graph, BaseSchedule* schedule, int slave, launcher* curLaunch){
//	taskID = 0;
	functID = 0;

	/* Actor Machine */
//	initStateAM = 0;

	if(nbAM == MAX_NB_AM) exitWithCode(1058);
	this->actor.am = &AMGraphTbl[nbAM++];
	this->actor.am->generate(graph, schedule, slave, curLaunch);


}

CreateTaskMsg::CreateTaskMsg(SRDAGGraph *graph, SRDAGVertex* srvertex, launcher* curLaunch){
//	taskID = 0;
	functID = srvertex->getReference()->getFunction_index();

	if(nbLRTActors == MAX_NB_VERTICES) exitWithCode(1059);
	LRTActorTbl[nbLRTActors] = LRTActor(graph, srvertex, curLaunch);
	this->actor.lrtActor = &LRTActorTbl[nbLRTActors];
	nbLRTActors++;
}

CreateTaskMsg::CreateTaskMsg(SRDAGGraph* graph, SRDAGVertex* vertex, AMGraph* am) {
	taskID = graph->getVertexIndex(vertex);
	functID = vertex->getCsDagReference()->getFunctionIndex();

//	nbFifoIn = vertex->getNbInputEdge();
//	for(int i=0; i<nbFifoIn; i++)
//		FifosInID[i] = graph->getEdgeIndex(vertex->getInputEdge(i));
//
//	nbFifoOut = vertex->getNbOutputEdge();
//	for(int i=0; i<nbFifoOut; i++)
//		FifosOutID[i] = graph->getEdgeIndex(vertex->getOutputEdge(i));

	/* Actor Machine */
//	initStateAM = 0;
//	this->am = am;
	am->generate(vertex);
}


CreateTaskMsg::CreateTaskMsg(PiSDFConfigVertex* vertex, AMGraph* am, INT32 stopAfterComplet) {
	taskID = vertex->getId();
	functID = vertex->getFunction_index();
//	this->stopAfterComplet = stopAfterComplet;
//	this->am = am;

//	nbFifoIn = vertex->getNbInputEdges();
//	for(INT32 i=0; i<nbFifoIn; i++)
//		FifosInID[i] = ((PiSDFEdge*)(vertex->getInputEdge(i)))->getId();



//	nbFifoOut = vertex->getNbOutputEdges();
//	for(INT32 i=0; i<nbFifoOut; i++)
//		FifosOutID[i] = ((PiSDFEdge*)(vertex->getOutputEdge(i)))->getId();

	/* Actor Machine */
//	initStateAM = 0;
//	TODO:am->generate(vertex);
}

void CreateTaskMsg::send(int LRTID){
	UINT32 msg[MAX_MSG_LENGTH];
	int k=0, i;

	msg[k++] = MSG_CREATE_TASK;
	msg[k++] = taskID;

//	msg[k++] = am->getNbVertices();
//	msg[k++] = am->getNbConds();
//	msg[k++] = am->getNbActions();
//	msg[k++] = initStateAM;

	/* Send vertices */
//	for(i=0; i<am->getNbVertices(); i++){
//		msg[k++] = am->getVertex(i)->getType();
//		msg[k++] = am->getVertex(i)->getSucID(0);
//		msg[k++] = am->getVertex(i)->getSucID(1);
//		switch(am->getVertex(i)->getType()){
//		case EXEC:
//			msg[k++] = am->getVertex(i)->getAction();
//			break;
//		case TEST:
//			msg[k++] = am->getVertex(i)->getCondID();
//			break;
//		case WAIT:
//		case STATE:
//			msg[k++] = 0;
//			break;
//		default:
//			//todo error
//			break;
//		}
//	}
//
//	/* Send Conditions */
//	for(i=0; i<am->getNbConds(); i++){
//		msg[k++] = am->getCond(i)->type;
//		msg[k++] = am->getCond(i)->fifo.id;
//		msg[k++] = am->getCond(i)->fifo.size;
//	}
//
//	// new
//	for(i=0; i<am->getNbActions(); i++){
//		AMAction* action = am->getAction(i);
//		msg[k++] = action->getFunctionId();
//		msg[k++] = action->getNbFifoIn();
//		msg[k++] = action->getNbFifoOut();
//		msg[k++] = action->getNbArgs();
//
//		for(int j=0; j<action->getNbFifoIn(); j++)
//			msg[k++] = action->getFifoIn(j);
//		for(int j=0; j<action->getNbFifoOut(); j++)
//			msg[k++] = action->getFifoOut(j);
//		for(int j=0; j<action->getNbArgs(); j++)
//			msg[k++] = action->getArg(j);
//	}
//
//
//
//	RTQueuePush(LRTID, RTCtrlQueue, msg, k*sizeof(UINT32));
}

//AMGraph* CreateTaskMsg::getAm(){
//	return &AM;
//}

void CreateTaskMsg::toDot(const char* path){
	if(isAM)
		actor.am->toDot(path);
}

int CreateTaskMsg::prepare(int* data, int offset){
	int k=0, i;

	data[offset + k++] = MSG_CREATE_TASK;
	data[offset + k++] = taskID;

//	data[offset + k++] = am->getNbVertices();
//	data[offset + k++] = am->getNbConds();
//	data[offset + k++] = am->getNbActions();
//	data[offset + k++] = initStateAM;

	/* Send vertices */
//	for(i=0; i<am->getNbVertices(); i++){
//		data[offset + k++] = am->getVertex(i)->getType();
//		data[offset + k++] = am->getVertex(i)->getSucID(0);
//		data[offset + k++] = am->getVertex(i)->getSucID(1);
//		switch(am->getVertex(i)->getType()){
//		case EXEC:
//			data[offset + k++] = am->getVertex(i)->getAction();
//			break;
//		case TEST:
//			data[offset + k++] = am->getVertex(i)->getCondID();
//			break;
//		case WAIT:
//		case STATE:
//			data[offset + k++] = 0;
//			break;
//		default:
//			//todo error
//			break;
//		}
//	}
//
//	/* Send Conditions */
//	for(i=0; i<am->getNbConds(); i++){
//		data[offset + k++] = am->getCond(i)->type;
//		data[offset + k++] = am->getCond(i)->fifo.id;
//		data[offset + k++] = am->getCond(i)->fifo.size;
//	}
//
//	/* Send Actions */
//	for(i=0; i<am->getNbActions(); i++){
//		AMAction* action = am->getAction(i);
//		data[offset + k++] = action->getFunctionId();
//		data[offset + k++] = action->getNbFifoIn();
//		data[offset + k++] = action->getNbFifoOut();
//		data[offset + k++] = action->getNbArgs();
//
//		for(int j=0; j<action->getNbFifoIn(); j++)
//			data[offset + k++] = action->getFifoIn(j);
//		for(int j=0; j<action->getNbFifoOut(); j++)
//			data[offset + k++] = action->getFifoOut(j);
//		for(int j=0; j<action->getNbArgs(); j++)
//			data[offset + k++] = action->getArg(j);
//	}



	return k;
}


void CreateTaskMsg::prepare(int slave, launcher* launch){
	launch->addUINT32ToSend(slave, MSG_CREATE_TASK);
//	launch->addUINT32ToSend(slave, taskID);
	launch->addUINT32ToSend(slave, this->functID);			// Function's id of the task.
//	launch->addUINT32ToSend(slave, stopAfterComplet);		// Whether the task is stopped after completion.
	launch->addUINT32ToSend(slave, this->isAM);

//	if(this->isAM == FALSE)
//		this->actor.lrtActor->prepare(slave, launch);
//	else
		// this->actor.am->generate();


//	launch->addUINT32ToReceive(slave, MSG_CREATE_TASK);
}
