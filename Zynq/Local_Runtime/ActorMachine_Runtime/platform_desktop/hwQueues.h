/*
 * HWQueues.h
 *
 *  Created on: Jun 12, 2013
 *      Author: jheulot
 */

#ifndef HWQUEUES_H_
#define HWQUEUES_H_

#include "types.h"

void OS_QInit();

UINT32 OS_CtrlQPush(void* data, int size);
UINT32 OS_CtrlQPop(void* data, int size);
UINT32 OS_CtrlQPop_UINT32();
void OS_CtrlQPush_UINT32(UINT32 value);
UINT32 OS_CtrlQPop_nonBlocking(void* data, int size);

UINT32 OS_InfoQPush(void* data, int size);
UINT32 OS_InfoQPop(void* data, int size);
UINT32 OS_InfoQPop_UINT32();
void OS_InfoQPush_UINT32(UINT32 value);
UINT32 OS_InfoQPop_nonBlocking(void* data, int size);

#endif /* HWQUEUES_H_ */
