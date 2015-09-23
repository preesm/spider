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

#include "Launcher.h"
#include <SpiderCommunicator.h>
#include <Message.h>

#include <graphs/Archi/Archi.h>
#include <graphs/SRDAG/SRDAGGraph.h>
#include <graphs/SRDAG/SRDAGVertex.h>

#include <time.h>
#include <algorithm>
#include <stdio.h>

#include <platform.h>
#include <spider.h>

Launcher Launcher::instance_;

Launcher::Launcher(){
	curNParam_ = 0;
	nLaunched_ = 0;
}

void Launcher::launchVertex(SRDAGVertex* vertex, bool useActorPrecedence){
	if(vertex->getState() == SRDAG_EXEC){
		int slave = vertex->getSlave();
		send_StartJobMsg(slave, vertex, useActorPrecedence);
		nLaunched_++;
		vertex->setState(SRDAG_RUN);
	}
}

Launcher* Launcher::get(){
	return &instance_;
}

void Launcher::send_ClearTimeMsg(int lrtIx){
	ClearTimeMsg* msg = (ClearTimeMsg*)Platform::getSpiderCommunicator()->ctrl_start_send(lrtIx, sizeof(ClearTimeMsg));
	msg->msgIx = MSG_CLEAR_TIME;
	Platform::getSpiderCommunicator()->ctrl_end_send(lrtIx, sizeof(ClearTimeMsg));
}

void Launcher::send_StartJobMsg(int lrtIx, SRDAGVertex* vertex, bool useActorPrecedence){
	/** retreive Infos for msg */
	int nParams = 0;
	switch(vertex->getType()){
	case SRDAG_NORMAL:
		nParams = vertex->getNInParam();
		break;
	case SRDAG_FORK:
	case SRDAG_JOIN:
		nParams = 2+vertex->getNConnectedInEdge()+vertex->getNConnectedOutEdge();
		break;
	case SRDAG_ROUNDBUFFER:
	case SRDAG_BROADCAST:
		nParams = 2;
		break;
	case SRDAG_INIT:
	case SRDAG_END:
		nParams = 1;
		break;
	}

	int size = 1*sizeof(StartJobMsg)
					+ vertex->getNConnectedInEdge()*sizeof(Fifo)
					+ vertex->getNConnectedOutEdge()*sizeof(Fifo)
					+ nParams*sizeof(Param);
	long msgAdd = (long) Platform::getSpiderCommunicator()->ctrl_start_send(
			lrtIx,
			size
			);

	StartJobMsg* msg = (StartJobMsg*) msgAdd;
	Fifo *inFifos = (Fifo*) ((char*)msgAdd + 1*sizeof(StartJobMsg));
	Fifo *outFifos = (Fifo*) ((char*)inFifos + vertex->getNConnectedInEdge()*sizeof(Fifo));
	Param *inParams = (Param*) ((char*)outFifos + vertex->getNConnectedOutEdge()*sizeof(Fifo));

	msg->msgIx = MSG_START_JOB;
	msg->srdagIx = vertex->getId();
	msg->specialActor = vertex->getType() != SRDAG_NORMAL;
	msg->fctIx = vertex->getFctId();

	msg->nbInEdge = vertex->getNConnectedInEdge();
	msg->nbOutEdge = vertex->getNConnectedOutEdge();
	msg->nbInParam = nParams;
	msg->nbOutParam = vertex->getNOutParam();

	for(int i=0; i<vertex->getNConnectedInEdge(); i++){
		SRDAGEdge* edge = vertex->getInEdge(i);
		inFifos[i].id = edge->getAllocIx();
		inFifos[i].alloc = edge->getAlloc();
		inFifos[i].size = edge->getRate();

		if(useActorPrecedence &&
				edge->getSrc()->getSlave() == edge->getSnk()->getSlave()){
			inFifos[i].ntoken = 0;
//			if(edge->getSrc()->getSlave()>=4)
//				printf("N token = 0\n");
		}
		else
			inFifos[i].ntoken = edge->getNToken();
	}

	for(int i=0; i<vertex->getNConnectedOutEdge(); i++){
		SRDAGEdge* edge = vertex->getOutEdge(i);
		outFifos[i].id = edge->getAllocIx();
		outFifos[i].alloc = edge->getAlloc();
		outFifos[i].size = edge->getRate();

		if(useActorPrecedence
				&& edge->getSrc() && edge->getSnk()
				&& edge->getSrc()->getSlave() == edge->getSnk()->getSlave())
			outFifos[i].ntoken = 0;
		else
			outFifos[i].ntoken = edge->getNToken();
	}

	switch(vertex->getType()){
	case SRDAG_NORMAL:
		for(int i=0; i<nParams; i++){
			inParams[i] = vertex->getInParam(i);
		}
//		memcpy(inParams, vertex->getInParams(), nParams*sizeof(Param));
		break;
	case SRDAG_FORK:
		inParams[0] = vertex->getNConnectedInEdge();
		inParams[1] = vertex->getNConnectedOutEdge();
		inParams[2] = vertex->getInEdge(0)->getRate();
		for(int i=0; i<vertex->getNConnectedOutEdge(); i++){
			inParams[3+i] = vertex->getOutEdge(i)->getRate();
		}
		break;
	case SRDAG_JOIN:
		inParams[0] = vertex->getNConnectedInEdge();
		inParams[1] = vertex->getNConnectedOutEdge();
		inParams[2] = vertex->getOutEdge(0)->getRate();
		for(int i=0; i<vertex->getNConnectedInEdge(); i++){
			inParams[3+i] = vertex->getInEdge(i)->getRate();
		}
		break;
	case SRDAG_ROUNDBUFFER:
		inParams[0] = vertex->getInEdge(0)->getRate();
		inParams[1] = vertex->getOutEdge(0)->getRate();
		break;
	case SRDAG_BROADCAST:
		inParams[0] = vertex->getInEdge(0)->getRate();
		inParams[1] = vertex->getNConnectedOutEdge();
		break;
	case SRDAG_INIT:
		inParams[0] = vertex->getOutEdge(0)->getRate();
		break;
	case SRDAG_END:
		inParams[0] = vertex->getInEdge(0)->getRate();
		break;
	}

	curNParam_ += vertex->getNOutParam();

	Platform::getSpiderCommunicator()->ctrl_end_send(lrtIx, size);
}

void Launcher::resolveParams(Archi* archi, SRDAGGraph* topDag){
	int slave = 0;
	while(curNParam_ != 0){
		ParamValueMsg* msg;
		if(Platform::getSpiderCommunicator()->ctrl_start_recv(slave, (void**)(&msg))){
			if(msg->msgIx != MSG_PARAM_VALUE)
				throw "Unexpected Msg received\n";
			SRDAGVertex* cfgVertex = topDag->getVertexFromIx(msg->srdagIx);
			Param* params = (Param*) (msg+1);
			for(int j = 0; j < cfgVertex->getNOutParam(); j++){
				int* param = cfgVertex->getOutParam(j);
				*param = params[j];
//				printf("Recv param = %d\n", *param);
			}
			curNParam_ -= cfgVertex->getNOutParam();
			Platform::getSpiderCommunicator()->ctrl_end_recv(slave);
		}
		slave = (slave+1)%archi->getNPE();
	}
}

void Launcher::sendTraceSpider(TraceSpiderType type, Time start, Time end){
	TraceMsg* msgTrace = (TraceMsg*) Platform::getSpiderCommunicator()->trace_start_send(sizeof(TraceMsg));

	msgTrace->msgIx = TRACE_SPIDER;
	msgTrace->spiderTask = type;
	msgTrace->srdagIx = -1;
	msgTrace->start = start;
	msgTrace->end = end;
	msgTrace->lrtIx = Platform::getLrt()->getIx();

	Platform::getSpiderCommunicator()->trace_end_send(sizeof(TraceMsgType));
	nLaunched_++;
}

int Launcher::getNLaunched(){
	return nLaunched_;
}

void Launcher::rstNLaunched(){
	nLaunched_ = 0;
}
