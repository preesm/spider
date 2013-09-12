/*
 * AMCond.h
 *
 *  Created on: Jun 17, 2013
 *      Author: jheulot
 */

#ifndef AMCOND_H_
#define AMCOND_H_

typedef enum{
	COND_UNINITIALIZED=0,
	FIFO_IN=1,
	FIFO_OUT=2
}COND_TYPE;

class AMCond {
public:
	COND_TYPE type;

	union{
		/* FIFO Condition specific attributes */
		struct {
			int id;
			int size;
		} fifo;
	};
public:
	AMCond(){
		type = COND_UNINITIALIZED;
	}

	/* FIFO Condition specific constructor */
	AMCond(COND_TYPE fifoType, int id, int size){
		type = fifoType;
		fifo.id = id;
		fifo.size = size;
	}
};

#endif /* AMCOND_H_ */
