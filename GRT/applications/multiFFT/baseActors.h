/*
 * baseActor.h
 *
 *  Created on: May 5, 2014
 *      Author: jheulot
 */

#ifndef BASEACTOR_H_
#define BASEACTOR_H_

#define MIN(a,b) (a>b?b:a)
//******** Special actors ***********//
void RB(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 nbTknIn, nbTknOut;//, i;
	UINT32 quotient, residual;
	UINT8 *data;


	nbTknIn = params[0];
	nbTknOut = params[1];

	data = inputFIFOs[0];//OSAllocWorkingMemory(MAX(nbTknIn,nbTknOut));

//	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, nbTknIn * sizeof(UINT8), data);


//	printf("Round buffering %d to %d: %d", nbTknIn, nbTknOut, data[0]);
//	for (i = 1; i < nbTknIn; i++) {
//		printf(", %d", data[i]);
//	}
//	printf("\n");

	if(nbTknIn == nbTknOut){
		memcpy(outputFIFOs[0], inputFIFOs[0], nbTknIn);
//		platform_writeFifo(outputFIFOs[0].id, outputFIFOs[0].add, nbTknOut * sizeof(UINT8), data);
	}
	else if(nbTknIn < nbTknOut){
		int offset=0;
		while(nbTknOut){
			int rest = MIN(nbTknIn,nbTknOut);
			memcpy(outputFIFOs[0]+offset, inputFIFOs[0], rest);
			nbTknOut -= rest;
			offset += rest;
		}
	}else{
		memcpy(outputFIFOs[0], inputFIFOs[0]+nbTknIn-nbTknOut, nbTknOut);
	}
}

void broadcast(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 nbToken = params[0]*sizeof(float);

//	memcpy(outputFIFOs[0], inputFIFOs[0], nbToken);
//	memcpy(outputFIFOs[1], inputFIFOs[0], nbToken);
//	UINT32 nbFifoOut, nbTknIn, i;
//	UINT8 *data;
//
//	nbFifoOut = params[0];
//	nbTknIn = params[1];
//
//	data = OSAllocWorkingMemory(nbTknIn);
//
//	platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, nbTknIn * sizeof(UINT8), data);
//
////	printf("Broadcasting: %d", data[0]);
////	for (i = 1; i < nbTknIn; i++) {
////		printf(", %d", data[i]);
////	}
////	printf("\n");
//
//	for (i = 0; i < nbFifoOut; i++) {
//		platform_writeFifo(outputFIFOs[i].id, outputFIFOs[i].add, nbTknIn * sizeof(UINT8), data);
//	}
}


void Xplode(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 nbFifoIn, nbFifoOut, i, index;
	UINT8 *data;
	UINT32 nbTknIn, nbTknOut;
//	UINT32 inFifoBase, outFifoBase;

//	printf("Ex/Implode\n");

	nbFifoIn = params[0];
//	inFifoBase = params[1];
	nbFifoOut = params[1];
//	outFifoBase = params[3];

	index = 0;
	if(nbFifoIn == 1){
		/* Explode */
//		nbTknIn = params[2];
//		data = OSAllocWorkingMemory(nbTknIn);

//		platform_readFifo(inputFIFOs[0].id,inputFIFOs[0].add, nbTknIn, data);

//		printf("Exploding : {%d tkn} -> ", nbTknIn);
//		for(i=0; i<nbFifoOut; i++){
//			nbTknOut = params[i + 3];
//			printf(" {%d tkn}", nbTknOut);
//			memcpy(outputFIFOs[i], inputFIFOs[0]+index, nbTknOut);
//			platform_writeFifo(outputFIFOs[i].id, outputFIFOs[i].add, nbTknOut, data+index);
//			index += nbTknOut;
//		}
//		printf("\n");
	}else if(nbFifoOut == 1){
		/* Implode */
		if(inputFIFOs[0] != outputFIFOs[0]){
	//		printf("Imploding : ");
			nbTknOut = params[nbFifoIn + 2];
	//		data = OSAllocWorkingMemory(nbTknOut);

			for(i=0; i<nbFifoIn; i++){
				nbTknIn = params[i + 2];
	//			printf("{%d tkn}", nbTknIn);
	//			platform_readFifo(inputFIFOs[i].id, inputFIFOs[i].add, nbTknIn, data+index);
				memcpy(outputFIFOs[0]+index, inputFIFOs[i], nbTknIn);
				index += nbTknIn;
			}
	//		printf(" -> {%d tkn}", nbTknOut);
	//		printf("\n");

	//		platform_writeFifo(outputFIFOs[0].id,outputFIFOs[0].add, nbTknOut, data);
		}
	}else{
		printf("Error in Xplode\n");
		exit(-1);
	}
}

void InitVx(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 nbTokens = params[0];
	memset(outputFIFOs[0], 0, nbTokens);
}

void EndVx(UINT8* inputFIFOs[],
		UINT8* outputFIFOs[],
		UINT32 params[])
{
	UINT32 nbTokens = params[0];
}

#endif /* BASEACTOR_H_ */
