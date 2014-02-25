
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/

#include <string.h>
#include <graphs/PiSDF/PiSDFGraph.h>

//#define H_VALUE	3
//#define W_VALUE	4
//#define B_VALUE	1
//#define N_VALUE	1

#define HIERARCHY_LEVEL 2

static PiSDFGraph graphs[MAX_NB_PiSDF_SUB_GRAPHS];
static UINT8 nb_graphs = 0;


void PiSDFDecVOP(PiSDFGraph* graph, BaseVertex* parentVertex, Scenario* scenario){
	/*** Decoder_VOP subgraph ***/

	// Interface vertices.
	PiSDFIfVertex *vertexInVOL = (PiSDFIfVertex*)graph->addVertex("InputVOL_L2", input_vertex);
	vertexInVOL->setDirection(0);
	vertexInVOL->setParentVertex(parentVertex);
//	vertexInVOL->setParentEdgeIndex(0);
	vertexInVOL->setFunction_index(8);
	graph->setRootVertex(vertexInVOL);

	PiSDFIfVertex *vertexInVOP = (PiSDFIfVertex*)graph->addVertex("InputVOP_L2", input_vertex);
	vertexInVOP->setDirection(0);
	vertexInVOP->setParentVertex(parentVertex);
//	vertexInVOP->setParentEdgeIndex(1);
	vertexInVOP->setFunction_index(9);

	PiSDFIfVertex *vertexInFrmData = (PiSDFIfVertex*)graph->addVertex("InputFrm_L2", input_vertex);
	vertexInFrmData->setDirection(0);
	vertexInFrmData->setParentVertex(parentVertex);
//	vertexInFrmData->setParentEdgeIndex(2);
	vertexInFrmData->setFunction_index(10);

	PiSDFIfVertex *vertexInPrevImg = (PiSDFIfVertex*)graph->addVertex("InputPrevImg_L2", input_vertex);
	vertexInPrevImg->setDirection(0);
	vertexInPrevImg->setParentVertex(parentVertex);
//	vertexInPrevImg->setParentEdgeIndex(3);
	vertexInPrevImg->setFunction_index(11);

	PiSDFIfVertex *vertexOutYUV_0 = (PiSDFIfVertex*)graph->addVertex("OutputYUV_0", output_vertex);
	vertexOutYUV_0->setDirection(1);
	vertexOutYUV_0->setParentVertex(parentVertex);
//	vertexOutYUV_0->setParentEdgeIndex(0);
	vertexOutYUV_0->setFunction_index(19);

	PiSDFIfVertex *vertexOutYUV_1 = (PiSDFIfVertex*)graph->addVertex("OutputYUV_1", output_vertex);
	vertexOutYUV_1->setDirection(1);
	vertexOutYUV_1->setParentVertex(parentVertex);
//	vertexOutYUV_1->setParentEdgeIndex(1);
	vertexOutYUV_1->setFunction_index(19);


	// Configuration vertices.
	PiSDFParameter* paramVOPType = graph->addParameter("VOPType");
	PiSDFConfigVertex* vertexSetVOPType = (PiSDFConfigVertex*)graph->addVertex("SetVOPType", config_vertex);
	vertexSetVOPType->addRelatedParam(paramVOPType);
	vertexSetVOPType->setFunction_index(12);


	// Switch, Select & Broad vertices.
	BaseVertex* vertexSwitch_0 = graph->addVertex("Switch_0", switch_vertex);
	vertexSwitch_0->addParameter(paramVOPType);
	vertexSwitch_0->setFunction_index(13);

	BaseVertex* vertexSwitch_1 = graph->addVertex("Switch_1", switch_vertex);
	vertexSwitch_1->addParameter(paramVOPType);
	vertexSwitch_1->setFunction_index(14);

	BaseVertex* vertexSelect_0 = graph->addVertex("Select_0", select_vertex);
	vertexSelect_0->addParameter(paramVOPType);
	vertexSelect_0->setFunction_index(18);


	// PiSDF Vertices.
	PiSDFVertex *vertexDecVOPI = (PiSDFVertex *)graph->addVertex("DecVOP_I", pisdf_vertex);
	vertexDecVOPI->setFunction_index(15);

	PiSDFVertex *vertexDecVOPP = (PiSDFVertex *)graph->addVertex("DecVOP_P", pisdf_vertex);
	vertexDecVOPP->setFunction_index(16);


	// Edges.
	graph->addEdge(vertexInVOL, "1", vertexSwitch_0, "1", "0");

	graph->addEdge(vertexInVOP, "1", vertexSetVOPType, "1", "0");
	graph->addEdge(vertexInVOP, "1", vertexSwitch_0, "1", "0");

	graph->addEdge(vertexInFrmData, "1", vertexSwitch_0, "1", "0");

	graph->addEdge(vertexInPrevImg, "1", vertexSwitch_1, "1", "0");

	graph->addEdge(vertexSwitch_0, "1 - VOPType", vertexDecVOPI, "1", "0"); // VOL
	graph->addEdge(vertexSwitch_0, "1 - VOPType", vertexDecVOPI, "1", "0"); // VOP
	graph->addEdge(vertexSwitch_0, "1 - VOPType", vertexDecVOPI, "1", "0"); // frame
	graph->addEdge(vertexSwitch_0, "VOPType", vertexDecVOPP, "1", "0"); // VOL
	graph->addEdge(vertexSwitch_0, "VOPType", vertexDecVOPP, "1", "0"); // VOP
	graph->addEdge(vertexSwitch_0, "VOPType", vertexDecVOPP, "1", "0"); // frame

	graph->addEdge(vertexSwitch_1, "VOPType", vertexDecVOPP, "1", "0");
//	graph->addEdge(vertexSwitch_1, "1 - VOPType", vertexDecVOPI, "1", "0"); // TODO: Trash vertex is missing.

	graph->addEdge(vertexDecVOPI, "1", vertexSelect_0, "1 - VOPType", "0");

	graph->addEdge(vertexDecVOPP, "1", vertexSelect_0, "VOPType", "0");

	graph->addEdge(vertexSelect_0, "1", vertexOutYUV_0, "1", "0");
	graph->addEdge(vertexSelect_0, "1", vertexOutYUV_1, "1", "0");


//	graph->addEdge(vertexSwitch, "(4-VOPType)/4", vertexDecVOPI, "1", "0");
//	graph->addEdge(vertexSwitch, "VOPType%2", vertexDecVOPP, "1", "0");
//	graph->addEdge(vertexSwitch, "(VOPType/2)%2", vertexDecVOPB, "1", "0");
//	graph->addEdge(vertexSwitch, "VOPType/4", vertexDecSkip, "1", "0");

//	graph->addEdge(vertexDecVOPI, "1", vertexSelect, "(4-VOPType)/4", "0");
//	graph->addEdge(vertexDecVOPP, "1", vertexSelect, "VOPType%2", "0");
//	graph->addEdge(vertexDecVOPB, "1", vertexSelect, "(VOPType/2)%2", "0");
//	graph->addEdge(vertexDecSkip, "1", vertexSelect, "VOPType/4", "0");
	/*** End of Decoder_VOP subgraph ***/
}


void PiSDFDecodeDsply(PiSDFGraph* graph, BaseVertex* parentVertex, Scenario* scenario){

	// Interface vertices.
	PiSDFIfVertex *vertexInVOL = (PiSDFIfVertex*)graph->addVertex("InputVOL_L1", input_vertex);
	vertexInVOL->setDirection(0);
	vertexInVOL->setParentVertex(parentVertex);
//	vertexInVOL->setParentEdgeIndex(0);
	vertexInVOL->setParentEdge(parentVertex->getInputEdge(0));
	vertexInVOL->setFunction_index(2);

	PiSDFIfVertex *vertexInPosVOL = (PiSDFIfVertex*)graph->addVertex("InputPosVOL_L1", input_vertex);
	vertexInPosVOL->setDirection(0);
	vertexInPosVOL->setParentVertex(parentVertex);
//	vertexInPosVOL->setParentEdgeIndex(1);
	vertexInPosVOL->setParentEdge(parentVertex->getInputEdge(1));
	vertexInPosVOL->setFunction_index(4);

	PiSDFIfVertex *vertexInVOLCompl = (PiSDFIfVertex*)graph->addVertex("InputVOLCompl_L1", input_vertex);
	vertexInVOLCompl->setDirection(0);
	vertexInVOLCompl->setParentVertex(parentVertex);
//	vertexInVOLCompl->setParentEdgeIndex(2);
	vertexInVOLCompl->setParentEdge(parentVertex->getInputEdge(2));
	vertexInVOLCompl->setFunction_index(3);

	PiSDFIfVertex *vertexInImgDim = (PiSDFIfVertex*)graph->addVertex("InputImgDim_L1", input_vertex);
	vertexInImgDim->setDirection(0);
	vertexInImgDim->setParentVertex(parentVertex);
//	vertexInImgDim->setParentEdgeIndex(3);
	vertexInImgDim->setParentEdge(parentVertex->getInputEdge(3));
	vertexInImgDim->setFunction_index(5);


	// Parameters.
	PiSDFParameter *paramVOPType = graph->addParameter("VOPType");
	PiSDFParameter *paramNbMb = graph->addParameter("NbMb"); // Static


	// Configuration vertices
	PiSDFConfigVertex *vertexBroadVOL = (PiSDFConfigVertex*)graph->addVertex("BroadVOL", config_vertex);
	vertexBroadVOL->setFunction_index(5);

	PiSDFConfigVertex *vertex_read_vop = (PiSDFConfigVertex *)graph->addVertex("ReadVOP", config_vertex);
//	vertex_read_vop->addConstraint(0, 1);
//	vertex_read_vop->addTiming(1, "100");
	vertex_read_vop->setFunction_index(6);
	vertex_read_vop->addRelatedParam(paramVOPType);

	// Round buffer vertices
	BaseVertex* roundB_0_L1 = graph->addVertex("RoundBuf_0_L1", roundBuff_vertex);
	BaseVertex* roundB_1_L1 = graph->addVertex("RoundBuf_1_L1", roundBuff_vertex);
	BaseVertex* roundB_2_L1 = graph->addVertex("RoundBuf_2_L1", roundBuff_vertex);

	// Others..
	PiSDFVertex *vertex_display_vop = (PiSDFVertex*)graph->addVertex("DisplayVOP", pisdf_vertex);
//	vertex_display_vop->addConstraint(0, 1);
//	vertex_display_vop->addTiming(1, "100");
	vertex_display_vop->setFunction_index(7);


	PiSDFVertex *vertex_decod_vop	= (PiSDFVertex *)graph->addVertex("DecodVOP", pisdf_vertex);
//	vertex_decod_vop->addConstraint(0, 1);
//	vertex_decod_vop->addTiming(1, "100");
	vertex_decod_vop->setFunction_index(8);
	vertex_decod_vop->addParameter(paramVOPType);
	vertex_decod_vop->addParameter(paramNbMb);



	// Edges.
	graph->addEdge(vertexInVOL, "1", vertexBroadVOL, "1", "0"); // VOL

	graph->addEdge(vertexInVOLCompl, "1", vertex_read_vop, "1", "0");

	graph->addEdge(vertexInPosVOL, "1", vertex_read_vop, "1", "0"); // VOLPos

	graph->addEdge(vertexInImgDim, "1", vertex_display_vop, "1", "0");

	graph->addEdge(vertexBroadVOL, "1", vertex_read_vop, "1", "0"); // VOL
	graph->addEdge(vertexBroadVOL, "1", roundB_0_L1, "1", "0"); // VOL

	graph->addEdge(vertex_read_vop, "1", roundB_1_L1, "1", "0"); // VOP data.
	graph->addEdge(vertex_read_vop, "1", roundB_2_L1, "1", "0"); // frame data.
	graph->addEdge(vertex_read_vop, "1", vertex_read_vop, "1", "1"); // cycles the file position.

	graph->addEdge(roundB_0_L1, "1", vertex_decod_vop, "1", "0");

	graph->addEdge(roundB_1_L1, "1", vertex_decod_vop, "1", "0");

	graph->addEdge(roundB_2_L1, "1", vertex_decod_vop, "1", "0");

	graph->addEdge(vertex_decod_vop, "1", vertex_display_vop, "1", "0"); // image.
	graph->addEdge(vertex_decod_vop, "1", vertex_decod_vop, "1", "1"); // cycles the image for decoding the next frame.


	// Setting root vertex.
	graph->setRootVertex(vertex_read_vop);


#if HIERARCHY_LEVEL > 2
	// Subgraphs
	if(nb_graphs >= MAX_NB_PiSDF_SUB_GRAPHS - 1) exitWithCode(1054);
	PiSDFGraph *decod_vop_subGraph = &graphs[nb_graphs]; nb_graphs++;
	PiSDFDecVOP(decod_vop_subGraph, vertex_decod_vop, scenario);
	vertex_decod_vop->setSubGraph(decod_vop_subGraph);
#endif
}


void PiSDFDecoderMpeg4Part2(PiSDFGraph* graph, BaseVertex* parentVertex, Scenario* scenario){
	// Parameters.
	PiSDFParameter *paramNbMb = graph->addParameter("NbMb");

	// Configure vertices.
	PiSDFConfigVertex *vertex_read_vol = (PiSDFConfigVertex *)graph->addVertex("ReadVOL", config_vertex);
//	vertex_read_vop->addConstraint(0, 1);
//	vertex_read_vop->addTiming(1, "100");
	vertex_read_vol->setFunction_index(0);
	vertex_read_vol->addRelatedParam(paramNbMb);
	graph->setRootVertex(vertex_read_vol);

	// Round buffer vertices
	BaseVertex* roundB_0 = graph->addVertex("RoundBuf_0", roundBuff_vertex);
	BaseVertex* roundB_1 = graph->addVertex("RoundBuf_1", roundBuff_vertex);
	BaseVertex* roundB_2 = graph->addVertex("RoundBuf_2", roundBuff_vertex);
	BaseVertex* roundB_3 = graph->addVertex("RoundBuf_3", roundBuff_vertex);

	// Others
	PiSDFVertex *vertex_decode_dsply = (PiSDFVertex *)graph->addVertex("DecodeDsply", pisdf_vertex);
//	vertex_read_vop->addConstraint(0, 1);
//	vertex_read_vop->addTiming(1, "100");
	vertex_decode_dsply->addParameter(paramNbMb);
	vertex_decode_dsply->setFunction_index(1);

	// Edges.
	graph->addEdge(vertex_read_vol, "1", roundB_0, "1", "0");
	graph->addEdge(vertex_read_vol, "1", roundB_1, "1", "0");
	graph->addEdge(vertex_read_vol, "1", roundB_2, "1", "0");
	graph->addEdge(vertex_read_vol, "1", roundB_3, "1", "0");
	graph->addEdge(vertex_read_vol, "1", vertex_read_vol, "1", "1");

	graph->addEdge(roundB_0, "1", vertex_decode_dsply, "1", "0");
	graph->addEdge(roundB_1, "1", vertex_decode_dsply, "1", "0");
	graph->addEdge(roundB_2, "1", vertex_decode_dsply, "1", "0");
	graph->addEdge(roundB_3, "1", vertex_decode_dsply, "1", "0");

	// Subgraphs
	if(nb_graphs >= MAX_NB_PiSDF_SUB_GRAPHS - 1) exitWithCode(1054);
	PiSDFGraph *decod_dsply_subGraph = &graphs[nb_graphs]; nb_graphs++;
	PiSDFDecodeDsply(decod_dsply_subGraph, vertex_decode_dsply, scenario);
	vertex_decode_dsply->setSubGraph(decod_dsply_subGraph);
}


void top(PiSDFGraph* graph, Scenario* scenario){
	PiSDFVertex *vxDecoderMpeg4Part2 = (PiSDFVertex *)graph->addVertex("Mpeg4_Part2", pisdf_vertex);

	// Subgraphs
	if(nb_graphs >= MAX_NB_PiSDF_SUB_GRAPHS - 1) exitWithCode(1054);
	PiSDFGraph *Mpeg4Part2_subGraph = &graphs[nb_graphs]; nb_graphs++;
	PiSDFDecoderMpeg4Part2(Mpeg4Part2_subGraph, vxDecoderMpeg4Part2, scenario);
	vxDecoderMpeg4Part2->setSubGraph(Mpeg4Part2_subGraph);
}
