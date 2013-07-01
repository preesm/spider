/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#ifndef CSDAG_GRAPH
#define CSDAG_GRAPH

#include "CSDAGVertex.h"
#include "CSDAGEdge.h"

#include "../../SchedulerDimensions.h"

/**
 * A CSDAG graph
 * 
 * @author mpelcat
 */
class CSDAGGraph{

	protected :
		/**
		 number of CSDAG vertices
		*/
		int nbVertices;

		/**
		 table of CSDAG vertices
		*/
		CSDAGVertex vertices[MAX_CSDAG_VERTICES];

		/**
		 number of CSDAG edges
		*/
		int nbEdges;

		/**
		 table of CSDAG edges
		*/
		CSDAGEdge edges[MAX_CSDAG_EDGES];

	public : 

		/**
		 table of all the edges production and consumption patterns and vertices parameter patterns in the graph. 
		 Allocated precisely because the pattern size can vary much (up to hundreds of abstract_syntax_elt).
		*/
		abstract_syntax_elt patternsTable[MAX_CSDAG_PATTERN_TABLE_SIZE];
		int patternsTableSize; // Table size in abstract_syntax_elt

		/**
		 Constructor
		*/
		CSDAGGraph();

		/**
		 Destructor
		*/
		~CSDAGGraph();

		/**
		 Adding a vertex to the graph. Vertices and edges must be added in topological order.
		 There is no initial token on edges
		 
		 @param vertexName: the name of the new vertex
		 @return the new vertex
		*/
		CSDAGVertex* addVertex(const char* vertexName);

		/**
		 Adding an edge to the graph. Vertices and edges must be added in topological order.
		 There is no initial token on edges
		 
		 @param source: The source vertex of the edge
		 @param production: number of tokens (chars) produced by the source
		 @param sink: The sink vertex of the edge
		 @param consumption: number of tokens (chars) consumed by the sink
		 @return the created edge
		*/
		CSDAGEdge* addEdge(CSDAGVertex* source, const char* production, CSDAGVertex* sink, const char* consumption);

		/**
		 Gets the actor at the given index
		 
		 @param index: index of the actor in the actor list
		 @return actor
		*/
		CSDAGVertex* getVertex(int index);

		/**
		 Gets the index of the given actor
		 
		 @param vertex: actor vertex
		 @return index of the actor in the actor list
		*/
		int getVertexIndex(CSDAGVertex* vertex);

		/**
		 Gets the actor number
		 
		 @return number of vertices
		*/
		int getNbVertices();

		/**
		 Gets the edge at the given index
		 
		 @param index: index of the edge in the edge list
		 @return edge
		*/
		CSDAGEdge* getEdge(int index);

		/**
		 Gets the edge number
		 
		 @return number of edges
		*/
		int getNbEdges();

		/**
		 Gets the input edges of a given vertex
		 
		 @param vertex: input vertex
		 @param output: table to store the edges
		 @return the number of input edges
		*/
		int getInputEdges(CSDAGVertex* vertex, CSDAGEdge** output);

		/**
		 Resolves the timings of each vertex in the CSDAG graph

		 @param archi the current architecture
		*/
		void resolveTimings(Architecture* archi);

};


/**
 Gets the actor at the given index
 
 @param index: index of the actor in the actor list
 @return actor
*/
inline CSDAGVertex* CSDAGGraph::getVertex(int index){
	return &vertices[index];
}

/**
 Gets the index of the given actor
 
 @param vertex: actor vertex
 @return index of the actor in the actor list
*/
inline int CSDAGGraph::getVertexIndex(CSDAGVertex* vertex){
	int index;
	for(index=0;index<nbVertices;index++){
		if(vertex == &vertices[index]){
			return index;
		}
	}
	return -1;
}

/**
 Gets the actor number
 
 @return number of vertices
*/
inline int CSDAGGraph::getNbVertices(){
	return nbVertices;
}

/**
 Gets the edge at the given index
 
 @param index: index of the edge in the edge list
 @return edge
*/
inline CSDAGEdge* CSDAGGraph::getEdge(int index){
	return &edges[index];
}

/**
 Gets the edge number
 
 @return number of edges
*/
inline int CSDAGGraph::getNbEdges(){
	return nbEdges;
}

/**
 Gets the input edges of a given vertex
 
 @param vertex: input vertex
 @param output: table to store the edges
 @return the number of input edges
*/
inline int CSDAGGraph::getInputEdges(CSDAGVertex* vertex, CSDAGEdge** output){
	int nbEdges = 0;
	for(int i=0; i<this->nbEdges; i++){
		CSDAGEdge* edge = &edges[i];
		CSDAGVertex* sink = edge->getSink();
		if(sink == vertex){
			output[nbEdges] = edge;
			nbEdges++;
		}
	}
	return nbEdges;
}

#endif
