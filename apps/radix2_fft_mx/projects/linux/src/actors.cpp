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
#include "DSP_fft16x16_imre_cn.h"
#include "gen_twiddle_fft16x16_imre.h"
}

#include <stdio.h>
#include <string.h>
#include <math.h>

#define VERBOSE 0

static CplxSp twi64k[64*1024];

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

static inline float snr(CplxSp* sig, CplxSp* ref, int n){
	CplxSp diff[N_DATA];
	int i;
	for(i=0; i<n; i++){
		diff[i].real = sig[i].real - ref[i].real;
		diff[i].imag = sig[i].imag - ref[i].imag;
	}

	float rms_sig = 0;
	float rms_diff = 0;
	for(int i=0; i<n; i++){
		rms_sig  += sig[i].real *sig[i].real  + sig[i].imag *sig[i].imag;
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
	*P 		= 5;
	*n1 	= 2*1024;
	*n2 	= 4;
}

void genIx(Param n, int* ixs){
#if VERBOSE
	printf("Execute genIx: n=%ld\n", n);
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

void src(Param size, CplxSp *out){
#if VERBOSE
	printf("Execute Src\n");
#endif
	if(N_DATA != size)
		printf("Bad size, bad SNR expected\n");

    memcpy(out, data_in, size*2*sizeof(float));
}

void snk(Param size, CplxSp *in){
#if VERBOSE
	printf("Execute Snk\n");
#endif
	CplxSp in_scaled[size];
	int scaling = round(log2(data_out[0].real/in[0].real));

	for(int i=0; i<size; i++){
		in_scaled[i].real = in[i].real * (1<<scaling);
		in_scaled[i].imag = in[i].imag * (1<<scaling);
	}

    // Compute SNR
    float snrVal = snr(in_scaled, data_out, size);
    printf("SNR %f dB scaling %d\n", snrVal,(1<<scaling));
}

void T(Param N1, Param N2, CplxSp* in, CplxSp *out){
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

	Cplx16* w;
	switch(size){
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
		printf("Error no twiddles computed for %ld\n", size);
		return;
	}

	for(int i=0; i<n; i++){
	    DSP_fft16x16_imre_cn((short*)w, size, (short*)in, (short*)out);
	    in += size;
	    out += size;
	}
}

void fft_2(Param n, Param p, Param N2, Param N1, char* ix, CplxSp* i0, CplxSp* i1, CplxSp* o0, CplxSp* o1){
#if VERBOSE
	printf("Execute fft_2\n");
#endif

	const int id	  = (*ix)*n;
	const int m = (id % (N2*(1<<(p))));
	const unsigned short incr = N1 / (1 << (p+1));

	for(int k=0; k<n; k++){
		unsigned short r = (m+k)*incr;

		float Br = i1[k].real*twi64k[r].real - i1[k].imag*twi64k[r].imag;
		float Bi = i1[k].real*twi64k[r].imag + i1[k].imag*twi64k[r].real;

		o0[k].real = i0[k].real + Br;
		o0[k].imag = i0[k].imag + Bi;

		o1[k].real = i0[k].real - Br;
		o1[k].imag = i0[k].imag - Bi;
	}
}


void CplxSp_to_Cplx16(Param n, CplxSp* in, Cplx16* out){
#if VERBOSE
	printf("Execute CplxSp_to_Cplx16\n");
#endif

	for(int i=0; i<n; i++){
		out[i].real = in[i].real;
		out[i].imag = in[i].imag;
	}
}

void Cplx16_to_CplxSp(Param n, Cplx16* in, CplxSp* out){
#if VERBOSE
	printf("Execute CplxSp_to_Cplx16\n");
#endif

	for(int i=0; i<n; i++){
		out[i].real = in[i].real;
		out[i].imag = in[i].imag;
	}
}
