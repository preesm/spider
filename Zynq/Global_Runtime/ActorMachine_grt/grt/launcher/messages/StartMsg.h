/*
 * StartMsg.h
 *
 *  Created on: Jun 26, 2013
 *      Author: jheulot
 */

#ifndef STARTMSG_H_
#define STARTMSG_H_

#include "LRTMsg.h"
#include "../launcher.h"

class StartMsg: public LRTMsg {
public:
	StartMsg();

	void send(int LRTID);
	int prepare(int* data, int offset);
	void prepare(int slave, launcher* l);
};

#endif /* STARTMSG_H_ */
