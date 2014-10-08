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

void stereo_top(PiSDFGraph* _graphs);
void stereo_stereo(PiSDFGraph* graph);
void stereo_topDisp(PiSDFGraph* graph);

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

PiSDFGraph* initPisdf_stereo(PiSDFGraph* _graphs){
	graphs = _graphs;

	PiSDFGraph* top = addGraph();

	PiSDFVertex *vxTop = (PiSDFVertex *)top->addVertex("top", normal_vertex);

	PiSDFGraph* mpSchedGraph = addGraph();
	vxTop->setSubGraph(mpSchedGraph);
	mpSchedGraph->setParentVertex(vxTop);

	stereo_top(mpSchedGraph);

	return top;
}

void stereo_top(PiSDFGraph* graph){

	// Parameters.
	PiSDFParameter *paramHeight = graph->addParameter("height");
	PiSDFParameter *paramWidth = graph->addParameter("width");

	PiSDFParameter *paramZero = graph->addParameter("0");
	paramZero->setValue(0);
	PiSDFParameter *paramOne = graph->addParameter("1");
	paramOne->setValue(1);

	PiSDFParameter *paramNbDisp = graph->addParameter("nbDisp");
	PiSDFParameter *paramNbIter = graph->addParameter("nbIter");
	PiSDFParameter *paramNbSlices = graph->addParameter("nbSlices");

	PiSDFParameter *paramVideoInput = graph->addParameter("videoInput");

	// Configure vertices.
	PiSDFConfigVertex *vxConfig = (PiSDFConfigVertex *)graph->addVertex("config", config_vertex);
	vxConfig->setFunction_index(15);
	vxConfig->addRelatedParam(paramWidth);
	vxConfig->addRelatedParam(paramHeight);
	vxConfig->addRelatedParam(paramNbDisp);
	vxConfig->addRelatedParam(paramNbIter);
	vxConfig->addRelatedParam(paramNbSlices);
	vxConfig->addRelatedParam(paramVideoInput);

	// Special vertices
	PiSDFVertex *vxBr_Lr = (PiSDFVertex *)graph->addVertex("Br_Lr", normal_vertex);
	vxBr_Lr->setSubType(SubType_Broadcast);
	vxBr_Lr->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxBr_Lg = (PiSDFVertex *)graph->addVertex("Br_Lg", normal_vertex);
	vxBr_Lg->setSubType(SubType_Broadcast);
	vxBr_Lg->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxBr_Lb = (PiSDFVertex *)graph->addVertex("Br_Lb", normal_vertex);
	vxBr_Lb->setSubType(SubType_Broadcast);
	vxBr_Lb->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxBr_Rr = (PiSDFVertex *)graph->addVertex("Br_Rr", normal_vertex);
	vxBr_Rr->setSubType(SubType_Broadcast);
	vxBr_Rr->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxBr_Rg = (PiSDFVertex *)graph->addVertex("Br_Rg", normal_vertex);
	vxBr_Rg->setSubType(SubType_Broadcast);
	vxBr_Rg->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxBr_Rb = (PiSDFVertex *)graph->addVertex("Br_Rb", normal_vertex);
	vxBr_Rb->setSubType(SubType_Broadcast);
	vxBr_Rb->setFunction_index(BROADCAST_FUNCT_IX);

	// Other vertices
	PiSDFVertex *vxCam_L = (PiSDFVertex *)graph->addVertex("Cam_L", normal_vertex);
	vxCam_L->addParameter(paramWidth);
	vxCam_L->addParameter(paramHeight);
	vxCam_L->addParameter(paramZero);
	vxCam_L->addParameter(paramVideoInput);
	vxCam_L->setFunction_index(0);

	PiSDFVertex *vxCam_R = (PiSDFVertex *)graph->addVertex("Cam_R", normal_vertex);
	vxCam_R->addParameter(paramWidth);
	vxCam_R->addParameter(paramHeight);
	vxCam_R->addParameter(paramOne);
	vxCam_R->addParameter(paramVideoInput);
	vxCam_R->setFunction_index(0);

	PiSDFVertex *vxStereoTop = (PiSDFVertex *)graph->addVertex("StereoTop", normal_vertex);
	vxStereoTop->addParameter(paramWidth);
	vxStereoTop->addParameter(paramHeight);
	vxStereoTop->addParameter(paramNbDisp);
	vxStereoTop->addParameter(paramNbIter);
	vxStereoTop->addParameter(paramNbSlices);

//	PiSDFVertex *vxStereoMono = (PiSDFVertex *)graph->addVertex("vxStereoMono", normal_vertex);
//	vxStereoMono->addParameter(paramWidth);
//	vxStereoMono->addParameter(paramHeight);
//	vxStereoMono->addParameter(paramNbDisp);
//	vxStereoMono->addParameter(paramNbIter);
//	vxStereoMono->addParameter(paramNbSlices);
//	vxStereoMono->setFunction_index(16);

	PiSDFVertex *vxSplit = (PiSDFVertex *)graph->addVertex("vxSplit", normal_vertex);
	vxSplit->addParameter(paramWidth);
	vxSplit->addParameter(paramHeight);
	vxSplit->addParameter(paramNbSlices);
	vxSplit->setFunction_index(11);

	PiSDFVertex *vxMedianSlice = (PiSDFVertex *)graph->addVertex("vxMedianSlice", normal_vertex);
	vxMedianSlice->addParameter(paramWidth);
	vxMedianSlice->addParameter(paramHeight);
	vxMedianSlice->addParameter(paramNbSlices);
	vxMedianSlice->setFunction_index(12);

	PiSDFVertex *vxDisplay = (PiSDFVertex *)graph->addVertex("Display", normal_vertex);
	vxDisplay->addParameter(paramWidth);
	vxDisplay->addParameter(paramHeight);
	vxDisplay->addParameter(paramNbDisp);
	vxDisplay->setFunction_index(1);

	// Edges.
	graph->addEdge(vxCam_L, 0, "height*width", vxBr_Lr, 0, "height*width", "0");
	graph->addEdge(vxCam_L, 1, "height*width", vxBr_Lg, 0, "height*width", "0");
	graph->addEdge(vxCam_L, 2, "height*width", vxBr_Lb, 0, "height*width", "0");

	graph->addEdge(vxCam_R, 0, "height*width", vxBr_Rr, 0, "height*width", "0");
	graph->addEdge(vxCam_R, 1, "height*width", vxBr_Rg, 0, "height*width", "0");
	graph->addEdge(vxCam_R, 2, "height*width", vxBr_Rb, 0, "height*width", "0");

	graph->addEdge(vxBr_Lr, 0, "height*width", vxStereoTop, 0, "height*width", "0");
	graph->addEdge(vxBr_Lg, 0, "height*width", vxStereoTop, 1, "height*width", "0");
	graph->addEdge(vxBr_Lb, 0, "height*width", vxStereoTop, 2, "height*width", "0");
	graph->addEdge(vxBr_Rr, 0, "height*width", vxStereoTop, 3, "height*width", "0");
	graph->addEdge(vxBr_Rg, 0, "height*width", vxStereoTop, 4, "height*width", "0");
	graph->addEdge(vxBr_Rb, 0, "height*width", vxStereoTop, 5, "height*width", "0");

//	graph->addEdge(vxBr_Lr, 1, "height*width", vxStereoMono, 0, "height*width", "0");
//	graph->addEdge(vxBr_Lg, 1, "height*width", vxStereoMono, 1, "height*width", "0");
//	graph->addEdge(vxBr_Lb, 1, "height*width", vxStereoMono, 2, "height*width", "0");
//	graph->addEdge(vxBr_Rr, 1, "height*width", vxStereoMono, 3, "height*width", "0");
//	graph->addEdge(vxBr_Rg, 1, "height*width", vxStereoMono, 4, "height*width", "0");
//	graph->addEdge(vxBr_Rb, 1, "height*width", vxStereoMono, 5, "height*width", "0");

	graph->addEdge(vxBr_Lr, 1, "height*width", vxDisplay, 0, "height*width", "0");
	graph->addEdge(vxBr_Lg, 1, "height*width", vxDisplay, 1, "height*width", "0");
	graph->addEdge(vxBr_Lb, 1, "height*width", vxDisplay, 2, "height*width", "0");
	graph->addEdge(vxBr_Rr, 1, "height*width", vxDisplay, 3, "height*width", "0");
	graph->addEdge(vxBr_Rg, 1, "height*width", vxDisplay, 4, "height*width", "0");
	graph->addEdge(vxBr_Rb, 1, "height*width", vxDisplay, 5, "height*width", "0");
//	graph->addEdge(vxStereoMono, 0, "height*width", vxDisplay, 7, "height*width", "0");

	graph->addEdge(vxStereoTop, 0, "height*width", vxSplit, 0, "height*width", "0");
	graph->addEdge(vxSplit, 0, "(height+2*nbSlices)*width", vxMedianSlice, 0, "(2+height/nbSlices)*width", "0");
	graph->addEdge(vxMedianSlice, 0, "height*width/nbSlices", vxDisplay, 6, "height*width", "0");

	// Timings
	vxConfig->setTiming(0, "100");
	vxBr_Lr->setTiming(0, "100");
	vxBr_Lg->setTiming(0, "100");
	vxBr_Lb->setTiming(0, "100");
	vxBr_Rr->setTiming(0, "100");
	vxBr_Rg->setTiming(0, "100");
	vxBr_Rb->setTiming(0, "100");
	vxCam_L->setTiming(0, "100");
	vxCam_R->setTiming(0, "100");
	vxSplit->setTiming(0, "100");
	vxMedianSlice->setTiming(0, "100");
	vxDisplay->setTiming(0, "100");

	vxConfig->setTiming(1, "100");
	vxBr_Lr->setTiming(1, "100");
	vxBr_Lg->setTiming(1, "100");
	vxBr_Lb->setTiming(1, "100");
	vxBr_Rr->setTiming(1, "100");
	vxBr_Rg->setTiming(1, "100");
	vxBr_Rb->setTiming(1, "100");
	vxSplit->setTiming(1, "100");
	vxMedianSlice->setTiming(1, "100");

	// Subgraphs
	PiSDFGraph *Stereo_sub = addGraph();
	vxStereoTop->setSubGraph(Stereo_sub);
	Stereo_sub->setParentVertex(vxStereoTop);

	stereo_stereo(Stereo_sub);
}


void stereo_stereo(PiSDFGraph* graph){
	/* Parameters */
	PiSDFParameter *paramWidth = graph->addParameter("width2");
	paramWidth->setParameterParentID(0);
	PiSDFParameter *paramHeight = graph->addParameter("height2");
	paramHeight->setParameterParentID(1);

	PiSDFParameter *paramZero = graph->addParameter("0");
	paramZero->setValue(0);
	PiSDFParameter *paramOne = graph->addParameter("1");
	paramOne->setValue(1);

	PiSDFParameter *paramNbDisp = graph->addParameter("nbDisp2");
	paramNbDisp->setParameterParentID(2);
	PiSDFParameter *paramNbIter = graph->addParameter("nbIter2");
	paramNbIter->setParameterParentID(3);
	PiSDFParameter *paramNbSlices = graph->addParameter("nbSlices2");
	paramNbSlices->setParameterParentID(4);

	/* Interfaces */
	PiSDFIfVertex *ifLr = (PiSDFIfVertex*)graph->addVertex("ifLr", input_vertex);
	ifLr->setDirection(0);
	ifLr->setParentVertex(graph->getParentVertex());
	ifLr->setParentEdge(graph->getParentVertex()->getInputEdge(0));
	ifLr->setFunction_index(RB_FUNCT_IX);

	PiSDFIfVertex *ifLg = (PiSDFIfVertex*)graph->addVertex("ifLg", input_vertex);
	ifLg->setDirection(0);
	ifLg->setParentVertex(graph->getParentVertex());
	ifLg->setParentEdge(graph->getParentVertex()->getInputEdge(1));
	ifLg->setFunction_index(RB_FUNCT_IX);

	PiSDFIfVertex *ifLb = (PiSDFIfVertex*)graph->addVertex("ifLb", input_vertex);
	ifLb->setDirection(0);
	ifLb->setParentVertex(graph->getParentVertex());
	ifLb->setParentEdge(graph->getParentVertex()->getInputEdge(2));
	ifLb->setFunction_index(RB_FUNCT_IX);

	PiSDFIfVertex *ifRr = (PiSDFIfVertex*)graph->addVertex("ifRr", input_vertex);
	ifRr->setDirection(0);
	ifRr->setParentVertex(graph->getParentVertex());
	ifRr->setParentEdge(graph->getParentVertex()->getInputEdge(3));
	ifRr->setFunction_index(RB_FUNCT_IX);

	PiSDFIfVertex *ifRg = (PiSDFIfVertex*)graph->addVertex("ifRg", input_vertex);
	ifRg->setDirection(0);
	ifRg->setParentVertex(graph->getParentVertex());
	ifRg->setParentEdge(graph->getParentVertex()->getInputEdge(4));
	ifRg->setFunction_index(RB_FUNCT_IX);

	PiSDFIfVertex *ifRb = (PiSDFIfVertex*)graph->addVertex("ifRb", input_vertex);
	ifRb->setDirection(0);
	ifRb->setParentVertex(graph->getParentVertex());
	ifRb->setParentEdge(graph->getParentVertex()->getInputEdge(5));
	ifRb->setFunction_index(RB_FUNCT_IX);

	PiSDFIfVertex *ifOut = (PiSDFIfVertex*)graph->addVertex("ifOut", output_vertex);
	ifOut->setDirection(1);
	ifOut->setParentVertex(graph->getParentVertex());
	ifOut->setParentEdge(graph->getParentVertex()->getOutputEdge(0));
	ifOut->setFunction_index(RB_FUNCT_IX);

	// Special vertices
	PiSDFVertex *vxBr_Lr = (PiSDFVertex *)graph->addVertex("Br_Lr", normal_vertex);
	vxBr_Lr->setSubType(SubType_Broadcast);
	vxBr_Lr->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxBr_Lg = (PiSDFVertex *)graph->addVertex("Br_Lg", normal_vertex);
	vxBr_Lg->setSubType(SubType_Broadcast);
	vxBr_Lg->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxBr_Lb = (PiSDFVertex *)graph->addVertex("Br_Lb", normal_vertex);
	vxBr_Lb->setSubType(SubType_Broadcast);
	vxBr_Lb->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxBr_Lgr = (PiSDFVertex *)graph->addVertex("Br_Lgr", normal_vertex);
	vxBr_Lgr->setSubType(SubType_Broadcast);
	vxBr_Lgr->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxBr_Rgr = (PiSDFVertex *)graph->addVertex("Br_Rgr", normal_vertex);
	vxBr_Rgr->setSubType(SubType_Broadcast);
	vxBr_Rgr->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxBr_Del = (PiSDFVertex *)graph->addVertex("vxBr_Del", normal_vertex);
	vxBr_Del->setSubType(SubType_Broadcast);
	vxBr_Del->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxRb_Hr = (PiSDFVertex *)graph->addVertex("Rb_Hr", normal_vertex);
	vxRb_Hr->setSubType(SubType_RoundBuffer);
	vxRb_Hr->setFunction_index(RB_FUNCT_IX);

	PiSDFVertex *vxRb_Hg = (PiSDFVertex *)graph->addVertex("Rb_Hg", normal_vertex);
	vxRb_Hg->setSubType(SubType_RoundBuffer);
	vxRb_Hg->setFunction_index(RB_FUNCT_IX);

	PiSDFVertex *vxRb_Hb = (PiSDFVertex *)graph->addVertex("Rb_Hb", normal_vertex);
	vxRb_Hb->setSubType(SubType_RoundBuffer);
	vxRb_Hb->setFunction_index(RB_FUNCT_IX);

	PiSDFVertex *vxRb_Vr = (PiSDFVertex *)graph->addVertex("Rb_Vr", normal_vertex);
	vxRb_Vr->setSubType(SubType_RoundBuffer);
	vxRb_Vr->setFunction_index(RB_FUNCT_IX);

	PiSDFVertex *vxRb_Vg = (PiSDFVertex *)graph->addVertex("Rb_Vg", normal_vertex);
	vxRb_Vg->setSubType(SubType_RoundBuffer);
	vxRb_Vg->setFunction_index(RB_FUNCT_IX);

	PiSDFVertex *vxRb_Vb = (PiSDFVertex *)graph->addVertex("Rb_Vb", normal_vertex);
	vxRb_Vb->setSubType(SubType_RoundBuffer);
	vxRb_Vb->setFunction_index(RB_FUNCT_IX);

	/* Vertices */
	PiSDFVertex *vxRGB2Gray_L = (PiSDFVertex *)graph->addVertex("RGB2Gray_L", normal_vertex);
	vxRGB2Gray_L->addParameter(paramWidth);
	vxRGB2Gray_L->addParameter(paramHeight);
	vxRGB2Gray_L->setFunction_index(2);

	PiSDFVertex *vxRGB2Gray_R = (PiSDFVertex *)graph->addVertex("RGB2Gray_R", normal_vertex);
	vxRGB2Gray_R->addParameter(paramWidth);
	vxRGB2Gray_R->addParameter(paramHeight);
	vxRGB2Gray_R->setFunction_index(2);

	PiSDFVertex *vxCen_L = (PiSDFVertex *)graph->addVertex("Cen_L", normal_vertex);
	vxCen_L->addParameter(paramWidth);
	vxCen_L->addParameter(paramHeight);
	vxCen_L->setFunction_index(3);

	PiSDFVertex *vxCen_R = (PiSDFVertex *)graph->addVertex("Cen_R", normal_vertex);
	vxCen_R->addParameter(paramWidth);
	vxCen_R->addParameter(paramHeight);
	vxCen_R->setFunction_index(3);

	PiSDFVertex *vxGen_D = (PiSDFVertex *)graph->addVertex("Gen_D", normal_vertex);
	vxGen_D->addParameter(paramNbIter);
	vxGen_D->setFunction_index(4);

	PiSDFVertex *vxHweight = (PiSDFVertex *)graph->addVertex("HWeight", normal_vertex);
	vxHweight->addParameter(paramWidth);
	vxHweight->addParameter(paramHeight);
	vxHweight->addParameter(paramZero);
	vxHweight->setFunction_index(5);

	PiSDFVertex *vxVweight = (PiSDFVertex *)graph->addVertex("VWeight", normal_vertex);
	vxVweight->addParameter(paramWidth);
	vxVweight->addParameter(paramHeight);
	vxVweight->addParameter(paramOne);
	vxVweight->setFunction_index(5);

	PiSDFVertex *vxDispTop = (PiSDFVertex *)graph->addVertex("DispTop", normal_vertex);
	vxDispTop->addParameter(paramWidth);
	vxDispTop->addParameter(paramHeight);
	vxDispTop->addParameter(paramNbDisp);
	vxDispTop->addParameter(paramNbIter);
	vxDispTop->setFunction_index(6);


	/* Edges */
	graph->addEdge(ifLr, 0, "height2*width2", vxBr_Lr, 0, "height2*width2", "0");
	graph->addEdge(ifLg, 0, "height2*width2", vxBr_Lg, 0, "height2*width2", "0");
	graph->addEdge(ifLb, 0, "height2*width2", vxBr_Lb, 0, "height2*width2", "0");

	graph->addEdge(vxBr_Lr, 0, "height2*width2", vxRGB2Gray_L, 0, "height2*width2", "0");
	graph->addEdge(vxBr_Lg, 0, "height2*width2", vxRGB2Gray_L, 1, "height2*width2", "0");
	graph->addEdge(vxBr_Lb, 0, "height2*width2", vxRGB2Gray_L, 2, "height2*width2", "0");

	graph->addEdge(vxRGB2Gray_L, 0, "height2*width2", vxBr_Lgr, 0, "height2*width2", "0");

	graph->addEdge(ifRr, 0, "height2*width2", vxRGB2Gray_R, 0, "height2*width2", "0");
	graph->addEdge(ifRg, 0, "height2*width2", vxRGB2Gray_R, 1, "height2*width2", "0");
	graph->addEdge(ifRb, 0, "height2*width2", vxRGB2Gray_R, 2, "height2*width2", "0");

	graph->addEdge(vxRGB2Gray_R, 0, "height2*width2", vxBr_Rgr, 0, "height2*width2", "0");

	graph->addEdge(vxBr_Lgr, 0, "height2*width2", vxCen_L, 0, "height2*width2", "0");
	graph->addEdge(vxBr_Rgr, 0, "height2*width2", vxCen_R, 0, "height2*width2", "0");

	graph->addEdge(vxBr_Lr, 1, "height2*width2", vxRb_Hr, 0, "height2*width2", "0");
	graph->addEdge(vxBr_Lg, 1, "height2*width2", vxRb_Hg, 0, "height2*width2", "0");
	graph->addEdge(vxBr_Lb, 1, "height2*width2", vxRb_Hb, 0, "height2*width2", "0");
	graph->addEdge(vxBr_Lr, 2, "height2*width2", vxRb_Vr, 0, "height2*width2", "0");
	graph->addEdge(vxBr_Lg, 2, "height2*width2", vxRb_Vg, 0, "height2*width2", "0");
	graph->addEdge(vxBr_Lb, 2, "height2*width2", vxRb_Vb, 0, "height2*width2", "0");

	graph->addEdge(vxRb_Hr, 0, "nbIter2*height2*width2", vxHweight, 0, "height2*width2", "0");
	graph->addEdge(vxRb_Hg, 0, "nbIter2*height2*width2", vxHweight, 1, "height2*width2", "0");
	graph->addEdge(vxRb_Hb, 0, "nbIter2*height2*width2", vxHweight, 2, "height2*width2", "0");
	graph->addEdge(vxRb_Vr, 0, "nbIter2*height2*width2", vxVweight, 0, "height2*width2", "0");
	graph->addEdge(vxRb_Vg, 0, "nbIter2*height2*width2", vxVweight, 1, "height2*width2", "0");
	graph->addEdge(vxRb_Vb, 0, "nbIter2*height2*width2", vxVweight, 2, "height2*width2", "0");

	graph->addEdge(vxGen_D, 0, "nbIter2", vxBr_Del, 0, "nbIter2", "0");
	graph->addEdge(vxBr_Del, 0, "nbIter2", vxHweight, 3, "1", "0");
	graph->addEdge(vxBr_Del, 1, "nbIter2", vxVweight, 3, "1", "0");

	graph->addEdge(vxBr_Lgr, 1, "height2*width2", vxDispTop, 0, "height2*width2", "0");
	graph->addEdge(vxCen_L,  0, "height2*width2", vxDispTop, 1, "height2*width2", "0");
	graph->addEdge(vxBr_Rgr, 1, "height2*width2", vxDispTop, 2, "height2*width2", "0");
	graph->addEdge(vxCen_R,  0, "height2*width2", vxDispTop, 3, "height2*width2", "0");
	graph->addEdge(vxHweight,0, "height2*width2", vxDispTop, 4, "nbIter2*height2*width2", "0");
	graph->addEdge(vxVweight,0, "height2*width2", vxDispTop, 5, "nbIter2*height2*width2", "0");
	graph->addEdge(vxBr_Del, 2, "nbIter2", vxDispTop, 6, "nbIter2", "0");

	graph->addEdge(vxDispTop, 0, "height2*width2", ifOut, 0, "height2*width2", "0");

	/* Timings */
	vxBr_Lr->setTiming(0, "100");
	vxBr_Lg->setTiming(0, "100");
	vxBr_Lb->setTiming(0, "100");
	vxBr_Lgr->setTiming(0, "100");
	vxBr_Rgr->setTiming(0, "100");
	vxBr_Del->setTiming(0, "100");
	vxRb_Hr->setTiming(0, "100");
	vxRb_Hg->setTiming(0, "100");
	vxRb_Hb->setTiming(0, "100");
	vxRb_Vr->setTiming(0, "100");
	vxRb_Vg->setTiming(0, "100");
	vxRb_Vb->setTiming(0, "100");
	vxRGB2Gray_L->setTiming(0, "100");
	vxRGB2Gray_R->setTiming(0, "100");
	vxCen_L->setTiming(0, "100");
	vxCen_R->setTiming(0, "100");
	vxGen_D->setTiming(0, "100");
	vxHweight->setTiming(0, "100");
	vxVweight->setTiming(0, "100");
	vxDispTop->setTiming(0, "100");

	vxBr_Lr->setTiming(1, "100");
	vxBr_Lg->setTiming(1, "100");
	vxBr_Lb->setTiming(1, "100");
	vxBr_Lgr->setTiming(1, "100");
	vxBr_Rgr->setTiming(1, "100");
	vxBr_Del->setTiming(1, "100");
	vxRb_Hr->setTiming(1, "100");
	vxRb_Hg->setTiming(1, "100");
	vxRb_Hb->setTiming(1, "100");
	vxRb_Vr->setTiming(1, "100");
	vxRb_Vg->setTiming(1, "100");
	vxRb_Vb->setTiming(1, "100");
	vxRGB2Gray_L->setTiming(1, "100");
	vxRGB2Gray_R->setTiming(1, "100");
	vxCen_L->setTiming(1, "100");
	vxCen_R->setTiming(1, "100");
	vxGen_D->setTiming(1, "100");
	vxHweight->setTiming(1, "100");
	vxVweight->setTiming(1, "100");
	vxDispTop->setTiming(1, "100");

	// Subgraphs
	PiSDFGraph *Stereo_topDisp = addGraph();
	vxDispTop->setSubGraph(Stereo_topDisp);
	Stereo_topDisp->setParentVertex(vxDispTop);

	stereo_topDisp(Stereo_topDisp);
}

void stereo_topDisp(PiSDFGraph* graph){
	/* Parameters */
	PiSDFParameter *paramWidth = graph->addParameter("width3");
	paramWidth->setParameterParentID(0);
	PiSDFParameter *paramHeight = graph->addParameter("height3");
	paramHeight->setParameterParentID(1);

	PiSDFParameter *paramNbDisp = graph->addParameter("nbDisp3");
	paramNbDisp->setParameterParentID(2);
	PiSDFParameter *paramNbIter = graph->addParameter("nbIter3");
	paramNbIter->setParameterParentID(3);

	/* Interfaces */
	PiSDFIfVertex *ifLgray = (PiSDFIfVertex*)graph->addVertex("ifLgray", input_vertex);
	ifLgray->setDirection(0);
	ifLgray->setParentVertex(graph->getParentVertex());
	ifLgray->setParentEdge(graph->getParentVertex()->getInputEdge(0));
	ifLgray->setFunction_index(RB_FUNCT_IX);

	PiSDFIfVertex *ifLcen = (PiSDFIfVertex*)graph->addVertex("ifLcen", input_vertex);
	ifLcen->setDirection(0);
	ifLcen->setParentVertex(graph->getParentVertex());
	ifLcen->setParentEdge(graph->getParentVertex()->getInputEdge(1));
	ifLcen->setFunction_index(RB_FUNCT_IX);

	PiSDFIfVertex *ifRgray = (PiSDFIfVertex*)graph->addVertex("ifRgray", input_vertex);
	ifRgray->setDirection(0);
	ifRgray->setParentVertex(graph->getParentVertex());
	ifRgray->setParentEdge(graph->getParentVertex()->getInputEdge(2));
	ifRgray->setFunction_index(RB_FUNCT_IX);

	PiSDFIfVertex *ifRcen = (PiSDFIfVertex*)graph->addVertex("ifRcen", input_vertex);
	ifRcen->setDirection(0);
	ifRcen->setParentVertex(graph->getParentVertex());
	ifRcen->setParentEdge(graph->getParentVertex()->getInputEdge(3));
	ifRcen->setFunction_index(RB_FUNCT_IX);

	PiSDFIfVertex *ifHWeight = (PiSDFIfVertex*)graph->addVertex("ifHWeight", input_vertex);
	ifHWeight->setDirection(0);
	ifHWeight->setParentVertex(graph->getParentVertex());
	ifHWeight->setParentEdge(graph->getParentVertex()->getInputEdge(4));
	ifHWeight->setFunction_index(RB_FUNCT_IX);

	PiSDFIfVertex *ifVWeight = (PiSDFIfVertex*)graph->addVertex("ifVWeight", input_vertex);
	ifVWeight->setDirection(0);
	ifVWeight->setParentVertex(graph->getParentVertex());
	ifVWeight->setParentEdge(graph->getParentVertex()->getInputEdge(5));
	ifVWeight->setFunction_index(RB_FUNCT_IX);

	PiSDFIfVertex *ifDeltas = (PiSDFIfVertex*)graph->addVertex("ifDeltas", input_vertex);
	ifDeltas->setDirection(0);
	ifDeltas->setParentVertex(graph->getParentVertex());
	ifDeltas->setParentEdge(graph->getParentVertex()->getInputEdge(6));
	ifDeltas->setFunction_index(RB_FUNCT_IX);

	PiSDFIfVertex *ifOut = (PiSDFIfVertex*)graph->addVertex("ifOut", output_vertex);
	ifOut->setDirection(1);
	ifOut->setParentVertex(graph->getParentVertex());
	ifOut->setParentEdge(graph->getParentVertex()->getOutputEdge(0));
	ifOut->setFunction_index(RB_FUNCT_IX);

	// Special vertices
	PiSDFVertex *vxBr_Disp = (PiSDFVertex *)graph->addVertex("Br_Disp", normal_vertex);
	vxBr_Disp->setSubType(SubType_Broadcast);
	vxBr_Disp->setFunction_index(BROADCAST_FUNCT_IX);

	PiSDFVertex *vxBr_Out = (PiSDFVertex *)graph->addVertex("Br_Out", normal_vertex);
	vxBr_Out->setSubType(SubType_Broadcast);
	vxBr_Out->setFunction_index(BROADCAST_FUNCT_IX);

	/* Vertices */
	PiSDFVertex *vxCostConst = (PiSDFVertex *)graph->addVertex("CostConst", normal_vertex);
	vxCostConst->addParameter(paramWidth);
	vxCostConst->addParameter(paramHeight);
	vxCostConst->setFunction_index(7);

	PiSDFVertex *vxAggregateTop = (PiSDFVertex *)graph->addVertex("AggregateTop", normal_vertex);
	vxAggregateTop->addParameter(paramWidth);
	vxAggregateTop->addParameter(paramHeight);
	vxAggregateTop->addParameter(paramNbIter);
	vxAggregateTop->setFunction_index(8);

	PiSDFVertex *vxSelect = (PiSDFVertex *)graph->addVertex("Select", normal_vertex);
	vxSelect->addParameter(paramWidth);
	vxSelect->addParameter(paramHeight);
	vxSelect->setFunction_index(9);

	PiSDFVertex *vxDispGen = (PiSDFVertex *)graph->addVertex("DispGen", normal_vertex);
	vxDispGen->addParameter(paramNbDisp);
	vxDispGen->setFunction_index(10);

	/* Edges */
	graph->addEdge(ifLgray, 0, "height3*width3", vxCostConst, 0, "height3*width3", "0");
	graph->addEdge(ifLcen,  0, "height3*width3", vxCostConst, 1, "height3*width3", "0");
	graph->addEdge(ifRgray, 0, "height3*width3", vxCostConst, 2, "height3*width3", "0");
	graph->addEdge(ifRcen,  0, "height3*width3", vxCostConst, 3, "height3*width3", "0");

	graph->addEdge(vxDispGen,  0, "nbDisp3", vxBr_Disp, 0, "nbDisp3", "0");
	graph->addEdge(vxBr_Disp,  0, "nbDisp3", vxCostConst, 4, "1", "0");

	graph->addEdge(ifVWeight,  0, "height3*width3*nbIter3", vxAggregateTop, 0, "height3*width3*nbIter3", "0");
	graph->addEdge(ifHWeight,  0, "height3*width3*nbIter3", vxAggregateTop, 1, "height3*width3*nbIter3", "0");
	graph->addEdge(ifDeltas,   0, "nbIter3", vxAggregateTop, 2, "nbIter3", "0");
	graph->addEdge(vxCostConst, 0, "height3*width3", vxAggregateTop, 3, "height3*width3", "0");

	graph->addEdge(vxSelect, 0, "height3*width3", vxSelect, 0, "height3*width3", "height3*width3");
	graph->addEdge(vxBr_Out, 1, "height3*width3", vxSelect, 1, "height3*width3", "height3*width3");
	graph->addEdge(vxAggregateTop, 0, "height3*width3", vxSelect, 2, "height3*width3", "0");
	graph->addEdge(vxBr_Disp, 1, "nbDisp3", vxSelect, 3, "1", "0");

	graph->addEdge(vxSelect, 1, "height3*width3", vxBr_Out, 0, "height3*width3", "0");
	graph->addEdge(vxBr_Out, 0, "height3*width3", ifOut, 0, "height3*width3", "0");

	/* Timings */
	vxBr_Disp->setTiming(0, "100");
	vxBr_Out->setTiming(0, "100");
	vxCostConst->setTiming(0, "100");
	vxAggregateTop->setTiming(0, "100");
	vxSelect->setTiming(0, "100");
	vxDispGen->setTiming(0, "100");

	vxBr_Disp->setTiming(1, "100");
	vxBr_Out->setTiming(1, "100");
	vxCostConst->setTiming(1, "100");
	vxAggregateTop->setTiming(1, "100");
	vxSelect->setTiming(1, "100");
	vxDispGen->setTiming(1, "100");
}
