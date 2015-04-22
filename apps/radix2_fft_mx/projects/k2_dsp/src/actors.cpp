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

extern "C"{
#include "ti/dsplib/src/DSP_fft16x16_imre/DSP_fft16x16_imre.h"
#include "gen_twiddle_fft16x16_imre.h"
#include "edma.h"
}

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <c6x.h>
#include <time.h>
#include <assert.h>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#define VERBOSE 0

#pragma DATA_SECTION(".twiddles")
static CplxSp twi64k[64*1024];

#pragma DATA_SECTION(".twiddles")
static Cplx16 gen_twi32k[32*1024];

#pragma DATA_SECTION(".twiddles")
static Cplx16 gen_twi16k[16*1024];

#pragma DATA_SECTION(".twiddles")
static Cplx16 gen_twi8k [8*1024];

#pragma DATA_SECTION(".twiddles")
static Cplx16 gen_twi4k [4*1024];

#pragma DATA_SECTION(".twiddles")
static Cplx16 gen_twi2k [2*1024];

#pragma DATA_SECTION(".twiddles")
static Cplx16 gen_twi1k [1024];

void initActors2();

void initActors(){
	edma_init();

	gen_twiddle_fft16x16_imre((short*)gen_twi32k, 32*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi16k, 16*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi8k,   8*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi4k,   4*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi2k,   2*1024);
	gen_twiddle_fft16x16_imre((short*)gen_twi1k,     1024);

	for(int i=0; i<32*1024; i++){
		twi64k[i].real = -sin(-2*M_PI*i/(64*1024));
		twi64k[i].imag = cos(-2*M_PI*i/(64*1024));
	}
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
	printf("DSP should not execute cfgFFT\n");
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

void src(Param size, CplxSp *out){
#if VERBOSE
	printf("Execute Src\n");
#endif
	printf("DSP should not execute Src\n");
}

void snk(Param size, CplxSp *in){
#if VERBOSE
	printf("Execute Snk\n");
#endif
	printf("DSP should not execute Snk\n");
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
	    DSP_fft16x16_imre((short*)w, size, (short*)in, (short*)out);
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

	CplxSp const* restrict pd_in0 = i0;
	CplxSp const* restrict pd_in1 = i1;
	CplxSp const* restrict pd_twi = twi64k;
	CplxSp * restrict pd_out0 = o0;
	CplxSp * restrict pd_out1 = o1;

	#pragma MUST_ITERATE(8, ,8)
	for(int k=0; k<n; k++){
		unsigned short r = (m+k)*incr;

		__float2_t  v_in0  = _amem8_f2_const(&pd_in0[k]);
		__float2_t  v_in1  = _amem8_f2_const(&pd_in1[k]);

		/* Get the corresponding twiddle factor */
		__float2_t  v_twi  = _amem8_f2_const(&pd_twi[r]);

		/* Execute:
		 * 		out0 = in0 + in1*twi
		 * 		out1 = in0 - in1*twi
		 * 	with out0, out1, in0, in1 and twi complex floating numbers
		 * 		[even]: real part and [odd]: imag part
		 */
		__float2_t  v_in1Twi =  _complex_mpysp(v_in1, v_twi);

		_amem8_f2(&pd_out0[k]) = _daddsp(v_in0, v_in1Twi);
		_amem8_f2(&pd_out1[k]) = _dsubsp(v_in0, v_in1Twi);
	}
}


void CplxSp_to_Cplx16(Param n, CplxSp* in, Cplx16* out){
#if VERBOSE
	printf("Execute CplxSp_to_Cplx16\n");
#endif

	CplxSp const* restrict pd_in = in;
	Cplx16 * restrict pd_out = out;

	#pragma MUST_ITERATE(8,,8)
	for(int i=0; i<n; i++){
		(*pd_out).real = (*pd_in).real;
		(*pd_out).imag = (*pd_in).imag;
		pd_in++;
		pd_out++;
	}
}

void Cplx16_to_CplxSp(Param n, Cplx16* in, CplxSp* out){
#if VERBOSE
	printf("Execute CplxSp_to_Cplx16\n");
#endif

	Cplx16 const* restrict pd_in = in;
	CplxSp * restrict pd_out = out;

	#pragma MUST_ITERATE(8,,8)
	for(int i=0; i<n; i++){
		(*pd_out).real = (*pd_in).real;
		(*pd_out).imag = (*pd_in).imag;
		pd_in++;
		pd_out++;
	}
}
