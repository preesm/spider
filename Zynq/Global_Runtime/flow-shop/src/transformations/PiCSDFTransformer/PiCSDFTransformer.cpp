/*
 * PiCSDFTransformer.cpp
 *
 *  Created on: 12 juin 2013
 *      Author: yoliva
 */

#include "MatrixHandler.h"
#include "PiCSDFTransformer.h"

void PiCSDFTransformer::transform(PiCSDFGraph* inputGraph, SRDAGGraph* outputGraph, Architecture* archi)
{

	// The topology matrix initialized to 0. Edges x Vertices.
	int topo_matrix [MAX_CSDAG_EDGES * MAX_CSDAG_VERTICES];
	memset(topo_matrix, 0, sizeof(topo_matrix));

	int nb_rows = inputGraph->getNbEdges();
	int nb_cols = inputGraph->getNbVertices();

	PiCSDFEdge *edge;

	int prod, cons;

	// Filling the topology matrix. See Max's thesis chapter 3.
	for(int i = 0; i < nb_rows; i++)
	{
		edge = inputGraph->getEdge(i);
		int sourceVertexIndex = inputGraph->getVertexIndex(edge->getSource());
		int sinkVertexIndex = inputGraph->getVertexIndex(edge->getSink());

		// Resolving the production/consumption expressions.
		globalParser.interpret(edge->getProduction(), &prod);
		globalParser.interpret(edge->getConsumption(), &cons);

		topo_matrix[i * nb_cols + sourceVertexIndex] = prod;
		topo_matrix[i * nb_cols + sinkVertexIndex] = cons;

		edge->setProductionInt(prod);
		edge->setConsumptionInt(cons);
	}

	// Computing the null space (BRV) of the matrix.
	if(nullspace(nb_rows, nb_cols, topo_matrix, brv) == 0)
	{

		for (int j = 0; j < nb_cols; j++) {
			// Setting the number of repetitions of the current CSDAG Vertex.
			inputGraph->getVertex(j)->setRepetitionNb(brv[j]);

			// Creating the vertices of the SRDAG output graph for each CSDAG vertex.
			addVertices(inputGraph, inputGraph->getVertex(j), brv[j], outputGraph);
		}

		// Creating the edges of the SRDAG output graph.
		for(int i = 0; i < nb_rows; i++){
			edge = inputGraph->getEdge(i);
			createEdges(inputGraph, inputGraph->getVertexIndex(edge->getSource()), i,
						inputGraph->getVertexIndex(edge->getSink()), outputGraph);
		}
	}
}
//
//void PiCSDFTransformer::linkvertices(PiCSDFGraph* inputGraph, CSDAGVertex* sinkVertex, PiCSDFGraph* inputGraph, SRDAGGraph* outputGraph)
//{
//	for (int i = 0; i < inputGraph->getNbEdges(); i++) {
//		PiCSDFEdge *edge = outputGraph->getEdge(i);
//
//		int nbDelays = edge->getDelay();
//
//		int nbSourceRepetitions = edge->getSource()->getRepetitionNb();
//		int nbTargetRepetitions = edge->getSink()->getRepetitionNb();
//
//		// Total number of token exchanged (produced and consumed) for an edge.
//		int totalNbTokens = edge->getProductionInt() * nbSourceRepetitions;
//
//		// Absolute target is the targeted consumed token among the total number of consumed/produced tokens.
//		int absoluteTarget = nbDelays;
//		int absoluteSource = 0;
//
//		// totProd is updated to store the number of token consumed by the
//		// targets that are "satisfied" by the added edges.
//		int totProd = 0;
//
//		// Iterating until all consumptions are "satisfied".
//		while (totProd < (edge->getProductionInt() * nbSourceRepetitions)) {
//			// Index of the source vertex's instance (among all the replicas).
//			int sourceIndex = (absoluteSource / edge->getProductionInt())% nbSourceRepetitions;
//
//			// Index of the target vertex's instance to be connected.
//			int targetIndex = (absoluteTarget / edge->getConsumptionInt())% nbTargetRepetitions;
//
//			// Number of token already produced/consumed through the underlying connection (source->target).
//			int sourceProd = absoluteSource % edge->getProductionInt();
//			int targetCons = absoluteTarget % edge->getConsumptionInt();
//
//			// Production and consumption rate on the underlying connection.
//			int rest =((edge->getConsumptionInt() - sourceProd) > (edge->getConsumptionInt() - targetCons))?
//					(edge->getConsumptionInt() - targetCons):(edge->getConsumptionInt() - sourceProd); // Minimum.
//
//			/*
//			 * iterationDiff represents the number of iteration separating the current couple (source->target).
//			 * If it is > 0, the corresponding edge must have delays (with delay = prod = cons).
//			 * With the previous example:
//			 * 		A_1 will target B_(1+targetIndex%3) = B_0 (with a delay of 1)
//			 * 		A_2 will target B_(2+targetIndex%3) = B_1 (with a delay of 1)
//			 * Warning, this integer division is not factorable.
//			 */
//			int iterationDiff = absoluteTarget / totalNbTokens - absoluteSource / totalNbTokens;
//		}
//	}
//	// Getting sinkVertex input edges
//	this->nb_input_edges = inputGraph->getInputEdges(sinkVertex,input_edges);
//}



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
