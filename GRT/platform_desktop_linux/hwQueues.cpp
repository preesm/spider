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

#include "types.h"
#include "hwQueues.h"
#include <SchedulerDimensions.h>

typedef enum{
	CTRL=0,
	INFO=1
} MailboxType;

typedef enum{
	IN=0,
	OUT=1
} FifoDir;

static int OS_QLRT[NB_MAX_CTRLQ][2][2];

#define BASE_PATH "/home/jheulot/dev/"

void OS_QInit(int nbSlaves){
	int i, flags;

	for(i=0; i<nbSlaves; i++){
		char tempStr[50];

		sprintf(tempStr, "%sCtrl_%dtoGrt",BASE_PATH,i);
		OS_QLRT[i][CTRL][IN] =  open(tempStr, O_RDWR);
		sprintf(tempStr, "%sCtrl_Grtto%d",BASE_PATH,i);
		OS_QLRT[i][CTRL][OUT] = open(tempStr, O_RDWR);
		flags = fcntl(OS_QLRT[i][CTRL][IN], F_GETFL, 0);
		fcntl(OS_QLRT[i][CTRL][IN], F_SETFL, flags | O_NONBLOCK);

		sprintf(tempStr, "%sInfo_%dtoGrt",BASE_PATH,i);
		OS_QLRT[i][INFO][IN] = open(tempStr, O_RDWR);
		sprintf(tempStr, "%sInfo_Grtto%d",BASE_PATH,i);
		OS_QLRT[i][INFO][OUT] = open(tempStr, O_RDWR);
		flags = fcntl(OS_QLRT[i][INFO][IN], F_GETFL, 0);
		fcntl(OS_QLRT[i][INFO][IN], F_SETFL, flags | O_NONBLOCK);
	}

	UINT32 c;
	for(i=0; i<nbSlaves; i++){
		while(OS_CtrlQPop_nonBlocking(i, &c, sizeof(UINT32))==sizeof(UINT32));
		while(OS_InfoQPop_nonBlocking(i, &c, sizeof(UINT32))==sizeof(UINT32));
	}
}

/* Ctrl Mailbox */
UINT32 OS_CtrlQPush(int id, void* data, int size){
	int file = OS_QLRT[id][CTRL][OUT];

	int i=0;
	while(i < size){
		int res = write(file, (char*)data+i, size-i);
		if(res>0) i+=res;
	}
	return i;
}

UINT32 OS_CtrlQPop(int id, void* data, int size){
	int file = OS_QLRT[id][CTRL][IN];

	int i=0;
	while(i < size){
		int res = read(file, (char*)data+i, size-i);
		if(res>0) i+=res;
	}
	return i;
}

UINT32 OS_CtrlQPushInt(int id, unsigned int data){
	return OS_CtrlQPush(id, &data, sizeof(unsigned int));
}


UINT32 OS_CtrlQPopInt(int id){
	UINT32 data;
	OS_CtrlQPop(id, &data, sizeof(UINT32));
	return data;
}

UINT32 OS_CtrlQPop_nonBlocking(int id, void* data, int size){
	int file = OS_QLRT[id][CTRL][IN];

	return read(file, (char*)data, size);
}

/* Info Mailbox */
UINT32 OS_InfoQPush(int id, void* data, int size){
	int file = OS_QLRT[id][INFO][OUT];

	int i=0;
	while(i < size){
		int res = write(file, (char*)data+i, size-i);
		if(res>0) i+=res;
	}
	return i;
}

UINT32 OS_InfoQPop(int id, void* data, int size){
	int file = OS_QLRT[id][INFO][IN];

	int i=0;
	while(i < size){
		int res = read(file, (char*)data+i, size-i);
		if(res>0) i+=res;
	}
	return i;
}

UINT32 OS_InfoQPushInt(int id, unsigned int data){
	return OS_InfoQPush(id, &data, sizeof(unsigned int));
}


UINT32 OS_InfoQPopInt(int id){
	UINT32 data;
	OS_InfoQPop(id, &data, sizeof(UINT32));
	return data;
}

UINT32 OS_InfoQPop_nonBlocking(int id, void* data, int size){
	int file = OS_QLRT[id][INFO][IN];

	return read(file, (char*)data, size);
}
