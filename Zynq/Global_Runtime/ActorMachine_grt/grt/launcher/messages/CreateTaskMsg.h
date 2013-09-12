/*
 * TaskMsg.h
 *
 *  Created on: Jun 26, 2013
 *      Author: jheulot
 */

#ifndef TASKMSG_H_
#define TASKMSG_H_

#include "LRTMsg.h"
#include <types.h>
#include "../../graphs/SRDAG/SRDAGGraph.h"
#include "../../graphs/SRDAG/SRDAGEdge.h"
#include "../../graphs/ActorMachine/AMGraph.h"
#include "../../graphs/Schedule/Schedule.h"
#include "../../SchedulerDimensions.h"
#include "../launcher.h"

class CreateTaskMsg: public LRTMsg {
private:
	INT32 taskID;
	INT32 functID;
	INT32 nbFifoIn;
	INT32 nbFifoOut;
	INT32 FifosInID[MAX_NB_FIFO];
	INT32 FifosOutID[MAX_NB_FIFO];

	/* Actor Machine */
	INT32 initStateAM;
	AMGraph AM;

public:
	CreateTaskMsg():taskID(0),functID(0),nbFifoIn(0),nbFifoOut(0),initStateAM(0){};
	CreateTaskMsg(SRDAGGraph* graph, SRDAGVertex* vertex);
	CreateTaskMsg(SRDAGGraph* graph, Schedule* schedule, int slave);

	void send(int LRTID);
	int prepare(int* data, int offset);
	void prepare(int slave, launcher* launch);
	void toDot(const char* path);
	AMGraph* getAm();
};

#endif /* TASKMSG_H_ */
