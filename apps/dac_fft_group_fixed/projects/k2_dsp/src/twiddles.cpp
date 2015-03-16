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

#include <lrt.h>
#include <stdio.h>
#include <assert.h>
#include <c6x.h>
#include <math.h>

#include "actors.h"

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

#pragma DATA_SECTION(".twiddles")
static short twi64k[2*64*1024];

static short d2s(double d)
{
    d = floor(0.5 + d);  // Explicit rounding to integer //
    if (d >=  32767.0) return  32767;
    if (d <= -32768.0) return -32768;
    return (short)d;
}

int gen_twiddle_fft16x16(short *w, int n)
{
    int i, j, k;
    double M = 32767.5;

    for (j = 1, k = 0; j < n >> 2; j = j << 2) {
        for (i = 0; i < n >> 2; i += j << 1) {
            w[k +  3] =  d2s(M * -sin(2.0 * M_PI * (i + j) / n));
            w[k +  2] =  d2s(M * cos(2.0 * M_PI * (i + j) / n));
            w[k +  1] =  d2s(M * -sin(2.0 * M_PI * (i    ) / n));
            w[k +  0] =  d2s(M * cos(2.0 * M_PI * (i    ) / n));

            k += 4;
        }
    }
    w[k + 3] =  w[k - 1];
    w[k + 2] =  w[k - 2];
    w[k + 1] =  w[k - 3];
    w[k + 0] =  w[k - 4];
    k += 4;

    return k;
}

void initActors2(){
    double M = 32767.5;
//	for(int i=0; i<64*1024; i++){
//		twi64k[2*i  ] = d2s(-M*sin(-2*M_PI*i/(64*1024))/2);
//		twi64k[2*i+1] = d2s(M*cos(-2*M_PI*i/(64*1024))/2);
//	}

    for(int r=0; r<256; r++){
    	for(int c=0; c<256; c++){
    		twi64k[2*(r*256+c)  ] =  d2s(-M*sin(-2*M_PI*r*c/(64*1024))/2);
    		twi64k[2*(r*256+c)+1] =  d2s( M*cos(-2*M_PI*r*c/(64*1024))/2);
    	}
    }

//    gen_twiddle_fft16x16(twi64k, 64*1024);
}

void twiddles(Param size, Param n, int* ix, short* in, short* out){
#if VERBOSE
	printf("Execute twiddles\n");
#endif
	int r = (*ix)*n;

	typedef unsigned long 		complex_16;
	typedef unsigned long long 	complex_16_2;
	typedef unsigned long long 	complex_32;
	typedef int32x4_t 			complex_32_2;

	complex_16_2 const* restrict pl_in  = (complex_16_2*) in;
	complex_16_2 const* restrict pl_w = (complex_16_2*) (twi64k+2*r*size);
	complex_16_2 * restrict pl_out = (complex_16_2*) out;

	/* Specify aligned input/output/twi for optimizations */
	_nassert((int) pl_in  % 8 == 0); 	// input  is 64-bit aligned
	_nassert((int) pl_out % 8 == 0); 	// output is 64-bit aligned
	_nassert((int) pl_w   % 8 == 0); 	// twiddles is 64-bit aligned

	#pragma MUST_ITERATE(8,,8)
	for(int i=0; i<n*size/2; i++)
		*(pl_out++) = _dcmpyr1(*(pl_in++), *(pl_w++));
}
