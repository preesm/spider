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
#include <c6x.h>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#define VERBOSE 0

#pragma DATA_SECTION(".twiddlesRed")
static float twi64k[2*3*32*1024];

void initActors2(){
	for(int i=0; i<32*1024; i++){
//		twi64k[2*i  ] = cos(-2*M_PI*i/(64*1024));
//		twi64k[2*i+1] = sin(-2*M_PI*i/(64*1024));
		twi64k[6*i  ] = -sin(-2*M_PI*i/(64*1024));
		twi64k[6*i+1] =  cos(-2*M_PI*i/(64*1024));
		twi64k[6*i+2] = -sin(-4*M_PI*i/(64*1024));
		twi64k[6*i+3] =  cos(-4*M_PI*i/(64*1024));
		twi64k[6*i+4] = -sin(-6*M_PI*i/(64*1024));
		twi64k[6*i+5] =  cos(-6*M_PI*i/(64*1024));
	}
}

void fftRadix4(
		Param NStep,
		Param fftSize,
		Param Step,
		float* restrict p_in0,
		float* restrict p_in1,
		float* restrict p_in2,
		float* restrict p_in3,
		char*  restrict ix,
		float* restrict p_out0,
		float* restrict p_out1,
		float* restrict p_out2,
		float* restrict p_out3){
#if VERBOSE
	printf("Execute fftRadix4 %d %d\n", Step, *ix);
#endif

	const int id	  = *ix;
	const int nStage = NStep;
	const int stage = Step;
	const int nVectors = 1<<(2*NStep);
	const unsigned short vectorSize = fftSize/nVectors;

	double const* restrict pd_in0 = (double*) p_in0;
	double const* restrict pd_in1 = (double*) p_in1;
	double const* restrict pd_in2 = (double*) p_in2;
	double const* restrict pd_in3 = (double*) p_in3;
	double const* restrict pd_twi = (double*) twi64k;
	double * restrict pd_out0 = (double*) p_out0;
	double * restrict pd_out1 = (double*) p_out1;
	double * restrict pd_out2 = (double*) p_out2;
	double * restrict pd_out3 = (double*) p_out3;

	/* Specify aligned input/output/twi for optimizations */
	_nassert((int) pd_in0 % 8 == 0); 	// input0 is 64-bit aligned
	_nassert((int) pd_in1 % 8 == 0); 	// input1 is 64-bit aligned
	_nassert((int) pd_in2 % 8 == 0); 	// input2 is 64-bit aligned
	_nassert((int) pd_in3 % 8 == 0); 	// input3 is 64-bit aligned
	_nassert((int) pd_twi % 8 == 0); 	// twiddles is 64-bit aligned
	_nassert((int) pd_out0 % 8 == 0); 	// output0 is 64-bit aligned
	_nassert((int) pd_out1 % 8 == 0); 	// output1 is 64-bit aligned
	_nassert((int) pd_out2 % 8 == 0); 	// output2 is 64-bit aligned
	_nassert((int) pd_out3 % 8 == 0); 	// output3 is 64-bit aligned

	const int m = (id % (1<<(2*stage)))*vectorSize;
	const unsigned short incr = 3*(1<<(2*(nStage-stage-1)));

#pragma MUST_ITERATE(8, ,8)

	for(unsigned short k=0; k<vectorSize; k++){
		unsigned short r = (m+k)*incr;

		__float2_t  in0  = _amem8_f2_const(&pd_in0[k]);
		__float2_t  in1  = _amem8_f2_const(&pd_in1[k]);
		__float2_t  in2  = _amem8_f2_const(&pd_in2[k]);
		__float2_t  in3  = _amem8_f2_const(&pd_in3[k]);

		__float2_t  twi2  = _amem8_f2_const(&pd_twi[r  ]);
		__float2_t  twi1  = _amem8_f2_const(&pd_twi[r+1]);
		__float2_t  twi3  = _amem8_f2_const(&pd_twi[r+2]);

		__float2_t  win1 =  _complex_mpysp(in1, twi1); 	/* win1 = in1 * twi1 */
		__float2_t  win2 =  _complex_mpysp(in2, twi2); 	/* win2 = in2 * twi2 */
		__float2_t  win3 =  _complex_mpysp(in3, twi3); 	/* win3 = in3 * twi3 */

		__float2_t a0 = _daddsp( in0, win1); 			/* a0 =  in0 + win1 */
		__float2_t s0 = _dsubsp( in0, win1); 			/* s0 =  in0 - win1 */
		__float2_t a1 = _daddsp(win2, win3); 			/* a1 = win2 + win3 */

		float s1_r_t = - _hif2(win2) + _hif2(win3);
		float s1_i_t = _lof2(win2) - _lof2(win3);
		__float2_t s1 = _ftof2(s1_i_t, s1_r_t); 		/* s1 = i*(win2 - win3) */

		_amem8_f2(&pd_out0[k]) = _daddsp(a0, a1); 		/* out0 = a0 + a1 */
		_amem8_f2(&pd_out1[k]) = _dsubsp(s0, s1); 		/* out1 = s0 - s1 */
		_amem8_f2(&pd_out2[k]) = _dsubsp(a0, a1); 		/* out2 = a0 - a1 */
		_amem8_f2(&pd_out3[k]) = _daddsp(s0, s1); 		/* out3 = s0 + s1 */
	}
}
