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
#include "data_fx_imre.h"

extern "C"{
#include "DSP_fft16x16_imre_cn.h"
#include "gen_twiddle_fft16x16_imre.h"
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

static Cplx16 twi64k[32*1024];

static Cplx16 gen_twi32k[32*1024];
static Cplx16 gen_twi16k[16*1024];
static Cplx16 gen_twi8k [8*1024];
static Cplx16 gen_twi4k [4*1024];
static Cplx16 gen_twi2k [2*1024];
static Cplx16 gen_twi1k [1024];

void initActors(){
	for(int i=0; i<32*1024; i++){
		twi64k[i].real = cos(-2*M_PI*i/(64*1024));
		twi64k[i].imag = sin(-2*M_PI*i/(64*1024));
	}
	gen_twiddle_fft16x16_imre((short*)gen_twi32k, 32*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi16k, 16*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi8k,   8*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi4k,   4*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi2k,   2*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi1k,     1024);
}

float snr(Cplx32* sig, CplxSp* ref, int n){
	CplxSp diff[N_DATA];
	int i;
	for(i=0; i<n; i++){
		diff[i].real = sig[i].real - ref[i].real;
		diff[i].imag = sig[i].imag - ref[i].imag;
	}

	float rms_sig = 0;
	float rms_diff = 0;
	for(int i=0; i<n; i++){
		rms_sig  += ((float)sig[i].real)*sig[i].real + ((float)sig[i].imag)*sig[i].imag;
		rms_diff += diff[i].real*diff[i].real + diff[i].imag*diff[i].imag;
	}
	rms_sig  = sqrt(rms_sig/n);
	rms_diff = sqrt(rms_diff/n);

	printf("RMS Signal : %f\n", rms_sig);
	printf("RMS Noise  : %f\n", rms_diff);

	return 20*log(rms_sig/rms_diff);
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


void src(Param fftSize, Cplx16 *out){
#if VERBOSE
	printf("Execute Src\n");
#endif
	if(N_DATA != fftSize)
		printf("Bad size, bad SNR expected\n");

    memcpy(out, data_in, fftSize*sizeof(Cplx16));
}

void snk(Param fftSize, Cplx16 *in){
#if VERBOSE
	printf("Execute Snk\n");
#endif
	Cplx32 in_scaled[fftSize];
	int scaling = round(log2(data_out[0].real/in[0].real));

	for(int i=0; i<fftSize; i++){
		in_scaled[i].real = in[i].real << scaling;
		in_scaled[i].imag = in[i].imag << scaling;
	}

    // Compute SNR
    float snrVal = snr(in_scaled, data_out, fftSize);
    printf("SNR %f dB scaling %d\n", snrVal, 1<<scaling);
}

void fftRadix2(
		Param NStep,
		Param fftSize,
		Param Step,
		Cplx16* in0,
		Cplx16* in1,
		char*  ix,
		Cplx16* out0,
		Cplx16* out1){
#if VERBOSE
	printf("Execute fftRadix2\n");
#endif


	int localSize = fftSize/(1<<NStep);
	int id = *ix % (1<<Step);
	int increment = 1 << (NStep-Step-1);

	for(int k=0; k<localSize; k++){
		int m = (id * localSize + k) * increment;

//		printf("m/q%d == %f\n", m, fftSize*((1.*id/(1 << (Step+1))) + 1.*k/(localSize*(1<<(Step+1)))));

		float Ar = in0[k].real;
		float Ai = in0[k].imag;
		float Br = in1[k].real*cos(-2*M_PI*m/fftSize) - in1[k].imag*sin(-2*M_PI*m/fftSize);
		float Bi = in1[k].real*sin(-2*M_PI*m/fftSize) + in1[k].imag*cos(-2*M_PI*m/fftSize);

		out0[k].real = (Ar + Br);
		out0[k].imag = (Ai + Bi);

		out1[k].real = (Ar - Br);
		out1[k].imag = (Ai - Bi);
	}
}

void ordering(Param fftSize, Param NStep, Cplx16* in, Cplx16 *out){
#if VERBOSE
	printf("Execute ordering\n");
#endif
	int P = 1<<NStep;
	for(int proc=0; proc<P; proc++){
		for(int k=0; k<fftSize/P; k++){
			out[proc*fftSize/P+k].real = in[bitRev(proc, NStep)+k*P].real;
			out[proc*fftSize/P+k].imag = in[bitRev(proc, NStep)+k*P].imag;
		}
	}
}

void fft(
		Param NStep,
		Param fftSize,
		Cplx16* in,
		Cplx16* out){

#if VERBOSE
	printf("Execute fft\n");
#endif

	int localFFTSize = fftSize/(1<<NStep);

	Cplx16* w;
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

    DSP_fft16x16_imre_cn((short*)w, localFFTSize, (short*)in, (short*)out);

//    int scaling = 1;
//	for(int i=0; i<localFFTSize; i++){
//		out[i].real <<= scaling;
//		out[i].imag <<= scaling;
//	}
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
