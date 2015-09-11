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
#include "top_hclm.h"

#include "actors.h"

void top_hclm_opt_cfg_N(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void top_hclm_opt_src(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void top_hclm_opt_snk(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void FIR_Chan_opt_cfg_M(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void FIR_Chan_opt_initSw(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);
void FIR_Chan_opt_FIR(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]);

lrtFct top_hclm_opt_fcts[N_FCT_TOP_HCLM_OPT] = {
	&top_hclm_opt_cfg_N,
	&top_hclm_opt_src,
	NULL,
	&top_hclm_opt_snk,
	&FIR_Chan_opt_cfg_M,
	&FIR_Chan_opt_initSw,
	NULL,
	&FIR_Chan_opt_FIR,
};

void top_hclm_opt_cfg_N(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	cfg_N(
		/* NVal   */ (Param) inParams[1],
		/* MStart */ (Param) inParams[2],
		/* MNext  */ (Param) inParams[3],
		/* NMax   */ (Param) inParams[0],
		/* N      */ (Param*) &outParams[0],
		/* M      */ (char*) outputFIFOs[0]
	);
}

void top_hclm_opt_src(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	src(
		/* NbS */ (Param) inParams[0],
		/* N   */ (Param) inParams[1],
		/* out */ (float*) outputFIFOs[0]
	);
}

void top_hclm_opt_snk(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	snk(
		/* NbS */ (Param) inParams[0],
		/* N   */ (Param) inParams[1],
		/* in  */ (float*) inputFIFOs[0]
	);
}

void FIR_Chan_opt_cfg_M(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	cfg_M(
		/* in */ (char*) inputFIFOs[0],
		/* M  */ (Param*) &outParams[0]
	);
}

void FIR_Chan_opt_initSw(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	initSw(
		/* M    */ (Param) inParams[0],
		/* ixs  */ (char*) outputFIFOs[1],
		/* sels */ (char*) outputFIFOs[0]
	);
}

void FIR_Chan_opt_FIR(void* inputFIFOs[], void* outputFIFOs[], Param inParams[], Param outParams[]){
	FIR(
		/* NbS */ (Param) inParams[0],
		/* ix  */ (char*) inputFIFOs[1],
		/* in  */ (float*) inputFIFOs[0],
		/* out */ (float*) outputFIFOs[0]
	);
}

