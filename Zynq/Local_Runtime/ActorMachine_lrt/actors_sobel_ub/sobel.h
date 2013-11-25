/*
 * sobel.h
 *
 *  Created on: Jul 5, 2013
 *      Author: jheulot
 */

#ifndef SOBEL_H_
#define SOBEL_H_

#define WIDTH 720
#define HEIGHT 400

void sobel();
void sobelArgs(unsigned char* in, unsigned char* out, int width, int height, int nbSlices);

#endif /* SOBEL_H_ */
