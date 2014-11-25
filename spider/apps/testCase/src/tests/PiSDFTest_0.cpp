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
/****************************     TEST 0     ***********************************/
/*******************************************************************************/

PiSDFGraph* test0(Stack* stack, int N){
	PiSDFGraph* graph = sAlloc(stack, 1, PiSDFGraph);
	// Graph
	*graph = PiSDFGraph(
			/*Edges*/ 	1,
			/*Params*/	1,
			/*InIf*/	0,
			/*OutIf*/	0,
			/*Config*/	1,
			/*Normal*/	2,
			stack);

	// Parameters.
//	PiSDFParam *paramN = PiSDFGraphAddDynamicParam(graph, "N");
	PiSDFParam *paramN = graph->addStaticParam("N", N);

	// Configure vertices.
	PiSDFVertex *vxC = graph->addConfigVertex(
			"C", /*Fct*/ 0,
			PISDF_SUBTYPE_NORMAL,
			/*In*/ 0,  /*Out*/ 0,
			/*Par*/ 0, /*Cfg*/ 0);

	// Other vertices
	PiSDFVertex *vxA = graph->addBodyVertex(
			"A", /*Fct*/ 1,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 0);

	PiSDFVertex *vxB = graph->addBodyVertex(
			"B", /*Fct*/ 2,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 1);
	vxB->addInParam(0, paramN);

	// Edges.
	graph->connect(
			/*Src*/ vxA, /*SrcPrt*/ 0, /*Prod*/ "2",
			/*Snk*/ vxB, /*SnkPrt*/ 0, /*Cons*/ "N",
			/*Delay*/ "0", 0);

	// Timings
//	Parser_InitVariable(&vxC->timings[0], &vxC->params,  "10", &pisdfAlloc);
//	Parser_InitVariable(&vxA->timings[0], &vxA->params,  "10", pisdfAlloc);
//	Parser_InitVariable(&vxB->timings[0], &vxB->params,  "10", pisdfAlloc);

	// Subgraphs

	return graph;
}

PiSDFGraph* initPisdf_test0(Stack* stack, int N){
	PiSDFGraph* top = sAlloc(stack, 1, PiSDFGraph);
	*top = PiSDFGraph(0,0,0,0,0,1, stack);

	top->addHierVertex(
			"top", test0(stack, N),
			0, 0, 0);

	return top;
}

SRDAGGraph* result_Test0_1(PiSDFGraph* pisdf, Stack* stack){
	SRDAGGraph* srdag = sAlloc(stack, 1, SRDAGGraph);
	*srdag = SRDAGGraph(stack);

	PiSDFGraph* topPisdf = pisdf->getBody(0)->getSubGraph();
	SRDAGVertex* vxC = srdag->addVertex(topPisdf->getConfig(0));
	SRDAGVertex* vxA = srdag->addVertex(topPisdf->getBody(0));
	SRDAGVertex* vxB0 = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxB1 = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxF = srdag->addFork(2);

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

SRDAGGraph* result_Test0_2(PiSDFGraph* pisdf, Stack* stack){
	SRDAGGraph* srdag = sAlloc(stack, 1, SRDAGGraph);
	*srdag = SRDAGGraph(stack);

	PiSDFGraph* topPisdf = pisdf->getBody(0)->getSubGraph();
	SRDAGVertex* vxC = srdag->addVertex(topPisdf->getConfig(0));
	SRDAGVertex* vxA = srdag->addVertex(topPisdf->getBody(0));
	SRDAGVertex* vxB = srdag->addVertex(topPisdf->getBody(1));

	srdag->addEdge(
			vxA, 0,
			vxB, 0,
			2);

	return srdag;
}

SRDAGGraph* result_Test0_3(PiSDFGraph* pisdf, Stack* stack){
	SRDAGGraph* srdag = sAlloc(stack, 1, SRDAGGraph);
	*srdag = SRDAGGraph(stack);

	PiSDFGraph* topPisdf = pisdf->getBody(0)->getSubGraph();
	SRDAGVertex* vxC = srdag->addVertex(topPisdf->getConfig(0));
	SRDAGVertex* vxA0 = srdag->addVertex(topPisdf->getBody(0));
	SRDAGVertex* vxA1 = srdag->addVertex(topPisdf->getBody(0));
	SRDAGVertex* vxA2 = srdag->addVertex(topPisdf->getBody(0));
	SRDAGVertex* vxB0 = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxB1 = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxF = srdag->addFork(2);
	SRDAGVertex* vxJ0 = srdag->addJoin(2);
	SRDAGVertex* vxJ1 = srdag->addJoin(2);

	srdag->addEdge(
			vxA0, 0,
			vxJ0, 0,
			2);
	srdag->addEdge(
			vxA1, 0,
			vxF, 0,
			2);
	srdag->addEdge(
			vxA2, 0,
			vxJ1, 1,
			2);
	srdag->addEdge(
			vxF, 0,
			vxJ0, 1,
			1);
	srdag->addEdge(
			vxF, 1,
			vxJ1, 0,
			1);
	srdag->addEdge(
			vxJ0, 0,
			vxB0, 0,
			3);
	srdag->addEdge(
			vxJ1, 0,
			vxB1, 0,
			3);

	return srdag;
}

static SRDAGGraph* (*result_Test0[]) (PiSDFGraph* pisdf, Stack* stack) = {
		result_Test0_1,
		result_Test0_2,
		result_Test0_3
};

void test_Test0(PiSDFGraph* pisdf, SRDAGGraph* srdag, int N, Stack* stack){
	char name[100];
	snprintf(name, 100, "Test0_%d", N);
	BipartiteGraph::compareGraphs(srdag, result_Test0[N-1](pisdf, stack), stack, name);
}
