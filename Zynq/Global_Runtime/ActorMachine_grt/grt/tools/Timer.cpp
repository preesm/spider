/*
 * Timer.cpp
 *
 *  Created on: Jan 21, 2013
 *      Author: jheulot
 */

#include "Timer.h"

#ifdef ZYNQ
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include <linux/types.h>
#include <sys/ioctl.h>

#include <timerArm_module/timerArm.h>
#include <sys/time.h>

#include "SchedulingError.h"

#else
#include <sys/time.h>
#include <unistd.h>
#include <cstdio>
#endif

Timer timer;

Timer::Timer() {
#ifdef ZYNQ
	file = open("/dev/timerArm", O_RDWR, S_IRUSR | S_IWUSR);
	if(file == -1){
		fprintf(stderr, "Cannot open /dev/timerArm\n");
		exitWithCode(1046);
	}
#endif
}

Timer::~Timer() {
	// TODO Auto-generated destructor stub
}

unsigned int Timer::getValue(){
	unsigned int time;
#ifdef ZYNQ
	ioctl(file,TIMERARM_IOCT_GETVALUE, &time);
#else
	timeval end, val;
    gettimeofday(&end, NULL);
    timersub(&end, &start, &val);
    time = (val.tv_sec*1000000 + val.tv_usec)*667;
#endif
	return time;
}

void Timer::resetAndStart(){
#ifdef ZYNQ
	ioctl(file,TIMERARM_IOCT_RESET);
	ioctl(file,TIMERARM_IOCT_START);
#else
    gettimeofday(&start, NULL);
#endif
}

unsigned int  Timer::print(const char* txt){
	unsigned int time;
#ifdef ZYNQ
	ioctl(file,TIMERARM_IOCT_STOP);
	ioctl(file,TIMERARM_IOCT_GETVALUE, &time);
#else
	timeval end, val;
    gettimeofday(&end, NULL);
    timersub(&end, &start, &val);
    time = (val.tv_sec*1000000 + val.tv_usec)*667;
#endif
	printf("%s: %d\t(%fms)\n", txt, time, time/667000.0);
#ifdef ZYNQ
	ioctl(file,TIMERARM_IOCT_START);
#endif
	return time;
}

unsigned int Timer::getValueAndReset(){
	unsigned int time;
#ifdef ZYNQ
	ioctl(file,TIMERARM_IOCT_STOP);
	ioctl(file,TIMERARM_IOCT_GETVALUE, &time);
	ioctl(file,TIMERARM_IOCT_RESET);
	ioctl(file,TIMERARM_IOCT_START);
#else
	timeval end, val;
    gettimeofday(&end, NULL);
    timersub(&end, &start, &val);
    time = (val.tv_sec*1000000 + val.tv_usec)*667;
    gettimeofday(&start, NULL);
#endif
	return time;
}

unsigned int Timer::printAndReset(const char* txt){
	unsigned int time;
#ifdef ZYNQ
	ioctl(file,TIMERARM_IOCT_STOP);
	ioctl(file,TIMERARM_IOCT_GETVALUE, &time);
#else
	timeval end, val;
    gettimeofday(&end, NULL);
    timersub(&end, &start, &val);
    time = (val.tv_sec*1000000 + val.tv_usec)*667;
#endif
	printf("%s: %d\t(%fms)\n", txt, time, time/667000.0);
#ifdef ZYNQ
	ioctl(file,TIMERARM_IOCT_RESET);
	ioctl(file,TIMERARM_IOCT_START);
#else
    gettimeofday(&start, NULL);
#endif
	return time;
}

