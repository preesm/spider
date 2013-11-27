
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
#include "mpeg4_part2.h"
#include <graphs/PiSDF/PiSDFVertex.h>

#define H_VALUE	3
#define W_VALUE	4
#define B_VALUE	1
#define N_VALUE	1

static PiSDFGraph graphs[MAX_NB_PiSDF_SUB_GRAPHS];
static UINT8 nb_graphs = 0;


void create_PiSDF_dec_VOP(PiSDFGraph* graph, BaseVertex* parentVertex){
	/*** Decoder_VOP subgraph ***/

	// Interface vertices.
	PiSDFIfVertex *vertexInVOL = (PiSDFIfVertex*)graph->addVertex("InputVOL", input_vertex);
	vertexInVOL->setDirection(0);
	vertexInVOL->setParentVertex(parentVertex);
	vertexInVOL->setParentEdgeIndex(0);
//	vertexInVOL->setFunction_index(2);
	graph->setRootVertex(vertexInVOL);

	PiSDFIfVertex *vertexInVOP = (PiSDFIfVertex*)graph->addVertex("InputVOP", input_vertex);
	vertexInVOP->setDirection(0);
	vertexInVOP->setParentVertex(parentVertex);
	vertexInVOP->setParentEdgeIndex(1);
//	vertexIn->setFunction_index(3);

	PiSDFIfVertex *vertexInPos = (PiSDFIfVertex*)graph->addVertex("InputPos", input_vertex);
	vertexInPos->setDirection(0);
	vertexInPos->setParentVertex(parentVertex);
	vertexInPos->setParentEdgeIndex(2);
//	vertexInPos->setFunction_index(4);


	PiSDFIfVertex *vertexOutYUV = (PiSDFIfVertex*)graph->addVertex("OutputYUV", output_vertex);
	vertexOutYUV->setDirection(1);
	vertexOutYUV->setParentVertex(parentVertex);
//	vertexInPos->setParentEdgeIndex(2);
//	vertexOutYUV->setFunction_index(7);


	// Configuration vertices.
	PiSDFParameter* paramVOPType = graph->addParameter("VOPType");
	PiSDFConfigVertex* vertexSetVOPType = (PiSDFConfigVertex*)graph->addVertex("SetVOPType", config_vertex);
	vertexSetVOPType->addRelatedParam(paramVOPType);
//	vertexSetVOPType->setFunction_index();


	// Switch, Select & Broad vertices.
	BaseVertex* vertexSwitch_0 = graph->addVertex("Switch_0", switch_vertex);
	vertexSwitch_0->addParameter(paramVOPType);
//	vertexSwitch_0->setFunction_index();
	BaseVertex* vertexSwitch_1 = graph->addVertex("Switch_1", switch_vertex);
	vertexSwitch_1->addParameter(paramVOPType);
//	vertexSwitch_1->setFunction_index();
	BaseVertex* vertexSwitch_2 = graph->addVertex("Switch_2", switch_vertex);
	vertexSwitch_2->addParameter(paramVOPType);
//	vertexSwitch_2->setFunction_index();
	BaseVertex* vertexSelect_0 = graph->addVertex("Select_0", select_vertex);
	vertexSelect_0->addParameter(paramVOPType);
//	vertexSelect_0->setFunction_index();
	BaseVertex* vertexBroad_0 = graph->addVertex("Broad_0", broad_vertex);


	// PiSDF Vertices.
	PiSDFVertex *vertexDecVOPI = (PiSDFVertex *)graph->addVertex("DecVOP_I", pisdf_vertex);
	vertexDecVOPI->setFunction_index(5);
	PiSDFVertex *vertexDecVOPP = (PiSDFVertex *)graph->addVertex("DecVOP_P", pisdf_vertex);
	vertexDecVOPP->setFunction_index(5);


	// Edges.
	graph->addEdge(vertexInVOL, "1", vertexSwitch_1, "1", "0");

	graph->addEdge(vertexInVOP, "1", vertexSetVOPType, "1", "0");

	graph->addEdge(vertexInVOP, "1", vertexSwitch_0, "1", "0");

	graph->addEdge(vertexInPos, "1", vertexSwitch_2, "1", "0");

	graph->addEdge(vertexSwitch_0, "1 - VOPType", vertexDecVOPI, "1", "0");
	graph->addEdge(vertexSwitch_0, "VOPType", vertexDecVOPP, "1", "0");

	graph->addEdge(vertexSwitch_1, "1 - VOPType", vertexDecVOPI, "1", "0");
	graph->addEdge(vertexSwitch_1, "VOPType", vertexDecVOPP, "1", "0");

	graph->addEdge(vertexSwitch_2, "1 - VOPType", vertexDecVOPI, "1", "0");
	graph->addEdge(vertexSwitch_2, "VOPType", vertexDecVOPP, "1", "0");

	graph->addEdge(vertexDecVOPI, "1", vertexSelect_0, "1 - VOPType", "0");

	graph->addEdge(vertexDecVOPP, "1", vertexSelect_0, "1 - VOPType", "0");

	graph->addEdge(vertexSelect_0, "1", vertexBroad_0, "1", "0");

	graph->addEdge(vertexBroad_0, "1", vertexDecVOPP, "1", "1");
	graph->addEdge(vertexBroad_0, "1", vertexOutYUV, "1", "0");



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


void create_PiSDF_decode_dsply(PiSDFGraph* graph, BaseVertex* parentVertex){

	// Input vertices.
	PiSDFIfVertex *vertexInVOL = (PiSDFIfVertex*)graph->addVertex("InputVOL", input_vertex);
	vertexInVOL->setDirection(0);
	vertexInVOL->setParentVertex(parentVertex);
	vertexInVOL->setParentEdgeIndex(0);
	vertexInVOL->setFunction_index(2);

	PiSDFIfVertex *vertexInVOLCompl = (PiSDFIfVertex*)graph->addVertex("InputVOLCompl", input_vertex);
	vertexInVOLCompl->setDirection(0);
	vertexInVOLCompl->setParentVertex(parentVertex);
	vertexInVOLCompl->setParentEdgeIndex(2);
	vertexInVOLCompl->setFunction_index(3);

	PiSDFIfVertex *vertexInPos = (PiSDFIfVertex*)graph->addVertex("InputPos", input_vertex);
	vertexInPos->setDirection(0);
	vertexInPos->setParentVertex(parentVertex);
	vertexInPos->setParentEdgeIndex(1);
	vertexInPos->setFunction_index(4);

	PiSDFIfVertex *vertexInImgDim = (PiSDFIfVertex*)graph->addVertex("InputImgDim", input_vertex);
	vertexInImgDim->setDirection(0);
	vertexInImgDim->setParentVertex(parentVertex);
	vertexInImgDim->setParentEdgeIndex(3);
	vertexInImgDim->setFunction_index(5);

	// Vertices.
	PiSDFVertex *vertex_read_vop = (PiSDFVertex *)graph->addVertex("ReadVOP", pisdf_vertex);
//	vertex_read_vop->addConstraint(0, 1);
//	vertex_read_vop->addTiming(1, "100");
	vertex_read_vop->setFunction_index(6);

	graph->setRootVertex(vertex_read_vop);


	PiSDFVertex *vertex_decod_vop	= (PiSDFVertex *)graph->addVertex("DecodVOP", pisdf_vertex);
//	vertex_decod_vop->addConstraint(0, 1);
//	vertex_decod_vop->addTiming(1, "100");
	vertex_decod_vop->setFunction_index(7);


	PiSDFVertex *vertex_display_vop = (PiSDFVertex*)graph->addVertex("DisplayVOP", pisdf_vertex);
//	vertex_display_vop->addConstraint(0, 1);
//	vertex_display_vop->addTiming(1, "100");
	vertex_display_vop->setFunction_index(8);



	// Edges.
	graph->addEdge(vertexInVOL, "1", vertex_read_vop, "1", "0");
	graph->addEdge(vertexInVOL, "1", vertex_decod_vop, "1", "0");
	graph->addEdge(vertexInPos, "1", vertex_read_vop, "1", "0");
	graph->addEdge(vertexInPos, "1", vertex_decod_vop, "1", "0");
	graph->addEdge(vertexInVOLCompl, "1", vertex_read_vop, "1", "0");
	graph->addEdge(vertexInImgDim, "1", vertex_display_vop, "1", "0");
	graph->addEdge(vertex_read_vop, "1", vertex_decod_vop, "1", "0");
	graph->addEdge(vertex_decod_vop, "1", vertex_display_vop, "1", "0");




//	// Subgraphs
//	if(nb_graphs >= MAX_NB_PiSDF_SUB_GRAPHS - 1) exitWithCode(1054);
//	PiSDFGraph *decod_vop_subGraph = &graphs[nb_graphs]; nb_graphs++;
//	create_PiSDF_dec_VOP(decod_vop_subGraph, vertex_decod_vop);
//	vertex_decod_vop->setSubGraph(decod_vop_subGraph);
}


void create_PiSDF_dec_top(PiSDFGraph* graph){
	// Vertices.
	PiSDFVertex *vertex_read_vol = (PiSDFVertex *)graph->addVertex("ReadVOL", pisdf_vertex);
//	vertex_read_vop->addConstraint(0, 1);
//	vertex_read_vop->addTiming(1, "100");
	vertex_read_vol->setFunction_index(0);
	graph->setRootVertex(vertex_read_vol);


	PiSDFVertex *vertex_decode_dsply = (PiSDFVertex *)graph->addVertex("DecodeDsply", pisdf_vertex);
//	vertex_read_vop->addConstraint(0, 1);
//	vertex_read_vop->addTiming(1, "100");
	vertex_decode_dsply->setFunction_index(1);

	// Edges.
	graph->addEdge(vertex_read_vol, "1", vertex_decode_dsply, "1", "0");
	graph->addEdge(vertex_read_vol, "1", vertex_decode_dsply, "1", "0");
	graph->addEdge(vertex_read_vol, "1", vertex_decode_dsply, "1", "0");
	graph->addEdge(vertex_read_vol, "1", vertex_decode_dsply, "1", "0");

	// Subgraphs
	if(nb_graphs >= MAX_NB_PiSDF_SUB_GRAPHS - 1) exitWithCode(1054);
	PiSDFGraph *decod_dsply_subGraph = &graphs[nb_graphs]; nb_graphs++;
	create_PiSDF_decode_dsply(decod_dsply_subGraph, vertex_decode_dsply);

	vertex_decode_dsply->setSubGraph(decod_dsply_subGraph);
}


void create_PiSDF_mpeg_part2(PiSDFGraph* graph){
//	create_PiSDF_dec_I_block(graph);
//	create_PiSDF_dec_I_frame(graph);
	create_PiSDF_dec_top(graph);
}


//int executeConfigActorDPCM(CSDAGVertex* vertex){
//	if(strcmp(vertex->getName(), "SetH") == 0)
//		return H_VALUE;
//	if(strcmp(vertex->getName(), "SetW") == 0)
//		return W_VALUE;
//	if(strcmp(vertex->getName(), "SetB") == 0)
//		return B_VALUE;
////	if(strcmp(vertex->getName(), "SetN") == 0)
//	return N_VALUE;
//}
//
//
//void resolvePiCSDFParams(PiCSDFGraph* graph){
//	for (int i = 0; i < graph->getNbConfigOutPorts(); i++) {
//		// Getting the output port.
//		PiCSDFConfigPort *outPort = graph->getConfigOutPort(i);
//
//		// Retrieving the value from the actor's execution.
//		int value = executeConfigActorDPCM(outPort->vertex);
//
//		// Updating the corresponding parameter' value.
//		((variable*)(outPort->parameter->expression[0].elt_union.var))->value = value;
//	}
//}
