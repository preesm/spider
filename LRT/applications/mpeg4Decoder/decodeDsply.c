
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
#include <hwQueues.h>


//--- VOL parameters --//
struct_VOLsimple VideoObjectLayer_VOLsimple;
uchar VideoObjectLayer_vop_complexity[5];
int VideoObjectLayer_pos_o;
int VideoObjectLayer_xsize_o;
int VideoObjectLayer_ysize_o;

//--- VOP parameters ---//
int VideoObjectPlane_vop_coding_type;
struct_VOP VideoObjectPlane_VOP;
int VideoObjectPlane_pos;

uchar mem_Y_last_buf[Y_RESOLUTION];
uchar mem_U_last_buf[U_RESOLUTION];
uchar mem_V_last_buf[V_RESOLUTION];

image_type Display_Extract_Image_Y_o[685832];

uchar stock_mb_type_P[1620];
vector save_mv[1620];

REVERSE_EVENT init_vlc_tables_P_PC_decod1_DCT3D_P[4096];
REVERSE_EVENT init_vlc_tables_I_PC_decod1_DCT3D_I[4096];


void decodeDsply(UINT32 inputFIFOIds[],
		 UINT32 inputFIFOAddrs[],
		 UINT32 outputFIFOIds[],
		 UINT32 outputFIFOAddrs[],
		 UINT32 params[]){



	init_vlc_tables_P(init_vlc_tables_P_PC_decod1_DCT3D_P);
	init_vlc_tables_I(init_vlc_tables_I_PC_decod1_DCT3D_I);

	FILE* pFile = NULL;
	pFile = fopen(M4V_FILE_PATH, "rb");
	if (pFile == NULL)
	{
	  printf("Cannot open m4v_file file '%s' \n", M4V_FILE_PATH);
	  exit(-1);
	}

	int ii;
	for (ii = 0; ii < 3; ii++){
		uchar buffer[BUFFER_SIZE];
		uint nbBytesRead = 0;

		uint maxPlaneSize = 0;

		// Reading Visual Object (I don't know why but there is no VOS).
		readUpToNextStartCode(pFile, buffer, &nbBytesRead);

		// Reading Visual Object Layer.
		readUpToNextStartCode(pFile, buffer, &nbBytesRead);
		VideoObjectLayer(
				BUFFER_START_POSITION,
				nbBytesRead,
				buffer,
				&VideoObjectLayer_VOLsimple,
				VideoObjectLayer_vop_complexity,
				&VideoObjectLayer_pos_o,
				&VideoObjectLayer_xsize_o,
				&VideoObjectLayer_ysize_o);

		int keyframes[2] = {0};

		while(!feof(pFile))
		{
			// Reading Video Plane Object.
			readUpToNextStartCode(pFile, buffer, &nbBytesRead); if(nbBytesRead > maxPlaneSize) maxPlaneSize = nbBytesRead; // TODO: remove this
			uchar startCodeValue = buffer[3];
			if(nbBytesRead < 4){
				printf("%x\n", startCodeValue);
				break;
			}
			VideoObjectPlaneI(
					BUFFER_START_POSITION,
					&buffer[4],						// Skipping the start code.
					&VideoObjectLayer_VOLsimple,
					VideoObjectLayer_vop_complexity,
					&VideoObjectPlane_pos,
					&VideoObjectPlane_VOP,
					&VideoObjectPlane_vop_coding_type);

			int pos_fin_vlc;
			int frame_address;
			/* switch VOP coding type */
			switch (VideoObjectPlane_vop_coding_type) {

			  case 0 : {
				decode_I_frame(
						&buffer[4],
						&VideoObjectLayer_VOLsimple,
						VideoObjectPlane_pos,
						&VideoObjectPlane_VOP,
						init_vlc_tables_I_PC_decod1_DCT3D_I,
						&pos_fin_vlc,
						&frame_address,
						mem_Y_last_buf,
						mem_U_last_buf,
						mem_V_last_buf,
						keyframes);

				break; }

			  case 1 : {
				decode_P_frame(
						&buffer[4],
						&VideoObjectLayer_VOLsimple,
						VideoObjectPlane_pos,
						&VideoObjectPlane_VOP,
						init_vlc_tables_I_PC_decod1_DCT3D_I,
						init_vlc_tables_P_PC_decod1_DCT3D_P,
						stock_mb_type_P,
						&pos_fin_vlc,
						&frame_address,
						mem_Y_last_buf,
						mem_U_last_buf,
						mem_V_last_buf,
						keyframes,
						save_mv);

				break; }
			}

			int Zero_PC_decod1_out = 0;
			int XDIM = ((int *) Display_Extract_Image_Y_o)[0] = VideoObjectLayer_xsize_o;
			int YDIM = ((int *) Display_Extract_Image_Y_o)[1] = VideoObjectLayer_ysize_o;
			uchar *Y = Display_Extract_Image_Y_o + 8;
			uchar *U = Y + (XDIM + 32) * YDIM;
			uchar *V = U + (XDIM + 32) * YDIM/4;
			extract_picture(
				   XDIM, YDIM,
				   16, Zero_PC_decod1_out,
				   mem_Y_last_buf, mem_U_last_buf, mem_V_last_buf, frame_address,
				   Y, U, V);
			SDL_Display(16, XDIM, YDIM, Y, U, V);
		}
		rewind(pFile);
	}

	fclose(pFile);
}


