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
/****************************     TEST 4     ***********************************/
/*******************************************************************************/
#define VERBOSE 0

void test4_A(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute A\n");
#endif

	out[0] = 1;
	out[1] = 2;
	out[2] = 3;
	out[3] = 4;
}

void test4_B(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in  = (char*)inputFIFOs[0];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute B\n");
#endif

	out[0] = in[0]+1;
}

void test4_C(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in  = (char*)inputFIFOs[0];
	char* out = (char*)outputFIFOs[0];

#if VERBOSE
	printf("Execute C\n");
#endif

	out[0] = in[0]+2;
}

void test4_Check(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	char* in = (char*)inputFIFOs[0];

	char expected[4] = {2,3,5,6};
	int nb = 4;

	printf("Test: ");
	for(int i=0; i<nb; i++){
		if(in[i] != expected[i]){
			printf("FAILED\n");
			return;
		}
	}
	printf("PASSED\n");
}

lrtFct test4_fcts[NB_FCT_TEST4] = {&test4_A, &test4_B, &test4_C, &test4_Check};

PiSDFGraph* test4(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
			/*Edges*/ 	6,
			/*Params*/	0,
			/*InIf*/	0,
			/*OutIf*/	0,
			/*Config*/	0,
			/*Normal*/	6,
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
			/*In*/ 1, /*Out*/ 1,
			/*Par*/ 0);
	PiSDFVertex *vxC = graph->addBodyVertex(
			"C", /*Fct*/ 2,
			/*In*/ 1, /*Out*/ 1,
			/*Par*/ 0);
	PiSDFVertex *vxCheck = graph->addBodyVertex(
			"Check", /*Fct*/ 3,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 0);
	PiSDFVertex *vxF = graph->addSpecialVertex(
			/*Type*/ PISDF_SUBTYPE_FORK,
			/*In*/ 1, /*Out*/ 2,
			/*Par*/ 0);
	PiSDFVertex *vxJ = graph->addSpecialVertex(
			/*Type*/ PISDF_SUBTYPE_JOIN,
			/*In*/ 2, /*Out*/ 1,
			/*Par*/ 0);

	// Edges.

	graph->connect(
			/*Src*/ vxA, /*SrcPrt*/ 0, /*Prod*/ "4",
			/*Snk*/ vxF, /*SnkPrt*/ 0, /*Cons*/ "4",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxF, /*SrcPrt*/ 0, /*Prod*/ "2",
			/*Snk*/ vxB, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxF, /*SrcPrt*/ 1, /*Prod*/ "2",
			/*Snk*/ vxC, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxB, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxJ, /*SnkPrt*/ 0, /*Cons*/ "2",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxC, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxJ, /*SnkPrt*/ 1, /*Cons*/ "2",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxJ, /*SrcPrt*/ 0, /*Prod*/ "4",
			/*Snk*/ vxCheck, /*SnkPrt*/ 0, /*Cons*/ "4",
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

PiSDFGraph* initPisdf_test4(Archi* archi, Stack* stack){
	PiSDFGraph* top = CREATE(stack, PiSDFGraph)(
			0,0,0,0,0,1, archi, stack);

	top->addHierVertex(
			"top", test4(archi, stack),
			0, 0, 0);

	return top;
}

void freePisdf_test4(PiSDFGraph* top, Stack* stack){
	top->~PiSDFGraph();
	stack->free(top);
}

SRDAGGraph* result_test4(PiSDFGraph* pisdf, Stack* stack){
	SRDAGGraph* srdag = CREATE(stack, SRDAGGraph)(stack);

	PiSDFGraph* topPisdf = pisdf->getBody(0)->getSubGraph();
	SRDAGVertex* vxA = srdag->addVertex(topPisdf->getBody(0));
	SRDAGVertex* vxB0 = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxB1 = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxC0 = srdag->addVertex(topPisdf->getBody(2));
	SRDAGVertex* vxC1 = srdag->addVertex(topPisdf->getBody(2));
	SRDAGVertex* vxF  = srdag->addFork(4);

	SRDAGVertex* vxJ  = srdag->addJoin(4);
	SRDAGVertex* vxCheck = srdag->addVertex(topPisdf->getBody(3));

	srdag->addEdge(
			vxA, 0,
			vxF, 0,
			4);
	srdag->addEdge(
			vxF, 0,
			vxB0, 0,
			1);
	srdag->addEdge(
			vxF, 1,
			vxB1, 0,
			1);
	srdag->addEdge(
			vxF, 2,
			vxC0, 0,
			1);
	srdag->addEdge(
			vxF, 3,
			vxC1, 0,
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
			vxC0, 0,
			vxJ, 2,
			1);
	srdag->addEdge(
			vxC1, 0,
			vxJ, 3,
			1);

	srdag->addEdge(
			vxJ, 0,
			vxCheck, 0,
			4);
	return srdag;
}


void test_Test4(PiSDFGraph* pisdf, SRDAGGraph* srdag, Stack* stack){
	SRDAGGraph* model = result_test4(pisdf, stack);
	BipartiteGraph::compareGraphs(srdag, model, stack, "test4");
	model->~SRDAGGraph();
	stack->free(model);
}
