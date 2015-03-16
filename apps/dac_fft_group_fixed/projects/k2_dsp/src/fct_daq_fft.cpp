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
#include "daq_fft.h"

#include "actors.h"

void src(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void T_1(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void FFT_2(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void T_3(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void Twi_4(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void FFT_5(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void T_6(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void snk(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void cfg(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void genIx(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);

lrtFct daq_fft_fcts[N_FCT_DAQ_FFT] = {
	&src,
	&T_1,
	&FFT_2,
	&T_3,
	&Twi_4,
	&FFT_5,
	&T_6,
	&snk,
	&cfg,
	&genIx,
};

void src(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	src(
		/* size */ (Param) inParams[0],
		/* out  */ (short*) outputFIFOs[0]
	);
}

void T_1(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	transpose(
		/* Nc  */ (Param) inParams[0],
		/* Nr  */ (Param) inParams[1],
		/* in  */ (short*) inputFIFOs[0],
		/* out */ (short*) outputFIFOs[0]
	);
}

void FFT_2(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	fft(
		/* size */ (Param) inParams[0],
		/* n    */ (Param) inParams[1],
		/* in   */ (short*) inputFIFOs[0],
		/* out  */ (short*) outputFIFOs[0]
	);
}

void T_3(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	transpose(
		/* Nc  */ (Param) inParams[0],
		/* Nr  */ (Param) inParams[1],
		/* in  */ (short*) inputFIFOs[0],
		/* out */ (short*) outputFIFOs[0]
	);
}

void Twi_4(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	twiddles(
		/* size */ (Param) inParams[0],
		/* n    */ (Param) inParams[1],
		/* ix   */ (int*) inputFIFOs[0],
		/* in   */ (short*) inputFIFOs[1],
		/* out  */ (short*) outputFIFOs[0]
	);
}

void FFT_5(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	fft(
		/* size */ (Param) inParams[0],
		/* n    */ (Param) inParams[1],
		/* in   */ (short*) inputFIFOs[0],
		/* out  */ (short*) outputFIFOs[0]
	);
}

void T_6(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	transpose(
		/* Nc  */ (Param) inParams[0],
		/* Nr  */ (Param) inParams[1],
		/* in  */ (short*) inputFIFOs[0],
		/* out */ (short*) outputFIFOs[0]
	);
}

void snk(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	snk(
		/* size */ (Param) inParams[0],
		/* in   */ (short*) inputFIFOs[0]
	);
}

void cfg(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	cfg(
		/* size */ (Param) inParams[0],
		/* Nc   */ (Param*) &outParams[1],
		/* Nr   */ (Param*) &outParams[0],
		/* n1   */ (Param*) &outParams[2],
		/* n2   */ (Param*) &outParams[3]
	);
}

void genIx(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	genIx(
		/* Nr  */ (Param) inParams[0],
		/* n   */ (Param) inParams[1],
		/* ixs */ (int*) outputFIFOs[0]
	);
}

