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

// Method building PiSDFGraph top_fft
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
	PiSDFParam *param_fftSize = graph->addStaticParam("fftSize", 8.0);

	/* Vertices */
	PiSDFVertex* bo_Src = graph->addBodyVertex(
		/*Name*/    "Src",
		/*FctId*/   -1,
		/*InData*/  0,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_Src->addInParam(0, param_fftSize);
	bo_Src->isExecutableOnPE(CORE_CORE0);

	PiSDFVertex* bo_Snk = graph->addBodyVertex(
		/*Name*/    "Snk",
		/*FctId*/   -1,
		/*InData*/  1,
		/*OutData*/ 0,
		/*InParam*/ 1);
	bo_Snk->addInParam(0, param_fftSize);
	bo_Snk->isExecutableOnPE(CORE_CORE0);

	PiSDFVertex* bo_FFT = graph->addHierVertex(
		/*Name*/    "FFT",
		/*Graph*/	FFT(archi, stack),
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

// Method building PiSDFGraph FFT
PiSDFGraph* FFT(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
		/*Edges*/    6,
		/*Params*/   1,
		/*InputIf*/  1,
		/*OutputIf*/ 1,
		/*Config*/   0,
		/*Body*/     3,
		/*Archi*/    archi,
		/*Stack*/    stack);

	/* Parameters */
	PiSDFParam *param_fftSize = graph->addStaticParam("fftSize", 8);

	/* Vertices */
	PiSDFVertex* if_in = graph->addInputIf(
		/*Name*/    "if_in",
		/*InParam*/ 1);
	if_in->addInParam(0, param_fftSize);

	PiSDFVertex* if_out = graph->addOutputIf(
		/*Name*/    "if_out",
		/*InParam*/ 1);
	if_out->addInParam(0, param_fftSize);

	PiSDFVertex* bo_Fork = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_FORK,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 0);
	PiSDFVertex* bo_Join = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_JOIN,
		/*InData*/  2,
		/*OutData*/ 1,
		/*InParam*/ 0);
	PiSDFVertex* bo_fft_core = graph->addBodyVertex(
		/*Name*/    "fft_core",
		/*FctId*/   -1,
		/*InData*/  2,
		/*OutData*/ 2,
		/*InParam*/ 0);
	bo_fft_core->isExecutableOnPE(CORE_CORE0);


	/* Edges */
	graph->connect(
		/*Src*/ if_in, /*SrcPrt*/ 0, /*Prod*/ "fftSize",
		/*Snk*/ bo_Fork, /*SnkPrt*/ 0, /*Cons*/ "8",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Fork, /*SrcPrt*/ 0, /*Prod*/ "4",
		/*Snk*/ bo_fft_core, /*SnkPrt*/ 0, /*Cons*/ "1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Fork, /*SrcPrt*/ 1, /*Prod*/ "4",
		/*Snk*/ bo_fft_core, /*SnkPrt*/ 1, /*Cons*/ "1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_fft_core, /*SrcPrt*/ 0, /*Prod*/ "1",
		/*Snk*/ bo_Join, /*SnkPrt*/ 0, /*Cons*/ "1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_fft_core, /*SrcPrt*/ 1, /*Prod*/ "1",
		/*Snk*/ bo_Join, /*SnkPrt*/ 1, /*Cons*/ "1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Join, /*SrcPrt*/ 0, /*Prod*/ "2",
		/*Snk*/ if_out, /*SnkPrt*/ 0, /*Cons*/ "fftSize",
		/*Delay*/ "0",0);

	return graph;
}
