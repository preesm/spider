/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

#include "stereo.h"

extern "C"{
#include "include/aggregateCost.h"
#include "include/census.h"
#include "include/clock.h"
#include "include/communication.h"
#include "include/computeWeights.h"
#include "include/costConstruction.h"
#include "include/disparityGen.h"
#include "include/disparitySelect.h"
#include "include/displayRGB.h"
#include "include/medianFilter.h"
#include "include/offsetGen.h"
#include "include/ppm.h"
#include "include/rgb2Gray.h"
#include "include/sink.h"
#include "include/splitMerge.h"
#include "include/yuv2RGB.h"
#include "include/yuvRead.h"
}

#include <platform_types.h>

void readPPM(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int height = params[0];
	int width = params[1];
	int id = params[2];

	/* Inputs */

	/* Outputs */
	UINT8* rgb = outputFIFOs[0];

	/* Fct Call */
	readPPM(id, height, width, rgb);
}

void rgb2gray(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int size = params[0];

	/* Inputs */
	UINT8* in = inputFIFOs[0];

	/* Outputs */
	UINT8* out = outputFIFOs[0];

	/* Fct Call */
	rgb2Gray(size, in, out);
}

void census(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];

	/* Inputs */
	UINT8* in = inputFIFOs[0];

	/* Outputs */
	UINT8* out = outputFIFOs[0];

	/* Fct Call */
	census(height, width, in, out);
}

void split(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int overlap = params[0];
	int nbSlice = params[1];
	int width = params[2];
	int height = params[3];

	/* Inputs */
	UINT8* in = inputFIFOs[0];

	/* Outputs */
	UINT8* out = outputFIFOs[0];

	/* Fct Call */
	split(nbSlice, width, height, in, out);
}

void median(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int height = params[0];
	int width = params[1];
	int nbSlice = params[2];
	int overlap = params[3];

	/* Inputs */
	UINT8* in = inputFIFOs[0];

	/* Outputs */
	UINT8* out = outputFIFOs[0];

	/* Fct Call */
	medianFilter(height/nbSlice+2*overlap, width, overlap, in, out);
}

void writePPM(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];

	/* Inputs */
	UINT8* in = inputFIFOs[0];

	/* Outputs */

	/* Fct Call */
	writePPM(height, width, in);
}

void offsetGen(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int nbIter = params[0];

	/* Inputs */

	/* Outputs */
	UINT8* offsets = outputFIFOs[0];

	/* Fct Call */
	offsetGen(nbIter, offsets);
}

void dispGen(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int minDisp = params[0];
	int maxDisp = params[1];

	/* Inputs */

	/* Outputs */
	UINT8* disps= outputFIFOs[0];

	/* Fct Call */
	disparityGen(minDisp, maxDisp, disps);
}

void compWeight(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int horOrVert = params[0];
	int width = params[1];
	int height = params[2];

	/* Inputs */
	UINT8* offset= inputFIFOs[0];
	UINT8* rgbL= inputFIFOs[1];

	/* Outputs */
	UINT8* weights= outputFIFOs[0];

	/* Fct Call */
	computeWeights(height, width, horOrVert, offset, rgbL, (float*)weights);
}

void costConstr(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int truncValue = params[0];
	int width = params[1];
	int height = params[2];

	/* Inputs */
	UINT8* grayL= inputFIFOs[0];
	UINT8* grayR= inputFIFOs[1];
	UINT8* cenL= inputFIFOs[2];
	UINT8* cenR= inputFIFOs[3];
	UINT8* disp= inputFIFOs[4];

	/* Outputs */
	UINT8* dispError= outputFIFOs[0];

	/* Fct Call */
	costConstruction(height, width, truncValue, disp, grayL, grayR, cenL, cenR, (float*) dispError);
}

void aggregateCost(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];
	int nbIter = params[2];

	/* Inputs */
	UINT8* offsets= inputFIFOs[0];
	UINT8* vWeights= inputFIFOs[1];
	UINT8* hWeights= inputFIFOs[2];
	UINT8* dispError= inputFIFOs[3];

	/* Outputs */
	UINT8* aggDisp= outputFIFOs[0];

	/* Fct Call */
	aggregateCost(height, width, nbIter, (float*)dispError, offsets, (float*)hWeights, (float*)vWeights, (float*)aggDisp);
}

void dispSelect(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int scale = params[0];
	int width = params[1];
	int height = params[2];
	int minDisp = params[3];
	int nbDisp = params[4];

	/* Inputs */
	UINT8* aggDisp= inputFIFOs[0];
	UINT8* disp= inputFIFOs[1];
	UINT8* currentRes= inputFIFOs[2];
	UINT8* bestCost= inputFIFOs[3];

	/* Outputs */
	UINT8* result= outputFIFOs[0];
	UINT8* backBestCost= outputFIFOs[1];

	/* Fct Call */
	disparitySelect(height, width, nbDisp, scale, minDisp, disp, (float*)aggDisp, (float*)bestCost, currentRes, result, (float*)backBestCost);
}

