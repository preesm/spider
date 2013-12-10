/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet				*
 * 																			*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr					*
 * 																			*
 * This software is a computer program whose purpose is to execute			*
 * parallel applications.													*
 * 																			*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 												*
 * 																			*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 																*
 * 																			*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 									*
 * 																			*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.			*
 ****************************************************************************/

#include <tools/SchedulingError.h>
#include "PiSDFGraph.h"

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

	rootVertex = NULL;

	nbExecVertices = 0;
	nbDiscardVertices = 0;
}


BaseVertex* PiSDFGraph::addVertex(const char *vertexName, VERTEXT_TYPE type)
{
	BaseVertex* vertex;

	switch (type) {
		case pisdf_vertex:
			if(nb_pisdf_vertices < MAX_NB_PiSDF_VERTICES){
				vertex = (BaseVertex*)&pisdf_vertices[nb_pisdf_vertices];
				nb_pisdf_vertices++;
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		case config_vertex:
			if(nb_config_vertices < MAX_NB_PiSDF_CONFIG_VERTICES){
				vertex = (BaseVertex*)&config_vertices[nb_config_vertices];
				nb_config_vertices++;
				glbNbConfigVertices++;
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		case join_vertex:
			if(nb_join_vertices < MAX_NB_PiSDF_JOIN_VERTICES){
				vertex = (BaseVertex*)&join_vertices[nb_join_vertices];
				nb_join_vertices++;
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		case input_vertex:
			if(nb_input_vertices < MAX_NB_PiSDF_INPUT_VERTICES){
				vertex = (BaseVertex*)&input_vertices[nb_input_vertices];
				nb_input_vertices++;
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		case broad_vertex:
			if(nb_broad_vertices < MAX_NB_PiSDF_BROAD_VERTICES){
				vertex = (BaseVertex*)&broad_vertices[nb_broad_vertices];
				nb_broad_vertices++;
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		case output_vertex:
			if(nb_output_vertices < MAX_NB_PiSDF_OUTPUT_VERTICES){
				vertex = (BaseVertex*)&output_vertices[nb_output_vertices];
				nb_output_vertices++;
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		case switch_vertex:
			if(nb_switch_vertices < MAX_NB_PiSDF_SWITCH_VERTICES){
				vertex = (BaseVertex*)&switch_vertices[nb_switch_vertices];
				nb_switch_vertices++;
			}
			else{
				// Adding a vertex while the graph is already full
				exitWithCode(1000);
			}
			break;
		case select_vertex:
			if(nb_select_vertices < MAX_NB_PiSDF_OUTPUT_VERTICES){
				vertex = (BaseVertex*)&select_vertices[nb_select_vertices];
				nb_select_vertices++;
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
	//		vertex->setBase(this);
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
PiSDFEdge *PiSDFGraph::addEdge(BaseVertex* source, const char* production, BaseVertex* sink, const char* consumption, const char* delay){
	PiSDFEdge* edge = NULL;
	if(nb_edges < MAX_NB_INPUT_EDGES + MAX_NB_OUTPUT_EDGES){
		edge = &edges[nb_edges];
		edge->setId(nb_edges);
//		edge->setBase(this);
		edge->setSource(source);
		edge->setProduction(production);
		edge->setSink(sink);
		edge->setConsumption(consumption);
		edge->setDelay(delay);
		nb_edges++;

		source->addOutputEdge(edge);
		sink->addInputEdge(edge);
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
		// Adding an edge while the graph is already full
		exitWithCode(1001);
	}
	return param;
}


//void PiSDFGraph::setExecutableVertices(BaseVertex* vertex)
//{
//	if(vertex->getExecutable())
//	{
//		// Calling this function for the child graph, if necessary.
//		if(vertex->getType() == pisdf_vertex){
//			PiSDFGraph* subGraph = ((PiSDFVertex*)vertex)->getSubGraph();
//			if(subGraph != (PiSDFGraph*)0)
//			{
//				subGraph->setExecutableVertices(subGraph->getRootVertex());
//				if(subGraph->getExecComplete()) // The sub graph can be executed completely.
//				{
//					// Checking successors of the current parent vertex.
//					for (UINT32 i = 0; i < vertex->getNbOutputEdges(); i++) {
//						PiSDFEdge* edge = vertex->getOutputEdge(i);
//						BaseVertex* sinkVertex = edge->getSink();
//						if(! sinkVertex->getVisited())
//							setExecutableVertices(edge->getSink());
//					}
//				}
//				return; // So that the parent vertex is not included in the list.
//			}
//		}
//		// Adding vertex to the list of executable vertices.
//		ExecutableVertices[nbExecutableVertices++] = vertex;
//		vertex->setVisited(true);
//
//		// Checking successors.
//		for (UINT32 i = 0; i < vertex->getNbOutputEdges(); i++) {
//			PiSDFEdge* edge = vertex->getOutputEdge(i);
//			BaseVertex* sinkVertex = edge->getSink();
//
//			// Discarding edges with zero production.
//			if(edge->getProductionInt() > 0)
//				// Adding the vertex if not visited yet.
//				if (! sinkVertex->getVisited())
//					setExecutableVertices(edge->getSink());
//		}
//
//		if(vertex->getType() == output_vertex)
//			this->setExecComplete(true); // It is a sub graph and can be executed completely.
//	}
//}

//void PiSDFGraph::setExecutableVertices()
//{
//	for (UINT32 i = 0; i < this->getNb_config_vertices(); i++) {
//		PiSDFConfigVertex* vertex = &this->config_vertices[i];
//		if(vertex->getExecutable())
//			// Adding vertex to the list of executable vertices.
//			ExecutableVertices[nbExecutableVertices++] = vertex;
//	}
//}

void PiSDFGraph::copyExecutableVertices(BaseVertex* startVertex, SDFGraph *outSDF)
{
	startVertex->checkForExecution(outSDF);
	if(startVertex->getExecutable() == possible)
	{
		// Checking if the startVertex contains a sub graph.
		if(startVertex->getType() == pisdf_vertex)
		{
			PiSDFGraph* subGraph = ((PiSDFVertex*)startVertex)->getSubGraph();
			if(subGraph != (PiSDFGraph*)0)
			{
				/***  Only hierarchical vertices ***/

				// Checking the sub graph.
				subGraph->getSDFGraph(outSDF);
//				subGraph->copyExecutableVertices(subGraph->getRootVertex(), outSDF);

				if(subGraph->getNbExecVertices() == (subGraph->getNb_vertices() - subGraph->getNbDiscardVertices()))
//				if(subGraph->getExecComplete()) // The sub graph can be executed completely,
				{								// so the parent's successors can be examined.
//					startVertex->setExecutable(possible);
					// Checking successors of the current parent vertex.
					for (UINT32 i = 0; i < startVertex->getNbOutputEdges(); i++)
					{
						PiSDFEdge* edge = startVertex->getOutputEdge(i);
						BaseVertex* sinkVertex = edge->getSink();
						// Discarding edges with zero production and vertices already visited.
						if((edge->getProductionInt() > 0) && (sinkVertex->getExecutable() == undefined))
								copyExecutableVertices(edge->getSink(), outSDF);
					}
				}
				else
					// Making that the hierarchical vertex is examined during the next iteration.
					startVertex->setExecutable(undefined);


//				startVertex->setVisited(true); // So that subsequent incoming edges ignore this vertex.
				return; // So that the hierarchical vertex is not included.
			}
		}

		/***  Only non hierarchical vertices ***/


//		if(startVertex->getVisited()) return; // If the first vertex was already visited, the graph was already visited.

		// Adding vertex to the SDF graph.
		outSDF->addVertex(startVertex);
		if(startVertex->getType()== config_vertex)
			outSDF->addConfigVertex(startVertex);
//		startVertex->setVisited(true);

		// Checking successors.
		for (UINT32 i = 0; i < startVertex->getNbOutputEdges(); i++)
		{
			PiSDFEdge* edge = startVertex->getOutputEdge(i);
			BaseVertex* sinkVertex = edge->getSink();
			// Discarding edges with zero production and vertices already marked as executable.
			if(edge->getProductionInt() > 0){
				if(sinkVertex->getExecutable() == undefined)
					copyExecutableVertices(sinkVertex, outSDF);
			}
			else{
				if(sinkVertex->getExecutable() != impossible){
					sinkVertex->setExecutable(impossible);
					nbDiscardVertices++;
				}
			}

		}
		nbExecVertices++;
	}
}


void PiSDFGraph::linkExecutableVertices(SDFGraph *outSDF){
	for (UINT32 i = 0; i < outSDF->getNbVertices(); i++)
	{
		BaseVertex* vertex = outSDF->getVertex(i);

		if(vertex->getType() == input_vertex)
		{
			// Getting incoming edge from parent.
			UINT16 parentEdgeIndex = ((PiSDFIfVertex*)vertex)->getParentEdgeIndex();
			PiSDFEdge* originalEdge = ((PiSDFIfVertex*)vertex)->getParentVertex()->getInputEdge(parentEdgeIndex);
			if (originalEdge->getConsumptionInt() > 0)
				outSDF->addEdge(originalEdge->getSource(),
								originalEdge->getProductionInt(),
								vertex,
								originalEdge->getConsumptionInt());
		}
		else
		{
			for (UINT32 j = 0; j < vertex->getNbInputEdges(); j++)
			{
				PiSDFEdge* originalEdge = vertex->getInputEdge(j);
				if ((originalEdge->getConsumptionInt() > 0) && (outSDF->checkEdge(vertex->getInputEdge(j))))
					outSDF->addEdge(originalEdge->getSource(), originalEdge->getProductionInt(),
									vertex, originalEdge->getConsumptionInt());
			}

			if(vertex->getType() == output_vertex)
			{
				// Getting sinks from parent.
				for (UINT32 j = 0; j < ((PiSDFIfVertex*)vertex)->getParentVertex()->getNbOutputEdges(); j++)
				{
					PiSDFEdge* originalEdge = ((PiSDFIfVertex*)vertex)->getParentVertex()->getOutputEdge(j);
					if (originalEdge->getProductionInt() > 0)
						outSDF->addEdge(vertex, originalEdge->getProductionInt(),
										originalEdge->getSink(), originalEdge->getConsumptionInt());
				}
				// Ignoring the next vertex that must be the parent's successor.
				// Which is already treated as a sink for the output vertex.
				// TODO: ...i'm not sure of this..
				i++;
			}
		}
	}
}

void PiSDFGraph::getSDFGraph(SDFGraph *outSDF)
{
//	if(nb_input_vertices > 0){ // Hierarchy is being treated.
//		for (UINT32 i = 0; i < nb_input_vertices; i++) {
//			if(!input_vertices[i].getVisited())
//				copyExecutableVertices(&input_vertices[i], outSDF);
//		}
//	}
//	else{
//		// Looking for a starting vertex, i.e. one that has not been marked as executable yet.
//		BaseVertex* startVertex = NULL;
//		UINT32 i = 0;
//		do {
//			startVertex = vertices[i];
//			i++;
//		} while ((startVertex->getExecutable())&&(i < nb_vertices));
//
//		if(startVertex != NULL)
//			copyExecutableVertices(startVertex, outSDF);
//		else
//			exitWithCode(1061);
//	}

	for (UINT32 i = 0; i < nb_vertices; i++) {
		if(vertices[i]->getExecutable() == undefined)
			copyExecutableVertices(vertices[i], outSDF);
	}

}

void PiSDFGraph::clearAfterVisit(){
	// Clearing vertices (member "executable" = "undefined").
	for (UINT32 i = 0; i < nb_edges; i++) {
		BaseVertex* vertex;
		vertex = edges[i].getSource();
		vertex->setExecutable(undefined);
		if (vertex->getType() == pisdf_vertex){
			PiSDFGraph* subGraph = ((PiSDFVertex*)(vertex))->getSubGraph();
			if(subGraph != (PiSDFGraph*)0)
				subGraph->clearAfterVisit();
		}

		vertex = edges[i].getSink();
		vertex->setExecutable(undefined);
		if (vertex->getType() == pisdf_vertex){
			PiSDFGraph* subGraph = ((PiSDFVertex*)(vertex))->getSubGraph();
			if(subGraph != (PiSDFGraph*)0)
				subGraph->clearAfterVisit();
		}
	}

	// Clearing parameters. (member "solved" = false)
	for (UINT32 i = 0; i < nb_config_vertices; i++) {
		PiSDFConfigVertex* vertex = &config_vertices[i];
		for (UINT32 j = 0; j < vertex->getNbRelatedParams(); j++) {
			PiSDFParameter* param = vertex->getRelatedParam(j);
			param->setResolved(false);
		}
	}

	// Clearing other members..
	nbExecVertices = 0;
	nbDiscardVertices = 0;
}
