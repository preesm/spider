/*
 * sobel.h
 *
 *  Created on: Jul 5, 2013
 *      Author: jheulot
 */

#ifndef SOBEL_H_
#define SOBEL_H_

#define DISPLAY_W 720
#define DISPLAY_H 400

#define PORT_IN 2013
#define PORT_OUT 2014
#define SERVER "192.168.1.1"

void displayYUVInit();
void displayYUV();
void displayYUVArgs(unsigned char* y, unsigned char* u, unsigned char* v, int width, int height, int color);

void readYUVInit();
void readYUV();
void readYUVArgs (unsigned char* y, unsigned char* u, unsigned char* v, int width, int height, int color);

void split();
void splitArgs(unsigned char* in, unsigned char* out, int width, int height, int nbSlices);
void merge();
void mergeArgs(unsigned char* in, unsigned char* out, int width, int height, int nbSlices);

void sobel();
void sobelArgs(unsigned char* in, unsigned char* out, int width, int height, int nbSlices);

#endif /* SOBEL_H_ */
