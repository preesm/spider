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
#include <tools/DotWriter.h>
#include <tools/ScheduleWriter.h>
#include "PiSDFGraph.h"
#include <transformations/PiSDFTransformer/PiSDFTransformer.h>


extern DotWriter dotWriter;

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
	nb_roundB_vertices = 0;

	rootVertex = NULL;

	nbExecVertices = 0;
	nbDiscardVertices = 0;
	executable = false;
}


BaseVertex* PiSDFGraph::addVertex(const char *vertexName, VERTEX_TYPE type)
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
		case roundBuff_vertex:
			if(nb_select_vertices < MAX_NB_PiSDF_OUTPUT_VERTICES){
				vertex = (BaseVertex*)&select_vertices[nb_roundB_vertices];
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

PiSDFEdge *PiSDFGraph::addEdge(BaseVertex* source, abstract_syntax_elt* production, BaseVertex* sink, abstract_syntax_elt* consumption, abstract_syntax_elt* delay){
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


//
//void PiSDFGraph::copyExecutableVertices(BaseVertex* startVertex, SDFGraph *outSDF)
//{
//	startVertex->checkForExecution(outSDF);
//	if(startVertex->getExecutable() == possible)
//	{
//		// Checking if the startVertex contains a sub graph.
//		if(startVertex->getType() == pisdf_vertex)
//		{
//			PiSDFGraph* subGraph = ((PiSDFVertex*)startVertex)->getSubGraph();
//			if(subGraph != (PiSDFGraph*)0)
//			{
//				/***  Only hierarchical vertices ***/
//
//				// Checking the sub graph.
//				subGraph->getSDFGraph(outSDF);
////				subGraph->copyExecutableVertices(subGraph->getRootVertex(), outSDF);
//
//				if(subGraph->getNbExecVertices() == (subGraph->getNb_vertices() - subGraph->getNbDiscardVertices()))
////				if(subGraph->getExecComplete()) // The sub graph can be executed completely,
//				{								// so the parent's successors can be examined.
////					startVertex->setExecutable(possible);
//					// Checking successors of the current parent vertex.
//					for (UINT32 i = 0; i < startVertex->getNbOutputEdges(); i++)
//					{
//						PiSDFEdge* edge = startVertex->getOutputEdge(i);
//						BaseVertex* sinkVertex = edge->getSink();
//						// Discarding edges with zero production and vertices already visited.
//						if((edge->getProductionInt() > 0) && (sinkVertex->getExecutable() == undefined))
//								copyExecutableVertices(edge->getSink(), outSDF);
//					}
//				}
//				else
//					// Making that the hierarchical vertex is examined during the next iteration.
//					startVertex->setExecutable(undefined);
//
//
////				startVertex->setVisited(true); // So that subsequent incoming edges ignore this vertex.
//				return; // So that the hierarchical vertex is not included.
//			}
//		}
//
//		/***  Only non hierarchical vertices ***/
//
//
////		if(startVertex->getVisited()) return; // If the first vertex was already visited, the graph was already visited.
//
//		// Adding vertex to the SDF graph.
//		outSDF->addVertex(startVertex);
//		if(startVertex->getType()== config_vertex)
//			outSDF->addConfigVertex(startVertex);
////		startVertex->setVisited(true);
//
//		// Checking successors.
//		for (UINT32 i = 0; i < startVertex->getNbOutputEdges(); i++)
//		{
//			PiSDFEdge* edge = startVertex->getOutputEdge(i);
//			BaseVertex* sinkVertex = edge->getSink();
//			// Discarding edges with zero production and vertices already marked as executable.
//			if(edge->getProductionInt() > 0){
//				if(sinkVertex->getExecutable() == undefined)
//					copyExecutableVertices(sinkVertex, outSDF);
//			}
//			else{
//				if(sinkVertex->getExecutable() != impossible){
//					sinkVertex->setExecutable(impossible);
//					nbDiscardVertices++;
//				}
//			}
//
//		}
//		nbExecVertices++;
//	}
//}



void PiSDFGraph::copyRequiredEdges(BaseVertex* startVertex)
{
	startVertex->checkForExecution();
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
				subGraph->findRequiredEdges();
//				subGraph->copyExecutableVertices(subGraph->getRootVertex(), outSDF);

				if(subGraph->getNbExecVertices() == (subGraph->getNb_vertices() - subGraph->getNbDiscardVertices())) // The sub graph can be executed completely.
				{
					// Checking successors of the current parent vertex.
					for (UINT32 i = 0; i < startVertex->getNbOutputEdges(); i++)
					{
						PiSDFEdge* edge = startVertex->getOutputEdge(i);
						BaseVertex* sinkVertex = edge->getSink();
						// Discarding edges with zero production and vertices already visited.
						if((edge->getProductionInt() > 0) && (sinkVertex->getExecutable() == undefined))
							copyRequiredEdges(edge->getSink());
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

		for (UINT32 i = 0; i < startVertex->getNbInputEdges(); i++)
		{
			/*
			 * Adding input edges to the table of required edges, except cycles.
			 * Cycles are marked while treating output edges.
			 */
			PiSDFEdge* edge = startVertex->getInputEdge(i);
			/*
			 *
			 */
			if(edge->getSource()->getType() == input_vertex)
				if(findVisitedIf((PiSDFIfVertex*)(edge->getSource())))
					break;

			if(edge->getSource() != startVertex)
				if(!edge->getRequired()){// Avoids that the edge be included more than once.
					requiredEdges[glbNbRequiredEdges++] = edge;
					edge->setRequired(true);
				}
		}

		for (UINT32 i = 0; i < startVertex->getNbOutputEdges(); i++)
		{
			// Adding output edges to the table of required edges.
			PiSDFEdge* edge = startVertex->getOutputEdge(i);
			if(!edge->getRequired()){// Avoids that the edge be included more than once.
				requiredEdges[glbNbRequiredEdges++] = edge;
				edge->setRequired(true);

				/*
				 * If the sink is a hierarchical vertex, the corresponding input vertex is stored
				 * so that its output edge don't be included in the table of required edges.
				 */
				if(edge->getSink()->getType() == pisdf_vertex){
					PiSDFGraph *sinkSubgraph;
					if(((PiSDFVertex*)(edge->getSink()))->hasSubGraph(&sinkSubgraph))
						visitedIfs[glbNbVisitedIfs++] = sinkSubgraph->getInputVertex(edge);
				}
			}
		}


		glbNbExecVertices++; // Counts executable vertices from the entire graph.
		nbExecVertices++;	// Counts executable vertices from the current level.
		if(startVertex->getType()== config_vertex)
			glbNbExecConfigVertices++; // Counts executable configure vertices from the entire graph.
	}
}


//
///*
// * TODO: Try to link the vertices at the same time they are selected as
// * 		executable within 'copyExecutableVertices'.
// */
//void PiSDFGraph::linkExecutableVertices(SDFGraph *outSDF){
//	UINT32 nbVertices = outSDF->getNbVertices();
//	for (UINT32 i = 0; i < nbVertices; i++)
//	{
//		BaseVertex* vertex = outSDF->getVertex(i);
//		// Excluding interface vertices.
//		if((vertex->getType() != input_vertex)&&(vertex->getType() != output_vertex))
//		{
//			UINT32 nbInputEdges = vertex->getNbInputEdges();
//			// Examining input edges.
//			for (UINT32 j = 0; j < nbInputEdges; j++)
//			{
//				PiSDFEdge* inputEdge = vertex->getInputEdge(j);
//				BaseVertex *sourceVertex = inputEdge->getSource();
//				if(sourceVertex->getType() == input_vertex)
//				{
//					// Getting incoming edge, i.e. from parent's predecessor vertex to input vertex.
//					UINT16 parentEdgeIndex = ((PiSDFIfVertex*)sourceVertex)->getParentEdgeIndex();
//					PiSDFEdge* parentEdge = ((PiSDFIfVertex*)sourceVertex)->getParentVertex()->getInputEdge(parentEdgeIndex);
//
//					// Bypassing the input vertex by adding an edge between the parent's predecessor and the current vertex.
//					if (parentEdge->getConsumptionInt() > 0)
//						outSDF->addEdge(parentEdge->getSource(),
//										parentEdge->getProductionInt(),
//										vertex,
//										parentEdge->getConsumptionInt());
//				}
//				else
//				{
//					/*
//					 * Checking if the input edge is valid, i.e. the consumption > 0 and if its source vertex is executable.
//					 * 'checkEdge' verifies if source and sink vertices are executables while only the source needs to be verified.
//					 * TODO: Verification wont be required of the linking is done within 'copyExecutableVertices'.
//					 */
//					if ((inputEdge->getConsumptionInt() > 0) && (outSDF->checkEdge(inputEdge)))
//					{
//						outSDF->addEdge(inputEdge->getSource(), inputEdge->getProductionInt(),
//										vertex, inputEdge->getConsumptionInt());
//					}
//				}
//			}
//		}
//		else if(vertex->getType() == output_vertex)
//		{
//			PiSDFEdge* inputEdge = vertex->getInputEdge(0); // There is only one input.
//			/*
//			 * Checking if the input edge is valid, i.e. the consumption > 0 and if its source vertex is executable.
//			 * 'checkEdge' verifies if source and sink vertices are executables while only the source needs to be verified.
//			 * TODO: Verification wont be required of the linking is done within 'copyExecutableVertices'.
//			 */
//			if ((inputEdge->getConsumptionInt() > 0) && (outSDF->checkEdge(inputEdge)))
//			{
//				// Getting outgoing edge, i.e. from output vertex to parent's successor vertex.
//				UINT16 parentEdgeIndex = ((PiSDFIfVertex*)vertex)->getParentEdgeIndex();
//				PiSDFEdge* parentEdge = ((PiSDFIfVertex*)vertex)->getParentVertex()->getOutputEdge(parentEdgeIndex);
//
//				// Bypassing the output vertex by adding an edge between the current vertex' predecessor and the parent's successor.
//				outSDF->addEdge(inputEdge->getSource(), inputEdge->getProductionInt(),
//								parentEdge->getSink(), parentEdge->getConsumptionInt());
//			}
//
//			// Removing output vertices from 'outSDF'.
//			outSDF->removeVertex(i);
//		}
//		else
//			// Removing input vertex from 'outSDF'.
//			outSDF->removeVertex(i);
//	}
//}


void PiSDFGraph::connectExecVertices(SDFGraph *outSDF)
{
	// Connecting the executable vertices.
	for (UINT32 i = 0; i < outSDF->getNbVertices(); i++) {
		BaseVertex* execVertex = outSDF->getVertex(i);

		for (UINT32 i = 0; i < execVertex->getNbOutputEdges(); i++){
			PiSDFEdge* edge = execVertex->getOutputEdge(i);
			BaseVertex* sinkVertex = edge->getSink();

			// Bypassing hierarchical vertices.
			if(sinkVertex->getType() == pisdf_vertex)
			{
				PiSDFGraph* subGraph = ((PiSDFVertex*)sinkVertex)->getSubGraph();
				if(subGraph != (PiSDFGraph*)0)
				{
					// Getting the successor of the corresponding input vertex.
					sinkVertex = subGraph->getInputVertex(edge)->getOutputEdge(0)->getSink();
				}
				outSDF->addEdge(execVertex, edge->getProductionInt(), sinkVertex, edge->getConsumptionInt());
			}
			// Bypassing output vertices.
			else if(sinkVertex->getType() == output_vertex)
			{
				PiSDFEdge* parentEdge = ((PiSDFIfVertex*)sinkVertex)->getParentEdge();
				sinkVertex = parentEdge->getSink();
				// Adding the edge with the parent vertex as sink.
				outSDF->addEdge(execVertex, edge->getProductionInt(), sinkVertex, parentEdge->getConsumptionInt());
			}
			// Connecting common vertices.
			else
				outSDF->addEdge(execVertex, edge->getProductionInt(), sinkVertex, edge->getConsumptionInt());

			/*
			 * Adding the sink vertex if it's not already present. Even if the sink vertex isn't executable,
			 * it is needed to build the topological matrix.
			 */
			if(outSDF->getVertexIndex(sinkVertex) == -1)
				outSDF->addVertex(sinkVertex);
		}
	}
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


void PiSDFGraph::createSubGraph(SDFGraph *outSDF)
{
	for (UINT32 i = 0; i < glbNbRequiredEdges; i++) {
		PiSDFEdge* edge = requiredEdges[i];


		BaseVertex* sourceVertex = edge->getSource();
		BaseVertex* sinkVertex = edge->getSink();
		BaseVertex* newSourceVertex = NULL;
		BaseVertex* newSinkVertex = NULL;
		/*** Adding edges to the new graph. ***/




		if(sinkVertex->getType() == pisdf_vertex)
		{
			PiSDFGraph* subGraph = ((PiSDFVertex*)sinkVertex)->getSubGraph();
			if(subGraph != (PiSDFGraph*)0) // Bypasses hierarchical vertices.
			{
				// Getting real sink vertex, i.e. not the input vertex but its successor.
				sinkVertex = subGraph->getInputVertex(edge)->getOutputEdge(0)->getSink();
			}
		}

		if(sourceVertex->getType() == input_vertex)
		{
			// Checking if this edge has
			// Getting real source vertex, i.e. not the input vertex but the parent's predecessor.
			sourceVertex = ((PiSDFIfVertex*)sourceVertex)->getParentEdge()->getSource();
		}

		// Adding the edge if not already present.
//		if((outSDF->getEdgeIndex(sourceVertex, sinkVertex) == -1) &&
//			())
//
//			outSDF->addEdge(sourceVertex, edge->getProductionInt(), sinkVertex, edge->getConsumptionInt());
//		outSDF->addEdge(sourceVertex, edge->getProductionInt(), sinkVertex, edge->getConsumptionInt());


//		else if(sinkVertex->getType() == output_vertex)
//		{
//			// Bypassing output vertices.
//			PiSDFEdge* parentEdge = ((PiSDFIfVertex*)sinkVertex)->getParentEdge();
//			sinkVertex = parentEdge->getSink();
//			// Adding the edge with the parent's successor vertex as sink.
//			outSDF->addEdge(sourceVertex, edge->getProductionInt(), sinkVertex, parentEdge->getConsumptionInt());
//		}
//		else
//			outSDF->addEdge(sourceVertex, edge->getProductionInt(), sinkVertex, edge->getConsumptionInt());



		// Adding vertices to the sub-graph if not already done.
		if(outSDF->getVertexIndex(sourceVertex) == -1)
			outSDF->addVertex(sourceVertex);

		if(outSDF->getVertexIndex(sinkVertex) == -1)
			outSDF->addVertex(sinkVertex);
	}
}


void PiSDFGraph::findRequiredEdges()
{
	for (UINT32 i = 0; i < nb_vertices; i++) {
		BaseVertex* vertex = vertices[i];
		if((vertex->getType() != input_vertex) && (vertex->getType() != output_vertex)){
			if(vertex->getExecutable() == undefined){
	//			copyExecutableVertices(vertices[i], outSDF);
				copyRequiredEdges(vertex);

				// Checking successors.
				for (UINT32 i = 0; i < vertex->getNbOutputEdges(); i++)
				{
					PiSDFEdge* edge = vertex->getOutputEdge(i);
					BaseVertex* sinkVertex = edge->getSink();
					if((sinkVertex->getExecutable() == undefined)&&(sinkVertex->getType()!= output_vertex))// Discards successors already marked as executable.
						copyRequiredEdges(sinkVertex);
				}
			}
		}
	}
}


void PiSDFGraph::clearIntraIteration(){
	// Clearing table of required edges.
	for (UINT32 i = 0; i < glbNbRequiredEdges; i++) {
		requiredEdges[i]->setRequired(false);
		requiredEdges[i] = NULL;
	}
	glbNbRequiredEdges = 0;

	for(UINT32 i = 0; i < glbNbVisitedIfs; i++){
		visitedIfs[i] = NULL;
	}
	glbNbVisitedIfs = 0;
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


/*
 * Inserts round buffer vertices between configure vertices and normal vertices.
 */
void PiSDFGraph::insertRoundBuffers(){
	for (UINT32 i = 0; i < nb_config_vertices; i++) {
		PiSDFConfigVertex* vertex = &config_vertices[i];
		for (UINT32 j = 0; j < vertex->getNbOutputEdges(); j++)
		{
			PiSDFEdge* edge = vertex->getOutputEdge(j);
			if (edge->getSink()->getType() != config_vertex){

				// Creating the new round buffer vertex.
				char name[MAX_VERTEX_NAME_SIZE];
				sprintf(name, "RoundB_%u", j);
				BaseVertex* roundB = addVertex(name, roundBuff_vertex);

				// Adding an new edge between the round buffer and the sink.
				addEdge(roundB, edge->getConsumption(), edge->getSink(), edge->getConsumption(), edge->getDelay());

				// Modifying the original edge so that the sink vertex is the round buffer.
				edge->setSink(roundB);
				edge->setConsumption(edge->getProduction());
			}
		}
	}
}

/*
 * Says if Vx is an interface/round-buffer preceding a configure vx.
 */
bool PiSDFGraph::isConfigVxPred(BaseVertex* Vx){
	if((Vx->getType() == input_vertex) || (Vx->getType() == roundBuff_vertex))
		return (Vx->getOutputEdge(0)->getSink()->getType() == config_vertex);
}

/*
 * Creates SDF graph including normal vertices.
 */
void PiSDFGraph::createSDF(SDFGraph* outSDF){
	for (UINT32 i = 0; i < nb_vertices; i++) {
		BaseVertex* refSource = vertices[i];
		if((refSource->getType() != config_vertex) && 	// Excludes configure Vxs.
			!isConfigVxPred(refSource)){ 				// Excludes interface/round_buffer Vxs which precede configure Vxs.
			// Adding the Vx into the output graph if not already present.
			if(outSDF->getVertexIndex(refSource) == -1) outSDF->addVertex(refSource);

			// Adding output edges.
			for (UINT32 j = 0; j < refSource->getNbOutputEdges(); j++) {
				PiSDFEdge* edge = refSource->getOutputEdge(j);
				BaseVertex* refSink = edge->getSink();
				// Adding the sink vx if not already present.
				if(outSDF->getVertexIndex(refSink) == -1) outSDF->addVertex(refSink);

				// Adding edges to SDF graph.
				outSDF->addEdge(refSource, edge->getProductionInt(), refSink, edge->getConsumptionInt());
			}
		}
	}
}

/*
 * Creates SrDAG including only 'vxsType' vertices.
 * Assumes the list of vertices is in topological order.
 */
void PiSDFGraph::createSrDAGInputConfigVxs(SRDAGGraph* outSrDAG){
	// Adding configure vxs.
	for (UINT32 i = 0; i < nb_config_vertices; i++) {
		PiSDFConfigVertex* refConfigVertex = &config_vertices[i];

		// Finding the vx if it was already added into outSrDAG.
		SRDAGVertex* srDagVertex = 0;
		outSrDAG->getVerticesFromReference(refConfigVertex, &srDagVertex);
		// Adding the vx if not found.
		if(!srDagVertex){
			srDagVertex = outSrDAG->addVertex();
			srDagVertex->setReference(refConfigVertex);
			srDagVertex->setReferenceIndex(refConfigVertex->getId());
			srDagVertex->setState(SrVxStExecutable);
		}

		// Adding edges from input vxs (and the input vx too of course).
		for (UINT32 j = 0; j < refConfigVertex->getNbInputEdges(); j++) {
			PiSDFEdge* edge = refConfigVertex->getInputEdge(j);
			BaseVertex* predec = edge->getSource();
			if(predec->getType() == input_vertex){
				SRDAGVertex*source = outSrDAG->addVertex();
				source->setReference(predec);
				source->setReferenceIndex(predec->getId());

				// Evaluating expressions. TODO: Maybe not needed.
				int prod, cons, delay;
				globalParser.interpret(edge->getProduction(), &prod);
				globalParser.interpret(edge->getConsumption(), &cons);
				globalParser.interpret(edge->getDelay(), &delay);
				edge->setProductionInt(prod);
				edge->setConsumtionInt(cons);
				edge->setDelayInt(delay);
				edge->setEvaluated(TRUE);

				// Adding edge into outSrDAG between input vx and configure vx.
				outSrDAG->addEdge(source, edge->getProductionInt(), srDagVertex);
			}
		}

		// Adding output edges (and the input vx too of course).
		for (UINT32 j = 0; j < refConfigVertex->getNbOutputEdges(); j++) {
			PiSDFEdge* edge = refConfigVertex->getOutputEdge(j);
			BaseVertex* succec = edge->getSink();
			// Finding the sink vx if it was already added into outSrDAG.
			SRDAGVertex* sink = 0;
			outSrDAG->getVerticesFromReference(succec, &sink);
			// Adding the sink vx if not found.
			if(!sink){
				sink = outSrDAG->addVertex();
				sink->setReference(succec);
				sink->setReferenceIndex(succec->getId());
			}

			// Evaluating expressions. Assuming the parameters which the configure vertices depend on, are solved.
			int prod, cons, delay;
			globalParser.interpret(edge->getProduction(), &prod);
			globalParser.interpret(edge->getConsumption(), &cons);
			globalParser.interpret(edge->getDelay(), &delay);
			edge->setProductionInt(prod);
			edge->setConsumtionInt(cons);
			edge->setDelayInt(delay);
			edge->setEvaluated(TRUE);

			// Adding edge into outSrDAG between configure vx and successor vx (round buffer or other configure vx)
			outSrDAG->addEdge(srDagVertex, edge->getProductionInt(), sink);
		}
	}


	// Adding input vxs into outSrDAG which are not connected to configure vxs.
	for (UINT32 i = 0; i < nb_input_vertices; i++) {
		PiSDFIfVertex* refInputVx = &input_vertices[i];
		if(refInputVx->getOutputEdge(0)->getSink()->getType() != config_vertex){
			SRDAGVertex* srDagVertex = outSrDAG->addVertex();
			srDagVertex->setReference(refInputVx);
			srDagVertex->setReferenceIndex(refInputVx->getId());
		}
	}
}


void PiSDFGraph::multiStepScheduling(BaseSchedule* schedule,
							ListScheduler* listScheduler,
							Architecture* arch,
							launcher* launch,
							ExecutionStat* execStat,
							SRDAGGraph* dag){
	if(nb_config_vertices > 0){
		// Creating SrDAG with the configure vertices.
		// TODO: treat delays
		if(dag->getNbVertices() == 0)
			createSrDAGInputConfigVxs(dag);
		else{
			SRDAGGraph 	localDag;
			createSrDAGInputConfigVxs(&localDag);
		#if PRINT_GRAPH
			// Printing the dag.
			dotWriter.write(&localDag, SUB_SRDAG_FILE_PATH, 1, 1);
			dotWriter.write(&localDag, SUB_SRDAG_FIFO_ID_FILE_PATH, 1, 0);
		#endif
			dag->merge(&localDag);
		}

	#if PRINT_GRAPH
		// Printing the dag.
		dotWriter.write(dag, SUB_SRDAG_FILE_PATH, 1, 1);
		dotWriter.write(dag, SUB_SRDAG_FIFO_ID_FILE_PATH, 1, 0);
	#endif

		// Scheduling the DAG.
		schedule->reset();
		listScheduler->schedule(dag, schedule, arch);

		ScheduleWriter schedWriter;
		schedWriter.write(schedule, dag, arch, "test.xml");

		// Preparing FIFOs information.
		launch->prepareFIFOsInfo(dag);

		// Preparing tasks' informations
		launch->prepareTasksInfo(dag, arch, schedule, false, execStat);

		// Resolving parameters.
		solveParameters(dag);
	}


	// Resolving productions/consumptions.
	evaluateExpressions();

	// Generating SDF from PiSDF excluding the configure vertices.
	SDFGraph sdf;
	createSDF(&sdf);

#if PRINT_GRAPH
	// Printing the SDF sub-graph.
	dotWriter.write(&sdf, SUB_SDF_FILE_0_PATH, 1);
#endif

	// Computing BRV of normal vertices.
	PiSDFTransformer transformer;
	transformer.computeBVR(&sdf);

	// Updating the productions of the round buffer vertices.
	sdf.updateRBProd();

#if PRINT_GRAPH
	// Printing the SDF sub-graph.
	dotWriter.write(&sdf, SUB_SDF_FILE_0_PATH, 1);
#endif


	// Transforming SDF with no configure vertices into SrDAG.
	// TODO: treat delays
	if(dag->getNbVertices() == 0)
		transformer.transform(&sdf, dag);
	else{
		SRDAGGraph 	localDag;
		transformer.transform(&sdf, &localDag);
	#if PRINT_GRAPH
		// Printing the dag.
		dotWriter.write(&localDag, SUB_SRDAG_FILE_PATH, 1, 1);
		dotWriter.write(&localDag, SUB_SRDAG_FIFO_ID_FILE_PATH, 1, 0);
	#endif
		dag->merge(&localDag);
	}
	// Updating vxs' states.
	updateDAGStates(dag);

#if PRINT_GRAPH
	// Printing the dag.
	dotWriter.write(dag, SRDAG_FILE_PATH, 1, 1);
	dotWriter.write(dag, SRDAG_FIFO_ID_FILE_PATH, 1, 0);
#endif

}


void PiSDFGraph::solveParameters(SRDAGGraph* dag){
	for (UINT32 i = 0; i < nb_config_vertices; i++) {
		PiSDFConfigVertex* configVertex = &config_vertices[i];
//		configVertex->setStatus(VxStExecuted);
		for (UINT32 j = 0; j < configVertex->getNbRelatedParams(); j++) {
			PiSDFParameter* param = configVertex->getRelatedParam(j);
			// TODO: to find out the returned value when there are several parameters.
			if (!param->getResolved()){
#if EXEC == 1
				UINT32 slaveId;
				if(schedule.findSlaveId(configVertex->getId(), configVertex, &slaveId)){
					UINT64 value = RTQueuePop_UINT32(slaveId, RTCtrlQueue);
					configVertex->getRelatedParam(j)->setValue(value);
				}
#else
//					UINT64 value = 352 * 255 / 256; // for the mpeg4 decoder application.
				UINT64 value = 2; 				// for the DoubleLoop application.

				configVertex->getRelatedParam(j)->setValue(value);
#endif
			}
		}

		// Getting the corresponding vertex in the SrDAG. Note that since the
		// reference is a configure vx, there is only one replica in the SrDAG.
		SRDAGVertex* srVx;
		dag->getVerticesFromReference(configVertex, &srVx);
		srVx->setState(SrVxStExecuted);
	}
}


void PiSDFGraph::updateDAGStates(SRDAGGraph* dag){
	for (UINT32 i = 0; i < dag->getNbVertices(); i++) {
		SRDAGVertex* Vx = dag->getVertex(i);
		if(Vx->getState() == SrVxStNoExecuted){
			if(Vx->getReference()->getType() == pisdf_vertex){
				PiSDFGraph* subGraph;
				if(((PiSDFVertex*)Vx->getReference())->hasSubGraph(&subGraph))
					Vx->setState(SrVxStHierarchy);
				else
					if(Vx->checkPredec())
						Vx->setState(SrVxStExecutable);
			}
			else if(Vx->getReference()->getType() == roundBuff_vertex){
				Vx->setState(SrVxStExecutable);
			}
		}
	}
}

