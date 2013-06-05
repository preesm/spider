/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#ifndef LIST_SCHEDULER
#define LIST_SCHEDULER

#include "../../graphs/SRDAG/SRDAGGraph.h"
#include "../../graphs/SRDAG/SRDAGVertex.h"
#include "../../graphs/SRDAG/SRDAGEdge.h"
#include "../../graphs/CSDAG/CSDAGGraph.h"
#include "../../graphs/CSDAG/CSDAGVertex.h"
#include "../../graphs/CSDAG/CSDAGEdge.h"
/**
 * The scheduler assigns a slave for each actor.
 * 
 * @author mpelcat
 */
class ListScheduler {

	private :
		/**
		 Architecture
		*/
		Architecture* archi;
		
		/**
		 Temporary storage for the vertex executed last on each slave
		*/
		SRDAGVertex* lastVertexOfSlave[MAX_SLAVES];
	public : 
		/**
		 Constructor
		*/
		ListScheduler();


		/**
		 Destructor
		*/
		~ListScheduler();

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
		int getMakespan(SRDAGGraph* hGraph);

		int getThroughput(SRDAGGraph* hGraph);

		/**
		 Scheduling a SRDAG

		 @param csGraph: the reference CSDAG graph containing timings and constraints
		 @param hGraph: the graph being scheduled
		*/
		void schedule(CSDAGGraph* csGraph, SRDAGGraph* hGraph);
};

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
	int currentSlaveIndex = vertex->getSlaveIndex();
	SRDAGEdge* currentInputEdge = firstInputEdge;

	// iterating the input edges to calculate the one that gives the highest t-level to vertex;
	while(currentInputEdge != NULL && currentInputEdge->getSink() == vertex){
		SRDAGVertex* currentPred = currentInputEdge->getSource();

		// The source slave id is necessary to get the timing of the current vertex
		int sourceSlaveTypeId = archi->getSlaveType(currentPred->getSlaveIndex());
		int comRate = archi->getComRate(currentPred->getSlaveIndex(),currentSlaveIndex);
		int comTiming = comRate * currentInputEdge->getTokenRate() >> 6;
		currentTLevel = currentPred->getTLevel() + 
						currentPred->getCsDagReference()->getIntTiming(sourceSlaveTypeId) + comTiming;
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
