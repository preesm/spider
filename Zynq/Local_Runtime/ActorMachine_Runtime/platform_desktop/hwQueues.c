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

typedef enum{
	CTRL,
	INFO
} MailboxType;

static int OS_QIn[2], OS_QOut[2];
int cpuId;

void OS_QInit(){
	OS_QIn[CTRL] = open("/root/Ctrl_Grtto0", O_RDWR | O_NONBLOCK);
	if (OS_QIn[CTRL] == -1) {perror("open"); abort();}

	OS_QOut[CTRL] = open("/root/Ctrl_0toGrt", O_RDWR | O_NONBLOCK);
	if (OS_QIn[CTRL] == -1) {perror("open"); abort();}

	OS_QIn[INFO] = open("/root/Info_Grtto0", O_RDWR | O_NONBLOCK);
	if (OS_QIn[INFO] == -1) {perror("open"); abort();}

	OS_QOut[INFO] = open("/root/Info_0toGrt", O_RDWR | O_NONBLOCK);
	if (OS_QIn[INFO] == -1) {perror("open"); abort();}
}

/*******************/
/* Control Mailbox */
/*******************/

UINT32 OS_CtrlQPush(void* data, int size){
	return write(OS_QOut[CTRL], data, size);
}

UINT32 OS_CtrlQPop(void* data, int size){
	int i=0;
	while(i < size){
		int res = read(OS_QIn[CTRL], (char*)data+i, size-i);
		if(res>0) i+=res;
	}
	return i;
}

UINT32 OS_CtrlQPop_UINT32(){
	UINT32 value;
	OS_CtrlQPop(&value, sizeof(UINT32));
	return value;
}

void OS_CtrlQPush_UINT32(UINT32 value){
	UINT32 val = value;
	OS_CtrlQPush(&val, sizeof(UINT32));
}

UINT32 OS_CtrlQPop_nonBlocking(void* data, int size){
	return read(OS_QIn[CTRL], data, size);
}

/****************/
/* Info Mailbox */
/****************/

UINT32 OS_InfoQPush(void* data, int size){
	return write(OS_QOut[INFO], data, size);
}

UINT32 OS_InfoQPop(void* data, int size){
	int i=0;
	while(i < size){
		int res = read(OS_QIn[INFO], (char*)data+i, size-i);
		if(res>0) i+=res;
	}
	return i;
}

UINT32 OS_InfoQPop_UINT32(){
	UINT32 value;
	OS_InfoQPop(&value, sizeof(UINT32));
	return value;
}

void OS_InfoQPush_UINT32(UINT32 value){
	UINT32 val = value;
	OS_InfoQPush(&val, sizeof(UINT32));
}

UINT32 OS_InfoQPop_nonBlocking(void* data, int size){
	return read(OS_QIn[INFO], data, size);
}
