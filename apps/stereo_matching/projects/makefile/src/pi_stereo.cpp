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

PiSDFGraph* stereo();
PiSDFGraph* costParallel();
PiSDFGraph* DispComp();

/**
 * This is the method you need to call to build a complete PiSDF graph.
 */
void init_stereo(){
	PiSDFGraph* top = Spider::createGraph(
		/*Edges*/    0,
		/*Params*/   0,
		/*InputIf*/  0,
		/*OutputIf*/ 0,
		/*Config*/   0,
		/*Body*/     1);

	Spider::addHierVertex(
		/*Graph*/    top,
		/*Name*/     "top",
		/*Graph*/    stereo(),
		/*InputIf*/  0,
		/*OutputIf*/ 0,
		/*Params*/   0);

	Spider::setGraph(top);
}

// Method building PiSDFGraphstereo
PiSDFGraph* stereo(){
	PiSDFGraph* graph = Spider::createGraph(
		/*Edges*/    16,
		/*Params*/   11,
		/*InputIf*/  0,
		/*OutputIf*/ 0,
		/*Config*/   1,
		/*Body*/     12);

	/* Parameters */
	PiSDFParam *param_nSlice = Spider::addDynamicParam(graph, "nSlice");
	PiSDFParam *param_truncValue = Spider::addDynamicParam(graph, "truncValue");
	PiSDFParam *param_scale = Spider::addDynamicParam(graph, "scale");
	PiSDFParam *param_nIter = Spider::addDynamicParam(graph, "nIter");
	PiSDFParam *param_sizeFilter = Spider::addDynamicParam(graph, "sizeFilter");
	PiSDFParam *param_minDisp = Spider::addDynamicParam(graph, "minDisp");
	PiSDFParam *param_maxDisp = Spider::addDynamicParam(graph, "maxDisp");
	PiSDFParam *param_height = Spider::addDynamicParam(graph, "height");
	PiSDFParam *param_width = Spider::addDynamicParam(graph, "width");
	PiSDFParam *param_size = Spider::addDependentParam(graph, "size", "width*height");
	PiSDFParam *param_sliceHeight = Spider::addDependentParam(graph, "sliceHeight", "height/nSlice+2*sizeFilter");

	/* Vertices */
	PiSDFVertex* cf_Config = Spider::addConfigVertex(
		/*Graph*/   graph,
		/*Name*/    "Config",
		/*FctId*/   STEREO_CONFIG_FCT,
		/*SubType*/ PISDF_SUBTYPE_NORMAL,
		/*InData*/  0,
		/*OutData*/ 0,
		/*InParam*/ 0,
		/*OutParam*/9);
	Spider::addOutParam(cf_Config, 0, param_scale);
	Spider::addOutParam(cf_Config, 1, param_maxDisp);
	Spider::addOutParam(cf_Config, 2, param_minDisp);
	Spider::addOutParam(cf_Config, 3, param_nSlice);
	Spider::addOutParam(cf_Config, 4, param_truncValue);
	Spider::addOutParam(cf_Config, 5, param_height);
	Spider::addOutParam(cf_Config, 6, param_width);
	Spider::addOutParam(cf_Config, 7, param_sizeFilter);
	Spider::addOutParam(cf_Config, 8, param_nIter);
	Spider::isExecutableOnPE(cf_Config, CORE_CORE0);
	Spider::setTimingOnType(cf_Config, CORE_TYPE_X86, "1000");

	PiSDFVertex* bo_Camera = Spider::addBodyVertex(
		/*Graph*/   graph,
		/*Name*/    "Camera",
		/*FctId*/   STEREO_CAMERA_FCT,
		/*InData*/  0,
		/*OutData*/ 2,
		/*InParam*/ 2);
	Spider::addInParam(bo_Camera, 0, param_width);
	Spider::addInParam(bo_Camera, 1, param_height);
	Spider::isExecutableOnPE(bo_Camera, CORE_CORE0);
	Spider::setTimingOnType(bo_Camera, CORE_TYPE_X86, "1000");

	PiSDFVertex* bo_Br_rgbL = Spider::addSpecialVertex(
		/*Graph*/   graph,
		/*Type*/    PISDF_SUBTYPE_BROADCAST,
		/*InData*/  1,
		/*OutData*/ 3,
		/*InParam*/ 1);
	Spider::addInParam(bo_Br_rgbL, 0, param_size);

	PiSDFVertex* bo_RGB2Gray_L = Spider::addBodyVertex(
		/*Graph*/   graph,
		/*Name*/    "RGB2Gray_L",
		/*FctId*/   STEREO_RGB2GRAY_L_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 1);
	Spider::addInParam(bo_RGB2Gray_L, 0, param_size);
	Spider::isExecutableOnPE(bo_RGB2Gray_L, CORE_CORE0);
	Spider::setTimingOnType(bo_RGB2Gray_L, CORE_TYPE_X86, "1000");

	PiSDFVertex* bo_RGB2Gray_R = Spider::addBodyVertex(
		/*Graph*/   graph,
		/*Name*/    "RGB2Gray_R",
		/*FctId*/   STEREO_RGB2GRAY_R_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 1);
	Spider::addInParam(bo_RGB2Gray_R, 0, param_size);
	Spider::isExecutableOnPE(bo_RGB2Gray_R, CORE_CORE0);
	Spider::setTimingOnType(bo_RGB2Gray_R, CORE_TYPE_X86, "1000");

	PiSDFVertex* bo_Br_grayL = Spider::addSpecialVertex(
		/*Graph*/   graph,
		/*Type*/    PISDF_SUBTYPE_BROADCAST,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 1);
	Spider::addInParam(bo_Br_grayL, 0, param_size);

	PiSDFVertex* bo_Br_grayR = Spider::addSpecialVertex(
		/*Graph*/   graph,
		/*Type*/    PISDF_SUBTYPE_BROADCAST,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 1);
	Spider::addInParam(bo_Br_grayR, 0, param_size);

	PiSDFVertex* bo_Census_L = Spider::addBodyVertex(
		/*Graph*/   graph,
		/*Name*/    "Census_L",
		/*FctId*/   STEREO_CENSUS_L_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 2);
	Spider::addInParam(bo_Census_L, 0, param_width);
	Spider::addInParam(bo_Census_L, 1, param_height);
	Spider::isExecutableOnPE(bo_Census_L, CORE_CORE0);
	Spider::setTimingOnType(bo_Census_L, CORE_TYPE_X86, "1000");

	PiSDFVertex* bo_Census_R = Spider::addBodyVertex(
		/*Graph*/   graph,
		/*Name*/    "Census_R",
		/*FctId*/   STEREO_CENSUS_R_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 2);
	Spider::addInParam(bo_Census_R, 0, param_width);
	Spider::addInParam(bo_Census_R, 1, param_height);
	Spider::isExecutableOnPE(bo_Census_R, CORE_CORE0);
	Spider::setTimingOnType(bo_Census_R, CORE_TYPE_X86, "1000");

	PiSDFVertex* bo_Split = Spider::addBodyVertex(
		/*Graph*/   graph,
		/*Name*/    "Split",
		/*FctId*/   STEREO_SPLIT_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 4);
	Spider::addInParam(bo_Split, 0, param_width);
	Spider::addInParam(bo_Split, 1, param_height);
	Spider::addInParam(bo_Split, 2, param_nSlice);
	Spider::addInParam(bo_Split, 3, param_sizeFilter);
	Spider::isExecutableOnPE(bo_Split, CORE_CORE0);
	Spider::setTimingOnType(bo_Split, CORE_TYPE_X86, "1000");

	PiSDFVertex* bo_MedianFilter = Spider::addBodyVertex(
		/*Graph*/   graph,
		/*Name*/    "MedianFilter",
		/*FctId*/   STEREO_MEDIANFILTER_FCT,
		/*InData*/  1,
		/*OutData*/ 1,
		/*InParam*/ 3);
	Spider::addInParam(bo_MedianFilter, 0, param_width);
	Spider::addInParam(bo_MedianFilter, 1, param_sizeFilter);
	Spider::addInParam(bo_MedianFilter, 2, param_sliceHeight);
	Spider::isExecutableOnPE(bo_MedianFilter, CORE_CORE0);
	Spider::setTimingOnType(bo_MedianFilter, CORE_TYPE_X86, "1000");

	PiSDFVertex* bo_Display = Spider::addBodyVertex(
		/*Graph*/   graph,
		/*Name*/    "Display",
		/*FctId*/   STEREO_DISPLAY_FCT,
		/*InData*/  2,
		/*OutData*/ 0,
		/*InParam*/ 2);
	Spider::addInParam(bo_Display, 0, param_width);
	Spider::addInParam(bo_Display, 1, param_height);
	Spider::isExecutableOnPE(bo_Display, CORE_CORE0);
	Spider::setTimingOnType(bo_Display, CORE_TYPE_X86, "1000");

	PiSDFVertex* bo_costParallel = Spider::addHierVertex(
		/*Graph*/   graph,
		/*Name*/    "costParallel",
		/*Graph*/   costParallel(),
		/*InData*/  5,
		/*OutData*/ 1,
		/*InParam*/ 7);
	Spider::addInParam(bo_costParallel, 0, param_nIter);
	Spider::addInParam(bo_costParallel, 1, param_height);
	Spider::addInParam(bo_costParallel, 2, param_width);
	Spider::addInParam(bo_costParallel, 3, param_maxDisp);
	Spider::addInParam(bo_costParallel, 4, param_minDisp);
	Spider::addInParam(bo_costParallel, 5, param_truncValue);
	Spider::addInParam(bo_costParallel, 6, param_scale);


	/* Edges */
	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_RGB2Gray_L, /*SrcPrt*/ 0, /*Prod*/ "(size)*1",
		/*Snk*/ bo_Br_grayL, /*SnkPrt*/ 0, /*Cons*/ "(size)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_RGB2Gray_R, /*SrcPrt*/ 0, /*Prod*/ "(size)*1",
		/*Snk*/ bo_Br_grayR, /*SnkPrt*/ 0, /*Cons*/ "(size)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Camera, /*SrcPrt*/ 0, /*Prod*/ "(height*width)*3",
		/*Snk*/ bo_Br_rgbL, /*SnkPrt*/ 0, /*Cons*/ "(size)*3",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Br_rgbL, /*SrcPrt*/ 0, /*Prod*/ "(size)*3",
		/*Snk*/ bo_RGB2Gray_L, /*SnkPrt*/ 0, /*Cons*/ "(size)*3",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Br_rgbL, /*SrcPrt*/ 1, /*Prod*/ "(size)*3",
		/*Snk*/ bo_costParallel, /*SnkPrt*/ 4, /*Cons*/ "(width*height)*3",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Br_rgbL, /*SrcPrt*/ 2, /*Prod*/ "(size)*3",
		/*Snk*/ bo_Display, /*SnkPrt*/ 1, /*Cons*/ "(width*height)*3",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_costParallel, /*SrcPrt*/ 0, /*Prod*/ "(height*width)*1",
		/*Snk*/ bo_Split, /*SnkPrt*/ 0, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Split, /*SrcPrt*/ 0, /*Prod*/ "(nSlice*(height*width/nSlice+2*sizeFilter*width))*1",
		/*Snk*/ bo_MedianFilter, /*SnkPrt*/ 0, /*Cons*/ "(sliceHeight*width)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_MedianFilter, /*SrcPrt*/ 0, /*Prod*/ "(sliceHeight*width-2*sizeFilter*width)*1",
		/*Snk*/ bo_Display, /*SnkPrt*/ 0, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Br_grayL, /*SrcPrt*/ 0, /*Prod*/ "(size)*1",
		/*Snk*/ bo_Census_L, /*SnkPrt*/ 0, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Br_grayR, /*SrcPrt*/ 0, /*Prod*/ "(size)*1",
		/*Snk*/ bo_Census_R, /*SnkPrt*/ 0, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Br_grayL, /*SrcPrt*/ 1, /*Prod*/ "(size)*1",
		/*Snk*/ bo_costParallel, /*SnkPrt*/ 2, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Br_grayR, /*SrcPrt*/ 1, /*Prod*/ "(size)*1",
		/*Snk*/ bo_costParallel, /*SnkPrt*/ 3, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Census_L, /*SrcPrt*/ 0, /*Prod*/ "(height*width)*1",
		/*Snk*/ bo_costParallel, /*SnkPrt*/ 0, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Census_R, /*SrcPrt*/ 0, /*Prod*/ "(height*width)*1",
		/*Snk*/ bo_costParallel, /*SnkPrt*/ 1, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Camera, /*SrcPrt*/ 1, /*Prod*/ "(height*width)*3",
		/*Snk*/ bo_RGB2Gray_R, /*SnkPrt*/ 0, /*Cons*/ "(size)*3",
		/*Delay*/ "0",0);

	return graph;
}

// Method building PiSDFGraphcostParallel
PiSDFGraph* costParallel(){
	PiSDFGraph* graph = Spider::createGraph(
		/*Edges*/    18,
		/*Params*/   9,
		/*InputIf*/  5,
		/*OutputIf*/ 1,
		/*Config*/   0,
		/*Body*/     9);

	/* Parameters */
	PiSDFParam *param_nIter = Spider::addHeritedParam(graph, "nIter", 0);
	PiSDFParam *param_height = Spider::addHeritedParam(graph, "height", 1);
	PiSDFParam *param_width = Spider::addHeritedParam(graph, "width", 2);
	PiSDFParam *param_maxDisp = Spider::addHeritedParam(graph, "maxDisp", 3);
	PiSDFParam *param_minDisp = Spider::addHeritedParam(graph, "minDisp", 4);
	PiSDFParam *param_truncValue = Spider::addHeritedParam(graph, "truncValue", 5);
	PiSDFParam *param_scale = Spider::addHeritedParam(graph, "scale", 6);
	PiSDFParam *param_size = Spider::addDependentParam(graph, "size", "height*width");
	PiSDFParam *param_nDisp = Spider::addDependentParam(graph, "nDisp", "maxDisp-minDisp+1");

	/* Vertices */
	PiSDFVertex* if_cenL = Spider::addInputIf(
		/*Graph*/   graph,
		/*Name*/    "if_cenL",
		/*InParam*/ 1);
	Spider::addInParam(if_cenL, 0, param_size);

	PiSDFVertex* if_cenR = Spider::addInputIf(
		/*Graph*/   graph,
		/*Name*/    "if_cenR",
		/*InParam*/ 1);
	Spider::addInParam(if_cenR, 0, param_size);

	PiSDFVertex* if_grayL = Spider::addInputIf(
		/*Graph*/   graph,
		/*Name*/    "if_grayL",
		/*InParam*/ 1);
	Spider::addInParam(if_grayL, 0, param_size);

	PiSDFVertex* if_grayR = Spider::addInputIf(
		/*Graph*/   graph,
		/*Name*/    "if_grayR",
		/*InParam*/ 1);
	Spider::addInParam(if_grayR, 0, param_size);

	PiSDFVertex* if_rgb = Spider::addInputIf(
		/*Graph*/   graph,
		/*Name*/    "if_rgb",
		/*InParam*/ 1);
	Spider::addInParam(if_rgb, 0, param_size);

	PiSDFVertex* bo_Br_rgb = Spider::addSpecialVertex(
		/*Graph*/   graph,
		/*Type*/    PISDF_SUBTYPE_BROADCAST,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 1);
	Spider::addInParam(bo_Br_rgb, 0, param_size);

	PiSDFVertex* bo_GenIx = Spider::addBodyVertex(
		/*Graph*/   graph,
		/*Name*/    "GenIx",
		/*FctId*/   COSTPARALLEL_GENIX_FCT,
		/*InData*/  0,
		/*OutData*/ 1,
		/*InParam*/ 1);
	Spider::addInParam(bo_GenIx, 0, param_nIter);
	Spider::isExecutableOnPE(bo_GenIx, CORE_CORE0);
	Spider::setTimingOnType(bo_GenIx, CORE_TYPE_X86, "1000");

	PiSDFVertex* bo_GenDisp = Spider::addBodyVertex(
		/*Graph*/   graph,
		/*Name*/    "GenDisp",
		/*FctId*/   COSTPARALLEL_GENDISP_FCT,
		/*InData*/  0,
		/*OutData*/ 1,
		/*InParam*/ 2);
	Spider::addInParam(bo_GenDisp, 0, param_minDisp);
	Spider::addInParam(bo_GenDisp, 1, param_maxDisp);
	Spider::isExecutableOnPE(bo_GenDisp, CORE_CORE0);
	Spider::setTimingOnType(bo_GenDisp, CORE_TYPE_X86, "1000");

	PiSDFVertex* bo_VWeights = Spider::addBodyVertex(
		/*Graph*/   graph,
		/*Name*/    "VWeights",
		/*FctId*/   COSTPARALLEL_VWEIGHTS_FCT,
		/*InData*/  2,
		/*OutData*/ 1,
		/*InParam*/ 2);
	Spider::addInParam(bo_VWeights, 0, param_height);
	Spider::addInParam(bo_VWeights, 1, param_width);
	Spider::isExecutableOnPE(bo_VWeights, CORE_CORE0);
	Spider::setTimingOnType(bo_VWeights, CORE_TYPE_X86, "1000");

	PiSDFVertex* bo_HWeight = Spider::addBodyVertex(
		/*Graph*/   graph,
		/*Name*/    "HWeight",
		/*FctId*/   COSTPARALLEL_HWEIGHT_FCT,
		/*InData*/  2,
		/*OutData*/ 1,
		/*InParam*/ 2);
	Spider::addInParam(bo_HWeight, 0, param_height);
	Spider::addInParam(bo_HWeight, 1, param_width);
	Spider::isExecutableOnPE(bo_HWeight, CORE_CORE0);
	Spider::setTimingOnType(bo_HWeight, CORE_TYPE_X86, "1000");

	PiSDFVertex* bo_CostConstruction = Spider::addBodyVertex(
		/*Graph*/   graph,
		/*Name*/    "CostConstruction",
		/*FctId*/   COSTPARALLEL_COSTCONSTRUCTION_FCT,
		/*InData*/  5,
		/*OutData*/ 1,
		/*InParam*/ 3);
	Spider::addInParam(bo_CostConstruction, 0, param_truncValue);
	Spider::addInParam(bo_CostConstruction, 1, param_height);
	Spider::addInParam(bo_CostConstruction, 2, param_width);
	Spider::isExecutableOnPE(bo_CostConstruction, CORE_CORE0);
	Spider::setTimingOnType(bo_CostConstruction, CORE_TYPE_X86, "1000");

	PiSDFVertex* if_rawDisparity = Spider::addOutputIf(
		/*Graph*/   graph,
		/*Name*/    "if_rawDisparity",
		/*InParam*/ 1);
	Spider::addInParam(if_rawDisparity, 0, param_size);

	PiSDFVertex* bo_Br_Ix = Spider::addSpecialVertex(
		/*Graph*/   graph,
		/*Type*/    PISDF_SUBTYPE_BROADCAST,
		/*InData*/  1,
		/*OutData*/ 3,
		/*InParam*/ 1);
	Spider::addInParam(bo_Br_Ix, 0, param_nIter);

	PiSDFVertex* bo_Br_Disp = Spider::addSpecialVertex(
		/*Graph*/   graph,
		/*Type*/    PISDF_SUBTYPE_BROADCAST,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 1);
	Spider::addInParam(bo_Br_Disp, 0, param_nDisp);

	PiSDFVertex* bo_DispComp = Spider::addHierVertex(
		/*Graph*/   graph,
		/*Name*/    "DispComp",
		/*Graph*/   DispComp(),
		/*InData*/  5,
		/*OutData*/ 1,
		/*InParam*/ 6);
	Spider::addInParam(bo_DispComp, 0, param_height);
	Spider::addInParam(bo_DispComp, 1, param_width);
	Spider::addInParam(bo_DispComp, 2, param_nIter);
	Spider::addInParam(bo_DispComp, 3, param_scale);
	Spider::addInParam(bo_DispComp, 4, param_minDisp);
	Spider::addInParam(bo_DispComp, 5, param_maxDisp);


	/* Edges */
	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_DispComp, /*SrcPrt*/ 0, /*Prod*/ "(height*width)*1",
		/*Snk*/ if_rawDisparity, /*SnkPrt*/ 0, /*Cons*/ "(size)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_CostConstruction, /*SrcPrt*/ 0, /*Prod*/ "(height*width)*1",
		/*Snk*/ bo_DispComp, /*SnkPrt*/ 4, /*Cons*/ "(height*width*(maxDisp-minDisp+1))*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_VWeights, /*SrcPrt*/ 0, /*Prod*/ "(height*width)*1",
		/*Snk*/ bo_DispComp, /*SnkPrt*/ 1, /*Cons*/ "(height*width*nIter)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_HWeight, /*SrcPrt*/ 0, /*Prod*/ "(height*width)*1",
		/*Snk*/ bo_DispComp, /*SnkPrt*/ 2, /*Cons*/ "(height*width*nIter)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ if_rgb, /*SrcPrt*/ 0, /*Prod*/ "(size)*3",
		/*Snk*/ bo_Br_rgb, /*SnkPrt*/ 0, /*Cons*/ "(size)*3",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_GenIx, /*SrcPrt*/ 0, /*Prod*/ "(nIter)*1",
		/*Snk*/ bo_Br_Ix, /*SnkPrt*/ 0, /*Cons*/ "(nIter)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_GenDisp, /*SrcPrt*/ 0, /*Prod*/ "(maxDisp-minDisp+1)*1",
		/*Snk*/ bo_Br_Disp, /*SnkPrt*/ 0, /*Cons*/ "(nDisp)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Br_Disp, /*SrcPrt*/ 0, /*Prod*/ "(nDisp)*1",
		/*Snk*/ bo_CostConstruction, /*SnkPrt*/ 0, /*Cons*/ "(1)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Br_Disp, /*SrcPrt*/ 1, /*Prod*/ "(nDisp)*1",
		/*Snk*/ bo_DispComp, /*SnkPrt*/ 3, /*Cons*/ "(maxDisp-minDisp+1)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Br_Ix, /*SrcPrt*/ 0, /*Prod*/ "(nIter)*1",
		/*Snk*/ bo_DispComp, /*SnkPrt*/ 0, /*Cons*/ "(nIter)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Br_Ix, /*SrcPrt*/ 1, /*Prod*/ "(nIter)*1",
		/*Snk*/ bo_HWeight, /*SnkPrt*/ 0, /*Cons*/ "(1)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Br_Ix, /*SrcPrt*/ 2, /*Prod*/ "(nIter)*1",
		/*Snk*/ bo_VWeights, /*SnkPrt*/ 0, /*Cons*/ "(1)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Br_rgb, /*SrcPrt*/ 0, /*Prod*/ "(size)*3",
		/*Snk*/ bo_HWeight, /*SnkPrt*/ 1, /*Cons*/ "(height*width)*3",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Br_rgb, /*SrcPrt*/ 1, /*Prod*/ "(size)*3",
		/*Snk*/ bo_VWeights, /*SnkPrt*/ 1, /*Cons*/ "(height*width)*3",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ if_cenL, /*SrcPrt*/ 0, /*Prod*/ "(size)*1",
		/*Snk*/ bo_CostConstruction, /*SnkPrt*/ 1, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ if_cenR, /*SrcPrt*/ 0, /*Prod*/ "(size)*1",
		/*Snk*/ bo_CostConstruction, /*SnkPrt*/ 2, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ if_grayL, /*SrcPrt*/ 0, /*Prod*/ "(size)*1",
		/*Snk*/ bo_CostConstruction, /*SnkPrt*/ 3, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ if_grayR, /*SrcPrt*/ 0, /*Prod*/ "(size)*1",
		/*Snk*/ bo_CostConstruction, /*SnkPrt*/ 4, /*Cons*/ "(height*width)*1",
		/*Delay*/ "0",0);

	return graph;
}

// Method building PiSDFGraphDispComp
PiSDFGraph* DispComp(){
	PiSDFGraph* graph = Spider::createGraph(
		/*Edges*/    10,
		/*Params*/   8,
		/*InputIf*/  5,
		/*OutputIf*/ 1,
		/*Config*/   0,
		/*Body*/     3);

	/* Parameters */
	PiSDFParam *param_height = Spider::addHeritedParam(graph, "height", 0);
	PiSDFParam *param_width = Spider::addHeritedParam(graph, "width", 1);
	PiSDFParam *param_nIter = Spider::addHeritedParam(graph, "nIter", 2);
	PiSDFParam *param_scale = Spider::addHeritedParam(graph, "scale", 3);
	PiSDFParam *param_minDisp = Spider::addHeritedParam(graph, "minDisp", 4);
	PiSDFParam *param_maxDisp = Spider::addHeritedParam(graph, "maxDisp", 5);
	PiSDFParam *param_size = Spider::addDependentParam(graph, "size", "height*width");
	PiSDFParam *param_nDisp = Spider::addDependentParam(graph, "nDisp", "maxDisp-minDisp+1");

	/* Vertices */
	PiSDFVertex* bo_AggregateCost = Spider::addBodyVertex(
		/*Graph*/   graph,
		/*Name*/    "AggregateCost",
		/*FctId*/   DISPCOMP_AGGREGATECOST_FCT,
		/*InData*/  4,
		/*OutData*/ 1,
		/*InParam*/ 3);
	Spider::addInParam(bo_AggregateCost, 0, param_height);
	Spider::addInParam(bo_AggregateCost, 1, param_width);
	Spider::addInParam(bo_AggregateCost, 2, param_nIter);
	Spider::isExecutableOnPE(bo_AggregateCost, CORE_CORE0);
	Spider::setTimingOnType(bo_AggregateCost, CORE_TYPE_X86, "1000");

	PiSDFVertex* bo_DisparitySelect = Spider::addBodyVertex(
		/*Graph*/   graph,
		/*Name*/    "DisparitySelect",
		/*FctId*/   DISPCOMP_DISPARITYSELECT_FCT,
		/*InData*/  4,
		/*OutData*/ 2,
		/*InParam*/ 5);
	Spider::addInParam(bo_DisparitySelect, 0, param_height);
	Spider::addInParam(bo_DisparitySelect, 1, param_width);
	Spider::addInParam(bo_DisparitySelect, 2, param_scale);
	Spider::addInParam(bo_DisparitySelect, 3, param_minDisp);
	Spider::addInParam(bo_DisparitySelect, 4, param_maxDisp);
	Spider::isExecutableOnPE(bo_DisparitySelect, CORE_CORE0);
	Spider::setTimingOnType(bo_DisparitySelect, CORE_TYPE_X86, "1000");

	PiSDFVertex* bo_Br_Disp = Spider::addSpecialVertex(
		/*Graph*/   graph,
		/*Type*/    PISDF_SUBTYPE_BROADCAST,
		/*InData*/  1,
		/*OutData*/ 2,
		/*InParam*/ 1);
	Spider::addInParam(bo_Br_Disp, 0, param_size);

	PiSDFVertex* if_rawDisparity = Spider::addOutputIf(
		/*Graph*/   graph,
		/*Name*/    "if_rawDisparity",
		/*InParam*/ 1);
	Spider::addInParam(if_rawDisparity, 0, param_size);

	PiSDFVertex* if_offsets = Spider::addInputIf(
		/*Graph*/   graph,
		/*Name*/    "if_offsets",
		/*InParam*/ 2);
	Spider::addInParam(if_offsets, 0, param_nDisp);
	Spider::addInParam(if_offsets, 1, param_nIter);

	PiSDFVertex* if_vWeights = Spider::addInputIf(
		/*Graph*/   graph,
		/*Name*/    "if_vWeights",
		/*InParam*/ 2);
	Spider::addInParam(if_vWeights, 0, param_size);
	Spider::addInParam(if_vWeights, 1, param_nIter);

	PiSDFVertex* if_hWeights = Spider::addInputIf(
		/*Graph*/   graph,
		/*Name*/    "if_hWeights",
		/*InParam*/ 2);
	Spider::addInParam(if_hWeights, 0, param_size);
	Spider::addInParam(if_hWeights, 1, param_nIter);

	PiSDFVertex* if_dispIx = Spider::addInputIf(
		/*Graph*/   graph,
		/*Name*/    "if_dispIx",
		/*InParam*/ 1);
	Spider::addInParam(if_dispIx, 0, param_nDisp);

	PiSDFVertex* if_cost = Spider::addInputIf(
		/*Graph*/   graph,
		/*Name*/    "if_cost",
		/*InParam*/ 2);
	Spider::addInParam(if_cost, 0, param_size);
	Spider::addInParam(if_cost, 1, param_nDisp);


	/* Edges */
	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_DisparitySelect, /*SrcPrt*/ 1, /*Prod*/ "(width*height+1)*1",
		/*Snk*/ bo_DisparitySelect, /*SnkPrt*/ 3, /*Cons*/ "(width*height+1)*1",
		/*Delay*/ "(size+1)*1",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_DisparitySelect, /*SrcPrt*/ 0, /*Prod*/ "(height*width)*1",
		/*Snk*/ bo_Br_Disp, /*SnkPrt*/ 0, /*Cons*/ "(size)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Br_Disp, /*SrcPrt*/ 0, /*Prod*/ "(size)*1",
		/*Snk*/ if_rawDisparity, /*SnkPrt*/ 0, /*Cons*/ "(size)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_Br_Disp, /*SrcPrt*/ 1, /*Prod*/ "(size)*1",
		/*Snk*/ bo_DisparitySelect, /*SnkPrt*/ 2, /*Cons*/ "(width*height)*1",
		/*Delay*/ "(size)*1",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ if_offsets, /*SrcPrt*/ 0, /*Prod*/ "(nIter)*1",
		/*Snk*/ bo_AggregateCost, /*SnkPrt*/ 2, /*Cons*/ "(nIter)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ if_vWeights, /*SrcPrt*/ 0, /*Prod*/ "(size*nIter)*1",
		/*Snk*/ bo_AggregateCost, /*SnkPrt*/ 3, /*Cons*/ "(height*width*nIter)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ if_hWeights, /*SrcPrt*/ 0, /*Prod*/ "(size*nIter)*1",
		/*Snk*/ bo_AggregateCost, /*SnkPrt*/ 1, /*Cons*/ "(height*width*nIter)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ if_dispIx, /*SrcPrt*/ 0, /*Prod*/ "(nDisp)*1",
		/*Snk*/ bo_DisparitySelect, /*SnkPrt*/ 1, /*Cons*/ "(1)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ if_cost, /*SrcPrt*/ 0, /*Prod*/ "(size*nDisp)*1",
		/*Snk*/ bo_AggregateCost, /*SnkPrt*/ 0, /*Cons*/ "(width*height)*1",
		/*Delay*/ "0",0);

	Spider::connect(
		/*Graph*/   graph,
		/*Src*/ bo_AggregateCost, /*SrcPrt*/ 0, /*Prod*/ "(width*height)*1",
		/*Snk*/ bo_DisparitySelect, /*SnkPrt*/ 0, /*Cons*/ "(width*height)*1",
		/*Delay*/ "0",0);

	return graph;
}

void free_stereo(){
	Spider::cleanPiSDF();
}
