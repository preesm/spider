
/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet				*
 * 																			*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr					*
 * 																			*
 * This software is a computer program whose purpose is to execute			*
 * parallel applications.													*
 * 																			*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 												*
 * 																			*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 																*
 * 																			*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 									*
 * 																			*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.			*
 ****************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "sharedMem.h"

#define MUTEX_BASE_NAME	"Mutex_"

typedef struct OS_SHMEM {
	UINT32 	base;
//	UINT32 	dataBase;
	UINT32 	length;
//	FILE*	file;
	char	file_name[50];
	HANDLE 	ghMutex;
} OS_SHMEM;

static OS_SHMEM OSShMemTbl[MAX_SLAVES];
static int nbOSShMem = 0;

void addShMem(UINT32 base, UINT32 dataBase, UINT32 length, const char* filename) {
	FILE *pFile;
	if (nbOSShMem <= MAX_SLAVES) {
		sprintf(OSShMemTbl[nbOSShMem].file_name, filename);

//		pFile = fopen(OSShMemTbl[nbOSShMem].file_name, "wb+");
//		if (pFile == (FILE*)0) {
//			perror("");
//			exit(1);
//		}
//		fclose(pFile);
		OSShMemTbl[nbOSShMem].base 	 	= base;
//		OSShMemTbl[nbOSShMem].dataBase	= dataBase;
		OSShMemTbl[nbOSShMem].length = length;
		nbOSShMem++;
	} else {
		fprintf(stderr,
				"Error too many Memory regions, "
				"change OS_MAX_SH_MEM macro\n");
		exit(1);
	}
}

void ShMemInit(UINT8 nbSlaves) {
	char mutexName[50];
	// Adding a memory for the mailboxes.
	addShMem(MBOX_MEM_BASE_ADDR, MBOX_MEM_BASE_ADDR, MBOX_MEM_SIZE, MBOX_MEM_FILE_PATH);

	for (int i = 0; i < nbSlaves; i++) {
		// Creating windows mutex to synchronize access to the memory file.
		sprintf(mutexName, "%s%d", MUTEX_BASE_NAME, i);
		OSShMemTbl[i].ghMutex = OpenMutex(
			SYNCHRONIZE,
	        FALSE,
	        mutexName);

	    if (OSShMemTbl[i].ghMutex == NULL)
	    {
	        printf("CreateMutex error: %d\n", GetLastError());
	        exit(1);
	    }
	}
}


UINT32 ShMemRead(UINT32 address, void* data, UINT32 size) {
	FILE *pFile;
	int NumBytes;
	int i;
	DWORD dwWaitResult;
	UINT32* ptr = (UINT32*)data;
	NumBytes = 0;

	for (i = 0; i < nbOSShMem && NumBytes == 0; i++) {
		if (OSShMemTbl[i].base <= address
				&& OSShMemTbl[i].base + OSShMemTbl[i].length > address
				&& OSShMemTbl[i].base <= address + size
				&& OSShMemTbl[i].base + OSShMemTbl[i].length > address + size) {

			// Request ownership of mutex.
			dwWaitResult = WaitForSingleObject(
				OSShMemTbl[i].ghMutex,    // handle to mutex
	            INFINITE);  // no time-out interval

			pFile = fopen(OSShMemTbl[i].file_name, "rb+");
			fseek(pFile, address-OSShMemTbl[i].base, SEEK_SET);
			NumBytes = fread(data, size, 1, pFile);
			fclose(pFile);
			// Release ownership of the mutex object
            if (! ReleaseMutex(OSShMemTbl[i].ghMutex))
            {
                // Handle error.
            }
			return NumBytes;
		}
		else{
			printf("Address : 0x%x is out of range 0x%x - 0x%x\n", address, OSShMemTbl[i].base, OSShMemTbl[i].length);
	        exit(1);
		}
	}
	return NumBytes;
}


UINT32 ShMemWrite(UINT32 address, void* data, UINT32 size) {
	FILE *pFile;
	int NumBytes;
	int i;
	DWORD dwWaitResult;
	UINT32* ptr = (UINT32*)data;
	NumBytes = 0;

	for (i = 0; i < nbOSShMem && NumBytes == 0; i++) {
		if (OSShMemTbl[i].base <= address
				&& OSShMemTbl[i].base + OSShMemTbl[i].length > address
				&& OSShMemTbl[i].base <= address + size
				&& OSShMemTbl[i].base + OSShMemTbl[i].length > address + size) {

			// Request ownership of mutex.
			dwWaitResult = WaitForSingleObject(
				OSShMemTbl[i].ghMutex,    // handle to mutex
	            INFINITE);  // no time-out interval

			pFile = fopen(OSShMemTbl[i].file_name, "rb+");
			fseek(pFile, address-OSShMemTbl[i].base, SEEK_SET);
			NumBytes = fwrite(data, size, 1, pFile);
			fclose(pFile);
			// Release ownership of the mutex object
            if (! ReleaseMutex(OSShMemTbl[i].ghMutex))
            {
                // Handle error.
            }
			return NumBytes;
		}
		else{
			printf("Address : 0x%x is out of range 0x%x - 0x%x\n", address, OSShMemTbl[i].base, OSShMemTbl[i].length);
	        exit(1);
		}
	}
	return NumBytes;
}
