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

#include "daq_fft.h"

PiSDFGraph* daq_fft(Archi* archi, Stack* stack);

/**
 * This is the method you need to call to build a complete PiSDF graph.
 */
PiSDFGraph* init_daq_fft(Archi* archi, Stack* stack){
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
		/*Graph*/    daq_fft(archi, stack),
		/*InputIf*/  0,
		/*OutputIf*/ 0,
		/*Params*/   0);

	return top;
}

// Method building PiSDFGraphdaq_fft
PiSDFGraph* daq_fft(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
		/*Edges*/    8,
		/*Params*/   3,
		/*InputIf*/  0,
		/*OutputIf*/ 0,
		/*Config*/   1,
		/*Body*/     9,
		/*Archi*/    archi,
		/*Stack*/    stack);

	/* Parameters */
	PiSDFParam *param_Nr = graph->addDynamicParam("Nr");
	PiSDFParam *param_Nc = graph->addDynamicParam("Nc");
	PiSDFParam *param_size = graph->addStaticParam("size", 65536);

	/* Vertices */
	PiSDFVertex* bo_src = graph->addBodyVertex(
		/*Name*/    "src",
		/*FctId*/   SRC_FCT,
		/*InData*/  0,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_src->addInParam(0, param_size);
	bo_src->isExecutableOnPE(CORE_CORE0);
	bo_src->setTimingOnType(CORE_TYPE_X86, "191305", stack);

	PiSDFVertex* bo_T_1 = graph->addBodyVertex(
		/*Name*/    "T_1",
		/*FctId*/   T_1_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 2);
	bo_T_1->addInParam(0, param_Nc);
	bo_T_1->addInParam(1, param_Nr);
	bo_T_1->isExecutableOnPE(CORE_CORE0);
	bo_T_1->setTimingOnType(CORE_TYPE_X86, "705126", stack);

	PiSDFVertex* bo_FFT_2 = graph->addBodyVertex(
		/*Name*/    "FFT_2",
		/*FctId*/   FFT_2_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_FFT_2->addInParam(0, param_Nr);
	bo_FFT_2->isExecutableOnPE(CORE_CORE0);
	bo_FFT_2->setTimingOnType(CORE_TYPE_X86, "16488", stack);

	PiSDFVertex* bo_T_3 = graph->addBodyVertex(
		/*Name*/    "T_3",
		/*FctId*/   T_3_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 2);
	bo_T_3->addInParam(0, param_Nr);
	bo_T_3->addInParam(1, param_Nc);
	bo_T_3->isExecutableOnPE(CORE_CORE0);
	bo_T_3->setTimingOnType(CORE_TYPE_X86, "714374", stack);

	PiSDFVertex* bo_Twi_4 = graph->addBodyVertex(
		/*Name*/    "Twi_4",
		/*FctId*/   TWI_4_FCT,
		/*InData*/  2,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_Twi_4->addInParam(0, param_Nc);
	bo_Twi_4->isExecutableOnPE(CORE_CORE0);
	bo_Twi_4->setTimingOnType(CORE_TYPE_X86, "3848", stack);

	PiSDFVertex* bo_FFT_5 = graph->addBodyVertex(
		/*Name*/    "FFT_5",
		/*FctId*/   FFT_5_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_FFT_5->addInParam(0, param_Nc);
	bo_FFT_5->isExecutableOnPE(CORE_CORE0);
	bo_FFT_5->setTimingOnType(CORE_TYPE_X86, "16614", stack);

	PiSDFVertex* bo_T_6 = graph->addBodyVertex(
		/*Name*/    "T_6",
		/*FctId*/   T_6_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 2);
	bo_T_6->addInParam(0, param_Nc);
	bo_T_6->addInParam(1, param_Nr);
	bo_T_6->isExecutableOnPE(CORE_CORE0);
	bo_T_6->setTimingOnType(CORE_TYPE_X86, "707298", stack);

	PiSDFVertex* bo_snk = graph->addBodyVertex(
		/*Name*/    "snk",
		/*FctId*/   SNK_FCT,
		/*InData*/  1,
		/*OutData*/ 0,
		/*InParam*/ 1);
	bo_snk->addInParam(0, param_size);
	bo_snk->isExecutableOnPE(CORE_CORE0);
	bo_snk->setTimingOnType(CORE_TYPE_X86, "1198965", stack);

	PiSDFVertex* cf_cfg = graph->addConfigVertex(
		/*Name*/    "cfg",
		/*FctId*/   CFG_FCT,
		/*SubType*/ PISDF_SUBTYPE_NORMAL,
		/*InData*/  0,
		/*OutData*/ 0,
		/*InParam*/ 1,
		/*OutParam*/2);
	cf_cfg->addOutParam(0, param_Nr);
	cf_cfg->addOutParam(1, param_Nc);
	cf_cfg->addInParam(0, param_size);
	cf_cfg->isExecutableOnPE(CORE_CORE0);
	cf_cfg->setTimingOnType(CORE_TYPE_X86, "276", stack);

	PiSDFVertex* bo_genIx = graph->addBodyVertex(
		/*Name*/    "genIx",
		/*FctId*/   GENIX_FCT,
		/*InData*/  0,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_genIx->addInParam(0, param_Nr);
	bo_genIx->isExecutableOnPE(CORE_CORE0);
	bo_genIx->setTimingOnType(CORE_TYPE_X86, "1015", stack);


	/* Edges */
	graph->connect(
		/*Src*/ bo_src, /*SrcPrt*/ 0, /*Prod*/ "8*(size)",
		/*Snk*/ bo_T_1, /*SnkPrt*/ 0, /*Cons*/ "8*(Nc*Nr)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_T_1, /*SrcPrt*/ 0, /*Prod*/ "8*(Nc*Nr)",
		/*Snk*/ bo_FFT_2, /*SnkPrt*/ 0, /*Cons*/ "8*(size)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_FFT_2, /*SrcPrt*/ 0, /*Prod*/ "8*(size)",
		/*Snk*/ bo_T_3, /*SnkPrt*/ 0, /*Cons*/ "8*(Nc*Nr)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_T_3, /*SrcPrt*/ 0, /*Prod*/ "8*(Nc*Nr)",
		/*Snk*/ bo_Twi_4, /*SnkPrt*/ 1, /*Cons*/ "8*(size)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Twi_4, /*SrcPrt*/ 0, /*Prod*/ "8*(size)",
		/*Snk*/ bo_FFT_5, /*SnkPrt*/ 0, /*Cons*/ "8*(size)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_FFT_5, /*SrcPrt*/ 0, /*Prod*/ "8*(size)",
		/*Snk*/ bo_T_6, /*SnkPrt*/ 0, /*Cons*/ "8*(Nc*Nr)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_T_6, /*SrcPrt*/ 0, /*Prod*/ "8*(Nc*Nr)",
		/*Snk*/ bo_snk, /*SnkPrt*/ 0, /*Cons*/ "8*(size)",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_genIx, /*SrcPrt*/ 0, /*Prod*/ "4*(Nr)",
		/*Snk*/ bo_Twi_4, /*SnkPrt*/ 0, /*Cons*/ "4*(1)",
		/*Delay*/ "0",0);

	return graph;
}

void free_daq_fft(PiSDFGraph* top, Stack* stack){
	top->~PiSDFGraph();
	stack->free(top);
}
