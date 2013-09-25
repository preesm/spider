/*
 * mpeg_part2.cpp
 *
 *  Created on: 10 sept. 2013
 *      Author: yoliva
 */
#include <string.h>
#include "mpeg_part2.h"


#define H_VALUE	3
#define W_VALUE	4
#define B_VALUE	1
#define N_VALUE	1


void create_PiSDF_dec_I_block(PiCSDFGraph* graph)
{
	// Vertices.
	CSDAGVertex *vertex_read_dec_I_block = graph->addVertex("Read_Dec_I_Block");
	CSDAGVertex *vertex_write_dec_I_block = graph->addVertex("Write_Dec_I_Block");

	CSDAGVertex *vertex_vlc_inv_I = graph->addVertex("VLC_inv_I");
	CSDAGVertex *vertex_inv_ac_dc_pred = graph->addVertex("Inv_AC_DC_Pred");
	CSDAGVertex *vertex_inv_quant_I = graph->addVertex("Inv_Quant_I");
	CSDAGVertex *vertex_inv_dc_top = graph->addVertex("Inv_DC_Top");
	CSDAGVertex *vertex_sat_block = graph->addVertex("Inv_Sat_Block");


	// Edges.
	graph->addEdge(vertex_read_dec_I_block, "1", vertex_vlc_inv_I, "1", "0");
	graph->addEdge(vertex_read_dec_I_block, "1", vertex_inv_ac_dc_pred, "1", "0");
	graph->addEdge(vertex_read_dec_I_block, "1", vertex_inv_quant_I, "1", "0");

	graph->addEdge(vertex_vlc_inv_I, "1", vertex_inv_ac_dc_pred, "1", "0");
	graph->addEdge(vertex_inv_ac_dc_pred, "1", vertex_inv_quant_I, "1", "0");
	graph->addEdge(vertex_inv_quant_I, "1", vertex_inv_dc_top, "1", "0");
	graph->addEdge(vertex_inv_dc_top, "1", vertex_sat_block, "1", "0");

	graph->addEdge(vertex_vlc_inv_I, "1", vertex_write_dec_I_block, "1", "0");
	graph->addEdge(vertex_inv_quant_I, "1", vertex_write_dec_I_block, "1", "0");
	graph->addEdge(vertex_sat_block, "1", vertex_write_dec_I_block, "1", "0");
}


void create_PiSDF_dec_I_frame(PiCSDFGraph* graph)
{
	// Vertices.
	CSDAGVertex *vertex_read_dec_I_frame = graph->addVertex("Read_Dec_I_Frame");
	CSDAGVertex *vertex_write_dec_I_frame = graph->addVertex("Write_Dec_I_Frame");

	CSDAGVertex *vertex_stock_b_lum_init = graph->addVertex("Stock_B_Lum_Init");
	CSDAGVertex *vertex_video_pack_hdr = graph->addVertex("Video_Pack_Hdr");
	CSDAGVertex *vertex_param_mb_I = graph->addVertex("Param_MB_I");
	CSDAGVertex *vertex_stock_b_lum = graph->addVertex("Stock_B_Lum");
	CSDAGVertex *vertex_stock_b_Cr = graph->addVertex("Stock_B_Cr");
	CSDAGVertex *vertex_stock_b_Cb = graph->addVertex("Stock_B_Cb");
	CSDAGVertex *vertex_dec_b_I = graph->addVertex("Dec_B_I");
	CSDAGVertex *vertex_stock_b_in_pict = graph->addVertex("Stock_B_In_Pict");
	CSDAGVertex *vertex_up_vlc_inv_xi_pos_prec = graph->addVertex("Up_VLC_Inv_Xi_Pos_Prec");
	CSDAGVertex *vertex_up_pos_o = graph->addVertex("Up_Pos_O");

	// Configuration vertices.
	CSDAGVertex *vertex_set_frame_size = graph->addConfigVertex("Set_Frame_Size");

	// Variables.
	globalParser.addVariable("frame_size", 1);

	// Parameters.
	PiCSDFParameter* param_frame_size = graph->addParameter("Frame_Size", "frame_size");

	// Configuration input ports.
	graph->addConfigPort(vertex_stock_b_lum_init, param_frame_size, 0);
	graph->addConfigPort(vertex_write_dec_I_frame, param_frame_size, 0);

	// Configuration output ports.
	graph->addConfigPort(vertex_set_frame_size, param_frame_size, 1);

	// Edges.
	graph->addEdge(vertex_read_dec_I_frame, "1", vertex_stock_b_lum_init, "1", "0");
//	graph->addEdge(vertex_read_dec_I_frame, "1", vertex_set_frame_size, "1", "0");
//	graph->addEdge(vertex_set_frame_size, "1", vertex_stock_b_lum_init, "1", "0");
//	graph->addEdge(vertex_set_frame_size, "1", vertex_write_dec_I_frame, "1", "0");
	graph->addEdge(vertex_stock_b_lum_init, "frame_size", vertex_video_pack_hdr, "1", "0");
	graph->addEdge(vertex_video_pack_hdr, "1", vertex_param_mb_I, "1", "0");
	graph->addEdge(vertex_param_mb_I, "1", vertex_stock_b_lum, "1", "0");
	graph->addEdge(vertex_param_mb_I, "1", vertex_stock_b_Cb, "1", "0");
	graph->addEdge(vertex_param_mb_I, "1", vertex_stock_b_Cr, "1", "0");
	graph->addEdge(vertex_stock_b_lum, "6", vertex_dec_b_I, "1", "0");
	graph->addEdge(vertex_stock_b_Cb, "6", vertex_dec_b_I, "1", "0");
	graph->addEdge(vertex_stock_b_Cr, "6", vertex_dec_b_I, "1", "0");
	graph->addEdge(vertex_dec_b_I, "1", vertex_stock_b_in_pict, "1", "0");
	graph->addEdge(vertex_stock_b_in_pict, "1", vertex_up_vlc_inv_xi_pos_prec, "1", "0");
	graph->addEdge(vertex_up_vlc_inv_xi_pos_prec, "1", vertex_dec_b_I, "1", "0");
	graph->addEdge(vertex_up_vlc_inv_xi_pos_prec, "1", vertex_up_pos_o, "6", "0");
	graph->addEdge(vertex_up_pos_o, "1", vertex_write_dec_I_frame, "frame_size", "0");

}


void create_PiSDF_dec_I_frame_single(PiCSDFGraph* graph){
	// Vertices.
	graph->addVertex("Dec_I_Frame");
}

void create_PiSDF_mpeg_part2(PiCSDFGraph* graph){
//	create_PiSDF_dec_I_block(graph);
//	create_PiSDF_dec_I_frame(graph);
	create_PiSDF_dec_I_frame_single(graph);
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
