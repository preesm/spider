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

#include "../sobel.h"


static unsigned char input[DISPLAY_W*DISPLAY_H*2];
static unsigned char output[DISPLAY_W*DISPLAY_H*2];

void split(){
	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();

	if(action->nb_param != 3 || action->nb_fifo_in != 1 || action->nb_fifo_out != 1){
		printf("Split: Error in parameters or fifos count\n");
		printf("param = %d (3), in = %d (1), out = %d (1)\n", action->nb_param, action->nb_fifo_in, action->nb_fifo_out);
		exit(1);
	}
	int width  = action->param_value[0];
	int height = action->param_value[1];
	int nbSlices = action->param_value[2];
	int imageSize = width*height;
	int sliceWidth = width;
	int sliceHeight = height/nbSlices+2;
	int sliceSize = sliceWidth*sliceHeight;
	int nbOutputLines = height + nbSlices * 2;
	int currentSlice;

    read_input_fifo(action->fifo_in_id[0], imageSize, input);

	int outputLine, copiedLine;
	memset(output,0,width);
	for(outputLine = 1; outputLine < nbOutputLines - 1; outputLine++){
		currentSlice = outputLine / sliceHeight;
		copiedLine = outputLine - 1 - 2 * currentSlice;
		memcpy(output+outputLine * width, input+copiedLine * width, width);
	}

	write_output_fifo(action->fifo_out_id[0], sliceSize*nbSlices, output);
}


void merge(){
	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();

	if(action->nb_param != 3 || action->nb_fifo_in != 1 || action->nb_fifo_out != 1){
		printf("Merge: Error in parameters or fifos count\n");
		printf("param = %d (3), in = %d (1), out = %d (1)\n", action->nb_param, action->nb_fifo_in, action->nb_fifo_out);
		exit(1);
	}
	int width  = action->param_value[0];
	int height = action->param_value[1];
	int nbSlices = action->param_value[2];
	int imageSize = width*height;
	int sliceWidth = width;
	int sliceHeight = height/nbSlices+2;
	int sliceSize = sliceWidth*sliceHeight;
	int currentSlice, lineToCopy, frameLine = 0;

    read_input_fifo(action->fifo_in_id[0], sliceSize*nbSlices, input);

	for (frameLine=0; frameLine < height; frameLine++) {
		currentSlice = frameLine / (height/nbSlices);
		lineToCopy = frameLine + 1 + 2 * currentSlice;
		memcpy(output+frameLine * width, input+lineToCopy * width, width);
	}

	write_output_fifo(action->fifo_out_id[0], imageSize, output);
}

