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

#include "fft.h"

#include <execution/execution.h>

//#ifdef DSP
//#define RESTRICT restrict
//#else
#define RESTRICT __restrict__
//#endif


extern "C"{
	#include "data_in.h"
	#include "gen_twiddle_fft16x16.h"
	#include "DSP_fft16x16_cn.h"
	#include "DSPF_sp_mat_trans_cn.h"
}

#include <math.h>

#include <stdio.h>
#define PRINT 1

#include <platform_types.h>

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
	short diff[2*n];
	int i;
	for(i=0; i<n; i++){
		diff[2*i]   = sig[2*i]   - ref[2*i];
		diff[2*i+1] = sig[2*i+1] - ref[2*i+1];
	}
	return 20*log((float)normVect(sig, n)/(float)normVect(diff, n));
}

void twiddleMult(const short * RESTRICT in, const int nc, const int nr, short* RESTRICT out){
	int r, c;
    for(r=0; r<NR; r++){
        for(c=0; c<NC; c++){
        	int id = r*NC+c;
        	short a = in[2*id  ];
        	short b = in[2*id+1];
        	float co = cos(-2*M_PI*r*c/N);
        	float si = sin(-2*M_PI*r*c/N);
			out[2*id  ] = a*co-b*si;
            out[2*id+1] = a*si+b*co;
    	}
	}
}

void src(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	short* out = (short*)outputFIFOs[0];
	int nc = params[0];
	int nr = params[1];

	memcpy(out, data_in, 2*nc*nr*sizeof(short));
}

void snk(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	short* in = (short*)inputFIFOs[0];
	int nc = params[0];
	int nr = params[1];

    float snrVal = snr(in, data_out, nc*nr);
    printf("Method 2: SNR %f dB\n", snrVal);
}

void transpose(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	short* in = (short*)inputFIFOs[0];
	short* out = (short*)outputFIFOs[0];
	int nc = params[0];
	int nr = params[1];

	int i, j;
	for (i = 0; i < nc; i++) {
		for (j = 0; j < nr; j++) {
			out[2*(i*nr + j)]   = in[2*(j*nr + i)];
			out[2*(i*nr + j)+1] = in[2*(j*nr + i)+1];
		}
	}

//	DSPF_sp_mat_trans_cn((float*)in, nr, nc, (float*)out);
}

void scale(
    short *   x,          /* Input matrix  */
    short           n       /* FFT Size   */
	){
	int i;
    int scaling = log(n)/log(2) - ceil(log(n)/log(4)) - 1;
    for(i=0; i<2*n; i++){
    	x[i] = x[i] << scaling;
    }
}

void fft(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	short* in = (short*)inputFIFOs[0];
	short* out = (short*)outputFIFOs[0];
	int n = params[0];

	short w[2*n];

    gen_twiddle_fft16x16(w, n);
    DSP_fft16x16_cn(w, n, in, out);
    scale(out, n);
}

void twiddle(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	short* in = (short*)inputFIFOs[0];
	short* out = (short*)outputFIFOs[0];
	int nc = params[0];
	int nr = params[1];

	twiddleMult(in, nc, nr, out);

}

void null(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
}
