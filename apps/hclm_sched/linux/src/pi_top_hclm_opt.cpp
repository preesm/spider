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

#include "top_hclm.h"

PiSDFGraph* top_hclm_opt(Archi* archi, Stack* stack, Param MNext = 0, Param MStart = 10, Param NMax = 20, Param NVal = 10, Param NbS = 4000);
PiSDFGraph* FIR_Chan_opt(Archi* archi, Stack* stack);

/**
 * This is the method you need to call to build a complete PiSDF graph.
 */
PiSDFGraph* init_top_hclm_opt(Archi* archi, Stack* stack, Param MNext, Param MStart, Param NMax, Param NVal, Param NbS){
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
		/*Graph*/    top_hclm_opt(archi, stack, MNext, MStart, NMax, NVal, NbS),
		/*InputIf*/  0,
		/*OutputIf*/ 0,
		/*Params*/   0);

	return top;
}

// Method building PiSDFGraphtop_hclm
PiSDFGraph* top_hclm_opt(Archi* archi, Stack* stack, Param MNext, Param MStart, Param NMax, Param NVal, Param NbS){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
		/*Edges*/    5,
		/*Params*/   6,
		/*InputIf*/  0,
		/*OutputIf*/ 0,
		/*Config*/   1,
		/*Body*/     5,
		/*Archi*/    archi,
		/*Stack*/    stack);

	/* Parameters */
	PiSDFParam *param_NbS = graph->addStaticParam("NbS", NbS);
	PiSDFParam *param_N = graph->addDynamicParam("N");
	PiSDFParam *param_NMax = graph->addStaticParam("NMax", NMax);
	PiSDFParam *param_MNext = graph->addStaticParam("MNext", MNext);
	PiSDFParam *param_MStart = graph->addStaticParam("MStart", MStart);
	PiSDFParam *param_NVal = graph->addStaticParam("NVal", NVal);

	/* Vertices */
	PiSDFVertex* cf_cfg_N = graph->addConfigVertex(
		/*Name*/    "cfg_N",
		/*FctId*/   TOP_HCLM_CFG_N_FCT,
		/*SubType*/ PISDF_SUBTYPE_NORMAL,
		/*InData*/  0,
		/*OutData*/ 1,
		/*InParam*/ 4,
		/*OutParam*/1);
	cf_cfg_N->addOutParam(0, param_N);
	cf_cfg_N->addInParam(0, param_NMax);
	cf_cfg_N->addInParam(1, param_NVal);
	cf_cfg_N->addInParam(2, param_MStart);
	cf_cfg_N->addInParam(3, param_MNext);
	cf_cfg_N->isExecutableOnPE(CORE_CORE0);
	cf_cfg_N->setTimingOnType(CORE_TYPE_X86, "100", stack);

	PiSDFVertex* bo_F = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_FORK,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 2);
	bo_F->addInParam(0, param_NMax);
	bo_F->addInParam(1, param_N);

	PiSDFVertex* bo_src = graph->addBodyVertex(
		/*Name*/    "src",
		/*FctId*/   TOP_HCLM_SRC_FCT,
		/*InData*/  0,
		/*OutData*/ 1,
		/*InParam*/ 2);
	bo_src->addInParam(0, param_NbS);
	bo_src->addInParam(1, param_N);
	bo_src->isExecutableOnPE(CORE_CORE0);
	bo_src->setTimingOnType(CORE_TYPE_X86, "100", stack);

	PiSDFVertex* bo_end = graph->addSpecialVertex(
			/*Type*/    PISDF_SUBTYPE_END,
			/*InData*/  1,
			/*OutData*/ 0,
			/*InParam*/ 2);
	bo_end->addInParam(0, param_NMax);
	bo_end->addInParam(1, param_N);

	PiSDFVertex* bo_snk = graph->addBodyVertex(
		/*Name*/    "snk",
		/*FctId*/   TOP_HCLM_SNK_FCT,
		/*InData*/  1,
		/*OutData*/ 0,
		/*InParam*/ 2);
	bo_snk->addInParam(0, param_NbS);
	bo_snk->addInParam(1, param_N);
	bo_snk->isExecutableOnPE(CORE_CORE0);
	bo_snk->setTimingOnType(CORE_TYPE_X86, "100", stack);

	PiSDFVertex* bo_FIR_Chan = graph->addHierVertex(
		/*Name*/    "FIR_Chan",
		/*Graph*/   FIR_Chan_opt(archi, stack),
		/*InData*/  2,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_FIR_Chan->addInParam(0, param_NbS);


	/* Edges */
	graph->connect(
		/*Src*/ cf_cfg_N, /*SrcPrt*/ 0, /*Prod*/ "(NMax)*1",
		/*Snk*/ bo_F, /*SnkPrt*/ 0, /*Cons*/ "(NMax)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_F, /*SrcPrt*/ 0, /*Prod*/ "(N)*1",
		/*Snk*/ bo_FIR_Chan, /*SnkPrt*/ 0, /*Cons*/ "(1)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_F, /*SrcPrt*/ 1, /*Prod*/ "(NMax-N)*1",
		/*Snk*/ bo_end, /*SnkPrt*/ 0, /*Cons*/ "(NMax-N)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_src, /*SrcPrt*/ 0, /*Prod*/ "(NbS*N)*4",
		/*Snk*/ bo_FIR_Chan, /*SnkPrt*/ 1, /*Cons*/ "(NbS)*4",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_FIR_Chan, /*SrcPrt*/ 0, /*Prod*/ "(NbS)*4",
		/*Snk*/ bo_snk, /*SnkPrt*/ 0, /*Cons*/ "(NbS*N)*4",
		/*Delay*/ "0",0);

	return graph;
}

// Method building PiSDFGraphFIR_Chan
PiSDFGraph* FIR_Chan_opt(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
		/*Edges*/    8,
		/*Params*/   2,
		/*InputIf*/  2,
		/*OutputIf*/ 1,
		/*Config*/   1,
		/*Body*/     4,
		/*Archi*/    archi,
		/*Stack*/    stack);

	/* Parameters */
	PiSDFParam *param_NbS = graph->addHeritedParam("NbS", 0);
	PiSDFParam *param_M = graph->addDynamicParam("M");

	/* Vertices */
	PiSDFVertex* if_M_in = graph->addInputIf(
		/*Name*/    "if_M_in",
		/*InParam*/ 0);

	PiSDFVertex* if_in = graph->addInputIf(
		/*Name*/    "if_in",
		/*InParam*/ 1);
	if_in->addInParam(0, param_NbS);

	PiSDFVertex* if_out = graph->addOutputIf(
		/*Name*/    "if_out",
		/*InParam*/ 1);
	if_out->addInParam(0, param_NbS);

	PiSDFVertex* cf_cfg_M = graph->addConfigVertex(
		/*Name*/    "cfg_M",
		/*FctId*/   FIR_CHAN_CFG_M_FCT,
		/*SubType*/ PISDF_SUBTYPE_NORMAL,
		/*InData*/  1,
		/*OutData*/ 0,
		/*InParam*/ 0,
		/*OutParam*/1);
	cf_cfg_M->addOutParam(0, param_M);
	cf_cfg_M->isExecutableOnPE(CORE_CORE0);
	cf_cfg_M->setTimingOnType(CORE_TYPE_X86, "100", stack);

	PiSDFVertex* bo_initSw = graph->addBodyVertex(
		/*Name*/    "initSw",
		/*FctId*/   FIR_CHAN_INITSW_FCT,
		/*InData*/  0,
		/*OutData*/ 2,
		/*InParam*/ 1);
	bo_initSw->addInParam(0, param_M);
	bo_initSw->isExecutableOnPE(CORE_CORE0);
	bo_initSw->setTimingOnType(CORE_TYPE_X86, "100", stack);

	PiSDFVertex* bo_FIR = graph->addBodyVertex(
		/*Name*/    "FIR",
		/*FctId*/   FIR_CHAN_FIR_FCT,
		/*InData*/  2,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_FIR->addInParam(0, param_NbS);
	bo_FIR->isExecutableOnPE(CORE_CORE0);
	bo_FIR->setTimingOnType(CORE_TYPE_X86, "100", stack);

	PiSDFVertex* bo_Br = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_BROADCAST,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 1);
	bo_Br->addInParam(0, param_NbS);


	/* Edges */
	graph->connect(
		/*Src*/ if_M_in, /*SrcPrt*/ 0, /*Prod*/ "(1)*1",
		/*Snk*/ cf_cfg_M, /*SnkPrt*/ 0, /*Cons*/ "(1)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_initSw, /*SrcPrt*/ 0, /*Prod*/ "(M)*1",
		/*Snk*/ bo_FIR, /*SnkPrt*/ 1, /*Cons*/ "(1)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_FIR, /*SrcPrt*/ 0, /*Prod*/ "(NbS)*4",
		/*Snk*/ bo_Br, /*SnkPrt*/ 0, /*Cons*/ "(NbS)*4",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br, /*SrcPrt*/ 0, /*Prod*/ "(NbS)*4",
		/*Snk*/ if_out, /*SnkPrt*/ 0, /*Cons*/ "(NbS)*4",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br, /*SrcPrt*/ 1, /*Prod*/ "(NbS)*4",
		/*Snk*/ bo_FIR, /*SnkPrt*/ 0, /*Cons*/ "(NbS)*4",
		/*Delay*/ "(NbS)*4", if_in);

	return graph;
}

void free_top_hclm_opt(PiSDFGraph* top, Stack* stack){
	top->~PiSDFGraph();
	stack->free(top);
}
