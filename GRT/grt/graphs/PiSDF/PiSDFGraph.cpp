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

#include <tools/SchedulingError.h>
#include "PiSDFGraph.h"
#include <transformations/PiSDFTransformer/PiSDFTransformer.h>
#include "debuggingOptions.h"

PiSDFGraph::PiSDFGraph() {
	parentVertex = NULL;

	nbExecVertices = 0;
	nbDiscardVertices = 0;
	executable = false;

	edges.reset();
	parameters.reset();
	vertices.reset();

	pisdf_vertices.reset();
	config_vertices.reset();
	input_vertices.reset();
	output_vertices.reset();
}

void PiSDFGraph::reset() {
	parentVertex = NULL;

	nbExecVertices = 0;
	nbDiscardVertices = 0;
	executable = false;

	edges.reset();
	parameters.reset();
	vertices.reset();

	pisdf_vertices.reset();
	config_vertices.reset();
	input_vertices.reset();
	output_vertices.reset();
}

void PiSDFGraph::resetRefs(){
	for(int i=0; i<vertices.getNb(); i++){
		vertices[i]->resetRefs();
		if(vertices[i]->getType() == normal_vertex &&
				((PiSDFVertex*)vertices[i])->hasSubGraph()){
			((PiSDFVertex*)vertices[i])->getSubGraph()->resetRefs();
		}
	}

}

PiSDFAbstractVertex* PiSDFGraph::addVertex(const char *vertexName, VERTEX_TYPE type)
{
	PiSDFAbstractVertex* vertex;

	switch (type) {
		case normal_vertex:
			if(pisdf_vertices.getNb() < MAX_NB_PiSDF_VERTICES){
				vertex = pisdf_vertices.add();
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		case config_vertex:
			if(config_vertices.getNb() < MAX_NB_PiSDF_CONFIG_VERTICES){
				vertex = config_vertices.add();
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		case input_vertex:
			if(input_vertices.getNb() < MAX_NB_PiSDF_INPUT_VERTICES){
				vertex = input_vertices.add();
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		case output_vertex:
			if(output_vertices.getNb() < MAX_NB_PiSDF_OUTPUT_VERTICES){
				vertex = output_vertices.add();
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		default:
			break;
	}

	vertex->reset();
	vertex->setId(vertices.getNb());
	vertex->setName(vertexName);
	vertex->setType(type);
	vertex->setGraph(this);
	vertices.add(vertex);
	return vertex;
}


/**
 Adding an edge to the graph. Vertices and edges must be added in topological order.

 @param source: The source vertex of the edge
 @param production: number of tokens (chars) produced by the source
 @param sink: The sink vertex of the edge
 @param consumption: number of tokens (chars) consumed by the sink
 @param initial_tokens: number of initial tokens.
 @return the created edge
*/
PiSDFEdge *PiSDFGraph::addEdge(PiSDFAbstractVertex* source, UINT32 sourcePortId, const char* production, PiSDFAbstractVertex* sink, UINT32 sinkPortId, const char* consumption, const char* delay){
	PiSDFEdge* edge = NULL;
	if(edges.getNb() < MAX_NB_PiSDF_EDGES){
		edge = edges.add();
		edge->setId(edges.getId(edge));
//		edge->setPiSDF(this);
		edge->setSource(source);
		edge->setProduction(production);
		edge->setSink(sink);
		edge->setConsumption(consumption);
		edge->setDelay(delay);

		source->setOutputEdge(edge, sourcePortId);
		sink->setInputEdge(edge, sinkPortId);
	}
	else{
		// Adding an edge while the graph is already full
		exitWithCode(1001);
	}
	return edge;
}

PiSDFEdge *PiSDFGraph::addEdge(PiSDFAbstractVertex* source, UINT32 sourcePortId, abstract_syntax_elt* production, PiSDFAbstractVertex* sink, UINT32 sinkPortId, abstract_syntax_elt* consumption, abstract_syntax_elt* delay){
	PiSDFEdge* edge = NULL;
	if(edges.getNb() < MAX_NB_PiSDF_EDGES){
		edge = edges.add();
		edge->setId(edges.getId(edge));
//		edge->setPiSDF(this);
		edge->setSource(source);
		edge->setProduction(production);
		edge->setSink(sink);
		edge->setConsumption(consumption);
		edge->setDelay(delay);

		source->setOutputEdge(edge, sourcePortId);
		sink->setInputEdge(edge, sinkPortId);
	}
	else{
		// Adding an edge while the graph is already full
		exitWithCode(1001);
	}
	return edge;
}


PiSDFParameter* PiSDFGraph::addParameter(const char *name)
{
	PiSDFParameter* param = NULL;
	if(parameters.getNb() < MAX_NB_PiSDF_PARAMS)
	{
		param = parameters.add();
		param->reset();
		param->setName(name);
		param->setGraph(this);
		param->setVariable(globalParser.addVariable(name, 1));
		return param;
	}
	else{
		exitWithCode(1057);
	}
	return param;
}



void PiSDFGraph::evaluateExpressions(/*Scenario* scenario*/)
{
	int value;
	for (UINT32 i = 0; i < edges.getNb(); i++){
		PiSDFEdge* edge = this->getEdge(i);

		if(!edge->getEvaluated()){
			// Updating production's integer value.
			globalParser.interpret(edge->getProduction(), &value);
			edge->setProductionInt(value);

			// Updating consumption's integer value.
			globalParser.interpret(edge->getConsumption(), &value);
			edge->setConsumtionInt(value);

			// Updating delay's integer value.
			globalParser.interpret(edge->getDelay(), &value);
			edge->setDelayInt(value);
		}
	}

	for(UINT32 i = 0; i < pisdf_vertices.getNb(); i++){
		PiSDFVertex* vertex = this->getPiSDFVertex(i);
		if(!vertex->hasSubGraph()){
			for(int j=0; j<MAX_SLAVE_TYPES; j++){
				if(vertex->getConstraints(j)){
					globalParser.interpret(vertex->getTiming(j), &value);
					vertex->setResolvedTiming(j, value);
				}
			}
		}
	}
}

