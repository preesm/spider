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
#include "src/compute_stereo.h"
#include "src/stereomatch.h"


#include <stdio.h>
#define PRINT 1

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <platform_types.h>
#include <platform_queue.h>
#include <lrt_definitions.h>

void stereoMono(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];
	int nbDisp = params[2];
	int nbIter = params[3];
	int nbSlices = params[4];

	/* Inputs */
	UINT8* Lr = inputFIFOs[0];
	UINT8* Lg = inputFIFOs[1];
	UINT8* Lb = inputFIFOs[2];
	UINT8* Rr = inputFIFOs[3];
	UINT8* Rg = inputFIFOs[4];
	UINT8* Rb = inputFIFOs[5];

	/* Outputs */
	UINT8* out = outputFIFOs[0];

#if PRINT
	printf("stereoMono %d %d %d %d\n", width, height, nbDisp, nbIter);
#endif

	/* Fct */
	struct str_image_rgb imcl, imcr;
	struct str_image_gray disp;

	image_rgb_init(&imcl, width, height);
	image_rgb_init(&imcr, width, height);
	image_gray_init(&disp, width, height);

	memcpy(imcl.r, Lr, width*height);
	memcpy(imcl.g, Lg, width*height);
	memcpy(imcl.b, Lb, width*height);

	memcpy(imcr.r, Rr, width*height);
	memcpy(imcr.g, Rg, width*height);
	memcpy(imcr.b, Rb, width*height);

	stereoMatch(&imcl, &imcr, nbDisp, &disp);

	memcpy(out, disp.g, width*height);
}

void rgb2Gray(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];

	/* Inputs */
	UINT8* r = inputFIFOs[0];
	UINT8* g = inputFIFOs[1];
	UINT8* b = inputFIFOs[2];

	/* Outputs */
	UINT8* gray = outputFIFOs[0];

#if PRINT
	printf("rgb2gray %d %d\n", width, height);
#endif
	int i;

	for(i=0; i<width*height; i++){
		gray[i] = ((int)r[i] + (int)g[i] + (int)b[i])/3;
	}
}

void census(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];

	/* Inputs */
	UINT8* gray = inputFIFOs[0];

	/* Outputs */
	UINT8* cens = outputFIFOs[0];

#if PRINT
	printf("census %d %d\n", width, height);
#endif

	calcCensus(height, width, gray, cens);
}

void genDelta(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int nbIter = params[0];

	/* Inputs */

	/* Outputs */
	UINT8* out_deltas = outputFIFOs[0];


#if PRINT
	printf("genDelta %d\n", nbIter);
#endif
	int i;

	for(i=0; i<nbIter; i++)
		out_deltas[i] = ((i+1)*(i+1))%33;
}

void compWeight(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];
	int horV = params[2];

	/* Inputs */
	UINT8* r = inputFIFOs[0];
	UINT8* g = inputFIFOs[1];
	UINT8* b = inputFIFOs[2];
	UINT8* delta = inputFIFOs[3];

	/* Outputs */
	UINT8* out = outputFIFOs[0];

#if PRINT
	printf("compWeight %d %d %d del%d\n", width,  height, horV, *delta);
#endif

	if(horV == 0){
		compute_hweigth(height, width, *delta, r, g, b, out);
	}else{
		compute_vweigth(height, width, *delta, r, g, b, out);
	}
}

void null(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	printf("Null fct executed !\n");
}

void config(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	static int it=0;

	int nbIter = 6-it;
	int nbDisp = 10+10*it;
	it = (it+1)%6;

//	unsigned int outParams[2] = {nbDisp, nbIter};

	unsigned int outParams[2] = {60, 6};

//	pushParam(curVertexId,2,outParams);

	platform_queue_push_UINT32(MSG_PARAM_VALUE);
	platform_queue_push_UINT32(OSTCBCur->srdagId);
	platform_queue_push_UINT32(60);
	platform_queue_push_UINT32(6);
	platform_queue_push_finalize();

}

void disp(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];
	int nbDisp = params[2];
	int nbIter = params[3];

	/* Inputs */
	UINT8* Lg = inputFIFOs[0];
	UINT8* Lcen = inputFIFOs[1];
	UINT8* Rg = inputFIFOs[2];
	UINT8* Rcen = inputFIFOs[3];
	UINT8* hw = inputFIFOs[4];
	UINT8* vw = inputFIFOs[5];
	UINT8* deltas = inputFIFOs[6];

	/* Outputs */
	UINT8* out = outputFIFOs[0];


#if PRINT
	printf("disp %d %d %d %d\n", width,  height, nbDisp, nbIter);
#endif

	unsigned char cost1[450*375];
	unsigned char cost2[450*375];
	unsigned char buffval[450*375];

	int d, i;

	for(d=0; d<nbDisp; d++){
		/*cost construction*/
		cost_construction(
				height, width, d,
				Rg, Lg, Rcen, Lcen,
				cost1);

		/*recursive aggregation*/
		for(i=0; i<nbIter; i++)
		{
			aggregateV(height, width, deltas[i], cost1, vw+i*height*width, cost2);
			aggregateH(height, width, deltas[i], cost2, hw+i*height*width, cost1);
		}

		/*disparity is argmin of cost*/
		if(d == 0)
		{
			/*first iteration*/
			memset(out, 0, height*width*sizeof(uint8_t));
			memcpy(buffval, cost1, height*width*sizeof(uint8_t));
		}
		else
		{
			/*select disparity to minimize cost*/
			for(i=0; i<height*width; i++)
			{
				/*buffval contains the current minimum cost*/
				if(cost1[i] < buffval[i])
				{
					buffval[i] = cost1[i];	/*buffval = min(cost)*/
					out[i] = d;				/*out = argmin(cost)*/
				}
			}
		}
	}
}

void costConst(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];

	/* Inputs */
	UINT8* Lg = inputFIFOs[0];
	UINT8* Lcen = inputFIFOs[1];
	UINT8* Rg = inputFIFOs[2];
	UINT8* Rcen = inputFIFOs[3];
	UINT8* disp = inputFIFOs[4];

	/* Outputs */
	UINT8* out = outputFIFOs[0];


#if PRINT
	printf("costConst %d %d\n", width,  height);
#endif
	cost_construction(height, width, *disp,
				Rg, Lg, Rcen, Lcen,
				out);
}

void aggregate(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];
	int nbIter = params[2];

	/* Inputs */
	UINT8* vweight = inputFIFOs[0];
	UINT8* hweight = inputFIFOs[1];
	UINT8* deltas = inputFIFOs[2];
	UINT8* input = inputFIFOs[3];

	/* Outputs */
	UINT8* out = outputFIFOs[0];


#if PRINT
	printf("aggregate %d %d %d\n", width,  height, nbIter);
#endif

	static unsigned char tmp[450*375];
	int i;

	memcpy(out, input, width*height);
	/*recursive aggregation*/
	for(i=0; i<nbIter; i++){
		aggregateV(height, width, deltas[i], out, vweight+i*height*width, tmp);
		aggregateH(height, width, deltas[i], tmp, hweight+i*height*width, out);
	}
}

void select(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];

	/* Inputs */
	UINT8* val_in = inputFIFOs[0];
	UINT8* disp_in = inputFIFOs[1];
	UINT8* input = inputFIFOs[2];
	UINT8* disp = inputFIFOs[3];

	/* Outputs */
	UINT8* val_out = outputFIFOs[0];
	UINT8* disp_out = outputFIFOs[1];

	int i;

#if PRINT
	printf("select %d %d\n", width,  height);
#endif
	/* disparity is argmin of cost */
	if(*disp == 0){
		/* first iteration */
		memset(disp_out, 0, height*width);
		memcpy(val_out, input, height*width);
	}else{
		/* select disparity to minimize cost */
		for(i=0; i<height*width; i++){
			/* buffval contains the current minimum cost */
			if(input[i] < val_in[i]){
				val_out[i] = input[i];	/*buffval = min(cost)*/
				disp_out[i] = *disp;				/*out = argmin(cost)*/
			}else{
				val_out[i] = val_in[i];	/*buffval = min(cost)*/
				disp_out[i] = disp_in[i];
			}
		}
	}
}

void genDisp(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int nbDisp = params[0];

	/* Inputs */

	/* Outputs */
	UINT8* out = outputFIFOs[0];

	int i;

#if PRINT
	printf("genDisp %d\n", nbDisp);
#endif
	for(i=0; i<nbDisp; i++){
		out[i] = i;
	}
}

static inline void swap(unsigned char *a, unsigned char *b){
	unsigned char buf = *a;
	*a=*b;
	*b=buf;
}

static void quickSortPartition(int startIdx, int endIdx, int *pivotIdx, unsigned char *values){
	int idx;
	int swapIdx = startIdx;
	swap(values+*pivotIdx,values+endIdx);
	for(idx = startIdx; idx < endIdx; idx++){
		if(values[idx]<=values[endIdx]){
			swap(values+swapIdx,values+idx);
			swapIdx++;
		}
	}
	swap(values+swapIdx, values+endIdx);
	*pivotIdx = swapIdx;
}


static void quickSort(int startIdx, int endIdx, unsigned char *values){
	if(startIdx<endIdx){
		int pivotIdx = startIdx;
		quickSortPartition(startIdx, endIdx, &pivotIdx, values);
		quickSort(startIdx,pivotIdx-1,values);
		quickSort(pivotIdx+1,endIdx,values);
	}
}

void medianSlice(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];
	int nbSlices = params[2];

	int subHeight = height/nbSlices+2;

	/* Inputs */
	UINT8* in = inputFIFOs[0];

	/* Outputs */
	UINT8* out = outputFIFOs[0];


#if PRINT
	printf("median %d %d %d\n", width, height, nbSlices);
#endif

	int i,j,k,l,y,x;
	// Process pixels one by one
	for(j=0; j<height/nbSlices; j++){
		out[j*width] = 0;
		for(i=1;i<width-1;i++){
			unsigned char pixels[9];
			// output pixel is the median of a 3x3 window
			// Get the 9 pixels
			int k=0;
			for(y=j-1;y<=j+1;y++){
				for(x=i-1;x<=i+1;x++){
					pixels[k++] = in[y*width+x];
				}
			}

			// Sort the 9 values
			quickSort(0, 8, pixels);
			out[j*width+i] = pixels[9/2];
		}
		out[j*width+width-1] = 0;
	}
}

void split(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];
	int nbSlices = params[2];

	int subHeight = height/nbSlices+2;
	int sliceSize = subHeight*width;

	/* Inputs */
	UINT8* in = inputFIFOs[0];

	/* Outputs */
	UINT8* out = outputFIFOs[0];


#if PRINT
	printf("split %d %d %d\n", width, height, nbSlices);
#endif

	int i;

	// Fill first and last line with 0
	memset(out,0,width);
	// First Slice
	memcpy(out+width, in, sliceSize);
	// Copy next line if several slice
	if (nbSlices > 1){
		memcpy(out +  width + sliceSize , in + sliceSize, width);
	}
	// Slice other than first and last
	for(i=1; i<nbSlices-1; i++){
		int destIndex = i*(sliceSize+2*width);
		memcpy(out + destIndex, in+i*sliceSize-width, sliceSize+2*width);
	}
	// Last Slice
	i = nbSlices-1;
	if(nbSlices > 1){
		// we have i = nbSlice -1;
		int destIndex = i*(sliceSize+2*width);
		memcpy(out + destIndex, in+i*sliceSize-width, sliceSize+width);
	}
	// Last line
	memset(out + (height+nbSlices*2-1)*width,0,width);
}
