
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


/**
 * The scheduler assigns a slave for each actor.
 * 
 * @author mpelcat
 */
#include "ListScheduler.h"
#include <algorithm>    // std::min

/**
 Constructor

 @param nbSlaves: current number of slaves
*/
ListScheduler::ListScheduler()
{
	archi = NULL;
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
	int max = 0;
	for(int i = 0; i < srDag->getNbVertices(); i++)
		if(srDag->getVertex(i)->getTLevel() + srDag->getVertex(i)->getCsDagReference()->getIntTiming(0) > max)
			max = srDag->getVertex(i)->getTLevel() + srDag->getVertex(i)->getCsDagReference()->getIntTiming(0);

	return max;
}

int ListScheduler::getThroughput(SRDAGGraph* srDag){
	int slaveEndTime[MAX_SLAVES];
	int slaveStartTime[MAX_SLAVES];

	memset(slaveEndTime, 0, MAX_SLAVES*sizeof(int));
	memset(slaveStartTime, -1, MAX_SLAVES*sizeof(int));

	for(int i=0;  i < srDag->getNbVertices(); i++){
		int slaveId = srDag->getVertex(i)->getSlaveIndex();
		int startVertex = srDag->getVertex(i)->getTLevel();
		int endVertex = startVertex + srDag->getVertex(i)->getCsDagReference()->getIntTiming(0);

		if(endVertex >  slaveEndTime[slaveId]){
			slaveEndTime[slaveId] = endVertex;
		}
		if(slaveStartTime[slaveId] == -1 || startVertex < slaveStartTime[slaveId]){
			slaveStartTime[srDag->getVertex(i)->getSlaveIndex()] = startVertex;
		}
	}

	int max = 0;
	for(int i = 0; i < archi->getNbSlaves(); i++)
		if(slaveStartTime[i] != -1)
			max = std::max(slaveEndTime[i] - slaveStartTime[i], max);
	return max;
}


/**
 Computing a vertex t-level from its input edges

 @param vertex: vertex for which t-level is recomputed
 @param firstInputEdge: vertex input edges in a linked list
 @param precedenceEdge: the current precedence edge

 @return the incremented first input edge
*/
inline
SRDAGEdge* ListScheduler::recomputeTLevel(SRDAGVertex* vertex, SRDAGEdge* firstInputEdge, SRDAGEdge* precedenceEdge){
	int maxTLevel = 0;
	int currentTLevel;
//	int currentSlaveIndex = vertex->getSlaveIndex();
	SRDAGEdge* currentInputEdge = firstInputEdge;

	// iterating the input edges to calculate the one that gives the highest t-level to vertex;
	while(currentInputEdge != NULL && currentInputEdge->getSink() == vertex){
		SRDAGVertex* currentPred = currentInputEdge->getSource();

		// The source slave id is necessary to get the timing of the current vertex
		int sourceSlaveTypeId = archi->getSlaveType(currentPred->getSlaveIndex());
//todo	int comRate = archi->getComRate(currentPred->getSlaveIndex(),currentSlaveIndex);
//		int comTiming = archi->getTimeCom(currentPred->getSlaveIndex(),currentSlaveIndex);

		currentTLevel = currentPred->getTLevel() +
						currentPred->getCsDagReference()->getIntTiming(sourceSlaveTypeId);// +
//						comTiming;

		if(currentTLevel>maxTLevel){
			maxTLevel = currentTLevel;
		}
		currentInputEdge = currentInputEdge->nextInSinkOrder;
	}

	// Adding the precedence edge with the same calculation
	if(precedenceEdge->getSource() != NULL){
		SRDAGEdge* currentInputEdge = precedenceEdge;
		SRDAGVertex* currentPred = currentInputEdge->getSource();

		int sourceSlaveTypeId = archi->getSlaveType(currentPred->getSlaveIndex());
		currentTLevel = currentPred->getTLevel() +
						currentPred->getCsDagReference()->getIntTiming(sourceSlaveTypeId);
		if(currentTLevel>maxTLevel){
			maxTLevel = currentTLevel;
		}
	}

	vertex->setTLevel(maxTLevel);

	return currentInputEdge;
}

/**
 Scheduling a SRDAG

 @param csGraph: the reference CSDAG graph containing timings and constraints
 @param hGraph: the graph being scheduled
*/
void ListScheduler::schedule(CSDAGGraph* csGraph, SRDAGGraph* hGraph){
	int nbSlaves = archi->getNbSlaves();

	// Resetting references to the last vertex for each slave
	memset(lastVertexOfSlave,'\0',MAX_SLAVES*sizeof(SRDAGVertex*));

	int nbVertices = hGraph->getNbVertices();
	// Getting all edges in their sink order: facilitates input edges retrieval:
	// the edges are in a linked list in the right order
	SRDAGEdge* currentInputEdge = SRDAGEdge::firstInSinkOrder;
	SRDAGEdge* nextInputEdge;

	// Adding precedence edges while mapping/scheduling
	SRDAGEdge currentPrecedenceEdge;

	// Iterating the vertices
	for(int i=0; i<nbVertices; i++){
		SRDAGVertex* vertex = hGraph->getVertex(i);
		int startTime = 0x7fffffff; // Careful! for signed 32 bit ints
		int earliestStartTimeIndex;

		// iterating the slaves
		for(int j=0; j<nbSlaves; j++){
			// Invalidating the currentPrecedenceEdge
			currentPrecedenceEdge.setSource(NULL);

			// checking the constraints
			if(vertex->getCsDagReference()->getConstraint(j) != 0){
				// Implementing the current vertex on the current slave
				vertex->setSlaveIndex(j);
				SRDAGVertex* lastVertex = lastVertexOfSlave[j];
				// Adding a precedence edge
				if(lastVertex != NULL){
					currentPrecedenceEdge.setSource(lastVertex);
					currentPrecedenceEdge.setTokenRate(-1);
					currentPrecedenceEdge.setSink(vertex);
				}

				// Going to the first input edge (should not be called in normal use)
				// If i == 0, vertex is first in the graph and thus has no input edge
				while(i != 0 && currentInputEdge!= NULL && currentInputEdge->getSink() != vertex){
					currentInputEdge = currentInputEdge->nextInSinkOrder;
				}
				// The new TLevel is computed from input edges. Input edges are retrieved
				// in order in a linked list. Pointer on the linked list is updated for the next
				// computation
				nextInputEdge = recomputeTLevel(vertex, currentInputEdge,&currentPrecedenceEdge);

				// calculating the start time and keeping the earliest
				int newStartTime = vertex->getTLevel();
				if(newStartTime < startTime){
					startTime = newStartTime;
					earliestStartTimeIndex = j;
				}
			}
		}

		currentPrecedenceEdge.setSource(NULL);
		// Adding a precedence edge
		if(lastVertexOfSlave[earliestStartTimeIndex] != NULL){
			currentPrecedenceEdge.setSource(lastVertexOfSlave[earliestStartTimeIndex]);
			currentPrecedenceEdge.setTokenRate(-1);
			currentPrecedenceEdge.setSink(vertex);
		}

		// Setting the current vertex as the last one to be executed on slave earliestStartTimeIndex
		lastVertexOfSlave[earliestStartTimeIndex] = vertex;


		if(vertex->getSlaveIndex() != earliestStartTimeIndex){
			vertex->setSlaveIndex(earliestStartTimeIndex);
			nextInputEdge = recomputeTLevel(vertex, currentInputEdge, &currentPrecedenceEdge);
		}
		currentInputEdge = nextInputEdge;
	}
}

/**
 Scheduling a SRDAG

 @param csGraph: the reference CSDAG graph containing timings and constraints
 @param hGraph: the graph being scheduled
*/
void ListScheduler::schedule(SRDAGGraph* hGraph, Schedule* schedule, Architecture* arch){
	int nbVertices = hGraph->getNbVertices();

	// Adding precedence edges while mapping/scheduling
	SRDAGEdge currentPrecedenceEdge;

	// Iterating the vertices
	for(int i=0; i<nbVertices; i++){
		SRDAGVertex* vertex = hGraph->getVertex(i);

		int bestSlave;
		unsigned int bestStartTime=-1;
		unsigned int bestEndTime=-1;
		unsigned int bestComInTime=-1;
		unsigned int bestComOutTime=-1;

		/* Getting minimum start time due to precedent vertices */
		unsigned int minimumStartTime = 0;
		for(int i=0; i<vertex->getNbInputEdge(); i++){
			SRDAGVertex* precVertex = vertex->getInputEdge(i)->getSource();
			minimumStartTime = std::max(
					minimumStartTime,
					schedule->getVertexEndTime(precVertex->getBase()->getVertexIndex(precVertex))
					);
		}

		// Iterating the slaves
		for(int slave=0; slave<arch->getNbActiveSlaves(); slave++){
			int slaveType = arch->getSlaveType(slave);
			// checking the constraints
			if(vertex->getCsDagReference()->getConstraint(slave) != 0){
				unsigned int startTime = std::max(schedule->getReadyTime(slave), minimumStartTime);
				unsigned int execTime = vertex->getCsDagReference()->getIntTiming(slaveType);
				unsigned int comInTime=0, comOutTime=0;
				for(int input=0; input<vertex->getNbInputEdge(); input++){
					comInTime += arch->getTimeCom(slave, Read, vertex->getInputEdge(input)->getTokenRate());
				}
				for(int output=0; output<vertex->getNbOutputEdge(); output++){
					comOutTime += arch->getTimeCom(slave, Write, vertex->getOutputEdge(output)->getTokenRate());
				}
				unsigned int endTime = startTime + execTime + comInTime + comOutTime;
				if(endTime < bestEndTime){
					bestSlave = slave;
					bestEndTime = endTime;
					bestStartTime = startTime;
					bestComInTime = comInTime;
					bestComOutTime = comOutTime;
				}
			}
		}

		schedule->addCom(bestSlave, bestStartTime, bestStartTime+bestComInTime);
		schedule->addVertex(bestSlave, vertex, bestStartTime, bestEndTime);
		schedule->addCom(bestSlave, bestEndTime-bestComOutTime, bestEndTime);
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
		if(! scenario->getConstraints(vertex->getReference()->getId(), slave)){
			unsigned int startTime = std::max(schedule->getReadyTime(slave), minimumStartTime);
			unsigned int execTime = scenario->getTiming(vertex->getReference()->getId(), slaveType);
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


void ListScheduler::schedule(BaseVertex** vertices, UINT32 nbVertices, BaseSchedule* schedule)
{
	// Iterating the vertices
	for(UINT32 i=0; i<nbVertices; i++){
		BaseVertex* vertex = vertices[i];

//		this->schedule(schedule, , vertex);
//		int bestSlave;
//		unsigned int bestStartTime=-1;
//		unsigned int bestEndTime=-1;
//		unsigned int bestComInTime=-1;
//		unsigned int bestComOutTime=-1;

////		/* Getting minimum start time due to precedent vertices */
//		unsigned int minimumStartTime = 0;
////		for(int i=0; i<vertex->getNbInputEdge(); i++){
////			BaseVertex* precVertex = vertex->getInputEdge(i)->getSource();
////			minimumStartTime = std::max(
////					minimumStartTime,
////					schedule->getVertexEndTime(precVertex->getBase()->getVertexIndex(precVertex))
////					);
////		}
//
//		// Iterating the slaves
//		for(int slave=0; slave<archi->getNbActiveSlaves(); slave++){
//			int slaveType = archi->getSlaveType(slave);
//			// checking the constraints
//			if(not scenario->getConstraints(vertex->getId(), slave)){
//				unsigned int startTime = std::max(schedule->getReadyTime(slave), minimumStartTime);
//				unsigned int execTime = scenario->getTiming(vertex->getId(), slaveType);
//				unsigned int comInTime=0, comOutTime=0;
////				for(int input=0; input<vertex->getNbInputEdge(); input++){
////					comInTime += arch->getTimeCom(slave, Read, vertex->getInputEdge(input)->getTokenRate());
////				}
////				for(int output=0; output<vertex->getNbOutputEdge(); output++){
////					comOutTime += arch->getTimeCom(slave, Write, vertex->getOutputEdge(output)->getTokenRate());
////				}
//				unsigned int endTime = startTime + execTime + comInTime + comOutTime;
//				if(endTime < bestEndTime){
//					bestSlave = slave;
//					bestEndTime = endTime;
//					bestStartTime = startTime;
//					bestComInTime = comInTime;
//					bestComOutTime = comOutTime;
//				}
//			}
//		}
//
////		schedule->addCom(bestSlave, bestStartTime, bestStartTime+bestComInTime);
//		schedule->addVertex(bestSlave, vertex, bestStartTime, bestEndTime);
////		schedule->addCom(bestSlave, bestEndTime-bestComOutTime, bestEndTime);
	}
}


