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
PiCSDFEdge* PiCSDFGraph::addEdge(CSDAGVertex* source, const char* production, CSDAGVertex* sink, const char* consumption, const char* delay){
	PiCSDFEdge* edge = NULL;
	if(nbEdges < MAX_CSDAG_EDGES){
		edge = &edges[nbEdges];
		edge->setBase(this);
		edge->setSource(source);
		edge->setProduction(production);
		edge->setSink(sink);
		edge->setConsumption(consumption);
		edge->setDelay(delay);
		nbEdges++;
	}
	else{
		// Adding an edge while the graph is already full
		exitWithCode(1001);
	}
	return edge;
}

/**
 Adding a configuration vertex to the graph.

 @param vertexName: the name of the new vertex
 @return the new vertex
*/
CSDAGVertex* PiCSDFGraph::addConfigVertex(const char* vertexName){
	CSDAGVertex* vertex = NULL;
	if(nbConfigVertices < MAX_PISDF_CONFIG_VERTICES){
		vertex = &configVertices[nbConfigVertices];
		vertex->setBase(this);
		vertex->setName(vertexName);
		nbConfigVertices++;
	}
	else{
		// Adding a vertex while the graph is already full
		exitWithCode(1000);
	}
	return vertex;
}

/**
 * Get number of configuration vertices.
 */
int PiCSDFGraph::getNbConfigVertices(){
	return nbConfigVertices;
}


/**
 Adding a configuration port to the graph

 @param vertex: pointer to the vertex connected to the port.
 	 	param:	pointer to the parameter connected to the port.
 	 	dir:	port direction. 0:input, 1:output.

 @return the new configuration port.
*/
PiCSDFConfigPort* PiCSDFGraph::addConfigPort(CSDAGVertex* vertex, PiCSDFParameter* param, int dir){
	PiCSDFConfigPort* configPort = NULL;
	if(dir == 0) // Input port.
	{
		if(nbConfigInPorts < MAX_PISDF_CONFIG_PORTS); //TODO handle the error. exitWithCode()
		configPort = &configInPorts[nbConfigInPorts];
		nbConfigInPorts++;
	}
	else // Output port.
	{
		if(nbConfigOutPorts < MAX_PISDF_CONFIG_PORTS); //TODO handle the error. exitWithCode()
		configPort = &configOutPorts[nbConfigOutPorts];
		nbConfigOutPorts++;
	}

	configPort->vertex = vertex;
	configPort->parameter = param;
	configPort->direction = dir;

	return configPort;
}


/*
 * Getting a configuration input port.
 */
PiCSDFConfigPort* PiCSDFGraph::getConfigInPort(int index){
	return &configInPorts[index];
}


/*
 * Getting a configuration output port.
 */
PiCSDFConfigPort* PiCSDFGraph::getConfigOutPort(int index){
	return &configOutPorts[index];
}


/**
 * Get number of configuration input ports.
 */
int PiCSDFGraph::getNbConfigInPorts(){
	return nbConfigInPorts;
}


/**
 * Get number of configuration input ports.
 */
int PiCSDFGraph::getNbConfigOutPorts(){
	return nbConfigOutPorts;
}


/**
 Adding a parameter to the graph

 @param expression: //expression defining the parameter's value.

 @return the new parameter.
*/
PiCSDFParameter* PiCSDFGraph::addParameter(const char* expression){
	PiCSDFParameter* parameter = NULL;
	if(nbParameters < MAX_PISDF_CONFIG_PORTS){
		parameter = &parameters[nbParameters];
		// Parsing the expression
		globalParser.parse(expression, parameter->expression);
		nbParameters++;
	}
	else{
		//TODO handle the error.
//		exitWithCode(1000);
	}
	return parameter;
}


/**
 * Getting the number of vertices.
 */
