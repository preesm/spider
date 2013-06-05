/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
/**
 * A CSDAG transformer transforms a CSDAG graph into an SRDAG one. 
 * It duplicates edges and breaks the repetition patterns to generate the SRDAG graph.
 * 
 * @author mpelcat
 */
#include "JobTransformer.h"
#include "../../tools/SchedulingError.h"
#include "../../tools/Timer.h"
#include <cstdio>
#include <cstdlib>

/**
 Constructor
*/
JobTransformer::JobTransformer(){
	nb_input_edges = 0;
	theJobSet = NULL;
}

/**
 Destructor
*/
JobTransformer::~JobTransformer()
{
}

/**
 finalizeJobBuilding: this is called from iterate() whenever
 a complete Job has been detected. This function transfers
 the detected Job to the repository.

*/
void JobTransformer::finalizeJobBuilding() {
	JobInstance *qJob;
	int JobType;
	unsigned int hash_n;

	if(currentJob.getOpCount() == 0) return;

	currentJob.computeHash();
	hash_n = currentJob.getHash();
	JobType = theJobSet->findJob(hash_n);

	if(JobType == -1)
	{
		int opOffset = 0;
		Job *pJob = theJobSet->addJob();

		for(int i = 0; i < currentJob.getOpCount(); i++)
		{
			int opi;
			opi = pJob->addOperation(currentJob.getOperation(i));
			pJob->setOffset(opi, opOffset);
			opOffset += theJobSet->getOperation(currentJob.getOperation(i))->getDuration();
		}
		pJob->computeHash();
		JobType = pJob->getType();
	}

// --------------------- Job queue related things ------------------------
	qJob = theJobSet->addJobToQueue(JobType);
	for(int i = 0; i < currentJob.getOpCount(); i++)
		qJob->addOperationReference(i, theJobSet->getOperation(currentJob.getOperation(i))->getReference());
// -----------------------------------------------------------------------

	currentJob.flush();
}

/**
 iterate: a self-calling function that walks through the SRDAG
          graph in the order in which flow-shop Jobs and
		  operations must be scheduled finally

 @param hGraph: the graph being scheduled
 @param vertex: the SRDAGvertex under inspection
*/
void JobTransformer::iterate(SRDAGVertex* vertex/*, int nbSlaves*/){
	if(vertex->getVisited() == true)
		return;

	for(int i = 0; i < vertex->getNbInputEdge(); i++){
		if(vertex->getInputEdge(i)->getSource()->getVisited() != true){
			// --------------- this is the actual useful part ------------
			finalizeJobBuilding();
			return;
			// -----------------------------------------------------------
		}
	}

// --------------- this is the actual useful part ------------------------
	int opType = vertex->getCsDagReference()->getFunctionIndex();
	int opi = theJobSet->findOperation(opType);
	if(opi == -1)
	{
		Operation *op = theJobSet->addOperation();
		op->setDuration(vertex->getCsDagReference()->getIntTiming(0));
		op->setType(opType);
		if(vertex->getNbInputEdge()!=0)
			op->setConso(vertex->getInputEdge(0)->getTokenRate());
		else
			op->setConso(0);
		if(vertex->getNbOutputEdge()!=0)
			op->setProd(vertex->getOutputEdge(0)->getTokenRate());
		else
			op->setProd(0);
		opi = op->getType();
	}
	theJobSet->getOperation(opi)->setReference(vertex);
	currentJob.addOperation(opType);
	
	if(vertex->getNbOutputEdge() == 0)
		finalizeJobBuilding();
// -----------------------------------------------------------------------

	vertex->setVisited(true);
/*
	if(currentJob.getOpCount() == nbSlaves){
		finalizeJobBuilding();
	}
*/

	for(int i = 0; i < vertex->getNbOutputEdge(); i++){
		SRDAGVertex* next = vertex->getOutputEdge(i)->getSink();
		iterate(next/*, nbSlaves*/);
	}
}

/**
 Transforms a CSDAG graph in DAG

 @param input: input CSDAG graph with pattern expressions on production and consumption
 @param output: output SRDAG graph with variable expressions on production and consumption
*/
void JobTransformer::transform(SRDAGGraph* input, JobSet* output, Architecture* archi){
	theJobSet = output;
	theJobSet->setSRDAG(input);
	theJobSet->resetVisited();
	iterate(theJobSet->getSRDAG()->getVertex(0)/*, archi->getNbSlaves()*/);
}
