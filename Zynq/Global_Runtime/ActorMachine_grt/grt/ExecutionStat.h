/*
 * ExecutionStat.h
 *
 *  Created on: Jan 23, 2013
 *      Author: jheulot
 */

#ifndef EXECUTIONSTAT_H_
#define EXECUTIONSTAT_H_

typedef enum{
	Default		  = 0,
	AMManagement  = 1,
	Scheduling	  = 2,
	FifoCheck	  = 3,
	DataTransfert = 4,
	CtrlFifoHandling = 5,
	Action		  = 6,
	MaxMonitor	  = Action+6,
} Timings;

class ExecutionStat {
public:
	unsigned int flowScheduleTime;
	unsigned int listScheduleTime;
	unsigned int srDAGTransfTime;
	unsigned int jobTransfTime;
	unsigned int flowMappingTime;
	unsigned int flowMakespan;
	unsigned int listMakespan;
	unsigned int flowThroughput;
	unsigned int listThroughput;
	unsigned int criticalPath;
	unsigned int t1Latency;
	unsigned int nbFunction;
	unsigned int nbRunningCore;
	unsigned int nbSlavesFunction[MAX_CSDAG_VERTICES];

	unsigned int nbSRDAGVertices;
	unsigned int nbSRDAGEdges;
	unsigned int fifoNb;

	unsigned int nbAMVertices[MAX_SLAVES];
	unsigned int nbAMActions[MAX_SLAVES];
	unsigned int nbAMConds[MAX_SLAVES];

	unsigned int memAllocated;
	unsigned int msgLength[MAX_SLAVES];

	unsigned int timings[MAX_SLAVES][MAX_CSDAG_VERTICES+6];


	ExecutionStat(){
		flowScheduleTime = listScheduleTime = srDAGTransfTime = 0;
		jobTransfTime = flowMappingTime = flowMakespan = 0;
		listMakespan = flowThroughput = listThroughput = 0;
		criticalPath = t1Latency = nbFunction=0;
	};

	virtual ~ExecutionStat(){};

	inline unsigned int getFlowExecutionTime(){
		return flowScheduleTime+srDAGTransfTime+jobTransfTime+flowMappingTime;
	}

	inline unsigned int getListExecutionTime(){
		return listScheduleTime+srDAGTransfTime;
	}

	void average(ExecutionStat* tab, int nb){
		double temp1=0, temp2=0, temp3=0, temp4=0, temp5=0;
		for(int i=0; i<nb; i++){
			temp1 += tab[i].flowScheduleTime;
			temp2 += tab[i].listScheduleTime;
			temp3 += tab[i].srDAGTransfTime;
			temp4 += tab[i].jobTransfTime;
			temp5 += tab[i].flowMappingTime;
		}
		flowScheduleTime = temp1/nb;
		listScheduleTime = temp2/nb;
		srDAGTransfTime  = temp3/nb;
		jobTransfTime    = temp4/nb;
		flowMappingTime  = temp5/nb;
	}
};

#endif /* EXECUTIONSTAT_H_ */
