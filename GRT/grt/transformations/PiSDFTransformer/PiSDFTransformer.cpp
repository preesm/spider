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


//#if PRINT_GRAPH
static char file[MAX_FILE_NAME_SIZE];
//#endif

#define MAX(a,b) ((a>b)?a:b)
#define MIN(a,b) ((a<b)?a:b)

/**
 Different SRDAG repetitions of an PiSDF vertex source to generate edges
 */
static SRDAGVertex* sourceRepetitions[MAX_VERTEX_REPETITION];

/**
 Different SRDAG repetitions of an PiSDF vertex sink to generate edges
 */
static SRDAGVertex* sinkRepetitions[MAX_VERTEX_REPETITION];


static UINT32 vertexId[MAX_NB_PiSDF_VERTICES];
static int topo_matrix [MAX_NB_PiSDF_EDGES * MAX_NB_PiSDF_VERTICES];
static int tempBrv[MAX_NB_PiSDF_VERTICES];
static BaseSchedule schedule;

void PiSDFTransformer::addVertices(PiSDFVertex* vertex, UINT32 nb_repetitions, UINT32 iteration, SRDAGGraph* outputGraph){
	// Adding one SRDAG vertex per repetition
	for(UINT32 j = 0; j < nb_repetitions; j++){
		SRDAGVertex* srdag_vertex = outputGraph->addVertex();

		if(vertex->getFunction_index() == BROADCAST_FUNCT_IX)
			srdag_vertex->setType(Broadcast);
		else
			srdag_vertex->setType(Normal);

		srdag_vertex->setFunctIx(vertex->getFunction_index());
		srdag_vertex->setReference(vertex);
		srdag_vertex->setReferenceIndex(j);
		srdag_vertex->setIterationIndex(iteration);
		srdag_vertex->setExecTime(vertex->getExecTime());

		for(UINT32 i=0; i<vertex->getNbParameters(); i++)
			srdag_vertex->setParamValue(i,vertex->getParameter(i)->getValue());

//		if(srdag_vertex->isHierarchical())
//			outputGraph->storeHierVertex(srdag_vertex);
	}
}


void PiSDFTransformer::linkvertices(PiSDFGraph* currentPiSDF, UINT32 iteration, SRDAGGraph* topDag, int* brv)
{
	UINT32 cntExpVxs = 0;
	UINT32 cntImpVxs = 0;

	for (UINT32 i = 0; i < currentPiSDF->getNb_edges(); i++) {
		PiSDFEdge *edge = currentPiSDF->getEdge(i);

		// Already treated edge
		if(edge->getSink()->getType() == config_vertex)
			continue;

		UINT32 nbDelays = edge->getDelayInt();

		UINT32 nbSourceRepetitions = brv[edge->getSource()->getId()];
		UINT32 nbSinkRepetitions = brv[edge->getSink()->getId()];

		if(nbSourceRepetitions > MAX_VERTEX_REPETITION || nbSinkRepetitions > nbSinkRepetitions){
			printf("MAX_VERTEX_REPETITION too small\n");
			abort();
		}

		UINT32 sourceProduction = edge->getProductionInt();
		UINT32 sinkConsumption = edge->getConsumptionInt();

		if(edge->getSource()->getType() == input_vertex
				|| edge->getSource()->getType() == config_vertex){
			sourceProduction = sinkConsumption*nbSinkRepetitions;
		}
		if(edge->getSink()->getType() == output_vertex){
			sinkConsumption = sourceProduction*nbSourceRepetitions;
		}

		UINT32 sourceIndex = 0;
		UINT32 sinkIndex   = 0;

		UINT32 curSourceToken;
		UINT32 curSinkToken;

		// Fill source/sink repetition list
		if(nbDelays == 0){
			topDag->getVerticesFromReference(edge->getSource(), iteration, sourceRepetitions);
			topDag->getVerticesFromReference(edge->getSink(), iteration, sinkRepetitions);

			curSourceToken = sourceProduction;
			curSinkToken   = sinkConsumption;
		}else{
			// If there is delay, first source is an init vector and last sink is an end vector.

			// Adding an init vertex.
			SRDAGVertex *init_vertex = topDag->addVertex();
			init_vertex->setType(Init); 	// Indicates it is an implode vertex.
			init_vertex->setFunctIx(INIT_FUNCT_IX);
//			init_vertex->setReference(origin_vertex->getReference());
//			imp_vertex->setReferenceIndex(origin_vertex->getReferenceIndex());
//			imp_vertex->setIterationIndex(origin_vertex->getIterationIndex());
//			cntImpVxs++;
			sourceRepetitions[0] = init_vertex;

			topDag->getVerticesFromReference(edge->getSource(), iteration, sourceRepetitions+1);
			topDag->getVerticesFromReference(edge->getSink(), iteration, sinkRepetitions);


			SRDAGVertex *end_vertex = topDag->addVertex();
			end_vertex->setType(End); 	// Indicates it is an implode vertex.
			end_vertex->setFunctIx(END_FUNCT_IX);
			sinkRepetitions[nbSinkRepetitions] = end_vertex;

			nbSourceRepetitions++;
			nbSinkRepetitions++;

			curSourceToken = nbDelays;
			curSinkToken   = sinkConsumption;

		}


		// Absolute target is the targeted consumed token among the total number of consumed/produced tokens.
//		UINT32 absoluteSource = 0;
//		UINT32 producedTokens = 0;
//		UINT32 absoluteTarget = nbDelays;
//		UINT32 availableTokens = nbDelays;



		// Iterating until all consumptions are "satisfied".
		while (sourceIndex < nbSourceRepetitions
				|| sinkIndex < nbSinkRepetitions) {
			// Production/consumption rate for the current source/target.
			UINT32 rest = MIN(curSourceToken, curSinkToken); // Minimum.


			/*
			 * Adding explode/implode vertices if required.
			 */

			if (rest < curSourceToken &&
				(sourceRepetitions[sourceIndex]->getType() != Explode)){

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

				switch(origin_vertex->getType()){
				case Normal:
				case RoundBuffer:
					sourceVertex = origin_vertex;
					sourcePortId = origin_vertex->getReference()->getOutputEdgeId(edge);
					break;
				case ConfigureActor:
					sourceVertex = origin_vertex->getOutputEdge(edge->getSource()->getOutputEdgeId(edge))->getSink();
					sourcePortId = 0;
					break;
				case Init:
					sourceVertex = origin_vertex;
					sourcePortId = 0;
					break;
				default:
				case End:
				case Explode:
				case Implode:
					printf("Unexpected case in pisdf transfo\n");
					abort();
					break;
				}

				// Adding an edge between the source and the explode.
				topDag->addEdge(
						sourceVertex, sourcePortId,
						curSourceToken,
						exp_vertex, 0,
						edge->getRefEdge());
			}

			if (rest < curSinkToken &&
				(sinkRepetitions[sinkIndex]->getType() != Implode)){ // Type == 0 indicates it is a normal vertex.

				// Adding an implode vertex.
				SRDAGVertex *imp_vertex = topDag->addVertex();
				imp_vertex->setType(Implode); 	// Indicates it is an implode vertex.
				imp_vertex->setExpImpId(i); // Distinction among implode vertices for the same SRDAGVertex.

				// Replacing the sink vertex by the implode vertex in the array of sources.
				SRDAGVertex *origin_vertex = sinkRepetitions[sinkIndex];//	// Adding vxs
				sinkRepetitions[sinkIndex] = imp_vertex;


				// Setting attributes from original vertex.
				imp_vertex->setFunctIx(XPLODE_FUNCT_IX);
				imp_vertex->setReference(origin_vertex->getReference());
				imp_vertex->setReferenceIndex(origin_vertex->getReferenceIndex());
				imp_vertex->setIterationIndex(origin_vertex->getIterationIndex());
				cntImpVxs++;

				UINT32 sinkPortId;

				switch(origin_vertex->getType()){
				case Normal:
				case RoundBuffer:
					sinkPortId = origin_vertex->getReference()->getInputEdgeId(edge);
					break;
				case End:
					sinkPortId = 0;
					break;
				default:
				case ConfigureActor:
				case Init:
				case Explode:
				case Implode:
					printf("Unexpected case in pisdf transfo\n");
					abort();
					break;
				}

				// Adding an edge between the implode and the sink.
				topDag->addEdge(
						imp_vertex, 0,
						curSinkToken,
						origin_vertex, sinkPortId,
						edge->getRefEdge());
			}

			//Creating the new edge between normal vertices or between a normal and an explode/implode one.
			SRDAGVertex* sourceVertex, *sinkVertex;
			UINT32 sourcePortId, sinkPortId;
			sourceVertex = sourceRepetitions[sourceIndex];
			if(sourceVertex->getType() == Explode){
				sourcePortId = sourceVertex->getNbOutputEdge();
			}else if(sourceVertex->getType() == Init){
				sourcePortId = 0;
			}else if(edge->getSource()->getType() == config_vertex){
				sourceVertex = sourceVertex->getOutputEdge(edge->getSource()->getOutputEdgeId(edge))->getSink();
				sourcePortId = 0;
			}else{
				sourcePortId = edge->getSource()->getOutputEdgeId(edge);
			}

			sinkVertex = sinkRepetitions[sinkIndex];
			if(sinkVertex->getType() == Implode){
				sinkPortId = sinkVertex->getNbInputEdge();
			}else if(sinkVertex->getType() == End){
				sinkPortId = 0;
			}else{
				sinkPortId = edge->getSink()->getInputEdgeId(edge);
			}


			SRDAGEdge* new_edge = topDag->addEdge(
					sourceVertex, sourcePortId,
					rest,
					sinkVertex, sinkPortId,
					edge->getRefEdge());
			new_edge->setDelay(0);


			// Update the number of token produced/consumed by the current source/target.
			curSourceToken -= rest;
			curSinkToken -= rest;

			if(curSourceToken == 0){
				sourceIndex++;
				curSourceToken += sourceProduction;
			}

			if(curSinkToken == 0){
				sinkIndex++;
				if(sinkIndex == nbSinkRepetitions-1 && nbDelays != 0)
					curSinkToken += nbDelays;
				else
					curSinkToken += sinkConsumption;
			}

			// Update the totProd for the current edge (totProd is used in the condition of the While loop)

			// In case of a round buffer
			// If all needed tokens were already produced
			// but not all tokens were produced (i.e. not all source copies
			// were considered yet)
//			if ((totProd == (edge->getConsumptionInt() * nbTargetRepetitions)) &&
////				targetCopies.get(0) instanceof SDFInterfaceVertex &&
//				(absoluteSource / edge->getProductionInt()) < nbSourceRepetitions) {
//				totProd = 0;
//			}
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
		for(UINT32 i=0; i<pi_ca->getNbParameters(); i++)
			dag_ca->setParamValue(i,pi_ca->getParameter(i)->getValue());

		for(UINT32 j=0; j<pi_ca->getNbInputEdges(); j++){
			SRDAGVertex* refvertex[1];
			PiSDFEdge* edge = pi_ca->getInputEdge(j);
			topDag->getVerticesFromReference(edge->getSource(), currHSrDagVx->getReferenceIndex(), refvertex);

			if(edge->getProductionInt() == edge->getConsumptionInt()){
				SRDAGVertex* rb = refvertex[0];
				SRDAGEdge* edge = rb->getInputEdge(0);

				rb->removeInputEdge(edge);
				edge->setSink(dag_ca);
				dag_ca->setInputEdge(edge, j);
				topDag->removeVx(rb);
			}else{
				topDag->addEdge(
					refvertex[0], pi_ca->getInputEdgeIx(edge),
					edge->getConsumptionInt(),
					dag_ca, j,
					edge);
			}
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
			topo_matrix[brvNbEdges * brvNbVertices + vertexId[edge->getSink()->getId()]] = -(INT64)(edge->getConsumptionInt());
			brvNbEdges++;
		}
	}


	if(brvNbEdges > 0){
		// Computing the null space (BRV) of the matrix.
		// TODO: It may be improved by another algorithm to compute the null space (I'm not very sure of this one...)
		if(!nullspace(brvNbEdges, brvNbVertices, topo_matrix, tempBrv) == 0)
		{
			printf("Not Schedulable\n");
		}
	}else if(brvNbVertices == 1){
		tempBrv[0] = 1;
	}

	// Setting the number of repetitions for each vertex.
	for (UINT32 i = 0; i < currentPiSDF->getNbVertices(); i++) {
		PiSDFAbstractVertex* vertex = currentPiSDF->getVertex(i);
		if(vertex->getType() == pisdf_vertex)
			vertex->setNbRepetition(tempBrv[vertexId[vertex->getId()]]);
		else
			vertex->setNbRepetition(1);
	}

	/* Updating the productions of the round buffer vertices. */
	UINT32 coef=1;

	/* Looking on interfaces */
	for(UINT32 i = 0; i < currentPiSDF->getNb_input_vertices(); i++){
		PiSDFIfVertex* interface = currentPiSDF->getInput_vertex(i);
		coef = MAX(coef, std::ceil((double)(interface->getOutputEdge(0)->getProductionInt())
											/((double)(interface->getOutputEdge(0)->getConsumptionInt()*interface->getOutputEdge(0)->getSink()->getNbRepetition()))));
	}
	for(UINT32 i = 0; i < currentPiSDF->getNb_output_vertices(); i++){
		PiSDFIfVertex* interface = currentPiSDF->getOutput_vertex(i);
		coef = MAX(coef, std::ceil((double)interface->getInputEdge(0)->getConsumptionInt())
											/((double)interface->getInputEdge(0)->getProductionInt()*interface->getInputEdge(0)->getSource()->getNbRepetition()));
	}
	/* Looking on implicit RB between CA and /CA */
	for(UINT32 i = 0; i < currentPiSDF->getNb_edges(); i++){
		PiSDFEdge* edge = currentPiSDF->getEdge(i);
		if(edge->getSource()->getType() == config_vertex
				&& edge->getSink()->getType() != config_vertex){
			coef = MAX(coef, std::ceil((double)edge->getProductionInt()
												/((double)edge->getConsumptionInt()*edge->getSink()->getNbRepetition())));
		}
	}

	for(int i=0; i<brvNbVertices; i++){
		tempBrv[i] = tempBrv[i] * coef;
	}

	// Setting the number of repetitions for each vertex.
	for (UINT32 i = 0; i < currentPiSDF->getNbVertices(); i++) {
		PiSDFAbstractVertex* vertex = currentPiSDF->getVertex(i);
		if(vertex->getType() == pisdf_vertex){
			brv[vertex->getId()] = tempBrv[vertexId[vertex->getId()]];
			vertex->setNbRepetition(tempBrv[vertexId[vertex->getId()]]);
		}else{
			vertex->setNbRepetition(1);
		}
	}
}

static void PiSDFTransformer::singleRateTransformation(PiSDFGraph *currentPiSDF, SRDAGVertex *currHSrDagVx, SRDAGGraph *topDag, int *brv){
	/* Replace SRDAG of /CA in topDag */
	for (UINT32 i = 0; i < currentPiSDF->getNb_pisdf_vertices(); i++) {
		PiSDFVertex* vertex = currentPiSDF->getPiSDFVertex(i);

		// Creating the new vertices.
		PiSDFTransformer::addVertices(vertex, brv[vertex->getId()], currHSrDagVx->getReferenceIndex(), topDag);
	}

	// Connecting the vertices of the SrDAG ouput graph.
	PiSDFTransformer::linkvertices(currentPiSDF, currHSrDagVx->getReferenceIndex(), topDag, brv);
}

static UINT32 getValue(PiSDFParameter *param, SRDAGGraph* topDag, SRDAGVertex *currHSrDagVx){
	SRDAGVertex* config_vertex;
	PiSDFConfigVertex* pisdf_config = param->getSetter();
	if(pisdf_config!=0){
		#if EXEC == 1
			topDag->getVerticesFromReference(param->getSetter(), currHSrDagVx->getReferenceIndex(), &config_vertex);
			return config_vertex->getRelatedParamValue(param->getSetterIx());
		#else
			return param->getValue();
		#endif
	}else if(param->getParameterParentID() != -1){
		return currHSrDagVx->getReference()->getParameter(param->getParameterParentID())->getValue();
//		return getValue(param->getParameterParent(), topDag, config_vertex);
	}else{
		return param->getValue();
	}
}

static int reduceImplImpl(SRDAGGraph* topDag){
	for(int i=0; i<topDag->getNbVertices(); i++){
		SRDAGVertex* implode0 = topDag->getVertex(i);
		if(implode0->getType() == Implode && implode0->getState() != SrVxStDeleted){
			for(int j=0; j<implode0->getNbOutputEdge(); j++){
				SRDAGVertex* implode1 = implode0->getOutputEdge(j)->getSink();
				if(implode1->getType() == Implode && implode1->getState() != SrVxStDeleted){

					int nbToAdd = implode0->getNbInputEdge();
					int ixToAdd = implode1->getInputEdgeId(implode0->getOutputEdge(0));

					int nbTotEdges = implode1->getNbInputEdge()+nbToAdd-1;

					// Shift edges after
					for(int k=nbTotEdges-1; k>ixToAdd+nbToAdd-1; k--){
						SRDAGEdge *edge = implode1->getInputEdge(k-nbToAdd+1);
						implode1->removeInputEdge(edge);
						implode1->setInputEdge(edge, k);
					}
					// Add edges
					implode1->removeInputEdge(implode1->getInputEdge(ixToAdd));
					for(int k=0; k<nbToAdd; k++){
						SRDAGEdge *edge = implode0->getInputEdge(k);
						implode1->setInputEdge(edge, k+ixToAdd);
						edge->setSink(implode1);

						implode0->removeInputEdge(edge);
					}

					implode0->setState(SrVxStDeleted);
					return 1;
				}
			}
		}
	}
	return 0;
}

static int removeImpExp(SRDAGGraph* topDag){
	for(int i=0; i<topDag->getNbVertices(); i++){
		SRDAGVertex* implode = topDag->getVertex(i);
		if(implode->getType() == Implode && implode->getState() != SrVxStDeleted){
			SRDAGVertex* explode = implode->getOutputEdge(0)->getSink();
			if(explode->getType() == Explode && explode->getState() != SrVxStDeleted){
				UINT32 totalNbTokens = 0;
				UINT32 totProd = 0;

				UINT32 sourceIndex=0, sinkIndex=0;

				PiSDFEdge* refEdge = explode->getInputEdge(0)->getRefEdge();

				SRDAGVertex* sources[MAX_SRDAG_INPUT_EDGES];
				UINT32 sourceProds[MAX_SRDAG_INPUT_EDGES];
				UINT32 sourcePortIx[MAX_SRDAG_INPUT_EDGES];
				BOOL sourceExplode[MAX_SRDAG_INPUT_EDGES];
				UINT32 nbSourceRepetitions = implode->getNbInputEdge();
				for(int k=0; k<nbSourceRepetitions; k++){
					SRDAGEdge *edge = implode->getInputEdge(k);
					sources[k] = edge->getSource();
					sourceProds[k] = edge->getTokenRate();
					sourcePortIx[k] = sources[k]->getOutputEdgeId(edge);

					sources[k]->removeOutputEdge(edge);
					totalNbTokens += sourceProds[k];
					sourceExplode[k] = FALSE;
				}
				implode->setState(SrVxStDeleted);

				SRDAGVertex* sinks[MAX_SRDAG_INPUT_EDGES];
				UINT32 sinkCons[MAX_SRDAG_INPUT_EDGES];
				UINT32 sinkPortIx[MAX_SRDAG_INPUT_EDGES];
				UINT32 nbSinkRepetitions = explode->getNbOutputEdge();
				BOOL sinkImplode[MAX_SRDAG_INPUT_EDGES];
				for(int k=0; k<nbSinkRepetitions; k++){
					SRDAGEdge *edge = explode->getOutputEdge(k);
					sinks[k] = edge->getSink();
					sinkCons[k] = edge->getTokenRate();
					sinkPortIx[k] = sinks[k]->getInputEdgeId(edge);

					sinks[k]->removeInputEdge(edge);
					sinkImplode[k] = FALSE;
				}
				explode->setState(SrVxStDeleted);

				UINT32 curProd = sourceProds[0];
				UINT32 curCons = sinkCons[0];

				while (totProd < totalNbTokens) {
					// Production/consumption rate for the current source/target.
					UINT32 rest = (curProd > curCons) ? curCons:curProd;

					/*
					 * Adding explode/implode vertices if required.
					 */

					if (rest < curProd && !sourceExplode[sourceIndex]){
						// Adding an explode vertex.
						SRDAGVertex *exp_vertex = topDag->addVertex();
						exp_vertex->setType(Explode); 			// Indicates it is an explode vx.
						exp_vertex->setExpImpId(explode->getId());

						// Replacing the source vertex by the explode vertex in the array of sources.
						SRDAGVertex *origin_vertex = sources[sourceIndex];
						sources[sourceIndex] = exp_vertex;

						exp_vertex->setFunctIx(XPLODE_FUNCT_IX);
						exp_vertex->setReference(origin_vertex->getReference());
						exp_vertex->setReferenceIndex(origin_vertex->getReferenceIndex());
						exp_vertex->setIterationIndex(origin_vertex->getIterationIndex());

						sourceExplode[sourceIndex] = TRUE;

						// Adding an edge between the source and the explode.
						topDag->addEdge(
								origin_vertex, sourcePortIx[sourceIndex],
								sourceProds[sourceIndex],
								exp_vertex, 0,
								refEdge);
					}

					if (rest < curCons && !sinkImplode[sinkIndex]){
						// Adding an implode vertex.
						SRDAGVertex *imp_vertex = topDag->addVertex();
						imp_vertex->setType(Implode); 	// Indicates it is an implode vertex.
						imp_vertex->setExpImpId(implode->getId()); // Distinction among implode vertices for the same SRDAGVertex.

						// Replacing the sink vertex by the implode vertex in the array of sources.
						SRDAGVertex *origin_vertex = sinks[sinkIndex];//	// Adding vxs
						sinks[sinkIndex] = imp_vertex;

						// Setting attributes from original vertex.
						imp_vertex->setFunctIx(XPLODE_FUNCT_IX);
						imp_vertex->setReference(origin_vertex->getReference());
						imp_vertex->setReferenceIndex(origin_vertex->getReferenceIndex());
						imp_vertex->setIterationIndex(origin_vertex->getIterationIndex());

						sinkImplode[sinkIndex] = TRUE;

						// Adding an edge between the implode and the sink.
						topDag->addEdge(
								imp_vertex, 0,
								sinkCons[sinkIndex],
								origin_vertex, sinkPortIx[sinkIndex],
								refEdge);
					}

					//Creating the new edge between normal vertices or between a normal and an explode/implode one.
					UINT32 sourcePortId, sinkPortId;
					if(sourceExplode[sourceIndex]){
						sourcePortId = sources[sourceIndex]->getNbOutputEdge();
					}else{
						sourcePortId = sourcePortIx[sourceIndex];
					}

					if(sinkImplode[sinkIndex])
						sinkPortId = sinks[sinkIndex]->getNbInputEdge();
					else
						sinkPortId = sinkPortIx[sinkIndex];


					SRDAGEdge* new_edge = topDag->addEdge(
							sources[sourceIndex], sourcePortId,
							rest,
							sinks[sinkIndex], sinkPortId,
							refEdge);

					// Update the totProd for the current edge (totProd is used in the condition of the While loop)
					totProd += rest;

					curCons -= rest;
					curProd -= rest;

					if(curProd == 0){
						curProd += sourceProds[++sourceIndex];
					}
					if(curCons == 0){
						curCons += sinkCons[++sinkIndex];
					}
				}
				return 1;
			}
		}
	}
	return 0;
}

static int removeRBExp(SRDAGGraph* topDag){
	for(int i=0; i<topDag->getNbVertices(); i++){
		SRDAGVertex* rb = topDag->getVertex(i);
		if(rb->getType() == RoundBuffer && rb->getState() != SrVxStDeleted){
			SRDAGVertex* explode = rb->getOutputEdge(0)->getSink();
			if(explode->getType() == Explode && rb->getState() != SrVxStDeleted){
				UINT32 rbCons = rb->getInputEdge(0)->getTokenRate();
				BOOL ok = TRUE;

				// Check prod/cons
				for(int j=0; j<explode->getNbOutputEdge(); j++){
					ok &= explode->getOutputEdge(j)->getTokenRate() == rbCons;
				}

				if(ok){
					rb->removeOutputEdgeIx(0);

					SRDAGVertex* broadcast = rb;
					broadcast->setType(Broadcast);
					broadcast->setFunctIx(BROADCAST_FUNCT_IX);

					int nbExplodeEdge = explode->getNbOutputEdge();

					for(int j=0; j<nbExplodeEdge; j++){
						SRDAGEdge* edge = explode->getOutputEdge(j);

						explode->removeOutputEdgeIx(j);
						edge->setSource(broadcast);
						broadcast->setOutputEdge(edge,j);
					}

					topDag->removeVx(explode);
					return 1;
				}
			}
		}
	}
	return 0;
}

static int removeImpRB(SRDAGGraph* topDag){
	for(int i=0; i<topDag->getNbVertices(); i++){
		SRDAGVertex* implode = topDag->getVertex(i);
		if(implode->getType() == Implode && implode->getState() != SrVxStDeleted){
			SRDAGVertex* rb = implode->getOutputEdge(0)->getSink();
			if(rb->getType() == RoundBuffer && rb->getState() != SrVxStDeleted){
				UINT32 rbConsumption = rb->getInputEdge(0)->getTokenRate();
				UINT32 rbProduction = rb->getOutputEdge(0)->getTokenRate();
				if(rbConsumption > rbProduction){
					int ixEnd = implode->getNbInputEdge();
					int cons=0;
					while(cons<rbProduction){
						cons += implode->getInputEdge(--ixEnd)->getTokenRate();
					}

					int lastEdgeIx = implode->getNbInputEdge()-1;
					for(int j=0; j<ixEnd; j++){
						SRDAGVertex *end_vertex = topDag->addVertex();
						end_vertex->setType(End);
						end_vertex->setFunctIx(END_FUNCT_IX);

						end_vertex->setInputEdge(implode->getInputEdge(j), 0);
						implode->getInputEdge(j)->setSink(end_vertex);

						implode->removeInputEdgeIx(j);

						if(j+ixEnd <= lastEdgeIx){
							implode->setInputEdge(implode->getInputEdge(j+ixEnd),j);
							implode->removeInputEdgeIx(j+ixEnd);
						}
					}
					implode->getOutputEdge(0)->setTokenRate(rbProduction);

					if(implode->getNbInputEdge() == 1){
						SRDAGEdge* edge = implode->getInputEdge(0);
						implode->removeInputEdgeIx(0);
						rb->removeInputEdgeIx(0);

						edge->setSink(rb);
						rb->setInputEdge(edge, 0);

						topDag->removeVx(implode);
					}

					if(rb->getInputEdge(0)->getTokenRate()
						== rb->getOutputEdge(0)->getTokenRate()){

						SRDAGEdge* edge = rb->getInputEdge(0);
						SRDAGVertex* nextVertex = rb->getOutputEdge(0)->getSink();
						UINT32 edgeIx = nextVertex->getInputEdgeId(rb->getOutputEdge(0));

						rb->removeInputEdgeIx(0);
						nextVertex->removeInputEdgeIx(edgeIx);

						edge->setSink(nextVertex);
						nextVertex->setInputEdge(edge, edgeIx);

						topDag->removeVx(rb);
					}

					return 1;
				}
			}
		}
	}
	return 0;
}

static int reduceExplExpl(SRDAGGraph* topDag){
	for(int i=0; i<topDag->getNbVertices(); i++){
		SRDAGVertex* explode0 = topDag->getVertex(i);
		if(explode0->getType() == Explode && explode0->getState() != SrVxStDeleted){
			for(int j=0; j<explode0->getNbOutputEdge(); j++){
				SRDAGVertex* explode1 = explode0->getOutputEdge(j)->getSink();
				if(explode1->getType() == Explode && explode1->getState() != SrVxStDeleted){

					int nbToAdd = explode1->getNbOutputEdge();
					int ixToAdd = explode0->getOutputEdgeId(explode1->getInputEdge(0));

					int nbTotEdges = explode0->getNbOutputEdge()+nbToAdd-1;

					// Shift edges after
					for(int k=nbTotEdges-1; k>ixToAdd+nbToAdd-1; k--){
						SRDAGEdge *edge = explode0->getOutputEdge(k-nbToAdd+1);
						explode0->removeOutputEdge(edge);
						explode0->setOutputEdge(edge, k);
					}
					// Add edges
					explode0->removeOutputEdge(explode0->getOutputEdge(ixToAdd));
					for(int k=0; k<nbToAdd; k++){
						SRDAGEdge *edge = explode1->getOutputEdge(k);
						explode0->setOutputEdge(edge, k+ixToAdd);
						edge->setSource(explode0);

						explode1->removeOutputEdge(edge);
					}

					explode1->setState(SrVxStDeleted);
					return 1;
				}
			}
		}
	}
	return 0;
}

static int reduceImplExpl(SRDAGGraph* topDag){
	for(int i=0; i<topDag->getNbVertices(); i++){
		SRDAGVertex* implode = topDag->getVertex(i);
		if(implode->getType() == Implode && implode->getState() != SrVxStDeleted){
			for(int j=0; j<implode->getNbOutputEdge(); j++){
				SRDAGVertex* explode = implode->getOutputEdge(j)->getSink();
				if(explode->getType() == Explode && explode->getState() != SrVxStDeleted){
					// if equal tokenRate ?
					if(implode->getInputEdge(0)->getTokenRate() > explode->getOutputEdge(0)->getTokenRate()
							&& explode->getNbOutputEdge() == 2
							&& implode->getNbInputEdge() == 2){
						SRDAGVertex* source = implode->getInputEdge(0)->getSource();
						SRDAGVertex* sink = explode->getOutputEdge(0)->getSink();
						int srcIx = source->getOutputEdgeId(implode->getInputEdge(0));
						int snkIx = sink->getInputEdgeId(explode->getOutputEdge(0));

						SRDAGEdge *imp_edge = implode->getInputEdge(0);
						SRDAGEdge *middle_edge = implode->getOutputEdge(0);
						SRDAGEdge *exp_edge = explode->getOutputEdge(0);
						SRDAGEdge *new_edge = explode->getOutputEdge(1);

						SRDAGVertex *implode2 = explode->getOutputEdge(1)->getSink();

//						SRDAGVertex* new_explode = topDag->addVertex();
//						new_explode->setType(Explode);
//						new_explode->setExpImpId(topDag->getNbEdges());
//						new_explode->setFunctIx(XPLODE_FUNCT_IX);
//						new_explode->setReference(source->getReference());
//						new_explode->setReferenceIndex(source->getReferenceIndex());
//						new_explode->setIterationIndex(source->getIterationIndex());

						// Remove
						explode->removeInputEdge(middle_edge);
						implode2->removeInputEdge(new_edge);
						implode->removeInputEdge(imp_edge);
						explode->removeOutputEdge(exp_edge);
						explode->removeOutputEdge(new_edge);

						exp_edge->setSource(explode);
						explode->setOutputEdge(exp_edge,0);

						imp_edge->setSink(explode);
						explode->setInputEdge(imp_edge,0);

						new_edge->setSource(explode);
						explode->setOutputEdge(new_edge,1);

						new_edge->setSink(implode);
						implode->setInputEdge(new_edge,0);
						new_edge->setTokenRate(imp_edge->getTokenRate()-exp_edge->getTokenRate());

						middle_edge->setSink(implode2);
						implode2->setInputEdge(middle_edge,0);
						middle_edge->setTokenRate(middle_edge->getTokenRate()-exp_edge->getTokenRate());

//						explode->setState(SrVxStDeleted);
						return 1;
					}
					if(implode->getInputEdge(1)->getTokenRate() > explode->getOutputEdge(1)->getTokenRate()
												&& explode->getNbOutputEdge() == 2
												&& implode->getNbInputEdge() == 2){
						SRDAGVertex* source = implode->getInputEdge(1)->getSource();
						SRDAGVertex* sink = explode->getOutputEdge(1)->getSink();
						int srcIx = source->getOutputEdgeId(implode->getInputEdge(1));
						int snkIx = sink->getInputEdgeId(explode->getOutputEdge(1));

						SRDAGEdge *imp_edge = implode->getInputEdge(1);
						SRDAGEdge *middle_edge = implode->getOutputEdge(0);
						SRDAGEdge *exp_edge = explode->getOutputEdge(1);
						SRDAGEdge *new_edge = explode->getOutputEdge(0);

						SRDAGVertex *implode2 = explode->getOutputEdge(0)->getSink();
						int implode2Ix = implode2->getInputEdgeId(new_edge);

//						SRDAGVertex* new_explode = topDag->addVertex();
//						new_explode->setType(Explode);
//						new_explode->setExpImpId(topDag->getNbEdges());
//						new_explode->setFunctIx(XPLODE_FUNCT_IX);
//						new_explode->setReference(source->getReference());
//						new_explode->setReferenceIndex(source->getReferenceIndex());
//						new_explode->setIterationIndex(source->getIterationIndex());

						// Remove
						explode->removeInputEdge(middle_edge);
						implode2->removeInputEdge(new_edge);
						implode->removeInputEdge(imp_edge);
						explode->removeOutputEdge(exp_edge);
						explode->removeOutputEdge(new_edge);

						exp_edge->setSource(explode);
						explode->setOutputEdge(exp_edge,1);

						imp_edge->setSink(explode);
						explode->setInputEdge(imp_edge,0);

						new_edge->setSource(explode);
						explode->setOutputEdge(new_edge,0);

						new_edge->setSink(implode);
						implode->setInputEdge(new_edge,1);
						new_edge->setTokenRate(imp_edge->getTokenRate()-exp_edge->getTokenRate());

						middle_edge->setSink(implode2);
						implode2->setInputEdge(middle_edge,implode2Ix);
						middle_edge->setTokenRate(middle_edge->getTokenRate()-exp_edge->getTokenRate());

//						explode->setState(SrVxStDeleted);
						return 1;
					}
					if(implode->getInputEdge(0)->getTokenRate() == explode->getOutputEdge(0)->getTokenRate()
												&& explode->getNbOutputEdge() == 2
												&& implode->getNbInputEdge() == 2){
						SRDAGVertex* source0 = implode->getInputEdge(0)->getSource();
						SRDAGVertex* sink0 = explode->getOutputEdge(0)->getSink();
						SRDAGVertex* source1 = implode->getInputEdge(1)->getSource();
						SRDAGVertex* sink1 = explode->getOutputEdge(1)->getSink();
						int snkIx0 = sink0->getInputEdgeId(explode->getOutputEdge(0));
						int snkIx1 = sink1->getInputEdgeId(explode->getOutputEdge(1));

						SRDAGEdge *edge0 = implode->getInputEdge(0);
						SRDAGEdge *edge1 = implode->getInputEdge(1);
						SRDAGEdge *del_edge0 = explode->getOutputEdge(0);
						SRDAGEdge *del_edge1 = explode->getOutputEdge(1);
						SRDAGEdge *del_middle = explode->getInputEdge(0);

						// Remove
						explode->setState(SrVxStDeleted);
						implode->setState(SrVxStDeleted);

						implode->removeInputEdge(edge0);
						implode->removeInputEdge(edge1);

						sink0->removeInputEdge(del_edge0);
						sink1->removeInputEdge(del_edge1);

						edge0->setSink(sink0);
						sink0->setInputEdge(edge0,snkIx0);

						edge1->setSink(sink1);
						sink1->setInputEdge(edge1,snkIx1);

						return 1;
					}
				}
			}
		}
	}
	return 0;
}

void PiSDFTransformer::multiStepScheduling(
							Architecture* arch,
							PiSDFGraph* pisdf,
							ListScheduler* listScheduler,
//							Scenario* scenario,
							SRDAGGraph* topDag,
							ExecutionStat* execStat){
	static int brv[MAX_SRDAG_VERTICES];
	PiSDFGraph*   currentPiSDF;
	UINT32 len;
	UINT8 	stepsCntr = 0;

	Queue<PiSDFGraph*, 15> graphFifo;
	Queue<SRDAGVertex*, 15> vertexFifo;

	schedule.reset();
	graphFifo.reset();
	schedule.setNbActiveSlaves(arch->getNbActiveSlaves());

	PiSDFVertex* root = (PiSDFVertex*)pisdf->getRootVertex();
	if(!root) exitWithCode(1070);
	if(!root->hasSubGraph()) exitWithCode(1069);

	SRDAGVertex* currHSrDagVx;
	topDag->getVerticesFromReference(root,0,&currHSrDagVx);

	platform_time_reset();
	Launcher::initGraphTime();

	root->getSubGraph(&currentPiSDF);

	do{
		do{
		#if PRINT_GRAPH
			// Printing the current PiSDF graph.
			len = snprintf(file, MAX_FILE_NAME_SIZE, "%s_%d.gv", PiSDF_FILE_PATH, stepsCntr);
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
				currentPiSDF->evaluateExpressions(/*scenario*/);

				/* Replace hierarchical actor in topDag with RBs */
				PiSDFTransformer::replaceHwithRB(currentPiSDF, topDag, currHSrDagVx);

				/* Put CA in topDag with RB between CA and /CA */
				PiSDFTransformer::addCAtoSRDAG(currentPiSDF, topDag, currHSrDagVx);

				currentPiSDF->updateDAGStates(topDag);

				graphFifo.push(currentPiSDF);
				vertexFifo.push(currHSrDagVx);
			}else{
				/* Resolve */
				currentPiSDF->evaluateExpressions(/*scenario*/);

				/* Replace hierarchical actor in topDag with RBs */
				PiSDFTransformer::replaceHwithRB(currentPiSDF, topDag, currHSrDagVx);

				PiSDFTransformer::computeBRV(currentPiSDF, brv);
				PiSDFTransformer::singleRateTransformation(currentPiSDF, currHSrDagVx, topDag, brv);
				currentPiSDF->updateDAGStates(topDag);
			}

			currHSrDagVx = topDag->getExHierVertex();
			if(currHSrDagVx)
				currentPiSDF = currHSrDagVx->getHierarchy();
		}while(currHSrDagVx); /* There is executable hierarchical actor in SRDAG */

//		PiSDFTransformer::removeUnusedRB(topDag);

		Launcher::endGraphTime();
		Launcher::initSchedulingTime();

		/* Schedule */
		listScheduler->schedule(topDag, &schedule, arch);

		// Assigning FIFO ids to executable vxs' edges.
//		Launcher::assignFifo(topDag);

	#if PRINT_GRAPH
		len = snprintf(file, MAX_FILE_NAME_SIZE, "%s_%d.xml", SCHED_FILE_NAME, stepsCntr);
		if(len > MAX_FILE_NAME_SIZE)
			exitWithCode(1072);
		ScheduleWriter::write(&schedule, topDag, arch, file);

		len = snprintf(file, MAX_FILE_NAME_SIZE, "%s_%d.gv", "topDag_mid", stepsCntr);
		if(len > MAX_FILE_NAME_SIZE)
			exitWithCode(1072);
		DotWriter::write(topDag, file, 1, 1);
	#endif

		Launcher::endSchedulingTime();


	#if EXEC == 1
		// Executing the executable vxs.
//		Launcher::launch(topDag, arch, &schedule);
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

		Launcher::initGraphTime();
		while(!graphFifo.isEmpty()){
			currentPiSDF = graphFifo.pop();
			currHSrDagVx = vertexFifo.pop();

//			#if EXEC == 1
			// Update iteration parameters' values.
			for(UINT32 i=0; i<currentPiSDF->getNb_parameters(); i++){
				PiSDFParameter *param = currentPiSDF->getParameter(i);
				UINT32 value = getValue(param, topDag, currHSrDagVx);
			#if SEE_PARAM_VAL
				printf("%s gets %d\n", param->getName(), value);
			#endif
				param->setValue(value);
			}
//			#endif
			// Resolving productions/consumptions.
			currentPiSDF->evaluateExpressions(/*scenario*/);

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

		currHSrDagVx = topDag->getExHierVertex();
		if(currHSrDagVx)
			currentPiSDF = currHSrDagVx->getHierarchy();
	}while(currHSrDagVx);

#if PRINT_GRAPH
	// Printing the topDag
	len = snprintf(file, MAX_FILE_NAME_SIZE, "%s.gv", "topDag_last_bo");
	if(len > MAX_FILE_NAME_SIZE)
		exitWithCode(1072);
	DotWriter::write(topDag, file, 1, 1);
#endif

	while(removeImpExp(topDag));
	while(reduceImplImpl(topDag));
	while(removeImpRB(topDag));
	while(removeRBExp(topDag));
	while(removeBr(topDag));

#if PRINT_GRAPH
	// Printing the topDag
	len = snprintf(file, MAX_FILE_NAME_SIZE, "%s.gv", "topDag_last_ao");
	if(len > MAX_FILE_NAME_SIZE)
		exitWithCode(1072);
	DotWriter::write(topDag, file, 1, 1);
#endif

//	while(reduceExplExpl(topDag));
	currentPiSDF->updateDAGStates(topDag);

//	len = snprintf(file, MAX_FILE_NAME_SIZE, "%s_%d.gv", "topDag_afterRed", stepsCntr);
//	if(len > MAX_FILE_NAME_SIZE)
//		exitWithCode(1072);
//	DotWriter::write(topDag, file, 1, 1);
//
//	len = snprintf(file, MAX_FILE_NAME_SIZE, "%s_%d.gv", "topDag_afterRed_fifoId", stepsCntr);
//	if(len > MAX_FILE_NAME_SIZE)
//		exitWithCode(1072);
//	DotWriter::write(topDag, file, 1, 0);

	Launcher::endGraphTime();
	Launcher::initSchedulingTime();
	/*
	 * Last scheduling and execution. After all hierarchical levels have been flattened,
	 * there is one more execution to do for completing one complete execution of the model.
	 */
	schedule.newStep();
	listScheduler->schedule(topDag, &schedule, arch);

	// Assigning FIFO ids to executable vxs' edges.
//	Launcher::assignFifo(topDag);

	Launcher::endSchedulingTime();

#if EXEC == 1
	/*
	 * Launching the execution on LRTs. The "true" means that is the last execution
	 * of the current iteration, so the local RTs clear the tasks table and
	 * send back execution information.
	 */
//	Launcher::launch(topDag, arch, &schedule);
	Launcher::createRealTimeGantt(arch, topDag, "Gantt.xml", execStat);

#if PRINT_REAL_GANTT
	len = snprintf(file, MAX_FILE_NAME_SIZE, "Gantt_simulated.xml");
	if(len > MAX_FILE_NAME_SIZE)
		exitWithCode(1072);
	ScheduleWriter::write(&schedule, topDag, arch, file);
#endif
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
