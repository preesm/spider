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
#include <cstdio>

/*******************************************************************************/
/****************************     TEST 2     ***********************************/
/*******************************************************************************/

PiSDFGraph* test2(Archi* archi, Stack* stack, int N){
	PiSDFGraph* graph = sAlloc(stack, 1, PiSDFGraph);

	// Graph
	*graph = PiSDFGraph(
			/*Edges*/ 	2,
			/*Params*/	1,
			/*InIf*/	0,
			/*OutIf*/	0,
			/*Config*/	1,
			/*Normal*/	2,
			archi,
			stack);

	// Parameters.
//	PiSDFParam *paramN = PiSDFGraphAddDynamicParam(graph, "N");
	PiSDFParam *paramN = graph->addStaticParam("N", N);

	// Configure vertices.
	PiSDFVertex *vxC = graph->addConfigVertex(
			"C", /*Fct*/ 0,
			PISDF_SUBTYPE_NORMAL,
			/*In*/ 0,  /*Out*/ 1,
			/*Par*/ 0, /*Cfg*/ 0);

	// Other vertices
	PiSDFVertex *vxA = graph->addBodyVertex(
			"A", /*Fct*/ 1,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 1);
	vxA->addInParam(0, paramN);

	PiSDFVertex *vxB = graph->addBodyVertex(
			"B", /*Fct*/ 2,
			/*In*/ 2, /*Out*/ 0,
			/*Par*/ 0);

	// Edges.
	graph->connect(
			/*Src*/ vxC, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxB, /*SnkPrt*/ 1, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxA, /*SrcPrt*/ 0, /*Prod*/ "N",
			/*Snk*/ vxB, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	// Timings
	vxA->isExecutableOnAllPE();
	vxA->setTimingOnType(0, "10", stack);
	vxB->isExecutableOnAllPE();
	vxB->setTimingOnType(0, "10", stack);
	vxC->isExecutableOnAllPE();
	vxC->setTimingOnType(0, "10", stack);

	// Subgraphs

	return graph;
}

PiSDFGraph* initPisdf_test2(Archi* archi, Stack* stack, int N){
	PiSDFGraph* top = sAlloc(stack, 1, PiSDFGraph);
	*top = PiSDFGraph(0,0,0,0,0,1, archi, stack);

	top->addHierVertex(
			"top", test2(archi, stack, N),
			0, 0, 0);

	return top;
}

SRDAGGraph* result_Test2_1(PiSDFGraph* pisdf, Stack* stack){
	SRDAGGraph* srdag = sAlloc(stack, 1, SRDAGGraph);
	*srdag = SRDAGGraph(stack);

	PiSDFGraph* topPisdf = pisdf->getBody(0)->getSubGraph();
	SRDAGVertex* vxC = srdag->addVertex(topPisdf->getConfig(0));
	SRDAGVertex* vxA = srdag->addVertex(topPisdf->getBody(0));
	SRDAGVertex* vxB = srdag->addVertex(topPisdf->getBody(1));

	srdag->addEdge(
			vxC, 0,
			vxB, 1,
			1);
	srdag->addEdge(
			vxA, 0,
			vxB, 0,
			1);

	return srdag;
}

SRDAGGraph* result_Test2_2(PiSDFGraph* pisdf, Stack* stack){
	SRDAGGraph* srdag = sAlloc(stack, 1, SRDAGGraph);
	*srdag = SRDAGGraph(stack);

	PiSDFGraph* topPisdf = pisdf->getBody(0)->getSubGraph();
	SRDAGVertex* vxC = srdag->addVertex(topPisdf->getConfig(0));
	SRDAGVertex* vxA = srdag->addVertex(topPisdf->getBody(0));
	SRDAGVertex* vxB0 = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxB1 = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxF  = srdag->addFork(2);
	SRDAGVertex* vxBr = srdag->addBroadcast(2);

	srdag->addEdge(
			vxC, 0,
			vxBr, 0,
			1);
	srdag->addEdge(
			vxBr, 0,
			vxB0, 1,
			1);
	srdag->addEdge(
			vxBr, 1,
			vxB1, 1,
			1);
	srdag->addEdge(
			vxA, 0,
			vxF, 0,
			2);
	srdag->addEdge(
			vxF, 0,
			vxB0, 0,
			1);
	srdag->addEdge(
			vxF, 1,
			vxB1, 0,
			1);
	return srdag;
}

static SRDAGGraph* (*result_Test2[]) (PiSDFGraph* pisdf, Stack* stack) = {
		result_Test2_1,
		result_Test2_2
};

void test_Test2(PiSDFGraph* pisdf, SRDAGGraph* srdag, int N, Stack* stack){
	char name[100];
	snprintf(name, 100, "Test2_%d", N);
	BipartiteGraph::compareGraphs(srdag, result_Test2[N-1](pisdf, stack), stack, name);
}
