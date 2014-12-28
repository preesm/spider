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
/****************************     TEST 8     ***********************************/
/*******************************************************************************/

PiSDFGraph* test8_sub(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
			/*Edges*/ 	3,
			/*Params*/	0,
			/*InIf*/	1,
			/*OutIf*/	1,
			/*Config*/	1,
			/*Normal*/	1,
			archi,
			stack);

	// Parameters.

	// Interfaces
	PiSDFVertex *ifIn = graph->addInputIf(
			"in",
			0 /*Par*/);
	PiSDFVertex *ifOut = graph->addOutputIf(
			"out",
			0 /*Par*/);

	// Configure vertices
	PiSDFVertex *cfC = graph->addConfigVertex(
			"C", /*Fct*/ 33,
			/*Type*/ PISDF_SUBTYPE_NORMAL,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 0, /*OutPar*/ 0);

	// Other vertices
	PiSDFVertex *vxH = graph->addBodyVertex(
			"H", /*Fct*/ 34,
			/*In*/ 2, /*Out*/ 1,
			/*Par*/ 0);

	// Edges.
	graph->connect(
			/*Src*/ ifIn, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxH, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxH, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ ifOut, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ cfC, /*SrcPrt*/ 0, /*Prod*/ "3",
			/*Snk*/ vxH, /*SnkPrt*/ 1, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	// Timings
	cfC->isExecutableOnAllPE();
	cfC->setTimingOnType(0, "10", stack);
	vxH->isExecutableOnAllPE();
	vxH->setTimingOnType(0, "10", stack);

	return graph;
}

PiSDFGraph* test8(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
			/*Edges*/ 	5,
			/*Params*/	0,
			/*InIf*/	0,
			/*OutIf*/	0,
			/*Config*/	0,
			/*Normal*/	5,
			archi,
			stack);

	// Parameters.

	// Configure vertices

	// Other vertices
	PiSDFVertex *vxA = graph->addBodyVertex(
			"A", /*Fct*/ 31,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 0);
	PiSDFVertex *vxB = graph->addBodyVertex(
			"B", /*Fct*/ 32,
			/*In*/ 1, /*Out*/ 1,
			/*Par*/ 0);
	PiSDFVertex *vxCheck = graph->addBodyVertex(
			"Check", /*Fct*/ 35,
			/*In*/ 2, /*Out*/ 0,
			/*Par*/ 0);
	PiSDFVertex *vxBr = graph->addSpecialVertex(
			/*Type*/ PISDF_SUBTYPE_BROADCAST,
			/*In*/ 1, /*Out*/ 2,
			/*Par*/ 0);
	PiSDFVertex *vxH = graph->addHierVertex(
			"H_top",
			/*SubGraph*/ test8_sub(archi, stack),
			/*In*/ 1, /*Out*/ 1,
			/*Par*/ 0);

	// Edges.
	graph->connect(
			/*Src*/ vxA, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxBr, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxBr, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxH, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxBr, /*SrcPrt*/ 1, /*Prod*/ "1",
			/*Snk*/ vxB, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxH, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxCheck, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxB, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxCheck, /*SnkPrt*/ 1, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	// Timings
	vxA->isExecutableOnAllPE();
	vxA->setTimingOnType(0, "10", stack);
	vxB->isExecutableOnAllPE();
	vxB->setTimingOnType(0, "10", stack);
	vxCheck->isExecutableOnAllPE();
	vxCheck->setTimingOnType(0, "10", stack);

	return graph;
}

PiSDFGraph* initPisdf_test8(Archi* archi, Stack* stack){
	PiSDFGraph* top = CREATE(stack, PiSDFGraph)(
			0,0,0,0,0,1, archi, stack);

	top->addHierVertex(
			"top", test8(archi, stack),
			0, 0, 0);

	return top;
}

void freePisdf_test8(PiSDFGraph* top, Stack* stack){
	top->~PiSDFGraph();
	stack->free(top);
}


SRDAGGraph* result_Test8(PiSDFGraph* pisdf, Stack* stack){
	SRDAGGraph* srdag = CREATE(stack, SRDAGGraph)(stack);

	PiSDFGraph* topPisdf = pisdf->getBody(0)->getSubGraph();
	SRDAGVertex* vxA  = srdag->addVertex(topPisdf->getBody(0));
	SRDAGVertex* vxB  = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxCheck  = srdag->addVertex(topPisdf->getBody(2));
	SRDAGVertex* vxC  = srdag->addVertex(topPisdf->getBody(4)->getSubGraph()->getConfig(0));
	SRDAGVertex* vxH0 = srdag->addVertex(topPisdf->getBody(4)->getSubGraph()->getBody(0));
	SRDAGVertex* vxH1 = srdag->addVertex(topPisdf->getBody(4)->getSubGraph()->getBody(0));
	SRDAGVertex* vxH2 = srdag->addVertex(topPisdf->getBody(4)->getSubGraph()->getBody(0));
	SRDAGVertex* vxF  = srdag->addFork(3);
	SRDAGVertex* vxBr = srdag->addBroadcast(4);
	SRDAGVertex* vxE0 = srdag->addEnd();
	SRDAGVertex* vxE1 = srdag->addEnd();

	srdag->addEdge(
			vxA , 0,
			vxBr, 0,
			1);
	srdag->addEdge(
			vxBr, 0,
			vxH0, 0,
			1);
	srdag->addEdge(
			vxBr, 1,
			vxH1, 0,
			1);
	srdag->addEdge(
			vxBr, 2,
			vxH2, 0,
			1);
	srdag->addEdge(
			vxBr, 3,
			vxB , 0,
			1);
	srdag->addEdge(
			vxB, 0,
			vxCheck, 1,
			1);
	srdag->addEdge(
			vxH0, 0,
			vxE0, 0,
			1);
	srdag->addEdge(
			vxH1, 0,
			vxE1, 0,
			1);
	srdag->addEdge(
			vxH2, 0,
			vxCheck , 0,
			1);
	srdag->addEdge(
			vxC, 0,
			vxF, 0,
			3);
	srdag->addEdge(
			vxF , 0,
			vxH0, 1,
			1);
	srdag->addEdge(
			vxF , 1,
			vxH1, 1,
			1);
	srdag->addEdge(
			vxF , 2,
			vxH2, 1,
			1);
	return srdag;
}


void test_Test8(PiSDFGraph* pisdf, SRDAGGraph* srdag, Stack* stack){
	SRDAGGraph* model = result_Test8(pisdf, stack);
	BipartiteGraph::compareGraphs(srdag, model, stack, "Test8");
	model->~SRDAGGraph();
	stack->free(model);
}
