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

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#define VERBOSE 0

#pragma DATA_SECTION(".twiddles")
static float twi64k[2*64*1024];

void initActors2(){
	for(int i=0; i<64*1024; i++){
		twi64k[2*i  ] = cos(-2*M_PI*i/(64*1024));
		twi64k[2*i+1] = sin(-2*M_PI*i/(64*1024));
	}
}

void fftRadix4(
		Param NStep,
		Param fftSize,
		Param Step,
		float* p_in0,
		float* p_in1,
		float* p_in2,
		float* p_in3,
		char*  ix,
		float* p_out0,
		float* p_out1,
		float* p_out2,
		float* p_out3){
#if VERBOSE
	printf("Execute fftRadix4 %d %d\n", Step, *ix);
#endif

	int id	  = *ix;
	int nStage = NStep;
	int stage = Step;
	int nVectors = 1<<(2*NStep);
	int vectorSize = fftSize/nVectors;

	int m = (id % (1<<(2*stage)))*vectorSize;
	int incr = 1<<(2*(nStage-stage-1));

	for(int k=0; k<vectorSize; k++){
		float* in0  = &p_in0[2*k];
		float* in1  = &p_in1[2*k];
		float* in2  = &p_in2[2*k];
		float* in3  = &p_in3[2*k];

		int r = (k+m)*incr;

		/* Get the m/q twiddle factor */
		float* twi1  = &twi64k[2*((2*r)%(64*1024))];
		float* twi2  = &twi64k[2*((  r)%(64*1024))];
		float* twi3  = &twi64k[2*((3*r)%(64*1024))];

		float win1_r = in1[0]*twi1[0] - in1[1]*twi1[1];
		float win1_i = in1[0]*twi1[1] + in1[1]*twi1[0];

		float win2_r = in2[0]*twi2[0] - in2[1]*twi2[1];
		float win2_i = in2[0]*twi2[1] + in2[1]*twi2[0];

		float win3_r = in3[0]*twi3[0] - in3[1]*twi3[1];
		float win3_i = in3[0]*twi3[1] + in3[1]*twi3[0];

		float a0_r = in0[0] + win1_r;
		float a0_i = in0[1] + win1_i;
		float s0_r = in0[0] - win1_r;
		float s0_i = in0[1] - win1_i;
		float a1_r = win2_r + win3_r;
		float a1_i = win2_i + win3_i;
		float s1_r = -(win2_i - win3_i);
		float s1_i = win2_r - win3_r;

		p_out0[2*k  ] = a0_r + a1_r;
		p_out0[2*k+1] = a0_i + a1_i;
		p_out1[2*k  ] = s0_r - s1_r;
		p_out1[2*k+1] = s0_i - s1_i;
		p_out2[2*k  ] = a0_r - a1_r;
		p_out2[2*k+1] = a0_i - a1_i;
		p_out3[2*k  ] = s0_r + s1_r;
		p_out3[2*k+1] = s0_i + s1_i;
	}
}
