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
	nb_edges = 0;
	nb_parameters = 0;

	nb_vertices = 0;
	nb_pisdf_vertices = 0;
	nb_config_vertices = 0;
	nb_join_vertices = 0;
	nb_input_vertices = 0;
	nb_broad_vertices = 0;
	nb_output_vertices = 0;
	nb_select_vertices = 0;
	nb_switch_vertices = 0;
	nb_roundB_vertices = 0;

	rootVertex = NULL;

	nbExecVertices = 0;
	nbDiscardVertices = 0;
	executable = false;
}


PiSDFAbstractVertex* PiSDFGraph::addVertex(const char *vertexName, VERTEX_TYPE type)
{
	PiSDFAbstractVertex* vertex;

	switch (type) {
		case pisdf_vertex:
			if(nb_pisdf_vertices < MAX_NB_PiSDF_VERTICES){
				vertex = (PiSDFAbstractVertex*)&pisdf_vertices[nb_pisdf_vertices];
				nb_pisdf_vertices++;
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		case config_vertex:
			if(nb_config_vertices < MAX_NB_PiSDF_CONFIG_VERTICES){
				vertex = (PiSDFAbstractVertex*)&config_vertices[nb_config_vertices];
				nb_config_vertices++;
//				glbNbConfigVertices++;
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		case join_vertex:
			if(nb_join_vertices < MAX_NB_PiSDF_JOIN_VERTICES){
				vertex = (PiSDFAbstractVertex*)&join_vertices[nb_join_vertices];
				nb_join_vertices++;
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		case input_vertex:
			if(nb_input_vertices < MAX_NB_PiSDF_INPUT_VERTICES){
				vertex = (PiSDFAbstractVertex*)&input_vertices[nb_input_vertices];
				nb_input_vertices++;
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		case broad_vertex:
			if(nb_broad_vertices < MAX_NB_PiSDF_BROAD_VERTICES){
				vertex = (PiSDFAbstractVertex*)&broad_vertices[nb_broad_vertices];
				nb_broad_vertices++;
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		case output_vertex:
			if(nb_output_vertices < MAX_NB_PiSDF_OUTPUT_VERTICES){
				vertex = (PiSDFAbstractVertex*)&output_vertices[nb_output_vertices];
				nb_output_vertices++;
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		case switch_vertex:
			if(nb_switch_vertices < MAX_NB_PiSDF_SWITCH_VERTICES){
				vertex = (PiSDFAbstractVertex*)&switch_vertices[nb_switch_vertices];
				nb_switch_vertices++;
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		case select_vertex:
			if(nb_select_vertices < MAX_NB_PiSDF_OUTPUT_VERTICES){
				vertex = (PiSDFAbstractVertex*)&select_vertices[nb_select_vertices];
				nb_select_vertices++;
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		case roundBuff_vertex:
			if(nb_select_vertices < MAX_NB_PiSDF_OUTPUT_VERTICES){
				vertex = (PiSDFAbstractVertex*)&select_vertices[nb_roundB_vertices];
				nb_roundB_vertices++;
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		default:
			break;
	}

	vertex->setId(nb_vertices);
	vertex->setName(vertexName);
	vertex->setType(type);
	//		vertex->setPiSDF(this);
	vertices[nb_vertices++] = vertex;
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
	if(nb_edges < MAX_NB_INPUT_EDGES + MAX_NB_OUTPUT_EDGES){
		edge = &edges[nb_edges];
		edge->setId(nb_edges);
//		edge->setPiSDF(this);
		edge->setSource(source);
		edge->setProduction(production);
		edge->setSink(sink);
		edge->setConsumption(consumption);
		edge->setDelay(delay);
		nb_edges++;

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
	if(nb_edges < MAX_NB_INPUT_EDGES + MAX_NB_OUTPUT_EDGES){
		edge = &edges[nb_edges];
		edge->setId(nb_edges);
//		edge->setPiSDF(this);
		edge->setSource(source);
		edge->setProduction(production);
		edge->setSink(sink);
		edge->setConsumption(consumption);
		edge->setDelay(delay);
		nb_edges++;

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
	if(nb_parameters < MAX_NB_PiSDF_PARAMS - 1)
	{
		param = &parameters[nb_parameters++];
		param->setName(name);
		param->setVariable(globalParser.addVariable(name, 1));
		return param;
	}
	else{
		exitWithCode(1057);
	}
	return param;
}



void PiSDFGraph::evaluateExpressions()
{
	int value;
	UINT32 nbEdges = this->getNb_edges();
	for (UINT32 i = 0; i < nbEdges; i++){
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
}

/*
 * Says if Vx is an interface/round-buffer preceding a configure vx.
 */
bool PiSDFGraph::isConfigVxPred(PiSDFAbstractVertex* Vx){
	if((Vx->getType() == input_vertex) || (Vx->getType() == roundBuff_vertex))
		return (Vx->getOutputEdge(0)->getSink()->getType() == config_vertex);
	else
		return false;
}

void PiSDFGraph::updateDAGStates(SRDAGGraph* dag){
	// Updating all input, round-buffer and hierarchical vxs.
	for (UINT32 i = 0; i < dag->getNbVertices(); i++) {
		dag->getVertex(i)->updateState();
	}
}

