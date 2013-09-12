/*
 * HWQueues.c
 *
 *  Created on: Jun 12, 2013
 *      Author: jheulot
 */

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
