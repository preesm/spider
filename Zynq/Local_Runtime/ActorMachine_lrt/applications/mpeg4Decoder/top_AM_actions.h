/*
 * top_AM_actions.h
 *
 *  Created on: 26 sept. 2013
 *      Author: yoliva
 */

#ifndef TOP_AM_ACTIONS_H_
#define TOP_AM_ACTIONS_H_

/* Top level. */
void readVOL();
void decodeDsply();



/*** DecodeDsply level -1 ***/
void inputVOL();
void inputComplexity();
void inputPosition();
void inputImgDimensions();

void readVOP();
void decodeVOP();
void displayVOP();

#endif /* TOP_AM_ACTIONS_H_ */
