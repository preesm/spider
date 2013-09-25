/*
 * PiCSDFTransformer.cpp
 *
 *  Created on: 12 juin 2013
 *      Author: yoliva
 */

#include "MatrixHandler.h"
#include "PiCSDFTransformer.h"


/**
 * Adds "nb_repetitions" SRDAGVertex vertices for the "csdag_vertex" vertex to the output SRDAG graph.
 */
void PiCSDFTransformer::addVertices(PiCSDFGraph* inputGraph, CSDAGVertex* csdag_vertex, int nb_repetitions, SRDAGGraph* outputGraph){
	// Adding one SRDAG vertex per repetition
	for(int j = 0; j < nb_repetitions; j++){
		SRDAGVertex* srdag_vertex = outputGraph->addVertex();

		// Setting attributes from original vertex.
		srdag_vertex->setCsDagReference(csdag_vertex);
		srdag_vertex->setReferenceIndex(j);
		for(int i = 0; i < csdag_vertex->getParamNb(); i++){
			srdag_vertex->setParamValue(i, csdag_vertex->getParamValue(j,i));
		}

	}
}



void PiCSDFTransformer::transform(PiCSDFGraph* inputGraph, SRDAGGraph* outputGraph, Architecture* archi)
{

	// The topology matrix initialized to 0. Edges x Vertices.
	int topo_matrix [MAX_CSDAG_EDGES * MAX_CSDAG_VERTICES];
	memset(topo_matrix, 0, sizeof(topo_matrix));

	int nb_rows = inputGraph->getNbEdges();
	int nb_cols = inputGraph->getNbVertices();

	// Filling the topology matrix. See Max's thesis chapter 3.
	for(int i = 0; i < nb_rows; i++)
	{
		PiCSDFEdge *edge = inputGraph->getEdge(i);
		int sourceVertexIndex = inputGraph->getVertexIndex(edge->getSource());
		int sinkVertexIndex = inputGraph->getVertexIndex(edge->getSink());

		int nb_tokens;

		// Resolving the production expression.
		globalParser.interpret(edge->getProduction(), &nb_tokens);
		topo_matrix[i * nb_cols + sourceVertexIndex] = nb_tokens;
		edge->setProductionInt(nb_tokens);

		// Resolving the consumption expression.
		globalParser.interpret(edge->getConsumption(), &nb_tokens);
		topo_matrix[i * nb_cols + sinkVertexIndex] = -nb_tokens;
		edge->setConsumptionInt(nb_tokens);
	}

	// Computing the null space (BRV) of the matrix.
	// TODO: It may be improved by another algorithm to compute the null space.
	if(nullspace(nb_rows, nb_cols, topo_matrix, brv) == 0)
	{
		for (int j = 0; j < nb_cols; j++) {
			// Setting the number of repetitions of the current CSDAG Vertex.
			inputGraph->getVertex(j)->setRepetitionNb(brv[j]);

			// Creating the vertices of the SRDAG output graph for each CSDAG vertex.
			addVertices(inputGraph, inputGraph->getVertex(j), brv[j], outputGraph);
		}

		// Connecting the vertices of the SrDAG ouput graph.
		linkvertices(inputGraph, outputGraph);
//		// Creating the edges of the SRDAG output graph.
//		for(int i = 0; i < nb_rows; i++){
//			PiCSDFEdge *edge = inputGraph->getEdge(i);
//			createEdges(inputGraph, inputGraph->getVertexIndex(edge->getSource()), i,
//						inputGraph->getVertexIndex(edge->getSink()), outputGraph);
//		}
	}
}


void PiCSDFTransformer::linkvertices(PiCSDFGraph* inputGraph, SRDAGGraph* outputGraph)
{
	for (int i = 0; i < inputGraph->getNbEdges(); i++) {
		PiCSDFEdge *edge = inputGraph->getEdge(i);

		// Resolving the delay expression.
		int nb_tokens;
		globalParser.interpret(edge->getDelay(), &nb_tokens);
		edge->setDelayInt(nb_tokens);

		int nbDelays = edge->getDelayInt();

		int nbSourceRepetitions = edge->getSource()->getRepetitionNb();
		int nbTargetRepetitions = edge->getSink()->getRepetitionNb();

		// Getting the replicas of the source vertex into sourceRepetitions.
		outputGraph->getVerticesFromCSDAGReference(edge->getSource(), sourceRepetitions);

		// Getting the replicas of the sink vertex into sinkRepetitions.
		outputGraph->getVerticesFromCSDAGReference(edge->getSink(), sinkRepetitions);

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

		// Iterating while the total of token is less than until all consumptions are "satisfied".
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

			if (rest < edge->getProductionInt() &&
				(sourceRepetitions[sourceIndex]->getType() == 0)){ // Type == 0 indicates it is a normal vertex.

				// Adding an explode vertex.
				SRDAGVertex *exp_vertex = outputGraph->addVertex();
				exp_vertex->setType(1); 	// Indicates it is an explode vertex.
				exp_vertex->setExpImpId(i); // Distinction among explode vertices for the same SRDAGVertex.

				// Replacing the source vertex by the explode vertex in the array of sources.
				SRDAGVertex *origin_vertex = sourceRepetitions[sourceIndex];
				sourceRepetitions[sourceIndex] = exp_vertex;

				// Setting attributes from original vertex.
				exp_vertex->setCsDagReference(origin_vertex->getCsDagReference());
				exp_vertex->setReferenceIndex(origin_vertex->getReferenceIndex());

				// Adding an edge between the source and the explode.
				outputGraph->addEdge(origin_vertex, edge->getProductionInt(), exp_vertex);
			}

			if (rest < edge->getConsumptionInt() &&
				(sinkRepetitions[targetIndex]->getType() == 0)){ // Type == 0 indicates it is a normal vertex.

				// Adding an implode vertex.
				SRDAGVertex *imp_vertex = outputGraph->addVertex();
				imp_vertex->setType(2); 	// Indicates it is an implode vertex.
				imp_vertex->setExpImpId(i); // Distinction among implode vertices for the same SRDAGVertex.

				// Replacing the sink vertex by the implode vertex in the array of sources.
				SRDAGVertex *origin_vertex = sinkRepetitions[targetIndex];
				sinkRepetitions[targetIndex] = imp_vertex;


				// Setting attributes from original vertex.
				imp_vertex->setCsDagReference(origin_vertex->getCsDagReference());
				imp_vertex->setReferenceIndex(origin_vertex->getReferenceIndex());

				// Adding an edge between the implode and the sink.
				outputGraph->addEdge(imp_vertex, edge->getConsumptionInt(), origin_vertex);
			}

			/*
			 * Adding the edge.
			 */
			//Creating the new edge between normal vertices or between a normal and an explode/implode one.
			SRDAGEdge* new_edge = outputGraph->addEdge(sourceRepetitions[sourceIndex], rest, sinkRepetitions[targetIndex]);


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
				int addedDelays = iterationDiff * new_edge->getTokenRate();

				// Check that there are enough delays available
				if (nbDelays < addedDelays) {
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
				}
				new_edge->setDelay(addedDelays);
				nbDelays = nbDelays - addedDelays;
			} else {
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
	// Getting sinkVertex input edges
//	this->nb_input_edges = inputGraph->getInputEdges(sinkVertex,input_edges);
}



//void PiCSDFTransformer::compute_BRV(PiCSDFGraph* inputGraph)
//{
//	// The topology matrix initialized to 0. Edges x Vertices.
//	int topo_matrix [MAX_CSDAG_EDGES][MAX_CSDAG_VERTICES];
//	memset(topo_matrix, 0, sizeof(topo_matrix));
//
//	// Filling the topology matrix. See Max's thesis chapter 3.
//	for(int i = 0; i<inputGraph->getNbEdges();i++)
//	{
//		PiCSDFEdge *edge = inputGraph->getEdge(i);
//		int sourceVertexIndex = inputGraph->getVertexIndex(edge->getSource());
//		int sinkVertexIndex = inputGraph->getVertexIndex(edge->getSink());
//
//		// Resolving the production/consumption expressions.
//		globalParser.interpret(edge->getProduction(), &topo_matrix[i][sourceVertexIndex]);
//		globalParser.interpret(edge->getConsumption(), &topo_matrix[i][sinkVertexIndex]);
//	}


//
//	// Reducing the topology matrix.
//	int j = i = 0;
//	while ((i < inputGraph->getNbEdges() - 1) && (j < inputGraph->getNbVertices() - 1))
//	{
//		int pivot_index = find_pivot(topo_matrix[i]);
//		if(topo_matrix[pivot_index][j] != 0)
//		{
//			swap_rows(topo_matrix[i], topo_matrix[pivot_index]);
//			divide_row(topo_matrix[i], topo_matrix[i][j]);
//			for (int l = i + 1; l < inputGraph->getNbEdges(); l++)
//			{
//				subst_rows(topo_matrix[l], topo_matrix[i], topo_matrix[i][j]);
//			}
//			i++;
//		}
//	}
//
//	// Building the BRV.
//	int rank = count_non_null_rows(topo_matrix);
//	if(rank == nbVertices - 1)
//	{
//		// Solving independent equations by applying sustitution.
//		for (int i = nbEdges - 2 ; i >= 0; i--)
//		{
//			for (int j = nbVertices - 1; j > i; j--)
//			{
//				if(abs(topo_matrix[i][j]) % abs(topo_matrix[i][i]) == 0)
//					pivot = abs(topo_matrix[i][j]) / abs(topo_matrix[i][i]);
//				else
//					pivot = abs(topo_matrix[i][j]) / abs(topo_matrix[i][i]);
//
//			}
//			coeff[j] = topo_matrix[i][j];
//			coeff[j - 1] =
//			if()
//			brv[i] = -();
//		}
//	}
//}
