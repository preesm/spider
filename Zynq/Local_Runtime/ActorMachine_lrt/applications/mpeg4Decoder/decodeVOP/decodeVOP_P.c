
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
 * FrmData to be ensured and,  more generally, to use and operate it in the 	*
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

void decodeVOP_P(UINT32 inputFIFOIds[],
		 UINT32 inputFIFOAddrs[],
		 UINT32 outputFIFOIds[],
		 UINT32 outputFIFOAddrs[],
		 UINT32 params[]){

	REVERSE_EVENT DCT3D_I[4096]; // init_vlc_tables_I_PC_decod1_DCT3D_I
	REVERSE_EVENT DCT3D_P[4096]; // init_vlc_tables_P_PC_decod1_DCT3D_P;
	const unsigned int   roundtab [16] = { 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2 };
	uchar stock_mb_type_P[1620];
	vector save_mv[1620];

	struct_VOLsimple VOL;
	readVOPOutData VOP;

	int keyframes[2] = {0};

	    int             i, j, k ;
	    int             MB_courant ;
	    int             MB_number ;
	    int             pos_X [1];
	    int             tab_pos_X [4];
	    struct_VOP      new_VOP [1];
	    int             pos [1];
	    int             pos2 [1];
	    int             mb_not_coded [1];
	    int             mb_type [1];
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
	    int           	width, height ;
	    int           	width_edged ;
	    vector          *current_vector ;
	    int             uv_dx, uv_dy ;
	    int             dx, dy ;
	    int             x_pos_inter, y_pos_inter ;
	    int             tab_x_pos_inter [4] = { 0, 8, 0, 8 };
	    int             tab_y_pos_inter [4] = { 0, 0, 8, 8 };
	    const int       edge_size2 = EDGE_SIZE >> 1 ;
	    unsigned char   *display [6];
	    unsigned char   *display_prec [6];
	    unsigned int    mov_display_prec [6];
	    int       		stride;
	    int pos_o;
	    uchar* FrmData;
	    int frame_address;


		readFifo(inputFIFOIds[0], inputFIFOAddrs[0], sizeof(struct_VOLsimple), (UINT8*)&VOL);
		readFifo(inputFIFOIds[1], inputFIFOAddrs[1], sizeof(readVOPOutData), (UINT8*)&VOP);
		readFifo(inputFIFOIds[2], inputFIFOAddrs[2], BUFFER_SIZE, (UINT8*)&FrmDataWithStartCode);
		readFifo(inputFIFOIds[3], inputFIFOAddrs[3], sizeof(decodeVOPOutData), (UINT8*)&imgPrec);

		// Initializations...
		init_vlc_tables_P(DCT3D_P);
		init_vlc_tables_I(DCT3D_I);
		stride = VOL.video_object_layer_width + 2 * EDGE_SIZE ;
		FrmData = &FrmDataWithStartCode[4]; // Skipping the start code.

	    pos_o = VOP.VideoObjectPlane_pos ;
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
//	    if (keyframes[0]==0){
//	        img.frame_address = stride * (VOL.video_object_layer_height + 2 * EDGE_SIZE);
//	        display [0] = img.mem_Y_last_buf ;
//	        display [1] = img.mem_Y_last_buf ;
//	        display [2] = img.mem_Y_last_buf ;
//	        display [3] = img.mem_Y_last_buf ;
//	        display [4] = img.mem_U_last_buf ;
//	        display [5] = img.mem_V_last_buf ;
//	        display_prec [0] = img.mem_Y_last_buf + img.frame_address;
//	        display_prec [1] = img.mem_Y_last_buf + img.frame_address;
//	        display_prec [2] = img.mem_Y_last_buf + img.frame_address;
//	        display_prec [3] = img.mem_Y_last_buf + img.frame_address;
//	        display_prec [4] = img.mem_U_last_buf + img.frame_address / 4;
//	        display_prec [5] = img.mem_V_last_buf + img.frame_address / 4;
//	        keyframes[0]=1;
//	        keyframes[1]=0;
//	    }
//	    else
	    {
//	        frame_address = stride * (VOL.video_object_layer_height + 2 * EDGE_SIZE);
//	        display [0] = (img.mem_Y_last_buf + frame_address);
//	        display [1] = (img.mem_Y_last_buf + frame_address);
//	        display [2] = (img.mem_Y_last_buf + frame_address);
//	        display [3] = (img.mem_Y_last_buf + frame_address);
//	        display [4] = (img.mem_U_last_buf + frame_address / 4);
//	        display [5] = (img.mem_V_last_buf + frame_address / 4);
	        display [0] = (img.mem_Y_last_buf);
	        display [1] = (img.mem_Y_last_buf);
	        display [2] = (img.mem_Y_last_buf);
	        display [3] = (img.mem_Y_last_buf);
	        display [4] = (img.mem_U_last_buf);
	        display [5] = (img.mem_V_last_buf);
	        display_prec [0] = imgPrec.mem_Y_last_buf ;
	        display_prec [1] = imgPrec.mem_Y_last_buf ;
	        display_prec [2] = imgPrec.mem_Y_last_buf ;
	        display_prec [3] = imgPrec.mem_Y_last_buf ;
	        display_prec [4] = imgPrec.mem_U_last_buf ;
	        display_prec [5] = imgPrec.mem_V_last_buf ;
	        keyframes[0]=0;
	        keyframes[1]=1;
	        frame_address = 0;
	    }

	    //mov_display_prec
	    mov_display_prec [0] = EDGE_SIZE + EDGE_SIZE * (stride);
	    mov_display_prec [1] = EDGE_SIZE + EDGE_SIZE * (stride);
	    mov_display_prec [2] = EDGE_SIZE + EDGE_SIZE * (stride);
	    mov_display_prec [3] = EDGE_SIZE + EDGE_SIZE * (stride);
	    mov_display_prec [4] = edge_size2 + edge_size2 * (stride >> 1);
	    mov_display_prec [5] = edge_size2 + edge_size2 * (stride >> 1);
	    StockBlocksLum_init(&VOL, InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16);
	    for ( MB_courant = 0 ;
	        MB_courant < VOL.video_object_layer_width * VOL.video_object_layer_height / 256 ;
	        MB_courant++ ) {
				VideoPacketHeaderP(FrmData, pos_o, &VOL, &VOP.VideoObjectPlane_VOP, &MB_courant, new_VOP, pos
	                , VideoPacketHeader_resync_marker, &MB_number);
	            MB_courant = MB_number ;
	            i = MB_courant % (VOL.video_object_layer_width / 16);
	            j = MB_courant / (VOL.video_object_layer_width / 16);
	            Param_MB_P(pos [0], FrmData, new_VOP, &MB_courant, &VOP.VideoObjectPlane_VOP, pos2, mb_not_coded, mb_type, stock_mb_type_P);
	            current_vector = &(save_mv [MB_courant]);
	            /* sauvegarde de 4 vecteur par MB, à optimiser */
	            if ( !mb_not_coded [0] ) {
	            	printf("%u\n", MB_courant);
	                /* CODED */
	                if ( mb_type [0] == MODE_INTER || mb_type [0] == MODE_INTER_Q ) {

	                    /* MODE INTER, INTER_Q */
	                    get_motion_vector(0, save_mv, &VOL, FrmData, pos2 [0], &MB_courant, &VOP.VideoObjectPlane_VOP, current_vector, pos);
	                    current_vector -> mvs [1].x = current_vector -> mvs [2].x = current_vector -> mvs [3].x
	                        = current_vector -> mvs [0].x ;
	                    current_vector -> mvs [1].y = current_vector -> mvs [2].y = current_vector -> mvs [3].y
	                        = current_vector -> mvs [0].y ;
	                    uv_dx = current_vector -> mvs [0].x ;
	                    uv_dy = current_vector -> mvs [0].y ;
	                    uv_dx = uv_dx & 3 ? uv_dx >> 1 | 1 : uv_dx / 2 ;
	                    uv_dy = uv_dy & 3 ? uv_dy >> 1 | 1 : uv_dy / 2 ;
	                    StockBlocksLum(i, VideoPacketHeader_resync_marker [0], InverseQuant_BlkXn + 1 * 16
	                        , InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16, &VOL, StockBlockLum_BuffA
	                        , StockBlockLum_BuffB, StockBlockLum_BuffC, StockBlockLum_BuffD, StockBlockLum_BuffE);
	                    StockBlocksCb(MB_courant, InverseQuant_BlkXn + 4 * 16, &VOL, VideoPacketHeader_resync_marker [0]
	                    , StockBlocksCb_BuffA, StockBlocksCb_BuffB, StockBlocksCb_BuffC);
	                    StockBlocksCr(MB_courant, InverseQuant_BlkXn + 5 * 16, &VOL, VideoPacketHeader_resync_marker [0]
	                    , StockBlocksCr_BuffA, StockBlocksCr_BuffB, StockBlocksCr_BuffC);
	                    VLCinverseXi_pos_prec [0] = pos [0];
	                    //display_img = imgLum ;
	                    width = VOL.video_object_layer_width ;
	                    height = VOL.video_object_layer_height ;
	                    x_pos_inter = 16 * i ;
	                    y_pos_inter = 16 * j ;
	                    pos_X [0] = ((i + j * stride) << 4) + EDGE_SIZE + EDGE_SIZE * stride;
	                    for ( k = 0 ; k < 4 ; k++ ) {
	                        dx = current_vector -> mvs [k].x ;
	                        dy = current_vector -> mvs [k].y ;
	                        decode_bloc_inter(k, FrmData, &VOP.VideoObjectPlane_VOP, VLCinverseXi_pos_prec [0], DCT3D_P, &VOL, width, height, stride
	                            , x_pos_inter + tab_x_pos_inter [k], y_pos_inter + tab_y_pos_inter [k], dx, dy
	                            ,  (display_prec [k] + mov_display_prec [k])
	                            , block_8x8, VLCinverseXi_pos);
	                        Stock_block_in_pict(stride, pos_X [0] + tab_pos_X [k], block_8x8,  display [k]);
	                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
	                    }
	                    pos_X [0] = ((i + j * (stride >> 1)) << 3) + edge_size2 + edge_size2 * (stride >> 1);
	                    dx = uv_dx ;
	                    dy = uv_dy ;
	                    width = VOL.video_object_layer_width / 2 ;
	                    height = VOL.video_object_layer_height / 2;
	                    width_edged = stride >> 1 ;
	                    x_pos_inter = 8 * i ;
	                    y_pos_inter = 8 * j ;
	                    for ( k = 4 ; k < 6 ; k++ ) {
	                        decode_bloc_inter(k, FrmData, &VOP.VideoObjectPlane_VOP, VLCinverseXi_pos_prec [0], DCT3D_P, &VOL, width, height, width_edged
	                            , x_pos_inter, y_pos_inter, dx, dy,  (display_prec [k] + mov_display_prec [k])
	                            , block_8x8, VLCinverseXi_pos);
	                        Stock_block_in_pict(stride >> 1, pos_X [0], block_8x8,  display [k]);
	                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
	                    }
	                    pos_o = VLCinverseXi_pos [0];
	                    for ( k = 1 ; k < 6 ; k++ ) {
	                        memset(InverseQuant_BlkXn + k * 16, 0, 16 * sizeof(short));
	                        *(InverseQuant_BlkXn + k * 16) = 1024 ;
	                    }
	                } //fin INTER MODE
	                else if ( mb_type [0] == MODE_INTER4V ) {

	                    /* MODE INTER4V */
	                    int sum ;
	                    get_motion_vector(0, save_mv, &VOL, FrmData, pos2 [0], &MB_courant, &VOP.VideoObjectPlane_VOP, current_vector, pos);
	                    get_motion_vector(1, save_mv, &VOL, FrmData, pos [0], &MB_courant, &VOP.VideoObjectPlane_VOP, current_vector, pos2);
	                    get_motion_vector(2, save_mv, &VOL, FrmData, pos2 [0], &MB_courant, &VOP.VideoObjectPlane_VOP, current_vector, pos);
	                    get_motion_vector(3, save_mv, &VOL, FrmData, pos [0], &MB_courant, &VOP.VideoObjectPlane_VOP, current_vector, pos2);
	                    sum = current_vector -> mvs [0].x + current_vector -> mvs [1].x + current_vector -> mvs [2].x
	                        + current_vector -> mvs [3].x ;
	                    uv_dx = sum == 0 ? 0 : SIGN(sum) * (roundtab [ABS(sum) % 16] + ABS(sum) / 16 * 2);
	                    sum = current_vector -> mvs [0].y + current_vector -> mvs [1].y + current_vector -> mvs [2].y
	                        + current_vector -> mvs [3].y ;
	                    uv_dy = sum == 0 ? 0 : SIGN(sum) * (roundtab [ABS(sum) % 16] + ABS(sum) / 16 * 2);
	                    StockBlocksLum(i, VideoPacketHeader_resync_marker [0], InverseQuant_BlkXn + 1 * 16
	                        , InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16, &VOL, StockBlockLum_BuffA
	                        , StockBlockLum_BuffB, StockBlockLum_BuffC, StockBlockLum_BuffD, StockBlockLum_BuffE);
	                    StockBlocksCb(MB_courant, InverseQuant_BlkXn + 4 * 16, &VOL, VideoPacketHeader_resync_marker [0]
	                    , StockBlocksCb_BuffA, StockBlocksCb_BuffB, StockBlocksCb_BuffC);
	                    StockBlocksCr(MB_courant, InverseQuant_BlkXn + 5 * 16, &VOL, VideoPacketHeader_resync_marker [0]
	                    , StockBlocksCr_BuffA, StockBlocksCr_BuffB, StockBlocksCr_BuffC);
	                    VLCinverseXi_pos_prec [0] = pos2 [0];
	                    //display_img = imgLum ;
	                    width = VOL.video_object_layer_width ;
	                    height = VOL.video_object_layer_height ;
	                    width_edged = VOL.video_object_layer_width + 2 * EDGE_SIZE ;
	                    pos_X [0] = ((i + j * stride) << 4) + EDGE_SIZE + EDGE_SIZE * stride;
	                    for ( k = 0 ; k < 4 ; k++ ) {
	                        dx = current_vector -> mvs [k].x ;
	                        dy = current_vector -> mvs [k].y ;
	                        x_pos_inter = 16 * i + tab_x_pos_inter [k];
	                        y_pos_inter = 16 * j + tab_y_pos_inter [k];
	                        decode_bloc_inter(k, FrmData, &VOP.VideoObjectPlane_VOP, VLCinverseXi_pos_prec [0], DCT3D_P, &VOL, width, height, width_edged
	                            , x_pos_inter, y_pos_inter, dx, dy,  (display_prec [k] + mov_display_prec [k])
	                            , block_8x8, VLCinverseXi_pos);
	                        Stock_block_in_pict(stride, pos_X [0] + tab_pos_X [k], block_8x8,  display [k]);
	                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
	                    } // for Xi
	                    x_pos_inter = 8 * i ;
	                    y_pos_inter = 8 * j ;
	                    dx = uv_dx ;
	                    dy = uv_dy ;
	                    width = VOL.video_object_layer_width / 2 ;
	                    height = VOL.video_object_layer_height / 2;
	                    width_edged = (VOL.video_object_layer_width + 2 * EDGE_SIZE) / 2 ;
	                    pos_X [0] = ((i + j * (stride >> 1)) << 3) + edge_size2 + edge_size2 * (stride >> 1);
	                    for ( k = 4 ; k < 6 ; k++ ) {
	                        decode_bloc_inter(k, FrmData, &VOP.VideoObjectPlane_VOP, VLCinverseXi_pos_prec [0], DCT3D_P, &VOL, width, height, width_edged
	                            , x_pos_inter, y_pos_inter, dx, dy,  (display_prec [k] + mov_display_prec [k])
	                            , block_8x8, VLCinverseXi_pos);
	                        Stock_block_in_pict(stride >> 1, pos_X [0], block_8x8,  display [k]);
	                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
	                    } // for Xi
	                    pos_o = VLCinverseXi_pos [0];
	                    for ( k = 1 ; k < 6 ; k++ ) {
	                        memset(InverseQuant_BlkXn + k * 16, 0, 16 * sizeof(short));
	                        *(InverseQuant_BlkXn + k * 16) = 1024 ;
	                    }
	                } //fin INTER4V MODE
	                else {

	                    /* MODE_INTRA, MODE_INTRA_Q */
	                    current_vector -> mvs [0].x = current_vector -> mvs [1].x = current_vector -> mvs [2].x
	                        = current_vector -> mvs [3].x = 0 ;
	                    current_vector -> mvs [0].y = current_vector -> mvs [1].y = current_vector -> mvs [2].y
	                        = current_vector -> mvs [3].y = 0 ;
	                    StockBlocksLum(i, VideoPacketHeader_resync_marker [0], InverseQuant_BlkXn + 1 * 16
	                        , InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16, &VOL, StockBlockLum_BuffA
	                        , StockBlockLum_BuffB, StockBlockLum_BuffC, StockBlockLum_BuffD, StockBlockLum_BuffE);
	                    StockBlocksCb(MB_courant, InverseQuant_BlkXn + 4 * 16, &VOL, VideoPacketHeader_resync_marker [0]
	                    , StockBlocksCb_BuffA, StockBlocksCb_BuffB, StockBlocksCb_BuffC);
	                    StockBlocksCr(MB_courant, InverseQuant_BlkXn + 5 * 16, &VOL, VideoPacketHeader_resync_marker [0]
	                    , StockBlocksCr_BuffA, StockBlocksCr_BuffB, StockBlocksCr_BuffC);
	                    VLCinverseXi_pos_prec [0] = pos2 [0];
	                    width = VOL.video_object_layer_width ;
	                    pos_X [0] = ((i + j * stride) << 4) + EDGE_SIZE + EDGE_SIZE * stride;
	                    //pos_X[0] = i * 16 + 16 * j * width ;
	                    for ( k = 0 ; k < 4 ; k++ ) {
	                        decode_bloc_intra(k, FrmData, &VOP.VideoObjectPlane_VOP, VLCinverseXi_pos_prec [0], DCT3D_I, &VOL, MB_courant
	                            , DCpred_buffA [k], DCpred_buffB [k], DCpred_buffC [k]
	                        , InverseQuant_BlkXn + k * 16, block_8x8, VLCinverseXi_pos);
	                        Stock_block_in_pict(stride, pos_X [0] + tab_pos_X [k], block_8x8,  display [k]);
	                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
	                    }
	                    width = VOL.video_object_layer_width >> 1 ;
	                    pos_X [0] = ((i + j * (stride >> 1)) << 3) + edge_size2 + edge_size2 * (stride >> 1);
	                    for ( k = 4 ; k < 6 ; k++ ) {
	                        decode_bloc_intra(k, FrmData, &VOP.VideoObjectPlane_VOP, VLCinverseXi_pos_prec [0], DCT3D_I, &VOL, MB_courant
	                            , DCpred_buffA [k], DCpred_buffB [k], DCpred_buffC [k]
	                        , InverseQuant_BlkXn + k * 16, block_8x8, VLCinverseXi_pos);
	                        Stock_block_in_pict(stride >> 1, pos_X [0], block_8x8,  display [k]);
	                        VLCinverseXi_pos_prec [0] = VLCinverseXi_pos [0];
	                    } // for Xi
	                    pos_o = VLCinverseXi_pos [0];
	                } //fin INTRA MODE
	            } //fin CODED
	            else {

	                /* NOT_CODED */
	                VOP.VideoObjectPlane_VOP.mb_type = MODE_NOT_CODED ;
	                current_vector -> mvs [0].x = current_vector -> mvs [1].x = current_vector -> mvs [2].x
	                    = current_vector -> mvs [3].x = 0 ;
	                current_vector -> mvs [0].y = current_vector -> mvs [1].y = current_vector -> mvs [2].y
	                    = current_vector -> mvs [3].y = 0 ;
	                pos_o = *pos2 ;
	                StockBlocksLum(i, VideoPacketHeader_resync_marker [0], InverseQuant_BlkXn + 1 * 16
	                    , InverseQuant_BlkXn + 2 * 16, InverseQuant_BlkXn + 3 * 16, &VOL, StockBlockLum_BuffA
	                    , StockBlockLum_BuffB, StockBlockLum_BuffC, StockBlockLum_BuffD, StockBlockLum_BuffE);
	                StockBlocksCb(MB_courant, InverseQuant_BlkXn + 4 * 16, &VOL, VideoPacketHeader_resync_marker [0]
	                , StockBlocksCb_BuffA, StockBlocksCb_BuffB, StockBlocksCb_BuffC);
	                StockBlocksCr(MB_courant, InverseQuant_BlkXn + 5 * 16, &VOL, VideoPacketHeader_resync_marker [0]
	                , StockBlocksCr_BuffA, StockBlocksCr_BuffB, StockBlocksCr_BuffC);
	                for ( k = 1 ; k < 6 ; k++ ) {
	                    memset(InverseQuant_BlkXn + k * 16, 0, 16 * sizeof(short));
	                    *(InverseQuant_BlkXn + k * 16) = 1024 ;
	                }
	                //pt_display_prec =  Y_prec ;
	                width = VOL.video_object_layer_width ;
	                pos_X [0] = ((i + j * stride) << 4) + EDGE_SIZE + EDGE_SIZE * stride;
	                for ( k = 0 ; k < 4 ; k++ ) {
	                    Stock_block_from_prev_pict_in_pict(stride, pos_X [0] + tab_pos_X [k],  display_prec [k]
	                    ,  display [k]);
	                }
	                width = VOL.video_object_layer_width >> 1 ;
	                pos_X [0] = ((i + j * (stride >> 1)) << 3) + edge_size2 + edge_size2 * (stride >> 1);
	                for ( k = 4 ; k < 6 ; k++ ) {
	                    Stock_block_from_prev_pict_in_pict(stride >> 1, pos_X [0],  display_prec [k]
	                    ,  display [k]);
	                }
	            } //fin NOT_CODED
	    } //for MB
	    image_setedges( (display [0]),  (display [4]),  (display [5])
	    	,  (display [0]),  (display [4]),  (display [5]), VOL.video_object_layer_width + 2 * EDGE_SIZE
	        , VOL.video_object_layer_height);
	    //for B-frames

	// Sending output data.
	writeFifo(outputFIFOIds[0], outputFIFOAddrs[0], sizeof(decodeVOPOutData), (UINT8*)&img);
	writeFifo(outputFIFOIds[1], outputFIFOAddrs[1], sizeof(decodeVOPOutData), (UINT8*)&img);
	writeFifo(outputFIFOIds[2], outputFIFOAddrs[2], sizeof(int), (UINT8*)&frame_address);
}
