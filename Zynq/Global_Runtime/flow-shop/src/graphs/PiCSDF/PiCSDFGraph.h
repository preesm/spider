/*
 * PiCSDFGraph.h
 *
 *  Created on: 12 juin 2013
 *      Author: yoliva
 */

#ifndef PICSDFGRAPH_H_
#define PICSDFGRAPH_H_

#include "../CSDAG/CSDAGGraph.h"
#include "PiCSDFEdge.h"

class PiCSDFGraph : public CSDAGGraph{
	private:
		/**
		 table of PiCSDF edges
		*/
		PiCSDFEdge edges[MAX_CSDAG_EDGES];
	public:

		/**
		 Adding an edge to the graph. Vertices and edges must be added in topological order.

		 @param source: The source vertex of the edge
		 @param production: number of tokens (chars) produced by the source
		 @param sink: The sink vertex of the edge
		 @param consumption: number of tokens (chars) consumed by the sink
		 @param initial_tokens: number of initial tokens.
		 @return the created edge
		*/
		PiCSDFEdge* addEdge(CSDAGVertex* source, const char* production, CSDAGVertex* sink, const char* consumption, const char* initial_tokens);


		/**
		 Gets the edge at the given index

		 @param index: index of the edge in the edge table
		 @return edge
		*/
		PiCSDFEdge* getEdge(int index);


		/**
		 Gets the input edges of a given vertex

		 @param vertex: input vertex
		 @param output: table to store the edges
		 @return the number of input edges
		*/
		int getInputEdges(CSDAGVertex* vertex, PiCSDFEdge** output);


};



/**
 Gets the edge at the given index

 @param index: index of the edge in the edge list
 @return edge
*/
inline PiCSDFEdge* PiCSDFGraph::getEdge(int index){
	return &edges[index];
}


/**
 Gets the input edges of a given vertex

 @param vertex: input vertex
 @param output: table to store the edges
 @return the number of input edges
*/
inline int PiCSDFGraph::getInputEdges(CSDAGVertex* vertex, PiCSDFEdge** output){
	int nbEdges = 0;
	for(int i=0; i<this->nbEdges; i++){
		PiCSDFEdge* edge = &edges[i];
		CSDAGVertex* sink = edge->getSink();
		if(sink == vertex){
			output[nbEdges] = edge;
			nbEdges++;
		}
	}
	return nbEdges;
}

#endif /* PICSDFGRAPH_H_ */
