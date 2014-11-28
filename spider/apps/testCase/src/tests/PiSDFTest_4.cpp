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
/****************************     TEST 4     ***********************************/
/*******************************************************************************/

PiSDFGraph* test4(Archi* archi, Stack* stack){
	PiSDFGraph* graph = sAlloc(stack, 1, PiSDFGraph);

	// Graph
	*graph = PiSDFGraph(
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
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 0);
	PiSDFVertex *vxC = graph->addBodyVertex(
			"C", /*Fct*/ 2,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 0);
	PiSDFVertex *vxJ = graph->addSpecialVertex(
			/*Type*/ PISDF_SUBTYPE_JOIN,
			/*In*/ 2, /*Out*/ 1,
			/*Par*/ 0);

	// Edges.
	graph->connect(
			/*Src*/ vxA, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxJ, /*SnkPrt*/ 0, /*Cons*/ "2",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxB, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxJ, /*SnkPrt*/ 1, /*Cons*/ "2",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxJ, /*SrcPrt*/ 0, /*Prod*/ "4",
			/*Snk*/ vxC, /*SnkPrt*/ 0, /*Cons*/ "4",
			/*Delay*/ "0", 0);

	// Timings
//	Parser_InitVariable(&vxC->timings[0], &vxC->params,  "10", &pisdfAlloc);
//	Parser_InitVariable(&vxA->timings[0], &vxA->params,  "10", pisdfAlloc);
//	Parser_InitVariable(&vxB->timings[0], &vxA->params,  "10", pisdfAlloc);

	// Subgraphs

	return graph;
}

PiSDFGraph* initPisdf_test4(Archi* archi, Stack* stack){
	PiSDFGraph* top = sAlloc(stack, 1, PiSDFGraph);
	*top = PiSDFGraph(0,0,0,0,0,1, archi, stack);

	top->addHierVertex(
			"top", test4(archi, stack),
			0, 0, 0);

	return top;
}


SRDAGGraph* result_Test4(PiSDFGraph* pisdf, Stack* stack){
	SRDAGGraph* srdag = sAlloc(stack, 1, SRDAGGraph);
	*srdag = SRDAGGraph(stack);

	PiSDFGraph* topPisdf = pisdf->getBody(0)->getSubGraph();
	SRDAGVertex* vxA0 = srdag->addVertex(topPisdf->getBody(0));
	SRDAGVertex* vxA1 = srdag->addVertex(topPisdf->getBody(0));
	SRDAGVertex* vxB0 = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxB1 = srdag->addVertex(topPisdf->getBody(1));
	SRDAGVertex* vxC  = srdag->addVertex(topPisdf->getBody(2));
	SRDAGVertex* vxJ  = srdag->addJoin(4);

	srdag->addEdge(
			vxA0, 0,
			vxJ, 0,
			1);
	srdag->addEdge(
			vxA1, 0,
			vxJ, 1,
			1);
	srdag->addEdge(
			vxB0, 0,
			vxJ, 2,
			1);
	srdag->addEdge(
			vxB1, 0,
			vxJ, 3,
			1);
	srdag->addEdge(
			vxJ, 0,
			vxC, 0,
			4);
	return srdag;
}


void test_Test4(PiSDFGraph* pisdf, SRDAGGraph* srdag, Stack* stack){
	BipartiteGraph::compareGraphs(srdag, result_Test4(pisdf, stack), stack, "Test4");
}
