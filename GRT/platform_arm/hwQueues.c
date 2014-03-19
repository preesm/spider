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
	CTRL,
	INFO
} MailboxType;

static int OS_QInArmLrt[2], OS_QOutArmLrt[2];
static int OS_QUBlaze[NB_MAX_CTRLQ-1][2];


void OS_QInit(int nbSlaves){
	int i, flags;
	OS_QInArmLrt[CTRL] =  open("/root/Ctrl_0toGrt", O_RDWR);
	OS_QOutArmLrt[CTRL] = open("/root/Ctrl_Grtto0", O_RDWR);
	flags = fcntl(OS_QInArmLrt[CTRL], F_GETFL, 0);
	fcntl(OS_QInArmLrt[CTRL], F_SETFL, flags | O_NONBLOCK);

	OS_QInArmLrt[INFO] =  open("/root/Info_0toGrt", O_RDWR);
	OS_QOutArmLrt[INFO] = open("/root/Info_Grtto0", O_RDWR);
	flags = fcntl(OS_QInArmLrt[INFO], F_GETFL, 0);
	fcntl(OS_QInArmLrt[INFO], F_SETFL, flags | O_NONBLOCK);

	OS_QUBlaze[0][CTRL] = open("/dev/40000000.mailbox_ctrl_1", O_RDWR);
	if(OS_QUBlaze[0][CTRL] < 0) printf("Error openning /dev/40000000.mailbox_ctrl_1\n");

	OS_QUBlaze[0][INFO] = open("/dev/40010000.mailbox_info_1", O_RDWR);
	if(OS_QUBlaze[0][INFO] < 0) printf("Error openning /dev/40010000.mailbox_info_1\n");

	OS_QUBlaze[1][CTRL] = open("/dev/40020000.mailbox_ctrl_2", O_RDWR);
	if(OS_QUBlaze[1][CTRL] < 0) printf("Error openning /dev/40020000.mailbox_ctrl_2\n");

	OS_QUBlaze[1][INFO] = open("/dev/40030000.mailbox_info_2", O_RDWR);
	if(OS_QUBlaze[1][INFO] < 0) printf("Error openning /dev/40030000.mailbox_info_2\n");

	OS_QUBlaze[2][CTRL] = open("/dev/40040000.mailbox_ctrl_3", O_RDWR);
	if(OS_QUBlaze[2][CTRL] < 0) printf("Error openning /dev/40040000.mailbox_ctrl_3\n");

	OS_QUBlaze[2][INFO] = open("/dev/40050000.mailbox_info_3", O_RDWR);
	if(OS_QUBlaze[2][INFO] < 0) printf("Error openning /dev/40050000.mailbox_info_3\n");

	OS_QUBlaze[3][CTRL] = open("/dev/40060000.mailbox_ctrl_4", O_RDWR);
	if(OS_QUBlaze[3][CTRL] < 0) printf("Error openning /dev/40060000.mailbox_ctrl_4\n");

	OS_QUBlaze[3][INFO] = open("/dev/40070000.mailbox_info_4", O_RDWR);
	if(OS_QUBlaze[3][INFO] < 0) printf("Error openning /dev/40070000.mailbox_info_4\n");

//	OS_QUBlaze[4][CTRL] = open("/dev/40080000.mailbox_ctrl_5", O_RDWR);
//	if(OS_QUBlaze[4][CTRL] < 0) printf("Error openning /dev/40080000.mailbox_ctrl_5\n");
//
//	OS_QUBlaze[4][INFO] = open("/dev/40090000.mailbox_info_5", O_RDWR);
//	if(OS_QUBlaze[4][INFO] < 0) printf("Error openning /dev/40090000.mailbox_info_5\n");


	UINT32 c;
	for(i=0; i<nbSlaves; i++){
		while(OS_CtrlQPop_nonBlocking(i, &c, sizeof(UINT32))==sizeof(UINT32));
		while(OS_InfoQPop_nonBlocking(i, &c, sizeof(UINT32))==sizeof(UINT32));
	}

//	todo auto
}

/* Ctrl Mailbox */
UINT32 OS_CtrlQPush(int id, void* data, int size){
	int file;
	if(id == 0)
		file = OS_QOutArmLrt[CTRL];
	else
		file = OS_QUBlaze[id-1][CTRL];

	int i=0;
	while(i < size){
		int res = write(file, (char*)data+i, size-i);
		if(res>0) i+=res;
	}
	return i;
}

UINT32 OS_CtrlQPop(int id, void* data, int size){
	int file;
	if(id == 0)
		file = OS_QInArmLrt[CTRL];
	else
		file = OS_QUBlaze[id-1][CTRL];

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
	int file;
	if(id == 0)
		file = OS_QInArmLrt[CTRL];
	else
		file = OS_QUBlaze[id-1][CTRL];

	return read(file, (char*)data, size);
}

/* Info Mailbox */
UINT32 OS_InfoQPush(int id, void* data, int size){
	int file;
	if(id == 0)
		file = OS_QOutArmLrt[INFO];
	else
		file = OS_QUBlaze[id-1][INFO];

	int i=0;
	while(i < size){
		int res = write(file, (char*)data+i, size-i);
		if(res>0) i+=res;
	}
	return i;
}

UINT32 OS_InfoQPop(int id, void* data, int size){
	int file;
	if(id == 0)
		file = OS_QInArmLrt[INFO];
	else
		file = OS_QUBlaze[id-1][INFO];

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
	int file;
	if(id == 0)
		file = OS_QInArmLrt[INFO];
	else
		file = OS_QUBlaze[id-1][INFO];

	return read(file, (char*)data, size);
}
