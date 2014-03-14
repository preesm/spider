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

void displayYUVInit (int width, int height);
void displayYUV();
void displayYUVargs(unsigned char* y, unsigned char* u, unsigned char* v, int width, int height, int color);

void readYUVInit(int width, int height, int nbFrames, const char* path);
void readYUV();
void readYUVargs (unsigned char* y, unsigned char* u, unsigned char* v, int width, int height, int color);

void split();
void merge();

void sobel();

#endif /* SOBEL_H_ */
