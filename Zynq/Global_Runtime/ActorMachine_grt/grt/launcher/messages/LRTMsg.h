/*
 * LRTMessage.h
 *
 *  Created on: Jun 26, 2013
 *      Author: jheulot
 */

#ifndef LRTMESSAGE_H_
#define LRTMESSAGE_H_

typedef enum{
	MSG_CREATE_TASK = 1,
	MSG_CREATE_FIFO = 2,
	MSG_START_SCHED = 3,
	MSG_STOP_TASK = 4,
	MSG_CLEAR_FIFO = 5,
	MSG_CURR_VERTEX_ID = 6,
} MSG_TYPE;

#define MAX_MSG_LENGTH 1000

class LRTMsg {
protected:


public:
	LRTMsg(){};
	virtual ~LRTMsg(){};

	virtual void send(int LRTid) = 0;
	virtual int prepare(int* data, int offset) = 0;

};

#endif /* LRTMESSAGE_H_ */
