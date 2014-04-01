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

void PiSDFTransformer::replaceHwithRB(SRDAGGraph* topDag, SRDAGVertex* H, PiSDFGraph* currentPiSDF){

}
