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

#include "ederc_nvar.h"
#include <stdio.h>
#include <string.h>

#define VERBOSE 0

#define M_VAL 10
#define NB_TAPS 512

void config(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param NMAX = inParams[0];
	Param N = inParams[1];
	Param *out_N = (Param*) &outParams[0];
	char* out_M = (char*) outputFIFOs[0];

#if VERBOSE
	printf("Execute Config\n");
#endif

	// Set parameter's value.
	*out_N = N;

	for(int i=0; i<N; i++)
		out_M[i] = M_VAL;
}

void mFilter(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param NMAX = inParams[0];
	Param N = inParams[1];

	char *in_M  = (char*) inputFIFOs[0];
	char *out_M = (char*) outputFIFOs[0];

#if VERBOSE
	printf("Execute MFilter\n");
#endif

	memcpy(out_M, in_M, N);
}

void src(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param N = inParams[0];
	Param NSamples = inParams[1];
	Param TEST = inParams[2];

	float *out = (float*)outputFIFOs[0];

#if VERBOSE
	printf("Execute Src\n");
#endif

	if(TEST){
		for(int i=0; i<N; i++){
			srand(1000);
			for(int j=0; j<NSamples; j++){
				out[j+i*NSamples] = 10*((float)rand())/RAND_MAX;
			}
		}
	}
}

void snk(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param N = inParams[0];
	Param NSamples = inParams[1];
	Param TEST = inParams[2];

	float *in = (float*)inputFIFOs[0];

#ifdef DSP
	const int expectedHash[13]={
		/*0 */ 0x06088D61,	/*1 */ 0x03902DCA,	/*2 */ 0x7BE6A549,
		/*3 */ 0x7D008129,	/*4 */ 0x152FE04C,	/*5 */ 0x61FA4D45,
		/*6 */ 0x762F3E52,	/*7 */ 0x1A0D6EA7,	/*8 */ 0x407294F4,
		/*9 */ 0x28182904,	/*10*/ 0x60F32492,	/*11*/ 0x630A18F6,
		/*12*/ 0x476BCCAB
	};
#else
	const int expectedHash[13]={
		/*0 */ 0x1D8CCC7, 	/*1 */ 0x69D0FCD, 	/*2 */ 0x9CA48CA,
		/*3 */ 0x95CFC62, 	/*4 */ 0x5CAE39A, 	/*5 */ 0x170030E8,
		/*6 */ 0x16C43A1F,	/*7 */ 0x1F40E5E3,	/*8 */ 0x4D50F02B,
		/*9 */ 0x7D6D759, 	/*10*/ 0x49638F8,	/*11*/ 0x7BD1349F,
		/*12*/ 0x712A25F4
	};
#endif

#if VERBOSE
	printf("Execute Snk\n");
#endif

	if(TEST){
		int hash;
		for(int i=0; i<N; i++){
			hash = 0;
			int* data = (int*)in;
			for(int j=0; j<NSamples; j++){
				hash = hash ^ data[j+i*NSamples];
			}
			if(hash != expectedHash[M_VAL]){
				printf("Bad Hash result: %#X instead of %#X\n", hash, expectedHash[M_VAL]);
				return;
			}
		}
		printf("Result OK\n");
	}
}

void setM(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param *out_M = (Param*) &outParams[0];
	char* in_M = (char*) inputFIFOs[0];

#if VERBOSE
	printf("Execute setM %d\n", in_M[0]);
#endif

	// Set parameter's value.
	*out_M = in_M[0];
}

void initSwitch(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param M = inParams[0];
	char* out = (char*) outputFIFOs[0];

#if VERBOSE
	printf("Execute initSwitch\n");
#endif

	out[0] = 0;
	memset(out+1, 1, M-1);
}

void Switch(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param NSamples = inParams[0];
	char* in0 = (char*) inputFIFOs[0];
	char* in1 = (char*) inputFIFOs[1];
	char* sel = (char*) inputFIFOs[2];
	char* out = (char*) outputFIFOs[0];

#if VERBOSE
	printf("Execute Switch\n");
#endif

	switch(*sel){
	case 0:
		memcpy(out, in0, NSamples*sizeof(float));
		break;
	case 1:
		memcpy(out, in1, NSamples*sizeof(float));
		break;
	default:
		printf("Bad sel received in Switch\n");
		break;
	}
}

void FIR(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param NSamples = inParams[0];
	float* in = (float*) inputFIFOs[0];
	float* out = (float*) outputFIFOs[0];
	float taps[NB_TAPS];
	float last[NB_TAPS];

#if VERBOSE
	printf("Execute FIR\n");
#endif

	int i, j;

//	memcpy(out, in, NSamples*sizeof(float));

	int last_id = 0;
	memset(last,0,NB_TAPS*sizeof(float));
	for(i=0; i<NB_TAPS; i++){
		taps[i] = 1.0/NB_TAPS;
	}

	for(i=0; i<NSamples; i++){
		out[i] = 0;
		last[last_id] = in[i];
		for(j=0; j<NB_TAPS; j++){
			out[i] += taps[j]*last[(last_id+j)%NB_TAPS];
		}
		last_id = (last_id+1)%NB_TAPS;
	}
}

lrtFct ederc_nvar_fcts[NB_FCT_EDERC_NVAR] = {
		&config,
		&mFilter,
		&src,
		&snk,
		&setM,
		&initSwitch,
		&Switch,
		&FIR
};
