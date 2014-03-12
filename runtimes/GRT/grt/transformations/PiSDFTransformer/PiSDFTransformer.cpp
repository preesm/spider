
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/

#include <transformations/MatrixHandler.h>
#include "PiSDFTransformer.h"

// The topology matrix initialized to 0. Edges x Vertices.
static int topo_matrix [MAX_NB_EDGES * MAX_NB_VERTICES] = {0};
static char name[MAX_VERTEX_NAME_SIZE];

void PiSDFTransformer::addVertices(BaseVertex* vertex, int nb_repetitions, SRDAGGraph* outputGraph, SRDAGVertex* hSrDagVx){
	// Adding one SRDAG vertex per repetition
	for(UINT32 j = 0; j < nb_repetitions; j++){
		SRDAGVertex* srdag_vertex = outputGraph->addVertex();

		// Setting attributes from original vertex.
		if(hSrDagVx)
			sprintf(name, "%s_%s_%d", hSrDagVx->getName(), vertex->getName(), j);
		else
			sprintf(name, "%s_%d", vertex->getName(), j);
		srdag_vertex->setName(name);
		srdag_vertex->setFunctIx(vertex->getFunction_index());
		srdag_vertex->setReference(vertex);
		srdag_vertex->setReferenceIndex(j);
		srdag_vertex->setParent(hSrDagVx);

	}
}


void PiSDFTransformer::computeBVR(SDFGraph *sdf)
{
	/*
	 * Setting temporal Ids to be used as indices in the topology matrix.
	 * Note that only normal Vxs are considered.
	 */
	UINT32 nbVertices = 0;
	for (UINT32 i = 0; i < sdf->getNbVertices(); i++) {
		BaseVertex* vertex = sdf->getVertex(i);
		if(vertex->getType() == pisdf_vertex) vertex->setTempId(nbVertices++);
	}

	UINT32 nbEdges = 0;
	memset(topo_matrix, 0, sizeof(topo_matrix));

	// Filling the topology matrix(nbEdges x nbVertices).
	for(UINT32 i = 0; i < sdf->getNbEdges(); i++){
		BaseEdge* edge = sdf->getEdge(i);
		if((edge->getSource() != edge->getSink()) &&
			(edge->getSource()->getType() ==  pisdf_vertex) &&
			(edge->getSink()->getType() ==  pisdf_vertex)){ // TODO: treat cycles.
			if((edge->getProductionInt() <= 0) || (edge->getConsumptionInt() <= 0 )) exitWithCode(1066);
			topo_matrix[nbEdges * nbVertices + edge->getSource()->getTempId()] = edge->getProductionInt();
			topo_matrix[nbEdges * nbVertices + edge->getSink()->getTempId()] =  -edge->getConsumptionInt();
			nbEdges++;
		}
	}


	if(nbEdges > 0){
		// Computing the null space (BRV) of the matrix.
		// TODO: It may be improved by another algorithm to compute the null space (I'm not very sure of this one...)
		if(nullspace(nbEdges, nbVertices, (int*)topo_matrix, brv) == 0)
		{
			// Setting the number of repetitions for each vertex.
			nbVertices = 0;
			for (UINT32 i = 0; i < sdf->getNbVertices(); i++) {
				BaseVertex* vertex = sdf->getVertex(i);
				if(vertex->getType() == pisdf_vertex) vertex->setNbRepetition(brv[nbVertices++]);
			}
		}
	}
}


void PiSDFTransformer::linkvertices(SDFGraph* sdf, SRDAGGraph* outputGraph, SRDAGVertex* hSrDagVx)
{
	UINT32 cntExpVxs = 0;
	UINT32 cntImpVxs = 0;

	for (UINT32 i = 0; i < sdf->getNbEdges(); i++) {
		BaseEdge *edge = sdf->getEdge(i);

		int nbDelays = edge->getDelayInt();

		int nbSourceRepetitions = edge->getSource()->getNbRepetition();
		int nbTargetRepetitions = edge->getSink()->getNbRepetition();

		// Getting the replicas of the source vertex into sourceRepetitions.
		outputGraph->getVerticesFromReference(edge->getSource(), sourceRepetitions);

		// Getting the replicas of the sink vertex into sinkRepetitions.
		outputGraph->getVerticesFromReference(edge->getSink(), sinkRepetitions);

		// Total number of token exchanged (produced and consumed) for an edge.
		int totalNbTokens = edge->getProductionInt() * nbSourceRepetitions;

		// Absolute target is the targeted consumed token among the total number of consumed/produced tokens.
		int absoluteSource = 0;
//		int producedTokens = 0;
		int absoluteTarget = nbDelays;
//		int availableTokens = nbDelays;

		// totProd is updated to store the number of token consumed by the
		// targets that are "satisfied" by the added edges.
		int totProd = 0;

		// Iterating until all consumptions are "satisfied".
		while (totProd < totalNbTokens) {
			/*
			 * Computing the indexes and rates.
			 */
			// Index of the source vertex's instance (among all the replicas).
			int sourceIndex = (absoluteSource / edge->getProductionInt())% nbSourceRepetitions;

			// Index of the target vertex's instance (among all the replicas).
			int targetIndex = (absoluteTarget / edge->getConsumptionInt())% nbTargetRepetitions;

			// Number of token already produced/consumed by the current source/target.
			int sourceProd = absoluteSource % edge->getProductionInt();
			int targetCons = absoluteTarget % edge->getConsumptionInt();

			// Production/consumption rate for the current source/target.
			int rest =((edge->getProductionInt() - sourceProd) < (edge->getConsumptionInt() - targetCons))?
					(edge->getProductionInt() - sourceProd):(edge->getConsumptionInt() - targetCons); // Minimum.


			/*
			 * Adding explode/implode vertices if required.
			 */

			if (rest < (int)edge->getProductionInt() &&
				(sourceRepetitions[sourceIndex]->getType() == 0)){ // Type == 0 indicates it is a normal SR vx.

				// Adding an explode vertex.
				SRDAGVertex *exp_vertex = outputGraph->addVertex();
				exp_vertex->setType(1); 			// Indicates it is an explode vx.
//				exp_vertex->setExpImpId(nbExpVxs++);
				exp_vertex->setExpImpId(i);

				// Replacing the source vertex by the explode vertex in the array of sources.
				SRDAGVertex *origin_vertex = sourceRepetitions[sourceIndex];
				sourceRepetitions[sourceIndex] = exp_vertex;

				// Setting attributes from original vertex.
				sprintf(name, "Exp%d_%s", cntExpVxs, origin_vertex->getName());
				exp_vertex->setName(name);
				exp_vertex->setFunctIx(XPLODE_FUNCT_IX);
				exp_vertex->setReference(origin_vertex->getReference());
				exp_vertex->setReferenceIndex(origin_vertex->getReferenceIndex());
				cntExpVxs++;

				// Adding an edge between the source and the explode.
				outputGraph->addEdge(origin_vertex, edge->getProductionInt(), exp_vertex, edge->getRefEdge());
			}

			if (rest < (int)edge->getConsumptionInt() &&
				(sinkRepetitions[targetIndex]->getType() == 0)){ // Type == 0 indicates it is a normal vertex.

				// Adding an implode vertex.
				SRDAGVertex *imp_vertex = outputGraph->addVertex();
				imp_vertex->setType(2); 	// Indicates it is an implode vertex.
				imp_vertex->setExpImpId(i); // Distinction among implode vertices for the same SRDAGVertex.

				// Replacing the sink vertex by the implode vertex in the array of sources.
				SRDAGVertex *origin_vertex = sinkRepetitions[targetIndex];
				sinkRepetitions[targetIndex] = imp_vertex;


				// Setting attributes from original vertex.
				sprintf(name, "Imp%d_%s", cntImpVxs, origin_vertex->getName());
				imp_vertex->setName(name);
				imp_vertex->setFunctIx(XPLODE_FUNCT_IX);
				imp_vertex->setReference(origin_vertex->getReference());
				imp_vertex->setReferenceIndex(origin_vertex->getReferenceIndex());
				cntImpVxs++;

				// Adding an edge between the implode and the sink.
				outputGraph->addEdge(imp_vertex, edge->getConsumptionInt(), origin_vertex, edge->getRefEdge());
			}



			/**************************
			 * Adding the edge.
			 **************************/

			// The delay.
			// This int represent the number of iteration separating the
			// currently indexed source and target (between which an edge is
			// added)
			// If this int is > to 0, this means that the added edge must
			// have
			// delays (with delay=prod=cons of the added edge).
			// Warning, this integer division is not factorable
			int iterationDiff = absoluteTarget / totalNbTokens - absoluteSource / totalNbTokens;

			// If the edge has a delay and that delay still exist in the
			// SRSDF (i.e. if the source & target do not belong to the same
			// "iteration")
			if (iterationDiff > 0) {
				// TODO: Treating delays
//					int addedDelays = iterationDiff * new_edge->getTokenRate();

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
				SRDAGEdge* new_edge = outputGraph->addEdge(sourceRepetitions[sourceIndex], rest, sinkRepetitions[targetIndex], edge->getRefEdge());
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

void PiSDFTransformer::transform(SDFGraph* sdf, SRDAGGraph *srGraph, SRDAGVertex* currHSrVx)
{
	if(sdf->getNbVertices() > 0){
		for (UINT32 i = 0; i < sdf->getNbVertices(); i++) {
			BaseVertex* vertex = sdf->getVertex(i);
//				if(vertex->getType() != roundBuff_vertex) vertex->setNbRepetition(brv[nbVertices++]);

			// Creating the new vertices.
			addVertices(vertex, vertex->getNbRepetition(), srGraph, currHSrVx);
		}

		// Connecting the vertices of the SrDAG ouput graph.
		linkvertices(sdf, srGraph, currHSrVx);
	}
}
