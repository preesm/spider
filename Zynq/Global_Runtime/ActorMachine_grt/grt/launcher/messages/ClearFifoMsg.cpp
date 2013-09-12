/*
 * ClearFifoMsg.cpp
 *
 *  Created on: Jun 26, 2013
 *      Author: jheulot
 */

#include "ClearFifoMsg.h"
#include <hwQueues.h>

ClearFifoMsg::ClearFifoMsg(INT32 _fifoID): fifoID(_fifoID) {
}

void ClearFifoMsg::send(int LRTID){
	OS_CtrlQPushInt(LRTID, MSG_CLEAR_FIFO);
	OS_CtrlQPushInt(LRTID, fifoID);
}

int ClearFifoMsg::prepare(int* data, int offset){
	int size = 0;
	data[offset + size++] = MSG_CLEAR_FIFO;
	data[offset + size++] = fifoID;
	return size;
}

void ClearFifoMsg::prepare(int slave, launcher* launch){
	launch->addUINT32ToSend(slave, MSG_CLEAR_FIFO);
	launch->addUINT32ToSend(slave, fifoID);
//	launch->addUINT32ToReceive(slave, MSG_CLEAR_FIFO);
}
