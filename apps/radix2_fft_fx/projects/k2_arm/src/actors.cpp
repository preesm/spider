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

#include "Radix2_fft.h"
#include "actors.h"
#include "data_fx_imre.h"

extern "C"{
#include "DSP_fft16x16_imre_cn.h"
#include "gen_twiddle_fft16x16_imre.h"
int fftc_send (int fftc_ix, Cplx16* in, Cplx16* out, int fftSize, int numBlocks);
}

#include <stdio.h>
#include <string.h>
#include <math.h>

#define VERBOSE 0

static Cplx16 twi64k[32*1024];

static Cplx16 gen_twi32k[32*1024];
static Cplx16 gen_twi16k[16*1024];
static Cplx16 gen_twi8k [8*1024];
static Cplx16 gen_twi4k [4*1024];
static Cplx16 gen_twi2k [2*1024];
static Cplx16 gen_twi1k [1024];
static Cplx16 gen_twi512 [512];
static Cplx16 gen_twi256 [256];
static Cplx16 gen_twi128 [128];

static short d2s(double d)
{
    d = floor(0.5 + d);  // Explicit rounding to integer //
    if (d >=  32767.0) return  32767;
    if (d <= -32768.0) return -32768;
    return (short)d;
}

void initActors(){
	for(int i=0; i<32*1024; i++){
		twi64k[i].real = d2s(32768.5*cos(-2*M_PI*i/(64*1024)));
		twi64k[i].imag = d2s(32768.5*sin(-2*M_PI*i/(64*1024)));
	}
	gen_twiddle_fft16x16_imre((short*)gen_twi32k, 32*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi16k, 16*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi8k,   8*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi4k,   4*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi2k,   2*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi1k,     1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi512,     512);
	gen_twiddle_fft16x16_imre((short*)gen_twi256,     256);
	gen_twiddle_fft16x16_imre((short*)gen_twi128,     128);
}

static inline float snr(Cplx32* sig, CplxSp* ref, int n){
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

	return 20*log(rms_sig/rms_diff);
}

static inline int bitRev(short v, int N){
	short r=0;
	int logN = log2(N);
	for(int n=0; n<logN; n++){
		r = (r<<1) + (v & 0x1);
		v = v>>1;
	}
	return r;
}

void cfgFFT(Param* size, Param* P, Param* n1, Param* n2){
#if VERBOSE
	printf("Execute cfgFFT\n");
#endif
	*size 	= N_DATA;
	*P 		= 4;
	*n1 	= 4*1024;
	*n2 	= 2;
}

void genIx(Param n, int* ixs){
#if VERBOSE
	printf("Execute genIx: n=%d\n", n);
#endif
	for(int i=0; i<n; i++){
		ixs[i] = i;
	}
}

void cfg(int* in, Param* out){
#if VERBOSE
	printf("Execute cfg: in=%d\n", *in);
#endif
	*out = *in;
}

void src(Param size, Cplx16 *out){
#if VERBOSE
	printf("Execute Src\n");
#endif
	if(N_DATA != size)
		printf("Bad size, bad SNR expected\n");

    memcpy(out, data_in, size*sizeof(Cplx16));
}

void snk(Param size, Cplx16 *in){
#if VERBOSE
	printf("Execute Snk\n");
#endif
	Cplx32 in_scaled[size];
	int scaling = round(log2(data_out[0].real/in[0].real));

	for(int i=0; i<size; i++){
		in_scaled[i].real = in[i].real << scaling;
		in_scaled[i].imag = in[i].imag << scaling;
	}

    // Compute SNR
    float snrVal = snr(in_scaled, data_out, size);
    printf("SNR %f dB scaling %d (%d bits)\n", snrVal, 1<<scaling, scaling);
}

void T(Param N1, Param N2, Cplx16* in, Cplx16 *out){
#if VERBOSE
	printf("Execute T\n");
#endif

	for(int n2=0; n2<N2; n2++){
		for(int n1=0; n1<N1; n1++){
			out[n1*N2+n2].real = in[bitRev(n1, N1)+n2*N1].real;
			out[n1*N2+n2].imag = in[bitRev(n1, N1)+n2*N1].imag;
		}
	}
}

void fft(Param size, Param n, Cplx16* in, Cplx16* out){
#if VERBOSE
	printf("Execute fft\n");
#endif

	switch(Platform::get()->getLrt()->getIx()){
	case CORE_ARM0:
		fftc_send (0, in, out, size, n);
		break;
	case CORE_ARM1:
		fftc_send (1, in, out, size, n);
		break;
	}
}

void fft_2(Param n, Param p, Param N2, Param N1, char* ix, Cplx16* i0, Cplx16* i1, Cplx16* o0, Cplx16* o1){
#if VERBOSE
	printf("Execute fft_2\n");
#endif

	const int id	  = (*ix)*n;
	const int m = (id % (N2*(1<<(p))));
	const unsigned short incr = N1 / (1 << (p+1));

	for(int k=0; k<n; k++){
		unsigned short r = (m+k)*incr;

		long i1r = i1[k].real;
		long i1i = i1[k].imag;
		short Br = (long)(i1r*twi64k[r].real - i1i*twi64k[r].imag)>>15;
		short Bi = (long)(i1r*twi64k[r].imag + i1i*twi64k[r].real)>>15;

		o0[k].real = (i0[k].real + Br)>>1;
		o0[k].imag = (i0[k].imag + Bi)>>1;

		o1[k].real = (i0[k].real - Br)>>1;
		o1[k].imag = (i0[k].imag - Bi)>>1;
	}
}
