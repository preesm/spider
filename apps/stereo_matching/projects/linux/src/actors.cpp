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

#include "actors.h"

extern "C"{
#include "stereomatch.h"
}

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void Config(
		OUT Param* nSlice,
		OUT Param* truncValue,
		OUT Param* scale,
		OUT Param* nIter,
		OUT Param* sizeFilter,
		OUT Param* minDisp,
		OUT Param* maxDisp,
		OUT Param* height,
		OUT Param* width){
	*width  = 434;
	*height = 380;
	*minDisp = 0;
	*maxDisp = 19;
	*sizeFilter = 1;
	*nIter = 5;
	*scale = 10;
	*truncValue = 1;
	*nSlice = 4;
}

void Camera(Param height, Param width, OUT uint8_t* rgb_L, OUT uint8_t* rgb_R){
	printf("Camera\n");
	int f;

	f = open("im2.ppm", O_RDWR);
	lseek(f, 15, SEEK_SET);
	read(f, rgb_L, height*width*3);
	close(f);

	f = open("im5.ppm", O_RDWR);
	lseek(f, 15, SEEK_SET);
	read(f, rgb_R, height*width*3);
	close(f);
}

void RGB2Gray(Param size, IN uint8_t* rgb, OUT uint8_t* gray){
	printf("RGB2Gray\n");

	for(int i=0; i<size; i++){
		gray[i] = ((int)rgb[3*i+0] + (int)rgb[3*i+1] + (int)rgb[3*i+2])/3;
	}
}

void Census(Param height, Param width, uint8_t* gray, OUT uint8_t* cen){
	printf("Census\n");
	census(height, width, gray, cen);
}

void Split(Param nSlice, Param sizeFilter, Param height, Param width, uint8_t* in, OUT uint8_t* out){
	printf("Split\n");
	int i;

//	memcpy(out, in, height*width);


	int subHeight = height/nSlice+2;
	int sliceSize = subHeight*width;

	// Fill first and last line with 0
	memset(out,0,width);
	// First Slice
	memcpy(out+width, in, sliceSize);
	// Copy next line if several slice
	if (nSlice > 1){
		memcpy(out +  width + sliceSize , in + sliceSize, width);
	}
	// Slice other than first and last
	for(i=1; i<nSlice-1; i++){
		int destIndex = i*(sliceSize+2*width);
		memcpy(out + destIndex, in+i*sliceSize-width, sliceSize+2*width);
	}
	// Last Slice
	i = nSlice-1;
	if(nSlice > 1){
		// we have i = nbSlice -1;
		int destIndex = i*(sliceSize+2*width);
		memcpy(out + destIndex, in+i*sliceSize-width, sliceSize+width);
	}
	// Last line
	memset(out + (height+nSlice*2-1)*width,0,width);
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

void MedianFilter(Param height, Param width, Param sizeFilter, uint8_t* in, OUT uint8_t* out){
	printf("MedianFilter\n");

//	memcpy(out, in, height*width);
	int k,l;
	// Process pixels one by one
	for(int j=0; j<height; j++){
		out[j*width] = 0;
		for(int i=1;i<width-1;i++){
			unsigned char pixels[9];
			// output pixel is the median of a 3x3 window
			// Get the 9 pixels
			int k=0;
			for(int y=j-1;y<=j+1;y++){
				for(int x=i-1;x<=i+1;x++){
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

void Display(Param height, Param width, uint8_t* rgb, uint8_t* depth){
	printf("Display\n");

	FILE * outFile;
	int i;
	if((outFile = fopen("result.ppm", "wb+")) == NULL )
    {
        fprintf(stderr,"ERROR: Task read cannot create/open ppm_file 'result.ppm'\n");
        return;
    }

	fprintf(outFile,"P6\n");
	fprintf(outFile,"%d %d\n",width,height);
	fprintf(outFile,"255\n");
	for(i=0; i<height*width;i++){
		unsigned char value = (*(depth+i));
		fwrite(&value,sizeof(char),1,outFile);
		fwrite(&value,sizeof(char),1,outFile);
		fwrite(&value,sizeof(char),1,outFile);
	}

	fclose(outFile);
}

void GenIx(Param nIter, OUT uint8_t* ixs){
	printf("GenIx\n");
	for(int i=0; i<nIter; i++){
		ixs[i] = i;
	}
}

void DisparityGen(Param maxDisp, Param minDisp, OUT uint8_t* dispIxs){
	printf("DisparityGen\n");
	for(int i=minDisp; i<=maxDisp; i++){
		dispIxs[i] = i;
	}
}

void HWeights(Param height, Param width, uint8_t* offset, uint8_t* rgbL, OUT uint8_t* out){
	printf("HWeights\n");
	compute_hweigth(height, width, *offset, rgbL, out);
}

void VWeights(Param height, Param width, uint8_t* offset, uint8_t* rgbL, OUT uint8_t* out){
	printf("VWeights\n");
	compute_vweigth(height, width, *offset, rgbL, out);
}

void CostConstruction(
		Param height, Param width, Param truncValue,
		uint8_t* grayL, uint8_t* grayR,
		uint8_t* cenL, uint8_t* cenR,
		uint8_t* disp, OUT uint8_t* error){
	printf("CostConstruction\n");

//	memcpy(error, grayL, height*width);

	cost_construction(height, width, *disp, grayR, grayL, cenR, cenL, error);
}

void AggregateCost(
		Param height, Param width, Param nIter,
		uint8_t* offsets,
		uint8_t* vWeights, uint8_t* hWeights,
		uint8_t* cost, OUT uint8_t* disp){
	printf("AggregateCost\n");

	uint8_t tmp[450*375];

	memcpy(disp, cost, width*height);
	/*recursive aggregation*/
	for(int i=0; i<nIter; i++){
		aggregateV(height, width, offsets[i], disp, vWeights+i*height*width, tmp);
		aggregateH(height, width, offsets[i], tmp, hWeights+i*height*width, disp);
	}
}

void DisparitySelect(
		Param height, Param width, Param scale,
		Param minDisp, Param maxDisp,
		uint8_t* dispVal, uint8_t* dispIx, uint8_t* curDisp,
		uint8_t* curCost, OUT uint8_t* disp, OUT uint8_t* cost){
	printf("DisparitySelect\n");

//	memcpy(disp, dispVal, width*height);

	/* disparity is argmin of cost */
	if(*dispIx == 0){
		/* first iteration */
		for(int i=0; i<height*width; i++){
			cost[i] = dispVal[i];
			disp[i] = (*dispIx)*scale;
		}
	}else{
		/* select disparity to minimize cost */
		for(int i=0; i<height*width; i++){
			if(dispVal[i] < curCost[i]){
				cost[i] = dispVal[i];
				disp[i] = (*dispIx)*scale;
			}else{
				cost[i] = curCost[i];
				disp[i] = curDisp[i];
			}
		}
	}
}

