/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet,            *
 * Hugo Miomandre                                                           *
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

#include "RoundRobin.h"

#include <graphs/SRDAG/SRDAGVertex.h>
#include <graphs/SRDAG/SRDAGGraph.h>
#include <launcher/Launcher.h>
#include <spider.h>

#include <algorithm>
#include <stdio.h>

RoundRobin::RoundRobin(){

#ifndef __k1__
	printf("RoundRobin was not tested on this platform\n");
#endif

	if(Spider::getGraphOptim()){
		printf("Graph Optim not supported with this scheduler\n");
		throw "Graph Optim not supported with this scheduler";
	}


	srdag_ = 0;
	schedule_ = 0;
	archi_ = 0;
	list_ = 0;
}

RoundRobin::~RoundRobin(){

}

static int compareSchedLevel(SRDAGVertex* vertexA, SRDAGVertex* vertexB){
	return vertexB->getSchedLvl() - vertexA->getSchedLvl();
}

void RoundRobin::addPrevActors(SRDAGVertex* vertex, List<SRDAGVertex*> *list){
	for(int i=0; i<vertex->getNConnectedInEdge(); i++){
		SRDAGVertex* prevVertex = vertex->getInEdge(i)->getSrc();
		if(!list->isPresent(prevVertex) && prevVertex->getState() == SRDAG_EXEC){
			list->add(prevVertex);
			computeSchedLevel(prevVertex);
			addPrevActors(prevVertex, list);
		}
	}
}

void RoundRobin::scheduleOnlyConfig(
		SRDAGGraph* graph,
		MemAlloc* memAlloc,
		Schedule* schedule,
		Archi* archi){
	srdag_ = graph;
	schedule_ = schedule;
	archi_ = archi;

	list_ = CREATE(TRANSFO_STACK, List<SRDAGVertex*>)(TRANSFO_STACK, srdag_->getNExecVertex());

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
	schedule_->setReadyTime(
			/* Spider Pe */ 		archi->getSpiderPeIx(),
			/* End of Mapping */ 	Platform::get()->getTime() + archi->getMappingTimeFct()(list_->getNb(),archi_->getNPE()));

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
	StackMonitor::free(TRANSFO_STACK, list_);
}

void RoundRobin::schedule(
		SRDAGGraph* graph,
		MemAlloc* memAlloc,
		Schedule* schedule,
		Archi* archi){
	srdag_ = graph;
	schedule_ = schedule;
	archi_ = archi;

	list_ = CREATE(TRANSFO_STACK, List<SRDAGVertex*>)(TRANSFO_STACK, srdag_->getNExecVertex());

	// Fill the list_ with SRDAGVertices in scheduling order
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
	schedule_->setReadyTime(
			/* Spider Pe */ 		archi->getSpiderPeIx(),
			/* End of Mapping */ 	Platform::get()->getTime() + archi->getMappingTimeFct()(list_->getNb(),archi_->getNPE()));

	for(int i=0; i<list_->getNb(); i++){
		this->scheduleVertex((*list_)[i]);
		Launcher::get()->launchVertex((*list_)[i]);
	}

	// for(int i=0; i<list_->getNb(); i++){
	// 	Launcher::get()->launchVertex((*list_)[i]);
	// }


	list_->~List();
	StackMonitor::free(TRANSFO_STACK, list_);
}

#if 0
int RoundRobin::computeSchedLevel(SRDAGVertex* vertex){
	int lvl = 0;
	if(vertex->getSchedLvl() == -1){
		for(int i=0; i<vertex->getNConnectedOutEdge(); i++){
			SRDAGVertex* succ = vertex->getOutEdge(i)->getSnk();
			if(succ && succ->getState() != SRDAG_NEXEC){
				Time minExecTime = (Time)-1;
				for(int j=0; j<archi_->getNPE(); j++){
					if(succ->isExecutableOn(j)){
						Time execTime = succ->executionTimeOn(archi_->getPEType(j));
						if(execTime == 0)
							throw "ListSchedulerOnTheGo: Null execution time may cause problems\n";
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
#else
int RoundRobin::computeSchedLevel(SRDAGVertex* vertex){
	int lvl = 0;
	if(vertex->getSchedLvl() == -1){
		for(int i=0; i<vertex->getNConnectedOutEdge(); i++){
			SRDAGVertex* succ = vertex->getOutEdge(i)->getSnk();
			if(succ && succ->getState() != SRDAG_NEXEC){
				Time minExecTime = (Time)-1;
				for(int j=0; j<archi_->getNPETypes(); j++){
					
					Time execTime = succ->executionTimeOn(archi_->getPEType(j));
					if (execTime == 0) continue;
					minExecTime = std::min(minExecTime, execTime);
				}
				lvl = std::max(lvl, computeSchedLevel(succ)+(int)minExecTime);
			}
		}
		vertex->setSchedLvl(lvl);
		return lvl;
	}
	return vertex->getSchedLvl();
}
#endif

void RoundRobin::scheduleVertex(SRDAGVertex* vertex){
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
	Time bestEndTime = (Time)-1; // Very high value.


		//Getting alloc size to determine if PE can handle it
		int vertexAllocSize = 0;
		for(int i=0; i<vertex->getNConnectedInEdge(); i++){
			vertexAllocSize += vertex->getInEdge(i)->getRate();
		}

		for(int i=0; i<vertex->getNConnectedOutEdge(); i++){
			vertexAllocSize += vertex->getOutEdge(i)->getRate();
		}

		//if (vertex->getType() == SRDAG_NORMAL) printf("%s requires %d bytes\n",vertex->getReference()->getName(),vertexAllocSize);


		static int pe = 0;
		int npe = archi_->getNPE();

		while(bestSlave == -1){
			pe++;

			if (vertex->getType() != SRDAG_NORMAL){
				bestSlave = 0;
				break;
			}

			if(!archi_->isActivated(pe%npe)){
				continue;
			}

			if(vertex->isExecutableOn(pe%npe) && vertexAllocSize < Platform::get()->getMaxActorAllocSize(pe%npe))
			{
				bestSlave = pe%npe;
			}
		}


	if(bestSlave == -1){
		printf("No slave found to execute one instance of %s\n", vertex->getReference()->getName());
	}
	//printf("=> choose pe %d\n", bestSlave);
//		schedule->addCom(bestSlave, bestStartTime, bestStartTime+bestComInTime);
	schedule_->addJob(bestSlave, vertex, bestStartTime, bestEndTime);

	//if (vertex->getType() == SRDAG_NORMAL) printf("%s scheduled on PE %d\n",vertex->getReference()->getName(),bestSlave);

	vertex->setSlave(bestSlave);
}
