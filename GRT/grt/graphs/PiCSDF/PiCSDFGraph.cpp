/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

#include <string.h>
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
	if(nbConfigVertices < MAX_NB_PiSDF_CONFIG_VERTICES){
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
		if(nbConfigInPorts < MAX_NB_PiSDF_INPUT_VERTICES); //TODO handle the error. exitWithCode()
		configPort = &configInPorts[nbConfigInPorts];
		nbConfigInPorts++;
	}
	else // Output port.
	{
		if(nbConfigOutPorts < MAX_NB_PiSDF_OUTPUT_VERTICES); //TODO handle the error. exitWithCode()
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
PiCSDFParameter* PiCSDFGraph::addParameter(const char* name, const char* expression){
	PiCSDFParameter* parameter = NULL;
	if(nbParameters < MAX_NB_PiSDF_PARAMS){
		parameter = &parameters[nbParameters];
		strcpy(parameter->name, name);
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
