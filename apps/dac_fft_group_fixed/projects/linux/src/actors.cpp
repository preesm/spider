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

#ifndef PI
# ifdef M_PI
#  define PI M_PI
# else
#  define PI 3.14159265358979323846
# endif
#endif

#define VERBOSE 0
#define TEST 1

static Cplx16 twi64k[64*1024];

static Cplx16 gen_twi32k[32*1024];
static Cplx16 gen_twi16k[16*1024];
static Cplx16 gen_twi8k [8*1024];
static Cplx16 gen_twi4k [4*1024];
static Cplx16 gen_twi2k [2*1024];
static Cplx16 gen_twi1k [1024];
static Cplx16 gen_twi256[256];

static short d2s(double d)
{
    d = floor(0.5 + d);  // Explicit rounding to integer //
    if (d >=  32767.0) return  32767;
    if (d <= -32768.0) return -32768;
    return (short)d;
}

void initActors(){
    double M = 32767.5;
	for(int i=0; i<64*1024; i++){
		twi64k[i].real = d2s(M*cos(-2*M_PI*i/(64*1024)));
		twi64k[i].imag = d2s(M*sin(-2*M_PI*i/(64*1024)));
	}
	gen_twiddle_fft16x16_imre((short*)gen_twi32k, 32*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi16k, 16*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi8k,   8*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi4k,   4*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi2k,   2*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi1k,     1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi256,     256);
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
		rms_sig  += sig[i].real*sig[i].real   + sig[i].imag*sig[i].imag;
		rms_diff += diff[i].real*diff[i].real + diff[i].imag*diff[i].imag;
	}
	rms_sig  = sqrt(rms_sig/n);
	rms_diff = sqrt(rms_diff/n);

//	printf("RMS Signal : %f\n", rms(sig, n));
//	printf("RMS Noise : %f\n", rms(diff, n));

	return 20*log(rms_sig/rms_diff);
}

void cfg(Param size, Param* Nc, Param* Nr, Param* n1, Param* n2){
#if VERBOSE
	printf("Execute Cfg\n");
#endif
	*Nc = 1<<((int)log2(size)/2);
	*Nr = size/(*Nc);
	*n1 = *Nc/8;
	*n2 = *Nr/8;
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
	int scaling = round(log2((double)data_out[1].real/in[1].real));

	for(int i=0; i<size; i++){
		in_scaled[i].real = in[i].real << scaling;
		in_scaled[i].imag = in[i].imag << scaling;
	}

    // Compute SNR
    float snrVal = snr(in_scaled, data_out, size);
    printf("SNR %f dB scaling %d\n", snrVal, 1<<scaling);
}

void fft(Param size, Param n, Cplx16* in, Cplx16* out){

#if VERBOSE
	printf("Execute fft\n");
#endif

	short* w;
	switch(size){
	case 32*1024:
		w = (short*)gen_twi32k;
		break;
	case 16*1024:
		w = (short*)gen_twi16k;
		break;
	case  8*1024:
		w = (short*)gen_twi8k;
		break;
	case  4*1024:
		w = (short*)gen_twi4k;
		break;
	case  2*1024:
		w = (short*)gen_twi2k;
		break;
	case    1024:
		w = (short*)gen_twi1k;
		break;
	case    256:
		w = (short*)gen_twi256;
		break;
	default:
		printf("Error no twiddles computed for %d\n", size);
		return;
	}

	for(int i=0; i<n; i++){
		DSP_fft16x16_imre_cn(w, size, (short*)in, (short*)out);
		in  += size;
		out += size;
	}
}

void transpose(Param Nc, Param Nr, Cplx16* in, Cplx16* out){
#if VERBOSE
	printf("Execute transpose\n");
#endif
	int i, j;
	for (i = 0; i < Nc; i++) {
		for (j = 0; j < Nr; j++) {
			out[i*Nr + j] = in[j*Nr + i];
			out[i*Nr + j] = in[j*Nr + i];
		}
	}
}

void twiddles(Param size, Param n, int* ix, Cplx16* in, Cplx16* out){
#if VERBOSE
	printf("Execute twiddles\n");
#endif
	int c;
	int r = (*ix)*n;

	for(int i=0; i<n; i++){
		for(c=0; c<size; c++){
			short real0 = (((long)in[c].real)*twi64k[r*c].real)>>16;
			short real1 = (((long)in[c].imag)*twi64k[r*c].imag)>>16;
			short imag0 = (((long)in[c].imag)*twi64k[r*c].real)>>16;
			short imag1 = (((long)in[c].real)*twi64k[r*c].imag)>>16;
			out[c].real = real0 - real1;
			out[c].imag = imag0 + imag1;
		}
		in  += size;
		out += size;
		r++;
	}
}

void genIx(Param Nr, Param n, int* ixs){
#if VERBOSE
	printf("Execute genIx\n");
#endif
	for(int i=0; i<Nr/n; i++){
		ixs[i] = i;
	}
}
