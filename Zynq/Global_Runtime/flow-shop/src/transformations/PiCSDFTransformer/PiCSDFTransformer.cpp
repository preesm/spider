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

	// Filling the topology matrix. See Max's thesis chapter 3.
	for(int i = 0; i < nb_rows; i++)
	{
		PiCSDFEdge *edge = inputGraph->getEdge(i);
		int sourceVertexIndex = inputGraph->getVertexIndex(edge->getSource());
		int sinkVertexIndex = inputGraph->getVertexIndex(edge->getSink());

		// Resolving the production/consumption expressions.
		globalParser.interpret(edge->getProduction(), &topo_matrix[i * nb_cols + sourceVertexIndex]);
		globalParser.interpret(edge->getConsumption(), &topo_matrix[i * nb_cols + sinkVertexIndex]);
	}

	// Computing the null space (BRV) of the matrix.
	if(nullspace(nb_rows, nb_cols, topo_matrix, brv) == 0)
	{
		for (int j = 0; j < nb_cols; j++) {
			addVertices(inputGraph, inputGraph->getVertex(j), brv[j], outputGraph);
		}
	}


//	for(i=0;i<input->getNbEdges();i++){
//		PiCSDFEdge *edge = input->getEdge(i);
//		int sourceVertexIndex = input->getVertexIndex(edge->getSource());
//		int sinkVertexIndex = input->getVertexIndex(edge->getSink());
//		if((brv[sourceVertexIndex] != 0) &&
//		   (brv[sinkVertexIndex] != 0)){
//			// Retrieving the different repetitions of source in the correct order
//			output->getVerticesFromCSDAGReference(input->getVertex(sourceVertexIndex),sourceRepetitions);
//			// The sink repetitions are already in the array sinkRepetitions
//
//			int initial_tokens = edge->getInitialTokens();
//
//			//
//			int* sourcePattern = resolvedInputEdgesPatterns[0][i];
//			int sourcePatternSize = brv[sourceVertexIndex];
//			int* sinkPattern = resolvedInputEdgesPatterns[1][i];
//			int sinkPatternSize = brv[sinkVertexIndex];
//		}
//	}
}

void PiCSDFTransformer::linkvertices(PiCSDFGraph* inputGraph, CSDAGVertex* sinkVertex)
{
	// Getting sinkVertex input edges
	this->nb_input_edges = inputGraph->getInputEdges(sinkVertex,input_edges);
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
