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

#include <string.h>
#include <platform_types.h>

#include "actors.h"

#include "lena_in.h"
#include "lena_out.h"

#define MAX(a,b) (a>b?a:b)

#pragma DATA_SECTION(input_image, ".image")
#pragma DATA_ALIGN(input_image, 128)

#pragma DATA_SECTION(output_image, ".image")
#pragma DATA_ALIGN(output_image, 128)

void rdFile(UINT8* inputFIFOs[],
			UINT8* outputFIFOs[],
			UINT32 params[])
{
	UINT32 NBSLICES_MEDIAN, NBSLICES_SOBEL;//, i;//, j;
	UINT32 HEIGHT, WIDTH;//, NBSLICESMAX_MEDIAN;

	HEIGHT = params[0];
	WIDTH = params[1];
//	NBSLICESMAX_MEDIAN = params[2];

	NBSLICES_MEDIAN = 16;
	NBSLICES_SOBEL = 32;
//	UINT8 medianTiles[4] = {4,4,4,8};

//	UINT8* out_medianTiles = outputFIFOs[0];
//	UINT8* out_image = outputFIFOs[1];

	// Sending parameter's value.
	platform_queue_push_UINT32(PlatformCtrlQueue, MSG_PARAM_VALUE);
	platform_queue_push_UINT32(PlatformCtrlQueue, OSTCBCur->vertexId);
	platform_queue_push_UINT32(PlatformCtrlQueue, NBSLICES_MEDIAN);
	platform_queue_push_UINT32(PlatformCtrlQueue, NBSLICES_SOBEL);
	platform_queue_push_finalize(PlatformCtrlQueue);

//	memcpy(out_medianTiles, medianTiles, NBSLICESMAX_MEDIAN);
//	memcpy(out_image, input_image, HEIGHT*WIDTH);
//	platform_writeFifo(outputFIFOs[0].id, outputFIFOs[0].add, NBSLICESMAX_MEDIAN, medianTiles);
//	platform_writeFifo(outputFIFOs[1].id, outputFIFOs[1].add, HEIGHT*WIDTH, input_image);
}


void initMedianLoop(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 NBSLICESMAX_MEDIAN, NBSLICES_MEDIAN;
	UINT8 *tiles;

	NBSLICES_MEDIAN = params[0];
	NBSLICESMAX_MEDIAN = params[1];

//	tiles = OSAllocWorkingMemory(NBSLICESMAX_MEDIAN);

	tiles = inputFIFOs[0];
	UINT8 *out_tiles = outputFIFOs[0];
//	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, NBSLICESMAX_MEDIAN, tiles);

//	int i;
//	printf("InitMedianLoop: ");
//	for(i=0; i<NBSLICES_MEDIAN; i++)
//		printf("%d ", tiles[i]);
//	printf("\n");

	memcpy(out_tiles, tiles, NBSLICES_MEDIAN);
//	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, NBSLICES_MEDIAN, tiles);

}


void configMedian(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT8 NBTILES_MEDIAN;

	NBTILES_MEDIAN = *(inputFIFOs[0]);
//	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, 1, &NBTILES_MEDIAN);

	platform_queue_push_UINT32(PlatformCtrlQueue, MSG_PARAM_VALUE);
	platform_queue_push_UINT32(PlatformCtrlQueue, OSTCBCur->vertexId);
	platform_queue_push_UINT32(PlatformCtrlQueue, NBTILES_MEDIAN);
	platform_queue_push_finalize(PlatformCtrlQueue);
}

void medianTile_fifo(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 HEIGHT, WIDTH, NBSLICES_MEDIAN;//, NBTILES_MEDIAN;
	UINT8 *tile_in, *tile_out;

	HEIGHT = params[0];
	WIDTH = params[1];
	NBSLICES_MEDIAN = params[2];
//	NBTILES_MEDIAN = params[3];

//	printf("MedianTile slices%d tile%d (%d:%#x) -> (%d:%#x)\n", NBSLICES_MEDIAN, NBTILES_MEDIAN, inputFIFOs[0].id, inputFIFOs[0].add, outputFIFOs[0].id, outputFIFOs[0].add);

//	int size = HEIGHT*WIDTH/(NBSLICES_MEDIAN*NBTILES_MEDIAN);

	tile_in = inputFIFOs[0];//OSAllocWorkingMemory(size);
	tile_out = outputFIFOs[0];//OSAllocWorkingMemory(size);

//	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, size, tile_in);

//	medianTile(tile_in, tile_out, NBSLICES_MEDIAN, NBTILES_MEDIAN, WIDTH, HEIGHT);
//	averageTile(tile_in, tile_out, NBSLICES_MEDIAN, NBTILES_MEDIAN, WIDTH, HEIGHT);
//	printf("sobel %#x -> %#x\n", tile_in, tile_out);
	sobelSlice(tile_in, tile_out, NBSLICES_MEDIAN, WIDTH, HEIGHT);
//	memcpy(tile_out, tile_in, size);

//	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, size, tile_out);
}

void medianTileFormat(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 HEIGHT, WIDTH, NBSLICES_MEDIAN, NBTILES_MEDIAN;
	UINT8 *slice_in, *slice_out, *base_slice_out;
	int i,j,l, tile;

	HEIGHT = params[0];
	WIDTH = params[1];
	NBSLICES_MEDIAN = params[2];
	NBTILES_MEDIAN = params[3];

//	printf("MedianTileFormat slices%d tile%d\n", NBSLICES_MEDIAN, NBTILES_MEDIAN);

	int sliceSize = HEIGHT*WIDTH/NBSLICES_MEDIAN;
	int sliceHeight = HEIGHT/NBSLICES_MEDIAN;
	int sliceWidth = WIDTH;
	int tileWidth = WIDTH/NBTILES_MEDIAN;
	int tileHeight = sliceHeight;
	int tileSize = WIDTH/NBTILES_MEDIAN*HEIGHT/NBSLICES_MEDIAN;

	slice_in = inputFIFOs[0];//OSAllocWorkingMemory(sliceSize);
	base_slice_out = slice_out = outputFIFOs[0];//OSAllocWorkingMemory(sliceSize);

//	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, sliceSize, slice_in);

	for(tile=0; tile<NBTILES_MEDIAN; tile++){
		UINT8* ptr_tile = slice_out+tile*tileSize;
		for(i=0; i<tileHeight; i++){
			memcpy(ptr_tile+i*tileWidth, slice_in+tile*tileWidth+i*sliceWidth, tileWidth);
		}
	}

//	for(i=0; i<NBTILES_MEDIAN; i++){
//		for(l=0; l<tileHeight; l++){
//			memcpy(slice_out+i*tileSize+l*tileWidth, slice_in + (i*tileHeight+l)*WIDTH + j*tileWidth, tileWidth);
//		}
//	}
//	memcpy(slice_out, slice_in, sliceSize);

//	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, sliceSize, base_slice_out);
}

void medianTileReconstruct(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 HEIGHT, WIDTH, NBSLICES_MEDIAN, NBTILES_MEDIAN;
	UINT8 *slice_in, *slice_out, *base_slice_out;
	int i,j,l, tile;

	HEIGHT = params[0];
	WIDTH = params[1];
	NBSLICES_MEDIAN = params[2];
	NBTILES_MEDIAN = params[3];

//	printf("MedianTileReconstruct slices%d tile%d\n", NBSLICES_MEDIAN, NBTILES_MEDIAN);

	int sliceSize = HEIGHT*WIDTH/NBSLICES_MEDIAN;
	int sliceHeight = HEIGHT/NBSLICES_MEDIAN;
	int sliceWidth = WIDTH;
	int tileWidth = WIDTH/NBTILES_MEDIAN;
	int tileHeight = sliceHeight;
	int tileSize = WIDTH/NBTILES_MEDIAN*HEIGHT/NBSLICES_MEDIAN;

	slice_in = inputFIFOs[0];//OSAllocWorkingMemory(sliceSize);
	base_slice_out = slice_out = outputFIFOs[0];//OSAllocWorkingMemory(sliceSize);

//	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, sliceSize, slice_in);

//	for(i=0; i<sliceHeight; i++){
//		for(j=0; j<NBTILES_MEDIAN; j++){
//			memcpy(slice_out, slice_in+j*tileSize, tileWidth);
//			slice_out += tileWidth;
//		}
//		slice_in += sliceWidth;
//	}
	for(tile=0; tile<NBTILES_MEDIAN; tile++){
		UINT8* ptr_tile = slice_in+tile*tileSize;
		for(i=0; i<tileHeight; i++){
			memcpy(slice_out+i*sliceWidth+tile*tileWidth, ptr_tile+i*tileWidth, tileWidth);
		}
	}
//	for(i=0; i<NBTILES_MEDIAN; i++){
//		for(l=0; l<tileHeight; l++){
//			memcpy(slice_out + (i*tileHeight+l)*WIDTH + j*tileWidth, slice_in+i*tileSize+l*tileWidth, tileWidth);
//		}
//	}
//	memcpy(slice_out, slice_in, sliceSize);

//	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, sliceSize, base_slice_out);
}

void sobelSlice_fifo(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 HEIGHT, WIDTH, NBSLICES_SOBEL;
	UINT8 *slice_in, *slice_out;

	HEIGHT = params[0];
	WIDTH = params[1];
	NBSLICES_SOBEL = params[2];

//	printf("SobelSlice slices%d\n", NBSLICES_SOBEL);

	slice_in = inputFIFOs[0];//OSAllocWorkingMemory(HEIGHT*WIDTH/NBSLICES_SOBEL);
	slice_out = outputFIFOs[0];//OSAllocWorkingMemory(HEIGHT*WIDTH/NBSLICES_SOBEL);

//	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, HEIGHT*WIDTH/NBSLICES_SOBEL, slice_in);

//	sobelSlice(slice_in, slice_out, NBSLICES_SOBEL, WIDTH, HEIGHT);
	averageSlice(slice_in, slice_out, NBSLICES_SOBEL, WIDTH, HEIGHT);
//	memcpy(slice_out, slice_in, HEIGHT*WIDTH/NBSLICES_SOBEL);

//	platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, HEIGHT*WIDTH/NBSLICES_SOBEL, slice_out);
}


void wrFile(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 i;
	UINT32 HEIGHT, WIDTH;
	UINT8 *image;

	HEIGHT = params[0];
	WIDTH = params[1];

	image = inputFIFOs[0];//OSAllocWorkingMemory(HEIGHT*WIDTH);

//	platform_readFifo(inputFIFOs[0].id, inputFIFOs[0].add, HEIGHT*WIDTH, image);

//	for(i=0; i<HEIGHT*WIDTH; i++){
//		if(output_image[i] != image[i]){
//			printf("Error at pixel %d: expected %d gets %d\n", i, output_image[i], image[i]);
//			break;
//		}
//	}

//	if(i != HEIGHT*WIDTH){
//		FILE* f = fopen("/home/jheulot/dev/lena_c6678_disp.data", "w+");
//		for(i=0; i<HEIGHT*WIDTH; i++){
//			unsigned char pxl = (output_image[i] == image[i])?0:255;
//			fputc(pxl,f);
//		}
////		fwrite(image, HEIGHT*WIDTH, 1, f);
//		fclose(f);
//	}
}

//******** Special actors ***********//
void RB(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 nbTknIn, nbTknOut;//, i;
	UINT32 quotient, residual;
	UINT8 *data;


	nbTknIn = params[0];
	nbTknOut = params[1];

	data = inputFIFOs[0];//OSAllocWorkingMemory(MAX(nbTknIn,nbTknOut));

//	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, nbTknIn * sizeof(UINT8), data);


//	printf("Round buffering %d to %d: %d", nbTknIn, nbTknOut, data[0]);
//	for (i = 1; i < nbTknIn; i++) {
//		printf(", %d", data[i]);
//	}
//	printf("\n");

	if(nbTknIn == nbTknOut){
		memcpy(outputFIFOs[0], inputFIFOs[0], nbTknIn);
//		platform_writeFifo(outputFIFOs[0].id, outputFIFOs[0].add, nbTknOut * sizeof(UINT8), data);
	}
//	else if(nbTknIn < nbTknOut){
//		quotient = nbTknOut / nbTknIn;
//		residual = nbTknOut % nbTknIn;
//		platform_writeFifo(outputFIFOs[0].id, outputFIFOs[0].add, quotient * sizeof(UINT8), data);
//		if(residual > 0)
//			platform_writeFifo(outputFIFOs[0].id, outputFIFOs[0].add, residual * sizeof(UINT8), data);
//	}
	else{
		printf("Error in RB, incoming tokens > outgoing tokens\n");
		exit(-1);
	}
}

void broadcast(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
//	UINT32 nbFifoOut, nbTknIn, i;
//	UINT8 *data;
//
//	nbFifoOut = params[0];
//	nbTknIn = params[1];
//
//	data = OSAllocWorkingMemory(nbTknIn);
//
//	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, nbTknIn * sizeof(UINT8), data);
//
////	printf("Broadcasting: %d", data[0]);
////	for (i = 1; i < nbTknIn; i++) {
////		printf(", %d", data[i]);
////	}
////	printf("\n");
//
//	for (i = 0; i < nbFifoOut; i++) {
//		platform_writeFifo(outputFIFOs[i].id, outputFIFOs[i].add, nbTknIn * sizeof(UINT8), data);
//	}
}


void Xplode(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 nbFifoIn, nbFifoOut, i, index;
	UINT8 *data;
	UINT32 nbTknIn, nbTknOut;
//	UINT32 inFifoBase, outFifoBase;

	nbFifoIn = params[0];
//	inFifoBase = params[1];
	nbFifoOut = params[1];
//	outFifoBase = params[3];

	index = 0;
	if(nbFifoIn == 1){
		/* Explode */
//		nbTknIn = params[2];
//		data = OSAllocWorkingMemory(nbTknIn);

//		platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, nbTknIn, data);

//		printf("Exploding : {%d tkn} -> ", nbTknIn);
//		for(i=0; i<nbFifoOut; i++){
//			nbTknOut = params[i + 3];
//			printf(" {%d tkn}", nbTknOut);
//			memcpy(outputFIFOs[i], inputFIFOs[0]+index, nbTknOut);
//			platform_writeFifo(outputFIFOs[i].id, outputFIFOs[i].add, nbTknOut, data+index);
//			index += nbTknOut;
//		}
//		printf("\n");
	}else if(nbFifoOut == 1){
		/* Implode */
//		printf("Imploding : ");
//		nbTknOut = params[nbFifoIn + 2];
//		data = OSAllocWorkingMemory(nbTknOut);

//		for(i=0; i<nbFifoIn; i++){
//			nbTknIn = params[i + 2];
//			printf("{%d tkn}", nbTknIn);
//			platform_readFifo(inputFIFOs[i].id, inputFIFOs[i].add, nbTknIn, data+index);
//			memcpy(outputFIFOs[0]+index, inputFIFOs[i], nbTknIn);
//			index += nbTknIn;
//		}
//		printf(" -> {%d tkn}", nbTknOut);
//		printf("\n");

//		platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, nbTknOut, data);
	}else{
		printf("Error in Xplode\n");
		exit(-1);
	}
}
