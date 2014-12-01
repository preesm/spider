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

#include <graphs/Archi/Archi.h>
#include <graphs/SRDAG/SRDAGGraph.h>
#include <graphs/SRDAG/SRDAGVertex.h>

#include <time.h>
#include <algorithm>

#include <platform.h>
#include <platform_file.h>
#include <platform_time.h>
#include <platform_ctrlQ.h>

Launcher Launcher::instance_;

Launcher::Launcher(){
	curNParam_ = 0;
}

void Launcher::launchVertex(SRDAGVertex* vertex, int slave){
#if EXEC == 1
	CreateTaskMsg::send(slave, vertex);
	if(vertex->getType() == ConfigureActor)
		nbParamToRecv += ((PiSDFConfigVertex*)(vertex->getReference()))->getNbRelatedParams();
#endif
	vertex->setState(SRDAG_RUN);
}

void Launcher::send_ClearTimeMsg(int lrtIx){
	platform_QPushInt(lrtIx, MSG_CLEAR_TIME);
	platform_QPush_finalize(lrtIx);
}

void Launcher::send_StartJobMsg(int lrtIx, SRDAGVertex* vertex){
	/*
	 * Create Message :
	 * 		- CreateMsg ID
	 * 		- Fonction ID
	 * 		- SRDAG ID
	 * 		- Type
	 * 		# Global Iteration ID
	 * 		# PiSDF ID
	 * 		# iteration ID
	 * 		# repetition ID
	 * 		- Nb Inputs
	 * 		- Inputs
	 * 		- Nb Outputs
	 * 		- Outputs
	 * 		- Nb Params
	 * 		- Params
	 */

//	if(lrtIx == 0){ // Master
//		pushExecution(vertex);
//		return;
//	}

	platform_QPushInt(lrtIx, MSG_START_JOB);
	platform_QPushInt(lrtIx, vertex->getFctId());
	platform_QPushInt(lrtIx, vertex->getId());
	platform_QPushInt(lrtIx, vertex->getType());
//	platform_QPushInt(lrtIx, getGlobalIteration());
//	platform_QPushInt(lrtIx, (long long)(vertex->getReference()));
//	platform_QPushInt(lrtIx, vertex->getIterationIndex());
//	platform_QPushInt(lrtIx, vertex->getReferenceIndex());

	platform_QPushInt(lrtIx, vertex->getNInEdge());
	for (int k = 0; k < vertex->getNInEdge(); k++){
		platform_QPushInt(lrtIx, vertex->getInEdge(k)->getAllocIx());
		platform_QPushInt(lrtIx, vertex->getInEdge(k)->getRate());
		platform_QPushInt(lrtIx, vertex->getInEdge(k)->getAlloc());
	}

	platform_QPushInt(lrtIx, vertex->getNOutEdge());
	for (int k = 0; k < vertex->getNOutEdge(); k++){
		platform_QPushInt(lrtIx, vertex->getOutEdge(k)->getAllocIx());
		platform_QPushInt(lrtIx, vertex->getOutEdge(k)->getRate());
		platform_QPushInt(lrtIx, vertex->getOutEdge(k)->getAlloc());
	}

	switch(vertex->getType()){
	case SRDAG_NORMAL:
		platform_QPushInt(lrtIx, vertex->getNInParam());
		platform_QPush(lrtIx, (void*)vertex->getInParams(), vertex->getNInParam()*sizeof(int));
		break;
	case SRDAG_FORK:
		platform_QPushInt(lrtIx, 2+vertex->getNInEdge()+vertex->getNOutEdge());
		platform_QPushInt(lrtIx, vertex->getNInEdge());
		platform_QPushInt(lrtIx, vertex->getNOutEdge());
		platform_QPushInt(lrtIx, vertex->getInEdge(0)->getRate());
		for(int i=0; i<vertex->getNOutEdge(); i++){
			platform_QPushInt(lrtIx, vertex->getOutEdge(i)->getRate());
		}
		break;
	case SRDAG_JOIN:
		platform_QPushInt(lrtIx, 2+vertex->getNInEdge()+vertex->getNOutEdge());
		platform_QPushInt(lrtIx, vertex->getNInEdge());
		platform_QPushInt(lrtIx, vertex->getNOutEdge());
		platform_QPushInt(lrtIx, vertex->getOutEdge(0)->getRate());
		for(int i=0; i<vertex->getNInEdge(); i++){
			platform_QPushInt(lrtIx, vertex->getInEdge(i)->getRate());
		}
		break;
	case SRDAG_ROUNDBUFFER:
		platform_QPushInt(lrtIx, 2);
		platform_QPushInt(lrtIx, vertex->getInEdge(0)->getRate());
		platform_QPushInt(lrtIx, vertex->getOutEdge(0)->getRate());
		break;
	case SRDAG_BROADCAST:
		platform_QPushInt(lrtIx, 2);
		platform_QPushInt(lrtIx, vertex->getInEdge(0)->getRate());
		platform_QPushInt(lrtIx, vertex->getNOutEdge());
		break;
	case SRDAG_INIT:
		platform_QPushInt(lrtIx, 1);
		platform_QPushInt(lrtIx, vertex->getOutEdge(0)->getRate());
		break;
	case SRDAG_END:
		platform_QPushInt(lrtIx, 1);
		platform_QPushInt(lrtIx, vertex->getInEdge(0)->getRate());
		break;
	}
	platform_QPush_finalize(lrtIx);
}

void Launcher::resolveParams(Archi* archi, SRDAGGraph* topDag){
	int slave = 0;
	while(curNParam_ != 0){
		int msgType;
		if(platform_QNonBlockingPop(slave, &msgType, sizeof(int)) == sizeof(int)){
			if(msgType != MSG_PARAM_VALUE)
				throw "Unexpected Msg received\n";
			int vxId = platform_QPopInt(slave);
			SRDAGVertex* cfgVertex = topDag->getVertex(vxId);
			for(int j = 0; j < cfgVertex->getNOutParam(); j++){
				int* param = cfgVertex->getOutParam(j);
				*param = platform_QPopInt(slave);
			}
			curNParam_ -= cfgVertex->getNOutParam();
		}
		slave = (slave+1)%archi->getNPE();
	}
}
