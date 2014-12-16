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
/****************************     TEST 3     ***********************************/
/*******************************************************************************/

PiSDFGraph* test3(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
			/*Edges*/ 	3,
			/*Params*/	0,
			/*InIf*/	0,
			/*OutIf*/	0,
			/*Config*/	0,
			/*Normal*/	4,
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
	PiSDFVertex *vxF = graph->addSpecialVertex(
			/*Type*/ PISDF_SUBTYPE_FORK,
			/*In*/ 1, /*Out*/ 2,
			/*Par*/ 0);

	// Edges.
	graph->connect(
			/*Src*/ vxA, /*SrcPrt*/ 0, /*Prod*/ "4",
			/*Snk*/ vxF, /*SnkPrt*/ 0, /*Cons*/ "2",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxF, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxB, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxF, /*SrcPrt*/ 1, /*Prod*/ "1",
			/*Snk*/ vxC, /*SnkPrt*/ 0, /*Cons*/ "1",
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

PiSDFGraph* initPisdf_test3(Archi* archi, Stack* stack){
	PiSDFGraph* top = CREATE(stack, PiSDFGraph)(
			0,0,0,0,0,1, archi, stack);

	top->addHierVertex(
			"top", test3(archi, stack),
			0, 0, 0);

	return top;
}

SRDAGGraph* result_Test3(PiSDFGraph* pisdf, Stack* stack){
	SRDAGGraph* srdag = CREATE(stack, SRDAGGraph)(stack);

	PiSDFGraph* topPisdf = pisdf->getBody(0)->getSubGraph();
	SRDAGVertex* vxA = srdag->addVertex(topPisdf->getBody(0));
	SRDAGVertex* vxB0 = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxB1 = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxC0 = srdag->addVertex(topPisdf->getBody(2));
	SRDAGVertex* vxC1 = srdag->addVertex(topPisdf->getBody(2));
	SRDAGVertex* vxF  = srdag->addFork(4);

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
			vxC0, 0,
			1);
	srdag->addEdge(
			vxF, 2,
			vxB1, 0,
			1);
	srdag->addEdge(
			vxF, 3,
			vxC1, 0,
			1);
	return srdag;
}


void test_Test3(PiSDFGraph* pisdf, SRDAGGraph* srdag, Stack* stack){
	BipartiteGraph::compareGraphs(srdag, result_Test3(pisdf, stack), stack, "Test3");
}
