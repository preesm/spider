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
#include "../../graphs/Schedule/Schedule.h"
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

		inline void scheduleVertex(
				SRDAGGraph* hGraph,
				SRDAGVertex* vertex,
				Schedule* schedule,
				Architecture* arch);
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
		void schedule(SRDAGGraph* hGraph, Schedule* schedule, Architecture* arch);
		void schedule(CSDAGGraph* csGraph, SRDAGGraph* hGraph);
};




#endif
