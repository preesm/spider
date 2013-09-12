/*
 * StopTaskMsg.cpp
 *
 *  Created on: Jun 26, 2013
 *      Author: jheulot
 */

#include "StopTaskMsg.h"
#include <hwQueues.h>

StopTaskMsg::StopTaskMsg(INT32 _TaskID, INT32 _VectorID): TaskID(_TaskID), VectorID(_VectorID) {
}

void StopTaskMsg::send(int LRTID){
	OS_CtrlQPushInt(LRTID, MSG_STOP_TASK);
	OS_CtrlQPushInt(LRTID, TaskID);
	OS_CtrlQPushInt(LRTID, VectorID);

	OS_CtrlQPopInt(LRTID);
}

void StopTaskMsg::sendWOCheck(int LRTID){
	OS_CtrlQPushInt(LRTID, MSG_STOP_TASK);
	OS_CtrlQPushInt(LRTID, TaskID);
	OS_CtrlQPushInt(LRTID, VectorID);
}

int StopTaskMsg::prepare(int* data, int offset){
	int size = 0;
	data[offset + size++] = MSG_STOP_TASK;
	data[offset + size++] = TaskID;
	data[offset + size++] = VectorID;
	return size;
}
