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

#include <transformations/MatrixHandler.h>
#include <graphs/PiSDF/PiSDFGraph.h>
#include "PiSDFTransformer.h"

#include <cmath>
#include <debuggingOptions.h>
#include <tools/DotWriter.h>
#include <tools/ScheduleWriter.h>
#include <tools/Queue.h>

#include <platform_time.h>


#if PRINT_GRAPH
static char file[MAX_FILE_NAME_SIZE];
#endif

#define MAX(a,b) ((a>b)?a:b)

/**
 Different SRDAG repetitions of an PiSDF vertex source to generate edges
 */
static SRDAGVertex* sourceRepetitions[MAX_VERTEX_REPETITION];

/**
 Different SRDAG repetitions of an PiSDF vertex sink to generate edges
 */
static SRDAGVertex* sinkRepetitions[MAX_VERTEX_REPETITION];


static UINT32 vertexId[MAX_NB_VERTICES];
static INT32 topo_matrix [MAX_NB_EDGES * MAX_NB_VERTICES];
static int tempBrv[MAX_NB_VERTICES];
static SRDAGGraph localDag;
static BaseSchedule schedule;

void PiSDFTransformer::addVertices(PiSDFAbstractVertex* vertex, UINT32 nb_repetitions, UINT32 iteration, SRDAGGraph* outputGraph){
	// Adding one SRDAG vertex per repetition
	for(UINT32 j = 0; j < nb_repetitions; j++){
		SRDAGVertex* srdag_vertex = outputGraph->addVertex();

		srdag_vertex->setFunctIx(vertex->getFunction_index());
		srdag_vertex->setReference(vertex);
		srdag_vertex->setReferenceIndex(j);
		srdag_vertex->setIterationIndex(iteration);
	}
}


void PiSDFTransformer::linkvertices(PiSDFGraph* currentPiSDF, UINT32 iteration, SRDAGGraph* topDag, int* brv)
{
	UINT32 cntExpVxs = 0;
	UINT32 cntImpVxs = 0;

	for (UINT32 i = 0; i < currentPiSDF->getNb_edges(); i++) {
		PiSDFEdge *edge = currentPiSDF->getEdge(i);

		if(edge->getSink()->getType() == config_vertex)
			continue;

		UINT32 nbDelays = edge->getDelayInt();

		UINT32 nbSourceRepetitions = brv[edge->getSource()->getId()];
		UINT32 nbTargetRepetitions = brv[edge->getSink()->getId()];

		// Getting the replicas of the source vertex into sourceRepetitions.
		topDag->getVerticesFromReference(edge->getSource(), iteration, sourceRepetitions);

		// Getting the replicas of the sink vertex into sinkRepetitions.
		topDag->getVerticesFromReference(edge->getSink(), iteration, sinkRepetitions);

		// Total number of token exchanged (produced and consumed) for an edge.
		UINT32 totalNbTokens = edge->getProductionInt() * nbSourceRepetitions;

		// Absolute target is the targeted consumed token among the total number of consumed/produced tokens.
		UINT32 absoluteSource = 0;
//		UINT32 producedTokens = 0;
		UINT32 absoluteTarget = nbDelays;
//		UINT32 availableTokens = nbDelays;

		// totProd is updated to store the number of token consumed by the
		// targets that are "satisfied" by the added edges.
		UINT32 totProd = 0;

		// Iterating until all consumptions are "satisfied".
		while (totProd < totalNbTokens) {
			/*
			 * Computing the indexes and rates.
			 */
			// Index of the source vertex's instance (among all the replicas).
			UINT32 sourceIndex = (absoluteSource / edge->getProductionInt())% nbSourceRepetitions;

			// Index of the target vertex's instance (among all the replicas).
			UINT32 targetIndex = (absoluteTarget / edge->getConsumptionInt())% nbTargetRepetitions;

			// Number of token already produced/consumed by the current source/target.
			UINT32 sourceProd = absoluteSource % edge->getProductionInt();
			UINT32 targetCons = absoluteTarget % edge->getConsumptionInt();

			// Production/consumption rate for the current source/target.
			UINT32 rest =((edge->getProductionInt() - sourceProd) < (edge->getConsumptionInt() - targetCons))?
					(edge->getProductionInt() - sourceProd):(edge->getConsumptionInt() - targetCons); // Minimum.


			/*
			 * Adding explode/implode vertices if required.
			 */

			if (rest < edge->getProductionInt() &&
				(sourceRepetitions[sourceIndex]->getType() == 0)){ // Type == 0 indicates it is a normal SR vx.

				// Adding an explode vertex.
				SRDAGVertex *exp_vertex = topDag->addVertex();
				exp_vertex->setType(Explode); 			// Indicates it is an explode vx.
//				exp_vertex->setExpImpId(nbExpVxs++);
				exp_vertex->setExpImpId(i);

				// Replacing the source vertex by the explode vertex in the array of sources.
				SRDAGVertex *origin_vertex = sourceRepetitions[sourceIndex];
				sourceRepetitions[sourceIndex] = exp_vertex;

				exp_vertex->setFunctIx(XPLODE_FUNCT_IX);
				exp_vertex->setReference(origin_vertex->getReference());
				exp_vertex->setReferenceIndex(origin_vertex->getReferenceIndex());
				exp_vertex->setIterationIndex(origin_vertex->getIterationIndex());
				cntExpVxs++;

				SRDAGVertex* sourceVertex;
				UINT32 sourcePortId;
				if(origin_vertex->getReference()->getType() == config_vertex){
					sourceVertex = origin_vertex->getOutputEdge(edge->getSource()->getOutputEdgeId(edge))->getSink();
					sourcePortId = 0;
				}else{
					sourceVertex = origin_vertex;
					sourcePortId = origin_vertex->getReference()->getOutputEdgeId(edge);
				}

				// Adding an edge between the source and the explode.
				topDag->addEdge(
						sourceVertex, sourcePortId,
						edge->getProductionInt(),
						exp_vertex, 0,
						edge->getRefEdge());
			}

			if (rest < edge->getConsumptionInt() &&
				(sinkRepetitions[targetIndex]->getType() == 0)){ // Type == 0 indicates it is a normal vertex.

				// Adding an implode vertex.
				SRDAGVertex *imp_vertex = topDag->addVertex();
				imp_vertex->setType(Implode); 	// Indicates it is an implode vertex.
				imp_vertex->setExpImpId(i); // Distinction among implode vertices for the same SRDAGVertex.

				// Replacing the sink vertex by the implode vertex in the array of sources.
				SRDAGVertex *origin_vertex = sinkRepetitions[targetIndex];//	// Adding vxs
				sinkRepetitions[targetIndex] = imp_vertex;


				// Setting attributes from original vertex.
				imp_vertex->setFunctIx(XPLODE_FUNCT_IX);
				imp_vertex->setReference(origin_vertex->getReference());
				imp_vertex->setReferenceIndex(origin_vertex->getReferenceIndex());
				imp_vertex->setIterationIndex(origin_vertex->getIterationIndex());
				cntImpVxs++;

				// Adding an edge between the implode and the sink.
				topDag->addEdge(
						imp_vertex, 0,
						edge->getConsumptionInt(),
						origin_vertex, origin_vertex->getReference()->getInputEdgeId(edge),
						edge->getRefEdge());
			}



			/**************************
			 * Adding the edge.
			 **************************/

			// The delay.
			// This UINT32 represent the number of iteration separating the
			// currently indexed source and target (between which an edge is
			// added)
			// If this UINT32 is > to 0, this means that the added edge must
			// have
			// delays (with delay=prod=cons of the added edge).
			// Warning, this integer division is not factorable
			UINT32 iterationDiff = absoluteTarget / totalNbTokens - absoluteSource / totalNbTokens;

			// If the edge has a delay and that delay still exist in the
			// SRSDF (i.e. if the source & target do not belong to the same
			// "iteration")
			if (iterationDiff > 0) {
				// TODO: Treating delays
//					UINT32 addedDelays = iterationDiff * new_edge->getTokenRate();

				// Check that there are enough delays available
//					if (nbDelays < addedDelays) {
					// kdesnos: I added this check, but it will most
					// probably never happen
//					throw new RuntimeException(
//							"Insufficient delays on edge "
//									+ edge.getSource().getName() + "."
//									+ edge.getSourceInterface().getName()
//									+ "=>" + edge.getTarget().getName()
//									+ "."
//									+ edge.getTargetInterface().getName()
//									+ ". At least " + addedDelays
//									+ " delays missing.");
//					}
//					new_edge->setDelay(addedDelays);
//					nbDelays = nbDelays - addedDelays;
			} else {
				//Creating the new edge between normal vertices or between a normal and an explode/implode one.
				SRDAGVertex* sourceVertex, *sinkVertex;
				UINT32 sourcePortId, sinkPortId;
				if(edge->getSource()->getType() == config_vertex){
					sourceVertex = sourceRepetitions[sourceIndex]->getOutputEdge(edge->getSource()->getOutputEdgeId(edge))->getSink();
					sourcePortId = 0;
				}else{
					sourceVertex = sourceRepetitions[sourceIndex];
					if(sourceVertex->getType() == Explode)
						sourcePortId = sourceVertex->getNbOutputEdge();
					else
						sourcePortId = edge->getSource()->getOutputEdgeId(edge);
				}

				sinkVertex = sinkRepetitions[targetIndex];
				if(sinkRepetitions[targetIndex]->getType() == Implode)
					sinkPortId = sinkVertex->getNbInputEdge();
				else
					sinkPortId = edge->getSink()->getInputEdgeId(edge);


				SRDAGEdge* new_edge = topDag->addEdge(
						sourceVertex, sourcePortId,
						rest,
						sinkVertex, sinkPortId,
						edge->getRefEdge());
				new_edge->setDelay(0);
			}

			// Update the number of token produced/consumed by the current source/target.
			absoluteTarget += rest;
			absoluteSource += rest;

			// Update the totProd for the current edge (totProd is used in the condition of the While loop)
			totProd += rest;

			// In case of a round buffer
			// If all needed tokens were already produced
			// but not all tokens were produced (i.e. not all source copies
			// were considered yet)
			if ((totProd == (edge->getConsumptionInt() * nbTargetRepetitions)) &&
//				targetCopies.get(0) instanceof SDFInterfaceVertex &&
				(absoluteSource / edge->getProductionInt()) < nbSourceRepetitions) {
				totProd = 0;
			}
		}
	}
}

void PiSDFTransformer::replaceHwithRB(PiSDFGraph* currentPiSDF, SRDAGGraph* topDag, SRDAGVertex* currHSrDagVx){
	/* Replace hierarchical actor in topDag with RBs */
	int nb=currHSrDagVx->getNbInputEdge();
	for(int i=0; i<nb; i++){
		SRDAGEdge* edge = currHSrDagVx->getInputEdge(i);
		SRDAGVertex* rb = topDag->addVertex();
		rb->setFunctIx(10); // RB
		rb->setReference(currentPiSDF->getInput_vertex(i));
		rb->setReferenceIndex(0);
		rb->setIterationIndex(currHSrDagVx->getReferenceIndex());
		rb->setType(RoundBuffer); // RB

		edge->getSink()->removeInputEdge(edge);
		edge->setSink(rb);
		rb->setInputEdge(edge, 0);
	}
	nb=currHSrDagVx->getNbOutputEdge();
	for(int i=0; i<nb; i++){
		SRDAGEdge* edge = currHSrDagVx->getOutputEdge(i);
		SRDAGVertex* rb = topDag->addVertex();
		rb->setFunctIx(10); // RB
		rb->setReference(currentPiSDF->getOutput_vertex(i));
		rb->setReferenceIndex(0);
		rb->setIterationIndex(currHSrDagVx->getReferenceIndex());
		rb->setType(RoundBuffer); // RB

		edge->getSource()->removeOutputEdge(edge);
		edge->setSource(rb);
		rb->setOutputEdge(edge, 0);
	}
	topDag->removeVx(currHSrDagVx);
}

void PiSDFTransformer::addCAtoSRDAG(PiSDFGraph* currentPiSDF, SRDAGGraph* topDag, SRDAGVertex* currHSrDagVx){
	/* Put CA in topDag with RB between CA and /CA */
	for(UINT32 i=0; i<currentPiSDF->getNb_config_vertices(); i++){
		PiSDFConfigVertex* pi_ca = currentPiSDF->getConfig_vertex(i);
		SRDAGVertex* dag_ca = topDag->addVertex();
		dag_ca->setFunctIx(pi_ca->getFunction_index());
		dag_ca->setReference(pi_ca);
		dag_ca->setReferenceIndex(0);
		dag_ca->setIterationIndex(currHSrDagVx->getReferenceIndex());
		dag_ca->setType(ConfigureActor);

		for(UINT32 j=0; j<pi_ca->getNbInputEdges(); j++){
			SRDAGVertex* refvertex[1];
			PiSDFEdge* edge = pi_ca->getInputEdge(j);
			topDag->getVerticesFromReference(edge->getSource(), currHSrDagVx->getReferenceIndex(), refvertex);
			topDag->addEdge(
					refvertex[0], pi_ca->getInputEdgeIx(edge),
					edge->getConsumptionInt(),
					dag_ca, j,
					edge);
		}
		for(UINT32 j=0; j<pi_ca->getNbOutputEdges(); j++){
			PiSDFEdge* edge = pi_ca->getOutputEdge(j);
			if(edge->getSink()->getType() == output_vertex){
				SRDAGVertex* refvertex[1];
				topDag->getVerticesFromReference(edge->getSink(), currHSrDagVx->getReferenceIndex(), refvertex);
				topDag->addEdge(
					dag_ca, j,
					edge->getProductionInt(),
					refvertex[0], 0,
					edge);
			}
			if(edge->getSink()->getType() != config_vertex){
				SRDAGVertex* rb = topDag->addVertex();
				rb->setFunctIx(10); // RB
				rb->setEdgeReference(edge);
				rb->setReferenceIndex(0);
				rb->setIterationIndex(currHSrDagVx->getReferenceIndex());
				rb->setType(RoundBuffer); // RB
				topDag->addEdge(
						dag_ca, j,
						edge->getProductionInt(),
						rb, 0,
						edge);
			}
		}
	}
}

void PiSDFTransformer::computeBRV(PiSDFGraph* currentPiSDF, int* brv){

	int brvNbEdges=0, brvNbVertices=0;
	for (UINT32 i = 0; i < currentPiSDF->getNbVertices(); i++) {
		PiSDFAbstractVertex* vertex = currentPiSDF->getVertex(i);
		if(vertex->getType() == pisdf_vertex)
			vertexId[i] = brvNbVertices++;
		else{
			vertexId[i] = -1;
			brv[i] = 1;
		}
	}

	memset(topo_matrix, 0, sizeof(topo_matrix));

	// Filling the topology matrix(nbEdges x nbVertices).
	for(UINT32 i = 0; i < currentPiSDF->getNb_edges(); i++){
		PiSDFEdge* edge = currentPiSDF->getEdge(i);
		if((edge->getSource() != edge->getSink()) &&
			(edge->getSource()->getType() ==  pisdf_vertex) &&
			(edge->getSink()->getType() ==  pisdf_vertex)){ // TODO: treat cycles.
			if((edge->getProductionInt() <= 0) || (edge->getConsumptionInt() <= 0 )) exitWithCode(1066);
			topo_matrix[brvNbEdges * brvNbVertices + vertexId[edge->getSource()->getId()]] = edge->getProductionInt();
			topo_matrix[brvNbEdges * brvNbVertices + vertexId[edge->getSink()->getId()]] = -edge->getConsumptionInt();
			brvNbEdges++;
		}
	}


	if(brvNbEdges > 0){
		// Computing the null space (BRV) of the matrix.
		// TODO: It may be improved by another algorithm to compute the null space (I'm not very sure of this one...)
		if(!nullspace(brvNbEdges, brvNbVertices, (int*)topo_matrix, tempBrv) == 0)
		{
			printf("Not Schedulable\n");
		}
	}

	/* Updating the productions of the round buffer vertices. */
	UINT32 coef=1;

	/* Looking on interfaces */
	for(UINT32 i = 0; i < currentPiSDF->getNb_input_vertices(); i++){
		PiSDFIfVertex* interface = currentPiSDF->getInput_vertex(i);
		coef = MAX(coef, std::ceil((double)interface->getOutputEdge(0)->getConsumptionInt()
											/(double)interface->getOutputEdge(0)->getProductionInt()));
	}
	for(UINT32 i = 0; i < currentPiSDF->getNb_output_vertices(); i++){
		PiSDFIfVertex* interface = currentPiSDF->getOutput_vertex(i);
		coef = MAX(coef, std::ceil((double)interface->getInputEdge(0)->getProductionInt())
											/(double)interface->getInputEdge(0)->getConsumptionInt());
	}
	/* Looking on implicit RB between CA and /CA */
	for(UINT32 i = 0; i < currentPiSDF->getNb_edges(); i++){
		PiSDFEdge* edge = currentPiSDF->getEdge(i);
		if(edge->getSource()->getType() == config_vertex
				&& edge->getSink()->getType() != config_vertex){
			coef = MAX(coef, std::ceil((double)edge->getConsumptionInt()
												/(double)edge->getProductionInt()));
		}
	}

	for(int i=0; i<brvNbVertices; i++){
		tempBrv[i] = tempBrv[i] * coef;
	}

	// Setting the number of repetitions for each vertex.
	brvNbVertices = 0;
	for (UINT32 i = 0; i < currentPiSDF->getNbVertices(); i++) {
		PiSDFAbstractVertex* vertex = currentPiSDF->getVertex(i);
		if(vertex->getType() == pisdf_vertex)
			brv[vertex->getId()] = tempBrv[vertexId[vertex->getId()]];
	}
}

static void PiSDFTransformer::singleRateTransformation(PiSDFGraph *currentPiSDF, SRDAGVertex *currHSrDagVx, SRDAGGraph *topDag, int *brv){
	/* Replace SRDAG of /CA in topDag */
	for (UINT32 i = 0; i < currentPiSDF->getNb_pisdf_vertices(); i++) {
		PiSDFAbstractVertex* vertex = currentPiSDF->getPiSDFVertex(i);

		// Creating the new vertices.
		PiSDFTransformer::addVertices(vertex, brv[vertex->getId()], currHSrDagVx->getReferenceIndex(), topDag);
	}

	// Connecting the vertices of the SrDAG ouput graph.
	PiSDFTransformer::linkvertices(currentPiSDF, currHSrDagVx->getReferenceIndex(), topDag, brv);
}

void PiSDFTransformer::multiStepScheduling(
							Architecture* arch,
							PiSDFGraph* pisdf,
							ListScheduler* listScheduler,
							SRDAGGraph* topDag){
	static int brv[MAX_NB_VERTICES];
	PiSDFGraph*   currentPiSDF;
	UINT32 len;
	UINT32 	lvlCntr = 0;
	UINT8 	stepsCntr = 0;

	Queue<PiSDFGraph*, 10> graphFifo;
	Queue<SRDAGVertex*, 10> vertexFifo;

	schedule.reset();
	graphFifo.reset();
	schedule.setNbActiveSlaves(arch->getNbActiveSlaves());

	PiSDFVertex* root = (PiSDFVertex*)pisdf->getRootVertex();
	if(!root) exitWithCode(1070);
	if(!root->hasSubGraph()) exitWithCode(1069);

	SRDAGVertex* currHSrDagVx;
	topDag->getVerticesFromReference(root,0,&currHSrDagVx);

	platform_time_reset();
	Launcher::initSchedulingTime();

	root->getSubGraph(&currentPiSDF);

	do{
		do{
		#if PRINT_GRAPH
			// Printing the current PiSDF graph.
			len = snprintf(file, MAX_FILE_NAME_SIZE, "%s_%d.gv", PiSDF_FILE_PATH, lvlCntr);
			if(len > MAX_FILE_NAME_SIZE){
				exitWithCode(1072);
			}
			DotWriter::write(currentPiSDF, file, 1);

			// Printing the topDag
			len = snprintf(file, MAX_FILE_NAME_SIZE, "%s_%d.gv", "topDag_start", stepsCntr);
			if(len > MAX_FILE_NAME_SIZE)
				exitWithCode(1072);
			DotWriter::write(topDag, file, 1, 0);
		#endif

			if(currentPiSDF->getNb_config_vertices() > 0){
				/* Resolve */
				currentPiSDF->evaluateExpressions();

				/* Replace hierarchical actor in topDag with RBs */
				PiSDFTransformer::replaceHwithRB(currentPiSDF, topDag, currHSrDagVx);

				/* Put CA in topDag with RB between CA and /CA */
				PiSDFTransformer::addCAtoSRDAG(currentPiSDF, topDag, currHSrDagVx);

				currentPiSDF->updateDAGStates(topDag);

				graphFifo.push(currentPiSDF);
				vertexFifo.push(currHSrDagVx);
			}else{
				/* Resolve */
				currentPiSDF->evaluateExpressions();

				/* Replace hierarchical actor in topDag with RBs */
				PiSDFTransformer::replaceHwithRB(currentPiSDF, topDag, currHSrDagVx);

				PiSDFTransformer::computeBRV(currentPiSDF, brv);
				PiSDFTransformer::singleRateTransformation(currentPiSDF, currHSrDagVx, topDag, brv);
				currentPiSDF->updateDAGStates(topDag);
			}

			currentPiSDF = 0;
			for (UINT32 i = 0; i < topDag->getNbVertices(); i++) {
				currHSrDagVx = topDag->getVertex(i);
				if(currHSrDagVx->isHierarchical() && currHSrDagVx->getState() == SrVxStExecutable){
					if(((PiSDFVertex*)(currHSrDagVx->getReference()))->hasSubGraph()){
						((PiSDFVertex*)(currHSrDagVx->getReference()))->getSubGraph(&currentPiSDF);
						lvlCntr++;
						break;
					}
				}
			}
		}while(currentPiSDF); /* There is executable hierarchical actor in SRDAG */

		/* Schedule */
		listScheduler->schedule(topDag, &schedule, arch);

		// Assigning FIFO ids to executable vxs' edges.
		Launcher::assignFifo(topDag);

	#if PRINT_GRAPH
		len = snprintf(file, MAX_FILE_NAME_SIZE, "%s_%d.xml", SCHED_FILE_NAME, stepsCntr);
		if(len > MAX_FILE_NAME_SIZE)
			exitWithCode(1072);
		ScheduleWriter::write(&schedule, topDag, arch, file);

		len = snprintf(file, MAX_FILE_NAME_SIZE, "%s_%d.gv", "topDag_mid", stepsCntr);
		if(len > MAX_FILE_NAME_SIZE)
			exitWithCode(1072);
		DotWriter::write(topDag, file, 1, 0);
	#endif

		Launcher::endSchedulingTime();


	#if EXEC == 1
		// Executing the executable vxs.
		Launcher::launch(topDag, arch, &schedule);
	#endif

		// Updating states. Sets all executable vxs to executed since their execution was already launched.
		topDag->updateExecuted();

		/*
		 * Resolving parameters. If the actors' execution is disabled, the parameters
		 * should had been set at compile time.
		 */
	#if EXEC == 1
		// Waiting for parameters' values from LRT (configure actors' execution).
		Launcher::resolveParameters(arch, topDag);
	#endif

		Launcher::initSchedulingTime();
		while(!graphFifo.isEmpty()){
			currentPiSDF = graphFifo.pop();
			currHSrDagVx = vertexFifo.pop();

			// Resolving productions/consumptions.
			currentPiSDF->evaluateExpressions();

			/* Compute BRV */
			/*
			 * Setting temporal Ids to be used as indices in the topology matrix.
			 * Note that only normal Vxs are considered.
			 */
			PiSDFTransformer::computeBRV(currentPiSDF, brv);

			PiSDFTransformer::singleRateTransformation(currentPiSDF, currHSrDagVx, topDag, brv);

			currentPiSDF->updateDAGStates(topDag);
			stepsCntr++;
		}

		currentPiSDF = 0;
		for (UINT32 i = 0; i < topDag->getNbVertices(); i++) {
			currHSrDagVx = topDag->getVertex(i);
			if(currHSrDagVx->isHierarchical() && currHSrDagVx->getState() == SrVxStExecutable){
				if(((PiSDFVertex*)(currHSrDagVx->getReference()))->hasSubGraph()){
					((PiSDFVertex*)(currHSrDagVx->getReference()))->getSubGraph(&currentPiSDF);
					lvlCntr++;
					break;
				}
			}
		}
	}while(currentPiSDF);

	/*
	 * Last scheduling and execution. After all hierarchical levels have been flattened,
	 * there is one more execution to do for completing one complete execution of the model.
	 */
	listScheduler->schedule(topDag, &schedule, arch);

	// Assigning FIFO ids to executable vxs' edges.
	Launcher::assignFifo(topDag);

	Launcher::endSchedulingTime();

#if EXEC == 1
	/*
	 * Launching the execution on LRTs. The "true" means that is the last execution
	 * of the current iteration, so the local RTs clear the tasks table and
	 * send back execution information.
	 */
	Launcher::launch(topDag, arch, &schedule);
	Launcher::createRealTimeGantt(arch, topDag, "Gantt.xml");
#endif

	// Updating states. Sets all executable vxs to executed since their execution was already launched.
	topDag->updateExecuted();

#if PRINT_GRAPH
	// Printing the final dag.
	len = snprintf(file, MAX_FILE_NAME_SIZE, "%s.gv", SRDAG_FILE_PATH);
	if(len > MAX_FILE_NAME_SIZE){
		exitWithCode(1072);
	}
	DotWriter::write(topDag, file, 1, 1);
	// Printing the final dag with FIFO ids.
	len = snprintf(file, MAX_FILE_NAME_SIZE, "%s.gv", SRDAG_FIFO_ID_FILE_PATH);
	if(len > MAX_FILE_NAME_SIZE){
		exitWithCode(1072);
	}
	DotWriter::write(topDag, file, 1, 0);
#endif

}
