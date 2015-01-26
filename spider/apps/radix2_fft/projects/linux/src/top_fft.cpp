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

#include "top_fft.h"
#include <stdio.h>
#include <string.h>

#define VERBOSE 1

#define M_VAL 2
#define NB_TAPS 512

void genStepSwitch(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param NStep = inParams[0];

	char* steps = (char*) outputFIFOs[0];
	char* sels = (char*) outputFIFOs[1];

#if VERBOSE
	printf("Execute genStepSwitch\n");
#endif

	steps[0] = 0;
	sels[0] = 0;
	for(int i=1; i<NStep; i++){
		steps[i] = i;
		sels[i] = 1;
	}
}

void cfgFftStep(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param NMAX = inParams[0];
	Param N = inParams[1];

	char *in  = (char*) inputFIFOs[0];
	Param* step = (Param*) &outParams[0];

#if VERBOSE
	printf("Execute cfgFftStep\n");
#endif

	*step = *in;
}

void src(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param fftSize = inParams[0];
	float *out = (float*)outputFIFOs[0];

#if VERBOSE
	printf("Execute Src\n");
#endif

}

void snk(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param fftSize = inParams[0];
	float *in = (float*)inputFIFOs[0];

#if VERBOSE
	printf("Execute Snk\n");
#endif

}

void fftRadix2(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param NStep = inParams[0];
	Param fftSize = inParams[1];

	char* in0 = (char*) inputFIFOs[0];
	char* in1 = (char*) inputFIFOs[1];
	char* out0 = (char*) outputFIFOs[0];
	char* out1 = (char*) outputFIFOs[1];

#if VERBOSE
	printf("Execute fftRadix2\n");
#endif
}

void ordering(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param fftSize = inParams[0];

	char* in = (char*) inputFIFOs[0];
	char* out = (char*) outputFIFOs[0];

#if VERBOSE
	printf("Execute ordering\n");
#endif
}

void fft(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param NStep = inParams[0];
	Param fftSize = inParams[1];

	char* in = (char*) inputFIFOs[0];
	char* out = (char*) outputFIFOs[0];

#if VERBOSE
	printf("Execute fft\n");
#endif
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

void configFft(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param fftSize = inParams[0];
	Param* NStep = (Param*) &outParams[0];

#if VERBOSE
	printf("Execute configFft\n");
#endif

	*NStep = 3;
}

void selcfg(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param *out_Sel = (Param*) &outParams[0];
	char* in_Sel = (char*) inputFIFOs[0];

#if VERBOSE
	printf("Execute selcfg\n");
#endif

	// Set parameter's value.
	*out_Sel = in_Sel[0];
}

void genIx(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Param NStep = inParams[0];
	char* ixs = (char*) outputFIFOs[0];

#if VERBOSE
	printf("Execute genIx\n");
#endif

	// Set parameter's value.
}

lrtFct top_fft_fcts[N_FCT_TOP_FFT] = {
		&src,
		&snk,
		&genStepSwitch,
		&cfgFftStep,
		&fftRadix2,
		&Switch,
		&configFft,
		&selcfg,
		&ordering,
		&fft,
		&genIx
};
