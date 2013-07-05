/*
 * test_graph_trans.cpp
 *
 *  Created on: 2 juil. 2013
 *      Author: yoliva
 */

#include "graphs/PiCSDF/PiCSDFGraph.h"
#include "graphs/PiCSDF/PiCSDFEdge.h"

#include "tools/DotWriter.h"

#include "transformations/PiCSDFTransformer/PiCSDFTransformer.h"

#include "applicationGraphs/DPCM.h"

#define NB_EDGES					4
#define NB_VERTICES					4

#define PRINT_GRAPH					1
#define CSDAG_FILE_PATH 			"csdag.gv"
#define SRDAG_FILE_PATH 			"srdag.gv"


void create_graph_1(PiCSDFGraph* graph){
	CSDAGVertex *vertexA = graph->addVertex("A");
	CSDAGVertex *vertexB = graph->addVertex("B");

	graph->addEdge(vertexA, "3", vertexB, "2", "0");
}


void create_graph_2(PiCSDFGraph* graph){
	CSDAGVertex *vertexA = graph->addVertex("A");
	CSDAGVertex *vertexB = graph->addVertex("B");
	CSDAGVertex *vertexC = graph->addVertex("C");
	CSDAGVertex *vertexD = graph->addVertex("D");

	graph->addEdge(vertexA, "3", vertexB, "2", "1");
	graph->addEdge(vertexA, "3", vertexC, "2", "0");
	graph->addEdge(vertexB, "2", vertexD, "4", "0");
	graph->addEdge(vertexC, "2", vertexD, "4", "0");
}


int main(int nArgs, char** pArgs)
{
	DotWriter 			dotWriter;
	PiCSDFTransformer	trans;

	// Creating PiSDF graph.
	PiCSDFGraph inputGraph;

//	create_graph_1(&inputGraph);
//	create_graph_2(&inputGraph);
	createPiCSDF_DPCM_1(&inputGraph);

	// Printing the PiSDF graph.
#if PRINT_GRAPH
	dotWriter.write((PiCSDFGraph*)&inputGraph, CSDAG_FILE_PATH, 1);
#endif

	// Transforming PiSDF to SrDAG if schedulable.
//	SRDAGGraph outputGraph;
//	trans.transform(&inputGraph, &outputGraph, (Architecture*)0);


	// Printing the SrDAG graph.
#if PRINT_GRAPH
//	dotWriter.write((SRDAGGraph*)&outputGraph, SRDAG_FILE_PATH, 1);
#endif
}
