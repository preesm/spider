/**
 * *****************************************************************************
 * Copyright or © or Copr. IETR/INSA: Maxime Pelcat, Jean-François Nezan,
 * Karol Desnos, Julien Heulot, Clément Guy, Yaset Oliva Venegas
 *
 * [mpelcat,jnezan,kdesnos,jheulot,cguy,yoliva]@insa-rennes.fr
 *
 * This software is a computer program whose purpose is to prototype
 * parallel applications.
 *
 * This software is governed by the CeCILL-C license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL-C
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-C license and that you accept its terms.
 * ****************************************************************************
 */

#include <spider/spider.h>
#include "stereo.h"

#include "actors.h"

void stereo_Config(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void stereo_Camera(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void stereo_RGB2Gray_L(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void stereo_RGB2Gray_R(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void stereo_Census_L(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void stereo_Census_R(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void stereo_Split(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void stereo_MedianFilter(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void stereo_Display(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void costParallel_GenIx(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void costParallel_GenDisp(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void costParallel_VWeights(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void costParallel_HWeight(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void costParallel_CostConstruction(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void DispComp_AggregateCost(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void DispComp_DisparitySelect(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);

lrtFct stereo_fcts[N_FCT_STEREO] = {
	&stereo_Config,
	&stereo_Camera,
	&stereo_RGB2Gray_L,
	&stereo_RGB2Gray_R,
	&stereo_Census_L,
	&stereo_Census_R,
	&stereo_Split,
	&stereo_MedianFilter,
	&stereo_Display,
	&costParallel_GenIx,
	&costParallel_GenDisp,
	&costParallel_VWeights,
	&costParallel_HWeight,
	&costParallel_CostConstruction,
	&DispComp_AggregateCost,
	&DispComp_DisparitySelect,
};

void stereo_Config(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Config(
		/* nSlice     */ (Param*) &outParams[3],
		/* truncValue */ (Param*) &outParams[4],
		/* scale      */ (Param*) &outParams[0],
		/* nIter      */ (Param*) &outParams[8],
		/* sizeFilter */ (Param*) &outParams[7],
		/* minDisp    */ (Param*) &outParams[2],
		/* maxDisp    */ (Param*) &outParams[1],
		/* height     */ (Param*) &outParams[5],
		/* width      */ (Param*) &outParams[6]
	);
}

void stereo_Camera(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Camera(
		/* height */ (Param) inParams[1],
		/* width  */ (Param) inParams[0],
		/* rgb_L  */ (uint8_t*) outputFIFOs[0],
		/* rgb_R  */ (uint8_t*) outputFIFOs[1]
	);
}

void stereo_RGB2Gray_L(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	RGB2Gray(
		/* size */ (Param) inParams[0],
		/* rgb  */ (uint8_t*) inputFIFOs[0],
		/* gray */ (uint8_t*) outputFIFOs[0]
	);
}

void stereo_RGB2Gray_R(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	RGB2Gray(
		/* size */ (Param) inParams[0],
		/* rgb  */ (uint8_t*) inputFIFOs[0],
		/* gray */ (uint8_t*) outputFIFOs[0]
	);
}

void stereo_Census_L(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Census(
		/* height */ (Param) inParams[1],
		/* width  */ (Param) inParams[0],
		/* gray   */ (uint8_t*) inputFIFOs[0],
		/* cen    */ (uint8_t*) outputFIFOs[0]
	);
}

void stereo_Census_R(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Census(
		/* height */ (Param) inParams[1],
		/* width  */ (Param) inParams[0],
		/* gray   */ (uint8_t*) inputFIFOs[0],
		/* cen    */ (uint8_t*) outputFIFOs[0]
	);
}

void stereo_Split(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Split(
		/* nSlice     */ (Param) inParams[2],
		/* sizeFilter */ (Param) inParams[3],
		/* height     */ (Param) inParams[1],
		/* width      */ (Param) inParams[0],
		/* in         */ (uint8_t*) inputFIFOs[0],
		/* out        */ (uint8_t*) outputFIFOs[0]
	);
}

void stereo_MedianFilter(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	MedianFilter(
		/* height     */ (Param) inParams[2],
		/* width      */ (Param) inParams[0],
		/* sizeFilter */ (Param) inParams[1],
		/* in         */ (uint8_t*) inputFIFOs[0],
		/* out        */ (uint8_t*) outputFIFOs[0]
	);
}

void stereo_Display(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Display(
		/* height */ (Param) inParams[1],
		/* width  */ (Param) inParams[0],
		/* rgb    */ (uint8_t*) inputFIFOs[1],
		/* depth  */ (uint8_t*) inputFIFOs[0]
	);
}

void costParallel_GenIx(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	GenIx(
		/* nIter */ (Param) inParams[0],
		/* ixs   */ (uint8_t*) outputFIFOs[0]
	);
}

void costParallel_GenDisp(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	DisparityGen(
		/* maxDisp */ (Param) inParams[1],
		/* minDisp */ (Param) inParams[0],
		/* dispIxs */ (uint8_t*) outputFIFOs[0]
	);
}

void costParallel_VWeights(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	VWeights(
		/* height */ (Param) inParams[0],
		/* width  */ (Param) inParams[1],
		/* offset */ (uint8_t*) inputFIFOs[0],
		/* rgbL   */ (uint8_t*) inputFIFOs[1],
		/* out    */ (uint8_t*) outputFIFOs[0]
	);
}

void costParallel_HWeight(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	HWeights(
		/* height */ (Param) inParams[0],
		/* width  */ (Param) inParams[1],
		/* offset */ (uint8_t*) inputFIFOs[0],
		/* rgbL   */ (uint8_t*) inputFIFOs[1],
		/* out    */ (uint8_t*) outputFIFOs[0]
	);
}

void costParallel_CostConstruction(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	CostConstruction(
		/* height     */ (Param) inParams[1],
		/* width      */ (Param) inParams[2],
		/* truncValue */ (Param) inParams[0],
		/* grayL      */ (uint8_t*) inputFIFOs[3],
		/* grayR      */ (uint8_t*) inputFIFOs[4],
		/* cenL       */ (uint8_t*) inputFIFOs[1],
		/* cenR       */ (uint8_t*) inputFIFOs[2],
		/* disp       */ (uint8_t*) inputFIFOs[0],
		/* error      */ (uint8_t*) outputFIFOs[0]
	);
}

void DispComp_AggregateCost(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	AggregateCost(
		/* height   */ (Param) inParams[0],
		/* width    */ (Param) inParams[1],
		/* nIter    */ (Param) inParams[2],
		/* offsets  */ (uint8_t*) inputFIFOs[2],
		/* vWeights */ (uint8_t*) inputFIFOs[3],
		/* hWeights */ (uint8_t*) inputFIFOs[1],
		/* cost     */ (uint8_t*) inputFIFOs[0],
		/* disp     */ (uint8_t*) outputFIFOs[0]
	);
}

void DispComp_DisparitySelect(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	DisparitySelect(
		/* height  */ (Param) inParams[0],
		/* width   */ (Param) inParams[1],
		/* scale   */ (Param) inParams[2],
		/* minDisp */ (Param) inParams[3],
		/* maxDisp */ (Param) inParams[4],
		/* dispVal */ (uint8_t*) inputFIFOs[0],
		/* dispIx  */ (uint8_t*) inputFIFOs[1],
		/* curDisp */ (uint8_t*) inputFIFOs[2],
		/* curCost */ (uint8_t*) inputFIFOs[3],
		/* disp    */ (uint8_t*) outputFIFOs[0],
		/* cost    */ (uint8_t*) outputFIFOs[1]
	);
}

