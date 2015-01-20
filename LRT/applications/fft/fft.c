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


#define RESTRICT


#include "data_in.h"
#include "gen_twiddle_fft16x16.h"
#include <ti/dsplib/src/DSP_fft16x16/DSP_fft16x16.h>

#define PI 3.14159265358979323846

#include <math.h>

#include <stdio.h>
#define PRINT 1

#include <platform_types.h>

static float twiddles[2*N];

void initTwiddle(){
	int r, c;
    for(r=0; r<NR; r++){
        for(c=0; c<NC; c++){
        	int id = r*NC+c;
        	twiddles[2*id  ] = cos(-2*PI*r*c/N);
        	twiddles[2*id+1] = sin(-2*PI*r*c/N);
    	}
	}

}

void twiddleMult(const short * RESTRICT in, const int nc, const int nr, short* RESTRICT out){
	int r, c;
    for(r=0; r<NR; r++){
        for(c=0; c<NC; c++){
        	int id = r*NC+c;
			out[2*id  ] = in[2*id  ]*twiddles[2*id  ] - in[2*id+1]*twiddles[2*id+1];
            out[2*id+1] = in[2*id  ]*twiddles[2*id+1] + in[2*id+1]*twiddles[2*id  ];
    	}
	}
}


void transpose(UINT8* inputFIFOs[], UINT8* outputFIFOs[], UINT32 params[]){
	short* in = (short*)inputFIFOs[0];
	short* out = (short*)outputFIFOs[0];
	int nc = params[0];
	int nr = params[1];

	DSPF_sp_mat_trans((float*)in, nr, nc, (float*)out);
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

	short w[2*N];

    gen_twiddle_fft16x16(w, n);
    DSP_fft16x16(w, n, in, out);
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
