/*
 * main.c
 *
 *  Created on: Jun 12, 2013
 *      Author: jheulot
 */

#include <stdlib.h>
#include <stdio.h>

#include "testCom.h"
#include "sobel.h"

#include "../lrt/lrt.h"
#include "../lrt/lrt_prototypes.h"

#include <platform.h>

///* 1080p */
//#define WIDTH  1920
//#define HEIGHT 1080
//#define VIDEOFILE "/home/julien/dev/sintel_trailer-1080p.raw"

///* 720p */
//#define WIDTH  1280
//#define HEIGHT 720
//#define VIDEOFILE "/home/julien/dev/sintel_trailer-720p.raw"

/* 480p */
#define WIDTH  720
#define HEIGHT 400
#define NBFRAMES 14315
#define VIDEOFILE "/home/julien/dev/bigbuckbunny.yuv"

//#define WIDTH  720
//#define HEIGHT 400
//#define NBFRAMES 813
//#define VIDEOFILE "/home/julien/dev/bigbuckbunny.yuv"

//#define MAX_EXPLODE WIDTH*HEIGHT*3
//static UINT8 bufferExplode[MAX_EXPLODE];

void Explode(){
	int i;
	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();

	int nbFifoIn = action->nb_fifo_in;
	int nbFifoOut = action->nb_fifo_out;
	int nbArgs = action->nb_param;

	if(nbArgs != nbFifoIn+nbFifoOut){
		printf("Error in Explode: Bad arg nb\n");
		exit(-1);
	}

	switchMonitor(DataTransfert);
	if(nbFifoIn == 1){
		/* Explode */
		UINT8* bufferExplode = OSAllocWorkingMemory(action->param_value[0]);

		read_input_fifo(action->fifo_in_id[0], action->param_value[0], bufferExplode);
		int count=0;
		for(i=0; i<nbFifoOut; i++){
			write_output_fifo(action->fifo_out_id[i], action->param_value[i+1], bufferExplode+count);
			count += action->param_value[i+1];
		}
	}else if(nbFifoOut == 1){
		/* Implode */
		UINT8* bufferExplode = OSAllocWorkingMemory(action->param_value[nbFifoIn]);

		int count=0;
		for(i=0; i<nbFifoIn; i++){
			read_input_fifo(action->fifo_in_id[i], action->param_value[i], bufferExplode+count);
			count += action->param_value[i+1];
		}
		write_output_fifo(action->fifo_out_id[0], action->param_value[nbFifoIn], bufferExplode);
	}else{
		printf("Error in Explode\n");
		exit(-1);
	}

	OSFreeWorkingMemory();
//	printf("Explode Finished\n");
}

int main(int argc, char** argv){
//	init_platform();

	if(argc>1)
		cpuId = atoi(argv[1]);
	else{
		printf("You must specify procID\n");
		abort();
	}

	if(cpuId == 0){
		displayYUVInit();
		readYUVInit();
	}

//	unsigned char y[WIDTH*HEIGHT];
//	unsigned char u[WIDTH*HEIGHT];
//	unsigned char v[WIDTH*HEIGHT];

	functions_tbl[0] = Explode;

	/* TestCom */
//	functions_tbl[1] = Generate;
//	functions_tbl[2] = Transform1;
//	functions_tbl[3] = Transform2;
//	functions_tbl[4] = Verify;

	/* Sobel */
	functions_tbl[1] = readYUV;
	functions_tbl[2] = split;
	functions_tbl[3] = sobel;
	functions_tbl[4] = merge;
	functions_tbl[5] = displayYUV;

	init_lrt();

//	while(1){
//		readYUVargs(y, u, v, WIDTH, HEIGHT);
//		displayYUVargs(y, u, v, WIDTH, HEIGHT);
//	}

//	cleanup_platform();

	return 0;
}
