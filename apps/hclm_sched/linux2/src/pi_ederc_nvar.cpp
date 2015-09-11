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

#include "ederc_nvar.h"

/*******************************************************************************/
/****************************     TEST 6     ***********************************/
/*******************************************************************************/

PiSDFGraph* ederc_nvar_sub(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
			/*Edges*/ 	7,
			/*Params*/	2,
			/*InIf*/	2,
			/*OutIf*/	1,
			/*Config*/	1,
			/*Normal*/	4,
			archi,
			stack);

	// Parameters.
	PiSDFParam* paramM = graph->addDynamicParam("M");
	PiSDFParam* paramNSamples = graph->addHeritedParam("NSamples", 0);

	// Configure vertices
	PiSDFVertex *vxSetM = graph->addConfigVertex(
			"SetM", /*Fct*/ 4,
			PISDF_SUBTYPE_NORMAL,
			/*In*/ 1,  /*Out*/ 0,
			/*Par*/ 0, /*Cfg*/ 1);
	vxSetM->addOutParam(0, paramM);

	// Interfaces
	PiSDFVertex *ifM = graph->addInputIf(
			"M",
			0 /*Par*/);

	PiSDFVertex *ifIn = graph->addInputIf(
			"in",
			1 /*Par*/);
	ifIn->addInParam(0, paramNSamples);

	PiSDFVertex *ifOut = graph->addOutputIf(
			"out",
			1 /*Par*/);
	ifOut->addInParam(0, paramNSamples);

	// Other vertices
	PiSDFVertex *vxInitFir = graph->addBodyVertex(
			"InitFir", /*Fct*/ 5,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 1);
	vxInitFir->addInParam(0, paramM);

	PiSDFVertex *vxFIR = graph->addBodyVertex(
			"FIR",
			/*Fct*/ 7,
			/*In*/ 2,
			/*Out*/ 1,
			/*Par*/ 1);
	vxFIR->addInParam(0, paramNSamples);

	PiSDFVertex *vxBr = graph->addSpecialVertex(
			PISDF_SUBTYPE_BROADCAST,
			/*In*/ 1, /*Out*/ 2,
			/*Par*/ 1);
	vxBr->addInParam(0, paramNSamples);

	// Edges.
	graph->connect(
			/*Src*/ ifM, 	/*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxSetM, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxBr, 	/*SrcPrt*/ 0, /*Prod*/ "4*NSamples",
			/*Snk*/ vxFIR, 	/*SnkPrt*/ 0, /*Cons*/ "4*NSamples",
			/*Delay*/ "4*NSamples", ifIn);

	graph->connect(
			/*Src*/ vxInitFir, 	/*SrcPrt*/ 0, /*Prod*/ "M",
			/*Snk*/ vxFIR, 		/*SnkPrt*/ 1, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxFIR, 	/*SrcPrt*/ 0, /*Prod*/ "4*NSamples",
			/*Snk*/ vxBr, 	/*SnkPrt*/ 0, /*Cons*/ "4*NSamples",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxBr, 	/*SrcPrt*/ 1, /*Prod*/ "4*NSamples",
			/*Snk*/ ifOut, 	/*SnkPrt*/ 0, /*Cons*/ "4*NSamples",
			/*Delay*/ "0", 0);

	// Timings
	vxSetM->isExecutableOnAllPE();
	vxSetM->setTimingOnType(0, "200", stack);
	vxInitFir->isExecutableOnAllPE();
	vxInitFir->setTimingOnType(0, "70", stack);
	vxFIR->isExecutableOnAllPE();
	vxFIR->setTimingOnType(0, "5501132/60", stack);
	vxBr->isExecutableOnAllPE();
	vxBr->setTimingOnType(0, "100", stack);

	return graph;
}

PiSDFGraph* ederc_nvar(Archi* archi, Stack* stack, int N, int Nmax, int NSamples, int test){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
			/*Edges*/ 	4,
			/*Params*/	5,
			/*InIf*/	0,
			/*OutIf*/	0,
			/*Config*/	1,
			/*Normal*/	4,
			archi,
			stack);

	// Parameters.
	PiSDFParam* paramNval = graph->addStaticParam("Nval", N);
	PiSDFParam* paramNmax = graph->addStaticParam("Nmax", Nmax);
	PiSDFParam* paramNSamples = graph->addStaticParam("NSamples", NSamples);
	PiSDFParam* paramTest = graph->addStaticParam("Test", test);
	PiSDFParam* paramN = graph->addDynamicParam("N");

	// Configure vertices
	PiSDFVertex *vxConfig = graph->addConfigVertex(
			"Config", /*Fct*/ 0,
			PISDF_SUBTYPE_NORMAL,
			/*In*/ 0,  /*Out*/ 1,
			/*Par*/ 2, /*Cfg*/ 1);
	vxConfig->addInParam(0, paramNmax);
	vxConfig->addInParam(1, paramNval);
	vxConfig->addOutParam(0, paramN);

	// Other vertices
	PiSDFVertex *vxMFilter = graph->addBodyVertex(
			"MFilter", /*Fct*/ 1,
			/*In*/ 1, /*Out*/ 1,
			/*Par*/ 2);
	vxMFilter->addInParam(0, paramNmax);
	vxMFilter->addInParam(1, paramN);

	PiSDFVertex *vxSrc = graph->addBodyVertex(
			"Src", /*Fct*/ 2,
			/*In*/ 0, /*Out*/ 1,
			/*Par*/ 3);
	vxSrc->addInParam(0, paramN);
	vxSrc->addInParam(1, paramNSamples);
	vxSrc->addInParam(2, paramTest);

	PiSDFVertex *vxSnk = graph->addBodyVertex(
			"Snk", /*Fct*/ 3,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 3);
	vxSnk->addInParam(0, paramN);
	vxSnk->addInParam(1, paramNSamples);
	vxSnk->addInParam(2, paramTest);

	PiSDFVertex *vxChannel = graph->addHierVertex(
			"Channel",
			/*SubGraph*/ ederc_nvar_sub(archi, stack),
			/*In*/ 2, /*Out*/ 1,
			/*Par*/ 1);
	vxChannel->addInParam(0, paramNSamples);

	// Edges.
	graph->connect(
			/*Src*/ vxConfig,	/*SrcPrt*/ 0, /*Prod*/ "Nmax",
			/*Snk*/ vxMFilter,	/*SnkPrt*/ 0, /*Cons*/ "Nmax",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxMFilter,	/*SrcPrt*/ 0, /*Prod*/ "N",
			/*Snk*/ vxChannel,	/*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxSrc,		/*SrcPrt*/ 0, /*Prod*/ "N*4*NSamples",
			/*Snk*/ vxChannel,	/*SnkPrt*/ 1, /*Cons*/ "4*NSamples",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxChannel,	/*SrcPrt*/ 0, /*Prod*/ "4*NSamples",
			/*Snk*/ vxSnk,		/*SnkPrt*/ 0, /*Cons*/ "N*4*NSamples",
			/*Delay*/ "0", 0);

	// Timings
	vxConfig->isExecutableOnAllPE();
	vxConfig->setTimingOnType(0, "210", stack);
	vxMFilter->isExecutableOnPE(0);
	vxMFilter->setTimingOnType(0, "80", stack);
	vxSrc->isExecutableOnPE(0);
	vxSrc->setTimingOnType(0, "70", stack);
	vxSnk->isExecutableOnPE(0);
	vxSnk->setTimingOnType(0, "70", stack);

	return graph;
}

PiSDFGraph* initPisdf_ederc_nvar(Archi* archi, Stack* stack, int N, int Nmax, int NSamples, int test){
	PiSDFGraph* top = CREATE(stack, PiSDFGraph)(
			0,0,0,0,0,1, archi, stack);

	top->addHierVertex(
			"top", ederc_nvar(archi, stack, N, Nmax, NSamples, test),
			0, 0, 0);

	return top;
}

void freePisdf_ederc_nvar(PiSDFGraph* top, Stack* stack){
	top->~PiSDFGraph();
	stack->free(top);
}
