/*
 * HWQueues.h
 *
 *  Created on: Jun 12, 2013
 *      Author: jheulot
 */

#ifndef HWQUEUES_H_
#define HWQUEUES_H_

#define NB_MAX_CTRLQ 8

#include <stdio.h>
#include <fcntl.h>

#include "types.h"

void OS_QInit(int nbSlaves);

UINT32 OS_CtrlQPush(INT32 id, void* data, int size);
UINT32 OS_CtrlQPop(INT32 id, void* data, int size);
UINT32 OS_CtrlQPop_nonBlocking(INT32 id, void* data, int size);
UINT32 OS_CtrlQPushInt(int id, unsigned int data);
UINT32 OS_CtrlQPopInt(int id);

UINT32 OS_InfoQPush(INT32 id, void* data, int size);
UINT32 OS_InfoQPop(INT32 id, void* data, int size);
UINT32 OS_InfoQPop_nonBlocking(INT32 id, void* data, int size);
UINT32 OS_InfoQPushInt(int id, unsigned int data);
UINT32 OS_InfoQPopInt(int id);

#endif /* HWQUEUES_H_ */
