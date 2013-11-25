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

static unsigned char input[DISPLAY_W*DISPLAY_H];
static unsigned char output[DISPLAY_W*DISPLAY_H];

void sobel(){
    int i,j;
    AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();

	if(action->nb_param != 3 || action->nb_fifo_in != 1 || action->nb_fifo_out != 1){
		printf("Sobel: Error in parameters or fifos count\n");
		printf("param = %d (3), in = %d (1), out = %d (1)\n", action->nb_param, action->nb_fifo_in, action->nb_fifo_out);
		exit(1);
	}
	int width  = action->param_value[0];
	int height = action->param_value[1];
	int nbSlices = action->param_value[2];
	int sliceWidth = width;
	int sliceHeight = height/nbSlices+2;
	int sliceSize = sliceWidth*sliceHeight;

    read_input_fifo(action->fifo_in_id[0], sliceSize, input);

	// Apply the filter
	memset(output, 0, sliceWidth);
    for(j=1; j<sliceHeight-1; j++){
        for(i=0; i<sliceWidth; i++){
            int gx = -input[(j-1)*sliceWidth + i-1] -2*input[  j*sliceWidth + i-1] -input[(j+1)*sliceWidth + i-1]
                     +input[(j-1)*sliceWidth + i+1] +2*input[  j*sliceWidth + i+1] +input[(j+1)*sliceWidth + i+1];
            int gy = -input[(j-1)*sliceWidth + i-1] -2*input[(j-1)*sliceWidth + i] -input[(j-1)*sliceWidth + i+1]
                     +input[(j+1)*sliceWidth + i-1] +2*input[(j+1)*sliceWidth + i] +input[(j+1)*sliceWidth + i+1];

            output[j*sliceWidth + i] = (abs(gx) + abs(gy))/8;
        }
    }
	memset(output + sliceSize - sliceWidth, 0, sliceWidth);

    write_output_fifo(action->fifo_out_id[0], sliceSize, output);
}


