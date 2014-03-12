
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/


#include <lrt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "types.h"
#include <fcntl.h>

typedef struct OS_SHMEM {
	UINT32 base;
	UINT32 length;
	int file;
} OS_SHMEM;

static OS_SHMEM OSShMemTbl[OS_MAX_SH_MEM];
static int nbOSShMem = 0;

static void addOSShMem(UINT32 base, UINT32 length, const char* filename) {
	if (nbOSShMem < OS_MAX_SH_MEM) {
		OSShMemTbl[nbOSShMem].file = open(filename, O_RDWR);
		if (OSShMemTbl[nbOSShMem].file == -1) {
			perror("open mem");
			abort();
		}
		OSShMemTbl[nbOSShMem].base = base;
		OSShMemTbl[nbOSShMem].length = length;
		nbOSShMem++;
	} else {
		fprintf(stderr,
				"Error too many Memory regions, "
				"change OS_MAX_SH_MEM macro\n");
		abort();
	}
}

void OS_ShMemInit() {
	printf("Openning /home/jheulot/dev/shMem...\n");
	addOSShMem(0x10000000, 0x10000000, "/home/jheulot/dev/shMem");
}

UINT32 OS_ShMemRead(UINT32 address, void* data, UINT32 size) {
	int i, res = 0;
	for (i = 0; i < nbOSShMem && res == 0; i++) {
		if (OSShMemTbl[i].base <= address
				&& OSShMemTbl[i].base + OSShMemTbl[i].length > address
				&& OSShMemTbl[i].base <= address + size
				&& OSShMemTbl[i].base + OSShMemTbl[i].length > address + size) {
			lseek(OSShMemTbl[i].file, address-OSShMemTbl[i].base, SEEK_SET);
			return res = read(OSShMemTbl[i].file, data, size);
		}
	}
	printf("Memory not found 0x%x\n", address);
	return res;
}

UINT32 OS_ShMemWrite(UINT32 address, void* data, UINT32 size) {
	int i, res = 0;
	for (i = 0; i < nbOSShMem && res == 0; i++) {
		if (OSShMemTbl[i].base <= address
				&& OSShMemTbl[i].base + OSShMemTbl[i].length > address
				&& OSShMemTbl[i].base <= address + size
				&& OSShMemTbl[i].base + OSShMemTbl[i].length > address + size) {
			lseek(OSShMemTbl[i].file, address-OSShMemTbl[i].base, SEEK_SET);
			res = write(OSShMemTbl[i].file, data, size);
		}
	}
	return res;
}
