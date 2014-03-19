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

#include "types.h"
#include <stdlib.h>
#include <string.h>
#include <linux/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <sys/time.h>

#include "types.h"
#include "../lrt/lrt.h"

#define DDR_BASE 	0x10000000
#define DDR_LENGTH 	0x10000000
#define DDR_FILE	"/dev/mem"

static void* ddr_address;
static int f;

void OS_ShMemInit() {
//	int f;

	if (!(f=open(DDR_FILE,O_RDWR | O_SYNC))) {
		fprintf(stderr, "%s: %s\n", DDR_FILE, strerror(errno));
		exit(1);
	}

	ddr_address=mmap(NULL, DDR_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, f, (int)DDR_BASE);

	if (ddr_address == (void *)-1) {
		fprintf(stderr,"mmap(): %s\n",strerror(errno));
		exit(1);
	}
	close(f);
}

UINT32 OS_ShMemRead(UINT32 address, void* data, UINT32 size) {
	void* start = ddr_address+(int)address-(int)DDR_BASE;
	if (DDR_BASE <= address
			&& DDR_BASE + DDR_LENGTH > address
			&& DDR_BASE <= address + size
			&& DDR_BASE + DDR_LENGTH > address + size) {
//		tmp = msync((void*)((int)start&(~4047)),4048, MS_INVALIDATE);
//		if (tmp == -1) {
//			fprintf(stderr,"msync(): %s\n",strerror(errno));
//			exit(1);
//		}
		memcpy(data,start, size);
//		lseek(f, address, SEEK_SET);
//		read(f, data, size);
		return size;
	}else
		printf("Memory not found 0x%x size 0x%x\n", (int)address, (int) size);
	return 0;
}

UINT32 OS_ShMemWrite(UINT32 address, void* data, UINT32 size) {
	void* start = ddr_address+(int)address-(int)DDR_BASE;
	if (DDR_BASE <= address
			&& DDR_BASE + DDR_LENGTH > address
			&& DDR_BASE <= address + size
			&& DDR_BASE + DDR_LENGTH > address + size) {

//		tmp = msync((void*)((int)start&(~4047)),4048, MS_SYNC);
//		if (tmp == -1) {
//			fprintf(stderr,"msync(): %s\n",strerror(errno));
//			exit(1);
//		}

				memcpy(start,data, size);
//		lseek(f, address, SEEK_SET);
//		write(f, data, size);
		return size;
	}else
		printf("Memory not found 0x%x size 0x%x\n", (int)address, (int) size);
	return 0;
}
