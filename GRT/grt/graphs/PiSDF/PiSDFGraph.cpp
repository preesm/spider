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
#include <tools/DotWriter.h>
#include <tools/ScheduleWriter.h>
#include "PiSDFGraph.h"
#include <transformations/PiSDFTransformer/PiSDFTransformer.h>
#include "debuggingOptions.h"

extern DotWriter dotWriter;
static char name[MAX_VERTEX_NAME_SIZE];

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
//				glbNbConfigVertices++;
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


void PiSDFGraph::clearIntraIteration(){
//	// Clearing table of required edges.
//	for (UINT32 i = 0; i < glbNbRequiredEdges; i++) {
//		requiredEdges[i]->setRequired(false);
//		requiredEdges[i] = NULL;
//	}
//	glbNbRequiredEdges = 0;
//
//	for(UINT32 i = 0; i < glbNbVisitedIfs; i++){
//		visitedIfs[i] = NULL;
//	}
//	glbNbVisitedIfs = 0;
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
	else
		return false;
}

/*
 * Creates SDF graph including normal vertices.
 */
void PiSDFGraph::createSDF(SDFGraph* outSDF){
	for (UINT32 i = 0; i < nb_vertices; i++) {
		BaseVertex* refSource = vertices[i];
		if((!refSource->invalidEdges()) &&				// Excludes vxs whose input and output edges do not exchange tokens.
			(refSource->getType() != config_vertex) && 	// Excludes configure Vxs.
			!isConfigVxPred(refSource)){ 				// Excludes interface/round_buffer Vxs which precede configure Vxs.
			// Adding the Vx into the output graph if not already present.
			if(outSDF->getVertexIndex(refSource) == -1) outSDF->addVertex(refSource);

			// Adding output edges.
			for (UINT32 j = 0; j < refSource->getNbOutputEdges(); j++) {
				PiSDFEdge* edge = refSource->getOutputEdge(j);
				if((edge->getProductionInt() > 0) && (edge->getConsumptionInt() > 0)){
					BaseVertex* refSink = edge->getSink();
					// Adding the sink vx if not already present.
					if(outSDF->getVertexIndex(refSink) == -1) outSDF->addVertex(refSink);

					// Adding edges to SDF graph.
					outSDF->addEdge(refSource, edge->getProductionInt(), refSink, edge->getConsumptionInt(), edge);
				}
			}
		}
	}
}

/*
 * Creates SrDAG including only 'vxsType' vertices.
 * Assumes the list of vertices is in topological order.
 */
void PiSDFGraph::createSrDAGInputConfigVxs(SRDAGGraph* outSrDAG, SRDAGVertex* hSrDagVx){
	// Adding configure vxs.
	for (UINT32 i = 0; i < nb_config_vertices; i++) {
		PiSDFConfigVertex* refConfigVertex = &config_vertices[i];

		// Finding the vx if it was already added into outSrDAG.
		SRDAGVertex* srDagVertex = 0;
		outSrDAG->getVerticesFromReference(refConfigVertex, &srDagVertex);
		// Adding the vx if not found.
		if(!srDagVertex){
			srDagVertex = outSrDAG->addVertex();
			if(hSrDagVx)
				sprintf(name, "%s_%s", hSrDagVx->getName(), refConfigVertex->getName());
			else
				sprintf(name, "%s", refConfigVertex->getName());

			srDagVertex->setName(name);
			srDagVertex->setFunctIx(refConfigVertex->getFunction_index());
			srDagVertex->setReference(refConfigVertex);
			srDagVertex->setReferenceIndex(refConfigVertex->getId());
			srDagVertex->setState(SrVxStExecutable);
		}
		else
			srDagVertex->setState(SrVxStExecutable);

		// Adding edges coming from input vxs (and the input vx too of course).
		for (UINT32 j = 0; j < refConfigVertex->getNbInputEdges(); j++) {
			PiSDFEdge* edge = refConfigVertex->getInputEdge(j);
			BaseVertex* predec = edge->getSource();
			if(predec->getType() == input_vertex){
				SRDAGVertex*source = outSrDAG->addVertex();
				if(hSrDagVx)
					sprintf(name, "%s_%s", hSrDagVx->getName(), predec->getName());
				else
					sprintf(name, "%s", predec->getName());
				source->setName(name);
				source->setFunctIx(predec->getFunction_index());
				source->setReference(predec);
				source->setReferenceIndex(predec->getId());
				source->setState(SrVxStExecutable);
				source->setParent(hSrDagVx);

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
				outSrDAG->addEdge(source, edge->getProductionInt(), srDagVertex, edge);
			}
		}

		// Adding output edges (and the output vx too of course).
		for (UINT32 j = 0; j < refConfigVertex->getNbOutputEdges(); j++) {
			PiSDFEdge* edge = refConfigVertex->getOutputEdge(j);
			BaseVertex* succec = edge->getSink();
			// Finding the sink vx if it was already added into outSrDAG.
			SRDAGVertex* sink = 0;
			outSrDAG->getVerticesFromReference(succec, &sink);
			// Adding the sink vx if not found.
			if(!sink){
				sink = outSrDAG->addVertex();
				if(hSrDagVx)
					sprintf(name, "%s_%s", hSrDagVx->getName(), succec->getName());
				else
					sprintf(name, "%s", succec->getName());
				sink->setName(name);
				sink->setFunctIx(succec->getFunction_index());
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
			outSrDAG->addEdge(srDagVertex, edge->getProductionInt(), sink, edge);
		}
	}


	// Adding input vxs into outSrDAG which are not connected to configure vxs.
	for (UINT32 i = 0; i < nb_input_vertices; i++) {
		PiSDFIfVertex* refInputVx = &input_vertices[i];
		if(refInputVx->getOutputEdge(0)->getSink()->getType() != config_vertex){
			SRDAGVertex* inputSrDagVx = outSrDAG->addVertex();
			if(hSrDagVx)
				sprintf(name, "%s_%s", hSrDagVx->getName(), refInputVx->getName());
			else
				sprintf(name, "%s", refInputVx->getName());
			inputSrDagVx->setName(name);
			inputSrDagVx->setFunctIx(refInputVx->getFunction_index());
			inputSrDagVx->setReference(refInputVx);
			inputSrDagVx->setReferenceIndex(refInputVx->getId());
			inputSrDagVx->setParent(hSrDagVx);
		}
	}
}


void PiSDFGraph::multiStepScheduling(BaseSchedule* schedule,
							ListScheduler* listScheduler,
							Architecture* arch,
							launcher* launch,
							ExecutionStat* execStat,
							SRDAGGraph* dag,
							SRDAGVertex* currHSrDagVx,
							UINT32 level,
							UINT8* step){
	if(nb_config_vertices > 0){
		/*
		 * Creating a local SrDAG with configure and input vertices of the current level.
		 * The global DAG is used for the very first level, for it is empty
		 * and no merging will be required.
		 * TODO: treat delays and cycles?
		 */
		if(dag->getNbVertices() == 0)
			createSrDAGInputConfigVxs(dag, currHSrDagVx);
		else{
			SRDAGGraph 	localDag;
			createSrDAGInputConfigVxs(&localDag, currHSrDagVx);

			// Merging the local DAG into the global DAG.
			dag->merge(&localDag, false, level, *step);
		}

		(*step)++;

		// Scheduling the global DAG.
		listScheduler->schedule(dag, schedule, arch);

		ScheduleWriter schedWriter;
//		sprintf(name, "%s_%d.xml", SCHED_FILE_NAME, *stepsCntr);
		sprintf(name, "%s.xml", SCHED_FILE_NAME);
		schedWriter.write(schedule, dag, arch, name);

		// Clearing the buffers which will contain the data to be sent/received to/from LRTs.
		launch->clear();

		// Assigning FIFO ids to executable vxs' edges.
		launch->assignFIFOId(dag, arch);

		// Preparing tasks' information that will be sent to LRTs.
		launch->prepareTasksInfo(dag, arch->getNbSlaves(), schedule, false, execStat);

#if PRINT_GRAPH
//		// Printing the dag with FIFOs' Ids.
//		sprintf(name, "%s_%d.gv", SRDAG_FIFO_ID_FILE_PATH, *stepsCntr);
//		dotWriter.write(dag, name, 1, 0);
#endif

#if EXEC == 1
		// Executing the executable vxs.
		launch->launch(arch->getNbSlaves());
#endif

		// Updating states. Sets all executable vxs to executed since their execution was already launched.
		dag->updateExecuted();

		/*
		 * Resolving parameters. If the actors' execution is disabled, the parameters
		 * should had been set at compile time.
		 */
#if EXEC == 1
		// Waiting for parameters' values from LRT (configure actors' execution).
		launch->resolveParameters(dag, arch->getNbSlaves());
#endif
	}

	// Resolving productions/consumptions.
	evaluateExpressions();

	// Generating a local (for the current level) SDF without configure vertices.
	SDFGraph sdf;
	createSDF(&sdf);

#if PRINT_GRAPH
//	// Printing the SDF sub-graph.
//	sprintf(name, "subSDF_%d_0.gv", lvlCntr);
//	dotWriter.write(&sdf, name, 1);
#endif

	// Computing BRV of the local SDF. TODO: ..place this function in the SDF class...
	PiSDFTransformer transformer;
	transformer.computeBVR(&sdf);

	// Updating the productions of the round buffer vertices.
	sdf.updateRBProd();

	// Marking the current hierarchical vx as deleted, for it is going to be replaced by its sub-graph.
	if (currHSrDagVx) currHSrDagVx->setState(SrVxStDeleted);

#if PRINT_GRAPH
//	// Printing the local SDF with updated productions.
//	sprintf(name, "subSDF_%d_1.gv", *lvlCntr);
//	dotWriter.write(&sdf, name, 1);
#endif

	// Transforming local SDF into a local SrDAG.
	// TODO: treat delays
	if(dag->getNbVertices() == 0)
		// First step, so the global DAG is used for it is empty.
		transformer.transform(&sdf, dag, currHSrDagVx);
	else{
		/*
		 * From the second step, a local DAG is used for flattening the current level,
		 * then it is merged with the global DAG.
		 */
		SRDAGGraph 	localDag;
		transformer.transform(&sdf, &localDag, currHSrDagVx);

//	#if PRINT_GRAPH
//		// Printing the local DAG.
//		sprintf(name, "%s_%d.gv", SUB_SRDAG_FILE_PATH, *stepsCntr);
//		dotWriter.write(&localDag, name, 1, 1);
//	#endif

		/*
		 * Merging the local DAG into the global DAG. If there are configure vertices at this level,
		 * an INTRA-level merging is done. Otherwise, an INTER-level merging is done.
		 * See the merge method in the SRDAGGraph class for details.
		 */
		if(nb_config_vertices > 0)
			dag->merge(&localDag, true, level, *step);
		else
			dag->merge(&localDag, false, level, *step);
		(*step)++;

//#if PRINT_GRAPH
//		// Printing the global DAG.
//		sprintf(name, "%s_%d.gv", SRDAG_FILE_PATH, *stepsCntr);
//		dotWriter.write(dag, name, 1, 1);
//#endif
	}
	// Updating vxs' states.
	updateDAGStates(dag);

////	(*stepsCntr)++;
//#if PRINT_GRAPH
//	// Printing the dag.
//	sprintf(name, "%s_%d.gv", SRDAG_FILE_PATH, *lvlCntr);
//	dotWriter.write(dag, name, 1, 1);
//#endif
}


/*
 * Setting the parameter's value for each configure vx. It assumes that there is only
 * one parameter per configure vx.
 */
void PiSDFGraph::solveParameters(SRDAGGraph* dag, BaseSchedule* schedule){
	for (UINT32 i = 0; i < nb_config_vertices; i++) {
		PiSDFConfigVertex* configVertex = &config_vertices[i];
		for (UINT32 j = 0; j < configVertex->getNbRelatedParams(); j++) {
			PiSDFParameter* param = configVertex->getRelatedParam(j);
			// TODO: to find out the returned value when there are several parameters.
//			if (!param->getResolved()){
#if EXEC == 1
				SRDAGVertex* vx;
				dag->getVerticesFromReference(configVertex, &vx);
				UINT32 slaveId;
				if(schedule->findSlaveId(vx->getScheduleIndex(), vx, &slaveId)){
					UINT64 value = RTQueuePop_UINT32(slaveId, RTCtrlQueue);
					configVertex->getRelatedParam(j)->setValue(value);
				}
#else
//					UINT64 value = 352 * 255 / 256; // for the mpeg4 decoder application.
				UINT64 value = 2; 				// for the DoubleLoop application.
				configVertex->getRelatedParam(j)->setValue(value);
#endif
//			}
		}

		// Getting the corresponding vertex in the SrDAG. Note that since the
		// reference is a configure vx, there is only one replica in the SrDAG.
		SRDAGVertex* srVx;
		dag->getVerticesFromReference(configVertex, &srVx);
		srVx->setState(SrVxStExecuted);
	}
}


void PiSDFGraph::updateDAGStates(SRDAGGraph* dag){
	// Updating all input, round-buffer and hierarchical vxs.
	for (UINT32 i = 0; i < dag->getNbVertices(); i++) {
		SRDAGVertex* Vx = dag->getVertex(i);
		if(Vx->getState() == SrVxStNoExecuted){
			if((Vx->getReference()->getType() == input_vertex)||
				(Vx->getReference()->getType() == roundBuff_vertex)){
				Vx->setState(SrVxStExecutable);
			}
			else if(Vx->getReference()->getType() == pisdf_vertex){
				PiSDFGraph* subGraph;
				if(((PiSDFVertex*)Vx->getReference())->hasSubGraph(&subGraph)) Vx->setState(SrVxStHierarchy);
			}
		}
	}

	// Updating other vxs.
	for (UINT32 i = 0; i < dag->getNbVertices(); i++) {
		SRDAGVertex* Vx = dag->getVertex(i);
		if(Vx->getState() == SrVxStNoExecuted){
				Vx->checkForExecution();
		}
	}
}

