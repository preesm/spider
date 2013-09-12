/*
 * ClearFifoMsg.h
 *
 *  Created on: Jun 26, 2013
 *      Author: jheulot
 */

#ifndef CLEARFIFOMSG_H_
#define CLEARFIFOMSG_H_

#include "LRTMsg.h"
#include "../launcher.h"
#include <types.h>

class ClearFifoMsg: public LRTMsg {
private:
	INT32 fifoID;

public:
	ClearFifoMsg():fifoID(0){};
	ClearFifoMsg(INT32 fifoID);

	void send(int LRTID);
	int prepare(int* data, int offset);
	void prepare(int slave, launcher* launch);
};

#endif /* CLEARFIFOMSG_H_ */
