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

#include "ListScheduler.h"

#include <graphs/SRDAG/SRDAGVertex.h>
#include <graphs/SRDAG/SRDAGGraph.h>
#include <launcher/Launcher.h>

#include <algorithm>
#include <stdio.h>

#define MAPPING_TIME 100

ListScheduler::ListScheduler(){
	srdag_ = 0;
	schedule_ = 0;
	archi_ = 0;
	list_ = 0;
}

ListScheduler::~ListScheduler(){

}

int compareSchedLevel(SRDAGVertex* vertexA, SRDAGVertex* vertexB){
	return vertexB->getSchedLvl() - vertexA->getSchedLvl();
}

void ListScheduler::addPrevActors(SRDAGVertex* vertex, List<SRDAGVertex*> *list){
	for(int i=0; i<vertex->getNConnectedInEdge(); i++){
		SRDAGVertex* prevVertex = vertex->getInEdge(i)->getSrc();
		if(!list->isPresent(prevVertex) && prevVertex->getState() == SRDAG_EXEC){
			list->add(prevVertex);
			computeSchedLevel(prevVertex);
			addPrevActors(prevVertex, list);
		}
	}
}

void ListScheduler::scheduleOnlyConfig(SRDAGGraph* graph, MemAlloc* memAlloc, Schedule* schedule, Archi* archi, Stack* stack){
	srdag_ = graph;
	schedule_ = schedule;
	archi_ = archi;

	list_ = CREATE(stack, List<SRDAGVertex*>)(stack, srdag_->getNExecVertex());

//	Launcher::initTaskOrderingTime();

	for(int i=0; i<srdag_->getNVertex(); i++){
		SRDAGVertex *vertex = srdag_->getVertex(i);
		if(vertex->getState() == SRDAG_EXEC && vertex->getNOutParam() > 0){
			list_->add(vertex);
			vertex->setSchedLvl(-1);
			addPrevActors(vertex, list_);
		}
	}

	memAlloc->alloc(list_);

	for(int i=0; i<list_->getNb(); i++){
		computeSchedLevel((*list_)[i]);
	}

	list_->sort(compareSchedLevel);

//	Launcher::endTaskOrderingTime();
//	Launcher::initMappingTime();

	schedule_->setAllMinReadyTime(Platform::get()->getTime());
	schedule_->setReadyTime(0, Platform::get()->getTime() + MAPPING_TIME*list_->getNb());

//	Launcher::setActorsNb(schedList.getNb());

	for(int i=0; i<list_->getNb(); i++){
//		printf("%d (%d), ", (*list_)[i]->getId(), (*list_)[i]->getSchedLvl());
		this->scheduleVertex((*list_)[i]);
	}
//	printf("\n");

	for(int i=0; i<list_->getNb(); i++){
		Launcher::get()->launchVertex((*list_)[i]);
	}

	list_->~List();
	stack->free(list_);
}

void ListScheduler::schedule(SRDAGGraph* graph, MemAlloc* memAlloc, Schedule* schedule, Archi* archi, Stack* stack){
	srdag_ = graph;
	schedule_ = schedule;
	archi_ = archi;

	list_ = CREATE(stack, List<SRDAGVertex*>)(stack, srdag_->getNExecVertex());

	for(int i=0; i<srdag_->getNVertex(); i++){
		SRDAGVertex *vertex = srdag_->getVertex(i);
		if(vertex->getState() == SRDAG_EXEC){
			list_->add(vertex);
			vertex->setSchedLvl(-1);
		}
	}

	memAlloc->alloc(list_);

	for(int i=0; i<list_->getNb(); i++){
		computeSchedLevel((*list_)[i]);
	}

	list_->sort(compareSchedLevel);

//	for (int i=0; i<list_->getNb(); i++){
//		printf("%d (%d), ", (*list_)[i]->getId(), (*list_)[i]->getSchedLvl());
//	}
//	printf("\n");

	schedule_->setAllMinReadyTime(Platform::get()->getTime());
//	schedule_->setReadyTime(0, Platform::get()->getTime() + MAPPING_TIME*list_->getNb());

	for(int i=0; i<list_->getNb(); i++){
		this->scheduleVertex((*list_)[i]);
	}

	for(int i=0; i<list_->getNb(); i++){
		Launcher::get()->launchVertex((*list_)[i]);
	}


	list_->~List();
	stack->free(list_);
}

int ListScheduler::computeSchedLevel(SRDAGVertex* vertex){
	int lvl = 0;
	if(vertex->getSchedLvl() == -1){
		for(int i=0; i<vertex->getNConnectedOutEdge(); i++){
			SRDAGVertex* succ = vertex->getOutEdge(i)->getSnk();
			if(succ && succ->getState() != SRDAG_NEXEC){
				Time minExecTime = (unsigned int)-1;
				for(int j=0; j<archi_->getNPE(); j++){
					if(succ->isExecutableOn(j)){
						Time execTime = succ->executionTimeOn(archi_->getPEType(j));
						if(execTime == 0)
							throw "ListScheduler: Null execution time may cause problems\n";
						minExecTime = std::min(minExecTime, execTime);
					}
				}
				lvl = std::max(lvl, computeSchedLevel(succ)+(int)minExecTime);
			}
		}
		vertex->setSchedLvl(lvl);
		return lvl;
	}
	return vertex->getSchedLvl();
}

void ListScheduler::scheduleVertex(SRDAGVertex* vertex){
	Time minimumStartTime=0;

	for(int i=0; i<vertex->getNConnectedInEdge(); i++){
		minimumStartTime = std::max(minimumStartTime,
				vertex->getInEdge(i)->getSrc()->getEndTime());
//		if(vertex->getInEdge(i)->getSrc()->getSlave() == -1){
//			throw "Try to start a vertex when previous one is not scheduled\n";
//		}
	}

	if(vertex->getState() == SRDAG_RUN){
		vertex->setStartTime(minimumStartTime);
		vertex->setEndTime(minimumStartTime);
		return;
	}

	int bestSlave = -1;
	Time bestStartTime = 0;
	Time bestWaitTime = 0;
	Time bestEndTime = (unsigned long)-1; // Very high value.

	// Getting a slave for the vertex.
	for(int pe = 0; pe < archi_->getNPE(); pe++){
		int slaveType = archi_->getPEType(pe);
		// checking the constraints
		if(vertex->isExecutableOn(pe)){
			Time startTime = std::max(schedule_->getReadyTime(pe), minimumStartTime);
			Time waitTime  = startTime - schedule_->getReadyTime(pe);
			Time execTime  = vertex->executionTimeOn(slaveType);
			Time comInTime = 0, comOutTime = 0;
			/** TODO compute communication time */
//			for(int input=0; input<vertex->getNConnectedInEdge(); input++){
//				if(vertex->getInEdge(input)->getSrc()->getSlave() != pe)
//					comInTime += 1000;
////				comInTime += arch->getTimeCom(slave, Read, vertex->getInputEdge(input)->getTokenRate());
//			}
//			for(int output=0; output<vertex->getNConnectedOutEdge(); output++){
//				if(vertex->getOutEdge(output)->getSnk()->getSlave() != pe)
//					comOutTime += 1000;
////				comOutTime += arch->getTimeCom(slave, Write, vertex->getOutputEdge(output)->getTokenRate());
//			}
			Time endTime = startTime + execTime + comInTime + comOutTime;
			if(endTime < bestEndTime
					|| (endTime == bestEndTime && waitTime<bestWaitTime)){
				bestSlave = pe;
				bestEndTime = endTime;
				bestStartTime = startTime;
				bestWaitTime = waitTime;
			}
		}
	}

	if(bestSlave == -1){
		printf("No slave found to execute one instance of %s\n", vertex->getReference()->getName());
	}
//		schedule->addCom(bestSlave, bestStartTime, bestStartTime+bestComInTime);
	schedule_->addJob(bestSlave, vertex, bestStartTime, bestEndTime);

	vertex->setSlave(bestSlave);
}
