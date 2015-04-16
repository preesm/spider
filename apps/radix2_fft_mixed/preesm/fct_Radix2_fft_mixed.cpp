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

#include <spider.h>
#include "Radix2_fft_mixed.h"

#include "actors.h"

void Radix2_fft_mixed_src(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void Radix2_fft_mixed_T(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void Radix2_fft_mixed_DFT_N2(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void Radix2_fft_mixed_snk(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void DFT_Radix2_genIx(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void DFT_Radix2_switch(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void Radix2_Stage_DFT_2(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void Radix2_Stage_cfg(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void Radix2_Stage_genIx(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);

lrtFct Radix2_fft_mixed_fcts[N_FCT_RADIX2_FFT_MIXED] = {
	&Radix2_fft_mixed_src,
	&Radix2_fft_mixed_T,
	&Radix2_fft_mixed_DFT_N2,
	&Radix2_fft_mixed_snk,
	&DFT_Radix2_genIx,
	&DFT_Radix2_switch,
	&Radix2_Stage_DFT_2,
	&Radix2_Stage_cfg,
	&Radix2_Stage_genIx,
};

void Radix2_fft_mixed_src(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	src(
		/* size */ (Param) inParams[0],
		/* out  */ (float*) outputFIFOs[0]
	);
}

void Radix2_fft_mixed_T(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	T(
		/* N1  */ (Param) inParams[0],
		/* N2  */ (Param) inParams[1],
		/* in  */ (float*) inputFIFOs[0],
		/* out */ (float*) outputFIFOs[0]
	);
}

void Radix2_fft_mixed_DFT_N2(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	fft(
		/* size */ (Param) inParams[0],
		/* n    */ (Param) inParams[1],
		/* in   */ (float*) inputFIFOs[0],
		/* out  */ (float*) outputFIFOs[0]
	);
}

void Radix2_fft_mixed_snk(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	snk(
		/* size */ (Param) inParams[0],
		/* in   */ (float*) inputFIFOs[0]
	);
}

void DFT_Radix2_genIx(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	genIx(
		/* n   */ (Param) inParams[0],
		/* ixs */ (char*) outputFIFOs[0]
	);
}

void DFT_Radix2_switch(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	Switch(
		/* size */ (Param) inParams[0],
		/* i0   */ (float*) inputFIFOs[0],
		/* i1   */ (float*) inputFIFOs[1],
		/* out  */ (float*) outputFIFOs[0]
	);
}

void Radix2_Stage_DFT_2(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	fft_2(
		/* n  */ (Param) inParams[0],
		/* ix */ (char*) inputFIFOs[2],
		/* i0 */ (float*) inputFIFOs[0],
		/* i1 */ (float*) inputFIFOs[1],
		/* o0 */ (float*) outputFIFOs[0],
		/* o1 */ (float*) outputFIFOs[1]
	);
}

void Radix2_Stage_cfg(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	cfg(
		/* in  */ (char*) inputFIFOs[0],
		/* out */ (Param*) &outParams[0]
	);
}

void Radix2_Stage_genIx(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	genIx(
		/* n   */ (Param) inParams[0],
		/* ixs */ (char*) outputFIFOs[0]
	);
}

