/*********************************************************

Author: Jani Boutellier

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#ifndef PROCESSOR_MAPPER_H
#define PROCESSOR_MAPPER_H

#include "../../graphs/SRDAG/SRDAGGraph.h"
#include "../../graphs/JobSet/JobSet.h"
#include "../../graphs/CSDAG/CSDAGVertex.h"

class ProcessorMapper {

	private :


		/**
		 Perform an iterative process to map operation types (CSDAG vertices) to processors 

		 @param hGraph: the SRDAG graph
		 @param nbSlaves: current number of slaves
		 @param taskCount: number of vertices in the CSDAG
		*/
		void automaticMapping(SRDAGGraph* hGraph, int nbSlaves, JobSet *theJobSet);

		void iterateMapping(JobSet* theJobSet, Operation* op);

	public : 
		/**
		 Constructor
		*/
		ProcessorMapper();

		/**
		 Destructor
		*/
		~ProcessorMapper();

		/**
		 Call the mapping function -- manual or automatic

		 @param hGraph: the SRDAG graph
		 @param theJobSet: the mapping result is written to the Job reposiroty carried by this pointer
		 @param nbSlaves: current number of slaves
		 @param taskCount: number of vertices in the CSDAG
		*/
		void doMapping(SRDAGGraph* hGraph, JobSet *theJobSet, int nbSlaves, int taskCount);

		void do7Mapping(JobSet *theJobSet);
};

#endif
