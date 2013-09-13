#include <pthread.h>

#include <lrt.h>
#include <lrt_prototypes.h>

#include <network/tcp.h>
#include <sobel.h>

/* 480p */
//#define WIDTH  720
//#define HEIGHT 400
//#define NBFRAMES 813
//#define VIDEOFILE "/root/bigbuckbunny.yuv"

//static int full;
//static unsigned char image[DISPLAY_W*DISPLAY_H*3/2];
static SOCKET sock_in;
//static pthread_t thread_readYUV;
//static int i=500;

//void* readYUV_run(void* arg);

//static FILE *ptfile ;

void readYUVInit(){
	sock_in  = tcpInit(SERVER, PORT_IN);

//	full = 0;
//	pthread_create(&thread_readYUV, NULL, readYUV_run, NULL);
//
//	int fsize;
//	if((ptfile = fopen(VIDEOFILE, "rb")) == NULL ){
//		fprintf(stderr,"ERROR: Task read cannot open yuv_file '%s'\n", VIDEOFILE);
//		return;
//	}
//
//	// Obtain file size:
//	fseek (ptfile , 0 , SEEK_END);
//	fsize = ftell (ptfile);
//	rewind (ptfile);
//	if(fsize < NBFRAMES*(WIDTH*HEIGHT*3/2)){
//		fprintf(stderr,"ERROR: Task read yuv_file incorrect size");
//		return;
//	}
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


void readYUVArgs (unsigned char* y, unsigned char* u, unsigned char* v, int width, int height, int color){
	int i=1;
	tcpSend(sock_in, &i, sizeof(int));
//	tcpReceive(sock_in, image, DISPLAY_W*DISPLAY_H*3/2);

	tcpReceive(sock_in, y, width*height);
	tcpReceive(sock_in, u, width*height/4);
	tcpReceive(sock_in, v, width*height/4);
}

//void readYUVArgs (unsigned char* y, unsigned char* u, unsigned char* v, int width, int height, int color){
//	static int frame = 0;
//
//    if(frame>=NBFRAMES){
//        rewind(ptfile);
//        frame = 0;
//    }
//
//    fread(y, sizeof(char), width*height, ptfile);
//    fread(u, sizeof(char), width*height/4, ptfile);
//    fread(v, sizeof(char), width*height/4, ptfile);
//
//    frame++;
//}

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

