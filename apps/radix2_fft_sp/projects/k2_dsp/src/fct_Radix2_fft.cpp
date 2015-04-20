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

#include <lrt.h>
#include "Radix2_fft.h"

#include "actors.h"

void Radix2_fft_src(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void Radix2_fft_T(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void Radix2_fft_DFT_N2(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void Radix2_fft_snk(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void Radix2_fft_cfgFFT(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void DFT_Radix2_genIx(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void Radix2_Stage_DFT_2(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void Radix2_Stage_cfg(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void Radix2_Stage_genIx(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);

lrtFct radix2_fft_fcts[N_FCT_RADIX2_FFT] = {
	&Radix2_fft_src,
	&Radix2_fft_T,
	&Radix2_fft_DFT_N2,
	&Radix2_fft_snk,
	&Radix2_fft_cfgFFT,
	&DFT_Radix2_genIx,
	&Radix2_Stage_DFT_2,
	&Radix2_Stage_cfg,
	&Radix2_Stage_genIx,
};

void Radix2_fft_src(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	src(
		/* size */ (Param) inParams[0],
		/* out  */ (CplxSp*) outputFIFOs[0]
	);
}

void Radix2_fft_T(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	T(
		/* N1  */ (Param) inParams[0],
		/* N2  */ (Param) inParams[1],
		/* in  */ (CplxSp*) inputFIFOs[0],
		/* out */ (CplxSp*) outputFIFOs[0]
	);
}

void Radix2_fft_DFT_N2(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	fft(
		/* size */ (Param) inParams[0],
		/* n    */ (Param) inParams[1],
		/* in   */ (CplxSp*) inputFIFOs[0],
		/* out  */ (CplxSp*) outputFIFOs[0]
	);
}

void Radix2_fft_snk(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	snk(
		/* size */ (Param) inParams[0],
		/* in   */ (CplxSp*) inputFIFOs[0]
	);
}

void Radix2_fft_cfgFFT(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	cfgFFT(
		/* size */ (Param*) &outParams[0],
		/* P    */ (Param*) &outParams[1],
		/* n1   */ (Param*) &outParams[2],
		/* n2   */ (Param*) &outParams[3]
	);
}

void DFT_Radix2_genIx(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	genIx(
		/* n   */ (Param) inParams[0],
		/* ixs */ (int*) outputFIFOs[0]
	);
}

void Radix2_Stage_DFT_2(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	fft_2(
		/* n  */ (Param) inParams[0],
		/* p  */ (Param) inParams[1],
		/* N2 */ (Param) inParams[3],
		/* N1 */ (Param) inParams[2],
		/* ix */ (char*) inputFIFOs[2],
		/* i0 */ (CplxSp*) inputFIFOs[0],
		/* i1 */ (CplxSp*) inputFIFOs[1],
		/* o0 */ (CplxSp*) outputFIFOs[0],
		/* o1 */ (CplxSp*) outputFIFOs[1]
	);
}

void Radix2_Stage_cfg(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	cfg(
		/* in  */ (int*) inputFIFOs[0],
		/* out */ (Param*) &outParams[0]
	);
}

void Radix2_Stage_genIx(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	genIx(
		/* n   */ (Param) inParams[0],
		/* ixs */ (int*) outputFIFOs[0]
	);
}

