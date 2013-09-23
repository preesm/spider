/*
 * Timer.h
 *
 *  Created on: Jan 21, 2013
 *      Author: jheulot
 */

#ifndef TIMER_H_
#define TIMER_H_

#ifndef ZYNQ
#include <time.h>
#endif

class Timer {
private:
#ifdef ZYNQ
	int file;
#else
	time_t start;
//	timeval start;
#endif


public:
	Timer();
	virtual ~Timer();

	unsigned int getValue();
	unsigned int print(const char* txt);
	unsigned int printAndReset(const char* txt);
	unsigned int getValueAndReset();
	void resetAndStart();
};

extern Timer timer;

#endif /* TIMER_H_ */
