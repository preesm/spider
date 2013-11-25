/*
 * time.c
 *
 *  Created on: Aug 28, 2013
 *      Author: jheulot
 */

#include <xtmrctr.h>
#include <zynq_time.h>
#include <xparameters.h>

static XTmrCtr timer;

void OS_TimeInit(){
    XTmrCtr_Initialize(&timer, XPAR_TMRCTR_0_DEVICE_ID);
	XTmrCtr_Stop(&timer, 0);
}

void OS_TimeReset(){
	XTmrCtr_Reset (&timer, 0);
}

void OS_TimeStart(){
	XTmrCtr_Start (&timer, 0);
}

void OS_TimeStop(){
	XTmrCtr_Stop(&timer, 0);
}

UINT32 OS_TimeGetValue(){
	return XTmrCtr_GetValue (&timer, 0);
}
