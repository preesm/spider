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

/** Actors */
#define VERBOSE 0

void test0_C(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	static int i=1;
#if VERBOSE
	printf("Execute C\n");
#endif
	outParams[0] = i++;
}

void test0_A(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute A\n");
#endif

	out[0] = 1;
	out[1] = 2;
}

void test0_B(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param N = inParams[0];
	char* in = (char*)inputFIFOs[0];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute B: ");
	for(int i=0;i<N; i++){
		printf("%d ", in[i]);
	}
	printf("\n");
#endif

	memcpy(out, in, N);
}

void test0_Check(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param N = inParams[0];
	char* in = (char*)inputFIFOs[0];

	char expected[6] = {1,2,1,2,1,2};
	int nb;
	switch(N){
	case 1:
	case 2:
		nb = 2;
		break;
	case 3:
		nb = 6;
		break;
	}

	printf("Test: ");
	for(int i=0; i<nb; i++){
		if(in[i] != expected[i]){
			printf("FAILED\n");
			return;
		}
	}
	printf("PASSED\n");
}

lrtFct test0_fcts[4] = {&test0_C, &test0_A, &test0_B, &test0_Check};

/** PISDF Graphs **/

PiSDFGraph* test0(Archi* archi, Stack* stack, int N){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
			/*Edges*/ 	2,
			/*Params*/	1,
			/*InIf*/	0,
			/*OutIf*/	0,
			/*Config*/	1,
			/*Normal*/	3,
			archi,
			stack);

	// Parameters.
	PiSDFParam *paramN = graph->addDynamicParam("N");

	// Configure vertices.
	PiSDFVertex *vxC = graph->addConfigVertex(
			"C", /*Fct*/ 0,
			PISDF_SUBTYPE_NORMAL,
			/*In*/ 0,  /*Out*/ 0,
			/*Par*/ 0, /*Cfg*/ 1);
	vxC->addOutParam(0, paramN);

	// Other vertices
	PiSDFVertex *vxA = graph->addBodyVertex(
			"A", /*Fct*/ 1,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 0);

	PiSDFVertex *vxB = graph->addBodyVertex(
			"B", /*Fct*/ 2,
			/*In*/ 1, /*Out*/ 1,
			/*Par*/ 1);
	vxB->addInParam(0, paramN);

	PiSDFVertex *vxCheck = graph->addBodyVertex(
			"Check", /*Fct*/ 3,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 1);
	vxCheck->addInParam(0, paramN);

	// Edges.
	graph->connect(
			/*Src*/ vxA, /*SrcPrt*/ 0, /*Prod*/ "2",
			/*Snk*/ vxB, /*SnkPrt*/ 0, /*Cons*/ "N",
			/*Delay*/ "0", 0);
	graph->connect(
			/*Src*/ vxB, /*SrcPrt*/ 0, /*Prod*/ "N",
			/*Snk*/ vxCheck, /*SnkPrt*/ 0, /*Cons*/ "4*(N/3)+2",
			/*Delay*/ "0", 0);

	// Timings
	vxA->isExecutableOnAllPE();
	vxA->setTimingOnType(0, "10", stack);
	vxB->isExecutableOnAllPE();
	vxB->setTimingOnType(0, "10", stack);
	vxC->isExecutableOnAllPE();
	vxC->setTimingOnType(0, "10", stack);
	vxCheck->isExecutableOnAllPE();
	vxCheck->setTimingOnType(0, "10", stack);

	// Subgraphs

	return graph;
}

PiSDFGraph* initPisdf_test0(Archi* archi, Stack* stack, int N){
	PiSDFGraph* top = CREATE(stack, PiSDFGraph)(
			0,0,0,0,0,1, archi, stack);

	top->addHierVertex(
			"top", test0(archi, stack, N),
			0, 0, 0);

	return top;
}

SRDAGGraph* result_Test0_1(PiSDFGraph* pisdf, Stack* stack){
	SRDAGGraph* srdag = CREATE(stack, SRDAGGraph)(stack);

	PiSDFGraph* topPisdf = pisdf->getBody(0)->getSubGraph();
	SRDAGVertex* vxC = srdag->addVertex(topPisdf->getConfig(0));
	SRDAGVertex* vxA = srdag->addVertex(topPisdf->getBody(0));
	SRDAGVertex* vxB0 = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxB1 = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxCheck = srdag->addVertex(topPisdf->getBody(2));
	SRDAGVertex* vxF = srdag->addFork(2);
	SRDAGVertex* vxJ = srdag->addJoin(2);

	vxB0->addInParam(0, 1);
	vxB1->addInParam(0, 1);
	vxCheck->addInParam(0, 1);

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
	srdag->addEdge(
			vxB0, 0,
			vxJ, 0,
			1);
	srdag->addEdge(
			vxB1, 0,
			vxJ, 1,
			1);
	srdag->addEdge(
			vxJ, 0,
			vxCheck, 0,
			2);

	return srdag;
}

SRDAGGraph* result_Test0_2(PiSDFGraph* pisdf, Stack* stack){
	SRDAGGraph* srdag = CREATE(stack, SRDAGGraph)(stack);

	PiSDFGraph* topPisdf = pisdf->getBody(0)->getSubGraph();
	SRDAGVertex* vxC = srdag->addVertex(topPisdf->getConfig(0));
	SRDAGVertex* vxA = srdag->addVertex(topPisdf->getBody(0));
	SRDAGVertex* vxB = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxCheck = srdag->addVertex(topPisdf->getBody(2));

	vxB->addInParam(0, 2);
	vxCheck->addInParam(0, 2);

	srdag->addEdge(
			vxA, 0,
			vxB, 0,
			2);
	srdag->addEdge(
			vxB, 0,
			vxCheck, 0,
			2);

	return srdag;
}

SRDAGGraph* result_Test0_3(PiSDFGraph* pisdf, Stack* stack){
	SRDAGGraph* srdag = CREATE(stack, SRDAGGraph)(stack);

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

	SRDAGVertex* vxCheck = srdag->addVertex(topPisdf->getBody(2));
	SRDAGVertex* vxJ2 = srdag->addJoin(2);

	vxB0->addInParam(0, 3);
	vxB1->addInParam(0, 3);
	vxCheck->addInParam(0, 3);

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

	srdag->addEdge(
			vxB0, 0,
			vxJ2, 0,
			3);
	srdag->addEdge(
			vxB1, 0,
			vxJ2, 1,
			3);
	srdag->addEdge(
			vxJ2, 0,
			vxCheck, 0,
			6);

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
	SRDAGGraph* model = result_Test0[N-1](pisdf, stack);
	BipartiteGraph::compareGraphs(srdag, model, stack, name);
	model->~SRDAGGraph();
	stack->free(model);
}
