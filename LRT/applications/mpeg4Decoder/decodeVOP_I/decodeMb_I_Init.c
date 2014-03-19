
/****************************************************************************
 * Copyright or Â© or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-FranÂçois Nezan, Jean-Christophe Prevotet				*
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

#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include <lrt_1W1RfifoMngr.h>
#include <hwQueues.h>
#include <lrt_core.h>
#include <lrt_taskMngr.h>

REVERSE_EVENT DCT3D_I[4096]; //init_vlc_tables_I_PC_decod1_DCT3D_I
//
//static int keyframes[2] = {0};
//

//struct_VOLsimple VOL;
readVOPOutData VOP;
uchar FrmDataWithStartCode[BUFFER_SIZE];

//decodeVOPOutData outputData;


//void decode_I_frame(const unsigned char *data,const struct_VOLsimple *VOL,const int position,struct_VOP *VOP,REVERSE_EVENT *DCT3D_I,int *pos_fin_vlc,int *address,unsigned char *Lum,unsigned char *Cb,unsigned char *Cr,int *keyframes);

void decodeMb_I_Init(UINT32 inputFIFOIds[],
		 UINT32 inputFIFOAddrs[],
		 UINT32 outputFIFOIds[],
		 UINT32 outputFIFOAddrs[],
		 UINT32 params[]){
//	int             i, j, k ;
//	int             MB_courant = 0 ;
//	int             MB_number ;
//	int             pos_X [1];
//	int             tab_pos_X [4];
//	struct_VOP      new_VOP [1];
//	int             VideoPacketHeader_pos [1];
//	int             MacroblockI_pos [1];
//	int             VideoPacketHeader_resync_marker [1];
//	short           StockBlockLum_BuffA [16];
//	short           StockBlockLum_BuffB [16];
//	short           StockBlockLum_BuffC [16];
//	short           StockBlockLum_BuffD [16];
//	short           StockBlockLum_BuffE [16];
//	short           StockBlocksCr_BuffA [16];
//	short           StockBlocksCr_BuffB [16];
//	short           StockBlocksCr_BuffC [16];
//	short           StockBlocksCb_BuffA [16];
//	short           StockBlocksCb_BuffB [16];
//	short           StockBlocksCb_BuffC [16];
//	int             VLCinverseXi_pos [1];
//	int             VLCinverseXi_pos_prec [1];
	short           InverseQuant_BlkXn [6 * 16];
//	unsigned char   block_8x8 [64];
//	short    *DCpred_buffA [6];
//	short    *DCpred_buffB [6];
//	short    *DCpred_buffC [6];
//	unsigned char    *display [6];
//	int             width ;
//	const int       edge_size2 = EDGE_SIZE >> 1 ;
//	const int       stride = VOL.video_object_layer_width + 2 * EDGE_SIZE ;
	int pos_o;
	uchar* FrmData;
//


//	readFifo(inputFIFOIds[0], inputFIFOAddrs[0], sizeof(struct_VOLsimple), (UINT8*)&VOL);
	readFifo(inputFIFOIds[1], inputFIFOAddrs[1], sizeof(readVOPOutData), (UINT8*)&VOP);
	readFifo(inputFIFOIds[2], inputFIFOAddrs[2], BUFFER_SIZE, (UINT8*)&FrmDataWithStartCode);
	//	readFifo(inputFIFOIds[3], inputFIFOAddrs[3], sizeof(decodeVOPOutData), (UINT8*)&outputData);






//
//    init_vlc_tables_I(DCT3D_I);
//
//
//	readFifo(inputFIFOIds[0], inputFIFOAddrs[0], sizeof(struct_VOLsimple), (UINT8*)&VOL);
//	readFifo(inputFIFOIds[1], inputFIFOAddrs[1], sizeof(readVOPOutData), (UINT8*)&VOP);
//	readFifo(inputFIFOIds[2], inputFIFOAddrs[2], BUFFER_SIZE, (UINT8*)&FrmDataWithStartCode);
////	readFifo(inputFIFOIds[3], inputFIFOAddrs[3], sizeof(decodeVOPOutData), (UINT8*)&img);
//
//	stride = VOL.video_object_layer_width + 2 * EDGE_SIZE ;
//	FrmData = &FrmDataWithStartCode[4]; // Skipping the start code.
//
//    //DCpred_buffA
//    DCpred_buffA [0] = StockBlockLum_BuffA ;
//    DCpred_buffA [1] = InverseQuant_BlkXn ;
//    DCpred_buffA [2] = (StockBlockLum_BuffE);
//    DCpred_buffA [3] = (InverseQuant_BlkXn + 2 * 16);
//    DCpred_buffA [4] = (StockBlocksCb_BuffA);
//    DCpred_buffA [5] = (StockBlocksCr_BuffA);
//    //DCpred_buffB
//    DCpred_buffB [0] = StockBlockLum_BuffB ;
//    DCpred_buffB [1] = StockBlockLum_BuffC ;
//    DCpred_buffB [2] = (StockBlockLum_BuffA);
//    DCpred_buffB [3] = (InverseQuant_BlkXn);
//    DCpred_buffB [4] = (StockBlocksCb_BuffB);
//    DCpred_buffB [5] = (StockBlocksCr_BuffB);
//    //DCpred_buffC
//    DCpred_buffC [0] = StockBlockLum_BuffC ;
//    DCpred_buffC [1] = StockBlockLum_BuffD ;
//    DCpred_buffC [2] = (InverseQuant_BlkXn);
//    DCpred_buffC [3] = (InverseQuant_BlkXn + 1 * 16);
//    DCpred_buffC [4] = (StockBlocksCb_BuffC);
//    DCpred_buffC [5] = (StockBlocksCr_BuffC);
//    //tab_pos_X
//    tab_pos_X [0] = 0 ;
//    tab_pos_X [1] = 8 ;
//    tab_pos_X [2] = 8 * stride ;
//    tab_pos_X [3] = 8 * stride + 8 ;
//    //display
//    if (keyframes[0]==0){
//    	frame_address = stride * (VOL.video_object_layer_height + 2 * EDGE_SIZE) ;
//        display [0] = img.mem_Y_last_buf;
//        display [1] = img.mem_Y_last_buf;
//        display [2] = img.mem_Y_last_buf;
//        display [3] = img.mem_Y_last_buf;
//        display [4] = img.mem_U_last_buf;
//        display [5] = img.mem_V_last_buf;
//        keyframes[0]=1;
//        keyframes[1]=0;
//        frame_address = 0;
//    }
//    else
//    {
//    	frame_address = stride * (VOL.video_object_layer_height + 2 * EDGE_SIZE) ;
////	        display [0] = img.mem_Y_last_buf + frame_address;
////	        display [1] = img.mem_Y_last_buf + frame_address;
////	        display [2] = img.mem_Y_last_buf + frame_address;
////	        display [3] = img.mem_Y_last_buf + frame_address;
////	        display [4] = img.mem_U_last_buf + frame_address / 4;
////	        display [5] = img.mem_V_last_buf + frame_address / 4;
//        display [0] = img.mem_Y_last_buf;
//        display [1] = img.mem_Y_last_buf;
//        display [2] = img.mem_Y_last_buf;
//        display [3] = img.mem_Y_last_buf;
//        display [4] = img.mem_U_last_buf;
//        display [5] = img.mem_V_last_buf;
//        keyframes[0]=0;
//        keyframes[1]=1;
//        frame_address = 0;
//    }
//
//    pos_o = VOP.VideoObjectPlane_pos;
//    StockBlocksLum_init(&VOL, InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16);












//	init_vlc_tables_I(DCT3D_I);
//	FrmData = &FrmDataWithStartCode[4]; // Skipping the start code.

	//display
//	if (keyframes[0]==0){
//		outputData.frame_address = stride * (VOL.video_object_layer_height + 2 * EDGE_SIZE) ;
//		display [0] = outputData.mem_Y_last_buf;
//		display [1] = outputData.mem_Y_last_buf;
//		display [2] = outputData.mem_Y_last_buf;
//		display [3] = outputData.mem_Y_last_buf;
//		display [4] = outputData.mem_U_last_buf;
//		display [5] = outputData.mem_V_last_buf;
//		keyframes[0]=1;
//		keyframes[1]=0;
//	}
//	else
//	{
//		outputData.frame_address = stride * (VOL.video_object_layer_height + 2 * EDGE_SIZE) ;
//		display [0] = outputData.mem_Y_last_buf + outputData.frame_address;
//		display [1] = outputData.mem_Y_last_buf + outputData.frame_address;
//		display [2] = outputData.mem_Y_last_buf + outputData.frame_address;
//		display [3] = outputData.mem_Y_last_buf + outputData.frame_address;
//		display [4] = outputData.mem_U_last_buf + outputData.frame_address / 4;
//		display [5] = outputData.mem_V_last_buf + outputData.frame_address / 4;
//		keyframes[0]=0;
//		keyframes[1]=1;
//		outputData.frame_address = 0;
//	}

//	pos_o = VOP.VideoObjectPlane_pos;
//	StockBlocksLum_init(&VOL, InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16);


	// Sending output data.
//	writeFifo(outputFIFOIds[0], outputFIFOAddrs[0], sizeof(decodeVOPOutData), (UINT8*)&outputData);
}
