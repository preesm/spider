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

#include <platform.h>

#include "actors.h"
#include "data_sp.h"

extern "C"{
#include "DSPF_sp_fftSPxSP_cn.h"
}

#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#define VERBOSE 0

unsigned char brev[64] = {
    0x0, 0x20, 0x10, 0x30, 0x8, 0x28, 0x18, 0x38,
    0x4, 0x24, 0x14, 0x34, 0xc, 0x2c, 0x1c, 0x3c,
    0x2, 0x22, 0x12, 0x32, 0xa, 0x2a, 0x1a, 0x3a,
    0x6, 0x26, 0x16, 0x36, 0xe, 0x2e, 0x1e, 0x3e,
    0x1, 0x21, 0x11, 0x31, 0x9, 0x29, 0x19, 0x39,
    0x5, 0x25, 0x15, 0x35, 0xd, 0x2d, 0x1d, 0x3d,
    0x3, 0x23, 0x13, 0x33, 0xb, 0x2b, 0x1b, 0x3b,
    0x7, 0x27, 0x17, 0x37, 0xf, 0x2f, 0x1f, 0x3f
};

static float twi64k_r[32*1024];
static float twi64k_i[32*1024];

static float gen_twi32k[2*32*1024];
static float gen_twi16k[2*16*1024];
static float gen_twi8k [2*8*1024];
static float gen_twi4k [2*4*1024];
static float gen_twi2k [2*2*1024];
static float gen_twi1k [2*1024];

void tw_gen (float *w, int n);

void initActors(){
	for(int i=0; i<32*1024; i++){
		twi64k_r[i] = cos(-2*M_PI*i/(64*1024));
		twi64k_i[i] = sin(-2*M_PI*i/(64*1024));
	}
	tw_gen(gen_twi32k, 32*1024);
	tw_gen(gen_twi16k, 16*1024);
	tw_gen(gen_twi8k,   8*1024);
	tw_gen(gen_twi4k,   4*1024);
	tw_gen(gen_twi2k,   2*1024);
	tw_gen(gen_twi1k,     1024);
}

static inline float getTwi_r(int m, int q){
//	return cos(-2*M_PI*m/q);
	return twi64k_r[64*1024/q*m];
}

static inline float getTwi_i(int m, int q){
//	return sin(-2*M_PI*m/q);
	return twi64k_i[m*64*1024/q];
}

/* Function for generating Specialized sequence of twiddle factors */
void tw_gen (float *w, int n)
{
    int i, j, k;
    const double PI = 3.141592654;

    for (j = 1, k = 0; j <= n >> 2; j = j << 2)
    {
        for (i = 0; i < n >> 2; i += j)
        {
#ifdef _LITTLE_ENDIAN
            w[k]     = (float) sin (2 * PI * i / n);
            w[k + 1] = (float) cos (2 * PI * i / n);
            w[k + 2] = (float) sin (4 * PI * i / n);
            w[k + 3] = (float) cos (4 * PI * i / n);
            w[k + 4] = (float) sin (6 * PI * i / n);
            w[k + 5] = (float) cos (6 * PI * i / n);
#else
            w[k]     = (float)  cos (2 * PI * i / n);
            w[k + 1] = (float) -sin (2 * PI * i / n);
            w[k + 2] = (float)  cos (4 * PI * i / n);
            w[k + 3] = (float) -sin (4 * PI * i / n);
            w[k + 4] = (float)  cos (6 * PI * i / n);
            w[k + 5] = (float) -sin (6 * PI * i / n);
#endif
            k += 6;
        }
    }
}


float norm2_cplx(float *x){
	float real = x[0];
	float imag = x[1];
	return sqrt(real*real + imag*imag);
}

float rms(float *x, int n){
	float sum = 0;
	for(int i=0; i<n; i++){
		float val = norm2_cplx(x + 2*i);
		sum += val*val;
	}
	return sqrt(sum/n);
}

float snr(float* sig, float* ref, int n){
	float diff[2*N_DATA];
	int i;
	for(i=0; i<n; i++){
		diff[2*i]   = sig[2*i]   - ref[2*i];
		diff[2*i+1] = sig[2*i+1] - ref[2*i+1];
	}

//	printf("RMS Signal : %f\n", rms(sig, n));
//	printf("RMS Noise : %f\n", rms(diff, n));

	return 20*log(rms(sig, n)/rms(diff, n));
}

inline int bitRev(short v, int logN){
	short r=0;
	for(int n=0; n<logN; n++){
		r = (r<<1) + (v & 0x1);
		v = v>>1;
	}
	return r;
}

void genStepSwitch(Param NStep, char* steps){
#if VERBOSE
	printf("Execute genStepSwitch\n");
#endif
	steps[0] = 0;
	for(int i=1; i<NStep; i++){
		steps[i] = i;
	}
}

void cfgFftStep(char* in, Param* step){
#if VERBOSE
	printf("Execute cfgFftStep\n");
#endif
	*step = *in;
}


void src(Param fftSize, float *out){
#if VERBOSE
	printf("Execute Src\n");
#endif
	if(N_DATA != fftSize)
		printf("Bad size, bad SNR expected\n");

    memcpy(out, data_in, fftSize*2*sizeof(float));
}

void snk(Param fftSize, float *in){
#if VERBOSE
	printf("Execute Snk\n");
#endif

    // Compute SNR
    float snrVal = snr(in, data_out, fftSize);
    printf("SNR %f dB\n", snrVal);
}

void mixFFT(int n, int p, int step, int id, float* in0, float* in1, float* out0, float* out1){

}

static inline int remBit(int n, int bit){
	return ((n >> (bit+1)) << bit) + (n & ((1<<bit)-1));
}

void fftRadix2(
		Param NStep,
		Param fftSize,
		Param Step,
		float* in0,
		float* in1,
		char*  ix,
		float* out0,
		float* out1){
#if VERBOSE
	printf("Execute fftRadix2\n");
#endif


	int localSize = fftSize/(1<<NStep);
	int id = *ix % (1<<Step);
	int increment = 1 << (NStep-Step-1);

	for(int k=0; k<localSize; k++){
		int m = (id * localSize + k) * increment;

//		printf("m/q%d == %f\n", m, fftSize*((1.*id/(1 << (Step+1))) + 1.*k/(localSize*(1<<(Step+1)))));

		float i0r = in0[2*k  ];
		float i0i = in0[2*k+1];
		float i1r = in1[2*k  ];
		float i1i = in1[2*k+1];

		float Ar = i0r;
		float Ai = i0i;
		float Br = i1r*cos(-2*M_PI*m/fftSize) - i1i*sin(-2*M_PI*m/fftSize);
		float Bi = i1r*sin(-2*M_PI*m/fftSize) + i1i*cos(-2*M_PI*m/fftSize);

		out0[2*k  ] = Ar + Br;
		out0[2*k+1] = Ai + Bi;

		out1[2*k  ] = Ar - Br;
		out1[2*k+1] = Ai - Bi;


//		float z_r = cos(-2*M_PI*m/fftSize);
//		float z_i = sin(-2*M_PI*m/fftSize);
//
//		float z_r = getTwi_r(m,q);
//		float z_i = getTwi_i(m,q);
//
//		float l2_r = l_r*z_r - l_i*z_i;
//		float l2_i = l_i*z_r + l_r*z_i;
//
//		out0[2*k  ] = k_r + l2_r;
//		out0[2*k+1] = k_i + l2_i;
//
//		out1[2*k  ] = k_r - l2_r;
//		out1[2*k+1] = k_i - l2_i;
	}
}

void ordering(Param fftSize, Param NStep, float* in, float *out){
#if VERBOSE
	printf("Execute ordering\n");
#endif
	int P = 1<<NStep;
	for(int proc=0; proc<P; proc++){
		for(int k=0; k<fftSize/P; k++){
			out[2*(proc*fftSize/P+k)  ] = in[2*(bitRev(proc, NStep)+k*P)  ];
			out[2*(proc*fftSize/P+k)+1] = in[2*(bitRev(proc, NStep)+k*P)+1];
		}
	}
}

void fft(
		Param NStep,
		Param fftSize,
		float* in,
		float* out){

//	static float w[2*N_DATA];

#if VERBOSE
	printf("Execute fft\n");
#endif

	int localFFTSize = fftSize/(1<<NStep);

	int rad;
	int j = 0;
	for (int i = 0; i <= 31; i++)
		if ((localFFTSize & (1 << i)) == 0)
			j++;
		else
			break;

	if (j % 2 == 0)
		rad = 4;
	else
		rad = 2;

	float* w;
	switch(localFFTSize){
	case 32*1024:
		w = gen_twi32k;
		break;
	case 16*1024:
		w = gen_twi16k;
		break;
	case  8*1024:
		w = gen_twi8k;
		break;
	case  4*1024:
		w = gen_twi4k;
		break;
	case  2*1024:
		w = gen_twi2k;
		break;
	case    1024:
		w = gen_twi1k;
		break;
	default:
		printf("Error no twiddles computed for %d\n", localFFTSize);
		return;
	}

    DSPF_sp_fftSPxSP_cn(localFFTSize, in, w, out, brev, rad, 0, localFFTSize);
}

void configFft(Param fftSize, Param* NStep){
#if VERBOSE
	printf("Execute configFft\n");
#endif

	static int nstep = 1;

	*NStep = nstep++;
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
