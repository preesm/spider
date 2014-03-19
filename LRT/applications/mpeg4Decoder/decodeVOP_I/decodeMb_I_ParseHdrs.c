
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


//
//static int keyframes[2] = {0};
//

struct_VOLsimple	VOL;
readVOPOutData 		VOP;
uchar 				FrmData[BUFFER_SIZE - 4]; 		// 4 'uchar' is the length of the Start Code that has been suppressed.
REVERSE_EVENT 		DCT3D_I[4096]; 					//init_vlc_tables_I_PC_decod1_DCT3D_I
short          		InverseQuant_BlkXn [6 * 16];

decodeVOPOutData outputData;


//void decode_I_frame(const unsigned char *data,const struct_VOLsimple *VOL,const int position,struct_VOP *VOP,REVERSE_EVENT *DCT3D_I,int *pos_fin_vlc,int *address,unsigned char *Lum,unsigned char *Cb,unsigned char *Cr,int *keyframes);

void decodeMb_I_ParseHdrs(UINT32 inputFIFOIds[],
		 UINT32 inputFIFOAddrs[],
		 UINT32 outputFIFOIds[],
		 UINT32 outputFIFOAddrs[],
		 UINT32 params[]){

//
//	   int             i, j, k ;
//	    int             MB_courant = 0 ;
//	    int             MB_number ;
////	    int             pos_X [1];
////	    int             tab_pos_X [4];
//	    struct_VOP      new_VOP [1];
//	    int             VideoPacketHeader_pos [1];
//		int             VideoPacketHeader_resync_marker [1];
//
//		int             MacroblockI_pos [1];
//	    short           StockBlockLum_BuffA [16];
//	    short           StockBlockLum_BuffB [16];
//	    short           StockBlockLum_BuffC [16];
//	    short           StockBlockLum_BuffD [16];
//	    short           StockBlockLum_BuffE [16];
//	    short           StockBlocksCr_BuffA [16];
//	    short           StockBlocksCr_BuffB [16];
//	    short           StockBlocksCr_BuffC [16];
//	    short           StockBlocksCb_BuffA [16];
//	    short           StockBlocksCb_BuffB [16];
//	    short           StockBlocksCb_BuffC [16];
////	    int             VLCinverseXi_pos [1];
////	    int             VLCinverseXi_pos_prec [1];
////	    short           InverseQuant_BlkXn [6 * 16];
////	    unsigned char   block_8x8 [64];
////	    short    		*DCpred_buffA [6];
////	    short    		*DCpred_buffB [6];
////	    short    		*DCpred_buffC [6];
////	    unsigned char   *display [6];
////	    int             width ;
////	    const int       edge_size2 = EDGE_SIZE >> 1 ;
////	    int       		stride;
//	    int pos_o;
////	    uchar* FrmData;
////	    int frame_address;
//
////	    pos_o = VOP.VideoObjectPlane_pos;
//
//			if ( MB_courant == 342)
//				MB_courant += 0;
//			VideoPacketHeaderI(FrmData, pos_o, &VOL, &VOP.VideoObjectPlane_VOP, &MB_courant, &new_VOP,
//							&VideoPacketHeader_pos, &VideoPacketHeader_resync_marker, &MB_number);
//			MB_courant = MB_number ;
//			i = MB_courant % (VOL.video_object_layer_width / 16);
//			j = MB_courant / (VOL.video_object_layer_width / 16);
//			Param_MB_I(VideoPacketHeader_pos, FrmData, new_VOP, &VOP.VideoObjectPlane_VOP, &MacroblockI_pos);
//			StockBlocksLum(i, &VideoPacketHeader_resync_marker, InverseQuant_BlkXn + 1 * 16, InverseQuant_BlkXn + 2 * 16
//				, InverseQuant_BlkXn + 3 * 16, &VOL, StockBlockLum_BuffA, StockBlockLum_BuffB, StockBlockLum_BuffC
//				, StockBlockLum_BuffD, StockBlockLum_BuffE);
//			StockBlocksCb(MB_courant, InverseQuant_BlkXn + 4 * 16, &VOL, &VideoPacketHeader_resync_marker
//			, StockBlocksCb_BuffA, StockBlocksCb_BuffB, StockBlocksCb_BuffC);
//			StockBlocksCr(MB_courant, InverseQuant_BlkXn + 5 * 16, &VOL, &VideoPacketHeader_resync_marker
//			, StockBlocksCr_BuffA, StockBlocksCr_BuffB, StockBlocksCr_BuffC);
////			VLCinverseXi_pos_prec = MacroblockI_pos;
//
//			width = VOL.video_object_layer_width ;
//			pos_X = ((i + j * stride) << 4) + EDGE_SIZE + EDGE_SIZE * stride;
//
//
//	// Sending output data.
//	writeFifo(outputFIFOIds[0], outputFIFOAddrs[0], sizeof(decodeVOPOutData), (UINT8*)&outputData);
}
