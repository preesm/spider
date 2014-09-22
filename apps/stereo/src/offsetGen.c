/*
	============================================================================
	Name        : offsetGen.h
	Author      : kdesnos
	Version     : 1.0
	Copyright   : CeCILL-C, IETR, INSA Rennes
	Description : Generation of an array of the offsets used in the 
	              computation of the depth map.
	============================================================================
*/

#include "../include/offsetGen.h"
#include <math.h>


void offsetGen (int nbIterations, unsigned char *offsets){
	int i;
	int val = 1;
	for(i=0; i< nbIterations; i++){
		offsets[i] = val%32;
		val *= 3;
	}
}
