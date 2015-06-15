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
#include "edma_bitrev.h"

extern "C"{
#include "ti/dsplib/src/DSP_fft16x16_imre/DSP_fft16x16_imre.h"
#include "gen_twiddle_fft16x16_imre.h"

#include <stdint.h>
#include <ti/csl/csl_cache.h>
#include <ti/csl/csl_cacheAux.h>
int fftc_send (int fftc_ix, Cplx16* in, Cplx16* out, int fftSize, int numBlocks);
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
static Cplx16 twi64k[64*1024];

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

#pragma DATA_SECTION(".twiddles")
static Cplx16 gen_twi512 [512];

#pragma DATA_SECTION(".twiddles")
static Cplx16 gen_twi256 [256];

#pragma DATA_SECTION(".twiddles")
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

	void* cpak0_global_addr = (void*)0x10800000; //location of data in the memory (L2 of DSP0)
	void* msmc_ptr = (void*) 0xc050c00; // /!\ This has been statically implemented.
	if(edmabr_init(cpak0_global_addr, msmc_ptr, 4, 4096, 1, 64, 4, 0, 0) != 0) {
		printf("Error while initializing the EDMA.");
	}
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

void src(Param size, Cplx16 *out){
#if VERBOSE
	printf("Execute Src\n");
#endif

	int res = edmabr_cpy_jumpaddr_chained_bitreverse();
	if (res != 0)
			printf("The EDMA copy didn't succeeded.");
}

void snk(Param size, Cplx16 *in){
#if VERBOSE
	printf("Execute Snk\n");
#endif
	printf("DSP should not execute Snk\n");
}

void T(Param N1, Param N2, Cplx16* in, Cplx16 *out){
#if VERBOSE
	printf("Execute T\n");
#endif
	//the SRC actor on the DSP is already doing the T function
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
	case    512:
		w = gen_twi512;
		break;
	case    256:
		w = gen_twi256;
		break;
	case    128:
		w = gen_twi128;
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

void fft_2(Param n, Param p, Param N2, Param N1, char* ix, Cplx16* i0, Cplx16* i1, Cplx16* o0, Cplx16* o1){
#if VERBOSE
	printf("Execute fft_2\n");
#endif

	int k;
  const int id          = (*ix)*n;
  const int m = (id % (N2*(1<<(p))));
  const unsigned short incr = N1 / (1 << (p+1));
  int64_t *ptrIn0;
  int64_t *ptrIn1;
  int64_t *ptr0ut0;
  int64_t *ptrOut1;
  int64_t input0, input1, twiddles;

  __x128_t B32;
  int64_t B16;
  int64_t B16_hi;
  int64_t B16_lo;
  ptrIn0 = (int64_t *)i0;
  ptrIn1 = (int64_t *)i1;
  ptr0ut0 = (int64_t *)o0;
  ptrOut1 = (int64_t *)o1;

  for(k=0; k<n; k+=2){
			   input1 = _amem8(ptrIn1++);
			   twiddles = _itoll(*(int32_t *)&twi64k[(m+k+1)*incr],*(int32_t *)&twi64k[(m+k)*incr]) ;


			   B32 = _dcmpy(twiddles, input1);
			   B16_hi = _dshr(_hi128(B32), 15);
			   B16_lo = _dshr(_lo128(B32), 15);
			   B16 = _itoll(_pack2(_hill(B16_hi), _loll(B16_hi)), _pack2(_hill(B16_lo), _loll(B16_lo)));
			   input0 = _amem8(ptrIn0++);
			   _amem8(ptr0ut0++) = _dshr2(_dadd2(input0, B16), 1);
			   _amem8(ptrOut1++) = _dshr2(_dsub2(input0, B16), 1);

  }
}
