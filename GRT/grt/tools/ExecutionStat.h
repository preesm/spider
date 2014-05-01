/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

#ifndef EXECUTIONSTAT_H_
#define EXECUTIONSTAT_H_

#include <graphs/PiSDF/PiSDFAbstractVertex.h>

#define MAX_STATS_VERTICES 100

typedef struct{
	int globalEndTime;
	int schedulingTime;
	int graphTransfoTime;

	UINT32 actorTimes[MAX_STATS_VERTICES];
	PiSDFAbstractVertex* actors[MAX_STATS_VERTICES];
	int nbActor;

	UINT32 explodeTime;
	UINT32 implodeTime;
	UINT32 roundBufferTime;
}ExecutionStat;

//typedef enum{
//	Default		  = 0,
//	AMManagement  = 1,
//	Scheduling	  = 2,
//	FifoCheck	  = 3,
//	DataTransfert = 4,
//	CtrlFifoHandling = 5,
//	Action		  = 6,
//	MaxMonitor	  = Action+6,
//} Timings;
//
//class ExecutionStat {
//public:
//	unsigned int listScheduleTime;
//	unsigned int srDAGTransfTime;
//	unsigned int listMakespan;
//	unsigned int listThroughput;
//	unsigned int criticalPath;
//	unsigned int t1Latency;
//	unsigned int nbFunction;
//	unsigned int nbRunningCore;
//	unsigned int nbSlavesFunction[MAX_CSDAG_VERTICES];
//
//	unsigned int nbSRDAGVertices;
//	unsigned int nbSRDAGEdges;
//	unsigned int fifoNb;
//
//	unsigned int nbAMVertices[MAX_SLAVES];
//	unsigned int nbAMActions[MAX_SLAVES];
//	unsigned int nbAMConds[MAX_SLAVES];
//
//	unsigned int memAllocated;
//	unsigned int msgLength[MAX_SLAVES];
//
//	unsigned int timings[MAX_SLAVES][MAX_CSDAG_VERTICES+6];
//
//
//	ExecutionStat(){
//		listScheduleTime = srDAGTransfTime = 0;
//		nbSRDAGVertices = nbSRDAGEdges = memAllocated = 0;
//		nbRunningCore = fifoNb = 0;
//		listMakespan = listThroughput = 0;
//		criticalPath = t1Latency = nbFunction=0;
//	};
//
//	virtual ~ExecutionStat(){};
//
//	inline unsigned int getListExecutionTime(){
//		return listScheduleTime+srDAGTransfTime;
//	}
//
//	void average(ExecutionStat* tab, int nb){
//		double temp1=0, temp2=0;
//		for(int i=0; i<nb; i++){
//			temp1 += tab[i].listScheduleTime;
//			temp2 += tab[i].srDAGTransfTime;
//		}
//		listScheduleTime = temp1/nb;
//		srDAGTransfTime  = temp2/nb;
//	}
//};

#endif /* EXECUTIONSTAT_H_ */
