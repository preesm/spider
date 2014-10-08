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

#include "stereo.h"

extern "C"{
#include "src/v4l2uvc.h"
#include "src/compute_stereo.h"
#include "src/stereomatch.h"
#include "image/image.h"
}

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <execution/execution.h>

#include <stdio.h>
#define PRINT 1

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <platform_types.h>

#define clamp(x) (x>255)? 255 : ((x<0)? 0 : x);
void yuyv_to_bgr (struct vdIn *vd, char* bgr){
	int i;
	unsigned char *ptrIn = vd->framebuffer;

	for (i = 0;  i < vd->height*vd->width;  i+=2){
	    int c = ptrIn[0] - 16;
	    int d = ptrIn[1] - 128;
	    int e = ptrIn[3] - 128;

	    bgr[i*3+0] = clamp(( 298 * c + 516 * d + 128) >> 8); // blue
	    bgr[i*3+1] = clamp(( 298 * c - 100 * d - 208 * e + 128) >> 8); // green
	    bgr[i*3+2] = clamp(( 298 * c + 409 * e + 128) >> 8); // red

	    c = ptrIn[2] - 16;
	    bgr[i*3+3] = clamp(( 298 * c + 516 * d + 128) >> 8); // blue
	    bgr[i*3+4] = clamp(( 298 * c - 100 * d - 208 * e + 128) >> 8); // green
	    bgr[i*3+5] = clamp(( 298 * c + 409 * e + 128) >> 8); // red

	    ptrIn += 4;
	}
}

void yuyv_to_rgb (struct vdIn *vd, unsigned char* r, unsigned char* g, unsigned char* b){
	int i;
	unsigned char *ptrIn = vd->framebuffer;

	for (i = 0;  i < vd->height*vd->width;  i+=2){
	    int c = ptrIn[0] - 16;
	    int d = ptrIn[1] - 128;
	    int e = ptrIn[3] - 128;

	    r[i] = clamp(( 298 * c + 409 * e + 128) >> 8); // red
	    g[i] = clamp(( 298 * c - 100 * d - 208 * e + 128) >> 8); // green
	    b[i] = clamp(( 298 * c + 516 * d + 128) >> 8); // blue

	    c = ptrIn[2] - 16;
	    r[i+1] = clamp(( 298 * c + 409 * e + 128) >> 8); // red
	    g[i+1] = clamp(( 298 * c - 100 * d - 208 * e + 128) >> 8); // green
	    b[i+1] = clamp(( 298 * c + 516 * d + 128) >> 8); // blue

	    ptrIn += 4;
	}
}


char* paths[] = {"./im0.ppm","./im1.ppm"};
static FILE * ptfile[2];
long imageStartPosition[2];

void readPPMInit(int id,int height, int width) {
    char magicNumber[3];
    int readWidth;
    int readHeight;
    int maxRGBValue;
	int fsize;

	printf("readPPMInit()\n");
    if((ptfile[id] = fopen(paths[id], "rb")) == NULL )
    {
        fprintf(stderr,"ERROR: Task read cannot open ppm_file '%s'\n", paths[id]);
        system("PAUSE");
        return;
    }

    // Read ppm file header
    // 1. Magic Numper
    fread(magicNumber, sizeof(char),2, ptfile[id]);
    magicNumber[2] = '\0';
    if(strcmp(magicNumber,"P6")){
        fprintf(stderr,"ERROR: PPM_file '%s' is not a valid PPM file.\n", paths[id]);
        system("PAUSE");
        return;
    }
    fseek(ptfile[id],1,SEEK_CUR); // skip space or EOL character


    // 2. Width and Height
    fscanf(ptfile[id],"%d", &readWidth);
    fscanf(ptfile[id],"%d", &readHeight);
    if(readWidth!=width || readHeight!= height){
        fprintf(stderr,"ERROR: PPM_file '%s' has an incorrect resolution.\nExpected: %dx%d\t Read: %dx%d\n", paths[id], width, height, readWidth,readHeight);
        system("PAUSE");
        return;
    }
    fseek(ptfile[id],1,SEEK_CUR); // skip space or EOL character

    // 3. Max RGB value
    fscanf(ptfile[id],"%d", &maxRGBValue);
    if(maxRGBValue > 255){
        fprintf(stderr,"ERROR: PPM_file '%s' has is coded with 32bits values, 8bits values are expected.\n", paths[id]);
        system("PAUSE");
        return;
    }
    fseek(ptfile[id],1,SEEK_CUR); // skip space or EOL character

    // Register the position of the file pointer
    imageStartPosition[id] = ftell(ptfile[id]);

    // check file size:
    fseek (ptfile[id] , 0 , SEEK_END);
    fsize = ftell (ptfile[id]) - imageStartPosition[id];
    fseek(ptfile[id],imageStartPosition[id], SEEK_SET);

    if(fsize != height*width*3)
    {
        fprintf(stderr,"ERROR: PPM_file has incorrect data size.\n\nExpected: %d\t Read: %d\n",height*width*3, fsize);
        system("PAUSE");
        return;
    }
}

void file(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];
	int id = params[2];

	/* Inputs */

	/* Outputs */
	UINT8* r = outputFIFOs[0];
	UINT8* g = outputFIFOs[1];
	UINT8* b = outputFIFOs[2];

//	char rgb[3*800*600];

#if PRINT
	printf("file %d %d %d\n", width, height, id);
#endif

	/* Fct */
//	if(ptfile[id] == NULL)
//		readPPMInit(id, height, width);

//	fseek(ptfile[id],imageStartPosition[id], SEEK_SET);

//	fread(rgb,sizeof(char), 3*width*height, ptfile[id]);

//    IplImage* image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);

    int imageWidth = 450;

    static char tmp[450*3];

    char name[10] = "im0.ppm";
    name[2] = '0'+id;

    int f = open(name, O_RDWR);
    lseek(f, 15, SEEK_SET);

    for(int i=0; i<height; i++){
//        lseek(f, 16+3*i*imageWidth,  SEEK_SET);
//        read(f, image->imageData+3*i*width, 3*width);

    	read(f, tmp, imageWidth*3);

        for(int j=0; j<width; j++){
        	r[i*width+j] = tmp[3*j+0];
        	g[i*width+j] = tmp[3*j+1];
        	b[i*width+j] = tmp[3*j+2];
        }
//    	fread(image->imageData+i*3*width,sizeof(char), 3*width, ptfile[id]);
//    	fgetc(ptfile[id]);
//    	fgetc(ptfile[id]);
//    	fgetc(ptfile[id]);
    }
//    read(f, image->imageData, image->imageSize);
    close(f);

//    cvConvertImage(image, image, 4);
//	cvShowImage( "base", image);
//	cvWaitKey(100);
//	cvReleaseImage(&image);

//	for(int i=0; i<width*height; i++){
//		r[i] = rgb[3*i+0];
//		g[i] = rgb[3*i+1];
//		b[i] = rgb[3*i+2];
//	}
}

void cam(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	static struct vdIn *videoInL = NULL, *videoInR = NULL;

	/* Params */
	int width = params[0];
	int height = params[1];
	int id = params[2];

	/* Inputs */

	/* Outputs */
	UINT8* r = outputFIFOs[0];
	UINT8* g = outputFIFOs[1];
	UINT8* b = outputFIFOs[2];

#if PRINT
	printf("cam %d %d %d\n", width, height, id);
#endif

	/* Fct Call */
	if(id == 0){
		if(videoInL == NULL){
			videoInL = (struct vdIn *) calloc (1, sizeof (struct vdIn));
			init_videoIn(videoInL, "/dev/video2", width, height, V4L2_PIX_FMT_YUYV, 1);
		}

//	    IplImage* imageL = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
//	    IplImage* imageLRect = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);

	    if (uvcGrab (videoInL) < 0) {
			fprintf (stderr, "Error grabbing\n");
			close_v4l2 (videoInL);
			exit (1);
		}

	    yuyv_to_rgb(videoInL, r,g,b);

//	    yuyv_to_bgr(videoInL, imageLRect->imageData);

//		CvMat *mx = (CvMat*) cvLoad("mx1.xml", NULL, NULL, NULL);
//		CvMat *my = (CvMat*) cvLoad("my1.xml", NULL, NULL, NULL);
//
//		cvRemap(imageL, imageLRect, mx, my, CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS, cvScalarAll(0));
//
//		for(int j=0; j<width*height; j++){
//			b[j] = imageLRect->imageData[3*j+0];
//			g[j] = imageLRect->imageData[3*j+1];
//			r[j] = imageLRect->imageData[3*j+2];
//		}

//		cvReleaseImage(&imageL);
//		cvReleaseImage(&imageLRect);
//		cvReleaseData(&mx);
//		cvReleaseData(&my);
	}else{
		if(videoInR == NULL){
			videoInR = (struct vdIn *) calloc (1, sizeof (struct vdIn));
			init_videoIn(videoInR, "/dev/video1", width, height, V4L2_PIX_FMT_YUYV, 1);
		}

//	    IplImage* imageR = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
//	    IplImage* imageRRect = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);

	    if (uvcGrab (videoInR) < 0) {
			fprintf (stderr, "Error grabbing\n");
			close_v4l2 (videoInL);
			exit (1);
		}
	    yuyv_to_rgb(videoInR, r,g,b);

//	    yuyv_to_bgr(videoInR, imageRRect->imageData);

//		CvMat *mx = (CvMat*) cvLoad("mx2.xml", NULL, NULL, NULL);
//		CvMat *my = (CvMat*) cvLoad("my2.xml", NULL, NULL, NULL);

//		cvRemap(imageR, imageRRect, mx, my, CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS, cvScalarAll(0));

//		for(int j=0; j<width*height; j++){
//			b[j] = imageRRect->imageData[3*j+0];
//			g[j] = imageRRect->imageData[3*j+1];
//			r[j] = imageRRect->imageData[3*j+2];
//		}

//		cvReleaseImage(&imageR);
//		cvReleaseImage(&imageRRect);
//		cvReleaseData(&mx);
//		cvReleaseData(&my);
	}
}

void writeFile(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];
	int nbDisp = params[2];

	/* Inputs */
	UINT8* Lr = inputFIFOs[0];
	UINT8* Lg = inputFIFOs[1];
	UINT8* Lb = inputFIFOs[2];
	UINT8* Rr = inputFIFOs[3];
	UINT8* Rg = inputFIFOs[4];
	UINT8* Rb = inputFIFOs[5];
	UINT8* disp = inputFIFOs[6];
//	UINT8* mono = inputFIFOs[7];

	/* Outputs */

#if PRINT
	printf("writeFile %d %d %d\n", width, height, nbDisp);
#endif

	FILE * outFile;
	int i;
	if((outFile = fopen("result.ppm", "wb+")) == NULL )
    {
        fprintf(stderr,"ERROR: Task read cannot create/open ppm_file 'result.ppm'\n");
        return;
    }

	int scale = 256/nbDisp;

	fprintf(outFile,"P6\n");
	fprintf(outFile,"%d %d\n",width,height);
	fprintf(outFile,"255\n");
	for(i=0; i<height*width;i++){
		unsigned char value = (*(disp+i))*scale;
		fwrite(&value,sizeof(char),1,outFile);
		fwrite(&value,sizeof(char),1,outFile);
		fwrite(&value,sizeof(char),1,outFile);
	}

	fclose(outFile);
}

//void display(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
//	/* Params */
//	int width = params[0];
//	int height = params[1];
//	int nbDisp = params[2];
//
//	/* Inputs */
//	UINT8* Lr = inputFIFOs[0];
//	UINT8* Lg = inputFIFOs[1];
//	UINT8* Lb = inputFIFOs[2];
//	UINT8* Rr = inputFIFOs[3];
//	UINT8* Rg = inputFIFOs[4];
//	UINT8* Rb = inputFIFOs[5];
//	UINT8* disp = inputFIFOs[6];
//	UINT8* mono = inputFIFOs[7];
//
//	/* Outputs */
//
//#if PRINT
//	printf("display %d %d %d\n", width, height, nbDisp);
//#endif
//
//	/* Fct */
//	bool ok=true;
//	for(int i=0; i<width*height; i++){
//		if(disp[i] != mono[i]){
//			printf("Check failed\n");
//			ok = false;
//			break;
//		}
//	}
//
//	if(ok)
//		printf("Check Ok ! \n");
//
//    IplImage* imageL = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
//    IplImage* imageR = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
//    IplImage* imageDisp = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
//    IplImage* imageMono = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
//
//    for(int i=0; i<width*height; i++){
//    	imageL->imageData[3*i+0] = Lb[i];
//    	imageL->imageData[3*i+1] = Lg[i];
//    	imageL->imageData[3*i+2] = Lr[i];
//
//    	imageR->imageData[3*i+0] = Rb[i];
//    	imageR->imageData[3*i+1] = Rg[i];
//    	imageR->imageData[3*i+2] = Rr[i];
//    }
//
//    int scale = 256/nbDisp;
//    for(int i=0; i<width*height; i++){
//    	disp[i]*=scale;
//    	mono[i]*=scale;
//    }
//
//    if(!ok){
//		IplImage* imageDiff = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
//		for(int i=0; i<width*height; i++){
//			if(disp[i] != mono[i])
//				imageDiff->imageData[i] = 255;
//			else
//				imageDiff->imageData[i] = 0;
//		}
//		cvShowImage( "Diff", imageDiff);
//		cvReleaseImage(&imageDiff);
//    }
//
//    memcpy(imageDisp->imageData, disp, width*height);
//    memcpy(imageMono->imageData, mono, width*height);
//
//	cvShowImage( "Left", imageL);
//	cvShowImage( "Right", imageR);
//	cvShowImage( "Disp", imageDisp);
//	cvShowImage( "Mono", imageMono);
//	cvWaitKey(100);
//
//    cvReleaseImage(&imageL);
//    cvReleaseImage(&imageR);
//    cvReleaseImage(&imageDisp);
//    cvReleaseImage(&imageMono);
//}

void stereoMono(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];
	int nbDisp = params[2];
	int nbIter = params[3];
	int nbSlices = params[4];

	/* Inputs */
	UINT8* Lr = inputFIFOs[0];
	UINT8* Lg = inputFIFOs[1];
	UINT8* Lb = inputFIFOs[2];
	UINT8* Rr = inputFIFOs[3];
	UINT8* Rg = inputFIFOs[4];
	UINT8* Rb = inputFIFOs[5];

	/* Outputs */
	UINT8* out = outputFIFOs[0];

#if PRINT
	printf("stereoMono %d %d %d %d\n", width, height, nbDisp, nbIter);
#endif

	/* Fct */
	struct str_image_rgb imcl, imcr;
	struct str_image_gray disp;

	image_rgb_init(&imcl, width, height);
	image_rgb_init(&imcr, width, height);
	image_gray_init(&disp, width, height);

	memcpy(imcl.r, Lr, width*height);
	memcpy(imcl.g, Lg, width*height);
	memcpy(imcl.b, Lb, width*height);

	memcpy(imcr.r, Rr, width*height);
	memcpy(imcr.g, Rg, width*height);
	memcpy(imcr.b, Rb, width*height);

	stereoMatch(&imcl, &imcr, nbDisp, &disp);

	memcpy(out, disp.g, width*height);
}

void rgb2Gray(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];

	/* Inputs */
	UINT8* r = inputFIFOs[0];
	UINT8* g = inputFIFOs[1];
	UINT8* b = inputFIFOs[2];

	/* Outputs */
	UINT8* gray = outputFIFOs[0];

#if PRINT
	printf("rgb2gray %d %d\n", width, height);
#endif

	for(int i=0; i<width*height; i++){
		gray[i] = ((int)r[i] + (int)g[i] + (int)b[i])/3;
	}
}

void census(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];

	/* Inputs */
	UINT8* gray = inputFIFOs[0];

	/* Outputs */
	UINT8* cens = outputFIFOs[0];

#if PRINT
	printf("census %d %d\n", width, height);
#endif

	census(height, width, gray, cens);
}

void genDelta(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int nbIter = params[0];

	/* Inputs */

	/* Outputs */
	UINT8* out_deltas = outputFIFOs[0];


#if PRINT
	printf("genDelta %d\n", nbIter);
#endif

	static const int deltas[] = {1, 4, 9, 16, 25, 3};

	for(int i=0; i<nbIter; i++)
		out_deltas[i] = deltas[i];
}

void compWeight(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];
	int horV = params[2];

	/* Inputs */
	UINT8* r = inputFIFOs[0];
	UINT8* g = inputFIFOs[1];
	UINT8* b = inputFIFOs[2];
	UINT8* delta = inputFIFOs[3];

	/* Outputs */
	UINT8* out = outputFIFOs[0];

#if PRINT
	printf("compWeight %d %d %d del%d\n", width,  height, horV, *delta);
#endif

	if(horV == 0){
		compute_hweigth(height, width, *delta, r, g, b, out);
	}else{
		compute_vweigth(height, width, *delta, r, g, b, out);
	}
}

void null(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){

}

void config(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	static int it=0;

	int nbIter = 6-it;
	int nbDisp = 10+10*it;
	it = (it+1)%6;

//	unsigned int outParams[2] = {nbDisp, nbIter};

	unsigned int outParams[2] = {60, 6};

	pushParam(curVertexId,2,outParams);

}

void disp(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];
	int nbDisp = params[2];
	int nbIter = params[3];

	/* Inputs */
	UINT8* Lg = inputFIFOs[0];
	UINT8* Lcen = inputFIFOs[1];
	UINT8* Rg = inputFIFOs[2];
	UINT8* Rcen = inputFIFOs[3];
	UINT8* hw = inputFIFOs[4];
	UINT8* vw = inputFIFOs[5];
	UINT8* deltas = inputFIFOs[6];

	/* Outputs */
	UINT8* out = outputFIFOs[0];


#if PRINT
	printf("disp %d %d %d %d\n", width,  height, nbDisp, nbIter);
#endif

	unsigned char cost1[450*375];
	unsigned char cost2[450*375];
	unsigned char buffval[450*375];

	for(int d=0; d<nbDisp; d++){
		/*cost construction*/
		cost_construction(
				height, width, d,
				Rg, Lg, Rcen, Lcen,
				cost1);

		/*recursive aggregation*/
		for(int i=0; i<nbIter; i++)
		{
			aggregateV(height, width, deltas[i], cost1, vw+i*height*width, cost2);
			aggregateH(height, width, deltas[i], cost2, hw+i*height*width, cost1);
		}

		/*disparity is argmin of cost*/
		if(d == 0)
		{
			/*first iteration*/
			memset(out, 0, height*width*sizeof(uint8_t));
			memcpy(buffval, cost1, height*width*sizeof(uint8_t));
		}
		else
		{
			/*select disparity to minimize cost*/
			for(int i=0; i<height*width; i++)
			{
				/*buffval contains the current minimum cost*/
				if(cost1[i] < buffval[i])
				{
					buffval[i] = cost1[i];	/*buffval = min(cost)*/
					out[i] = d;				/*out = argmin(cost)*/
				}
			}
		}
	}
}

void costConst(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];

	/* Inputs */
	UINT8* Lg = inputFIFOs[0];
	UINT8* Lcen = inputFIFOs[1];
	UINT8* Rg = inputFIFOs[2];
	UINT8* Rcen = inputFIFOs[3];
	UINT8* disp = inputFIFOs[4];

	/* Outputs */
	UINT8* out = outputFIFOs[0];


#if PRINT
	printf("costConst %d %d\n", width,  height);
#endif
	cost_construction(height, width, *disp,
				Rg, Lg, Rcen, Lcen,
				out);
}

void aggregate(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];
	int nbIter = params[2];

	/* Inputs */
	UINT8* vweight = inputFIFOs[0];
	UINT8* hweight = inputFIFOs[1];
	UINT8* deltas = inputFIFOs[2];
	UINT8* input = inputFIFOs[3];

	/* Outputs */
	UINT8* out = outputFIFOs[0];


#if PRINT
	printf("aggregate %d %d %d\n", width,  height, nbIter);
#endif

	unsigned char tmp[450*375];

	memcpy(out, input, width*height);
	/*recursive aggregation*/
	for(int i=0; i<nbIter; i++){
		aggregateV(height, width, deltas[i], out, vweight+i*height*width, tmp);
		aggregateH(height, width, deltas[i], tmp, hweight+i*height*width, out);
	}
}

void select(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];

	/* Inputs */
	UINT8* val_in = inputFIFOs[0];
	UINT8* disp_in = inputFIFOs[1];
	UINT8* input = inputFIFOs[2];
	UINT8* disp = inputFIFOs[3];

	/* Outputs */
	UINT8* val_out = outputFIFOs[0];
	UINT8* disp_out = outputFIFOs[1];


#if PRINT
	printf("select %d %d\n", width,  height);
#endif
	/* disparity is argmin of cost */
	if(*disp == 0){
		/* first iteration */
		memset(disp_out, 0, height*width);
		memcpy(val_out, input, height*width);
	}else{
		/* select disparity to minimize cost */
		for(int i=0; i<height*width; i++){
			/* buffval contains the current minimum cost */
			if(input[i] < val_in[i]){
				val_out[i] = input[i];	/*buffval = min(cost)*/
				disp_out[i] = *disp;				/*out = argmin(cost)*/
			}else{
				val_out[i] = val_in[i];	/*buffval = min(cost)*/
				disp_out[i] = disp_in[i];
			}
		}
	}
}

void genDisp(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int nbDisp = params[0];

	/* Inputs */

	/* Outputs */
	UINT8* out = outputFIFOs[0];


#if PRINT
	printf("genDisp %d\n", nbDisp);
#endif
	for(int i=0; i<nbDisp; i++){
		out[i] = i;
	}
}

static inline void swap(unsigned char *a, unsigned char *b){
	unsigned char buf = *a;
	*a=*b;
	*b=buf;
}

static void quickSortPartition(int startIdx, int endIdx, int *pivotIdx, unsigned char *values){
	int idx;
	int swapIdx = startIdx;
	swap(values+*pivotIdx,values+endIdx);
	for(idx = startIdx; idx < endIdx; idx++){
		if(values[idx]<=values[endIdx]){
			swap(values+swapIdx,values+idx);
			swapIdx++;
		}
	}
	swap(values+swapIdx, values+endIdx);
	*pivotIdx = swapIdx;
}


static void quickSort(int startIdx, int endIdx, unsigned char *values){
	if(startIdx<endIdx){
		int pivotIdx = startIdx;
		quickSortPartition(startIdx, endIdx, &pivotIdx, values);
		quickSort(startIdx,pivotIdx-1,values);
		quickSort(pivotIdx+1,endIdx,values);
	}
}

void medianSlice(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];
	int nbSlices = params[2];

	int subHeight = height/nbSlices+2;

	/* Inputs */
	UINT8* in = inputFIFOs[0];

	/* Outputs */
	UINT8* out = outputFIFOs[0];


#if PRINT
	printf("median %d %d %d\n", width, height, nbSlices);
#endif

	int k,l;
	// Process pixels one by one
	for(int j=0; j<height/nbSlices; j++){
		out[j*width] = 0;
		for(int i=1;i<width-1;i++){
			unsigned char pixels[9];
			// output pixel is the median of a 3x3 window
			// Get the 9 pixels
			int k=0;
			for(int y=j-1;y<=j+1;y++){
				for(int x=i-1;x<=i+1;x++){
					pixels[k++] = in[y*width+x];
				}
			}

			// Sort the 9 values
			quickSort(0, 8, pixels);
			out[j*width+i] = pixels[9/2];
		}
		out[j*width+width-1] = 0;
	}
}

void split(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	/* Params */
	int width = params[0];
	int height = params[1];
	int nbSlices = params[2];

	int subHeight = height/nbSlices+2;
	int sliceSize = subHeight*width;

	/* Inputs */
	UINT8* in = inputFIFOs[0];

	/* Outputs */
	UINT8* out = outputFIFOs[0];


#if PRINT
	printf("split %d %d %d\n", width, height, nbSlices);
#endif

	int i;

	// Fill first and last line with 0
	memset(out,0,width);
	// First Slice
	memcpy(out+width, in, sliceSize);
	// Copy next line if several slice
	if (nbSlices > 1){
		memcpy(out +  width + sliceSize , in + sliceSize, width);
	}
	// Slice other than first and last
	for(i=1; i<nbSlices-1; i++){
		int destIndex = i*(sliceSize+2*width);
		memcpy(out + destIndex, in+i*sliceSize-width, sliceSize+2*width);
	}
	// Last Slice
	i = nbSlices-1;
	if(nbSlices > 1){
		// we have i = nbSlice -1;
		int destIndex = i*(sliceSize+2*width);
		memcpy(out + destIndex, in+i*sliceSize-width, sliceSize+width);
	}
	// Last line
	memset(out + (height+nbSlices*2-1)*width,0,width);
}
