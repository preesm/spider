/*
 * test_graph_trans.cpp
 *
 *  Created on: 2 juil. 2013
 *      Author: yoliva
 */

#include "graphs/PiCSDF/PiCSDFGraph.h"
#include "graphs/PiCSDF/PiCSDFEdge.h"

#include "tools/DotWriter.h"




#define NB_EDGES					4
#define NB_VERTICES					4

#define PRINT_GRAPH					1
#define CSDAG_FILE_PATH 			"csdag.gv"





int main(int nArgs, char** pArgs)
{
	DotWriter 		dotWriter;

	// Creating PiSDF graph.
	PiCSDFGraph inputGraph;
	CSDAGVertex *vertexA = inputGraph.addVertex("A");
	CSDAGVertex *vertexB = inputGraph.addVertex("B");
	CSDAGVertex *vertexC = inputGraph.addVertex("C");
	CSDAGVertex *vertexD = inputGraph.addVertex("D");

	inputGraph.addEdge(vertexA, "3", vertexB, "2", "1");
	inputGraph.addEdge(vertexA, "3", vertexC, "2", "0");
	inputGraph.addEdge(vertexB, "2", vertexD, "4", "0");
	inputGraph.addEdge(vertexC, "2", vertexD, "4", "0");

	// Printing the PiSDF graph.
#if PRINT_GRAPH
	dotWriter.write(&inputGraph, CSDAG_FILE_PATH, 1);
#endif

	// Transforming PiSDF to SrDAG if schedulable.

	// Printing the SrDAG graph.
}
