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

#include "stereo.h"

PiSDFGraph* stereo(Archi* archi, Stack* stack);
PiSDFGraph* costParallel(Archi* archi, Stack* stack);
PiSDFGraph* DispComp(Archi* archi, Stack* stack);

/**
 * This is the method you need to call to build a complete PiSDF graph.
 */
PiSDFGraph* init_stereo(Archi* archi, Stack* stack){
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
		/*Graph*/    stereo(archi, stack),
		/*InputIf*/  0,
		/*OutputIf*/ 0,
		/*Params*/   0);

	return top;
}

// Method building PiSDFGraphstereo
PiSDFGraph* stereo(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
		/*Edges*/    16,
		/*Params*/   11,
		/*InputIf*/  0,
		/*OutputIf*/ 0,
		/*Config*/   1,
		/*Body*/     12,
		/*Archi*/    archi,
		/*Stack*/    stack);

	/* Parameters */
	PiSDFParam *param_nSlice = graph->addDynamicParam("nSlice");
	PiSDFParam *param_truncValue = graph->addDynamicParam("truncValue");
	PiSDFParam *param_scale = graph->addDynamicParam("scale");
	PiSDFParam *param_nIter = graph->addDynamicParam("nIter");
	PiSDFParam *param_sizeFilter = graph->addDynamicParam("sizeFilter");
	PiSDFParam *param_minDisp = graph->addDynamicParam("minDisp");
	PiSDFParam *param_maxDisp = graph->addDynamicParam("maxDisp");
	PiSDFParam *param_height = graph->addDynamicParam("height");
	PiSDFParam *param_width = graph->addDynamicParam("width");
	PiSDFParam *param_size = graph->addDependentParam("size", "width*height");
	PiSDFParam *param_sliceHeight = graph->addDependentParam("sliceHeight", "height/nSlice+2*sizeFilter");

	/* Vertices */
	PiSDFVertex* cf_Config = graph->addConfigVertex(
		/*Name*/    "Config",
		/*FctId*/   STEREO_CONFIG_FCT,
		/*SubType*/ PISDF_SUBTYPE_NORMAL,
		/*InData*/  0,
		/*OutData*/ 0,
		/*InParam*/ 0,
		/*OutParam*/9);
	cf_Config->addOutParam(0, param_scale);
	cf_Config->addOutParam(1, param_maxDisp);
	cf_Config->addOutParam(2, param_minDisp);
	cf_Config->addOutParam(3, param_nSlice);
	cf_Config->addOutParam(4, param_truncValue);
	cf_Config->addOutParam(5, param_height);
	cf_Config->addOutParam(6, param_width);
	cf_Config->addOutParam(7, param_sizeFilter);
	cf_Config->addOutParam(8, param_nIter);
	cf_Config->isExecutableOnPE(CORE_CORE0);
	cf_Config->setTimingOnType(CORE_TYPE_X86, "1000", stack);

	PiSDFVertex* bo_Camera = graph->addBodyVertex(
		/*Name*/    "Camera",
		/*FctId*/   STEREO_CAMERA_FCT,
		/*InData*/  0,
		/*OutData*/ 2,
		/*InParam*/ 2);
	bo_Camera->addInParam(0, param_width);
	bo_Camera->addInParam(1, param_height);
	bo_Camera->isExecutableOnPE(CORE_CORE0);
	bo_Camera->setTimingOnType(CORE_TYPE_X86, "1000", stack);

	PiSDFVertex* bo_Br_rgbL = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_BROADCAST,
		/*InData*/  1,
		/*OutData*/ 3,
		/*InParam*/ 1);
	bo_Br_rgbL->addInParam(0, param_size);

	PiSDFVertex* bo_RGB2Gray_L = graph->addBodyVertex(
		/*Name*/    "RGB2Gray_L",
		/*FctId*/   STEREO_RGB2GRAY_L_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_RGB2Gray_L->addInParam(0, param_size);
	bo_RGB2Gray_L->isExecutableOnPE(CORE_CORE0);
	bo_RGB2Gray_L->setTimingOnType(CORE_TYPE_X86, "1000", stack);

	PiSDFVertex* bo_RGB2Gray_R = graph->addBodyVertex(
		/*Name*/    "RGB2Gray_R",
		/*FctId*/   STEREO_RGB2GRAY_R_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_RGB2Gray_R->addInParam(0, param_size);
	bo_RGB2Gray_R->isExecutableOnPE(CORE_CORE0);
	bo_RGB2Gray_R->setTimingOnType(CORE_TYPE_X86, "1000", stack);

	PiSDFVertex* bo_Br_grayL = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_BROADCAST,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 1);
	bo_Br_grayL->addInParam(0, param_size);

	PiSDFVertex* bo_Br_grayR = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_BROADCAST,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 1);
	bo_Br_grayR->addInParam(0, param_size);

	PiSDFVertex* bo_Census_L = graph->addBodyVertex(
		/*Name*/    "Census_L",
		/*FctId*/   STEREO_CENSUS_L_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 2);
	bo_Census_L->addInParam(0, param_width);
	bo_Census_L->addInParam(1, param_height);
	bo_Census_L->isExecutableOnPE(CORE_CORE0);
	bo_Census_L->setTimingOnType(CORE_TYPE_X86, "1000", stack);

	PiSDFVertex* bo_Census_R = graph->addBodyVertex(
		/*Name*/    "Census_R",
		/*FctId*/   STEREO_CENSUS_R_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 2);
	bo_Census_R->addInParam(0, param_width);
	bo_Census_R->addInParam(1, param_height);
	bo_Census_R->isExecutableOnPE(CORE_CORE0);
	bo_Census_R->setTimingOnType(CORE_TYPE_X86, "1000", stack);

	PiSDFVertex* bo_Split = graph->addBodyVertex(
		/*Name*/    "Split",
		/*FctId*/   STEREO_SPLIT_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 4);
	bo_Split->addInParam(0, param_width);
	bo_Split->addInParam(1, param_height);
	bo_Split->addInParam(2, param_nSlice);
	bo_Split->addInParam(3, param_sizeFilter);
	bo_Split->isExecutableOnPE(CORE_CORE0);
	bo_Split->setTimingOnType(CORE_TYPE_X86, "1000", stack);

	PiSDFVertex* bo_MedianFilter = graph->addBodyVertex(
		/*Name*/    "MedianFilter",
		/*FctId*/   STEREO_MEDIANFILTER_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 3);
	bo_MedianFilter->addInParam(0, param_width);
	bo_MedianFilter->addInParam(1, param_sizeFilter);
	bo_MedianFilter->addInParam(2, param_sliceHeight);
	bo_MedianFilter->isExecutableOnPE(CORE_CORE0);
	bo_MedianFilter->setTimingOnType(CORE_TYPE_X86, "1000", stack);

	PiSDFVertex* bo_Display = graph->addBodyVertex(
		/*Name*/    "Display",
		/*FctId*/   STEREO_DISPLAY_FCT,
		/*InData*/  2,
		/*OutData*/ 0,
		/*InParam*/ 2);
	bo_Display->addInParam(0, param_width);
	bo_Display->addInParam(1, param_height);
	bo_Display->isExecutableOnPE(CORE_CORE0);
	bo_Display->setTimingOnType(CORE_TYPE_X86, "1000", stack);

	PiSDFVertex* bo_costParallel = graph->addHierVertex(
		/*Name*/    "costParallel",
		/*Graph*/   costParallel(archi, stack),
		/*InData*/  5,
		/*OutData*/ 1,
		/*InParam*/ 7);
	bo_costParallel->addInParam(0, param_nIter);
	bo_costParallel->addInParam(1, param_height);
	bo_costParallel->addInParam(2, param_width);
	bo_costParallel->addInParam(3, param_maxDisp);
	bo_costParallel->addInParam(4, param_minDisp);
	bo_costParallel->addInParam(5, param_truncValue);
	bo_costParallel->addInParam(6, param_scale);


	/* Edges */
	graph->connect(
		/*Src*/ bo_RGB2Gray_L, /*SrcPrt*/ 0, /*Prod*/ "(size)*1",
		/*Snk*/ bo_Br_grayL, /*SnkPrt*/ 0, /*Cons*/ "(size)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_RGB2Gray_R, /*SrcPrt*/ 0, /*Prod*/ "(size)*1",
		/*Snk*/ bo_Br_grayR, /*SnkPrt*/ 0, /*Cons*/ "(size)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Camera, /*SrcPrt*/ 0, /*Prod*/ "(height*width)*3",
		/*Snk*/ bo_Br_rgbL, /*SnkPrt*/ 0, /*Cons*/ "(size)*3",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br_rgbL, /*SrcPrt*/ 0, /*Prod*/ "(size)*3",
		/*Snk*/ bo_RGB2Gray_L, /*SnkPrt*/ 0, /*Cons*/ "(size)*3",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br_rgbL, /*SrcPrt*/ 1, /*Prod*/ "(size)*3",
		/*Snk*/ bo_costParallel, /*SnkPrt*/ 4, /*Cons*/ "(width*height)*3",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br_rgbL, /*SrcPrt*/ 2, /*Prod*/ "(size)*3",
		/*Snk*/ bo_Display, /*SnkPrt*/ 1, /*Cons*/ "(width*height)*3",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_costParallel, /*SrcPrt*/ 0, /*Prod*/ "(height*width)*1",
		/*Snk*/ bo_Split, /*SnkPrt*/ 0, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Split, /*SrcPrt*/ 0, /*Prod*/ "(nSlice*(height*width/nSlice+2*sizeFilter*width))*1",
		/*Snk*/ bo_MedianFilter, /*SnkPrt*/ 0, /*Cons*/ "(sliceHeight*width)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_MedianFilter, /*SrcPrt*/ 0, /*Prod*/ "(sliceHeight*width-2*sizeFilter*width)*1",
		/*Snk*/ bo_Display, /*SnkPrt*/ 0, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br_grayL, /*SrcPrt*/ 0, /*Prod*/ "(size)*1",
		/*Snk*/ bo_Census_L, /*SnkPrt*/ 0, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br_grayR, /*SrcPrt*/ 0, /*Prod*/ "(size)*1",
		/*Snk*/ bo_Census_R, /*SnkPrt*/ 0, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br_grayL, /*SrcPrt*/ 1, /*Prod*/ "(size)*1",
		/*Snk*/ bo_costParallel, /*SnkPrt*/ 2, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br_grayR, /*SrcPrt*/ 1, /*Prod*/ "(size)*1",
		/*Snk*/ bo_costParallel, /*SnkPrt*/ 3, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Census_L, /*SrcPrt*/ 0, /*Prod*/ "(height*width)*1",
		/*Snk*/ bo_costParallel, /*SnkPrt*/ 0, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Census_R, /*SrcPrt*/ 0, /*Prod*/ "(height*width)*1",
		/*Snk*/ bo_costParallel, /*SnkPrt*/ 1, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Camera, /*SrcPrt*/ 1, /*Prod*/ "(height*width)*3",
		/*Snk*/ bo_RGB2Gray_R, /*SnkPrt*/ 0, /*Cons*/ "(size)*3",
		/*Delay*/ "0",0);

	return graph;
}

// Method building PiSDFGraphcostParallel
PiSDFGraph* costParallel(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
		/*Edges*/    18,
		/*Params*/   9,
		/*InputIf*/  5,
		/*OutputIf*/ 1,
		/*Config*/   0,
		/*Body*/     9,
		/*Archi*/    archi,
		/*Stack*/    stack);

	/* Parameters */
	PiSDFParam *param_nIter = graph->addHeritedParam("nIter", 0);
	PiSDFParam *param_height = graph->addHeritedParam("height", 1);
	PiSDFParam *param_width = graph->addHeritedParam("width", 2);
	PiSDFParam *param_maxDisp = graph->addHeritedParam("maxDisp", 3);
	PiSDFParam *param_minDisp = graph->addHeritedParam("minDisp", 4);
	PiSDFParam *param_truncValue = graph->addHeritedParam("truncValue", 5);
	PiSDFParam *param_scale = graph->addHeritedParam("scale", 6);
	PiSDFParam *param_size = graph->addDependentParam("size", "height*width");
	PiSDFParam *param_nDisp = graph->addDependentParam("nDisp", "maxDisp-minDisp+1");

	/* Vertices */
	PiSDFVertex* if_cenL = graph->addInputIf(
		/*Name*/    "if_cenL",
		/*InParam*/ 1);
	if_cenL->addInParam(0, param_size);

	PiSDFVertex* if_cenR = graph->addInputIf(
		/*Name*/    "if_cenR",
		/*InParam*/ 1);
	if_cenR->addInParam(0, param_size);

	PiSDFVertex* if_grayL = graph->addInputIf(
		/*Name*/    "if_grayL",
		/*InParam*/ 1);
	if_grayL->addInParam(0, param_size);

	PiSDFVertex* if_grayR = graph->addInputIf(
		/*Name*/    "if_grayR",
		/*InParam*/ 1);
	if_grayR->addInParam(0, param_size);

	PiSDFVertex* if_rgb = graph->addInputIf(
		/*Name*/    "if_rgb",
		/*InParam*/ 1);
	if_rgb->addInParam(0, param_size);

	PiSDFVertex* bo_Br_rgb = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_BROADCAST,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 1);
	bo_Br_rgb->addInParam(0, param_size);

	PiSDFVertex* bo_GenIx = graph->addBodyVertex(
		/*Name*/    "GenIx",
		/*FctId*/   COSTPARALLEL_GENIX_FCT,
		/*InData*/  0,
		/*OutData*/ 1,
		/*InParam*/ 1);
	bo_GenIx->addInParam(0, param_nIter);
	bo_GenIx->isExecutableOnPE(CORE_CORE0);
	bo_GenIx->setTimingOnType(CORE_TYPE_X86, "1000", stack);

	PiSDFVertex* bo_GenDisp = graph->addBodyVertex(
		/*Name*/    "GenDisp",
		/*FctId*/   COSTPARALLEL_GENDISP_FCT,
		/*InData*/  0,
		/*OutData*/ 1,
		/*InParam*/ 2);
	bo_GenDisp->addInParam(0, param_minDisp);
	bo_GenDisp->addInParam(1, param_maxDisp);
	bo_GenDisp->isExecutableOnPE(CORE_CORE0);
	bo_GenDisp->setTimingOnType(CORE_TYPE_X86, "1000", stack);

	PiSDFVertex* bo_VWeights = graph->addBodyVertex(
		/*Name*/    "VWeights",
		/*FctId*/   COSTPARALLEL_VWEIGHTS_FCT,
		/*InData*/  2,
		/*OutData*/ 1,
		/*InParam*/ 2);
	bo_VWeights->addInParam(0, param_height);
	bo_VWeights->addInParam(1, param_width);
	bo_VWeights->isExecutableOnPE(CORE_CORE0);
	bo_VWeights->setTimingOnType(CORE_TYPE_X86, "1000", stack);

	PiSDFVertex* bo_HWeight = graph->addBodyVertex(
		/*Name*/    "HWeight",
		/*FctId*/   COSTPARALLEL_HWEIGHT_FCT,
		/*InData*/  2,
		/*OutData*/ 1,
		/*InParam*/ 2);
	bo_HWeight->addInParam(0, param_height);
	bo_HWeight->addInParam(1, param_width);
	bo_HWeight->isExecutableOnPE(CORE_CORE0);
	bo_HWeight->setTimingOnType(CORE_TYPE_X86, "1000", stack);

	PiSDFVertex* bo_CostConstruction = graph->addBodyVertex(
		/*Name*/    "CostConstruction",
		/*FctId*/   COSTPARALLEL_COSTCONSTRUCTION_FCT,
		/*InData*/  5,
		/*OutData*/ 1,
		/*InParam*/ 3);
	bo_CostConstruction->addInParam(0, param_truncValue);
	bo_CostConstruction->addInParam(1, param_height);
	bo_CostConstruction->addInParam(2, param_width);
	bo_CostConstruction->isExecutableOnPE(CORE_CORE0);
	bo_CostConstruction->setTimingOnType(CORE_TYPE_X86, "1000", stack);

	PiSDFVertex* if_rawDisparity = graph->addOutputIf(
		/*Name*/    "if_rawDisparity",
		/*InParam*/ 1);
	if_rawDisparity->addInParam(0, param_size);

	PiSDFVertex* bo_Br_Ix = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_BROADCAST,
		/*InData*/  1,
		/*OutData*/ 3,
		/*InParam*/ 1);
	bo_Br_Ix->addInParam(0, param_nIter);

	PiSDFVertex* bo_Br_Disp = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_BROADCAST,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 1);
	bo_Br_Disp->addInParam(0, param_nDisp);

	PiSDFVertex* bo_DispComp = graph->addHierVertex(
		/*Name*/    "DispComp",
		/*Graph*/   DispComp(archi, stack),
		/*InData*/  5,
		/*OutData*/ 1,
		/*InParam*/ 6);
	bo_DispComp->addInParam(0, param_height);
	bo_DispComp->addInParam(1, param_width);
	bo_DispComp->addInParam(2, param_nIter);
	bo_DispComp->addInParam(3, param_scale);
	bo_DispComp->addInParam(4, param_minDisp);
	bo_DispComp->addInParam(5, param_maxDisp);


	/* Edges */
	graph->connect(
		/*Src*/ bo_DispComp, /*SrcPrt*/ 0, /*Prod*/ "(height*width)*1",
		/*Snk*/ if_rawDisparity, /*SnkPrt*/ 0, /*Cons*/ "(size)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_CostConstruction, /*SrcPrt*/ 0, /*Prod*/ "(height*width)*1",
		/*Snk*/ bo_DispComp, /*SnkPrt*/ 4, /*Cons*/ "(height*width*(maxDisp-minDisp+1))*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_VWeights, /*SrcPrt*/ 0, /*Prod*/ "(height*width)*1",
		/*Snk*/ bo_DispComp, /*SnkPrt*/ 1, /*Cons*/ "(height*width*nIter)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_HWeight, /*SrcPrt*/ 0, /*Prod*/ "(height*width)*1",
		/*Snk*/ bo_DispComp, /*SnkPrt*/ 2, /*Cons*/ "(height*width*nIter)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ if_rgb, /*SrcPrt*/ 0, /*Prod*/ "(size)*3",
		/*Snk*/ bo_Br_rgb, /*SnkPrt*/ 0, /*Cons*/ "(size)*3",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_GenIx, /*SrcPrt*/ 0, /*Prod*/ "(nIter)*1",
		/*Snk*/ bo_Br_Ix, /*SnkPrt*/ 0, /*Cons*/ "(nIter)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_GenDisp, /*SrcPrt*/ 0, /*Prod*/ "(maxDisp-minDisp+1)*1",
		/*Snk*/ bo_Br_Disp, /*SnkPrt*/ 0, /*Cons*/ "(nDisp)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br_Disp, /*SrcPrt*/ 0, /*Prod*/ "(nDisp)*1",
		/*Snk*/ bo_CostConstruction, /*SnkPrt*/ 0, /*Cons*/ "(1)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br_Disp, /*SrcPrt*/ 1, /*Prod*/ "(nDisp)*1",
		/*Snk*/ bo_DispComp, /*SnkPrt*/ 3, /*Cons*/ "(maxDisp-minDisp+1)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br_Ix, /*SrcPrt*/ 0, /*Prod*/ "(nIter)*1",
		/*Snk*/ bo_DispComp, /*SnkPrt*/ 0, /*Cons*/ "(nIter)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br_Ix, /*SrcPrt*/ 1, /*Prod*/ "(nIter)*1",
		/*Snk*/ bo_HWeight, /*SnkPrt*/ 0, /*Cons*/ "(1)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br_Ix, /*SrcPrt*/ 2, /*Prod*/ "(nIter)*1",
		/*Snk*/ bo_VWeights, /*SnkPrt*/ 0, /*Cons*/ "(1)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br_rgb, /*SrcPrt*/ 0, /*Prod*/ "(size)*3",
		/*Snk*/ bo_HWeight, /*SnkPrt*/ 1, /*Cons*/ "(height*width)*3",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br_rgb, /*SrcPrt*/ 1, /*Prod*/ "(size)*3",
		/*Snk*/ bo_VWeights, /*SnkPrt*/ 1, /*Cons*/ "(height*width)*3",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ if_cenL, /*SrcPrt*/ 0, /*Prod*/ "(size)*1",
		/*Snk*/ bo_CostConstruction, /*SnkPrt*/ 1, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ if_cenR, /*SrcPrt*/ 0, /*Prod*/ "(size)*1",
		/*Snk*/ bo_CostConstruction, /*SnkPrt*/ 2, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ if_grayL, /*SrcPrt*/ 0, /*Prod*/ "(size)*1",
		/*Snk*/ bo_CostConstruction, /*SnkPrt*/ 3, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ if_grayR, /*SrcPrt*/ 0, /*Prod*/ "(size)*1",
		/*Snk*/ bo_CostConstruction, /*SnkPrt*/ 4, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	return graph;
}

// Method building PiSDFGraphDispComp
PiSDFGraph* DispComp(Archi* archi, Stack* stack){
	PiSDFGraph* graph = CREATE(stack, PiSDFGraph)(
		/*Edges*/    10,
		/*Params*/   8,
		/*InputIf*/  5,
		/*OutputIf*/ 1,
		/*Config*/   0,
		/*Body*/     3,
		/*Archi*/    archi,
		/*Stack*/    stack);

	/* Parameters */
	PiSDFParam *param_height = graph->addHeritedParam("height", 0);
	PiSDFParam *param_width = graph->addHeritedParam("width", 1);
	PiSDFParam *param_nIter = graph->addHeritedParam("nIter", 2);
	PiSDFParam *param_scale = graph->addHeritedParam("scale", 3);
	PiSDFParam *param_minDisp = graph->addHeritedParam("minDisp", 4);
	PiSDFParam *param_maxDisp = graph->addHeritedParam("maxDisp", 5);
	PiSDFParam *param_size = graph->addDependentParam("size", "height*width");
	PiSDFParam *param_nDisp = graph->addDependentParam("nDisp", "maxDisp-minDisp+1");

	/* Vertices */
	PiSDFVertex* bo_AggregateCost = graph->addBodyVertex(
		/*Name*/    "AggregateCost",
		/*FctId*/   DISPCOMP_AGGREGATECOST_FCT,
		/*InData*/  4,
		/*OutData*/ 1,
		/*InParam*/ 3);
	bo_AggregateCost->addInParam(0, param_height);
	bo_AggregateCost->addInParam(1, param_width);
	bo_AggregateCost->addInParam(2, param_nIter);
	bo_AggregateCost->isExecutableOnPE(CORE_CORE0);
	bo_AggregateCost->setTimingOnType(CORE_TYPE_X86, "1000", stack);

	PiSDFVertex* bo_DisparitySelect = graph->addBodyVertex(
		/*Name*/    "DisparitySelect",
		/*FctId*/   DISPCOMP_DISPARITYSELECT_FCT,
		/*InData*/  4,
		/*OutData*/ 2,
		/*InParam*/ 5);
	bo_DisparitySelect->addInParam(0, param_height);
	bo_DisparitySelect->addInParam(1, param_width);
	bo_DisparitySelect->addInParam(2, param_scale);
	bo_DisparitySelect->addInParam(3, param_minDisp);
	bo_DisparitySelect->addInParam(4, param_maxDisp);
	bo_DisparitySelect->isExecutableOnPE(CORE_CORE0);
	bo_DisparitySelect->setTimingOnType(CORE_TYPE_X86, "1000", stack);

	PiSDFVertex* bo_Br_Disp = graph->addSpecialVertex(
		/*Type*/    PISDF_SUBTYPE_BROADCAST,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 1);
	bo_Br_Disp->addInParam(0, param_size);

	PiSDFVertex* if_rawDisparity = graph->addOutputIf(
		/*Name*/    "if_rawDisparity",
		/*InParam*/ 1);
	if_rawDisparity->addInParam(0, param_size);

	PiSDFVertex* if_offsets = graph->addInputIf(
		/*Name*/    "if_offsets",
		/*InParam*/ 2);
	if_offsets->addInParam(0, param_nDisp);
	if_offsets->addInParam(1, param_nIter);

	PiSDFVertex* if_vWeights = graph->addInputIf(
		/*Name*/    "if_vWeights",
		/*InParam*/ 2);
	if_vWeights->addInParam(0, param_size);
	if_vWeights->addInParam(1, param_nIter);

	PiSDFVertex* if_hWeights = graph->addInputIf(
		/*Name*/    "if_hWeights",
		/*InParam*/ 2);
	if_hWeights->addInParam(0, param_size);
	if_hWeights->addInParam(1, param_nIter);

	PiSDFVertex* if_dispIx = graph->addInputIf(
		/*Name*/    "if_dispIx",
		/*InParam*/ 1);
	if_dispIx->addInParam(0, param_nDisp);

	PiSDFVertex* if_cost = graph->addInputIf(
		/*Name*/    "if_cost",
		/*InParam*/ 2);
	if_cost->addInParam(0, param_size);
	if_cost->addInParam(1, param_nDisp);


	/* Edges */
	graph->connect(
		/*Src*/ bo_DisparitySelect, /*SrcPrt*/ 1, /*Prod*/ "(width*height+1)*1",
		/*Snk*/ bo_DisparitySelect, /*SnkPrt*/ 3, /*Cons*/ "(width*height+1)*1",
		/*Delay*/ "(size+1)*1",0);

	graph->connect(
		/*Src*/ bo_DisparitySelect, /*SrcPrt*/ 0, /*Prod*/ "(height*width)*1",
		/*Snk*/ bo_Br_Disp, /*SnkPrt*/ 0, /*Cons*/ "(size)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br_Disp, /*SrcPrt*/ 0, /*Prod*/ "(size)*1",
		/*Snk*/ if_rawDisparity, /*SnkPrt*/ 0, /*Cons*/ "(size)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_Br_Disp, /*SrcPrt*/ 1, /*Prod*/ "(size)*1",
		/*Snk*/ bo_DisparitySelect, /*SnkPrt*/ 2, /*Cons*/ "(width*height)*1",
		/*Delay*/ "(size)*1",0);

	graph->connect(
		/*Src*/ if_offsets, /*SrcPrt*/ 0, /*Prod*/ "(nIter)*1",
		/*Snk*/ bo_AggregateCost, /*SnkPrt*/ 2, /*Cons*/ "(nIter)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ if_vWeights, /*SrcPrt*/ 0, /*Prod*/ "(size*nIter)*1",
		/*Snk*/ bo_AggregateCost, /*SnkPrt*/ 3, /*Cons*/ "(height*width*nIter)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ if_hWeights, /*SrcPrt*/ 0, /*Prod*/ "(size*nIter)*1",
		/*Snk*/ bo_AggregateCost, /*SnkPrt*/ 1, /*Cons*/ "(height*width*nIter)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ if_dispIx, /*SrcPrt*/ 0, /*Prod*/ "(nDisp)*1",
		/*Snk*/ bo_DisparitySelect, /*SnkPrt*/ 1, /*Cons*/ "(1)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ if_cost, /*SrcPrt*/ 0, /*Prod*/ "(size*nDisp)*1",
		/*Snk*/ bo_AggregateCost, /*SnkPrt*/ 0, /*Cons*/ "(width*height)*1",
		/*Delay*/ "0",0);

	graph->connect(
		/*Src*/ bo_AggregateCost, /*SrcPrt*/ 0, /*Prod*/ "(width*height)*1",
		/*Snk*/ bo_DisparitySelect, /*SnkPrt*/ 0, /*Cons*/ "(width*height)*1",
		/*Delay*/ "0",0);

	return graph;
}

void free_stereo(PiSDFGraph* top, Stack* stack){
	top->~PiSDFGraph();
	stack->free(top);
}
