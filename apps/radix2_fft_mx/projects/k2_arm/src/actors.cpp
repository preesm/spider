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
#include <arm_neon.h>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#define VERBOSE 1

unsigned char brev[64] = {
    0x0, 0x20, 0x10, 0x30, 0x8, 0x28, 0x18, 0x38,
    0x4, 0x24, 0x14, 0x34, 0xc, 0x2c, 0x1c, 0x3c,
    0x2, 0x22, 0x12, 0x32, 0xa, 0x2a, 0x1a, 0x3a,
    0x6, 0x26, 0x16, 0x36, 0xe, 0x2e, 0x1e, 0x3e,
    0x1, 0x21, 0x11, 0x31, 0x9, 0x29, 0x19, 0x39,
    0x5, 0x25, 0x15, 0x35, 0xd, 0x2d, 0x1d, 0x3d,
    0x3, 0x23, 0x13, 0x33, 0xb, 0x2b, 0x1b, 0x3b,
    0x7, 0x27, 0x17, 0x37, 0xf, 0x2f, 0x1f, 0x3f
};

static float twi64k[2*64*1024];
static float twi32k[2*64*1024];
static float twi16k[2*64*1024];
static float twi8k[2*64*1024];
static float twi4k[2*64*1024];

static float gen_twi32k[2*32*1024];
static float gen_twi16k[2*16*1024];
static float gen_twi8k [2*8*1024];
static float gen_twi4k [2*4*1024];
static float gen_twi2k [2*2*1024];
static float gen_twi1k [2*1024];

void tw_gen (float *w, int n);

void initActors(){
	for(int i=0; i<64*1024; i++){
		twi64k[2*i  ] = cos(-2*M_PI*i/(64*1024));
		twi64k[2*i+1] = sin(-2*M_PI*i/(64*1024));
	}
	for(int i=0; i<64*1024; i++){
		int new_i = i%(32*1024);
		twi32k[2*i  ] = cos(-2*M_PI*new_i/(32*1024));
		twi32k[2*i+1] = sin(-2*M_PI*new_i/(32*1024));
	}
	for(int i=0; i<64*1024; i++){
		int new_i = i%(16*1024);
		twi16k[2*i  ] = cos(-2*M_PI*new_i/(16*1024));
		twi16k[2*i+1] = sin(-2*M_PI*new_i/(16*1024));
	}
	for(int i=0; i<64*1024; i++){
		int new_i = i%(8*1024);
		twi8k[2*i  ] = cos(-2*M_PI*new_i/(8*1024));
		twi8k[2*i+1] = sin(-2*M_PI*new_i/(8*1024));
	}
	for(int i=0; i<64*1024; i++){
		int new_i = i%(32*1024);
		twi4k[2*i  ] = cos(-2*M_PI*new_i/(4*1024));
		twi4k[2*i+1] = sin(-2*M_PI*new_i/(4*1024));
	}
	tw_gen(gen_twi32k, 32*1024);
	tw_gen(gen_twi16k, 16*1024);
	tw_gen(gen_twi8k,   8*1024);
	tw_gen(gen_twi4k,   4*1024);
	tw_gen(gen_twi2k,   2*1024);
	tw_gen(gen_twi1k,     1024);
}

/* Function for generating Specialized sequence of twiddle factors */
void tw_gen (float *w, int n)
{
    int i, j, k;
    const double PI = 3.141592654;

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


float norm2_cplx(float *x){
	float real = x[0];
	float imag = x[1];
	return sqrt(real*real + imag*imag);
}

float rms(float *x, int n){
	float sum = 0;
	for(int i=0; i<n; i++){
		float val = norm2_cplx(x + 2*i);
		sum += val*val;
	}
	return sqrt(sum/n);
}

float snr(float* sig, float* ref, int n){
	float diff[2*N_DATA];
	int i;
	for(i=0; i<n; i++){
		diff[2*i]   = sig[2*i]   - ref[2*i];
		diff[2*i+1] = sig[2*i+1] - ref[2*i+1];
	}

//	printf("RMS Signal : %f\n", rms(sig, n));
//	printf("RMS Noise : %f\n", rms(diff, n));

	return 20*log(rms(sig, n)/rms(diff, n));
}

inline int bitRev(short v, int logN){
	short r=0;
	for(int n=0; n<logN; n++){
		r = (r<<1) + (v & 0x1);
		v = v>>1;
	}
	return r;
}

void genStepSwitch(Param NStep, char* steps){
#if VERBOSE
	printf("Execute genStepSwitch\n");
#endif
	steps[0] = 0;
	for(int i=1; i<NStep; i++){
		steps[i] = i;
	}
}

void cfgFftStep(char* in, Param* step){
#if VERBOSE
	printf("Execute cfgFftStep\n");
#endif
	*step = *in;
}


void src(Param fftSize, float *out){
#if VERBOSE
	printf("Execute Src\n");
#endif
	if(N_DATA != fftSize)
		printf("Bad size, bad SNR expected\n");

    memcpy(out, data_in, fftSize*2*sizeof(float));
}

void snk(Param fftSize, float *in){
#if VERBOSE
	printf("Execute Snk\n");
#endif

    // Compute SNR
    float snrVal = snr(in, data_out, fftSize);
    printf("SNR %f dB\n", snrVal);
}

void fftRadix2(
		Param NStep,
		Param fftSize,
		Param Step,
		float* __restrict in0,
		float* __restrict in1,
		char*  ix,
		float* __restrict out0,
		float* __restrict out1){
#if VERBOSE
	printf("Execute fftRadix2\n");
#endif

	int id	  = *ix;
	int n	  = fftSize;
	int p	  = 1<<NStep;
	int log2n = log2(fftSize);
	int log2p = NStep;
	int mask = (p-1) ^ ((1<<Step)-1);
	int proc = ((id & mask)<<1) + ( id & (~mask & (p-1)) );
//	printf("mixFFT : n%d p%d step%d id%d proc%d mask %#x %#x\n", n, p, step, id, proc, mask, ~mask & (p-1));

	int q = 1 << (int)(Step+1+log2n-log2p);
	const int ratio = 64*1024/q;

	const int m_start = proc*n/p;
	int m = m_start;

	const int nIter = n/p;
	const int qMask = q-1;

	float* __restrict__ twi_restrict;
	switch(q){
	case 64*1024:
		twi_restrict = twi64k;
		break;
	case 32*1024:
		twi_restrict = twi32k;
		break;
	case 16*1024:
		twi_restrict = twi16k;
		break;
	case  8*1024:
		twi_restrict = twi8k;
		break;
	case  4*1024:
		twi_restrict = twi4k;
		break;
	default:
		printf("Error no twiddles computed for %d\n", q);
		return;
	}

	float* __restrict__ in0_align = (float*)__builtin_assume_aligned (in0, 8);
	float* __restrict__ in1_align = (float*)__builtin_assume_aligned (in1, 8);
	float* __restrict__ twi_align = (float*)__builtin_assume_aligned (twi_restrict, 8);
	float* __restrict__ out0_align = (float*)__builtin_assume_aligned (out0, 8);
	float* __restrict__ out1_align = (float*)__builtin_assume_aligned (out1, 8);

	for(int k=0; k<nIter; k++){
//		short m_local = m & qMask;
//		printf("m %d m_local %d q %d\n", m, m_local, q);
//		float* twi = twi_align + 2*m_local;

		float k_r = in0_align[2*k  ];
		float k_i = in0_align[2*k+1];

		float l_r = in1_align[2*k  ];
		float l_i = in1_align[2*k+1];

		float z_r = twi_align[2*m  ];
		float z_i = twi_align[2*m+1];

		float l2_r = l_r*z_r - l_i*z_i;
		float l2_i = l_i*z_r + l_r*z_i;

		out0_align[2*k  ] = k_r + l2_r;
		out0_align[2*k+1] = k_i + l2_i;

		out1_align[2*k  ] = k_r - l2_r;
		out1_align[2*k+1] = k_i - l2_i;

		m++;

//		float32x2_t v_in0 = vld1_f32(&in0[2*k]);
//		float32x2_t v_in1 = vld1_f32(&in1[2*k]);
//		float32x2_t v_twi = vld1_f32(&twi64k[2*ratio*(m & qMask)]);
//
//		// Get the real values of a | v1_re | v1_re |
//		float32x2_t v1r2 = vdup_lane_f32(v_in1, 0);
//		// Get the imag values of a | v1_im | v1_im |
//		float32x2_t v1i2 = vdup_lane_f32(v_in1, 1);
//		// Multiply the real a with b
//		float32x2_t vrrri = vmul_f32(v1r2, v_twi);
//		// Multiply the imag a with b
//		float32x2_t virii = vmul_f32(v1i2, v_twi);
//
//		uint32x2_t xorVal = {0x00000000, 0x80000000};
//
//		// Conjugate v2
//		float32x2_t virmii = vreinterpret_f32_u32(veor_u32(vreinterpret_u32_f32(virii), xorVal));
//
//		// Swap real/imag elements in v2.
//		float32_t ir = vget_lane_f32(virmii, 0);
//		float32_t mii = vget_lane_f32(virmii, 1);
//		float32x2_t vmiiir = {mii, ir};
//
//		// Add and return the result
//		float32x2_t v1twi = vadd_f32(vrrri, vmiiir);
//
//		float32x2_t vOut0 = vadd_f32(v_in0, v1twi);
//		float32x2_t vOut1 = vsub_f32(v_in0, v1twi);
//
//		vst1_f32(&out0[2*k], vOut0);
//		vst1_f32(&out1[2*k], vOut1);
//
//		m++;

//		int k0 = 2*k;
//		int k1 = 2*(k+1);
//		int twi0 = 2*ratio*(m & qMask);
//		int twi1 = 2*ratio*((m+1) & qMask);
//
//		float32x2_t v_in0_0 = vld1_f32(&in0[k0]);		// in0 of 1st data
//		float32x2_t v_in1_0 = vld1_f32(&in1[k0]);		// in1 of 1st data
//		float32x2_t v_twi_0 = vld1_f32(&twi64k[twi0]);	// twi of 1st data
//
//		float32x2_t v_in0_1 = vld1_f32(&in0[k1]);		// in0 of 2nd data
//		float32x2_t v_in1_1 = vld1_f32(&in1[k1]);		// in1 of 2nd data
//		float32x2_t v_twi_1 = vld1_f32(&twi64k[twi1]);	// twi of 2nd data
//
//		/** Pack Data in 128bits vectors **/
//		// Get the values of in0:
//		// 		in0_0.r in0_0.i in0_1.r in0_1.i
//		float32x4_t vin0 = vcombine_f32(
//				v_in0_0,
//				v_in0_1);
//		// Get the real values of in1:
//		// 		in1_0.r in1_0.r in1_1.r in1_1.r
//		float32x4_t vin1r = vcombine_f32(
//				vdup_lane_f32(v_in1_0, 0),
//				vdup_lane_f32(v_in1_1, 0));
//		// Get the imag values of in1:
//		//		in1_0.i in1_0.i in1_1.i in1_1.i
//		float32x4_t vin1i = vcombine_f32(
//				vdup_lane_f32(v_in1_0, 1),
//				vdup_lane_f32(v_in1_1, 1));
//		// Get the twiddles factor in one vector:
//		//		twi_0.r twi_0.i twi_1.r twi_1.i
//		float32x4_t vtwi= vcombine_f32(
//				v_twi_0,
//				v_twi_1);
//
//		/** Complex multiplication **/
//		// Multiply real of in1 with twiddle
//		// 		in1_0.r*twi_0.r | in1_0.r*twi_0.i | in1_1.r*twi_1.r | in1_1.r*twi_1.i
//		float32x4_t mul0 = vmulq_f32(vin1r, vtwi);
//		// Multiply imag of in1 with twiddle
//		// 		in1_0.i*twi_0.r | in1_0.i*twi_0.i | in1_1.i*twi_1.r | in1_1.i*twi_1.i
//		float32x4_t mul1 = vmulq_f32(vin1i, vtwi);
//		// Conjugate imag*imag factor
////		float32x2_t toNeg = {
////				vget_lane_f32(vget_low_f32(mul1),1),
////				vget_lane_f32(vget_high_f32 (mul1),1)
////		};
////		float32x2_t neg = vneg_f32(toNeg);
////
////		// 		in1_0.i*twi_0.r | -in1_0.i*twi_0.i | in1_1.i*twi_1.r | -in1_1.i*twi_1.i
////		float32x2_t resMul0 = {vget_lane_f32(vget_low_f32 (mul1),0), vget_lane_f32(neg,0)};
////		float32x2_t resMul1 = {vget_lane_f32(vget_high_f32(mul1),0), vget_lane_f32(neg,1)};
////
////		float32x4_t resMul = vcombine_f32(
////				resMul0,
////				resMul1
////		);
//
//
//		uint32x4_t xorVal = {0x00000000, 0x80000000, 0x00000000, 0x80000000};
//		float32x4_t resMul = vreinterpretq_f32_u32(veorq_u32(vreinterpretq_u32_f32(mul1), xorVal));
//
//		// Swap real/imag elements in mul1.
//		// 		-in1_0.i*twi_0.i | in1_0.i*twi_0.r | -in1_1.i*twi_1.i | in1_1.i*twi_1.r
//		float32x4_t mul1_inv = vcombine_f32(
//				vrev64_f32(vget_low_f32 (resMul)),
//				vrev64_f32(vget_high_f32(resMul))
//		);
//
//		// Add mul0 and mul1_inv
//		// 		in1_0.r*twi_0.r - in1_0.i*twi_0.i
//		// 		in1_0.r*twi_0.i + in1_0.i*twi_0.r
//		//		in1_1.r*twi_1.r - in1_1.i*twi_1.i
//		//		in1_1.r*twi_1.i + in1_1.i*twi_1.r
//		float32x4_t vmul = vaddq_f32(mul0, mul1_inv);
//
//		/** Compute out0 = in0+vmul
//		 * 		and out1 = in0-vmul */
//		float32x4_t vOut0 = vaddq_f32(vin0, vmul);
//		float32x4_t vOut1 = vsubq_f32(vin0, vmul);
//
//		/** Store result */
//		vst1_f32(&out0[k0], vget_low_f32(vOut0));		// out0 of 1st data
//		vst1_f32(&out1[k0], vget_low_f32(vOut1));		// out1 of 1st data
//
//		vst1_f32(&out0[k1], vget_high_f32(vOut0));		// out0 of 2nd data
//		vst1_f32(&out1[k1], vget_high_f32(vOut1));		// out1 of 2nd data
//
//		m+=2;
	}
}

void ordering(Param fftSize, Param NStep, float* in, float *out){
#if VERBOSE
	printf("Execute ordering\n");
#endif
	int P = 1<<NStep;
	for(int proc=0; proc<P; proc++){
		int in_offset  = 2*bitRev(proc, NStep);
		int out_offset = 2*proc*fftSize/P;
		for(int k=0; k<fftSize/P; k++){
			out[out_offset + 2*k  ] = in[in_offset + 2*k*P  ];
			out[out_offset + 2*k+1] = in[in_offset + 2*k*P+1];
		}
	}


}

void fft(
		Param NStep,
		Param fftSize,
		float* in,
		float* out){

//	static float w[2*N_DATA];

#if VERBOSE
	printf("Execute fft\n");
#endif

	int localFFTSize = fftSize/(1<<NStep);

	int rad;
	int j = 0;
	for (int i = 0; i <= 31; i++)
		if ((localFFTSize & (1 << i)) == 0)
			j++;
		else
			break;

	if (j % 2 == 0)
		rad = 4;
	else
		rad = 2;

	float* w;
	switch(localFFTSize){
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
		printf("Error no twiddles computed for %d\n", localFFTSize);
		return;
	}

    DSPF_sp_fftSPxSP_cn(localFFTSize, in, w, out, brev, rad, 0, localFFTSize);
}

void configFft(Param fftSize, Param* NStep){
#if VERBOSE
	printf("Execute configFft\n");
#endif

	static int nstep = 1;

	*NStep = nstep++;
}

void selcfg(Param *out_Sel, char* in_Sel){
#if VERBOSE
	printf("Execute selcfg\n");
#endif

	// Set parameter's value.
	*out_Sel = in_Sel[0];
}

void genIx(Param NStep, char* ixs){
#if VERBOSE
	printf("Execute genIx\n");
#endif

	int maxIds = 1<<(NStep-1);
	for(int i=0; i<maxIds; i++){
		ixs[i] = i;
	}
}
