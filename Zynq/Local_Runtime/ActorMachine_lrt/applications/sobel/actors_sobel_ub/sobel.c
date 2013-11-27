/*
	============================================================================
	Name        : sobel.c
	Author      : kdesnos
	Version     :
	Copyright   :
	Description :
	============================================================================
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <lrt.h>
#include <lrt_prototypes.h>
#include <print.h>

#include "sobel.h"

//static unsigned char input [WIDTH*(HEIGHT/16+2)];
//static unsigned char output[WIDTH*(HEIGHT/16+2)];

void sobelArgs(unsigned char* in, unsigned char* out, int width, int height, int nbSlices){
	int i, j;

	int sliceWidth = width;
	int sliceHeight = height/nbSlices+2;

	// Apply the filter
//	memset(out, 0, sliceWidth);
    for(j=1; j<sliceHeight-1; j++){
        for(i=0; i<sliceWidth; i++){
            int gx = -in[(j-1)*sliceWidth + i-1] - in[  j*sliceWidth + i-1]*2 -in[(j+1)*sliceWidth + i-1]
                     +in[(j-1)*sliceWidth + i+1] + in[  j*sliceWidth + i+1]*2 +in[(j+1)*sliceWidth + i+1];
            int gy = -in[(j-1)*sliceWidth + i-1] - in[(j-1)*sliceWidth + i]*2 -in[(j-1)*sliceWidth + i+1]
                     +in[(j+1)*sliceWidth + i-1] + in[(j+1)*sliceWidth + i]*2 +in[(j+1)*sliceWidth + i+1];

            out[j*sliceWidth + i] = (abs(gx) + abs(gy))/8;
        }
    }
//	memset(out + sliceSize - sliceWidth, 0, sliceWidth);
}

void sobel(){
    AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();

	if(action->nb_param != 3 || action->nb_fifo_in != 1 || action->nb_fifo_out != 1){
		zynq_puts("Sobel: Error in parameters or fifos count\n");
//		printf("param = %d (3), in = %d (1), out = %d (1)\n", action->nb_param, action->nb_fifo_in, action->nb_fifo_out);
		exit(1);
	}
	int width  = action->param_value[0];
	int height = action->param_value[1];
	int nbSlices = action->param_value[2];
	int sliceWidth = width;
	int sliceHeight = height/nbSlices+2;
	int sliceSize = sliceWidth*sliceHeight;

	UINT8 *input = OSAllocWorkingMemory(sliceSize);
	UINT8 *output = OSAllocWorkingMemory(sliceSize);

	MonitorAction Act = switchMonitor(DataTransfert);
    read_input_fifo(action->fifo_in_id[0], sliceSize, input);

	switchMonitor(Action + action->functionID);
    sobelArgs(input, output, width, height, nbSlices);

	switchMonitor(DataTransfert);
    write_output_fifo(action->fifo_out_id[0], sliceSize, output);
    switchMonitor(Act);

	OSFreeWorkingMemory();
}


