/*
 * time.c
 *
 *  Created on: Aug 28, 2013
 *      Author: jheulot
 */

#include "zynq_time.h"
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <timerArm.h>

static int timerFile;

void OS_TimeInit(){
	timerFile = open("/dev/timerArm", O_RDWR, S_IRUSR | S_IWUSR);
	if(timerFile == -1){
		fprintf(stderr, "Cannot open /dev/timerArm\n");
		abort();
	}
}

void OS_TimeReset(){
	ioctl(timerFile,TIMERARM_IOCT_RESET);
}

void OS_TimeStart(){
	ioctl(timerFile,TIMERARM_IOCT_START);
}

void OS_TimeStop(){
	ioctl(timerFile,TIMERARM_IOCT_STOP);
}

UINT32 OS_TimeGetValue(){
	UINT32 elapsedTime=0;
	ioctl(timerFile,TIMERARM_IOCT_GETVALUE, &elapsedTime);
	return elapsedTime/6.66;
}
