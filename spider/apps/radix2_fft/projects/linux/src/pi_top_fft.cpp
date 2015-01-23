/**
 * *****************************************************************************
 * Copyright or © or Copr. IETR/INSA: Maxime Pelcat, Jean-François Nezan,
 * Karol Desnos, Julien Heulot, Clément Guy, Yaset Oliva Venegas
 *
 * [mpelcat,jnezan,kdesnos,jheulot,cguy,yoliva]@insa-rennes.fr
 *
 * This software is a computer program whose purpose is to prototype
 * parallel applications.
 *
 * This software is governed by the CeCILL-C license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL-C
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-C license and that you accept its terms.
 * ****************************************************************************
 */

#include <spider.h>

typedef enum{
	CORE_CORE0 = 0,
} PE;

typedef enum{
	CORE_TYPE_X86 = 0,
} PEType;

PiSDFGraph* top_fft(Archi* archi, Stack* stack);
PiSDFGraph* FFT(Archi* archi, Stack* stack);
PiSDFGraph* fftStep(Archi* archi, Stack* stack);


PiSDFGraph* switch_sub(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
			/*Edges*/ 	8,
			/*Params*/	2,
			/*InIf*/	3,
			/*OutIf*/	1,
			/*Config*/	1,
			/*Normal*/	5,
			archi,
			stack);

	// Parameters.
	PiSDFParam* paramSel = graph->addDynamicParam("Sel");
	PiSDFParam* paramNSamples = graph->addHeritedParam("NSamples", 0);

	// Configure vertices
	PiSDFVertex *vxSelCfg = graph->addConfigVertex(
			"SelCfg", /*Fct*/ 7,
			PISDF_SUBTYPE_NORMAL,
			/*In*/ 1,  /*Out*/ 0,
			/*Par*/ 0, /*Cfg*/ 1);
	vxSelCfg->addOutParam(0, paramSel);

	// Interfaces
	PiSDFVertex *ifSel = graph->addInputIf(
			"Sel",
			0 /*Par*/);

	PiSDFVertex *ifIn0 = graph->addInputIf(
			"In0",
			1 /*Par*/);
	ifIn0->addInParam(0, paramNSamples);

	PiSDFVertex *ifIn1 = graph->addInputIf(
			"In1",
			1 /*Par*/);
	ifIn1->addInParam(0, paramNSamples);

	PiSDFVertex *ifOut = graph->addOutputIf(
			"out",
			1 /*Par*/);
	ifOut->addInParam(0, paramNSamples);

	// Other vertices
	PiSDFVertex *vxF0 = graph->addSpecialVertex(
			PISDF_SUBTYPE_FORK,
			/*In*/ 1, /*Out*/ 2,
			/*Par*/ 2);
	vxF0->addInParam(0, paramSel);
	vxF0->addInParam(1, paramNSamples);

	PiSDFVertex *vxF1 = graph->addSpecialVertex(
			PISDF_SUBTYPE_FORK,
			/*In*/ 1, /*Out*/ 2,
			/*Par*/ 2);
	vxF1->addInParam(0, paramSel);
	vxF1->addInParam(1, paramNSamples);

	PiSDFVertex *vxJ = graph->addSpecialVertex(
			PISDF_SUBTYPE_JOIN,
			/*In*/ 2, /*Out*/ 1,
			/*Par*/ 2);
	vxJ->addInParam(0, paramSel);
	vxJ->addInParam(1, paramNSamples);

	PiSDFVertex *vxE0 = graph->addSpecialVertex(
			PISDF_SUBTYPE_END,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 2);
	vxE0->addInParam(0, paramSel);
	vxE0->addInParam(1, paramNSamples);

	PiSDFVertex *vxE1 = graph->addSpecialVertex(
			PISDF_SUBTYPE_END,
			/*In*/ 1, /*Out*/ 0,
			/*Par*/ 2);
	vxE1->addInParam(0, paramSel);
	vxE1->addInParam(1, paramNSamples);

	// Edges.
	graph->connect(
			/*Src*/ ifSel, 	/*SrcPrt*/ 0, /*Prod*/ "1",
			/*Snk*/ vxSelCfg, /*SnkPrt*/ 0, /*Cons*/ "1",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ ifIn0,	/*SrcPrt*/ 0, /*Prod*/ "NSamples",
			/*Snk*/ vxF0,	/*SnkPrt*/ 0, /*Cons*/ "NSamples",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ ifIn1,	/*SrcPrt*/ 0, /*Prod*/ "NSamples",
			/*Snk*/ vxF1,	/*SnkPrt*/ 0, /*Cons*/ "NSamples",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxF0,	/*SrcPrt*/ 0, /*Prod*/ "NSamples*Sel",
			/*Snk*/ vxE0, 	/*SnkPrt*/ 0, /*Cons*/ "NSamples*Sel",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxF0,	/*SrcPrt*/ 1, /*Prod*/ "(1-Sel)*NSamples",
			/*Snk*/ vxJ, 	/*SnkPrt*/ 0, /*Cons*/ "(1-Sel)*NSamples",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxF1,	/*SrcPrt*/ 0, /*Prod*/ "(1-Sel)*NSamples",
			/*Snk*/ vxE1, 	/*SnkPrt*/ 0, /*Cons*/ "(1-Sel)*NSamples",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxF1,	/*SrcPrt*/ 1, /*Prod*/ "NSamples*Sel",
			/*Snk*/ vxJ, 	/*SnkPrt*/ 1, /*Cons*/ "NSamples*Sel",
			/*Delay*/ "0", 0);

	graph->connect(
			/*Src*/ vxJ, 	/*SrcPrt*/ 0, /*Prod*/ "NSamples",
			/*Snk*/ ifOut, 	/*SnkPrt*/ 0, /*Cons*/ "NSamples",
			/*Delay*/ "0", 0);

	vxSelCfg->isExecutableOnAllPE();
	vxSelCfg->setTimingOnType(0, "200", stack);
	return graph;
}


/**
 * This is the method you need to call to build a complete PiSDF graph.
 */
PiSDFGraph* init_top_fft(Archi* archi, Stack* stack){
	PiSDFGraph* top = CREATE(stack, PiSDFGraph)(
		/*Edges*/    0,
		/*Params*/   0,
		/*InputIf*/  0,
		/*OutputIf*/ 0,
		/*Config*/   0,
		/*Body*/     1,
		/*Archi*/    archi,
		/*Stack*/    stack);

	top->addHierVertex(
		/*Name*/     "top",
		/*Graph*/    top_fft(archi, stack),
		/*InputIf*/  0,
		/*OutputIf*/ 0,
		/*Params*/   0);

	return top;
}

// Method building PiSDFGraphtop_fft
PiSDFGraph* top_fft(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
		/*Edges*/    2,
		/*Params*/   1,
		/*InputIf*/  0,
		/*OutputIf*/ 0,
		/*Config*/   0,
		/*Body*/     3,
		/*Archi*/    archi,
		/*Stack*/    stack);

	/* Parameters */
	PiSDFParam *param_fftSize = graph->addStaticParam("fftSize", 8);

	/* Vertices */
	PiSDFVertex* bo_Src = graph->addBodyVertex(
		/*Name*/    "Src",
		/*FctId*/   0,
		/*InData*/  0,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_Src->addInParam(0, param_fftSize);
	bo_Src->isExecutableOnPE(CORE_CORE0);
	bo_Src->setTimingOnType(0, "100", stack);

	PiSDFVertex* bo_Snk = graph->addBodyVertex(
		/*Name*/    "Snk",
		/*FctId*/   1,
		/*InData*/  1,
		/*OutData*/ 0,
		/*InParam*/ 1);
	bo_Snk->addInParam(0, param_fftSize);
	bo_Snk->isExecutableOnPE(CORE_CORE0);
	bo_Snk->setTimingOnType(0, "100", stack);

	PiSDFVertex* bo_FFT = graph->addHierVertex(
		/*Name*/    "FFT",
		/*Graph*/   FFT(archi, stack),
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_FFT->addInParam(0, param_fftSize);


	/* Edges */
	graph->connect(
		/*Src*/ bo_Src, /*SrcPrt*/ 0, /*Prod*/ "fftSize",
		/*Snk*/ bo_FFT, /*SnkPrt*/ 0, /*Cons*/ "fftSize",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_FFT, /*SrcPrt*/ 0, /*Prod*/ "fftSize",
		/*Snk*/ bo_Snk, /*SnkPrt*/ 0, /*Cons*/ "fftSize",
		/*Delay*/ "0",0);

	return graph;
}

// Method building PiSDFGraphFFT
PiSDFGraph* FFT(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
		/*Edges*/    7,
		/*Params*/   2,
		/*InputIf*/  1,
		/*OutputIf*/ 1,
		/*Config*/   1,
		/*Body*/     4,
		/*Archi*/    archi,
		/*Stack*/    stack);

	/* Parameters */
	PiSDFParam *param_fftSize = graph->addHeritedParam("fftSize", 0);
	PiSDFParam *param_NStep = graph->addDynamicParam("NStep");

	/* Vertices */
	PiSDFVertex* if_in = graph->addInputIf(
		/*Name*/    "if_in",
		/*InParam*/ 1);
	if_in->addInParam(0, param_fftSize);

	PiSDFVertex* if_out = graph->addOutputIf(
		/*Name*/    "if_out",
		/*InParam*/ 1);
	if_out->addInParam(0, param_fftSize);

	PiSDFVertex* cf_configFft = graph->addConfigVertex(
		/*Name*/    "configFft",
		/*FctId*/   6,
		/*SubType*/ PISDF_SUBTYPE_NORMAL,
		/*InData*/  0,
		/*OutData*/ 0,
		/*InParam*/ 1,
		/*OutParam*/1);
	cf_configFft->addOutParam(0, param_NStep);
	cf_configFft->addInParam(0, param_fftSize);
	cf_configFft->isExecutableOnPE(CORE_CORE0);
	cf_configFft->setTimingOnType(0, "100", stack);

//	PiSDFVertex* bo_Switch = graph->addBodyVertex(
//		/*Name*/    "Switch",
//		/*FctId*/   5,
//		/*InData*/  3,
//		/*OutData*/ 1,
//		/*InParam*/ 1);
	PiSDFVertex *bo_Switch = graph->addHierVertex(
			"Switch", switch_sub(archi, stack),
			/*In*/ 3,
			/*Out*/ 1,
			/*Par*/ 1);
	bo_Switch->addInParam(0, param_fftSize);
	bo_Switch->isExecutableOnPE(CORE_CORE0);
	bo_Switch->setTimingOnType(0, "100", stack);

	PiSDFVertex* bo_BrFftStep = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_BROADCAST,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 1);
	bo_BrFftStep->addInParam(0, param_fftSize);

	PiSDFVertex* bo_GenStepSwitch = graph->addBodyVertex(
		/*Name*/    "GenStepSwitch",
		/*FctId*/   2,
		/*InData*/  0,
		/*OutData*/ 2,
		/*InParam*/ 1);
	bo_GenStepSwitch->addInParam(0, param_NStep);
	bo_GenStepSwitch->isExecutableOnPE(CORE_CORE0);
	bo_GenStepSwitch->setTimingOnType(0, "100", stack);

	PiSDFVertex* bo_fftStep = graph->addHierVertex(
		/*Name*/    "fftStep",
		/*Graph*/   fftStep(archi, stack),
		/*InData*/  2,
		/*OutData*/ 1,
		/*InParam*/ 2);
	bo_fftStep->addInParam(0, param_fftSize);
	bo_fftStep->addInParam(1, param_NStep);


	/* Edges */
	graph->connect(
		/*Src*/ if_in, /*SrcPrt*/ 0, /*Prod*/ "fftSize",
		/*Snk*/ bo_Switch, /*SnkPrt*/ 1, /*Cons*/ "fftSize",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Switch, /*SrcPrt*/ 0, /*Prod*/ "fftSize",
		/*Snk*/ bo_fftStep, /*SnkPrt*/ 0, /*Cons*/ "fftSize",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_fftStep, /*SrcPrt*/ 0, /*Prod*/ "fftSize",
		/*Snk*/ bo_BrFftStep, /*SnkPrt*/ 0, /*Cons*/ "fftSize",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_BrFftStep, /*SrcPrt*/ 0, /*Prod*/ "fftSize",
		/*Snk*/ if_out, /*SnkPrt*/ 0, /*Cons*/ "fftSize",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_BrFftStep, /*SrcPrt*/ 1, /*Prod*/ "fftSize",
		/*Snk*/ bo_Switch, /*SnkPrt*/ 2, /*Cons*/ "fftSize",
		/*Delay*/ "fftSize",0);

	graph->connect(
		/*Src*/ bo_GenStepSwitch, /*SrcPrt*/ 0, /*Prod*/ "NStep",
		/*Snk*/ bo_fftStep, /*SnkPrt*/ 1, /*Cons*/ "1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_GenStepSwitch, /*SrcPrt*/ 1, /*Prod*/ "NStep",
		/*Snk*/ bo_Switch, /*SnkPrt*/ 0, /*Cons*/ "1",
		/*Delay*/ "0",0);

	return graph;
}

// Method building PiSDFGraphfftStep
PiSDFGraph* fftStep(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
		/*Edges*/    7,
		/*Params*/   3,
		/*InputIf*/  2,
		/*OutputIf*/ 1,
		/*Config*/   1,
		/*Body*/     3,
		/*Archi*/    archi,
		/*Stack*/    stack);

	/* Parameters */
	PiSDFParam *param_fftSize = graph->addHeritedParam("fftSize", 0);
	PiSDFParam *param_NStep = graph->addHeritedParam("NStep", 1);
	PiSDFParam *param_Step = graph->addDynamicParam("Step");

	/* Vertices */
	PiSDFVertex* if_in = graph->addInputIf(
		/*Name*/    "if_in",
		/*InParam*/ 1);
	if_in->addInParam(0, param_fftSize);

	PiSDFVertex* if_step = graph->addInputIf(
		/*Name*/    "if_step",
		/*InParam*/ 0);

	PiSDFVertex* if_out = graph->addOutputIf(
		/*Name*/    "if_out",
		/*InParam*/ 1);
	if_out->addInParam(0, param_fftSize);

	PiSDFVertex* cf_cfgFftStep = graph->addConfigVertex(
		/*Name*/    "cfgFftStep",
		/*FctId*/   3,
		/*SubType*/ PISDF_SUBTYPE_NORMAL,
		/*InData*/  1,
		/*OutData*/ 0,
		/*InParam*/ 0,
		/*OutParam*/1);
	cf_cfgFftStep->addOutParam(0, param_Step);
	cf_cfgFftStep->isExecutableOnPE(CORE_CORE0);
	cf_cfgFftStep->setTimingOnType(0, "100", stack);

	PiSDFVertex* bo_ForkFftStep = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_FORK,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 3);
	bo_ForkFftStep->addInParam(0, param_Step);
	bo_ForkFftStep->addInParam(1, param_fftSize);
	bo_ForkFftStep->addInParam(2, param_NStep);

	PiSDFVertex* bo_JoinFftStep = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_JOIN,
		/*InData*/  2,
		/*OutData*/ 1,
		/*InParam*/ 2);
	bo_JoinFftStep->addInParam(0, param_fftSize);
	bo_JoinFftStep->addInParam(1, param_NStep);

	PiSDFVertex* bo_fft_radix2 = graph->addBodyVertex(
		/*Name*/    "fft_radix2",
		/*FctId*/   4,
		/*InData*/  2,
		/*OutData*/ 2,
		/*InParam*/ 2);
	bo_fft_radix2->addInParam(0, param_NStep);
	bo_fft_radix2->addInParam(1, param_fftSize);
	bo_fft_radix2->isExecutableOnPE(CORE_CORE0);
	bo_fft_radix2->setTimingOnType(0, "100", stack);


	/* Edges */
	graph->connect(
		/*Src*/ if_step, /*SrcPrt*/ 0, /*Prod*/ "1",
		/*Snk*/ cf_cfgFftStep, /*SnkPrt*/ 0, /*Cons*/ "1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ if_in, /*SrcPrt*/ 0, /*Prod*/ "fftSize",
		/*Snk*/ bo_ForkFftStep, /*SnkPrt*/ 0, /*Cons*/ "fftSize/(2^(NStep-Step-1))",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_ForkFftStep, /*SrcPrt*/ 0, /*Prod*/ "fftSize/(2^(NStep-Step))",
		/*Snk*/ bo_fft_radix2, /*SnkPrt*/ 0, /*Cons*/ "fftSize/(2^NStep)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_ForkFftStep, /*SrcPrt*/ 1, /*Prod*/ "fftSize/(2^(NStep-Step))",
		/*Snk*/ bo_fft_radix2, /*SnkPrt*/ 1, /*Cons*/ "fftSize/(2^NStep)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_fft_radix2, /*SrcPrt*/ 0, /*Prod*/ "fftSize/(2^NStep)",
		/*Snk*/ bo_JoinFftStep, /*SnkPrt*/ 0, /*Cons*/ "fftSize/(2^NStep)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_fft_radix2, /*SrcPrt*/ 1, /*Prod*/ "fftSize/(2^NStep)",
		/*Snk*/ bo_JoinFftStep, /*SnkPrt*/ 1, /*Cons*/ "fftSize/(2^NStep)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_JoinFftStep, /*SrcPrt*/ 0, /*Prod*/ "fftSize/(2^(NStep-1))",
		/*Snk*/ if_out, /*SnkPrt*/ 0, /*Cons*/ "fftSize",
		/*Delay*/ "0",0);

	return graph;
}
