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

#include <pthread.h>

#include <lrt.h>
#include <lrt_prototypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <network/tcp.h>
#include <sobel.h>

/* 480p */
#define WIDTH  720
#define HEIGHT 400
#define NBFRAMES 250
#define VIDEOFILE "/root/bigbuckbunny.yuv"

//static int full;
////static unsigned char image[DISPLAY_W*DISPLAY_H*3/2];
//static SOCKET sock_in;
//static pthread_t thread_readYUV;
//static int i=500;

//void* readYUV_run(void* arg);

static FILE *ptfile ;

void readYUVInit(){
//	tcpInit(SERVER, PORT_IN);

//	full = 0;
//	pthread_create(&thread_readYUV, NULL, readYUV_run, NULL);
//
	int fsize;
	if((ptfile = fopen(VIDEOFILE, "rb")) == NULL ){
		fprintf(stderr,"ERROR: Task read cannot open yuv_file '%s'\n", VIDEOFILE);
		return;
	}

	// Obtain file size:
	fseek (ptfile , 0 , SEEK_END);
	fsize = ftell (ptfile);
	rewind (ptfile);
	if(fsize < NBFRAMES*(WIDTH*HEIGHT*3/2)){
		fprintf(stderr,"ERROR: Task read yuv_file incorrect size");
		return;
	}
}

void readYUV() {
	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();

	if(action->nb_param != 3 || action->nb_fifo_in != 0 || action->nb_fifo_out != 3){
		printf("Read: Error in parameters or fifos count\n");
		printf("param = %d (3), in = %d (0), out = %d (3)\n", action->nb_param, action->nb_fifo_in, action->nb_fifo_out);
		exit(1);
	}

	int width  = action->param_value[0];
	int height = action->param_value[1];
	int color = action->param_value[2];
	int imageSize = width*height;

	unsigned char *y = OSAllocWorkingMemory(imageSize);
	unsigned char *u = OSAllocWorkingMemory(imageSize/4);
	unsigned char *v = OSAllocWorkingMemory(imageSize/4);

//	while(full == 0) usleep(0);

	switchMonitor(Action + action->functionID);
	readYUVArgs(y, u, v, width, height, color);

	switchMonitor(DataTransfert);
	write_output_fifo(action->fifo_out_id[0], imageSize, y);
	write_output_fifo(action->fifo_out_id[1], color*imageSize/4, u);
	write_output_fifo(action->fifo_out_id[2], color*imageSize/4, v);

	OSFreeWorkingMemory();
//	full = 0;
}


//void readYUVArgs (unsigned char* y, unsigned char* u, unsigned char* v, int width, int height, int color){
////	int i=1;
////	tcpSend(&i, sizeof(int));
////	tcpReceive(sock_in, image, DISPLAY_W*DISPLAY_H*3/2);
//
//	tcpReceive(y, width*height);
//	tcpReceive(u, width*height/4);
//	tcpReceive(v, width*height/4);
//}

void readYUVArgs (unsigned char* y, unsigned char* u, unsigned char* v, int width, int height, int color){
	static int frame = 0;

    if(frame>=NBFRAMES){
        rewind(ptfile);
        frame = 0;
    }

    fread(y, sizeof(char), width*height, ptfile);
    fread(u, sizeof(char), width*height/4, ptfile);
    fread(v, sizeof(char), width*height/4, ptfile);

    frame++;
}

//void* readYUV_run(void* arg){
////	while(i>0){
//	int i=1;
//	while(1){
//		while(full == 1) usleep(0);
//		tcpSend(sock_in, &i, sizeof(int));
//		tcpReceive(sock_in, image, DISPLAY_W*DISPLAY_H*3/2);
////		i--;
//		full = 1;
//	}
//	return NULL;
//}

