/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
/**
 * A SRDAG graph. It contains SRDAG vertices and edges. It has a bigger table for vertices and edges than DAG.
 * Each edge production and consumption must be equal. There is no repetition vector for the vertices.
 * 
 * @author mpelcat
 */

#include "../SRDAG/SRDAGGraph.h"
#include "../SRDAG/SRDAGVertex.h"
#include "../SRDAG/SRDAGEdge.h"

#include "JobSet.h"
#include <stdio.h>
#include <string.h>

/**
 Constructor
*/
JobSet::JobSet()
{
	machineSetCount=MAX_CSDAG_VERTICES;
	JobCount=MAX_DAG_VERTEX_REPETITION;
	opCount=MAX_CSDAG_VERTICES;
	flush();
}

/**
 Destructor
*/
JobSet::~JobSet()
{
}

/**
 Adds a new Job to the JobSet.

 @return: a pointer to the added Job
*/
Job* JobSet::addJob()
{
	Jobs[JobCount].setType(JobCount);
	return &Jobs[JobCount++];
}


int JobSet::addMachineSet(){
	return machineSetCount++;
}

MachineSet* JobSet::getMachineSet(int id){
	return &machineSets[id];
}

/**
 Adds a new Operation to the JobSet.

 @return: a pointer to the added op
*/
Operation* JobSet::addOperation()
{
	return &ops[opCount++];
}

/**
 Print All Jobs into stdout
*/
void JobSet::printJobType(){
	printf("List of Jobs types (%d Jobs):\n", qLength);
	int* tab = new int[JobCount];
	memset(tab, 0, JobCount*sizeof(int));
	for(int j=0; j<qLength; j++){
		tab[JobQueue[j].getJobType()]++;
	}
	for(int i=0; i<JobCount; i++){
		printf("%d: ",i);
		for(int j=0; j<Jobs[i].getOpCount(); j++){
			printf("%d ", Jobs[i].getOperation(j));
		}
		printf(" -> %d times", tab[i]);
		printf("\n");
	}
}

void JobSet::printMapping(){
	for(int i=0; i<machineSetCount; i++){
		if(machineSets[i].getCount() == 1)
			printf("Set %d: Core n° %d \n", i, machineSets[i].getStart());
		else
			printf("Set %d: Core n° %d -> %d\n", i, machineSets[i].getStart(), machineSets[i].getLast());
	}
	for(int i=0; i<getOpCount(); i++){
		printf("Op %d -> Core Set %d\n", i, getOperation(i)->getMachineSetId());
	}
}

/**
 Adds a new Job to the JobSet.

 @return: a pointer to the added Job
*/
JobInstance* JobSet::addJobToQueue(int type)
{
	JobQueue[qLength].setJobType(type);
	return &JobQueue[qLength++];
}


/**
 Removes all edges and vertices
*/
void JobSet::flush(){
	qLength = 0;
	machineSetCount=0;
	for(int i = 0; i < JobCount; i++)
		Jobs[i].flush();
	JobCount = 0;
	for(int i = 0; i < opCount; i++)
		ops[i].flush();
	opCount = 0;
}

/**
 getJobFrombQueue

 @ret: a pointer to the requested Job instance
*/
JobInstance* JobSet::getJobFromQueue(int index)
{
	return &JobQueue[index];
}

/**
 getQueueLength

 @ret: the Job queue length
*/
int JobSet::getQueueLength()
{
	return qLength;
}

/**
 Retrieves a pointer to a specific Job instance.

 @return: a pointer to the Job
*/
Job* JobSet::getJob(int index)
{
	return &Jobs[index];
}

/**
 Retrieves a pointer to a specific Operation instance.

 @return: a pointer to the Operation
*/
Operation* JobSet::getOperation(int index)
{
	return &ops[index];
}

/**
 Get number of known Job types

 @return: number of Job types
*/
int JobSet::getJobCount()
{
	return JobCount;
}

/**
 Get Operation count
 
 @return: the Operation count
*/
int JobSet::getOpCount()
{
	return opCount;
}

/**
 findJob

 @param hash_n: the hash code of the Job which is to be looked for
 @ret: -1 if the Job was not found, otherwise the index of the existing Job
*/
int JobSet::findJob(unsigned int hash_n)
{
	for(int i = 0; i < JobCount; i++)
		if(Jobs[i].getHash() == hash_n)
			return i;
	return -1;
}

/**
 findOperation

 @param type: the type of Operation Job which is to be looked for
 @ret: -1 if the Operation was not found, otherwise the index of the existing op
*/
int JobSet::findOperation(int type)
{
	for(int i = 0; i < opCount; i++)
		if(ops[i].getType() == type)
			return i;
	return -1;
}
