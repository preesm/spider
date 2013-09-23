/*
 * sharedMem.c
 *
 *  Created on: Jun 12, 2013
 *      Author: jheulot
 */



#include <stdio.h>
#include <stdlib.h>
#include <lrt.h>
#include "types.h"



typedef struct OS_SHMEM {
	UINT32 	base;
	UINT32 	length;
	FILE*	file;
	char	file_name[50];
} OS_SHMEM;

static OS_SHMEM OSShMemTbl[OS_MAX_SH_MEM];
static int nbOSShMem = 0;

static void addOSShMem(UINT32 base, UINT32 length, const char* filename) {
	if (nbOSShMem < OS_MAX_SH_MEM) {
		sprintf(OSShMemTbl[nbOSShMem].file_name, "C:\\work\\COMPA\\shMem");

		OSShMemTbl[nbOSShMem].file = fopen(OSShMemTbl[nbOSShMem].file_name, "wb");
		if (OSShMemTbl[nbOSShMem].file == (FILE*)0) {
			perror("");
			exit(1);
		}
		fclose(OSShMemTbl[nbOSShMem].file);
		OSShMemTbl[nbOSShMem].base 	 = base;
		OSShMemTbl[nbOSShMem].length = length;
		nbOSShMem++;
	} else {
		fprintf(stderr,
				"Error too many Memory regions, "
				"change OS_MAX_SH_MEM macro\n");
		exit(1);
	}
}

void OS_ShMemInit() {
	printf("Opening shMem...\n");
	addOSShMem(0x10000000, 0x10000000, "");
}

UINT32 OS_ShMemRead(UINT32 address, void* data, UINT32 size) {
	int i, res = 0;
	for (i = 0; i < nbOSShMem && res == 0; i++) {
		if (OSShMemTbl[i].base <= address
				&& OSShMemTbl[i].base + OSShMemTbl[i].length > address
				&& OSShMemTbl[i].base <= address + size
				&& OSShMemTbl[i].base + OSShMemTbl[i].length > address + size) {
			fopen(OSShMemTbl[i].file_name, "rb");
			fseek(OSShMemTbl[i].file, address-OSShMemTbl[i].base, SEEK_SET);
			res = fread(data, size, 1, OSShMemTbl[i].file);
			fclose(OSShMemTbl[i].file);
			return res;
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
			fopen(OSShMemTbl[i].file_name, "rb+");
			fseek(OSShMemTbl[i].file, address-OSShMemTbl[i].base, SEEK_SET);
			res = fwrite(data, size, 1, OSShMemTbl[i].file);
			fclose(OSShMemTbl[i].file);
		}
	}
	return res;
}
