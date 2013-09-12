/*********************************************************

Author: Jani Boutellier

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#ifndef FLOWSHOP_SCHEDULER_H
#define FLOWSHOP_SCHEDULER_H

#include "../../graphs/SRDAG/SRDAGGraph.h"
#include "../../graphs/SRDAG/SRDAGVertex.h"
#include "../../graphs/SRDAG/SRDAGEdge.h"
#include "../../graphs/CSDAG/CSDAGGraph.h"
#include "../../graphs/CSDAG/CSDAGVertex.h"
#include "../../graphs/CSDAG/CSDAGEdge.h"
#include "../../graphs/JobSet/Job.h"
#include "../../graphs/JobSet/JobSet.h"
/**
 * The scheduler assigns a slave for each actor.
 */
class FlowShopScheduler {

	private :
		/**
		 Architecture
		*/
		Architecture* archi;
		
		/**
		 Subroutine: finding the least used machine

		@param opp: pointer to the operation under processing

		 @return: the index of the least used machine
		*/
		int findLeastUsed(Operation *opp);

		/**
		 Subroutine: non-division-modulo

		@param opp: pointer to the operation under processing

		 @return: the index of the least used machine
		*/
		int modulo(Operation *opp);

		 /**
		 Perform parallel flow-shop scheduling with no-wait timetabling

		 @param theJobSet: the full collection of all known Job types
		 @param nbSlaves: number of slave units to which the scheduling is done
		 @param nbTasks: number of tasks in the csDAG
		*/
		void scheduleNoWait(JobSet *theJobSet, int nbSlaves, int nbTasks);

		/**
		 Perform parallel flow-shop scheduling with semi-active timetabling

		 @param theJobSet: the full collection of all known Job types
		 @param nbSlaves: number of slave units to which the scheduling is done
		 @param nbTasks: number of tasks in the csDAG
		*/
		void scheduleSemiActive(JobSet *theJobSet, int nbSlaves, int nbTasks);

		void propagateSchedule(JobSet *theJobSet, SRDAGVertex* vertex, int proc, int setId);

		/**
		 Current computation termination time for each slave
		*/
		int slaveEndTime[MAX_SLAVES];

		int slaveStartTime[MAX_SLAVES];

	public : 
		/**
		 Constructor
		*/
		FlowShopScheduler();


		/**
		 Destructor
		*/
		~FlowShopScheduler();

		/**
		 Computing a vertex t-level from its input edges

		 @param vertex: vertex for which t-level is recomputed
		 @param firstInputEdge: vertex input edges in a linked list
		 @param precedenceEdge: the current precedence edge

		 @return the incremented first input edge
		*/
		SRDAGEdge* recomputeTLevel(SRDAGVertex* vertex, SRDAGEdge* firstInputEdge, SRDAGEdge* precedenceEdge);

		/**
		 Sets the slave architecture

		 @param archi: slave architecture
		*/
		void setArchitecture(Architecture* archi);

		/**
		 Return the schedule makespan

		 @return: the makespan
		*/
		int getMakespan();

		int getThroughput();

		/**
		 Scheduling a SRDAG

		 @param csDag: the reference CSDAG graph containing timings and constraints
		 @param hGraph: the graph being scheduled
		*/
		void schedule(CSDAGGraph* csDag, JobSet* hGraph);
};

/**
Jani: may need to be modified; if you want to delay action execution

 Computing a vertex t-level (earliest starting time) from its input edges

 @param vertex: vertex for which t-level is recomputed
 @param firstInputEdge: vertex input edges in a linked list
 @param precedenceEdge: the current precedence edge

 @return the incremented first input edge
*/
inline
SRDAGEdge* FlowShopScheduler::recomputeTLevel(SRDAGVertex* vertex, SRDAGEdge* firstInputEdge, SRDAGEdge* precedenceEdge){
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
//		int comTiming = comRate * currentInputEdge->getTokenRate() >> 6;
		currentTLevel = currentPred->getTLevel() + 
						currentPred->getCsDagReference()->getIntTiming(sourceSlaveTypeId) ;//+ comTiming;
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


#endif
