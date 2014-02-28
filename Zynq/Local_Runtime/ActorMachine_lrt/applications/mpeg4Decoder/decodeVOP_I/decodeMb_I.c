
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

#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include <lrt_1W1RfifoMngr.h>
#include <hwQueues.h>
#include <lrt_core.h>
#include <lrt_taskMngr.h>

static uchar FrmDataWithStartCode[BUFFER_SIZE];
static decodeVOPOutData imgPrec;
static decodeVOPOutData img;


short   InvDCT_f[6][128];
short   DCpred_F00pred[6];
short   DCpred_QFpred[6][7];
short   DCpred_QPpred[6];
int 	pos_fin_vlc[6];
int     DCpred_prediction_direction[6];

static void decodeBlk_I(
		const int k,
		const short *InvDCT_f,
		const short *RESTRICT DCpred_QFpred,
		const short DCpred_F00pred,
		const short DCpred_QPpred,
		const struct_VOP *RESTRICT VOP,
		const int DCpred_prediction_direction,
//		const unsigned char *RESTRICT data,
//		const int pos_i,
//		const REVERSE_EVENT *RESTRICT DCT3D_I,
		const struct_VOLsimple *RESTRICT VOLsimple,
//		const int MB_pos,
//		const short *RESTRICT Blk_A,
//		const short *RESTRICT Blk_B,
//		const short *RESTRICT Blk_C,
		short *RESTRICT InverseQuant_Blk_X,
		unsigned char *RESTRICT block_sat_X
//		int *RESTRICT pos_fin_vlc
		){

    short   iDCT_data [64];
    int     InverseACDCpred_dc_scaler;

    InverseACDCpred(
    		k,
    		InvDCT_f,
    		DCpred_QFpred,
    		DCpred_F00pred,
    		DCpred_QPpred,
    		VOP,
    		DCpred_prediction_direction,
    		iDCT_data,
    		&InverseACDCpred_dc_scaler);

    InverseQuantI(
    		iDCT_data,
    		VOP,
    		InverseACDCpred_dc_scaler,
    		VOLsimple,
    		InvDCT_f,
    		InverseQuant_Blk_X);

    InverseDCT_optim(InvDCT_f);
    block_sat(InvDCT_f, block_sat_X);
}



void decodeMB_I(UINT32 inputFIFOIds[],
		 UINT32 inputFIFOAddrs[],
		 UINT32 outputFIFOIds[],
		 UINT32 outputFIFOAddrs[],
		 UINT32 params[]){

	REVERSE_EVENT DCT3D_I[4096]; //init_vlc_tables_I_PC_decod1_DCT3D_I

	static int keyframes[2] = {0};


	struct_VOLsimple VOL;
	readVOPOutData VOP;
//	uchar FrmDataWithStartCode[BUFFER_SIZE];




	   int             i, j, k ;
	    int             MB_courant = 0 ;
	    int             MB_number ;
	    int             pos_X [1];
	    int             tab_pos_X [4];
	    struct_VOP      new_VOP [1];
	    int             VideoPacketHeader_pos [1];
	    int             MacroblockI_pos [1];
	    int             VideoPacketHeader_resync_marker [1];
	    short           StockBlockLum_BuffA [16];
	    short           StockBlockLum_BuffB [16];
	    short           StockBlockLum_BuffC [16];
	    short           StockBlockLum_BuffD [16];
	    short           StockBlockLum_BuffE [16];
	    short           StockBlocksCr_BuffA [16];
	    short           StockBlocksCr_BuffB [16];
	    short           StockBlocksCr_BuffC [16];
	    short           StockBlocksCb_BuffA [16];
	    short           StockBlocksCb_BuffB [16];
	    short           StockBlocksCb_BuffC [16];
	    int             VLCinverseXi_pos [1];
	    int             VLCinverseXi_pos_prec [1];
	    short           InverseQuant_BlkXn [6 * 16];
	    unsigned char   block_8x8 [64];
	    short    		*DCpred_buffA [6];
	    short    		*DCpred_buffB [6];
	    short    		*DCpred_buffC [6];
	    unsigned char   *display [6];
	    int             width ;
	    const int       edge_size2 = EDGE_SIZE >> 1 ;
	    int       		stride;
	    int pos_o;
	    uchar* FrmData;
	    int frame_address;

	    init_vlc_tables_I(DCT3D_I);


		readFifo(inputFIFOIds[0], inputFIFOAddrs[0], sizeof(struct_VOLsimple), (UINT8*)&VOL);
		readFifo(inputFIFOIds[1], inputFIFOAddrs[1], sizeof(readVOPOutData), (UINT8*)&VOP);
		readFifo(inputFIFOIds[2], inputFIFOAddrs[2], BUFFER_SIZE, (UINT8*)&FrmDataWithStartCode);
	//	readFifo(inputFIFOIds[3], inputFIFOAddrs[3], sizeof(decodeVOPOutData), (UINT8*)&img);

		stride = VOL.video_object_layer_width + 2 * EDGE_SIZE ;
		FrmData = &FrmDataWithStartCode[4]; // Skipping the start code.

	    //DCpred_buffA
	    DCpred_buffA [0] = StockBlockLum_BuffA ;
	    DCpred_buffA [1] = InverseQuant_BlkXn ;
	    DCpred_buffA [2] = (StockBlockLum_BuffE);
	    DCpred_buffA [3] = (InverseQuant_BlkXn + 2 * 16);
	    DCpred_buffA [4] = (StockBlocksCb_BuffA);
	    DCpred_buffA [5] = (StockBlocksCr_BuffA);
	    //DCpred_buffB
	    DCpred_buffB [0] = StockBlockLum_BuffB ;
	    DCpred_buffB [1] = StockBlockLum_BuffC ;
	    DCpred_buffB [2] = (StockBlockLum_BuffA);
	    DCpred_buffB [3] = (InverseQuant_BlkXn);
	    DCpred_buffB [4] = (StockBlocksCb_BuffB);
	    DCpred_buffB [5] = (StockBlocksCr_BuffB);
	    //DCpred_buffC
	    DCpred_buffC [0] = StockBlockLum_BuffC ;
	    DCpred_buffC [1] = StockBlockLum_BuffD ;
	    DCpred_buffC [2] = (InverseQuant_BlkXn);
	    DCpred_buffC [3] = (InverseQuant_BlkXn + 1 * 16);
	    DCpred_buffC [4] = (StockBlocksCb_BuffC);
	    DCpred_buffC [5] = (StockBlocksCr_BuffC);
	    //tab_pos_X
	    tab_pos_X [0] = 0 ;
	    tab_pos_X [1] = 8 ;
	    tab_pos_X [2] = 8 * stride ;
	    tab_pos_X [3] = 8 * stride + 8 ;
	    //display
	    if (keyframes[0]==0){
	    	frame_address = stride * (VOL.video_object_layer_height + 2 * EDGE_SIZE) ;
	        display [0] = img.mem_Y_last_buf;
	        display [1] = img.mem_Y_last_buf;
	        display [2] = img.mem_Y_last_buf;
	        display [3] = img.mem_Y_last_buf;
	        display [4] = img.mem_U_last_buf;
	        display [5] = img.mem_V_last_buf;
	        keyframes[0]=1;
	        keyframes[1]=0;
	        frame_address = 0;
	    }
	    else
	    {
	    	frame_address = stride * (VOL.video_object_layer_height + 2 * EDGE_SIZE) ;
//	        display [0] = img.mem_Y_last_buf + frame_address;
//	        display [1] = img.mem_Y_last_buf + frame_address;
//	        display [2] = img.mem_Y_last_buf + frame_address;
//	        display [3] = img.mem_Y_last_buf + frame_address;
//	        display [4] = img.mem_U_last_buf + frame_address / 4;
//	        display [5] = img.mem_V_last_buf + frame_address / 4;
	        display [0] = img.mem_Y_last_buf;
	        display [1] = img.mem_Y_last_buf;
	        display [2] = img.mem_Y_last_buf;
	        display [3] = img.mem_Y_last_buf;
	        display [4] = img.mem_U_last_buf;
	        display [5] = img.mem_V_last_buf;
	        keyframes[0]=0;
	        keyframes[1]=1;
	        frame_address = 0;
	    }

	    pos_o = VOP.VideoObjectPlane_pos;
	    StockBlocksLum_init(&VOL, InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16);
	    for ( MB_courant = 0 ;
	        MB_courant < VOL.video_object_layer_width * VOL.video_object_layer_height / 256 ;
	        MB_courant++ ) {


				if ( MB_courant == 342)
					MB_courant += 0;
				VideoPacketHeaderI(FrmData, pos_o, &VOL, &VOP.VideoObjectPlane_VOP, &MB_courant, new_VOP, VideoPacketHeader_pos
	                , VideoPacketHeader_resync_marker, &MB_number);
	            MB_courant = MB_number ;
	            i = MB_courant % (VOL.video_object_layer_width / 16);
	            j = MB_courant / (VOL.video_object_layer_width / 16);
	            Param_MB_I(VideoPacketHeader_pos [0], FrmData, new_VOP, &VOP.VideoObjectPlane_VOP, MacroblockI_pos);
	            StockBlocksLum(i, VideoPacketHeader_resync_marker [0], InverseQuant_BlkXn + 1 * 16, InverseQuant_BlkXn + 2 * 16
	                , InverseQuant_BlkXn + 3 * 16, &VOL, StockBlockLum_BuffA, StockBlockLum_BuffB, StockBlockLum_BuffC
	                , StockBlockLum_BuffD, StockBlockLum_BuffE);
	            StockBlocksCb(MB_courant, InverseQuant_BlkXn + 4 * 16, &VOL, VideoPacketHeader_resync_marker [0]
	            , StockBlocksCb_BuffA, StockBlocksCb_BuffB, StockBlocksCb_BuffC);
	            StockBlocksCr(MB_courant, InverseQuant_BlkXn + 5 * 16, &VOL, VideoPacketHeader_resync_marker [0]
	            , StockBlocksCr_BuffA, StockBlocksCr_BuffB, StockBlocksCr_BuffC);
	            VLCinverseXi_pos_prec [0] = MacroblockI_pos [0];


//                short   InvDCT_f[6][128];
//                short   DCpred_F00pred[6];
//                short   DCpred_QFpred[6][7];
//                short   DCpred_QPpred[6];
//	            int 	pos_fin_vlc[6];
//                int     DCpred_prediction_direction[6];

	            for ( k = 0 ; k < 6 ; k++ ) {
	                VLCinverseI(5 - k, MB_courant, VLCinverseXi_pos_prec[0], FrmData, DCT3D_I,
	                			DCpred_buffA[k], DCpred_buffB[k], DCpred_buffC[k], &VOP.VideoObjectPlane_VOP,
	                			&pos_fin_vlc[k], &InvDCT_f[k][0], DCpred_QFpred[k], &DCpred_F00pred[k], &DCpred_QPpred[k], &DCpred_prediction_direction[k]);
	                VLCinverseXi_pos_prec[0] = pos_fin_vlc[k];
	            }

	            // Luminances
	            width = VOL.video_object_layer_width ;
	            pos_X [0] = ((i + j * stride) << 4) + EDGE_SIZE + EDGE_SIZE * stride ;
	            decodeBlk_I(0, &InvDCT_f[0][0], DCpred_QFpred[0], DCpred_F00pred[0], DCpred_QPpred[0], &VOP.VideoObjectPlane_VOP, DCpred_prediction_direction[0],
	            			&VOL, InverseQuant_BlkXn + 0 * 16, block_8x8);
	            Stock_block_in_pict(width + 2 * EDGE_SIZE, pos_X [0] + tab_pos_X [0], block_8x8, display [0]);

	            decodeBlk_I(1, &InvDCT_f[1][0], DCpred_QFpred[1], DCpred_F00pred[1], DCpred_QPpred[1], &VOP.VideoObjectPlane_VOP, DCpred_prediction_direction[1],
	            			&VOL, InverseQuant_BlkXn + 1 * 16, block_8x8);
	            Stock_block_in_pict(width + 2 * EDGE_SIZE, pos_X [0] + tab_pos_X [1], block_8x8, display [0]);

	            decodeBlk_I(2, &InvDCT_f[2][0], DCpred_QFpred[2], DCpred_F00pred[2], DCpred_QPpred[2], &VOP.VideoObjectPlane_VOP, DCpred_prediction_direction[2],
	            			&VOL, InverseQuant_BlkXn + 2 * 16, block_8x8);
	            Stock_block_in_pict(width + 2 * EDGE_SIZE, pos_X [0] + tab_pos_X [2], block_8x8, display [0]);

	            decodeBlk_I(3, &InvDCT_f[3][0], DCpred_QFpred[3], DCpred_F00pred[3], DCpred_QPpred[3], &VOP.VideoObjectPlane_VOP, DCpred_prediction_direction[3],
	            			&VOL, InverseQuant_BlkXn + 3 * 16, block_8x8);
	            Stock_block_in_pict(width + 2 * EDGE_SIZE, pos_X [0] + tab_pos_X [3], block_8x8, display [0]);

	            // Chrominances
	            width = VOL.video_object_layer_width >> 1 ;
	            pos_X [0] = ((i + j * (stride >> 1)) << 3) + edge_size2 + edge_size2 * (stride >> 1);
	            decodeBlk_I(4, &InvDCT_f[4][0], DCpred_QFpred[4], DCpred_F00pred[4], DCpred_QPpred[4], &VOP.VideoObjectPlane_VOP, DCpred_prediction_direction[4],
            				&VOL, InverseQuant_BlkXn + 4 * 16, block_8x8);
	            Stock_block_in_pict(width + 2 * edge_size2, pos_X [0], block_8x8, display [4]);

	            decodeBlk_I(5, &InvDCT_f[5][0], DCpred_QFpred[5], DCpred_F00pred[5], DCpred_QPpred[5], &VOP.VideoObjectPlane_VOP, DCpred_prediction_direction[5],
            				&VOL, InverseQuant_BlkXn + 5 * 16, block_8x8);
	            Stock_block_in_pict(width + 2 * edge_size2, pos_X [0], block_8x8, display [5]);

	            pos_o = pos_fin_vlc[k-1];




	    }
	    image_setedges( (display [0]),  (display [4]),  (display [5])
	    	,  (display [0]),  (display [4]),  (display [5]), VOL.video_object_layer_width + 2 * EDGE_SIZE
	        , VOL.video_object_layer_height);


	// Sending output data.
	writeFifo(outputFIFOIds[0], outputFIFOAddrs[0], sizeof(decodeVOPOutData), (UINT8*)&img);
	writeFifo(outputFIFOIds[1], outputFIFOAddrs[1], sizeof(decodeVOPOutData), (UINT8*)&img);
	writeFifo(outputFIFOIds[2], outputFIFOAddrs[2], sizeof(int), (UINT8*)&frame_address);
}
