#ifndef JOB_SET
#define JOB_SET

#include "../SRDAG/SRDAGVertex.h"
#include "../SRDAG/SRDAGEdge.h"
#include "../../SchedulerDimensions.h"
#include "../../tools/SchedulingError.h"

#include "Job.h"
#include "JobInstance.h"
#include "MachineSet.h"

/**
 * A JobSet. A set of Jobs to be scheduled.
 * 
 * @author jboutell
 */
class JobSet {

	friend class FlowShopScheduler;

	private :

		SRDAGGraph* srDag;
		/**
		 The number of all Job types
		*/
		int JobCount;

		/**
		 The number of all Operation types
		*/
		int opCount;

		/**
		 the list of Jobs to be scheduled. Contains practically
		 references to the Job repository (JobSet) and to the
		 SRDAG vertices
		*/
		JobInstance JobQueue[MAX_DAG_VERTEX_REPETITION];

		/**
		 the number of entries used from JobQueue
		*/
		int qLength;

		/**
		 The set of all Job types; i.e. the Job "repository"
		*/
		Job Jobs[MAX_DAG_VERTEX_REPETITION];

		/**
		 The set of all Operation types
		*/
		Operation ops[MAX_CSDAG_VERTICES];

		int machineSetCount;

		MachineSet machineSets[MAX_CSDAG_VERTICES];

	public :

		/**
		 Constructor
		*/
		JobSet();

		/**
		 Destructor
		*/
		virtual ~JobSet();

		/**
		 Resets the structure
		*/
		void flush();

		void printJobType();
		void printMapping();

		/**
		 Adds a new Job to the JobSet.

		 @return: a pointer to the added Job
		*/
		Job* addJob();

		int addMachineSet();
		MachineSet* getMachineSet(int id);

		/**
		 Adds a new Operation to the JobSet.

		 @return: a pointer to the added op
		*/
		Operation* addOperation();

		/**
		 Retrieves a pointer to a specific Job instance.

		 @return: a pointer to the Job
		*/
		Job* getJob(int index);

		/**
		 Retrieves a pointer to a specific Operation instance.

		 @return: a pointer to the Operation
		*/
		Operation* getOperation(int index);

		/**
		 Get Operation count
		 
		 @return: the Operation count
		*/
		int getOpCount();

		/**
		 Get number of known Job types

		 @return: number of Job types
		*/
		int getJobCount();

		//TaskCache taskCache[MAX_CSDAG_VERTICES];

		/**
		 Adds a new Job to the Job Job queue.

		 @return: a pointer to the added Job
		*/
		JobInstance* addJobToQueue(int type);

		/**
		 Removes the last added edge
		*/
		void removeLastEdge();

		/**
		 getJobFrombQueue

		 @ret: a pointer to the requested Job instance
		*/
		JobInstance *getJobFromQueue(int index);

		/**
		 getQueueLength

		 @ret: the Job queue length
		*/
		int getQueueLength();

		/**
		 findJob

		 @param hash_n: the hash code of the Job which is to be looked for
		 @ret: -1 if the Job was not found, otherwise the index of the existing Job
		*/
		int findJob(unsigned int hash_n);

		/**
		 findOperation

		 @param type: the type of Operation Job which is to be looked for
		 @ret: -1 if the Operation was not found, otherwise the index of the existing op
		*/
		int findOperation(int type);


		SRDAGGraph* getSRDAG();

		void setSRDAG(SRDAGGraph* srDag);

		/**
		 Reset the "visited" status of all vertices
		*/
		void resetVisited();

};

inline SRDAGGraph* JobSet::getSRDAG(){
	return srDag;
}

inline void JobSet::setSRDAG(SRDAGGraph* s){
	srDag = s;
}

/**
 Reset the "visited" status of all vertices
*/
inline void JobSet::resetVisited(){
	srDag->resetVisited();
}

#endif
