/*
 * lrt_monitor.c
 *
 *  Created on: Sep 2, 2013
 *      Author: jheulot
 */

#include <zynq_time.h>
#include <print.h>
#include <types.h>
#include <string.h>
#include <gpio.h>
#include <hwQueues.h>

#include "lrt_definitions.h"
#include "lrt_prototypes.h"

static UINT32 monitorTimes[MaxMonitor];
static MonitorAction currentAction;

void initMonitor(){
	int i;
	for(i=0; i<MaxMonitor; i++) monitorTimes[i]=0;
	OS_TimeInit();
	OS_TimeStop();
	OS_TimeReset();
	initGpio();
}

void resetMonitor(){
	memset(monitorTimes, 0, sizeof(monitorTimes));
	currentAction = Default;
	OS_TimeStop();
	OS_TimeReset();
	OS_TimeStart();
}

MonitorAction switchMonitor(MonitorAction action){
	MonitorAction last = action;
	OS_TimeStop();
	if(action >= MaxMonitor) exitWithCode(1014);
	monitorTimes[currentAction] += OS_TimeGetValue();
	currentAction = action;
	if(currentAction >= Action) setLed(1); else setLed(0);
	OS_TimeReset();
	OS_TimeStart();
	return last;
}

void printResult(){
	int i=0;
	UINT32 totalTime = 0;
	UINT32 val;

	OS_TimeStop();
	monitorTimes[currentAction] += OS_TimeGetValue();

	for(i=0; i<MaxMonitor; i++){
		totalTime += monitorTimes[i];
	}

	for(i=0; i<MaxMonitor; i++){
		val = 100*monitorTimes[i];
		val /= totalTime;

		OS_InfoQPush_UINT32(val);
	}

//		if(monitorTimes[i] != 0){
//			zynq_puts("\nAction "); zynq_putdec(i-Action);
//			zynq_puts(" : "); 		zynq_putdec(100*monitorTimes[i]/totalTime);
//		}
//	}
//	zynq_puts("\n\n");
}
