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

#include <grt_definitions.h>

#include <spider.h>

void stereo_top(
		PiSDFGraph* _graphs,
		int height, int width,
		int maxDisp, int minDisp,
		int nbIter, int scale,
		int truncValue, int nbSlice,
		int overlap);

void stereo_costParallel(
		PiSDFGraph* graph,
		int height, int width,
		int maxDisp, int minDisp,
		int nbIter, int scale,
		int truncValue, int nbSlice,
		int overlap);

static PiSDFGraph* graphs;
static int nbGraphs = 0;

PiSDFGraph* addGraph(){
	if(nbGraphs >= MAX_NB_PiSDF_GRAPHS) exitWithCode(1054);
	PiSDFGraph* graph = &(graphs[nbGraphs++]);
	graph->reset();
	return graph;
}

void resetGraph(){
	nbGraphs = 0;
}

PiSDFGraph* initPisdf_stereo(
		PiSDFGraph* _graphs,
		int height, int width,
		int maxDisp, int minDisp,
		int nbIter, int scale,
		int truncValue, int nbSlice,
		int overlap){
	graphs = _graphs;

	PiSDFGraph* top = addGraph();

	PiSDFVertex *vxTop = (PiSDFVertex *)top->addVertex("top", normal_vertex);

	PiSDFGraph* mpSchedGraph = addGraph();
	vxTop->setSubGraph(mpSchedGraph);
	mpSchedGraph->setParentVertex(vxTop);

	stereo_top(mpSchedGraph,
			height, width,
			maxDisp, minDisp,
			nbIter, scale,
			truncValue, nbSlice,
			overlap);

	return top;
}

void stereo_top(
		PiSDFGraph* graph,
		int height, int width,
		int maxDisp, int minDisp,
		int nbIter, int scale,
		int truncValue, int nbSlice,
		int overlap){
	// Parameters.
	PiSDFParameter *paramHeight = graph->addParameter("Height");
	paramHeight->setValue(height);
	PiSDFParameter *paramWidth = graph->addParameter("Width");
	paramWidth->setValue(width);
	PiSDFParameter *paramSize = graph->addParameter("Size");
	paramSize->setValue(height*width);

	PiSDFParameter *paramZero = graph->addParameter("Zero");
	paramZero->setValue(0);
	PiSDFParameter *paramOne = graph->addParameter("One");
	paramOne->setValue(1);

	PiSDFParameter *paramMaxDisp = graph->addParameter("MaxDisp");
	paramMaxDisp->setValue(maxDisp);
	PiSDFParameter *paramMinDisp = graph->addParameter("MinDisp");
	paramMinDisp->setValue(minDisp);

	PiSDFParameter *paramNbIter = graph->addParameter("NbIter");
	paramNbIter->setValue(nbIter);
	PiSDFParameter *paramScale = graph->addParameter("Scale");
	paramScale->setValue(scale);

	PiSDFParameter *paramTruncValue = graph->addParameter("TruncValue");
	paramTruncValue->setValue(truncValue);
	PiSDFParameter *paramNbSlice = graph->addParameter("NbSlice");
	paramNbSlice->setValue(nbSlice);

	PiSDFParameter *paramOverlap = graph->addParameter("Overlap");
	paramOverlap->setValue(overlap);
	PiSDFParameter *paramSliceHeight = graph->addParameter("SliceHeight");
	paramSliceHeight->setValue(height/nbSlice+2*overlap);

	// Configure vertices.
//	PiSDFConfigVertex *vxConfig = (PiSDFConfigVertex *)graph->addVertex("config", config_vertex);
//	vxConfig->setFunction_index(0);
//	vxConfig->addParameter(paramNMAX);
//	vxConfig->addParameter(paramNVAL);
//	vxConfig->addRelatedParam(paramN);

	// Other vertices
	PiSDFVertex *vxRead_PPM0 = (PiSDFVertex *)graph->addVertex("Read_PPM0", normal_vertex);
	vxRead_PPM0->addParameter(paramHeight);
	vxRead_PPM0->addParameter(paramWidth);
	vxRead_PPM0->addParameter(paramZero);
	vxRead_PPM0->setFunction_index(0);

	PiSDFVertex *vxRead_PPM1 = (PiSDFVertex *)graph->addVertex("Read_PPM1", normal_vertex);
	vxRead_PPM1->addParameter(paramHeight);
	vxRead_PPM1->addParameter(paramWidth);
	vxRead_PPM1->addParameter(paramOne);
	vxRead_PPM1->setFunction_index(0);

	PiSDFVertex *vxBr0	= (PiSDFVertex *)graph->addVertex("BroadCast0", normal_vertex);
	vxBr0->setSubType(SubType_Broadcast);
	vxBr0->addParameter(paramSize);
	vxBr0->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxRGB2Gray_L = (PiSDFVertex *)graph->addVertex("RGB2Gray_L", normal_vertex);
	vxRGB2Gray_L->addParameter(paramSize);
	vxRGB2Gray_L->setFunction_index(1);

	PiSDFVertex *vxRGB2Gray_R = (PiSDFVertex *)graph->addVertex("RGB2Gray_L", normal_vertex);
	vxRGB2Gray_R->addParameter(paramSize);
	vxRGB2Gray_R->setFunction_index(1);

	PiSDFVertex *vxBr1	= (PiSDFVertex *)graph->addVertex("BroadCast1", normal_vertex);
	vxBr1->setSubType(SubType_Broadcast);
	vxBr1->addParameter(paramSize);
	vxBr1->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxBr2	= (PiSDFVertex *)graph->addVertex("BroadCast2", normal_vertex);
	vxBr2->setSubType(SubType_Broadcast);
	vxBr2->addParameter(paramSize);
	vxBr2->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxCensus_L = (PiSDFVertex *)graph->addVertex("Census_L", normal_vertex);
	vxCensus_L->addParameter(paramWidth);
	vxCensus_L->addParameter(paramHeight);
	vxCensus_L->setFunction_index(2);

	PiSDFVertex *vxCensus_R = (PiSDFVertex *)graph->addVertex("Census_R", normal_vertex);
	vxCensus_R->addParameter(paramWidth);
	vxCensus_R->addParameter(paramHeight);
	vxCensus_R->setFunction_index(2);

	PiSDFVertex *vxCost_Parallel_Work = (PiSDFVertex *)graph->addVertex("Cost_Parallel_Work", normal_vertex);
	vxCost_Parallel_Work->addParameter(paramTruncValue);
	vxCost_Parallel_Work->addParameter(paramScale);
	vxCost_Parallel_Work->addParameter(paramNbIter);
	vxCost_Parallel_Work->addParameter(paramMinDisp);
	vxCost_Parallel_Work->addParameter(paramMaxDisp);
	vxCost_Parallel_Work->addParameter(paramWidth);
	vxCost_Parallel_Work->addParameter(paramHeight);

	PiSDFVertex *vxSplit = (PiSDFVertex *)graph->addVertex("Split", normal_vertex);
	vxSplit->addParameter(paramOverlap);
	vxSplit->addParameter(paramNbSlice);
	vxSplit->addParameter(paramWidth);
	vxSplit->addParameter(paramHeight);
	vxSplit->setFunction_index(3);

	PiSDFVertex *vxMedian_Filter = (PiSDFVertex *)graph->addVertex("Median_Filter", normal_vertex);
	vxMedian_Filter->addParameter(paramHeight);
	vxMedian_Filter->addParameter(paramWidth);
	vxMedian_Filter->addParameter(paramNbSlice);
	vxMedian_Filter->addParameter(paramOverlap);
	vxMedian_Filter->setFunction_index(4);

	PiSDFVertex *vxWritePPM = (PiSDFVertex *)graph->addVertex("WritePPM", normal_vertex);
	vxWritePPM->addParameter(paramWidth);
	vxWritePPM->addParameter(paramHeight);
	vxWritePPM->setFunction_index(5);

	// Edges.
	graph->addEdge(vxRead_PPM0, 0, "Height*Width*3", vxBr0, 0, "3*Size", "0");
	graph->addEdge(vxBr0, 0, "3*Size", vxRGB2Gray_L, 0, "3*Size", "0");
	graph->addEdge(vxRead_PPM1, 0, "Height*Width*3", vxRGB2Gray_R, 0, "3*Size", "0");

	graph->addEdge(vxRGB2Gray_L, 0, "Size", vxBr1, 0, "Size", "0");
	graph->addEdge(vxBr1, 0, "Size", vxCensus_L, 0, "Height*Width", "0");

	graph->addEdge(vxRGB2Gray_R, 0, "Size", vxBr2, 0, "Size", "0");
	graph->addEdge(vxBr2, 0, "Size", vxCensus_R, 0, "Height*Width", "0");

	graph->addEdge(vxCensus_L,  0, "Height*Width", 	vxCost_Parallel_Work, 0, "Height*Width", "0");
	graph->addEdge(vxBr1, 		1, "Size",		   	vxCost_Parallel_Work, 1, "Height*Width", "0");
	graph->addEdge(vxBr0, 		1, "3*Size",	   	vxCost_Parallel_Work, 2, "3*Height*Width", "0");
	graph->addEdge(vxCensus_R,	0, "Height*Width",	vxCost_Parallel_Work, 3, "Height*Width", "0");
	graph->addEdge(vxBr2,		1, "Height*Width",	vxCost_Parallel_Work, 4, "Height*Width", "0");

	graph->addEdge(vxCost_Parallel_Work, 0, "Height*Width",	vxSplit, 0, "Height*Width", "0");
	graph->addEdge(vxSplit, 0, "NbSlice*(Height*Width/NbSlice+2*Overlap*Width)", vxMedian_Filter, 0, "Width*SliceHeight", "0");
	graph->addEdge(vxMedian_Filter, 0, "Width*SliceHeight-2*Overlap*Width", vxWritePPM, 0, "Width*Height", "0");

	// Timings
	vxRead_PPM0->setTiming(0,"100");
	vxRead_PPM1->setTiming(0,"100");
	vxBr0->setTiming(0,"100")	;
	vxRGB2Gray_L->setTiming(0,"100") ;
	vxRGB2Gray_R->setTiming(0,"100") ;
	vxBr1->setTiming(0,"100")	;
	vxBr2->setTiming(0,"100")	;
	vxCensus_L->setTiming(0,"100");
	vxCensus_R->setTiming(0,"100") ;
	vxSplit->setTiming(0,"100") ;
	vxMedian_Filter->setTiming(0,"100");
	vxWritePPM->setTiming(0,"100");

	// Subgraphs
	PiSDFGraph *Stereo_costParallel = addGraph();
	vxCost_Parallel_Work->setSubGraph(Stereo_costParallel);
	Stereo_costParallel->setParentVertex(vxCost_Parallel_Work);

	stereo_costParallel(Stereo_costParallel,
			height, width,
			maxDisp, minDisp,
			nbIter, scale,
			truncValue, nbSlice,
			overlap);
}


void stereo_costParallel(
		PiSDFGraph* graph,
		int height, int width,
		int maxDisp, int minDisp,
		int nbIter, int scale,
		int truncValue, int nbSlice,
		int overlap){
	/* Parameters */
	PiSDFParameter *paramHeight = graph->addParameter("Height2");
	paramHeight->setValue(height);
	PiSDFParameter *paramWidth = graph->addParameter("Width2");
	paramWidth->setValue(width);
	PiSDFParameter *paramSize = graph->addParameter("Size2");
	paramSize->setValue(height*width);

	PiSDFParameter *paramZero = graph->addParameter("Zero2");
	paramZero->setValue(0);
	PiSDFParameter *paramOne = graph->addParameter("One2");
	paramOne->setValue(1);

	PiSDFParameter *paramMaxDisp = graph->addParameter("MaxDisp2");
	paramMaxDisp->setValue(maxDisp);
	PiSDFParameter *paramMinDisp = graph->addParameter("MinDisp2");
	paramMinDisp->setValue(minDisp);

	PiSDFParameter *paramNbIter = graph->addParameter("NbIter2");
	paramNbIter->setValue(nbIter);
	PiSDFParameter *paramScale = graph->addParameter("Scale2");
	paramScale->setValue(scale);

	PiSDFParameter *paramTruncValue = graph->addParameter("TruncValue2");
	paramTruncValue->setValue(truncValue);

	PiSDFParameter *paramNbDisp = graph->addParameter("NbDisp2");
	paramNbDisp->setValue(maxDisp-minDisp);

	PiSDFParameter *paramVert = graph->addParameter("Vert2");
	paramVert->setValue(1);
	PiSDFParameter *paramHor = graph->addParameter("Hor2");
	paramHor->setValue(0);

	/* Interfaces */
	PiSDFIfVertex *ifCenL = (PiSDFIfVertex*)graph->addVertex("cenL", input_vertex);
	ifCenL->setDirection(0);
	ifCenL->setParentVertex(graph->getParentVertex());
	ifCenL->setParentEdge(graph->getParentVertex()->getInputEdge(0));
	ifCenL->setFunction_index(RB_FUNCT_IX);

	PiSDFIfVertex *ifGrayL = (PiSDFIfVertex*)graph->addVertex("grayL", input_vertex);
	ifGrayL->setDirection(0);
	ifGrayL->setParentVertex(graph->getParentVertex());
	ifGrayL->setParentEdge(graph->getParentVertex()->getInputEdge(1));
	ifGrayL->setFunction_index(RB_FUNCT_IX);

	PiSDFIfVertex *ifRgbL = (PiSDFIfVertex*)graph->addVertex("rgbL", input_vertex);
	ifRgbL->setDirection(0);
	ifRgbL->setParentVertex(graph->getParentVertex());
	ifRgbL->setParentEdge(graph->getParentVertex()->getInputEdge(2));
	ifRgbL->setFunction_index(RB_FUNCT_IX);

	PiSDFIfVertex *ifCenR = (PiSDFIfVertex*)graph->addVertex("cenR", input_vertex);
	ifCenR->setDirection(0);
	ifCenR->setParentVertex(graph->getParentVertex());
	ifCenR->setParentEdge(graph->getParentVertex()->getInputEdge(3));
	ifCenR->setFunction_index(RB_FUNCT_IX);

	PiSDFIfVertex *ifGrayR = (PiSDFIfVertex*)graph->addVertex("grayR", input_vertex);
	ifGrayR->setDirection(0);
	ifGrayR->setParentVertex(graph->getParentVertex());
	ifGrayR->setParentEdge(graph->getParentVertex()->getInputEdge(4));
	ifGrayR->setFunction_index(RB_FUNCT_IX);

	PiSDFIfVertex *ifRawD = (PiSDFIfVertex*)graph->addVertex("rawDisp", output_vertex);
	ifRawD->setDirection(1);
	ifRawD->setParentVertex(graph->getParentVertex());
	ifRawD->setParentEdge(graph->getParentVertex()->getInputEdge(0));
	ifRawD->setFunction_index(RB_FUNCT_IX);

	/* Vertices */
	PiSDFVertex *vxRb0	= (PiSDFVertex *)graph->addVertex("RoundBuffer0", normal_vertex);
	vxRb0->setSubType(SubType_RoundBuffer);
	vxRb0->addParameter(paramNbDisp);
	vxRb0->addParameter(paramNbIter);
	vxRb0->setFunction_index(RB_FUNCT_IX);

	PiSDFVertex *vxBr0	= (PiSDFVertex *)graph->addVertex("BroadCast0", normal_vertex);
	vxBr0->setSubType(SubType_Broadcast);
	vxBr0->addParameter(paramNbDisp);
	vxBr0->addParameter(paramNbIter);
	vxBr0->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxBr1	= (PiSDFVertex *)graph->addVertex("BroadCast1", normal_vertex);
	vxBr1->setSubType(SubType_Broadcast);
	vxBr1->addParameter(paramSize);
	vxBr1->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxRb2	= (PiSDFVertex *)graph->addVertex("RoundBuffer2", normal_vertex);
	vxRb2->setSubType(SubType_RoundBuffer);
	vxRb2->addParameter(paramNbDisp);
	vxRb2->addParameter(paramSize);
	vxRb2->addParameter(paramNbIter);
	vxRb2->setFunction_index(RB_FUNCT_IX);

	PiSDFVertex *vxRb3	= (PiSDFVertex *)graph->addVertex("RoundBuffer3", normal_vertex);
	vxRb3->setSubType(SubType_RoundBuffer);
	vxRb3->addParameter(paramNbDisp);
	vxRb3->addParameter(paramSize);
	vxRb3->addParameter(paramNbIter);
	vxRb3->setFunction_index(RB_FUNCT_IX);

	PiSDFVertex *vxBr4	= (PiSDFVertex *)graph->addVertex("BroadCast4", normal_vertex);
	vxBr4->setSubType(SubType_Broadcast);
	vxBr4->addParameter(paramNbDisp);
	vxBr4->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxBr5	= (PiSDFVertex *)graph->addVertex("BroadCast5", normal_vertex);
	vxBr5->setSubType(SubType_Broadcast);
	vxBr5->addParameter(paramSize);
	vxBr5->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxOffsetGen = (PiSDFVertex *)graph->addVertex("OffsetGen", normal_vertex);
	vxOffsetGen->addParameter(paramNbIter);
	vxOffsetGen->setFunction_index(6);

	PiSDFVertex *vxDispGen = (PiSDFVertex *)graph->addVertex("DispGen", normal_vertex);
	vxDispGen->addParameter(paramMinDisp);
	vxDispGen->addParameter(paramMaxDisp);
	vxDispGen->setFunction_index(7);

	PiSDFVertex *vxCompVertWeight = (PiSDFVertex *)graph->addVertex("CompVertWeight", normal_vertex);
	vxCompVertWeight->addParameter(paramVert);
	vxCompVertWeight->addParameter(paramWidth);
	vxCompVertWeight->addParameter(paramHeight);
	vxCompVertWeight->setFunction_index(8);

	PiSDFVertex *vxCompHorWeight = (PiSDFVertex *)graph->addVertex("CompHorWeight", normal_vertex);
	vxCompHorWeight->addParameter(paramHor);
	vxCompHorWeight->addParameter(paramWidth);
	vxCompHorWeight->addParameter(paramHeight);
	vxCompHorWeight->setFunction_index(8);

	PiSDFVertex *vxCostConstr = (PiSDFVertex *)graph->addVertex("CostConst", normal_vertex);
	vxCostConstr->addParameter(paramTruncValue);
	vxCostConstr->addParameter(paramWidth);
	vxCostConstr->addParameter(paramHeight);
	vxCostConstr->setFunction_index(9);

	PiSDFVertex *vxAggregateCost = (PiSDFVertex *)graph->addVertex("AggregateCost", normal_vertex);
	vxAggregateCost->addParameter(paramWidth);
	vxAggregateCost->addParameter(paramHeight);
	vxAggregateCost->addParameter(paramNbIter);
	vxAggregateCost->setFunction_index(10);

	PiSDFVertex *vxDispSelect = (PiSDFVertex *)graph->addVertex("DispSelect", normal_vertex);
	vxDispSelect->addParameter(paramScale);
	vxDispSelect->addParameter(paramWidth);
	vxDispSelect->addParameter(paramHeight);
	vxDispSelect->addParameter(paramMinDisp);
	vxDispSelect->addParameter(paramNbDisp);
	vxDispSelect->setFunction_index(11);

	/* Edges */
	graph->addEdge(ifRgbL, 0, "Size2*3", vxBr1, 0, "Size2*3", "0");
	graph->addEdge(ifGrayL, 0, "Size2", vxCostConstr, 0, "Size2", "0");
	graph->addEdge(ifGrayR, 0, "Size2", vxCostConstr, 1, "Size2", "0");
	graph->addEdge(ifCenL, 0, "Size2", vxCostConstr, 2, "Size2", "0");
	graph->addEdge(ifCenR, 0, "Size2", vxCostConstr, 3, "Size2", "0");

	graph->addEdge(vxOffsetGen, 0, "NbIter2", vxBr0, 0, "NbIter2", "0");

	graph->addEdge(vxDispGen, 0, "NbDisp2", vxBr4, 0, "NbDisp2", "0");

	graph->addEdge(vxBr0, 0, "NbIter2", vxRb0, 0, "NbIter2", "0");
	graph->addEdge(vxRb0, 0, "NbIter2*NbDisp2", vxAggregateCost, 0, "NbIter2", "0");
	graph->addEdge(vxBr0, 1, "NbIter2", vxCompVertWeight, 0, "1", "0");
	graph->addEdge(vxBr0, 2, "NbIter2", vxCompHorWeight, 0, "1", "0");

	graph->addEdge(vxBr1, 0, "Width2*Height2*3", vxCompVertWeight, 1, "Width2*Height2*3", "0");
	graph->addEdge(vxBr1, 1, "Width2*Height2*3", vxCompHorWeight, 1, "Width2*Height2*3", "0");

	graph->addEdge(vxCompVertWeight, 0, "Width2*Height2*3 *4", vxRb2, 0, "Size2*3*NbIter2 *4", "0");
	graph->addEdge(vxCompHorWeight, 0, "Width2*Height2*3 *4", vxRb3, 0, "Size2*3*NbIter2 *4", "0");

	graph->addEdge(vxRb2, 0, "Size2*3*NbIter2*NbDisp2 *4", vxAggregateCost, 1, "Height2*Width2*3*NbIter2 *4", "0");

	graph->addEdge(vxRb3, 0, "Size2*3*NbIter2*NbDisp2 *4", vxAggregateCost, 2, "Height2*Width2*3*NbIter2 *4", "0");

	graph->addEdge(vxBr4, 0, "NbDisp2", vxCostConstr, 4, "1", "0");
	graph->addEdge(vxBr4, 1, "NbDisp2", vxDispSelect, 1, "1", "0");

	graph->addEdge(vxCostConstr, 0, "Height2*Width2 *4", vxAggregateCost, 3, "Height2*Width2 *4", "0");

	graph->addEdge(vxAggregateCost, 0, "Height2*Width2 *4", vxDispSelect, 0, "Height2*Width2 *4", "0");

	graph->addEdge(vxDispSelect, 0, "Height2*Width2", vxBr5, 0, "Size2", "0");
	graph->addEdge(vxDispSelect, 1, "(Height2*Width2+1) *4", vxDispSelect, 3, "(Height2*Width2+1) *4", "(Height2*Width2+1) *4");

	graph->addEdge(vxBr5, 0, "Size2", ifRawD, 0, "Size2", "0");
	graph->addEdge(vxBr5, 1, "Size2", vxDispSelect, 2, "Size2", "Size2");

	/* Timings */
	vxOffsetGen->setTiming(0,"100");
	vxDispGen->setTiming(0,"100");
	vxCompVertWeight->setTiming(0,"100");
	vxCompHorWeight->setTiming(0,"100");
	vxCostConstr->setTiming(0,"100");
	vxAggregateCost->setTiming(0,"100");
	vxDispSelect->setTiming(0,"100");


	vxRb0->setTiming(0,"100");
	vxRb2->setTiming(0,"100");
	vxRb3->setTiming(0,"100");

}


