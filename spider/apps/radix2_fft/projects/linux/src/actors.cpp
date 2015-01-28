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

#include <spider.h>

#include "actors.h"
#include "data.h"

extern "C"{
	#include "DSP_fft16x16_cn.h"
	#include "gen_twiddle_fft16x16.h"
}

#include <stdio.h>
#include <string.h>
#include <math.h>

#define VERBOSE 1

short norm2_cplx(short *x){
	long real = x[0];
	long imag = x[1];
	return sqrt(real*real + imag*imag);
}

short normVect(short *x, int n){
	short max = 0;
	int i;
	for(i=0; i<n; i++){
		short val = norm2_cplx(x + 2*i);
		max = (val>max)?val:max;
	}
	return max;
}

float snr(short* sig, short* ref, int n){
	short diff[2*N_DATA];
	int i;
	for(i=0; i<n; i++){
		diff[2*i]   = sig[2*i]   - ref[2*i];
		diff[2*i+1] = sig[2*i+1] - ref[2*i+1];
	}
	return 20*log((float)normVect(sig, n)/(float)normVect(diff, n));
}

inline int bitRev(short v, int logN){
	short r=0;
	for(int n=0; n<logN; n++){
		r = (r<<1) + (v & 0x1);
		v = v>>1;
	}
	return r;
}

void genStepSwitch(Param NStep, char* steps, char* sels){
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

void cfgFftStep(char* in, Param* step){
#if VERBOSE
	printf("Execute cfgFftStep\n");
#endif
	*step = *in;
}


void src(Param fftSize, short *out){
#if VERBOSE
	printf("Execute Src\n");
#endif
	if(N_DATA != fftSize)
		printf("Bad size, bad SNR expected\n");

    memcpy(out, data_in, fftSize*2*sizeof(short));
}

void snk(Param fftSize, short *in){
#if VERBOSE
	printf("Execute Snk\n");
#endif

	for(int i=0; i<fftSize; i++){
		printf("%8d + %8d i\n", in[2*i], in[2*i+1]);
	}

    // Compute SNR
    float snrVal = snr(in, data_out, fftSize);
    printf("SNR %f dB\n", snrVal);
}

void mixFFT(int n, int p, int step, int id, short* in0, short* in1, short* out0, short* out1){
	int mask = (p-1) ^ ((1<<step)-1);
	int proc = ((id & mask)<<1) + ( id & (~mask & (p-1)) );
	printf("mixFFT : n%d p%d step%d id%d proc%d mask %#x %#x\n", n, p, step, id, proc, mask, ~mask & (p-1));
	for(int k=0; k<n/p; k++){
		int q = 1 << (int)(step+1+log2(n)-log2(p));
		int m = (proc*n/p+k) % q;

		short k_r = in0[2*k  ];
		short k_i = in0[2*k+1];
		short l_r = in1[2*k  ];
		short l_i = in1[2*k+1];

		float z_r = cos(-2*M_PI*m/q);
		float z_i = sin(-2*M_PI*m/q);

		short l2_r = l_r*z_r - l_i*z_i;
		short l2_i = l_i*z_r + l_r*z_i;

		out0[2*k  ] = k_r + l2_r;
		out0[2*k+1] = k_i + l2_i;

		out1[2*k  ] = k_r - l2_r;
		out1[2*k+1] = k_i - l2_i;
	}
}

void fftRadix2(
		Param NStep,
		Param fftSize,
		Param Step,
		short* in0,
		short* in1,
		char*  ix,
		short* out0,
		short* out1){
#if VERBOSE
	printf("Execute fftRadix2\n");
#endif

	mixFFT(128, 8, Step, *ix, in0, in1, out0, out1);
}

void ordering(Param fftSize, short* in, short *out){
#if VERBOSE
	printf("Execute ordering\n");
#endif

	for(int proc=0; proc<8; proc++){
		for(int k=0; k<fftSize/8; k++){
			out[2*(proc*fftSize/8+k)  ] = in[2*(bitRev(proc, 3)+k*8)  ];
			out[2*(proc*fftSize/8+k)+1] = in[2*(bitRev(proc, 3)+k*8)+1];
		}
	}
}

void fft(
		Param NStep,
		Param fftSize,
		short* in,
		short* out){

	static short w[2*N_DATA];

#if VERBOSE
	printf("Execute fft\n");
#endif

	int localFFTSize = 16;
    gen_twiddle_fft16x16(w, localFFTSize);

    DSP_fft16x16_cn(w, localFFTSize, in, out);

    for(int i=0; i<2*localFFTSize; i++)
    	out[i] = out[i]<<1;

}

void configFft(Param fftSize, Param* NStep){
#if VERBOSE
	printf("Execute configFft\n");
#endif

	*NStep = 3;
}

void selcfg(Param *out_Sel, char* in_Sel){
#if VERBOSE
	printf("Execute selcfg\n");
#endif

	// Set parameter's value.
	*out_Sel = in_Sel[0];
}

void genIx(Param NStep, char* ixs){
#if VERBOSE
	printf("Execute genIx\n");
#endif

	int maxIds = 1<<(NStep-1);
	for(int i=0; i<maxIds; i++){
		ixs[i] = i;
	}
}
