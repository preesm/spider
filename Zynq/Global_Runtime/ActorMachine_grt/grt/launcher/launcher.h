/*
 * launcher.h
 *
 *  Created on: May 31, 2013
 *      Author: jheulot
 */

#ifndef LAUNCHER_H_
#define LAUNCHER_H_

#include "Memory.h"
#include "../graphs/SRDAG/SRDAGGraph.h"
#include "../graphs/Schedule/Schedule.h"
#include "../ExecutionStat.h"

#include <types.h>

#define MAX_CTRL_DATA 2000

class launcher {
private:
	Memory sharedMem;
	UINT32 dataToSend[MAX_SLAVES][MAX_CTRL_DATA];
	UINT32 dataToSendCnt[MAX_SLAVES];
	UINT32 dataToReceive[MAX_SLAVES][MAX_CTRL_DATA];
	UINT32 dataToReceiveCnt[MAX_SLAVES];

	int launchedSlaveNb;

	void initFifos(SRDAGGraph* graph, int nbSlaves);
	void initTasks(SRDAGGraph* graph, Schedule* schedule, int nbSlaves);
	void initTasks(SRDAGGraph* graph);
	void stopTasks(SRDAGGraph* graph, Schedule* schedule, int nbSlaves);
	void start(int nbSlaves);

public:
	launcher(int nbSlaves);

	void launch(SRDAGGraph* graph, Architecture *archi);

	void prepare(SRDAGGraph* graph, Architecture *archi, Schedule* schedule, ExecutionStat* execStat);
	void launch(int nbSlaves);
	void stop();
	void stopWOCheck();

	int getNbLaunchedSlave();

	void launchOnce(SRDAGGraph* graph, Architecture *archi, Schedule* schedule);
	void reset();

	void addDataToSend(int slave, void* data, int size);
	void addDataToReceive(int slave, void* data, int size);

	void addUINT32ToSend(int slave, UINT32 val);
	void addUINT32ToReceive(int slave, UINT32 val);

	void flushDataToSend();
	void flushDataToReceive();
};

inline int launcher::getNbLaunchedSlave(){
	return launchedSlaveNb;
}
#endif /* LAUNCHER_H_ */
