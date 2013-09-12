/*
 * StopTaskMsg.h
 *
 *  Created on: Jun 26, 2013
 *      Author: jheulot
 */

#ifndef STOPTASKMSG_H_
#define STOPTASKMSG_H_

#include "LRTMsg.h"
#include <types.h>

class StopTaskMsg: public LRTMsg {
	INT32 TaskID;
	INT32 VectorID;

public:
	StopTaskMsg(){};
	StopTaskMsg(INT32 TaskID, INT32 VectorID);

	void send(int LRTID);
	void sendWOCheck(int LRTID);

	int prepare(int* data, int offset);
};

#endif /* STOPTASKMSG_H_ */
