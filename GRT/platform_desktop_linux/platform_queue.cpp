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

#include <platform_types.h>
#include <platform_queue.h>
#include <grt_definitions.h>

static int platform_QLRT[NB_MAX_CTRLQ][platformNbQTypes][2];

static const char* typeName[platformNbQTypes] = {
		"CTRL",
		"INFO",
		"JOB"
};

#define BASE_PATH "/home/jheulot/dev/"

void platform_queue_Init(UINT8 nbSlaves){
	int i, j, flags;

	for(i=0; i<nbSlaves; i++){
		char tempStr[50];

		for(j=0; j<platformNbQTypes; j++){
			sprintf(tempStr, "%s%s_%dtoGrt",BASE_PATH,typeName[j],i);
			platform_QLRT[i][j][platformQIn] =  open(tempStr, O_RDWR);
			if (platform_QLRT[i][j][platformQIn] == -1) {printf("Failed to open %s\n",tempStr); abort();}

			sprintf(tempStr, "%s%s_Grtto%d",BASE_PATH,typeName[j],i);
			platform_QLRT[i][j][platformQOut] = open(tempStr, O_RDWR);
			if (platform_QLRT[i][j][platformQOut] == -1) {printf("Failed to open %s\n",tempStr); abort();}

			flags = fcntl(platform_QLRT[i][j][platformQIn], F_GETFL, 0);
			fcntl(platform_QLRT[i][j][platformQIn], F_SETFL, flags | O_NONBLOCK);
		}
	}

	/* Reset all queues */
	UINT32 c;
	for(i=0; i<nbSlaves; i++){
		for(j=0; j<platformNbQTypes; j++){
			while(platform_QNonBlockingPop(i, (platformQType)j, &c, sizeof(UINT32))==sizeof(UINT32));
		}
	}
}

UINT32 platform_QPush(UINT8 slaveId, platformQType queueType, void* data, int size){
	int file = platform_QLRT[slaveId][queueType][platformQOut];

	int i=0;
	while(i < size){
		int res = write(file, (char*)data+i, size-i);
		if(res>0) i+=res;
	}
	return i;
}

UINT32 platform_QPushUINT32(UINT8 slaveId, platformQType queueType, UINT32 data){
	return platform_QPush(slaveId, queueType, &data, sizeof(unsigned int));
}

UINT32 platform_QPop(UINT8 slaveId, platformQType queueType, void* data, int size){
	int file = platform_QLRT[slaveId][queueType][platformQIn];

	int i=0;
	while(i < size){
		int res = read(file, (char*)data+i, size-i);
		if(res>0) i+=res;
	}
	return i;
}

UINT32 platform_QPopUINT32(UINT8 slaveId, platformQType queueType){
	UINT32 data;
	platform_QPop(slaveId, queueType, &data, sizeof(UINT32));
	return data;
}

UINT32 platform_QNonBlockingPop(UINT8 slaveId, platformQType queueType, void* data, int size){
	int file = platform_QLRT[slaveId][queueType][platformQIn];

	return read(file, (char*)data, size);
}
