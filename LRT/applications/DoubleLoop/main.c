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

#include <stdio.h>
#include <stdlib.h>
#include <platform_queue.h>
#include <platform_data_queue.h>
#include <platform.h>
#include <lrt_core.h>
#include <lrt_taskMngr.h>

#include "DoubleLoop.h"

#define STANDALONE_APP	0

int main(int argc, char **argv) {

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

//	if(argc > 1)
//		cpuId = atoi(argv[1]);
//	else{
//		printf("You must specify procID\n");
//		return -1;
//	}
//
	platform_init(0);

	functions_tbl[0] = rdFile;
	functions_tbl[2] = initNLoop;
	functions_tbl[3] = endNLoop;
	functions_tbl[4] = wrFile;

	//*** MLoop hierarchy ***//
	functions_tbl[5] = configM;
	functions_tbl[6] = initMLoop;
	functions_tbl[7] = f;
	functions_tbl[8] = endMLoop;

	//*** Special actors ***//
//	functions_tbl[9] = input;
	functions_tbl[10] = RB;
	functions_tbl[11] = broadcast;
//	functions_tbl[12] = output;
	functions_tbl[13] = Xplode;



	LRTInit();

#if STANDALONE_APP == 1
	UINT32 FifoIds[MAX_NB_FIFOs], FifoAddrs[MAX_NB_FIFOs];
	printf("Standalone application..\n");

	for (i = 0; i < MAX_NB_FIFOs; i++) {
		FifoIds[i] = i;
		FifoAddrs[i] = i * IMG_SIZE;
	}
	flushFIFO(-1);	// Clear all FIFOs.

	/*** readFile ***/
	outputFifoId[0] = FifoIds[0];
	outputFifoId[1] = FifoIds[1];
	outputFifoAddr[0] = FifoAddrs[0];
	outputFifoAddr[1] = FifoAddrs[1];
	readFile(inputFifoId, inputFifoAddr, outputFifoId, outputFifoAddr, 0);

#else

//	printf("Starting local runtime %d...\n", platform_getCoreId());
	LRTCtrlStart();
#endif
	return 0;
}
