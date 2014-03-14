
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/

#ifndef LAUNCHER_H_
#define LAUNCHER_H_

#include "Memory.h"
#include "../graphs/SRDAG/SRDAGGraph.h"
#include "../graphs/Base/BaseVertex.h"
#include <scheduling/Schedule/Schedule.h>
#include <scheduling/Schedule/BaseSchedule.h>
#include <scheduling/architecture/Architecture.h>
#include <tools/ExecutionStat.h>
#include <hwQueues.h>
#include <types.h>

typedef struct {
	UINT32 id;
	UINT32 size;
	UINT32 addr;
}LauncherFIFO;


class launcher {
private:

	UINT32 nbFIFOs;
	LauncherFIFO fifos[MAX_NB_FIFO];

	Memory sharedMem;
	UINT32 dataToSend[MAX_SLAVES][MAX_CTRL_DATA];
	UINT32 dataToSendCnt[MAX_SLAVES];
	UINT32 dataToReceive[MAX_SLAVES][MAX_CTRL_DATA];
	UINT32 dataToReceiveCnt[MAX_SLAVES];

	UINT32 jobDataToSend[MAX_SLAVES][MAX_JOB_DATA];
	UINT32 jobDataToSendCnt[MAX_SLAVES];
	UINT32 jobDataToReceive[MAX_SLAVES][MAX_JOB_DATA];
	UINT32 jobDataToReceiveCnt[MAX_SLAVES];

	int launchedSlaveNb;

	void initFifos(SRDAGGraph* graph, int nbSlaves);
	void initTasks(SRDAGGraph* graph, Schedule* schedule, int nbSlaves);
	void initTasks(SRDAGGraph* graph);
	void stopTasks(SRDAGGraph* graph, Schedule* schedule, int nbSlaves);
	void start(int nbSlaves);

public:
	launcher();

	void assignFIFOId(SRDAGGraph* graph, Architecture* arch);
	void clear();
	void createRealTimeGantt(Architecture *arch, SRDAGGraph *dag, const char *filePathName);
	void init(int nbSlaves);
	void launch(SRDAGGraph* graph, Architecture *archi);

	//
//	LauncherFIFO* addFIFO(UINT32 id, UINT32 size, UINT32 addr){
//		if (nbFIFOs >= MAX_NB_FIFO) exitWithCode(1060);
//		LauncherFIFO* fifo = &fifos[nbFIFOs++];
//		fifo->id = id;
//		fifo->size = size;
//		fifo->addr = addr;
//		return fifo;
//	}

	LauncherFIFO* addFIFO(SRDAGEdge* edge){
		if (nbFIFOs >= MAX_NB_FIFO) exitWithCode(1060);
		LauncherFIFO* fifo = &fifos[nbFIFOs];
		fifo->id = nbFIFOs;
		fifo->size = edge->getTokenRate() * DEFAULT_FIFO_SIZE;
		fifo->addr = sharedMem.alloc(edge->getTokenRate() * DEFAULT_FIFO_SIZE);

		edge->setFifoId(nbFIFOs);

		nbFIFOs++;

		return fifo;
	}

	UINT32 getNbFIFOs(){
		return nbFIFOs;
	}

	LauncherFIFO* getFIFO(UINT32 id){
		for (UINT32 i = 0; i < nbFIFOs; i++) {
			if (fifos[i].id == id)
				return &fifos[i];
		}
		return 0;
	}

	// Prepares the execution of a SRDAG or a group of actors (e.g. the configuration actors of a PiSDF).
	void prepare(SRDAGGraph* graph, Architecture *archi, Schedule* schedule, ExecutionStat* execStat);


	void prepareTasksInfo(SRDAGGraph* graph, UINT32 nbSlaves, BaseSchedule* schedule, bool isAM, ExecutionStat* execStat);

	/*
	 * Prepares the execution of configuration vertices. Note that the outputs will be written directly
	 * on the GlobalRT's queues and not on the shared memory.
	 */
	void prepareConfigExec(
			BaseVertex** configVertices,
			UINT32 nb_vertices,
			Architecture *archi,
			BaseSchedule* schedule,
			ExecutionStat* execStat);

	/*
	 * Gets the returning values of configuration vertices' executions,
	 * and resolves the corresponding parameters.
	 */
	void resolvePiSDFParameters(
			BaseVertex** configVertices,
			UINT32 nb_vertices,
			BaseSchedule* schedule,
			Architecture* archi);

	void launchWaitAck(int nbSlaves);

	void launch(int nbSlaves, bool clearAfterCompletion = false);
//	void launchJobs(UINT16 nbSlaves);
	void stop();
	void stopWOCheck();

	void toDot(const char* path, UINT32 slaveId);

	int getNbLaunchedSlave();

	void launchOnce(SRDAGGraph* graph, Architecture *archi, Schedule* schedule);
	void reset();

	void addDataToSend(int slave, void* data, int size);
	void addDataToReceive(int slave, void* data, int size);

	void addUINT32ToSend(int slave, UINT32 val, RTQueueType queue = RTCtrlQueue);
	void addUINT32ToReceive(int slave, UINT32 val);

	void flushDataToSend();
	void flushDataToReceive();

	UINT32 popExecInfo(UINT32 slaveId, UINT32* data);

	UINT32 rcvData(UINT32 slave, UINT32 msgType, UINT32* data);

	void resolveParameters(SRDAGGraph* dag, UINT32 nbSlaves);

	void sendClearTasks(int nbSlaves);
};

inline int launcher::getNbLaunchedSlave(){
	return launchedSlaveNb;
}
#endif /* LAUNCHER_H_ */
