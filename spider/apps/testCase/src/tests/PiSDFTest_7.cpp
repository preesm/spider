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

#include "../Tests.h"
#include <spider.h>

/*******************************************************************************/
/****************************     TEST 7     ***********************************/
/*******************************************************************************/

PiSDFGraph* test7_sub(Archi* archi, Stack* stack){
	PiSDFGraph* graph = sAlloc(stack, 1, PiSDFGraph);

	// Graph
	*graph = PiSDFGraph(
			/*Edges*/ 	2,
			/*Params*/	0,
			/*InIf*/	1,
			/*OutIf*/	1,
			/*Config*/	0,
			/*Normal*/	1,
			archi,
			stack);

	// Parameters.

	// Configure vertices

	// Interfaces
	PiSDFVertex *ifIn = graph->addInputIf(
			"in",
			0 /*Par*/);
	PiSDFVertex *ifOut = graph->addOutputIf(
			"out",
			0 /*Par*/);

	// Other vertices
	PiSDFVertex *vxH = graph->addBodyVertex(
			"H", /*Fct*/ 2,
			/*In*/ 1, /*Out*/ 1,
			/*Par*/ 0);

	// Edges.
	graph->connect(
			/*Src*/ ifIn, /*SrcPrt*/ 0, /*Prod*/ "2",
			/*Snk*/ vxH, /*SnkPrt*/ 0, /*Cons*/ "4",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxH, /*SrcPrt*/ 0, /*Prod*/ "4",
			/*Snk*/ ifOut, /*SnkPrt*/ 0, /*Cons*/ "2",
			/*Delay*/ "0", 0);

	// Timings
	vxH->isExecutableOnAllPE();
	vxH->setTimingOnType(0, "10", stack);

	// Subgraphs

	return graph;
}

PiSDFGraph* test7(Archi* archi, Stack* stack){
	PiSDFGraph* graph = sAlloc(stack, 1, PiSDFGraph);

	// Graph
	*graph = PiSDFGraph(
			/*Edges*/ 	2,
			/*Params*/	0,
			/*InIf*/	0,
			/*OutIf*/	0,
			/*Config*/	0,
			/*Normal*/	3,
			archi,
			stack);

	// Parameters.

	// Configure vertices

	// Other vertices
	PiSDFVertex *vxA = graph->addBodyVertex(
			"A", /*Fct*/ 0,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 0);
	PiSDFVertex *vxB = graph->addBodyVertex(
			"B", /*Fct*/ 1,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 0);
	PiSDFVertex *vxH = graph->addHierVertex(
			"H_top",
			/*SubGraph*/ test7_sub(archi, stack),
			/*In*/ 1, /*Out*/ 1,
			/*Par*/ 0);

	// Edges.
	graph->connect(
			/*Src*/ vxA, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxH, /*SnkPrt*/ 0, /*Cons*/ "2",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxH, /*SrcPrt*/ 0, /*Prod*/ "2",
			/*Snk*/ vxB, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	// Timings
	vxA->isExecutableOnAllPE();
	vxA->setTimingOnType(0, "10", stack);
	vxB->isExecutableOnAllPE();
	vxB->setTimingOnType(0, "10", stack);

	// Subgraphs

	return graph;
}

PiSDFGraph* initPisdf_test7(Archi* archi, Stack* stack){
	PiSDFGraph* top = sAlloc(stack, 1, PiSDFGraph);
	*top = PiSDFGraph(0,0,0,0,0,1, archi, stack);

	top->addHierVertex(
			"top", test7(archi, stack),
			0, 0, 0);

	return top;
}


SRDAGGraph* result_Test7(PiSDFGraph* pisdf, Stack* stack){
	SRDAGGraph* srdag = sAlloc(stack, 1, SRDAGGraph);
	*srdag = SRDAGGraph(stack);

	PiSDFGraph* topPisdf = pisdf->getBody(0)->getSubGraph();
	SRDAGVertex* vxA0 = srdag->addVertex(topPisdf->getBody(0));
	SRDAGVertex* vxA1 = srdag->addVertex(topPisdf->getBody(0));
	SRDAGVertex* vxB0 = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxB1 = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxH  = srdag->addVertex(topPisdf->getBody(2)->getSubGraph()->getBody(0));
	SRDAGVertex* vxBr = srdag->addBroadcast(2);
	SRDAGVertex* vxJ0 = srdag->addJoin(2);
	SRDAGVertex* vxJ1 = srdag->addJoin(2);
	SRDAGVertex* vxF  = srdag->addFork(3);
	SRDAGVertex* vxE  = srdag->addEnd();

	srdag->addEdge(
			vxA0, 0,
			vxJ0, 0,
			1);
	srdag->addEdge(
			vxA1, 0,
			vxJ0, 1,
			1);
	srdag->addEdge(
			vxJ0, 0,
			vxBr, 0,
			2);
	srdag->addEdge(
			vxBr, 0,
			vxJ1, 0,
			2);
	srdag->addEdge(
			vxBr, 1,
			vxJ1, 1,
			2);
	srdag->addEdge(
			vxJ1, 0,
			vxH , 0,
			4);
	srdag->addEdge(
			vxH , 0,
			vxF , 0,
			4);
	srdag->addEdge(
			vxF , 0,
			vxE , 0,
			2);
	srdag->addEdge(
			vxF , 1,
			vxB0 , 0,
			2);
	srdag->addEdge(
			vxF , 2,
			vxB1 , 0,
			2);
	return srdag;
}


void test_Test7(PiSDFGraph* pisdf, SRDAGGraph* srdag, Stack* stack){
	BipartiteGraph::compareGraphs(srdag, result_Test7(pisdf, stack), stack, "Test7");
}
