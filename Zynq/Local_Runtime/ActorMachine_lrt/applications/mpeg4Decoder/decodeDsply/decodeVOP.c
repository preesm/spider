
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
#include "lrt_1W1RfifoMngr.h"
#include "lrt_core.h"
#include "hwQueues.h"
#include "lrt_taskMngr.h"

//static uchar VOPCounter = 0;

REVERSE_EVENT init_vlc_tables_P_PC_decod1_DCT3D_P[4096];
REVERSE_EVENT init_vlc_tables_I_PC_decod1_DCT3D_I[4096];

//static FILE* pFile = NULL;
uchar FrmData[BUFFER_SIZE];

struct_VOLsimple VOL;
//static long filePosition = 28;
readVOPOutData VOP;

uchar stock_mb_type_P[1620];
vector save_mv[1620];
int pos_fin_vlc;
static int keyframes[2] = {0};

decodeVOPOutData Img;
decodeVOPOutData PrevImg;

UINT32 IVOPCounter = 0;
UINT32 PVOPCounter = 0;

void decodeVOP(UINT32 inputFIFOIds[],
		 UINT32 inputFIFOAddrs[],
		 UINT32 outputFIFOIds[],
		 UINT32 outputFIFOAddrs[],
		 UINT32 params[]){

	// Initializations...
	init_vlc_tables_P(init_vlc_tables_P_PC_decod1_DCT3D_P);
	init_vlc_tables_I(init_vlc_tables_I_PC_decod1_DCT3D_I);

	// Receiving data.
	readFifo(inputFIFOIds[0], inputFIFOAddrs[0], sizeof(struct_VOLsimple), (UINT8*)&VOL);
	readFifo(inputFIFOIds[1], inputFIFOAddrs[1], sizeof(decodeVOPInData), (UINT8*)&VOP);
	readFifo(inputFIFOIds[2], inputFIFOAddrs[2], BUFFER_SIZE, (UINT8*)&FrmData);
	readFifo(inputFIFOIds[3], inputFIFOAddrs[3], sizeof(decodeVOPOutData), (UINT8*)&PrevImg);


	/* switch on VOP coding type */
	switch (0) {

	  case 0 : {
		  IVOPCounter++;
		decode_I_frame(
				&FrmData[4], // Skipping the start code.
				&VOL,
				VOP.VideoObjectPlane_pos,
				&VOP.VideoObjectPlane_VOP,
				init_vlc_tables_I_PC_decod1_DCT3D_I,
				&pos_fin_vlc,
				&Img.frame_address,
				Img.mem_Y_last_buf,
				Img.mem_U_last_buf,
				Img.mem_V_last_buf,
				keyframes);

		break; }

	  case 1 : {
		  PVOPCounter++;
		decode_P_frame(
				&FrmData[4], // Skipping the start code.
				&VOL,
				VOP.VideoObjectPlane_pos,
				&VOP.VideoObjectPlane_VOP,
				init_vlc_tables_I_PC_decod1_DCT3D_I,
				init_vlc_tables_P_PC_decod1_DCT3D_P,
				stock_mb_type_P,
				&pos_fin_vlc,
				&PrevImg.frame_address,
				PrevImg.mem_Y_last_buf,
				PrevImg.mem_U_last_buf,
				PrevImg.mem_V_last_buf,
				keyframes,
				save_mv);

		break; }
	}

	// Sending data.
	writeFifo(outputFIFOIds[0], outputFIFOAddrs[0], sizeof(decodeVOPOutData), (UINT8*)&Img);
	writeFifo(outputFIFOIds[1], outputFIFOAddrs[1], sizeof(decodeVOPOutData), (UINT8*)&Img);
}
