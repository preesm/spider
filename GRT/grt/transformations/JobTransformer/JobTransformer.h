/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#ifndef JOB_TRANSFORMER
#define JOB_TRANSFORMER

#include "../../graphs/CSDAG/CSDAGGraph.h"
#include "../../graphs/CSDAG/CSDAGVertex.h"
#include "../../graphs/CSDAG/CSDAGEdge.h"
#include "../../graphs/SRDAG/SRDAGGraph.h"
#include "../../graphs/SRDAG/SRDAGVertex.h"
#include "../../graphs/SRDAG/SRDAGEdge.h"
#include "../../graphs/JobSet/JobSet.h"
#include "../../scheduling/FlowShopScheduler/processorMapper.h"

/**
 * A CSDAG transformer transforms a CSDAG graph into an SRDAG one. 
 * It duplicates edges and breaks the repetition patterns to generate the SRDAG graph.
 * 
 * @author mpelcat
 */
class JobTransformer {

	private :

		/**
		 Temporary storage for the vertex executed last on each slave
		*/
		SRDAGVertex* lastVertexOfSlave[MAX_SLAVES];

		/**
		 a temporary storage used in the iteration process
		*/
		Job currentJob;

		/**
		 iterate: a self-calling function that walks through the SRDAG
				  graph in the order in which flow-shop Jobs and
				  operations must be scheduled finally

		 @param hGraph: the graph being scheduled
		 @param vertex: the SRDAGvertex under inspection
		*/
		void iterate(SRDAGVertex* vertex/*, int nbSlaves*/);

		/**
		 finalizeJobBuilding: this is called from iterate() whenever
		 a complete Job has been detected. This function transfers
		 the detected Job to the repository.

		*/
		void finalizeJobBuilding();

		// Data needed to duplicate a vertex
		/**
		 Basis repetition vector of the CSDAG computed while iterating the graph
		*/
		int brv[MAX_CSDAG_VERTICES];
		/** 
		 Different input DAG edges of an CSDAG vertex and their number.
		*/
		int nb_input_edges;
		CSDAGEdge* input_edges[MAX_CSDAG_INPUT_EDGES];
		/** 
		 Resolved patterns of all the input edges of the current vertex and for source and sink (2) and their sizes.
		*/
		 unsigned short resolvedInputEdgesPatternsSizes[2][MAX_CSDAG_INPUT_EDGES];
		 int resolvedInputEdgesPatterns[2][MAX_CSDAG_INPUT_EDGES][MAX_CSDAG_PATTERN_SIZE];

		 JobSet *theJobSet;

		// Data needed to duplicate an edge
		/**
		 Different SRDAG repetitions of an CSDAG vertex source to generate edges
		 */
		SRDAGVertex* sourceRepetitions[MAX_CSDAG_VERTEX_REPETITION];
		/**
		 Different SRDAG repetitions of an CSDAG vertex sink to generate edges
		 */
		SRDAGVertex* sinkRepetitions[MAX_CSDAG_VERTEX_REPETITION];


	public : 
		/**
		 Constructor
		*/
		JobTransformer();


		/**
		 Destructor
		*/
		~JobTransformer();

		/**
		 Transforms a CSDAG graph in DAG

		 @param input: input CSDAG graph with pattern expressions on production and consumption
		 @param output: output SRDAG graph with variable expressions on production and consumption
		*/
		void transform(SRDAGGraph* input, JobSet* output, Architecture* archi);
		
};

#endif
