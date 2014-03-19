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
#include <stdlib.h>
#include <stdio.h>
#include <lrt.h>
#include <lrt_prototypes.h>

#include <sobel.h>

//static unsigned char input[DISPLAY_W*DISPLAY_H];
//static unsigned char output[DISPLAY_W*DISPLAY_H];

void sobelArgs(unsigned char* in, unsigned char* out, int width, int height, int nbSlices){
	int i, j;

	int sliceWidth = width;
	int sliceHeight = height/nbSlices+2;
//	int sliceSize = sliceWidth*sliceHeight;

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

	UINT8 *input = OSAllocWorkingMemory(sliceSize);
	UINT8 *output = OSAllocWorkingMemory(sliceSize);

	switchMonitor(DataTransfert);
    read_input_fifo(action->fifo_in_id[0], sliceSize, input);

	switchMonitor(Action + action->functionID);
    sobelArgs(input, output, width, height, nbSlices);

	switchMonitor(DataTransfert);
    write_output_fifo(action->fifo_out_id[0], sliceSize, output);

	OSFreeWorkingMemory();
}


