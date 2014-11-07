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

#include "Tests.h"
#include <spider.h>

/*******************************************************************************/
/****************************     TEST 0     ***********************************/
/*******************************************************************************/

PiSDFGraph* test0(Stack* stack){
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
	PiSDFParam *paramN = graph->addStaticParam("N", 3);

	// Configure vertices.
	PiSDFVertex *vxC = graph->addConfigVertex(
			"C", /*Fct*/ 0,
			PISDF_SUBTYPE_NORMAL,
			/*In*/ 0,  /*Out*/ 0,
			/*Par*/ 0, /*Cfg*/ 0);

	// Other vertices
	PiSDFVertex *vxA = graph->addBodyVertex(
			"A", /*Fct*/ 1, PISDF_SUBTYPE_NORMAL,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 0);

	PiSDFVertex *vxB = graph->addBodyVertex(
			"B", /*Fct*/ 1, PISDF_SUBTYPE_NORMAL,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 1);
	vxB->addInParam(0, paramN);

	// Edges.
	PiSDFEdge* edge;
	edge = graph->connect(
			/*Src*/ vxA, /*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxB, /*SnkPrt*/ 0, /*Cons*/ "N",
			/*Delay*/ "0", 0);

	// Timings
//	Parser_InitVariable(&vxC->timings[0], &vxC->params,  "10", &pisdfAlloc);
//	Parser_InitVariable(&vxA->timings[0], &vxA->params,  "10", pisdfAlloc);
//	Parser_InitVariable(&vxB->timings[0], &vxB->params,  "10", pisdfAlloc);

	// Subgraphs

	return graph;
}

PiSDFGraph* initPisdf_test0(Stack* stack){
	PiSDFGraph* top = sAlloc(stack, 1, PiSDFGraph);
	*top = PiSDFGraph(0,0,0,0,0,1, stack);

	PiSDFVertex *vxTop = top->addBodyVertex(
			"top", -1, PISDF_SUBTYPE_NORMAL,
			0, 0, 0);

	vxTop->setSubGraph(test0(stack));
	vxTop->getSubGraph()->setParentVertex(vxTop);

	return top;
}

///*******************************************************************************/
///****************************     TEST 1     ***********************************/
///*******************************************************************************/
//
//PiSDFGraph* test1(Stack* stack, int N){
//	// Graph
//	PiSDFGraph* graph = PiSDFCreateGraph(
//			/*Edges*/ 	1,
//			/*Params*/	1,
//			/*InIf*/	0,
//			/*OutIf*/	0,
//			/*Config*/	1,
//			/*Normal*/	1);
//
//	// Parameters.
////	PiSDFParam *paramN = PiSDFGraphAddDynamicParam(graph, "N");
//	PiSDFParam *paramN = PiSDFGraphAddStaticParamValue(graph, "N", N);
//
//	// Configure vertices.
//	PiSDFVertex *vxC = PiSDFGraphAddConfigVertex(graph, /*Fct*/ 0, "C", PiSDFSub_Normal, /*In*/ 0, /*Out*/ 1, /*Par*/ 0, /*Cfg*/ 1);
////	PiSDFVertex_AddOutputParam(vxC, paramN);
//
//	// Other vertices
//	PiSDFVertex *vxA = PiSDFGraphAddNormalVertex(graph, /*Fct*/ 1, "A", PiSDFSub_Normal, /*In*/ 1, /*Out*/ 0, /*Par*/ 1);
//	PiSDFVertex_AddParam(vxA, paramN);
//
//	// Edges.
//	PiSDFGraphAddEdge( /*Graph*/ graph,
//			/*Src*/ vxC, /*SrcPrt*/ 0, /*Prod*/ "3",
//			/*Snk*/ vxA, /*SnkPrt*/ 0, /*Cons*/ "N", /*Delay*/ 0);
//
//	// Timings
//	Parser_InitVariable(&vxC->timings[0], &vxC->params,  "10", &pisdfAlloc);
//	Parser_InitVariable(&vxA->timings[0], &vxA->params,  "10", pisdfAlloc);
//
//	// Subgraphs
//
//	return graph;
//}
//
//PiSDFGraph* initPisdf_test1(Stack* stack, int N){
//	PiSDFGraph* top = PiSDFCreateGraph(0,0,0,0,0,1);
//
//	PiSDFVertex *vxTop = PiSDFGraphAddNormalVertex(top, -1, "top", PiSDFSub_Normal, 0, 0, 0);
//
//	vxTop->subGraph = test1(N);
//	vxTop->subGraph->parentVertex = vxTop;
//
//	return top;
//}
//
///*******************************************************************************/
///****************************     TEST 2     ***********************************/
///*******************************************************************************/
//
//PiSDFGraph* test2(Stack* stack, int N){
//	// Graph
//	PiSDFGraph* graph = PiSDFCreateGraph(
//			/*Edges*/ 	2,
//			/*Params*/	1,
//			/*InIf*/	0,
//			/*OutIf*/	0,
//			/*Config*/	1,
//			/*Normal*/	2);
//
//	// Parameters.
////	PiSDFParam *paramN = PiSDFGraphAddDynamicParam(graph, "N");
//	PiSDFParam *paramN = PiSDFGraphAddStaticParamValue(graph, "N", N);
//
//	// Configure vertices.
//	PiSDFVertex *vxC = PiSDFGraphAddConfigVertex(graph, /*Fct*/ 0, "C", PiSDFSub_Normal, /*In*/ 0, /*Out*/ 1, /*Par*/ 0, /*Cfg*/ 1);
////	PiSDFVertex_AddOutputParam(vxC, paramN);
//
//	// Other vertices
//	PiSDFVertex *vxA = PiSDFGraphAddNormalVertex(graph, /*Fct*/ 1, "A", PiSDFSub_Normal, /*In*/ 0, /*Out*/ 1, /*Par*/ 1);
//	PiSDFVertex_AddParam(vxA, paramN);
//
//	PiSDFVertex *vxB = PiSDFGraphAddNormalVertex(graph, /*Fct*/ 1, "B", PiSDFSub_Normal, /*In*/ 2, /*Out*/ 0, /*Par*/ 0);
//
//	// Edges.
//	PiSDFGraphAddEdge( /*Graph*/ graph,
//			/*Src*/ vxC, /*SrcPrt*/ 0, /*Prod*/ "1",
//			/*Snk*/ vxB, /*SnkPrt*/ 1, /*Cons*/ "1", /*Delay*/ 0);
//
//	PiSDFGraphAddEdge( /*Graph*/ graph,
//			/*Src*/ vxA, /*SrcPrt*/ 0, /*Prod*/ "N",
//			/*Snk*/ vxB, /*SnkPrt*/ 0, /*Cons*/ "1", /*Delay*/ 0);
//
//	// Timings
//	Parser_InitVariable(&vxC->timings[0], &vxC->params,  "10", &pisdfAlloc);
//	Parser_InitVariable(&vxA->timings[0], &vxA->params,  "10", pisdfAlloc);
//	Parser_InitVariable(&vxB->timings[0], &vxA->params,  "10", pisdfAlloc);
//
//	// Subgraphs
//
//	return graph;
//}
//
//PiSDFGraph* initPisdf_test2(Stack* stack, int N){
//	PiSDFGraph* top = PiSDFCreateGraph(0,0,0,0,0,1);
//
//	PiSDFVertex *vxTop = PiSDFGraphAddNormalVertex(top, -1, "top", PiSDFSub_Normal, 0, 0, 0);
//
//	vxTop->subGraph = test2(N);
//	vxTop->subGraph->parentVertex = vxTop;
//
//	return top;
//}
