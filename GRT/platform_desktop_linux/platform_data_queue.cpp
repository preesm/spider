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

#include <platform_data_queue.h>
#include <grt_definitions.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <platform_types.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

#include <memoryAlloc.h>
#include <platform.h>
#include <string.h>

UINT8* shMem_sync,*shMem_data;

void platform_shMemInit() {
    int shmid;
    key_t key = SHARED_MEM_KEY;

	printf("Creating shared memory...\n");

    /*
     * Create the segment.
     */
    if ((shmid = shmget(key, SHARED_MEM_LENGHT+DATA_FIFO_REGION_SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shMem_sync = (UINT8*)shmat(shmid, NULL, 0)) == (UINT8 *) -1) {
        perror("shmat");
        exit(1);
    }

    shMem_data = shMem_sync+DATA_FIFO_REGION_SIZE;
    memset(shMem_sync,0,DATA_FIFO_REGION_SIZE);
}

void platform_flushFIFO(UINT32 id){
	UINT8* mutex = shMem_sync+id;
	*mutex=0;
}


void platform_writeFifo(UINT32 id, UINT32 addr, UINT32 size, UINT8* buffer) {
	volatile UINT8* mutex = shMem_sync+id;

	while(*mutex != 0);
	*mutex=1;
}


void platform_readFifo(UINT32 id, UINT32 addr, UINT32 size, UINT8* buffer) {
	volatile UINT8* mutex = shMem_sync+id;

	while(*mutex != 1);
	*mutex=0;
}
