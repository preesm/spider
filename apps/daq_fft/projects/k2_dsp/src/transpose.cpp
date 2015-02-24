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

#include <lrt.h>
#include <assert.h>
#include "actors.h"

extern "C"{
#include "edma.h"
}

void transpose(Param Nc, Param Nr, float* restrict in, float* restrict out){
#if VERBOSE
	printf("Execute transpose\n");
#endif
//	int i, j;
//	_nassert((int) in  % 8 == 0); 	// in  is 64-bit aligned
//	_nassert((int) out % 8 == 0); 	// out is 64-bit aligned
//	for (i = 0; i < Nr; i++) {
//#pragma MUST_ITERATE(8,,8)
//		for (j = 0; j < Nc; j++) {
//			out[2*(i*Nc + j)]   = in[2*(j*Nc + i)];
//			out[2*(i*Nc + j)+1] = in[2*(j*Nc + i)+1];
//		}
//	}

	/* See 3-5 of Enhanced Direct Memory Access (EDMA3) Controller User Guide from TI (sprugs5a)*/
	edma_cpy(
		/* Src */ in,
		/* Dst */ out,
		/*ACnt: element size*/ 			8,
		/*BCnt: line size*/ 			Nc,
		/*CCnt: n lines in matrice*/ 	Nr,
		/*SrcBIdx: = ACnt */			8,
		/*DstBIdx: = CCnt*ACnt */		8*Nr,
		/*SrcCIdx: = ACnt*BCnt */		8*Nc,
		/*DstCIdx: = ACnt */			8
	);
}
