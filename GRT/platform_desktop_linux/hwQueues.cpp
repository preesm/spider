/*
 * HWQueues.c
 *
 *  Created on: Jun 12, 2013
 *      Author: jheulot
 */

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
