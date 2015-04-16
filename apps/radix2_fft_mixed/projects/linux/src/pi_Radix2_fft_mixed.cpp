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

#include "Radix2_fft_mixed.h"

PiSDFGraph* Radix2_fft_mixed(Archi* archi, Stack* stack);
PiSDFGraph* DFT_Radix2(Archi* archi, Stack* stack);
PiSDFGraph* Radix2_Stage(Archi* archi, Stack* stack);

/**
 * This is the method you need to call to build a complete PiSDF graph.
 */
PiSDFGraph* init_Radix2_fft_mixed(Archi* archi, Stack* stack){
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
		/*Graph*/    Radix2_fft_mixed(archi, stack),
		/*InputIf*/  0,
		/*OutputIf*/ 0,
		/*Params*/   0);

	return top;
}

// Method building PiSDFGraphRadix2_fft_mixed
PiSDFGraph* Radix2_fft_mixed(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
		/*Edges*/    4,
		/*Params*/   6,
		/*InputIf*/  0,
		/*OutputIf*/ 0,
		/*Config*/   0,
		/*Body*/     5,
		/*Archi*/    archi,
		/*Stack*/    stack);

	/* Parameters */
	PiSDFParam *param_fftSize = graph->addStaticParam("fftSize", 64*1024);
	PiSDFParam *param_P = graph->addStaticParam("P", 1);
	PiSDFParam *param_n1 = graph->addStaticParam("n1", 32*1024);
	PiSDFParam *param_n2 = graph->addStaticParam("n2", 1);
	PiSDFParam *param_N1 = graph->addDependentParam("N1", "2^P");
	PiSDFParam *param_N2 = graph->addDependentParam("N2", "fftSize/2^P");

	/* Vertices */
	PiSDFVertex* bo_src = graph->addBodyVertex(
		/*Name*/    "src",
		/*FctId*/   RADIX2_FFT_MIXED_SRC_FCT,
		/*InData*/  0,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_src->addInParam(0, param_fftSize);
	bo_src->isExecutableOnPE(CORE_ARM0);
	bo_src->isExecutableOnPE(CORE_ARM1);
	bo_src->isExecutableOnPE(CORE_DSP0);
	bo_src->isExecutableOnPE(CORE_DSP3);
	bo_src->isExecutableOnPE(CORE_DSP1);
	bo_src->isExecutableOnPE(CORE_DSP2);
	bo_src->setTimingOnType(CORE_TYPE_CORTEXA15, "1000", stack);

	PiSDFVertex* bo_T = graph->addBodyVertex(
		/*Name*/    "T",
		/*FctId*/   RADIX2_FFT_MIXED_T_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 2);
	bo_T->addInParam(0, param_N1);
	bo_T->addInParam(1, param_N2);
	bo_T->isExecutableOnPE(CORE_ARM0);
	bo_T->isExecutableOnPE(CORE_ARM1);
	bo_T->isExecutableOnPE(CORE_DSP0);
	bo_T->isExecutableOnPE(CORE_DSP3);
	bo_T->isExecutableOnPE(CORE_DSP1);
	bo_T->isExecutableOnPE(CORE_DSP2);
	bo_T->setTimingOnType(CORE_TYPE_CORTEXA15, "1000", stack);

	PiSDFVertex* bo_DFT_N2 = graph->addBodyVertex(
		/*Name*/    "DFT_N2",
		/*FctId*/   RADIX2_FFT_MIXED_DFT_N2_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 2);
	bo_DFT_N2->addInParam(0, param_N2);
	bo_DFT_N2->addInParam(1, param_n2);
	bo_DFT_N2->isExecutableOnPE(CORE_ARM0);
	bo_DFT_N2->isExecutableOnPE(CORE_ARM1);
	bo_DFT_N2->isExecutableOnPE(CORE_DSP0);
	bo_DFT_N2->isExecutableOnPE(CORE_DSP3);
	bo_DFT_N2->isExecutableOnPE(CORE_DSP1);
	bo_DFT_N2->isExecutableOnPE(CORE_DSP2);
	bo_DFT_N2->setTimingOnType(CORE_TYPE_CORTEXA15, "1000", stack);

	PiSDFVertex* bo_snk = graph->addBodyVertex(
		/*Name*/    "snk",
		/*FctId*/   RADIX2_FFT_MIXED_SNK_FCT,
		/*InData*/  1,
		/*OutData*/ 0,
		/*InParam*/ 1);
	bo_snk->addInParam(0, param_fftSize);
	bo_snk->isExecutableOnPE(CORE_ARM0);
	bo_snk->isExecutableOnPE(CORE_ARM1);
	bo_snk->isExecutableOnPE(CORE_DSP0);
	bo_snk->isExecutableOnPE(CORE_DSP3);
	bo_snk->isExecutableOnPE(CORE_DSP1);
	bo_snk->isExecutableOnPE(CORE_DSP2);
	bo_snk->setTimingOnType(CORE_TYPE_CORTEXA15, "1000", stack);

	PiSDFVertex* bo_DFT_Radix2 = graph->addHierVertex(
		/*Name*/    "DFT_Radix2",
		/*Graph*/   DFT_Radix2(archi, stack),
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 4);
	bo_DFT_Radix2->addInParam(0, param_N1);
	bo_DFT_Radix2->addInParam(1, param_N2);
	bo_DFT_Radix2->addInParam(2, param_n1);
	bo_DFT_Radix2->addInParam(3, param_P);


	/* Edges */
	graph->connect(
		/*Src*/ bo_src, /*SrcPrt*/ 0, /*Prod*/ "(fftSize)*8",
		/*Snk*/ bo_T, /*SnkPrt*/ 0, /*Cons*/ "(N1*N2)*8",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_T, /*SrcPrt*/ 0, /*Prod*/ "(N1*N2)*8",
		/*Snk*/ bo_DFT_N2, /*SnkPrt*/ 0, /*Cons*/ "(n2*N2)*8",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_DFT_N2, /*SrcPrt*/ 0, /*Prod*/ "(n2*N2)*8",
		/*Snk*/ bo_DFT_Radix2, /*SnkPrt*/ 0, /*Cons*/ "(N1*N2)*8",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_DFT_Radix2, /*SrcPrt*/ 0, /*Prod*/ "(N1*N2)*8",
		/*Snk*/ bo_snk, /*SnkPrt*/ 0, /*Cons*/ "(fftSize)*8",
		/*Delay*/ "0",0);

	return graph;
}

// Method building PiSDFGraphDFT_Radix2
PiSDFGraph* DFT_Radix2(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
		/*Edges*/    6,
		/*Params*/   5,
		/*InputIf*/  1,
		/*OutputIf*/ 1,
		/*Config*/   0,
		/*Body*/     4,
		/*Archi*/    archi,
		/*Stack*/    stack);

	/* Parameters */
	PiSDFParam *param_N1 = graph->addHeritedParam("N1", 0);
	PiSDFParam *param_N2 = graph->addHeritedParam("N2", 1);
	PiSDFParam *param_n1 = graph->addHeritedParam("n1", 2);
	PiSDFParam *param_P = graph->addHeritedParam("P", 3);
	PiSDFParam *param_size = graph->addDependentParam("size", "N1*N2");

	/* Vertices */
	PiSDFVertex* bo_Br = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_BROADCAST,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 1);
	bo_Br->addInParam(0, param_size);

	PiSDFVertex* if_out = graph->addOutputIf(
		/*Name*/    "if_out",
		/*InParam*/ 1);
	if_out->addInParam(0, param_size);

	PiSDFVertex* if_in = graph->addInputIf(
		/*Name*/    "if_in",
		/*InParam*/ 1);
	if_in->addInParam(0, param_size);

	PiSDFVertex* bo_genIx = graph->addBodyVertex(
		/*Name*/    "genIx",
		/*FctId*/   DFT_RADIX2_GENIX_FCT,
		/*InData*/  0,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_genIx->addInParam(0, param_P);
	bo_genIx->isExecutableOnPE(CORE_ARM0);
	bo_genIx->isExecutableOnPE(CORE_ARM1);
	bo_genIx->isExecutableOnPE(CORE_DSP0);
	bo_genIx->isExecutableOnPE(CORE_DSP3);
	bo_genIx->isExecutableOnPE(CORE_DSP1);
	bo_genIx->isExecutableOnPE(CORE_DSP2);
	bo_genIx->setTimingOnType(CORE_TYPE_CORTEXA15, "1000", stack);

	PiSDFVertex* bo_Radix2_Stage = graph->addHierVertex(
		/*Name*/    "Radix2_Stage",
		/*Graph*/   Radix2_Stage(archi, stack),
		/*InData*/  2,
		/*OutData*/ 1,
		/*InParam*/ 3);
	bo_Radix2_Stage->addInParam(0, param_N1);
	bo_Radix2_Stage->addInParam(1, param_N2);
	bo_Radix2_Stage->addInParam(2, param_n1);


	/* Edges */
	graph->connect(
		/*Src*/ bo_genIx, /*SrcPrt*/ 0, /*Prod*/ "(P)*4",
		/*Snk*/ bo_Radix2_Stage, /*SnkPrt*/ 0, /*Cons*/ "(1)*4",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Radix2_Stage, /*SrcPrt*/ 0, /*Prod*/ "(N1*N2)*8",
		/*Snk*/ bo_Br, /*SnkPrt*/ 0, /*Cons*/ "(size)*8",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br, /*SrcPrt*/ 0, /*Prod*/ "(size)*8",
		/*Snk*/ if_out, /*SnkPrt*/ 0, /*Cons*/ "(size)*8",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br, /*SrcPrt*/ 1, /*Prod*/ "(size)*8",
		/*Snk*/ bo_Radix2_Stage, /*SnkPrt*/ 1, /*Cons*/ "(N1*N2)*8",
		/*Delay*/ "(size)*8",if_in);

	return graph;
}

// Method building PiSDFGraphRadix2_Stage
PiSDFGraph* Radix2_Stage(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
		/*Edges*/    8,
		/*Params*/   5,
		/*InputIf*/  2,
		/*OutputIf*/ 1,
		/*Config*/   1,
		/*Body*/     4,
		/*Archi*/    archi,
		/*Stack*/    stack);

	/* Parameters */
	PiSDFParam *param_N1 = graph->addHeritedParam("N1", 0);
	PiSDFParam *param_N2 = graph->addHeritedParam("N2", 1);
	PiSDFParam *param_n1 = graph->addHeritedParam("n1", 2);
	PiSDFParam *param_p = graph->addDynamicParam("p");
	PiSDFParam *param_nDFT2 = graph->addDependentParam("nDFT2", "N2*N1/n1/2");

	/* Vertices */
	PiSDFVertex* bo_DFT_2 = graph->addBodyVertex(
		/*Name*/    "DFT_2",
		/*FctId*/   RADIX2_STAGE_DFT_2_FCT,
		/*InData*/  3,
		/*OutData*/ 2,
		/*InParam*/ 4);
	bo_DFT_2->addInParam(0, param_n1);
	bo_DFT_2->addInParam(1, param_p);
	bo_DFT_2->addInParam(2, param_N2);
	bo_DFT_2->addInParam(3, param_N1);
	bo_DFT_2->isExecutableOnPE(CORE_ARM0);
	bo_DFT_2->isExecutableOnPE(CORE_ARM1);
	bo_DFT_2->isExecutableOnPE(CORE_DSP0);
	bo_DFT_2->isExecutableOnPE(CORE_DSP3);
	bo_DFT_2->isExecutableOnPE(CORE_DSP1);
	bo_DFT_2->isExecutableOnPE(CORE_DSP2);
	bo_DFT_2->setTimingOnType(CORE_TYPE_CORTEXA15, "1000", stack);

	PiSDFVertex* if_pValue = graph->addInputIf(
		/*Name*/    "if_pValue",
		/*InParam*/ 0);

	PiSDFVertex* if_in = graph->addInputIf(
		/*Name*/    "if_in",
		/*InParam*/ 2);
	if_in->addInParam(0, param_N1);
	if_in->addInParam(1, param_N2);

	PiSDFVertex* if_out = graph->addOutputIf(
		/*Name*/    "if_out",
		/*InParam*/ 2);
	if_out->addInParam(0, param_N1);
	if_out->addInParam(1, param_N2);

	PiSDFVertex* bo_F = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_FORK,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 2);
	bo_F->addInParam(0, param_N2);
	bo_F->addInParam(1, param_p);

	PiSDFVertex* bo_J = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_JOIN,
		/*InData*/  2,
		/*OutData*/ 1,
		/*InParam*/ 2);
	bo_J->addInParam(0, param_N2);
	bo_J->addInParam(1, param_p);

	PiSDFVertex* cf_cfg = graph->addConfigVertex(
		/*Name*/    "cfg",
		/*FctId*/   RADIX2_STAGE_CFG_FCT,
		/*SubType*/ PISDF_SUBTYPE_NORMAL,
		/*InData*/  1,
		/*OutData*/ 0,
		/*InParam*/ 0,
		/*OutParam*/1);
	cf_cfg->addOutParam(0, param_p);
	cf_cfg->isExecutableOnPE(CORE_ARM0);
	cf_cfg->isExecutableOnPE(CORE_ARM1);
	cf_cfg->isExecutableOnPE(CORE_DSP0);
	cf_cfg->isExecutableOnPE(CORE_DSP3);
	cf_cfg->isExecutableOnPE(CORE_DSP1);
	cf_cfg->isExecutableOnPE(CORE_DSP2);
	cf_cfg->setTimingOnType(CORE_TYPE_CORTEXA15, "1000", stack);

	PiSDFVertex* bo_genIx = graph->addBodyVertex(
		/*Name*/    "genIx",
		/*FctId*/   RADIX2_STAGE_GENIX_FCT,
		/*InData*/  0,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_genIx->addInParam(0, param_nDFT2);
	bo_genIx->isExecutableOnPE(CORE_ARM0);
	bo_genIx->isExecutableOnPE(CORE_ARM1);
	bo_genIx->isExecutableOnPE(CORE_DSP0);
	bo_genIx->isExecutableOnPE(CORE_DSP3);
	bo_genIx->isExecutableOnPE(CORE_DSP1);
	bo_genIx->isExecutableOnPE(CORE_DSP2);
	bo_genIx->setTimingOnType(CORE_TYPE_CORTEXA15, "1000", stack);


	/* Edges */
	graph->connect(
		/*Src*/ if_pValue, /*SrcPrt*/ 0, /*Prod*/ "(1)*4",
		/*Snk*/ cf_cfg, /*SnkPrt*/ 0, /*Cons*/ "(1)*4",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ if_in, /*SrcPrt*/ 0, /*Prod*/ "(N1*N2)*8",
		/*Snk*/ bo_F, /*SnkPrt*/ 0, /*Cons*/ "(N2*(2^(p+1)))*8",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_F, /*SrcPrt*/ 0, /*Prod*/ "(N2*(2^p))*8",
		/*Snk*/ bo_DFT_2, /*SnkPrt*/ 0, /*Cons*/ "(n1)*8",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_F, /*SrcPrt*/ 1, /*Prod*/ "(N2*(2^p))*8",
		/*Snk*/ bo_DFT_2, /*SnkPrt*/ 1, /*Cons*/ "(n1)*8",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_DFT_2, /*SrcPrt*/ 0, /*Prod*/ "(n1)*8",
		/*Snk*/ bo_J, /*SnkPrt*/ 0, /*Cons*/ "(N2*(2^p))*8",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_DFT_2, /*SrcPrt*/ 1, /*Prod*/ "(n1)*8",
		/*Snk*/ bo_J, /*SnkPrt*/ 1, /*Cons*/ "(N2*(2^p))*8",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_J, /*SrcPrt*/ 0, /*Prod*/ "(N2*(2^(p+1)))*8",
		/*Snk*/ if_out, /*SnkPrt*/ 0, /*Cons*/ "(N1*N2)*8",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_genIx, /*SrcPrt*/ 0, /*Prod*/ "(nDFT2)*4",
		/*Snk*/ bo_DFT_2, /*SnkPrt*/ 2, /*Cons*/ "(1)*4",
		/*Delay*/ "0",0);

	return graph;
}

void free_Radix2_fft_mixed(PiSDFGraph* top, Stack* stack){
	top->~PiSDFGraph();
	stack->free(top);
}
