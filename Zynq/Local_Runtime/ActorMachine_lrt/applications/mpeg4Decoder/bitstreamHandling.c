/*
 * VOL.c
 *
 *  Created on: 30 oct. 2013
 *      Author: yoliva
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "definitions.h"





/*
 * Reads bytes up to the next start code (including the start code).
 * It assumes that the initial position in the file is just after the previous start code.
 */
void readUpToNextStartCode(FILE* pFile, uchar* buffer, uint* nbBytesRead){
	// Reading immediate start code.
	fread(buffer, sizeof(int), 1, pFile);
	uint bufferPosition = sizeof(int);

	uchar prefixChance = 0;
	uchar prefixDetected = 0;

	do{
		uchar* data = &buffer[bufferPosition];

		fread(data, sizeof(uchar), 1, pFile); bufferPosition++;

		// Identifying start code prefix.
		if((*data == 0x00) && (prefixChance < 2)){
			prefixChance++;
		}
		else if((*data == 0x01) && (prefixChance == 2)){
			prefixDetected = 1;
		}
		else{
			prefixChance = 0;
		}
	}while(!prefixDetected && !feof(pFile));

	if(feof(pFile)){
		*nbBytesRead = bufferPosition;
	}
	else{
		*nbBytesRead = bufferPosition - 3; // -3 bytes so the next start code is not included.
		// Setting the file position back to the beginning of the next start code.
		fseek(pFile, -3, SEEK_CUR);
	}
}
