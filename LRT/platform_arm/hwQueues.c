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
#include "swfifoMngr.h"

#define IN_CTRL_QUEUE_BASE		0x20000000
#define OUT_CTRL_QUEUE_BASE		0x20000600

#define MBOX_SIZE				1024


typedef enum{
	CTRL,
	INFO
} MailboxType;

static LRT_FIFO_HNDLE RTQueue[nbQueueTypes][2];
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



void RTQueuesInit(){
	create_swfifo(&(RTQueue[RTCtrlQueue][RTInputQueue]), MBOX_SIZE, IN_CTRL_QUEUE_BASE);
	flush_swfifo(&(RTQueue[RTCtrlQueue][RTInputQueue]));
//	RTQueue[RTInfoQueue][RTInputQueue] =
//	RTQueue[RTJobQueue][RTInputQueue] =
//
	create_swfifo(&(RTQueue[RTCtrlQueue][RTOutputQueue]), MBOX_SIZE, OUT_CTRL_QUEUE_BASE);
	flush_swfifo(&(RTQueue[RTCtrlQueue][RTOutputQueue]));
//	RTQueue[RTInfoQueue][RTOutputQueue] =
//	RTQueue[RTJobQueue][RTOutputQueue] =
}


UINT32 RTQueuePush(RTQueueType queueType, void* data, int size){
	write_output_swfifo(&RTQueue[queueType][RTOutputQueue], size, data);
	return size;
}


UINT32 RTQueuePush_UINT32(RTQueueType queueType, UINT32 value){
	return RTQueuePush(queueType, &value, sizeof(UINT32));
}


UINT32 RTQueuePop(RTQueueType queueType, void* data, int size){
	read_input_swfifo(&RTQueue[queueType][RTInputQueue], size, data);
	return size;
}


UINT32 RTQueuePop_UINT32(RTQueueType queueType){
	UINT32 data;
	RTQueuePop(queueType, &data, sizeof(UINT32));
	return data;
}


UINT32 RTQueueNonBlockingPop(RTQueueType queueType, void* data, int size){
	if(check_input_swfifo(&RTQueue[queueType][RTInputQueue], size)){
		read_input_swfifo(&RTQueue[queueType][RTInputQueue], size, data);
		return size;
	}
	else
		return 0;
}
