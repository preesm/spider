
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

#include <stdio.h>
#include <stdlib.h>
#include "sharedMem.h"

#define SH_MEM_MUTEX_NAME	"SH_MEM_Mutex"
#define MBOX_MEM_MUTEX_NAME	"MBOX_MEM_Mutex_"
static OS_SHMEM OSShMemTbl[OS_MAX_SH_MEM];
static int nbOSShMem = 0;
int cpuId;
static OS_SHMEM *shMem;
static OS_SHMEM *mboxMem;

static OS_SHMEM * addOSShMem(UINT32 base, UINT32 dataBase, UINT32 length, const char* filename) {
	OS_SHMEM *mem;
	if (nbOSShMem <= OS_MAX_SH_MEM) {
		mem = &OSShMemTbl[nbOSShMem];
		sprintf(mem->file_name, filename);
		mem->base 	 	= base;
//		mem->dataBase	= dataBase;
		mem->length = length;
		nbOSShMem++;
		return mem;
	} else {
		fprintf(stderr,
				"Error too many Memory regions, "
				"change OS_MAX_SH_MEM macro\n");
	}
	return 0;
}

void addMboxMem(){
	char mboxFileName[50];
	sprintf(mboxFileName, "%s_%d", MBOX_MEM_FILE_PATH, cpuId);
	mboxMem = addOSShMem(MBOX_MEM_BASE_ADDR, MBOX_MEM_BASE_ADDR, MBOX_MEM_SIZE, mboxFileName);
}

void addShMem(){
	shMem = addOSShMem(SH_MEM_BASE_ADDR, SH_MEM_BASE_ADDR + SH_MEM_DATA_REGION_SIZE, SH_MEM_SIZE, SH_MEM_FILE_PATH);
}

void mboxMemInit(){
	char mutexName[50];
	FILE *pFile;

	// Creating windows mutex to synchronize access to the memory file.
	// The mutex is released after the queues' initializations.
	sprintf(mutexName, "%s%d", MBOX_MEM_MUTEX_NAME, cpuId);
	mboxMem->ghMutex = CreateMutex(
        NULL,              	// default security attributes
        TRUE,             	// initially owned
        mutexName);  		// object name

    if (mboxMem->ghMutex == NULL)
    {
        printf("CreateMutex error: %d\n", GetLastError());
        exit(1);
    }

	// Creating the system file.
	pFile = fopen(mboxMem->file_name, "wb+");
	if (pFile == (FILE*)0) {
		perror("Can't open file memory");
		exit(1);
	}
	fclose(pFile);
}

void shMemInit(UINT32 cpuId) {
	char mutexName[50];
	FILE *pFile;

	sprintf(mutexName, "%s", SH_MEM_MUTEX_NAME);
	if(cpuId == 0){
		// Creating windows mutex to synchronize access to the memory file.
		// The mutex is released after the queues' initializations.
		shMem->ghMutex = CreateMutex(
	        NULL,              	// default security attributes
	        FALSE,             	// initially not owned
	        mutexName);  		// object name

	    if (shMem->ghMutex == NULL)
	    {
	        printf("CreateMutex error: %d\n", GetLastError());
	        exit(1);
	    }

		// Creating the system file.
		pFile = fopen(shMem->file_name, "wb+");
		if (pFile == (FILE*)0) {
			perror("Can't open file memory");
			exit(1);
		}
		fclose(pFile);
	}
	else{
		// The other CPUs only get the mutex created by CPU_0.
		shMem->ghMutex = OpenMutex(
			SYNCHRONIZE,
	        FALSE,
	        mutexName);

	    if (shMem->ghMutex == NULL)
	    {
	        printf("CreateMutex error: %d\n", GetLastError());
	        exit(1);
	    }
	}
}


UINT32 OS_ShMemRead(UINT32 address, void* data, UINT32 size) {
	int i, res;
	FILE* pFile;
	DWORD dwWaitResult;

	res = 0;
	for (i = 0; i < nbOSShMem && res == 0; i++) {
		if (OSShMemTbl[i].base <= address
				&& OSShMemTbl[i].base + OSShMemTbl[i].length > address
				&& OSShMemTbl[i].base <= address + size
				&& OSShMemTbl[i].base + OSShMemTbl[i].length > address + size) {

			// Request ownership of mutex.
			dwWaitResult = WaitForSingleObject(
				OSShMemTbl[i].ghMutex,  // handle to mutex
	            INFINITE);  			// no time-out interval

//			if (dwWaitResult == WAIT_OBJECT_0)
//	        {
	            // The thread got ownership of the mutex
				pFile = fopen(OSShMemTbl[i].file_name, "rb+");
				if (pFile == (FILE*)0) {
					perror("Can't open file memory");
					exit(1);
				}
				fseek(pFile, address-OSShMemTbl[i].base, SEEK_SET);
				res = fread(data, size, 1, pFile);
				fclose(pFile);
//	        }
//			else if(dwWaitResult == WAIT_ABANDONED){
//	            // The thread got ownership of an abandoned mutex
//	            // The file is in an indeterminate state
//				perror("Mutex was abandoned");
//				exit(1);
//			}

			// Release ownership of the mutex object
			if (! ReleaseMutex(OSShMemTbl[i].ghMutex))
			{
				printf("Error %d at MUTEX release\n", GetLastError());
				exit(1);
			}
        	return res;
		}
	}
	printf("Memory address not found 0x%x\n", address);
	exit(1);
}

UINT32 OS_ShMemWrite(UINT32 address, void* data, UINT32 size) {
	int i, res;
	FILE* pFile;
	DWORD dwWaitResult;

	res = 0;
	for (i = 0; i < nbOSShMem && res == 0; i++) {
		if (OSShMemTbl[i].base <= address
				&& OSShMemTbl[i].base + OSShMemTbl[i].length > address
				&& OSShMemTbl[i].base <= address + size
				&& OSShMemTbl[i].base + OSShMemTbl[i].length > address + size) {

			// Request ownership of mutex.
			dwWaitResult = WaitForSingleObject(
				OSShMemTbl[i].ghMutex,  // handle to mutex
	            INFINITE);  			// no time-out interval

//			if (dwWaitResult == WAIT_OBJECT_0)
//	        {
	            // The thread got ownership of the mutex
				pFile = fopen(OSShMemTbl[i].file_name, "rb+");
				if (pFile == (FILE*)0) {
					perror("Can't open file memory");
					exit(1);
				}
				fseek(pFile, address-OSShMemTbl[i].base, SEEK_SET);
				res = fwrite(data, size, 1, pFile);
				fclose(pFile);
//	        }
//			else if(dwWaitResult == WAIT_ABANDONED){
//	            // The thread got ownership of an abandoned mutex
//	            // The file is in an indeterminate state
//				perror("Mutex was abandoned");
//				exit(1);
//			}

			// Release ownership of the mutex object
			if (! ReleaseMutex(OSShMemTbl[i].ghMutex))
			{
				printf("Error %d at MUTEX release\n", GetLastError());
				exit(1);
			}

			return res;
		}
	}
	printf("Memory address not found 0x%x\n", address);
	exit(1);
}

void releaseShMemMx(){
	// Release ownership of the mutex object
	if (! ReleaseMutex(shMem->ghMutex))
	{
		printf("Error %d at MUTEX release\n", GetLastError());
		exit(1);
	}
}


void releaseMboxMemMx(){
	// Release ownership of the mutex object
	if (! ReleaseMutex(mboxMem->ghMutex))
	{
		printf("Error %d at MUTEX release\n", GetLastError());
		exit(1);
	}
}


void resetShMemAddr(UINT32 address){
	int i;
	FILE* pFile;
	UINT32 data = 0;
	for (i = 0; i < nbOSShMem; i++) {
		if ((OSShMemTbl[i].base <= address) &&
			(OSShMemTbl[i].base + OSShMemTbl[i].length > address) &&
			(OSShMemTbl[i].base <= address + sizeof(UINT32)) &&
			(OSShMemTbl[i].base + OSShMemTbl[i].length > address + sizeof(UINT32))) {
			pFile = fopen(OSShMemTbl[i].file_name, "rb+");
			if (pFile == (FILE*)0) {
				perror("Can't open file memory");
				exit(1);
			}
			fseek(pFile, address-OSShMemTbl[i].base, SEEK_SET);
			fwrite(&data, 1, sizeof(UINT32), pFile);
			fclose(pFile);

			return;
		}
	}
	printf("Memory address not found 0x%x\n", address);
	exit(1);
}
