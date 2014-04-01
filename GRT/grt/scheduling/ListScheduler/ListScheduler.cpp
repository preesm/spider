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

/**
 Constructor

 @param nbSlaves: current number of slaves
*/
ListScheduler::ListScheduler()
{
	archi = (Architecture*)NULL;
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

BOOL ListScheduler::checkConstraint(SRDAGVertex* vertex, Architecture* arch, UINT32 slave){
	switch(vertex->getType()){
	case Normal:
	case ConfigureActor:
		return ! scenario->getConstraints(vertex->getReference()->getId(), slave);
	case Explode:
	case Implode:
	case RoundBuffer:
	default:
		return true;
	}
}

UINT32 ListScheduler::getTiming(SRDAGVertex* vertex, Architecture* arch, UINT32 slaveType){
	switch(vertex->getType()){
	case Normal:
	case ConfigureActor:
		return scenario->getTiming(vertex->getReference()->getId(), slaveType);
	case Explode:
	case Implode:
	case RoundBuffer:
	default:
		return 10;
	}
}

UINT32 ListScheduler::schedule(BaseSchedule* schedule, Architecture* arch, SRDAGVertex* vertex){
//	UINT32 noSchedule = -1; // Indicates that the vertex have not been scheduled.
	UINT32 minimumStartTime = 0;
	UINT32 precVertexEndTime;
	// Computing the minimum start time.
	for(int i=0; i<vertex->getNbInputEdge(); i++){
		SRDAGVertex* precVertex = vertex->getInputEdge(i)->getSource();
		if(precVertex != vertex){ // TODO: Normally there is no cycles in a DAG, so this is check is not needed.
			if(precVertex->getScheduleIndex() != -1)
				// Getting the end time of the predecessor, since it has already been scheduled.
				precVertexEndTime = schedule->getVertexEndTime(precVertex->getScheduleIndex(), precVertex);
			else
				// Scheduling the precedent vertex.
				precVertexEndTime = this->schedule(schedule, arch, precVertex);

			minimumStartTime = std::max(minimumStartTime, precVertexEndTime);
//			vertex->getReference()->setScheduled(true);
		}
	}

	UINT32 bestSlave;
	UINT32 bestStartTime = 0;
	UINT32 bestEndTime = -1; // Very high value.

	// Getting a slave for the vertex.
	for(int slave=0; slave<arch->getNbSlaves(); slave++){
		int slaveType = arch->getSlaveType(slave);
		// checking the constraints
		if(checkConstraint(vertex, arch, slave)){
			unsigned int startTime = std::max(schedule->getReadyTime(slave), minimumStartTime);
			unsigned int execTime = getTiming(vertex, arch, slaveType);
			unsigned int comInTime=0, comOutTime=0;
//				for(int input=0; input<vertex->getNbInputEdge(); input++){
//					comInTime += arch->getTimeCom(slave, Read, vertex->getInputEdge(input)->getTokenRate());
//				}
//				for(int output=0; output<vertex->getNbOutputEdge(); output++){
//					comOutTime += arch->getTimeCom(slave, Write, vertex->getOutputEdge(output)->getTokenRate());
//				}
			unsigned int endTime = startTime + execTime + comInTime + comOutTime;
			if(endTime < bestEndTime){
				bestSlave = slave;
				bestEndTime = endTime;
				bestStartTime = startTime;
//					bestComInTime = comInTime;
//					bestComOutTime = comOutTime;
			}
		}
	}
//		schedule->addCom(bestSlave, bestStartTime, bestStartTime+bestComInTime);
	int scheduleIndex = schedule->addSchedule(bestSlave, vertex, bestStartTime, bestEndTime);
//		schedule->addCom(bestSlave, bestEndTime-bestComOutTime, bestEndTime);
	vertex->setScheduleIndex(scheduleIndex);

	return bestEndTime;
}

void ListScheduler::schedule(SRDAGGraph* dag, BaseSchedule* schedule, Architecture* arch)
{
	int nbVertices = dag->getNbVertices();

	// Scheduling the vertices.
	for(int i=0; i<nbVertices; i++){
		SRDAGVertex* vertex = dag->getVertex(i);
		if((vertex->getScheduleIndex() == -1) &&
		   (vertex->getState() == SrVxStExecutable))
		{
			this->schedule(schedule, arch, vertex);
		}
	}
}

