/*
 * actors.h
 *
 *  Created on: Apr 9, 2014
 *      Author: jheulot
 */

#ifndef ACTORS_H_
#define ACTORS_H_

void sobelSlice(unsigned char* in, unsigned char* out, int nbSlices, int width, int height);
void sobelTile(unsigned char* in, unsigned char* out, int nbSlices, int nbTiles, int width, int height);
void medianSlice(unsigned char* in, unsigned char* out, int nbSlices, int width, int height);
void medianTile(unsigned char* in, unsigned char* out, int nbSlices, int nbTiles, int width, int height);
void averageTile(unsigned char* in, unsigned char* out, int nbSlices, int nbTiles, int width, int height);
void averageSlice(unsigned char* in, unsigned char* out, int nbSlices, int width, int height);

#endif /* ACTORS_H_ */
