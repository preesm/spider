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

#include "types.h"
#include "hwQueues.h"

typedef enum{
	IN=0,
	OUT=1
} FifoDir;

static int OS_QGRT[nbQueueTypes][2];
int cpuId;

static const char* typeName[3] = {
		"CTRL",
		"INFO",
		"JOB"
};

#define BASE_PATH "/home/jheulot/dev/"

void RTQueuesInit(){
	char tempStr[50];
	int i, flags;

	for(i=0; i<nbQueueTypes; i++){
		sprintf(tempStr, "%s%s_%dtoGrt",BASE_PATH,typeName[i],cpuId);
		OS_QGRT[i][OUT] =  open(tempStr, O_RDWR);
		if (OS_QGRT[i][OUT] == -1) {
			printf("Failed to open %s: creating...\n",tempStr);
			mkfifo(tempStr, S_IRWXU);
		}

		sprintf(tempStr, "%s%s_Grtto%d",BASE_PATH,typeName[i],cpuId);
		OS_QGRT[i][IN] = open(tempStr, O_RDWR);
		if (OS_QGRT[i][IN] == -1) {
			printf("Failed to open %s: creating...\n",tempStr);
			mkfifo(tempStr, S_IRWXU);
		}

		flags = fcntl(OS_QGRT[i][IN], F_GETFL, 0);
		fcntl(OS_QGRT[i][IN], F_SETFL, flags | O_NONBLOCK);
	}
}

UINT32 RTQueuePush(RTQueueType queueType, void* data, int size){
	int file = OS_QGRT[queueType][OUT];

	int i=0;
	while(i < size){
		int res = write(file, (char*)data+i, size-i);
		if(res>0) i+=res;
	}
	return i;
}

UINT32 RTQueuePush_UINT32(RTQueueType queueType, UINT32 data){
	return RTQueuePush(queueType, &data, sizeof(unsigned int));
}

UINT32 RTQueuePop(RTQueueType queueType, void* data, int size){
	int file = OS_QGRT[queueType][IN];

	int i=0;
	while(i < size){
		int res = read(file, (char*)data+i, size-i);
		if(res>0) i+=res;
	}
	return i;
}

UINT32 RTQueuePop_UINT32(RTQueueType queueType){
	UINT32 data;
	RTQueuePop(queueType, &data, sizeof(UINT32));
	return data;
}

UINT32 RTQueueNonBlockingPop(RTQueueType queueType, void* data, int size){
	int file = OS_QGRT[queueType][IN];

	return read(file, (char*)data, size);
}
