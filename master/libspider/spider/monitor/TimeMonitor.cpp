/*
 * TimeMonitor.cpp
 *
 *  Created on: Dec 1, 2015
 *      Author: jheulot
 */

#include "TimeMonitor.h"
#include <platform.h>
#include <Launcher.h>

const char* TimeMonitor::spiderTaskName[9] = {
		"",
		"Graph handling",
		"Memory Allocation",
		"Task Scheduling",
		"Graph Optimization",
		"Tmp 0",
		"Tmp 1",
		"Tmp 2",
		"Tmp 3"
};

Time TimeMonitor::start = 0;

void TimeMonitor::startMonitoring(){
	if(start != 0)
		throw "Try to monitor 2 different things in the same time";
	start = Platform::get()->getTime();
}

void TimeMonitor::endMonitoring(TraceSpiderType type){
	if(start == 0)
		throw "End monitor with no starting point";
	Launcher::get()->sendTraceSpider(type, start, Platform::get()->getTime());
	start = 0;
}
