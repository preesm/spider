
/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet				*
 * 																			*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr					*
 * 																			*
 * This software is a computer program whose purpose is to execute			*
 * parallel applications.													*
 * 																			*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 												*
 * 																			*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 																*
 * 																			*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 									*
 * 																			*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.			*
 ****************************************************************************/
#include <lrt_prototypes.h>
#include <mpeg.h>
#include "definitions.h"
//static int firstframe = 0;


REVERSE_EVENT init_vlc_tables_I_PC_decod1_DCT3D_I[4096];

int keyframes[2] = {0};

decodeVOPInData inputData;
decodeVOPOutData outputData;

void decode_I_frame(const unsigned char *data,const struct_VOLsimple *VOLsimple,const int position,struct_VOP *VOP,REVERSE_EVENT *DCT3D_I,int *pos_fin_vlc,int *address,unsigned char *Lum,unsigned char *Cb,unsigned char *Cr,int *keyframes);

void decodeVOP_I(){
	int frame_pos_fin_vlc;
	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();

	// Getting input data.
	read_input_fifo(action->fifo_in_id[0], sizeof(decodeVOPInData), (UINT8*)&inputData);




//	if(firstframe==0){
//		firstframe++;
//		Choix_I_P_haut_niveau_CondO6_o = 0 ;
//	}
//	else{
//		Choix_I_P_haut_niveau_CondO6_o = 1 ;
//	}

//	int frame_address;

//	decode_I_frame(
//			inputData.readm4v_double_buffering_buffer_out,
//			&inputData.VideoObjectPlane_Layer_VOLsimple,
//			inputData.VideoObjectPlane_pos,
//			&inputData.VideoObjectPlane_VOP,
//			init_vlc_tables_I_PC_decod1_DCT3D_I,
//			&frame_pos_fin_vlc,
//			&outputData.frame_address,
//			outputData.mem_Y_last_buf,
//			outputData.mem_U_last_buf,
//			outputData.mem_V_last_buf,
//			keyframes);

//	Display_CondI4_o = Choix_I_P_haut_niveau_decode_I_frame_address;
//	Choix_I_P_haut_niveau_CondO5_o = Choix_I_P_haut_niveau_decode_I_frame_pos_fin_vlc;

	// Sending output data.
	write_output_fifo(action->fifo_out_id[0], sizeof(decodeVOPOutData), (UINT8*)&outputData);
}
