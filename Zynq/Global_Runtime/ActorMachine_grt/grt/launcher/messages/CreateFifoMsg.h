/*
 * CreateFifoMsg.h
 *
 *  Created on: Jun 26, 2013
 *      Author: jheulot
 */

#ifndef CREATEFIFOMSG_H_
#define CREATEFIFOMSG_H_

#include "LRTMsg.h"
#include <types.h>
#include "../Memory.h"
#include "../launcher.h"
#include "../../graphs/SRDAG/SRDAGGraph.h"
#include "../../graphs/SRDAG/SRDAGEdge.h"

class CreateFifoMsg: public LRTMsg {
private:
	UINT32 fifoID;
	UINT32 fifoAdd;
	UINT32 fifoSize;

public:
	CreateFifoMsg():fifoID(0),fifoAdd(0),fifoSize(0){};
	CreateFifoMsg(SRDAGGraph* graph, SRDAGEdge* edge, Memory *mem);

	void send(int LRTID);
	int prepare(int* data, int offset);
	void prepare(int slave, launcher* l);
};

#endif /* CREATEFIFOMSG_H_ */
