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
#include "data_sp.h"
#include "pi_top_fft.h"

PiSDFGraph* top_fft(Archi* archi, Stack* stack);
PiSDFGraph* FFT(Archi* archi, Stack* stack);
PiSDFGraph* radixReduction(Archi* archi, Stack* stack);
PiSDFGraph* Switch(Archi* archi, Stack* stack);
PiSDFGraph* fftStep(Archi* archi, Stack* stack);

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
	PiSDFParam *param_fftSize = graph->addStaticParam("fftSize", N_DATA);

	/* Vertices */
	PiSDFVertex* bo_Src = graph->addBodyVertex(
		/*Name*/    "Src",
		/*FctId*/   SRC_FCT,
		/*InData*/  0,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_Src->addInParam(0, param_fftSize);
	bo_Src->isExecutableOnPE(CORE_CORE3);
	bo_Src->isExecutableOnPE(CORE_CORE0);
	bo_Src->isExecutableOnPE(CORE_CORE2);
	bo_Src->isExecutableOnPE(CORE_CORE1);
	bo_Src->setTimingOnType(CORE_TYPE_X86, "129170", stack);

	PiSDFVertex* bo_Snk = graph->addBodyVertex(
		/*Name*/    "Snk",
		/*FctId*/   SNK_FCT,
		/*InData*/  1,
		/*OutData*/ 0,
		/*InParam*/ 1);
	bo_Snk->addInParam(0, param_fftSize);
	bo_Snk->isExecutableOnPE(CORE_CORE3);
	bo_Snk->isExecutableOnPE(CORE_CORE0);
	bo_Snk->isExecutableOnPE(CORE_CORE2);
	bo_Snk->isExecutableOnPE(CORE_CORE1);
	bo_Snk->setTimingOnType(CORE_TYPE_X86, "2213630", stack);

	PiSDFVertex* bo_FFT = graph->addHierVertex(
		/*Name*/    "FFT",
		/*Graph*/   FFT(archi, stack),
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_FFT->addInParam(0, param_fftSize);


	/* Edges */
	graph->connect(
		/*Src*/ bo_Src, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize)",
		/*Snk*/ bo_FFT, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_FFT, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize)",
		/*Snk*/ bo_Snk, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize)",
		/*Delay*/ "0",0);

	return graph;
}

// Method building PiSDFGraphFFT
PiSDFGraph* FFT(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
		/*Edges*/    4,
		/*Params*/   2,
		/*InputIf*/  1,
		/*OutputIf*/ 1,
		/*Config*/   1,
		/*Body*/     3,
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
		/*FctId*/   CONFIGFFT_FCT,
		/*SubType*/ PISDF_SUBTYPE_NORMAL,
		/*InData*/  0,
		/*OutData*/ 0,
		/*InParam*/ 1,
		/*OutParam*/1);
	cf_configFft->addOutParam(0, param_NStep);
	cf_configFft->addInParam(0, param_fftSize);
	cf_configFft->isExecutableOnPE(CORE_CORE3);
	cf_configFft->isExecutableOnPE(CORE_CORE0);
	cf_configFft->isExecutableOnPE(CORE_CORE2);
	cf_configFft->isExecutableOnPE(CORE_CORE1);
	cf_configFft->setTimingOnType(CORE_TYPE_X86, "4070", stack);

	PiSDFVertex* bo_ordering = graph->addBodyVertex(
		/*Name*/    "ordering",
		/*FctId*/   ORDERING_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 2);
	bo_ordering->addInParam(0, param_fftSize);
	bo_ordering->addInParam(1, param_NStep);
	bo_ordering->isExecutableOnPE(CORE_CORE3);
	bo_ordering->isExecutableOnPE(CORE_CORE0);
	bo_ordering->isExecutableOnPE(CORE_CORE2);
	bo_ordering->isExecutableOnPE(CORE_CORE1);
	bo_ordering->setTimingOnType(CORE_TYPE_X86, "3220086", stack);

	PiSDFVertex* bo_monoFFT = graph->addBodyVertex(
		/*Name*/    "monoFFT",
		/*FctId*/   MONOFFT_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 2);
	bo_monoFFT->addInParam(0, param_NStep);
	bo_monoFFT->addInParam(1, param_fftSize);
	bo_monoFFT->isExecutableOnPE(CORE_CORE3);
	bo_monoFFT->isExecutableOnPE(CORE_CORE0);
	bo_monoFFT->isExecutableOnPE(CORE_CORE2);
	bo_monoFFT->isExecutableOnPE(CORE_CORE1);
	bo_monoFFT->setTimingOnType(CORE_TYPE_X86, "2*6155121/(2^NStep)", stack);

	PiSDFVertex* bo_radixReduction = graph->addHierVertex(
		/*Name*/    "radixReduction",
		/*Graph*/   radixReduction(archi, stack),
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 2);
	bo_radixReduction->addInParam(0, param_NStep);
	bo_radixReduction->addInParam(1, param_fftSize);


	/* Edges */
	graph->connect(
		/*Src*/ if_in, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize)",
		/*Snk*/ bo_ordering, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_ordering, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize)",
		/*Snk*/ bo_monoFFT, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize/(2^NStep))",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_monoFFT, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize/(2^NStep))",
		/*Snk*/ bo_radixReduction, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_radixReduction, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize)",
		/*Snk*/ if_out, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize)",
		/*Delay*/ "0",0);

	return graph;
}

// Method building PiSDFGraphradixReduction
PiSDFGraph* radixReduction(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
		/*Edges*/    7,
		/*Params*/   2,
		/*InputIf*/  1,
		/*OutputIf*/ 1,
		/*Config*/   0,
		/*Body*/     4,
		/*Archi*/    archi,
		/*Stack*/    stack);

	/* Parameters */
	PiSDFParam *param_NStep = graph->addHeritedParam("NStep", 0);
	PiSDFParam *param_fftSize = graph->addHeritedParam("fftSize", 1);

	/* Vertices */
	PiSDFVertex* if_in = graph->addInputIf(
		/*Name*/    "if_in",
		/*InParam*/ 1);
	if_in->addInParam(0, param_fftSize);

	PiSDFVertex* if_out = graph->addOutputIf(
		/*Name*/    "if_out",
		/*InParam*/ 1);
	if_out->addInParam(0, param_fftSize);

	PiSDFVertex* bo_GenSwitchSel = graph->addBodyVertex(
		/*Name*/    "GenSwitchSel",
		/*FctId*/   GENSWITCHSEL_FCT,
		/*InData*/  0,
		/*OutData*/ 2,
		/*InParam*/ 1);
	bo_GenSwitchSel->addInParam(0, param_NStep);
	bo_GenSwitchSel->isExecutableOnPE(CORE_CORE3);
	bo_GenSwitchSel->isExecutableOnPE(CORE_CORE0);
	bo_GenSwitchSel->isExecutableOnPE(CORE_CORE2);
	bo_GenSwitchSel->isExecutableOnPE(CORE_CORE1);
	bo_GenSwitchSel->setTimingOnType(CORE_TYPE_X86, "1702", stack);

	PiSDFVertex* bo_Br = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_BROADCAST,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 1);
	bo_Br->addInParam(0, param_fftSize);

	PiSDFVertex* bo_Switch = graph->addHierVertex(
		/*Name*/    "Switch",
		/*Graph*/   Switch(archi, stack),
		/*InData*/  3,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_Switch->addInParam(0, param_fftSize);

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
		/*Src*/ bo_GenSwitchSel, /*SrcPrt*/ 1, /*Prod*/ "1*(NStep)",
		/*Snk*/ bo_Switch, /*SnkPrt*/ 2, /*Cons*/ "1*(1)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ if_in, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize)",
		/*Snk*/ bo_Switch, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br, /*SrcPrt*/ 1, /*Prod*/ "8*(fftSize)",
		/*Snk*/ bo_Switch, /*SnkPrt*/ 1, /*Cons*/ "8*(fftSize)",
		/*Delay*/ "8*(fftSize)",0);

	graph->connect(
		/*Src*/ bo_Switch, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize)",
		/*Snk*/ bo_fftStep, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_fftStep, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize)",
		/*Snk*/ bo_Br, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize)",
		/*Snk*/ if_out, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_GenSwitchSel, /*SrcPrt*/ 0, /*Prod*/ "1*(NStep)",
		/*Snk*/ bo_fftStep, /*SnkPrt*/ 1, /*Cons*/ "1*(1)",
		/*Delay*/ "0",0);

	return graph;
}

// Method building PiSDFGraphSwitch
PiSDFGraph* Switch(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
		/*Edges*/    8,
		/*Params*/   2,
		/*InputIf*/  3,
		/*OutputIf*/ 1,
		/*Config*/   1,
		/*Body*/     5,
		/*Archi*/    archi,
		/*Stack*/    stack);

	/* Parameters */
	PiSDFParam *param_sel = graph->addDynamicParam("sel");
	PiSDFParam *param_fftSize = graph->addHeritedParam("fftSize", 0);

	/* Vertices */
	PiSDFVertex* if_in0 = graph->addInputIf(
		/*Name*/    "if_in0",
		/*InParam*/ 1);
	if_in0->addInParam(0, param_fftSize);

	PiSDFVertex* if_in1 = graph->addInputIf(
		/*Name*/    "if_in1",
		/*InParam*/ 1);
	if_in1->addInParam(0, param_fftSize);

	PiSDFVertex* if_sel_in = graph->addInputIf(
		/*Name*/    "if_sel_in",
		/*InParam*/ 0);

	PiSDFVertex* if_out = graph->addOutputIf(
		/*Name*/    "if_out",
		/*InParam*/ 1);
	if_out->addInParam(0, param_fftSize);

	PiSDFVertex* cf_selcfg = graph->addConfigVertex(
		/*Name*/    "selcfg",
		/*FctId*/   SELCFG_FCT,
		/*SubType*/ PISDF_SUBTYPE_NORMAL,
		/*InData*/  1,
		/*OutData*/ 0,
		/*InParam*/ 0,
		/*OutParam*/1);
	cf_selcfg->addOutParam(0, param_sel);
	cf_selcfg->isExecutableOnPE(CORE_CORE3);
	cf_selcfg->isExecutableOnPE(CORE_CORE0);
	cf_selcfg->isExecutableOnPE(CORE_CORE2);
	cf_selcfg->isExecutableOnPE(CORE_CORE1);
	cf_selcfg->setTimingOnType(CORE_TYPE_X86, "229", stack);

	PiSDFVertex* bo_f0 = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_FORK,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 2);
	bo_f0->addInParam(0, param_sel);
	bo_f0->addInParam(1, param_fftSize);

	PiSDFVertex* bo_f1 = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_FORK,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 2);
	bo_f1->addInParam(0, param_sel);
	bo_f1->addInParam(1, param_fftSize);

	PiSDFVertex* bo_j = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_JOIN,
		/*InData*/  2,
		/*OutData*/ 1,
		/*InParam*/ 2);
	bo_j->addInParam(0, param_sel);
	bo_j->addInParam(1, param_fftSize);

	PiSDFVertex* bo_end0 = graph->addSpecialVertex(
		PISDF_SUBTYPE_END,
		/*InData*/  1,
		/*OutData*/ 0,
		/*InParam*/ 2);
	bo_end0->addInParam(0, param_sel);
	bo_end0->addInParam(1, param_fftSize);

	PiSDFVertex* bo_end1 = graph->addSpecialVertex(
			PISDF_SUBTYPE_END,
		/*InData*/  1,
		/*OutData*/ 0,
		/*InParam*/ 2);
	bo_end1->addInParam(0, param_sel);
	bo_end1->addInParam(1, param_fftSize);

	/* Edges */
	graph->connect(
		/*Src*/ if_sel_in, /*SrcPrt*/ 0, /*Prod*/ "1*(1)",
		/*Snk*/ cf_selcfg, /*SnkPrt*/ 0, /*Cons*/ "1*(1)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ if_in0, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize)",
		/*Snk*/ bo_f0, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ if_in1, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize)",
		/*Snk*/ bo_f1, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_f0, /*SrcPrt*/ 1, /*Prod*/ "8*(fftSize*(1-sel))",
		/*Snk*/ bo_j, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize*(1-sel))",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_f1, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize*sel)",
		/*Snk*/ bo_j, /*SnkPrt*/ 1, /*Cons*/ "8*(fftSize*sel)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_f1, /*SrcPrt*/ 1, /*Prod*/ "8*(fftSize*(1-sel))",
		/*Snk*/ bo_end1, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize*(1-sel))",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_f0, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize*sel)",
		/*Snk*/ bo_end0, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize*sel)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_j, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize)",
		/*Snk*/ if_out, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize)",
		/*Delay*/ "0",0);

	return graph;
}

// Method building PiSDFGraphfftStep
PiSDFGraph* fftStep(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
		/*Edges*/    8,
		/*Params*/   3,
		/*InputIf*/  2,
		/*OutputIf*/ 1,
		/*Config*/   1,
		/*Body*/     4,
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
		/*FctId*/   CFGFFTSTEP_FCT,
		/*SubType*/ PISDF_SUBTYPE_NORMAL,
		/*InData*/  1,
		/*OutData*/ 0,
		/*InParam*/ 0,
		/*OutParam*/1);
	cf_cfgFftStep->addOutParam(0, param_Step);
	cf_cfgFftStep->isExecutableOnPE(CORE_CORE3);
	cf_cfgFftStep->isExecutableOnPE(CORE_CORE0);
	cf_cfgFftStep->isExecutableOnPE(CORE_CORE2);
	cf_cfgFftStep->isExecutableOnPE(CORE_CORE1);
	cf_cfgFftStep->setTimingOnType(CORE_TYPE_X86, "85", stack);

	PiSDFVertex* bo_ForkFftStep = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_FORK,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 3);
	bo_ForkFftStep->addInParam(0, param_NStep);
	bo_ForkFftStep->addInParam(1, param_Step);
	bo_ForkFftStep->addInParam(2, param_fftSize);

	PiSDFVertex* bo_JoinFftStep = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_JOIN,
		/*InData*/  2,
		/*OutData*/ 1,
		/*InParam*/ 3);
	bo_JoinFftStep->addInParam(0, param_fftSize);
	bo_JoinFftStep->addInParam(1, param_NStep);
	bo_JoinFftStep->addInParam(2, param_Step);

	PiSDFVertex* bo_fft_radix2 = graph->addBodyVertex(
		/*Name*/    "fft_radix2",
		/*FctId*/   FFT_RADIX2_FCT,
		/*InData*/  3,
		/*OutData*/ 2,
		/*InParam*/ 3);
	bo_fft_radix2->addInParam(0, param_NStep);
	bo_fft_radix2->addInParam(1, param_fftSize);
	bo_fft_radix2->addInParam(2, param_Step);
	bo_fft_radix2->isExecutableOnPE(CORE_CORE3);
	bo_fft_radix2->isExecutableOnPE(CORE_CORE0);
	bo_fft_radix2->isExecutableOnPE(CORE_CORE2);
	bo_fft_radix2->isExecutableOnPE(CORE_CORE1);
	bo_fft_radix2->setTimingOnType(CORE_TYPE_X86, "3524847/(NStep*(2^NStep))", stack);

	PiSDFVertex* bo_genIx = graph->addBodyVertex(
		/*Name*/    "genIx",
		/*FctId*/   GENIX_FCT,
		/*InData*/  0,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_genIx->addInParam(0, param_NStep);
	bo_genIx->isExecutableOnPE(CORE_CORE3);
	bo_genIx->isExecutableOnPE(CORE_CORE0);
	bo_genIx->isExecutableOnPE(CORE_CORE2);
	bo_genIx->isExecutableOnPE(CORE_CORE1);
	bo_genIx->setTimingOnType(CORE_TYPE_X86, "192", stack);


	/* Edges */
	graph->connect(
		/*Src*/ if_step, /*SrcPrt*/ 0, /*Prod*/ "1*(1)",
		/*Snk*/ cf_cfgFftStep, /*SnkPrt*/ 0, /*Cons*/ "1*(1)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ if_in, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize)",
		/*Snk*/ bo_ForkFftStep, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize/(2^(NStep-Step-1)))",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_ForkFftStep, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize/(2^(NStep-Step)))",
		/*Snk*/ bo_fft_radix2, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize/(2^NStep))",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_ForkFftStep, /*SrcPrt*/ 1, /*Prod*/ "8*(fftSize/(2^(NStep-Step)))",
		/*Snk*/ bo_fft_radix2, /*SnkPrt*/ 1, /*Cons*/ "8*(fftSize/(2^NStep))",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_fft_radix2, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize/(2^NStep))",
		/*Snk*/ bo_JoinFftStep, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize/(2^(NStep-Step)))",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_fft_radix2, /*SrcPrt*/ 1, /*Prod*/ "8*(fftSize/(2^NStep))",
		/*Snk*/ bo_JoinFftStep, /*SnkPrt*/ 1, /*Cons*/ "8*(fftSize/(2^(NStep-Step)))",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_JoinFftStep, /*SrcPrt*/ 0, /*Prod*/ "8*(fftSize/(2^(NStep-Step-1)))",
		/*Snk*/ if_out, /*SnkPrt*/ 0, /*Cons*/ "8*(fftSize)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_genIx, /*SrcPrt*/ 0, /*Prod*/ "1*(2^(NStep-1))",
		/*Snk*/ bo_fft_radix2, /*SnkPrt*/ 2, /*Cons*/ "1*(1)",
		/*Delay*/ "0",0);

	return graph;
}

void free_top_fft(PiSDFGraph* top, Stack* stack){
	top->~PiSDFGraph();
	stack->free(top);
}
