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

#define VERBOSE 1

static CplxSp gen_twi32k[32*1024];
static CplxSp gen_twi16k[16*1024];
static CplxSp gen_twi8k [8*1024];
static CplxSp gen_twi4k [4*1024];
static CplxSp gen_twi2k [2*1024];
static CplxSp gen_twi1k [1024];

static unsigned char brev[64] = {
    0x0, 0x20, 0x10, 0x30, 0x8, 0x28, 0x18, 0x38,
    0x4, 0x24, 0x14, 0x34, 0xc, 0x2c, 0x1c, 0x3c,
    0x2, 0x22, 0x12, 0x32, 0xa, 0x2a, 0x1a, 0x3a,
    0x6, 0x26, 0x16, 0x36, 0xe, 0x2e, 0x1e, 0x3e,
    0x1, 0x21, 0x11, 0x31, 0x9, 0x29, 0x19, 0x39,
    0x5, 0x25, 0x15, 0x35, 0xd, 0x2d, 0x1d, 0x3d,
    0x3, 0x23, 0x13, 0x33, 0xb, 0x2b, 0x1b, 0x3b,
    0x7, 0x27, 0x17, 0x37, 0xf, 0x2f, 0x1f, 0x3f
};

static inline void tw_gen (CplxSp *w, int n);

void initActors(){
	tw_gen(gen_twi32k, 32*1024);
	tw_gen(gen_twi16k, 16*1024);
	tw_gen(gen_twi8k,   8*1024);
	tw_gen(gen_twi4k,   4*1024);
	tw_gen(gen_twi2k,   2*1024);
	tw_gen(gen_twi1k,     1024);

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

static inline void tw_gen (CplxSp *cplx_w, int n)
{
    int i, j, k;
    const double PI = 3.141592654;

    float* w = (float*) cplx_w;

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

void genIx(Param n, char* ixs){
#if VERBOSE
	printf("Execute genIx\n");
#endif
	for(int i=0; i<n; i++){
		ixs[i] = i;
	}
}

void cfg(char* in, Param* out){
#if VERBOSE
	printf("Execute cfg\n");
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

    // Compute SNR
    float snrVal = snr(in, data_out, size);
    printf("SNR %f dB\n", snrVal);
}

void T(Param N1, Param N2, CplxSp* in, CplxSp *out){
#if VERBOSE
	printf("Execute T\n");
#endif
	for(int n2=0; n2<N2; n2++){
		for(int n1=0; n1<N1; n1++){
//			printf("%d <- %d\n", n1*N2+n2, bitRev(n1, N1)+n2*N1);
			out[n1*N2+n2].real = in[bitRev(n1, N1)+n2*N1].real;
			out[n1*N2+n2].imag = in[bitRev(n1, N1)+n2*N1].imag;
		}
	}
}
void fft(Param size, Param n, CplxSp* in, CplxSp* out){
#if VERBOSE
	printf("Execute fft\n");
#endif

	int rad;
	int j = 0;
	for (int i = 0; i <= 31; i++)
		if ((size & (1 << i)) == 0)
			j++;
		else
			break;

	if (j % 2 == 0)
		rad = 4;
	else
		rad = 2;

	CplxSp* w;
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
		printf("Error no twiddles computed for %d\n", size);
		return;
	}

	for(int i=0; i<10; i++){
		printf("%2d: %8d + %8d i\n", i, in[i].real, in[i].imag);
	}

	for(int i=0; i<n; i++){
	    DSPF_sp_fftSPxSP_cn(size, (float*)in, (float*)w, (float*)out, brev, rad, 0, size);
	    in += size;
	    out += size;
	}
}

void fft_2(Param n, Param p, Param N2, Param N1, char* ix, CplxSp* i0, CplxSp* i1, CplxSp* o0, CplxSp* o1){
#if VERBOSE
	printf("Execute fft_2\n");
#endif

	int fftSize = N1*N2;
	int localSize = N2;
	int id = *ix % (1<<p);
	int increment = N1 / (1 << (p+1));

	for(int k=0; k<localSize; k++){
		int m = (id * localSize + k) * increment;

//		printf("m/q%d == %f\n", m, fftSize*((1.*id/(1 << (p+1))) + 1.*k/(localSize*(1<<(p+1)))));

		float Br = i1[k].real*cos(-2*M_PI*m/fftSize) - i1[k].imag*sin(-2*M_PI*m/fftSize);
		float Bi = i1[k].real*sin(-2*M_PI*m/fftSize) + i1[k].imag*cos(-2*M_PI*m/fftSize);

		o0[k].real = i0[k].real + Br;
		o0[k].imag = i0[k].imag + Bi;

		o1[k].real = i0[k].real - Br;
		o1[k].imag = i0[k].imag - Bi;
	}
}


void Switch(Param size, CplxSp* i0, CplxSp* i1, CplxSp* out){
#if VERBOSE
	printf("Execute Switch\n");
#endif

}

