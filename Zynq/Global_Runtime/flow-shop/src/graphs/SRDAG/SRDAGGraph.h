/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#ifndef SRDAG_GRAPH
#define SRDAG_GRAPH

#include "SRDAGVertex.h"
#include "SRDAGEdge.h"

#include "../../SchedulerDimensions.h"
#include "../../tools/SchedulingError.h"

/**
 * A SRDAG graph. It contains SRDAG vertices and edges. It has a bigger table for vertices and edges than DAG.
 * Each edge production and consumption must be equal. There is no repetition vector for the vertices.
 * 
 * @author mpelcat
 */
class SRDAGGraph {

	private :
		/**
		 number of SRDAG vertices
		*/
		int nbVertices;

		/**
		 table of SRDAG vertices
		*/
		SRDAGVertex vertices[MAX_SRDAG_VERTICES];

		/**
		 number of SRDAG edges
		*/
		int nbEdges;

		/**
		 table of SRDAG edges
		*/
		SRDAGEdge edges[MAX_SRDAG_EDGES];

	public : 
		/**
		 Constructor
		*/
		SRDAGGraph();

		/**
		 Destructor
		*/
		~SRDAGGraph();

		/**
		 Adding a vertex to the graph. Vertices and edges must be added in topological order.
		 There is no initial token on edges
		 
		 @return the new vertex
		*/
		SRDAGVertex* addVertex();

		/**
		 Adding an edge to the graph. Vertices and edges must be added in topological order.
		 There is no initial token on edges
		 
		 @param source: The source vertex of the edge
		 @param tokenRate: number of tokens (chars) produced by the source and consumed by the sink
		 @param sink: The sink vertex of the edge
		 @return the created edge
		*/
		SRDAGEdge* addEdge(SRDAGVertex* source, int tokenRate, SRDAGVertex* sink);

		/**
		 Removes the last added edge
		*/
		void removeLastEdge();

		/**
		 Removes all edges and vertices
		*/
		void flush();

		/**
		 Gets the actor at the given index
		 
		 @param index: index of the actor in the actor list
		 @return actor
		*/
		SRDAGVertex* getVertex(int index);

		/**
		 Gets the srDag vertices that share the given DAG reference
		 
		 @param ref: the DAG reference
		 @param output: the output SRDAG vertices
		 @return the number of found references
		*/
		int getVerticesFromCSDAGReference(CSDAGVertex* ref, SRDAGVertex** output);

		/**
		 Gets the index of the given actor
		 
		 @param vertex: actor vertex
		 @return index of the actor in the actor list
		*/
		int getVertexIndex(SRDAGVertex* vertex);

		/**
		 Gets the index of the given edge
		 
		 @param edge: edge
		 @return index of the edge in the edge list
		*/
		int getEdgeIndex(SRDAGEdge* edge);

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
		SRDAGEdge* getEdge(int index);

		/**
		 Gets the edge number
		 
		 @return number of edges
		*/
		int getNbEdges();

		/**
		 Reset the "visited" status of all vertices
		*/
		void resetVisited();

		/**
		 Gets the input edges of a given vertex. Careful!! Slow!
		 
		 @param vertex: input vertex
		 @param output: table to store the edges
		 @return the number of input edges
		*/
		int getInputEdges(SRDAGVertex* vertex, SRDAGEdge** output);

		/**
		 Gets the output edges of a given vertex. Careful!! Slow!
		 
		 @param vertex: input vertex
		 @param output: table to store the edges
		 @return the number of output edges
		*/
		int getOutputEdges(SRDAGVertex* vertex, SRDAGEdge** output);

		/**
		 Gets pointers on the edges of the graph in the order of their source or sink.
		 Accelerates getting the input or output edges
		 
		 @param sourceOrSink: 1 for sorting in source order, 0 for sink order
		*/
		void sortEdges(int startIndex);

		int getMaxTime();
		int getCriticalPath();

};

/**
 Gets the actor at the given index
 
 @param index: index of the actor in the actor list
 @return actor
*/
inline SRDAGVertex* SRDAGGraph::getVertex(int index){
	return &vertices[index];
}

/**
 Gets the index of the given actor
 
 @param vertex: actor vertex
 @return index of the actor in the actor list
*/
inline int SRDAGGraph::getVertexIndex(SRDAGVertex* vertex){
	int index;
	for(index=0;index<nbVertices;index++){
		if(vertex == &vertices[index]){
			return index;
		}
	}
	return -1;
}

/**
 Gets the index of the given edge
 
 @param edge: edge
 @return index of the edge in the edge list
*/
inline int SRDAGGraph::getEdgeIndex(SRDAGEdge* edge){
	int index;
	for(index=0;index<nbEdges;index++){
		if(edge == &edges[index]){
			return index;
		}
	}
	return -1;
}

/**
 Gets the actor number
 
 @return number of vertices
*/
inline int SRDAGGraph::getNbVertices(){
	return nbVertices;
}

/**
 Gets the edge at the given index
 
 @param index: index of the edge in the edge list
 @return edge
*/
inline SRDAGEdge* SRDAGGraph::getEdge(int index){
	return &edges[index];
}

/**
 Gets the edge number
 
 @return number of edges
*/
inline int SRDAGGraph::getNbEdges(){
	return nbEdges;
}

/**
 Reset the "visited" status of all vertices
*/
inline void SRDAGGraph::resetVisited(){
	for(int i = 0; i < nbVertices; i++)
		vertices[i].setVisited(0);
}

/**
 Gets the srDag vertices that share the given DAG reference
 
 @param ref: the DAG reference
 @param output: the output SRDAG vertices
 @return the number of found references
*/
inline int SRDAGGraph::getVerticesFromCSDAGReference(CSDAGVertex* ref, SRDAGVertex** output){
	int size = 0;
	for(int i=0; i<nbVertices; i++){
		SRDAGVertex* vertex = &vertices[i];
		if(vertex->getCsDagReference() == ref){
			output[size] = vertex;
			size++;
		}
	}
	return size;
}

/**
 Gets the input edges of a given vertex. Careful!! Slow!
 
 @param vertex: input vertex
 @param output: table to store the edges
 @return the number of input edges
*/
inline int SRDAGGraph::getInputEdges(SRDAGVertex* vertex, SRDAGEdge** output){
	int nbEdges = 0;
	for(int i=0; i<this->nbEdges; i++){
		SRDAGEdge* edge = &edges[i];
		SRDAGVertex* sink = edge->getSink();
		if(sink == vertex){
			output[nbEdges] = edge;
			nbEdges++;
		}
	}
	return nbEdges;
}

/**
 Gets the output edges of a given vertex. Careful!! Slow!
 
 @param vertex: input vertex
 @param output: table to store the edges
 @return the number of output edges
*/
inline int SRDAGGraph::getOutputEdges(SRDAGVertex* vertex, SRDAGEdge** output){
	int nbEdges = 0;
	for(int i=0; i<this->nbEdges; i++){
		SRDAGEdge* edge = &edges[i];
		SRDAGVertex* source = edge->getSource();
		if(source == vertex){
			output[nbEdges] = edge;
			nbEdges++;
		}
	}
	return nbEdges;
}

/**
 Adding a vertex to the graph
 
 @param vertexName: the name of the new vertex

 @return the new vertex
*/
inline SRDAGVertex* SRDAGGraph::addVertex(){
	SRDAGVertex* vertex;
#ifdef _DEBUG
	if(nbVertices >= MAX_SRDAG_VERTICES){
		// Adding a vertex while the graph is already full
		exitWithCode(1000);
	}
#endif
	vertex = &vertices[nbVertices];
	vertex->setBase(this);
	nbVertices++;
	return vertex;
}

#endif
