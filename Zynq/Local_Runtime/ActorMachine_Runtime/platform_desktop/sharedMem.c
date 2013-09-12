/*
 * sharedMem.c
 *
 *  Created on: Jun 12, 2013
 *      Author: jheulot
 */


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
			perror("open");
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
