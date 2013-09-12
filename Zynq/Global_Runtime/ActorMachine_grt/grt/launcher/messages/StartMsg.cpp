/*
 * StartMsg.cpp
 *
 *  Created on: Jun 26, 2013
 *      Author: jheulot
 */

#include "StartMsg.h"
#include <hwQueues.h>

StartMsg::StartMsg() {
}

void StartMsg::send(int LRTID){
	OS_CtrlQPushInt(LRTID, MSG_START_SCHED);
}

int StartMsg::prepare(int* data, int offset){
	int size = 0;
	data[offset + size++] = MSG_START_SCHED;
	return size;
}

void StartMsg::prepare(int slave, launcher* launch){
	launch->addUINT32ToSend(slave, MSG_START_SCHED);
//	launch->addUINT32ToReceive(slave, MSG_START_SCHED);
}
