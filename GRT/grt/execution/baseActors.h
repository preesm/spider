/*
 * baseActor.h
 *
 *  Created on: May 5, 2014
 *      Author: jheulot
 */

#ifndef BASEACTOR_H_
#define BASEACTOR_H_

#include <platform_types.h>

//******** Special actors ***********//
void RB(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[]);

void broadcast(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[]);

void switchFct(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[]);

void Xplode(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[]);

void InitVx(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[]);

void EndVx(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[]);

#endif /* BASEACTOR_H_ */
