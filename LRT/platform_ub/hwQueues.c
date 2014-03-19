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

#include <xmbox.h>
#include "xparameters.h"

#include "types.h"
#include "hwQueues.h"
#include "platform.h"
#include "print.h"

typedef enum{
	CTRL,
	INFO
} MailboxType;

static XMbox mbox[2];
int cpuId;

void OS_QInit(){
	XMbox_Config cfg;
	int Status;

	/* Control Mailbox */
	cfg.DeviceId = 0;
	cfg.UseFSL = 0;

	Status = XMbox_CfgInitialize(&(mbox[CTRL]), &cfg, MBOX_CTRL_ADDR);
	if(Status != XST_SUCCESS){
		zynq_puts("Error in XMbox_CfgInitialize\n");
	}

	/* Info Mailbox */
	cfg.DeviceId = 0;
	cfg.UseFSL = 0;

	Status = XMbox_CfgInitialize(&(mbox[INFO]), &cfg, MBOX_INFO_ADDR);
	if(Status != XST_SUCCESS){
		zynq_puts("Error in XMbox_CfgInitialize\n");
	}

}

/*******************/
/* Control Mailbox */
/*******************/

UINT32 OS_CtrlQPush(void* data, int size){
	XMbox_WriteBlocking(&(mbox[CTRL]), data, size);
	return size;
}

UINT32 OS_CtrlQPop(void* data, int size){
	XMbox_ReadBlocking(&(mbox[CTRL]), data, size);
	return size;
}

UINT32 OS_CtrlQPop_UINT32(){
	UINT32 value;
	OS_CtrlQPop(&value, sizeof(UINT32));
	return value;
}

void OS_CtrlQPush_UINT32(UINT32 value){
	OS_CtrlQPush(&value, sizeof(UINT32));
}

UINT32 OS_CtrlQPop_nonBlocking(void* data, int size){
	if(XMbox_IsEmpty(&(mbox[CTRL]))) return 0;
	else return OS_CtrlQPop(data, size);
}

/****************/
/* Info Mailbox */
/****************/

UINT32 OS_InfoQPush(void* data, int size){
	XMbox_WriteBlocking(&(mbox[INFO]), data, size);
	return size;
}

UINT32 OS_InfoQPop(void* data, int size){
	XMbox_ReadBlocking(&(mbox[INFO]), data, size);
	return size;
}

UINT32 OS_InfoQPop_UINT32(){
	UINT32 value;
	OS_InfoQPop(&value, sizeof(UINT32));
	return value;
}

void OS_InfoQPush_UINT32(UINT32 value){
	OS_InfoQPush(&value, sizeof(UINT32));
}

UINT32 OS_InfoQPop_nonBlocking(void* data, int size){
	if(XMbox_IsEmpty(&(mbox[INFO]))) return 0;
	else return OS_InfoQPop(data, size);
}
