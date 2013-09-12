/*
 * CreateFifoMsg.cpp
 *
 *  Created on: Jun 26, 2013
 *      Author: jheulot
 */

#include "CreateFifoMsg.h"
#include "../../graphs/SRDAG/SRDAGEdge.h"
#include "../../graphs/SRDAG/SRDAGGraph.h"
#include "../Memory.h"
#include <hwQueues.h>

CreateFifoMsg::CreateFifoMsg(SRDAGGraph* graph, SRDAGEdge* edge, Memory *mem){
	fifoID = graph->getEdgeIndex(edge);
	fifoSize = edge->getTokenRate()+1;
	fifoAdd = mem->alloc(fifoSize);
}

void CreateFifoMsg::send(int LRTID){
	OS_CtrlQPushInt(LRTID, MSG_CREATE_FIFO);
	OS_CtrlQPushInt(LRTID, fifoID);
	OS_CtrlQPushInt(LRTID, fifoSize);
	OS_CtrlQPushInt(LRTID, fifoAdd);
}

int CreateFifoMsg::prepare(int* data, int offset){
	int size = 0;
	data[offset + size++] = MSG_CREATE_FIFO;
	data[offset + size++] = fifoID;
	data[offset + size++] = fifoSize;
	data[offset + size++] = fifoAdd;
	return size;
}

void CreateFifoMsg::prepare(int slave, launcher* launch){
	launch->addUINT32ToSend(slave, MSG_CREATE_FIFO);
	launch->addUINT32ToSend(slave, fifoID);
	launch->addUINT32ToSend(slave, fifoSize);
	launch->addUINT32ToSend(slave, fifoAdd);
//	launch->addUINT32ToReceive(slave, MSG_CREATE_FIFO);
}
