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
#include <c6x.h>
#include <time.h>
#include <assert.h>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#define VERBOSE 0

#pragma DATA_SECTION(".twiddles")
static float twi64k[64*1024];

void initActors2(){
	for(int i=0; i<32*1024; i++){
//		twi64k[2*i  ] = cos(-2*M_PI*i/(64*1024));
//		twi64k[2*i+1] = sin(-2*M_PI*i/(64*1024));
		twi64k[2*i  ] = -sin(-2*M_PI*i/(64*1024));
		twi64k[2*i+1] =  cos(-2*M_PI*i/(64*1024));
	}
}

void fftRadix2(
		Param NStep,
		Param fftSize,
		Param Step,
		float* restrict p_in0,
		float* restrict p_in1,
		char*  ix,
		float* restrict p_out0,
		float* restrict p_out1){
#if VERBOSE
	printf("Execute fftRadix2\n");
#endif

	const int id	  = *ix;
	const int nStage = NStep;
	const int stage = Step;
	const int nVectors = 1<<(NStep);
	const unsigned short vectorSize = fftSize/nVectors;

	double const* restrict pd_in0 = (double*) p_in0;
	double const* restrict pd_in1 = (double*) p_in1;
	double const* restrict pd_twi = (double*) twi64k;
	double * restrict pd_out0 = (double*) p_out0;
	double * restrict pd_out1 = (double*) p_out1;

	/* Specify aligned input/output/twi for optimizations */
	_nassert((int) pd_in0 % 8 == 0); 	// input0 is 64-bit aligned
	_nassert((int) pd_in1 % 8 == 0); 	// input1 is 64-bit aligned
	_nassert((int) pd_twi % 8 == 0); 	// twiddles is 64-bit aligned
	_nassert((int) pd_out0 % 8 == 0); 	// output0 is 64-bit aligned
	_nassert((int) pd_out1 % 8 == 0); 	// output1 is 64-bit aligned

	const int m = (id % (1<<(stage)))*vectorSize;
	const unsigned short incr = (1<<((nStage-stage-1)));

#pragma MUST_ITERATE(8, ,8)
	for(int k=0; k<vectorSize; k++){
		unsigned short r = (m+k)*incr;

		__float2_t  v_in0  = _amem8_f2_const(&pd_in0[k]);
		__float2_t  v_in1  = _amem8_f2_const(&pd_in1[k]);

		/* Get the m/q twiddle factor */
		__float2_t  v_twi  = _amem8_f2_const(&pd_twi[r]);

		/* Execute:
		 * 		out0 = in0 + in1*twi
		 * 		out1 = in0 - in1*twi
		 * 	with out0, out1, in0, in1 and twi complex floating numbers
		 * 		[even]: real part and [odd]: imag part
		 */
		__float2_t  v_in1Twi =  _complex_mpysp(v_in1, v_twi);
//		__float2_t  v_in1Twi_rev = _ftod(_lof(v_in1Twi), -_hif(v_in1Twi));

		_amem8_f2(&pd_out0[k]) = _daddsp(v_in0, v_in1Twi);
		_amem8_f2(&pd_out1[k]) = _dsubsp(v_in0, v_in1Twi);

	}
}
