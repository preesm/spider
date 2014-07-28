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

#ifndef TASKMSG_H_
#define TASKMSG_H_

#include <platform_types.h>
#include "../../graphs/SRDAG/SRDAGGraph.h"
#include "../../graphs/SRDAG/SRDAGEdge.h"
#include "../../graphs/PiSDF/PiSDFEdge.h"
#include "../../graphs/PiSDF/PiSDFConfigVertex.h"
#include "../ActorMachine/AMGraph.h"
#include <scheduling/Schedule/Schedule.h>
#include <grt_definitions.h>
#include <platform_queue.h>
#include "../launcher.h"
#include <execution/execution.h>

namespace CreateTaskMsg {
	void send(int LRTID, SRDAGVertex* vertex);
}

void inline CreateTaskMsg::send(int lrtID, SRDAGVertex* vertex){

	if(lrtID == 0){ // Master
		pushExecution(vertex);
		return;
	}

	platform_QPushUINT32(lrtID, platformCtrlQ, MSG_CREATE_TASK);
	platform_QPushUINT32(lrtID, platformCtrlQ, vertex->getFctIx());
	platform_QPushUINT32(lrtID, platformCtrlQ, vertex->getId());
	platform_QPushUINT32(lrtID, platformCtrlQ, 0); // Not an actor machine.

	platform_QPushUINT32(lrtID, platformCtrlQ, vertex->getNbInputEdge());
	platform_QPushUINT32(lrtID, platformCtrlQ, vertex->getNbOutputEdge());

	switch(vertex->getType()){
	case Normal:
	case ConfigureActor:
		platform_QPushUINT32(lrtID, platformCtrlQ, vertex->getParamNb());
		break;
	case Explode:
	case Implode:
		platform_QPushUINT32(lrtID, platformCtrlQ, 2+vertex->getNbInputEdge()+vertex->getNbOutputEdge());
		break;
	case RoundBuffer:
		platform_QPushUINT32(lrtID, platformCtrlQ, 2);
		break;
	case Broadcast:
		platform_QPushUINT32(lrtID, platformCtrlQ, 0);
		break;
	case Init:
	case End:
		platform_QPushUINT32(lrtID, platformCtrlQ, 1);
		break;
	}

	for (int k = 0; k < vertex->getNbInputEdge(); k++){
		platform_QPush(lrtID, platformCtrlQ, vertex->getInputEdge(k)->getFifo(), sizeof(FIFO));
	}

	for (int k = 0; k < vertex->getNbOutputEdge(); k++){
		platform_QPush(lrtID, platformCtrlQ, vertex->getOutputEdge(k)->getFifo(), sizeof(FIFO));
	}

	switch(vertex->getType()){
	case Normal:
	case ConfigureActor:
		platform_QPush(lrtID, platformCtrlQ, vertex->getParamArray(), vertex->getParamNb()*sizeof(int));
		break;
	case Explode:
		platform_QPushUINT32(lrtID, platformCtrlQ, vertex->getNbInputEdge());
		platform_QPushUINT32(lrtID, platformCtrlQ, vertex->getNbOutputEdge());
		platform_QPushUINT32(lrtID, platformCtrlQ, vertex->getInputEdge(0)->getTokenRate());
		for(int i=0; i<vertex->getNbOutputEdge(); i++){
			platform_QPushUINT32(lrtID, platformCtrlQ, vertex->getOutputEdge(i)->getTokenRate());
		}
		break;
	case Implode:
		platform_QPushUINT32(lrtID, platformCtrlQ, vertex->getNbInputEdge());
		platform_QPushUINT32(lrtID, platformCtrlQ, vertex->getNbOutputEdge());
		platform_QPushUINT32(lrtID, platformCtrlQ, vertex->getOutputEdge(0)->getTokenRate());
		for(int i=0; i<vertex->getNbInputEdge(); i++){
			platform_QPushUINT32(lrtID, platformCtrlQ, vertex->getInputEdge(i)->getTokenRate());
		}
		break;
		break;
	case RoundBuffer:
		platform_QPushUINT32(lrtID, platformCtrlQ, vertex->getInputEdge(0)->getTokenRate());
		platform_QPushUINT32(lrtID, platformCtrlQ, vertex->getOutputEdge(0)->getTokenRate());
		break;
	case Broadcast:
		break;
	case Init:
		platform_QPushUINT32(lrtID, platformCtrlQ, vertex->getOutputEdge(0)->getTokenRate());
		break;
	case End:
		platform_QPushUINT32(lrtID, platformCtrlQ, vertex->getInputEdge(0)->getTokenRate());
		break;
	}

	platform_QPush_finalize(lrtID, platformCtrlQ);
}

#endif /* TASKMSG_H_ */
