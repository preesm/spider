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
/****************************     TEST 9     ***********************************/
/*******************************************************************************/

PiSDFGraph* test9_sub(Archi* archi, Stack* stack){
	PiSDFGraph* graph = sAlloc(stack, 1, PiSDFGraph);

	// Graph
	*graph = PiSDFGraph(
			/*Edges*/ 	4,
			/*Params*/	0,
			/*InIf*/	1,
			/*OutIf*/	1,
			/*Config*/	0,
			/*Normal*/	2,
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
			"H", /*Fct*/ 3,
			/*In*/ 2, /*Out*/ 1,
			/*Par*/ 0);

	PiSDFVertex *vxBr = graph->addSpecialVertex(
			/*Type*/ PISDF_SUBTYPE_BROADCAST,
			/*In*/ 1, /*Out*/ 2,
			/*Par*/ 0);

	// Edges.
	graph->connect(
			/*Src*/ ifIn, /*SrcPrt*/ 0, /*Prod*/ "2",
			/*Snk*/ vxH, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxH, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxBr, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxBr, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ ifOut, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxBr, /*SrcPrt*/ 1, /*Prod*/ "1",
			/*Snk*/ vxH, /*SnkPrt*/ 1, /*Cons*/ "1",
			/*Delay*/ "1", 0);

	// Timings
//	Parser_InitVariable(&vxC->timings[0], &vxC->params,  "10", &pisdfAlloc);
//	Parser_InitVariable(&vxA->timings[0], &vxA->params,  "10", pisdfAlloc);
//	Parser_InitVariable(&vxB->timings[0], &vxA->params,  "10", pisdfAlloc);

	// Subgraphs

	return graph;
}

PiSDFGraph* test9(Archi* archi, Stack* stack){
	PiSDFGraph* graph = sAlloc(stack, 1, PiSDFGraph);

	// Graph
	*graph = PiSDFGraph(
			/*Edges*/ 	4,
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
			"A", /*Fct*/ 0,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 0);
	PiSDFVertex *vxB = graph->addBodyVertex(
			"B", /*Fct*/ 1,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 0);
	PiSDFVertex *vxC = graph->addBodyVertex(
			"C", /*Fct*/ 2,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 0);
	PiSDFVertex *vxBr = graph->addSpecialVertex(
			/*Type*/ PISDF_SUBTYPE_BROADCAST,
			/*In*/ 1, /*Out*/ 2,
			/*Par*/ 0);
	PiSDFVertex *vxH = graph->addHierVertex(
			"H_top",
			/*SubGraph*/ test9_sub(archi, stack),
			/*In*/ 1, /*Out*/ 1,
			/*Par*/ 0);

	// Edges.
	graph->connect(
			/*Src*/ vxA, /*SrcPrt*/ 0, /*Prod*/ "4",
			/*Snk*/ vxH, /*SnkPrt*/ 0, /*Cons*/ "2",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxH, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxBr, /*SnkPrt*/ 0, /*Cons*/ "2",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxBr, /*SrcPrt*/ 0, /*Prod*/ "2",
			/*Snk*/ vxB, /*SnkPrt*/ 0, /*Cons*/ "2",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxBr, /*SrcPrt*/ 1, /*Prod*/ "2",
			/*Snk*/ vxC, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	// Timings
//	Parser_InitVariable(&vxC->timings[0], &vxC->params,  "10", &pisdfAlloc);
//	Parser_InitVariable(&vxA->timings[0], &vxA->params,  "10", pisdfAlloc);
//	Parser_InitVariable(&vxB->timings[0], &vxA->params,  "10", pisdfAlloc);

	// Subgraphs

	return graph;
}

PiSDFGraph* initPisdf_test9(Archi* archi, Stack* stack){
	PiSDFGraph* top = sAlloc(stack, 1, PiSDFGraph);
	*top = PiSDFGraph(0,0,0,0,0,1, archi, stack);

	top->addHierVertex(
			"top", test9(archi, stack),
			0, 0, 0);

	return top;
}

SRDAGGraph* result_Test9(PiSDFGraph* pisdf, Stack* stack){
	SRDAGGraph* srdag = sAlloc(stack, 1, SRDAGGraph);
	*srdag = SRDAGGraph(stack);

	PiSDFGraph* topPisdf = pisdf->getBody(0)->getSubGraph();
	SRDAGVertex* vxA  = srdag->addVertex(topPisdf->getBody(0));
	SRDAGVertex* vxB  = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxC0 = srdag->addVertex(topPisdf->getBody(2));
	SRDAGVertex* vxC1 = srdag->addVertex(topPisdf->getBody(2));
	SRDAGVertex* vxH0 = srdag->addVertex(topPisdf->getBody(4)->getSubGraph()->getBody(0));
	SRDAGVertex* vxH1 = srdag->addVertex(topPisdf->getBody(4)->getSubGraph()->getBody(0));
	SRDAGVertex* vxH2 = srdag->addVertex(topPisdf->getBody(4)->getSubGraph()->getBody(0));
	SRDAGVertex* vxH3 = srdag->addVertex(topPisdf->getBody(4)->getSubGraph()->getBody(0));
	SRDAGVertex* vxJ  = srdag->addJoin(2);
	SRDAGVertex* vxF0 = srdag->addFork(4);
	SRDAGVertex* vxF1 = srdag->addFork(2);
	SRDAGVertex* vxBr = srdag->addBroadcast(2);
	SRDAGVertex* vxI0  = srdag->addInit();
	SRDAGVertex* vxI1  = srdag->addInit();

	srdag->addEdge(
			vxA , 0,
			vxF0, 0,
			4);
	srdag->addEdge(
			vxF0 , 0,
			vxH0, 0,
			1);
	srdag->addEdge(
			vxF0, 1,
			vxH1, 0,
			1);
	srdag->addEdge(
			vxF0, 2,
			vxH2, 0,
			1);
	srdag->addEdge(
			vxF0, 3,
			vxH3, 0,
			1);
	srdag->addEdge(
			vxI0, 0,
			vxH0, 1,
			1);
	srdag->addEdge(
			vxH0, 0,
			vxH1, 1,
			1);
	srdag->addEdge(
			vxI1, 0,
			vxH2, 1,
			1);
	srdag->addEdge(
			vxH2, 0,
			vxH3, 1,
			1);
	srdag->addEdge(
			vxH1, 0,
			vxJ , 0,
			1);
	srdag->addEdge(
			vxH3, 0,
			vxJ , 1,
			1);
	srdag->addEdge(
			vxJ , 0,
			vxBr, 0,
			2);
	srdag->addEdge(
			vxBr, 0,
			vxB , 0,
			2);
	srdag->addEdge(
			vxBr, 1,
			vxF1, 0,
			2);
	srdag->addEdge(
			vxF1, 0,
			vxC0, 0,
			1);
	srdag->addEdge(
			vxF1, 1,
			vxC1, 0,
			1);
	return srdag;
}


void test_Test9(PiSDFGraph* pisdf, SRDAGGraph* srdag, Stack* stack){
	BipartiteGraph::compareGraphs(srdag, result_Test9(pisdf, stack), stack, "Test9");
}
