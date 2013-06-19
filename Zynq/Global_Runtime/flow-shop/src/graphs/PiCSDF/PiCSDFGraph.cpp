/*
 * PiCSDFGraph.cpp
 *
 *  Created on: 12 juin 2013
 *      Author: yoliva
 */

#include "PiCSDFGraph.h"


/**
 Adding an edge to the graph. Vertices and edges must be added in topological order.

 @param source: The source vertex of the edge
 @param production: number of tokens (chars) produced by the source
 @param sink: The sink vertex of the edge
 @param consumption: number of tokens (chars) consumed by the sink
 @param initial_tokens: number of initial tokens.
 @return the created edge
*/
PiCSDFEdge* PiCSDFGraph::addEdge(CSDAGVertex* source, const char* production, CSDAGVertex* sink, const char* consumption, const char* initial_tokens){
	PiCSDFEdge* edge = NULL;
	if(nbEdges < MAX_CSDAG_EDGES){
		edge = &edges[nbEdges];
		edge->setBase(this);
		edge->setSource(source);
		edge->setProduction(production);
		edge->setSink(sink);
		edge->setConsumption(consumption);
		edge->setInitialTokens(initial_tokens);
		nbEdges++;
	}
	else{
		// Adding an edge while the graph is already full
		exitWithCode(1001);
	}
	return edge;
}

