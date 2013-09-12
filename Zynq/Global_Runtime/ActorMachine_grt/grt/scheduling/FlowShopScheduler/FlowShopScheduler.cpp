/*********************************************************

Author: Jani Boutellier

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
/**
 * The scheduler assigns a slave for each actor.
 */
#include "FlowShopScheduler.h"
#include <algorithm>    // std::max

//#define LEAST_USED
#define SEMI_ACTIVE

/**
 Constructor

 @param nbSlaves: current number of slaves
*/
FlowShopScheduler::FlowShopScheduler(){
	archi = NULL;
}

/**
 Destructor
*/
FlowShopScheduler::~FlowShopScheduler()
{
}

/**
 Return the schedule makespan

 @return: the makespan
*/
int FlowShopScheduler::getMakespan(){
	int max = 0;
	for(int i = 0; i < archi->getNbSlaves(); i++)
		if(slaveEndTime[i] > max)
			max = slaveEndTime[i];
	return max;
}

int FlowShopScheduler::getThroughput(){
	int max = 0;
	for(int i = 0; i < archi->getNbSlaves(); i++)
		if(slaveStartTime[i] != -1 && (slaveEndTime[i] - slaveStartTime[i]) > max)
			max = (slaveEndTime[i] - slaveStartTime[i]);
	return max;
}

/**
 Sets the slave architecture

 @param archi: slave architecture
*/
void FlowShopScheduler::setArchitecture(Architecture* archi){
	this->archi = archi;
}

/**
 Scheduling a SRDAG

 @param csGraph: the reference CSDAG graph containing timings and constraints
 @param hGraph: the graph being scheduled
*/
void FlowShopScheduler::schedule(CSDAGGraph* csGraph, JobSet* theJobSet){
	scheduleSemiActive(theJobSet, archi->getNbSlaves(), csGraph->getNbVertices());
}


void FlowShopScheduler::propagateSchedule(JobSet *theJobSet, SRDAGVertex* vertex, int proc, int setId){
	for(int i=0; i<vertex->getNbOutputEdge(); i++){
		SRDAGVertex* successor = vertex->getOutputEdge(i)->getSink();
		Operation* succOp = &theJobSet->ops[successor->getCsDagReference()->getFunctionIndex()];
		if(succOp->getMachineSetId() == setId){
			if(successor->getSlaveIndex() != -1){
				if(successor->getSlaveIndex() != proc){
					printf("error\n");
				}
			}else{
				successor->setSlaveIndex(proc);
				propagateSchedule(theJobSet, successor, proc, setId);
			}
		}
	}
	for(int i=0; i<vertex->getNbInputEdge(); i++){
		SRDAGVertex* predecessor = vertex->getInputEdge(i)->getSource();
		Operation* predOp = &theJobSet->ops[predecessor->getCsDagReference()->getFunctionIndex()];
		if(predOp->getMachineSetId() == setId){
			if(predecessor->getSlaveIndex() != -1){
				if(predecessor->getSlaveIndex() != proc){
					printf("error\n");
				}
			}else{
				predecessor->setSlaveIndex(proc);
				propagateSchedule(theJobSet, predecessor, proc, setId);
			}
		}
	}
}

/**
 Perform parallel flow-shop scheduling with semi-active timetabling

 @param theJobSet: the full collection of all known Job types
 @param nbSlaves: number of slave units to which the scheduling is done
 @param nbTasks: number of tasks in the csDAG
*/
void FlowShopScheduler::scheduleSemiActive(JobSet *theJobSet, int nbSlaves, int nbTasks){
	Operation *ops = theJobSet->ops;
    memset(slaveEndTime, 0, nbSlaves*sizeof(int));
    memset(slaveStartTime, -1, nbSlaves*sizeof(int));
    ops[nbTasks].endTime = 0;

    for(int i=0; i<theJobSet->getSRDAG()->getNbVertices(); i++){
    	theJobSet->getSRDAG()->getVertex(i)->setSlaveIndex(-1);
    }

	for(int JobInd = 0; JobInd < theJobSet->qLength; JobInd++)
	{
		JobInstance *qjp = &theJobSet->JobQueue[JobInd];
		Job* jtp = &theJobSet->Jobs[qjp->jobType];

		for(int opInd = 0; opInd < jtp->opCount; opInd++){
			int predecessorTime=0;
			int time = 0;
			int op = jtp->opps[opInd];
			int proc =	qjp->operations[opInd]->getSlaveIndex();

			MachineSet* mySet = theJobSet->getMachineSet(ops[op].getMachineSetId());

			for(int i=0; i<qjp->operations[opInd]->getNbInputEdge(); i++){
				SRDAGVertex* predecessor = qjp->operations[opInd]->getInputEdge(i)->getSource();
				predecessorTime = max(predecessorTime, predecessor->getTLevel()+predecessor->getCsDagReference()->getIntTiming(0));
			}

			if(proc == -1){
				int minTime = std::max(slaveEndTime[mySet->getStart()],predecessorTime);
				proc = mySet->getStart();
				for(int i = mySet->getStart(); i <= mySet->getLast(); i++){
					int tmpTime = std::max(slaveEndTime[i],time);
					if(tmpTime < minTime){
						proc = i;
						minTime = tmpTime;
					}
				}
				qjp->operations[opInd]->setSlaveIndex(proc);
				propagateSchedule(theJobSet,  qjp->operations[opInd], proc, ops[op].getMachineSetId());
			}

			time = max(predecessorTime, slaveEndTime[proc]);
			qjp->operations[opInd]->setTLevel(time);


			if(slaveStartTime[proc] == -1) slaveStartTime[proc] = time;
			slaveEndTime[proc] = time + qjp->operations[opInd]->getCsDagReference()->getIntTiming(0);
		}
	}
}


