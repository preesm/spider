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
#include <algorithm>    // std::min
#include <grt_definitions.h>
#include <debuggingOptions.h>
#include <launcher/launcher.h>
#include <string.h>

#include <execution/execution.h>
/**
 Constructor

 @param nbSlaves: current number of slaves
*/
ListScheduler::ListScheduler()
{
	archi = (Architecture*)NULL;
//	scenario = (Scenario*)NULL;
}

void ListScheduler::reset()
{
	archi = (Architecture*)NULL;
//	scenario = (Scenario*)NULL;
	memset(lastVertexOfSlave,0,sizeof(lastVertexOfSlave));
}


/**
 Destructor
*/
ListScheduler::~ListScheduler()
{
}

/**
 Sets the slave architecture

 @param archi: slave architecture
*/
void ListScheduler::setArchitecture(Architecture* archi){
	this->archi = archi;
}

/**
 Return the schedule makespan

 @return: the makespan
*/
int ListScheduler::getMakespan(SRDAGGraph* srDag)
{
//	int max = 0;
//	for(int i = 0; i < srDag->getNbVertices(); i++)
//		if(srDag->getVertex(i)->getTLevel() + srDag->getVertex(i)->getCsDagReference()->getIntTiming(0) > max)
//			max = srDag->getVertex(i)->getTLevel() + srDag->getVertex(i)->getCsDagReference()->getIntTiming(0);
//
//	return max;
	return 0;
}

int ListScheduler::getThroughput(SRDAGGraph* srDag){
//	int slaveEndTime[MAX_SLAVES];
//	int slaveStartTime[MAX_SLAVES];
//
//	memset(slaveEndTime, 0, MAX_SLAVES*sizeof(int));
//	memset(slaveStartTime, -1, MAX_SLAVES*sizeof(int));
//
//	for(int i=0;  i < srDag->getNbVertices(); i++){
//		int slaveId = srDag->getVertex(i)->getSlaveIndex();
//		int startVertex = srDag->getVertex(i)->getTLevel();
//		int endVertex = startVertex + srDag->getVertex(i)->getCsDagReference()->getIntTiming(0);
//
//		if(endVertex >  slaveEndTime[slaveId]){
//			slaveEndTime[slaveId] = endVertex;
//		}
//		if(slaveStartTime[slaveId] == -1 || startVertex < slaveStartTime[slaveId]){
//			slaveStartTime[srDag->getVertex(i)->getSlaveIndex()] = startVertex;
//		}
//	}
//
//	int max = 0;
//	for(int i = 0; i < archi->getNbSlaves(); i++)
//		if(slaveStartTime[i] != -1)
//			max = std::max(slaveEndTime[i] - slaveStartTime[i], max);
//	return max;
	return 0;
}

//UINT32 ListScheduler::evaluateMinStartTime(BaseSchedule* schedule, Architecture* arch, SRDAGVertex* vertex){
//	UINT32 minimumStartTime = 0;
//	UINT32 precVertexEndTime;
//
//	if(vertex->getMinStartTime() != -1)
//		return vertex->getMinStartTime();
//
//	for(UINT32 i=0; i<vertex->getNbInputEdge(); i++){
//		SRDAGVertex* precVertex = vertex->getInputEdge(i)->getSource();
//		if(precVertex != vertex){ // TODO: Normally there is no cycles in a DAG, so this is check is not needed.
//			if(precVertex->getScheduleIndex() != -1)
//				// Getting the end time of the predecessor, since it has already been scheduled.
//				precVertexEndTime = schedule->getVertexEndTime(precVertex->getScheduleIndex(), precVertex);
//			else{
//				// Scheduling the precedent vertex.
//				precVertexEndTime = evaluateMinStartTime(schedule, arch, precVertex) + precVertex->getExecTime();
////				printf("Previous Not scheduled (%d) -> (%d) !\n", precVertex->getId(), vertex->getId());
//			}
//
//			minimumStartTime = std::max(minimumStartTime, precVertexEndTime);
////			vertex->getReference()->setScheduled(true);
//		}
//	}
//	return minimumStartTime;
//}

UINT32 ListScheduler::schedule(BaseSchedule* schedule, Architecture* arch, SRDAGVertexAbstract* vertex){
//	UINT32 noSchedule = -1; // Indicates that the vertex have not been scheduled.
	if(vertex->getScheduleIndex() != -1)
		return vertex->getEndTime();

	UINT32 minimumStartTime;

	for(int i=0; i<vertex->getNbInputEdge(); i++){
		minimumStartTime = std::max(minimumStartTime,
				vertex->getInputEdge(i)->getSource()->getEndTime());
	}


	UINT32 bestSlave;
	UINT32 bestStartTime = 0;
	UINT32 bestWaitTime = 0;
	UINT32 bestEndTime = (UINT32)-1; // Very high value.

	// Getting a slave for the vertex.
	for(int slave=0; slave<arch->getNbActiveSlaves(); slave++){
		int slaveType = arch->getSlaveType(slave);
		// checking the constraints
		if(vertex->getConstraint(slaveType)){
			unsigned int startTime = std::max(schedule->getReadyTime(slave), minimumStartTime);
			unsigned int waitTime = startTime-schedule->getReadyTime(slave);
			unsigned int execTime = vertex->getExecTime(slaveType);
			unsigned int comInTime=0, comOutTime=0;
//				for(int input=0; input<vertex->getNbInputEdge(); input++){
//					comInTime += arch->getTimeCom(slave, Read, vertex->getInputEdge(input)->getTokenRate());
//				}
//				for(int output=0; output<vertex->getNbOutputEdge(); output++){
//					comOutTime += arch->getTimeCom(slave, Write, vertex->getOutputEdge(output)->getTokenRate());
//				}
			unsigned int endTime = startTime + execTime + comInTime + comOutTime;
			if(endTime < bestEndTime
					|| (endTime == bestEndTime && waitTime<bestWaitTime)){
				bestSlave = slave;
				bestEndTime = endTime;
				bestStartTime = startTime;
				bestWaitTime = waitTime;
//					bestComInTime = comInTime;
//					bestComOutTime = comOutTime;
			}
		}
	}
	
	if(bestSlave != 0){
	  int i=0;
	  i++;
	}
	
//		schedule->addCom(bestSlave, bestStartTime, bestStartTime+bestComInTime);
	int scheduleIndex = schedule->addSchedule(bestSlave, vertex, bestStartTime, bestEndTime);
	vertex->setScheduleIndex(scheduleIndex);

	Launcher::launchVertex(vertex, bestSlave);

	return bestEndTime;
}

#define MAX(a,b) ((a>b)?a:b)

UINT32 computeSchedLevel(SRDAGVertexAbstract* vertex){
	int level = 0;
	if(vertex->getSchedLevel() == -1){
		for(int i=0; i<vertex->getNbOutputEdge(); i++){
			SRDAGVertexAbstract* succ = vertex->getOutputEdge(i)->getSink();
			if(succ->getState() == SRDAG_Executable)
				// TODO BETTER Careful execTime of core type 0
				level = MAX(level, computeSchedLevel(succ)+succ->getExecTime(0));
		}
		vertex->setSchedLevel(level);
		return level;
	}
	return vertex->getSchedLevel();
}


int comparSchedLevel(SRDAGVertexAbstract* a, SRDAGVertexAbstract* b){
	return b->getSchedLevel() - a->getSchedLevel();
}

void ListScheduler::schedule(SRDAGGraph* dag, BaseSchedule* schedule, Architecture* arch)
{
	Launcher::initTaskOrderingTime();

	// Create ScheduleList
	static List<SRDAGVertexAbstract*, MAX_SRDAG_VERTICES> schedList;
	schedList.reset();

	SRDAGVertexAbstract* vertex;
	SetIterator<SRDAGVertexAbstract,MAX_SRDAG_VERTICES> iterV = dag->getVertexIterator();
	while((vertex = iterV.next()) != NULL){
		if(vertex->getState() == SRDAG_Executable){
			schedList.add(vertex);
			computeSchedLevel(vertex);
		}
	}

	schedList.sort(comparSchedLevel);

	Launcher::endTaskOrderingTime();
	Launcher::initMappingTime();

//	schedule->newStep();

	UINT32 end = platform_time_getValue() + MAPPING_TIME*schedList.getNb();

	schedule->setReadyTime(0, end);

	Launcher::setActorsNb(schedList.getNb());

	for(int i=0; i<schedList.getNb(); i++){
		this->schedule(schedule, arch, schedList[i]);
	}

	Launcher::endMappingTime();

#if EXEC == 1
	execute();
#endif
}
//
//UINT32 computeSchedLevel(SRDAGVertex* vertex){
//	int level = 0;
//	if(vertex->getSchedLevel() == -1){
//		for(int i=0; i<vertex->getNbInputEdge(); i++){
//			SRDAGVertex* pred = vertex->getInputEdge(i)->getSource();
//			if(pred->getState() == SrVxStExecutable)
//				level = MAX(level, computeSchedLevel(pred)+1);
//		}
//		if(vertex->getType() != Normal && vertex->getType() != ConfigureActor){
//			if(level >= 0)
//				level--;
//		}
//		vertex->setSchedLevel(level);
//		return level;
//	}
//	return vertex->getSchedLevel();
//}
//
//void ListScheduler::schedule(SRDAGGraph* dag, BaseSchedule* schedule, Architecture* arch)
//{
//	// Create ScheduleList
////	static List<SRDAGVertex*, MAX_SRDAG_VERTICES> schedList;
//	static SRDAGVertex* histo[MAX_SRDAG_VERTICES][MAX_SRDAG_VERTICES];
//	static int histoNb[MAX_SRDAG_VERTICES];
//	memset(histoNb,0,sizeof(histoNb));
////	schedList.reset();
//
//	int nbVertices=0;
//	int level;
//
//	for(int i=0; i<dag->getNbVertices(); i++){
//		SRDAGVertex* vertex = dag->getVertex(i);
//		if(vertex->getScheduleIndex() == -1 &&
//		   vertex->getState() == SrVxStExecutable &&
//		   (vertex->getType() == Normal || vertex->getType() == ConfigureActor))
//		{
//			level = computeSchedLevel(vertex);
////			printf("vx %d: lvl %d\n", vertex->getId(), level);
//			histo[level][histoNb[level]] = vertex;
//			histoNb[level]++;
//			nbVertices++;
//		}
//	}
//
//	level=0;
//	while(nbVertices){
//		bool stop=0;
//		while(!stop){
//			stop = 1;
//			for(int i=0; i<histoNb[level]-1; i++){
//				if(evaluateMinStartTime(schedule, arch, histo[level][i])
//						> evaluateMinStartTime(schedule, arch, histo[level][i+1])){
//					SRDAGVertex* temp = histo[level][i+1];
//					histo[level][i+1] = histo[level][i];
//					histo[level][i] = temp;
//					stop = 0;
//					break;
//				}
//			}
//		}
////		printf("level %d :", level);
//		for(int i=0; i<histoNb[level]; i++){
//			this->schedule(schedule, arch, histo[level][i]);
////			printf(" %d,", histo[level][i]->getId());
//		}
////		printf("\n");
//		nbVertices-=histoNb[level];
//		level++;
//	}
//
////	// Scheduling the vertices.
////	for(int i=0; i<dag->getNbVertices(); i++){
////		SRDAGVertex* vertex = dag->getVertex(i);
////		if((vertex->getScheduleIndex() == -1) &&
////		   (vertex->getState() == SrVxStExecutable))
////		{
////			this->schedule(schedule, arch, vertex);
////		}
////	}
//}
//
