/**
 * *****************************************************************************
 * Copyright or © or Copr. IETR/INSA: Maxime Pelcat, Jean-François Nezan,
 * Karol Desnos, Julien Heulot, Clément Guy, Yaset Oliva Venegas
 *
 * [mpelcat,jnezan,kdesnos,jheulot,cguy,yoliva]@insa-rennes.fr
 *
 * This software is a computer program whose purpose is to prototype
 * parallel applications.
 *
 * This software is governed by the CeCILL-C license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL-C
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-C license and that you accept its terms.
 * ****************************************************************************
 */

#include <platform.h>
#include "top_fft.h"

#include "actors.h"

void Src(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void Snk(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void configFft(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void ordering(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void monoFFT(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void GenSwitchSel(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void selcfg(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void cfgFftStep(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void fft_radix2(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void genIx(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);

lrtFct top_fft_fcts[N_FCT_TOP_FFT] = {
	&Src,
	&Snk,
	&configFft,
	&ordering,
	&monoFFT,
	&GenSwitchSel,
	&selcfg,
	0,
	0,
	&cfgFftStep,
	&fft_radix2,
	&genIx,
};

void Src(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	src(
		/* fftSize */ (Param) inParams[0],
		/* out     */ (Cplx16*) outputFIFOs[0]
	);
}

void Snk(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	snk(
		/* fftSize */ (Param) inParams[0],
		/* in      */ (Cplx16*) inputFIFOs[0]
	);
}

void configFft(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	configFft(
		/* fftSize */ (Param) inParams[0],
		/* NStep   */ (Param*) &outParams[0]
	);
}

void ordering(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	ordering(
		/* fftSize */ (Param) inParams[0],
		/* NStep   */ (Param) inParams[1],
		/* in      */ (Cplx16*) inputFIFOs[0],
		/* out     */ (Cplx16*) outputFIFOs[0]
	);
}

void monoFFT(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	fft(
		/* NStep   */ (Param) inParams[0],
		/* fftSize */ (Param) inParams[1],
		/* in      */ (Cplx16*) inputFIFOs[0],
		/* out     */ (Cplx16*) outputFIFOs[0]
	);
}

void GenSwitchSel(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	genStepSwitch(
		/* NStep */ (Param) inParams[0],
		/* steps */ (char*) outputFIFOs[0],
		/* sels  */ (char*) outputFIFOs[1]
	);
}

void selcfg(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	selcfg(
		/* sel    */ (Param*) &outParams[0],
		/* sel_in */ (char*) inputFIFOs[0]
	);
}

void cfgFftStep(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	cfgFftStep(
		/* in   */ (char*) inputFIFOs[0],
		/* step */ (Param*) &outParams[0]
	);
}

void fft_radix2(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	fftRadix2(
		/* NStep   */ (Param) inParams[0],
		/* fftSize */ (Param) inParams[1],
		/* Step    */ (Param) inParams[2],
		/* in0     */ (Cplx16*) inputFIFOs[0],
		/* in1     */ (Cplx16*) inputFIFOs[1],
		/* ix      */ (char*) inputFIFOs[2],
		/* out0    */ (Cplx16*) outputFIFOs[0],
		/* out1    */ (Cplx16*) outputFIFOs[1]
	);
}

void genIx(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	genIx(
		/* NStep */ (Param) inParams[0],
		/* ixs   */ (char*) outputFIFOs[0]
	);
}

