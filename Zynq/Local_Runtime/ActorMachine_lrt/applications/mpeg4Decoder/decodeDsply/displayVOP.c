
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


imgDimensionsData ImgDim;
decodeVOPOutData Img;

image_type Display_Extract_Image_Y_o[685832];
int Zero_PC_decod1_out = 0;

void displayVOP(UINT32 inputFIFOIds[],
		 UINT32 inputFIFOAddrs[],
		 UINT32 outputFIFOIds[],
		 UINT32 outputFIFOAddrs[],
		 UINT32 params[])
{
	int XDIM;
	int YDIM;
	uchar *Y;
	uchar *U;
	uchar *V;

	readFifo(inputFIFOIds[0], inputFIFOAddrs[0], sizeof(decodeVOPOutData), (UINT8*)&Img); // From decode VOP.
	readFifo(inputFIFOIds[1], inputFIFOAddrs[1], sizeof(imgDimensionsData), (UINT8*)&ImgDim); // From input port.

	XDIM = ((int *) Display_Extract_Image_Y_o)[0] = ImgDim.VideoObjectLayer_xsize_o;
	YDIM = ((int *) Display_Extract_Image_Y_o)[1] = ImgDim.VideoObjectLayer_ysize_o;
	Y = Display_Extract_Image_Y_o + 8;
	U = Y + (XDIM + 32) * YDIM;
	V = U + (XDIM + 32) * YDIM/4;
	extract_picture(
		   XDIM, YDIM,
		   16, Zero_PC_decod1_out,
		   Img.mem_Y_last_buf, Img.mem_U_last_buf, Img.mem_V_last_buf, Img.frame_address,
		   Y, U, V);
	SDL_Display(16, XDIM, YDIM, Y, U, V);
}
