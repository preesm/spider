
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

#ifndef _DECODER_TYPES_H_
#define _DECODER_TYPES_H_

#include <mpeg.h>
#include "myMpeg4Sources/types.h"


#define M4V_FILE_PATH		"C:\\work\\COMPA\\decodeur_mp4\\Sequences\\SIMPLE\\P-VOP\\jvc011.m4v"

// Frame dimensions assuming 4:2:0 sampling.
#define X_DIM			1786
#define Y_DIM			768
//#define X_DIM			352
//#define Y_DIM			288

#define Y_RESOLUTION	X_DIM * Y_DIM
#define U_RESOLUTION	X_DIM * Y_DIM / 4
#define V_RESOLUTION	U_RESOLUTION
#define IMG_SIZE		Y_RESOLUTION + U_RESOLUTION + V_RESOLUTION

#define Y_BUFFER_SIZE	Y_RESOLUTION
#define U_BUFFER_SIZE	U_RESOLUTION
#define V_BUFFER_SIZE	V_RESOLUTION

// Compressed frame size (empirical approximation).
#define I_PLANE_SIZE	4096
#define BUFFER_SIZE		I_PLANE_SIZE
//#define BUFFER_SIZE		414720

#define BUFFER_START_POSITION	0

typedef struct imgDimensionsData{
	int VideoObjectLayer_xsize_o;
	int VideoObjectLayer_ysize_o;
}imgDimensionsData;

//
//typedef struct readVOLOutData{
//	long filePosition;
//	struct_VOLsimple VideoObjectLayer_VOLsimple;
//	uchar VideoObjectLayer_vop_complexity[5];
//}readVOLOutData;


//typedef readVOLOutData readVOPInData;


typedef struct readVOPOutData{
	int VideoObjectPlane_pos;
	int VideoObjectPlane_vop_coding_type;
	struct_VOP VideoObjectPlane_VOP;
}readVOPOutData;


typedef readVOPOutData decodeVOPInData;


typedef struct decodeVOPOutData{
	uchar mem_Y_last_buf[Y_BUFFER_SIZE];
	uchar mem_U_last_buf[U_BUFFER_SIZE];
	uchar mem_V_last_buf[V_BUFFER_SIZE];
	int frame_address;
}decodeVOPOutData;


typedef decodeVOPOutData displayVOPInData;

#endif /* _DECODER_TYPES_H_ */
