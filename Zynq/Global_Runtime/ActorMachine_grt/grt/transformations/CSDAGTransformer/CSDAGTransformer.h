/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#ifndef CSDAG_TRANSFORMER
#define CSDAG_TRANSFORMER

#include "../../graphs/CSDAG/CSDAGGraph.h"
#include "../../graphs/CSDAG/CSDAGVertex.h"
#include "../../graphs/CSDAG/CSDAGEdge.h"
#include "../../graphs/SRDAG/SRDAGGraph.h"
#include "../../graphs/SRDAG/SRDAGVertex.h"
#include "../../graphs/SRDAG/SRDAGEdge.h"

/**
 * A CSDAG transformer transforms a CSDAG graph into an SRDAG one. 
 * It duplicates edges and breaks the repetition patterns to generate the SRDAG graph.
 * 
 * @author mpelcat
 */

typedef enum{
	Source=0,
	Sink=1,
} EdgeDir;

class CSDAGTransformer {

	protected :

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
		 int resolvedInputEdgesPatterns[2][MAX_CSDAG_INPUT_EDGES][MAX_CSDAG_VERTEX_REPETITION];




		// Data needed to duplicate an edge
		/**
		 Different SRDAG repetitions of an CSDAG vertex source to generate edges
		 */
		SRDAGVertex* sourceRepetitions[MAX_CSDAG_VERTEX_REPETITION];
		/**
		 Different SRDAG repetitions of an CSDAG vertex sink to generate edges
		 */
		SRDAGVertex* sinkRepetitions[MAX_CSDAG_VERTEX_REPETITION];

		CSDAGVertex* CSDAGExplodeVertex;
		int nbExplode;

	public : 
		/**
		 Constructor
		*/
		CSDAGTransformer();


		/**
		 Destructor
		*/
		~CSDAGTransformer();


		/**
		 Transforms a CSDAG graph in DAG

		 @param input: input CSDAG graph with pattern expressions on production and consumption
		 @param output: output SRDAG graph with variable expressions on production and consumption
		*/
		void transform(CSDAGGraph* input, SRDAGGraph* output, Architecture* archi);
		
		/**
		 Gets the input edges of the current vertex and their source and sink patterns. Treats the patterns to obtain the
		 same global production and consumption and to generate the number of repetitions.

		 @param inputGraph: inputGraph CSDAG graph with pattern expressions on production and consumption
		 @param sinkVertex: input CSDAG vertex which is the sink of the edges being treated
		 @return the sinkVertex repetition number
		*/
		int treatInputEdgesData(CSDAGGraph* inputGraph, CSDAGVertex* sinkVertex);

		/**
		 Transforms a CSDAG vertex and its input edges in DAG vertices and edges.

		 @param inputGraph: inputGraph CSDAG graph with pattern expressions on production and consumption
		 @param sinkVertex: input CSDAG vertex which is the sink of the edges being treated
		 @param brval: Basis repetition value of sinkVertex
		 @param outputGraph: output SRDAG graph with variable expressions on production and consumption
		 
		 @return the repetition of the current vertex
		*/
		void addVertices(CSDAGGraph* inputGraph, CSDAGVertex* sinkVertex, int brval, SRDAGGraph* outputGraph);

		/**
		 Creates the SRDAG edges corresponding to one CSDAG edge of the input graph. They are not added yet
		 because they must be added in the order of their sink.

		 @param inputGraph: inputGraph CSDAG graph with pattern expressions on production and consumption
		 @param sourceVertexIndex: index of the input edge source
		 @param edgeIndex: index of the input edge
		 @param sinkVertexIndex: index of the input edge sink
		 @param outputGraph: output SRDAG graph with variable expressions on production and consumption
		*/
		void createEdges(CSDAGGraph* inputGraph, int sourceVertexIndex, int edgeIndex, 
									   int sinkVertexIndex, SRDAGGraph* outputGraph);

		void createGraph(CSDAGGraph* inputGraph, CSDAGVertex* csdag_vertex, SRDAGGraph* outputGraph);
};

#endif
