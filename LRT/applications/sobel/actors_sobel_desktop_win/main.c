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

#include <stdlib.h>
#include <stdio.h>

#include "sobel.h"

#include "../lrt/lrt.h"
#include "../lrt/lrt_prototypes.h"

#include <platform.h>

#define MAX_EXPLODE 854*480*2

///* 1080p */
//#define WIDTH  1920
//#define HEIGHT 1080
//#define VIDEOFILE "/home/julien/dev/sintel_trailer-1080p.raw"

///* 720p */
//#define WIDTH  1280
//#define HEIGHT 720
//#define VIDEOFILE "/home/julien/dev/sintel_trailer-720p.raw"

/* 480p */
//#define WIDTH  720
//#define HEIGHT 400
//#define NBFRAMES 14315
//#define VIDEOFILE "/home/jheulot/dev/bigbuckbunny.yuv"

#define WIDTH  720
#define HEIGHT 400
#define NBFRAMES 813
#define VIDEOFILE "C:\\work\\COMPA\\bigbuckbunny.yuv"


static UINT8 bufferExplode[MAX_EXPLODE];

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

	if(nbFifoIn == 1){
		/* Explode */
		read_input_fifo(action->fifo_in_id[0], action->param_value[0], bufferExplode);
		int count=0;
		for(i=0; i<nbFifoOut; i++){
			write_output_fifo(action->fifo_out_id[i], action->param_value[i+1], bufferExplode+count);
			count += action->param_value[i+1];
		}
	}else if(nbFifoOut == 1){
		/* Implode */
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

//	printf("Explode Finished\n");
}

int main(int argc, char** argv){
//	init_platform();

//	printf("TCB: %d\n",sizeof(OS_TCB));
//	printf("FUNCTION_TYPES: %d\n",sizeof(FUNCTION_TYPE));
//	printf("LRT_FIFO_HNDL: %d\n",sizeof(LRT_FIFO_HNDLE));
//	printf("AM_VERTEX_STRUCT: %d\n",sizeof(AM_VERTEX_STRUCT));
//	printf("AM_ACTOR_COND_STRUCT: %d\n",sizeof(AM_ACTOR_COND_STRUCT));
//	printf("AM_ACTOR_ACTION_STRUCT: %d\n",sizeof(AM_ACTOR_ACTION_STRUCT));

	if(argc>1)
		cpuId = atoi(argv[1]);
	else{
		printf("You must specify procID\n");
		abort();
	}

	if(cpuId == 0){
		displayYUVInit(WIDTH, HEIGHT);
		readYUVInit(WIDTH, HEIGHT, NBFRAMES, VIDEOFILE);
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

	init_lrt(0, 0);

//	while(1){
//		readYUVargs(y, u, v, WIDTH, HEIGHT);
//		displayYUVargs(y, u, v, WIDTH, HEIGHT);
//	}

//	cleanup_platform();

	return 0;
}
