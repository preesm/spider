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

#include <network/tcp.h>
#include <sobel.h>

//static int full=0;

static SOCKET sock_out;
//static pthread_t thread_displayYUV;
//static int i=500;

//void* displayYUV_run(void* arg);

void displayYUVInit(){
	sock_out  = tcpInit(SERVER, PORT_OUT);
//	full = 0;
//	pthread_create(&thread_displayYUV, NULL, displayYUV_run, NULL);
}

void displayYUV(){
	AM_ACTOR_ACTION_STRUCT* action = OSCurActionQuery();


	if(action->nb_param != 3 || action->nb_fifo_in != 3 || action->nb_fifo_out != 0){
		printf("Display: Error in parameters or fifos count\n");
		printf("param = %d (3), in = %d (3), out = %d (0)\n", action->nb_param, action->nb_fifo_in, action->nb_fifo_out);
		exit(1);
	}
	int width  = action->param_value[0];
	int height = action->param_value[1];
	int color = action->param_value[2];
	int imageSize = width*height;

	unsigned char *image = OSAllocWorkingMemory(imageSize*3/2);
	unsigned char *y = image;
	unsigned char *u = image + width*height;
	unsigned char *v = image + width*height*5/4;

//	while(full == 1) usleep(0);

	switchMonitor(DataTransfert);
	read_input_fifo(action->fifo_in_id[0], imageSize, y);
	read_input_fifo(action->fifo_in_id[1], color*imageSize/4, u);
	read_input_fifo(action->fifo_in_id[2], color*imageSize/4, v);

	switchMonitor(Action + action->functionID);
	displayYUVArgs (y, u, v, width, height, color);

	OSFreeWorkingMemory();

//	full = 1;

//	displayYUVargs(y, u, v, width, height, color);
}


void displayYUVArgs (unsigned char* y, unsigned char* u, unsigned char* v, int width, int height, int color){
	if(!color){
		memset(u, 128, width*height/4);
		memset(v, 128, width*height/4);
	}

//	int i=1;
//	tcpSend(sock_out, &i, sizeof(int));
//	tcpSend(sock_out, image, DISPLAY_W*DISPLAY_H*3/2);
	tcpSend(sock_out, y, width*height);
	tcpSend(sock_out, u, width*height/4);
	tcpSend(sock_out, v, width*height/4);
}

//void* displayYUV_run(void* arg){
////	while(i>0){
//	int i=1;
//	while(1){
//		while(full == 0) usleep(0);
//		tcpSend(sock_out, &i, sizeof(int));
//		tcpSend(sock_out, image, DISPLAY_W*DISPLAY_H*3/2);
//		full = 0;
//	}
//	return NULL;
//}
