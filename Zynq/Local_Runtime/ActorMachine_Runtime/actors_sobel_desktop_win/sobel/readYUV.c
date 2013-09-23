/*
	============================================================================
	Name        : readYUV.c
	Author      : kdesnos
	Version     :
	Copyright   :
	Description :
	============================================================================
*/

#include "../sobel.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <lrt.h>
#include <lrt_prototypes.h>

/*========================================================================

   Global Variable

   ======================================================================*/
static FILE *ptfile ;
static int nbFrames;
static unsigned char y[DISPLAY_W*DISPLAY_H];
static unsigned char u[DISPLAY_W*DISPLAY_H/4];
static unsigned char v[DISPLAY_W*DISPLAY_H/4];

/*========================================================================

   initReadYUV DEFINITION

   ======================================================================*/
void readYUVInit(int width, int height, int _nbFrames, const char* path) {
    int fsize;
    if((ptfile = fopen(path, "rb")) == NULL ){
		fprintf(stderr,"ERROR: Task read cannot open yuv_file '%s'\n", path);
		return;
	}

    nbFrames = _nbFrames;

	// Obtain file size:
	fseek (ptfile , 0 , SEEK_END);
	fsize = ftell (ptfile);
	rewind (ptfile);
	if(fsize < nbFrames*(width*height*3/2)){
		fprintf(stderr,"ERROR: Task read yuv_file incorrect size");
		return;
	}
}

/*========================================================================

   readYUV DEFINITION

   ======================================================================*/
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

	readYUVargs (y, u, v, width, height, color);

	write_output_fifo(action->fifo_out_id[0], imageSize, y);
	write_output_fifo(action->fifo_out_id[1], color*imageSize/4, u);
	write_output_fifo(action->fifo_out_id[2], color*imageSize/4, v);
}

void readYUVargs (unsigned char* y, unsigned char* u, unsigned char* v, int width, int height, int color) {
	int imageSize = width*height;

    if(ftell(ptfile)>=nbFrames*(imageSize+imageSize/2)){
        rewind(ptfile);
    }

    fread(y, sizeof(char), imageSize, ptfile);
    if(color){
		fread(u, sizeof(char), imageSize/4, ptfile);
		fread(v, sizeof(char), imageSize/4, ptfile);
    }else
    	fseek(ptfile, imageSize/2, SEEK_CUR);
}

