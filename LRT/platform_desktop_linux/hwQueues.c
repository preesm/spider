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

typedef enum{
	CTRL=0,
	INFO=1
} MailboxType;

typedef enum{
	IN=0,
	OUT=1
} FifoDir;

static int OS_QGRT[2][2];
int cpuId;

#define BASE_PATH "/home/jheulot/dev/"

void OS_QInit(){
	char tempStr[50];

	sprintf(tempStr, "%sCtrl_Grtto%d",BASE_PATH,cpuId);
	OS_QGRT[CTRL][IN] = open(tempStr, O_RDWR | O_NONBLOCK);
	if (OS_QGRT[CTRL][IN] == -1) {perror("open Ctrl_Grtto"); abort();}

	sprintf(tempStr, "%sCtrl_%dtoGrt",BASE_PATH,cpuId);
	OS_QGRT[CTRL][OUT] = open(tempStr, O_RDWR | O_NONBLOCK);
	if (OS_QGRT[CTRL][OUT] == -1) {perror("open Ctrl_toGrt"); abort();}

	sprintf(tempStr, "%sInfo_Grtto%d",BASE_PATH,cpuId);
	OS_QGRT[INFO][IN] = open(tempStr, O_RDWR | O_NONBLOCK);
	if (OS_QGRT[INFO][IN] == -1) {perror("open Info_Grtto"); abort();}

	sprintf(tempStr, "%sInfo_%dtoGrt",BASE_PATH,cpuId);
	OS_QGRT[INFO][OUT] = open(tempStr, O_RDWR | O_NONBLOCK);
	if (OS_QGRT[INFO][OUT] == -1) {perror("open Info_toGrt"); abort();}
}

/*******************/
/* Control Mailbox */
/*******************/

UINT32 OS_CtrlQPush(void* data, int size){
	return write(OS_QGRT[CTRL][OUT], data, size);
}

UINT32 OS_CtrlQPop(void* data, int size){
	int i=0;
	while(i < size){
		int res = read(OS_QGRT[CTRL][IN], (char*)data+i, size-i);
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
	return read(OS_QGRT[CTRL][IN], data, size);
}

/****************/
/* Info Mailbox */
/****************/

UINT32 OS_InfoQPush(void* data, int size){
	return write(OS_QGRT[INFO][OUT], data, size);
}

UINT32 OS_InfoQPop(void* data, int size){
	int i=0;
	while(i < size){
		int res = read(OS_QGRT[INFO][IN], (char*)data+i, size-i);
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
	return read(OS_QGRT[INFO][IN], data, size);
}
