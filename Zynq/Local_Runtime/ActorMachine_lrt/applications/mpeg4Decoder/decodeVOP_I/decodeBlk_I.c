
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


/*
 * Inputs
 */
/*!
   intra-block processing.

	\param[in] k	 				: block number in macroblock
	\param[in] data 				: frame data
	\param[in] VOP 					: input VOP
	\param[in] pos_i		 		: position in bitstream
	\param[in] DCT3D_I 				: inverse VLC intra coefficients
	\param[in] VOLsimple			: input VOL
	\param[in] MB_pos				: current macroblock number
	\param[in] Blk_A				: neighboring block 'A'
	\param[in] Blk_B				: neighboring block 'B'
	\param[in] Blk_C				: neighboring block 'C'
	\param[out] InverseQuant_Blk_X	: block inverse quantization
	\param[out] block_sat_X			: processed output block
	\param[out] pos_fin_vlc			: position in bitstream after block processing
   */
struct_VOLsimple 	VOLsimple;
readVOPOutData 		VOP;
uchar 				data[BUFFER_SIZE - 4]; 		// 4 'uchar' is the length of the Start Code that has been suppressed.
int 				MB_pos;
int 				k;
int 				pos_i;
short 				Blk_A;
short 				Blk_B;
short 				Blk_C;

//static REVERSE_EVENT 		*DCT3D_I;

/*
 * Outputs
 */
int 				outPos;


REVERSE_EVENT 		DCT3D_I[4096]; 					//init_vlc_tables_I_PC_decod1_DCT3D_I
short          		InverseQuant_BlkXn [6 * 16];


void decodeBlk_I(UINT32 inputFIFOIds[],
		 UINT32 inputFIFOAddrs[],
		 UINT32 outputFIFOIds[],
		 UINT32 outputFIFOAddrs[],
		 UINT32 params[]){

	readFifo(inputFIFOIds[0], inputFIFOAddrs[0], sizeof(struct_VOLsimple), (UINT8*)&VOLsimple);
	readFifo(inputFIFOIds[1], inputFIFOAddrs[1], sizeof(readVOPOutData), (UINT8*)&VOP);
	readFifo(inputFIFOIds[2], inputFIFOAddrs[2], sizeof(data), (UINT8*)&data);
	readFifo(inputFIFOIds[3], inputFIFOAddrs[3], sizeof(int), (UINT8*)&MB_pos);
	readFifo(inputFIFOIds[4], inputFIFOAddrs[4], sizeof(int), (UINT8*)&k);
	readFifo(inputFIFOIds[5], inputFIFOAddrs[5], sizeof(int), (UINT8*)&pos_i);



//    int     DCpred_prediction_direction [1];
//    short   DCpred_F00pred [1];
//    short   DCpred_QPpred [1];
//    short   DCpred_QFpred [7];
//    int     InverseACDCpred_dc_scaler [1];

//    VLCinverseI(5 - k, MB_pos, pos_i, data, DCT3D_I, Blk_A, Blk_B, Blk_C, &VOP.VideoObjectPlane_VOP, pos_fin_vlc, InvDCT_f, DCpred_QFpred
//        , DCpred_F00pred, DCpred_QPpred, DCpred_prediction_direction);
//    InverseACDCpred(k, InvDCT_f, DCpred_QFpred, DCpred_F00pred [0], DCpred_QPpred [0], &VOP.VideoObjectPlane_VOP
//        , DCpred_prediction_direction [0], iDCT_data, InverseACDCpred_dc_scaler);
//    InverseQuantI(iDCT_data, &VOP.VideoObjectPlane_VOP, InverseACDCpred_dc_scaler [0], VOLsimple, InvDCT_f, InverseQuant_Blk_X);
//    InverseDCT_optim(InvDCT_f);
//    block_sat(InvDCT_f, block_sat_X);

}
