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
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <platform_types.h>
#include <platform_queue.h>

static int platform_QGRT[PlatformNbQueueTypes][2];
int cpuId;

static const char* typeName[3] = {
		"CTRL",
		"INFO",
		"JOB"
};

#define PiSDF_PATH "/home/jheulot/dev/"

void platform_queue_Init(){
	char tempStr[50];
	int i, flags;

	for(i=0; i<PlatformNbQueueTypes; i++){
		sprintf(tempStr, "%s%s_%dtoGrt",PiSDF_PATH,typeName[i],cpuId);
		platform_QGRT[i][PlatformOutputQueue] =  open(tempStr, O_RDWR);
		if (platform_QGRT[i][PlatformOutputQueue] == -1) {
			printf("Failed to open %s: creating...\n",tempStr);
			mkfifo(tempStr, S_IRWXU);
		}

		sprintf(tempStr, "%s%s_Grtto%d",PiSDF_PATH,typeName[i],cpuId);
		platform_QGRT[i][PlatformInputQueue] = open(tempStr, O_RDWR);
		if (platform_QGRT[i][PlatformInputQueue] == -1) {
			printf("Failed to open %s: creating...\n",tempStr);
			mkfifo(tempStr, S_IRWXU);
		}

		flags = fcntl(platform_QGRT[i][PlatformInputQueue], F_GETFL, 0);
		fcntl(platform_QGRT[i][PlatformInputQueue], F_SETFL, flags | O_NONBLOCK);
	}
}

UINT32 platform_queue_push(PlatformQueueType queueType, void* data, int size){
	int file = platform_QGRT[queueType][PlatformOutputQueue];

	int i=0;
	while(i < size){
		int res = write(file, (char*)data+i, size-i);
		if(res>0) i+=res;
	}
	return i;
}

UINT32 platform_queue_push_UINT32(PlatformQueueType queueType, UINT32 data){
	return platform_queue_push(queueType, &data, sizeof(unsigned int));
}

void platform_queue_push_finalize(PlatformQueueType queueType){

}

UINT32 platform_queue_pop(PlatformQueueType queueType, void* data, int size){
	int file = platform_QGRT[queueType][PlatformInputQueue];

	int i=0;
	while(i < size){
		int res = read(file, (char*)data+i, size-i);
		if(res>0) i+=res;
	}
	return i;
}

UINT32 platform_queue_pop_UINT32(PlatformQueueType queueType){
	UINT32 data;
	platform_queue_pop(queueType, &data, sizeof(UINT32));
	return data;
}

BOOL platform_queue_NBPop_UINT32(PlatformQueueType queueType, UINT32* data){
	int file = platform_QGRT[queueType][PlatformInputQueue];
	int res = read(file, data, sizeof(UINT32));
	if(res == -1)
		return 0;
	return res;
}
