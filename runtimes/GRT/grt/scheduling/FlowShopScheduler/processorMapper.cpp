/*********************************************************

Author: Jani Boutellier

This class performs iterative mapping of a set of operations
to a set of slave processors.

 *********************************************************/
 
/**
 * The scheduler assigns a slave for each actor.
 */
#include <cstdlib>
#include <cmath>
#include <algorithm>

#include "processorMapper.h"
#include <Timer.h>

//#define AUTOMATIC_MAPPING
//#define PRINTOUT_MAPPING

/**
 Constructor

 @param nbSlaves: current number of slaves
*/
ProcessorMapper::ProcessorMapper()
{
}

/**
 Destructor
*/
ProcessorMapper::~ProcessorMapper()
{
}

/**
 A task (operation) that has a latency less than SHORT_TASK_DURATION, does not get a processor of its own
*/

void ProcessorMapper::do7Mapping(JobSet *theJobSet){
	theJobSet->addMachineSet();
	theJobSet->addMachineSet();
	theJobSet->addMachineSet();
	theJobSet->addMachineSet();
	theJobSet->addMachineSet();
	theJobSet->getMachineSet(0)->setMachines(0,1);
	theJobSet->getMachineSet(1)->setMachines(1,2);
	theJobSet->getMachineSet(2)->setMachines(3,1);
	theJobSet->getMachineSet(3)->setMachines(4,2);
	theJobSet->getMachineSet(4)->setMachines(6,1);
	theJobSet->getOperation(0)->setMachineSet(0);
	theJobSet->getOperation(1)->setMachineSet(0);
	theJobSet->getOperation(2)->setMachineSet(1);
	theJobSet->getOperation(3)->setMachineSet(2);
	theJobSet->getOperation(4)->setMachineSet(3);
	theJobSet->getOperation(5)->setMachineSet(4);
}

/**
 Perform an iterative process to map operation types (CSDAG vertices) to processors 

 @param hGraph: the SRDAG graph
 @param nbSlaves: current number of slaves
 @param taskCount: number of vertices in the CSDAG
*/

void ProcessorMapper::iterateMapping (JobSet* theJobSet, Operation* op){
	// Return if the operation already get a MachineSet
	if(op->getMachineSetId() != -1) return;
	int maxMachineSet = 0;

	// Get the same MachineSet as the highest MachineSet of its inputs operations
	for(int i=0; i<op->vertex->getNbInputEdge(); i++){
		SRDAGVertex* inputVertex = op->vertex->getInputEdge(i)->getSource();
		Operation* inputOp = theJobSet->getOperation(inputVertex->getCsDagReference()->getFunctionIndex());
		if(inputOp->getMachineSetId() == -1) iterateMapping(theJobSet, inputOp);

		maxMachineSet = (inputOp->getMachineSetId() >  maxMachineSet) ?
				(inputOp->getMachineSetId()) : (maxMachineSet);
	}
	op->setMachineSet(maxMachineSet);
}

void ProcessorMapper::automaticMapping(SRDAGGraph* hGraph, int nbSlaves, JobSet *theJobSet){
	int taskCount = theJobSet->getOpCount();
	int timeOp[MAX_CSDAG_VERTICES];
	int timeSum[MAX_CSDAG_VERTICES];
	int timeSumSum=0;
	int nbOp[MAX_CSDAG_VERTICES];
	int alloc[MAX_CSDAG_VERTICES];
	int runningIndex = 0;
	int nbVertices = hGraph->getNbVertices();
	int procsLeft = nbSlaves;

	memset(timeSum, 0, sizeof(int)*MAX_CSDAG_VERTICES);
	memset(nbOp, 0, sizeof(int)*MAX_CSDAG_VERTICES);
		
	// Getting some statistics of the execution
	for(int i=0; i<nbVertices; i++){
		int csIndex;
		SRDAGVertex* vertex = hGraph->getVertex(i);
		csIndex = vertex->getCsDagReference()->getFunctionIndex();
		timeSum[csIndex] += vertex->getCsDagReference()->getIntTiming(0);
		timeOp[csIndex] = vertex->getCsDagReference()->getIntTiming(0);
		timeSumSum += vertex->getCsDagReference()->getIntTiming(0);
		nbOp[csIndex]++;
	}

	// First core get always a core
	alloc[0]=1;
	procsLeft--;

	// Then allocate 1 core to cores that have enough computing demand
	for(int i=1; i<taskCount ; i++){
		if(procsLeft && timeSum[i] > 0.5*timeSumSum/(nbSlaves-1)){
			alloc[i] = 1;
			procsLeft --;
		}else{
			alloc[i] = 0;
		}
	}

	// Allocate all the remaining processors
	while(procsLeft > 0)	{
		float cost[MAX_CSDAG_VERTICES];
		for(int i=1; i<taskCount; i++){
			if(alloc[i]==0 || alloc[i] == nbOp[i]){
				cost[i]=0;
			}else{
				/*if(i == taskCount-1) cost[i] = (float)timeOp[i]*nbOp[i] / alloc[i];
				else */
				cost[i] = (float)timeOp[i]*nbOp[i] / alloc[i];
			}
		}

		int maxCost=1;
		for(int i=2; i<taskCount; i++){
			if(cost[maxCost] < cost[i]) maxCost=i;
			//else if(cost[maxCost] == cost[i] && timeOp[maxCost] < timeOp[i]) maxCost=i;
		}

		alloc[maxCost]++;
		procsLeft--;
	}

	// Create MachineSet and assign them
	for(int i=0; i < taskCount; i++){
		if(alloc[i] > 0){
			int machineSetId = theJobSet->addMachineSet();
			theJobSet->getMachineSet(machineSetId)->setMachines(runningIndex,alloc[i]);
			theJobSet->getOperation(i)->setMachineSet(machineSetId);
			runningIndex += alloc[i];
		}
	}

	// Iteratively assign MachineSet to Operation which didn't have one
	for(int i=1; i < theJobSet->getOpCount(); i++){
		iterateMapping(theJobSet, theJobSet->getOperation(i));
	}
}

/**
 Call the mapping function -- manual or automatic

 @param hGraph: the SRDAG graph
 @param theJobSet: the mapping result is written to the Job reposiroty carried by this pointer
 @param nbSlaves: current number of slaves
 @param taskCount: number of vertices in the CSDAG
*/
void ProcessorMapper::doMapping(SRDAGGraph* hGraph, JobSet *theJobSet, int nbSlaves, int taskCount){
	automaticMapping(hGraph, nbSlaves, theJobSet);
}
