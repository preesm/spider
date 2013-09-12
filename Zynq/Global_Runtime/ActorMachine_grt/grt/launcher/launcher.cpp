#include "Memory.h"
#include "../graphs/SRDAG/SRDAGGraph.h"
#include "launcher.h"
#include "messages/CreateFifoMsg.h"
#include "messages/ClearFifoMsg.h"
#include "messages/CreateTaskMsg.h"
#include "messages/StopTaskMsg.h"
#include "messages/StartMsg.h"
#include "../ExecutionStat.h"
#include <hwQueues.h>
#include <algorithm>

launcher::launcher(int nbSlaves): sharedMem(Memory(0x10000000, 0x1000000)){
	launchedSlaveNb=0;
	OS_QInit(nbSlaves);
};

void launcher::initFifos(SRDAGGraph* graph, int nbSlaves){
	CreateFifoMsg msg_create;
	ClearFifoMsg msg_clear;

	/* Creating fifos */
	for(int i=0; i<graph->getNbEdges(); i++){
		msg_create = CreateFifoMsg(graph, graph->getEdge(i), &sharedMem);
		for(int j=0; j<nbSlaves; j++)
			msg_create.send(j);
	}
	printf("Memory: %d Bytes allocated\n", sharedMem.getTotalAllocated());

	/* Clearing fifos */
	for(int i=0; i<graph->getNbEdges(); i++){
		msg_clear = ClearFifoMsg(i);
		msg_clear.send(i%nbSlaves);
	}
}

void launcher::initTasks(SRDAGGraph* graph){
	CreateTaskMsg msg;
	for(int i=0; i<graph->getNbVertices(); i++){
		msg = CreateTaskMsg(graph, graph->getVertex(i));
		char name[20];
		sprintf(name, "%s_%d.gv", graph->getVertex(i)->getCsDagReference()->getName(), i);
		msg.toDot(name);
		msg.send(graph->getVertex(i)->getSlaveIndex());
	}

}

void launcher::initTasks(SRDAGGraph* graph, Schedule* schedule, int nbSlaves){
	CreateTaskMsg msg;
	for(int i=0; i<nbSlaves; i++){
		msg = CreateTaskMsg(graph, schedule, i);
		char name[20];
		sprintf(name, "Slave%d.gv", i);
		msg.toDot(name);
		msg.send(i);
	}
}

void launcher::stopTasks(SRDAGGraph* graph, Schedule* schedule, int nbSlaves){
	StopTaskMsg msg;
	for(int i=0; i<nbSlaves; i++){
		msg = StopTaskMsg(0,0);
		msg.send(i);
	}
}

void launcher::start(int nbSlaves){
	for(int j=0; j<nbSlaves; j++)
		StartMsg().send(j);
}

void launcher::launch(int nbSlaves){
	UINT32 data[MAX_CTRL_DATA];
	launchedSlaveNb = nbSlaves;
	for(int i=0; i<nbSlaves; i++){
		OS_CtrlQPush(i, dataToSend[i], dataToSendCnt[i]*sizeof(UINT32));
		OS_CtrlQPop(i, &data, dataToReceiveCnt[i]*sizeof(UINT32));
		for(int j=0; (UINT32)j< dataToReceiveCnt[i]; j++){
			if(dataToReceive[i][j] != data[j]){
				printf("Unattended ack message from Slave %d (%d instead of %d)\n", i, data[j], dataToReceive[i][j]);
				abort();
			}
		}
	}
}

void launcher::prepare(SRDAGGraph* graph, Architecture *archi, Schedule* schedule, ExecutionStat* execStat){
	CreateFifoMsg msg_createFifo;
	ClearFifoMsg msg_clearFifo;
	CreateTaskMsg msg_createTask;

	flushDataToSend();
	flushDataToReceive();

	/* Creating fifos */
	for(int i=0; i<graph->getNbEdges(); i++){
		msg_createFifo = CreateFifoMsg(graph, graph->getEdge(i), &sharedMem);
		for(int j=0; j<archi->getNbActiveSlaves(); j++)
			msg_createFifo.prepare(j,this);
//			dataToSendCnt[j] += msg_createFifo.prepare(dataToSend[j], dataToSendCnt[j]);
	}
	execStat->memAllocated = sharedMem.getTotalAllocated();
	execStat->fifoNb = graph->getNbEdges();

	/* Clearing fifos */
	for(int i=0; i<graph->getNbEdges(); i++){
		msg_clearFifo = ClearFifoMsg(i);
//		int j = i%archi->getNbActiveSlaves();
//		dataToSendCnt[j] += msg_clearFifo.prepare(dataToSend[j], dataToSendCnt[j]);
		msg_clearFifo.prepare(0, this);
	}

	/* Creating Tasks */
	for(int i=0; i<archi->getNbActiveSlaves(); i++){
		msg_createTask = CreateTaskMsg(graph, schedule, i);
		char name[20];
		sprintf(name, "Slave%d.gv", i);
		msg_createTask.toDot(name);
		execStat->nbAMVertices[i]	= msg_createTask.getAm()->getNbVertices();
		execStat->nbAMConds[i]		= msg_createTask.getAm()->getNbConds();
		execStat->nbAMActions[i]	= msg_createTask.getAm()->getNbActions();
//		dataToSendCnt[i] += msg_createTask.prepare(dataToSend[i], dataToSendCnt[i]);
		msg_createTask.prepare(i, this);
	}

	/* Launch schedule */
	for(int j=0; j<archi->getNbActiveSlaves(); j++)
//		dataToSendCnt[j] += StartMsg().prepare(dataToSend[j], dataToSendCnt[j]);
		 StartMsg().prepare(j, this);

	for(int j=0; j<archi->getNbActiveSlaves(); j++){
		execStat->msgLength[j] = dataToSendCnt[j]*4;
		if(dataToSendCnt[j]>MAX_CTRL_DATA){
			printf("dataToSendCnt>MAX_CTRL_DATA\n");
			abort();
		}
	}

	execStat->listMakespan = 0;
	for(int j=0; j<archi->getNbActiveSlaves(); j++){
		execStat->listMakespan = std::max(execStat->listMakespan, schedule->getReadyTime(j));
	}

	sharedMem.exportMem("mem.csv");
}

void launcher::launchOnce(SRDAGGraph* graph, Architecture *archi, Schedule* schedule){
	initFifos(graph, archi->getNbActiveSlaves());
	initTasks(graph, schedule, archi->getNbActiveSlaves());
	start(archi->getNbActiveSlaves());
}

void launcher::stop(){
//	initFifos(graph, archi->getNbSlaves());
//	stopTasks(graph, schedule, archi->getNbSlaves());

	StopTaskMsg msg;
	for(int i=0; i<launchedSlaveNb; i++){
		msg = StopTaskMsg(0,0);
		msg.send(i);
	}
	launchedSlaveNb=0;
}

void launcher::stopWOCheck(){
//	initFifos(graph, archi->getNbSlaves());
//	stopTasks(graph, schedule, archi->getNbSlaves());

	StopTaskMsg msg;
	for(int i=0; i<launchedSlaveNb; i++){
		msg = StopTaskMsg(0,0);
		msg.sendWOCheck(i);
	}
	launchedSlaveNb=0;
}

void launcher::reset(){
	sharedMem.reset();
}

void launcher::addDataToSend(int slave, void* data, int size){
	memcpy(dataToSend[slave]+dataToSendCnt[slave], data, size);
	dataToSendCnt[slave] += size;
}

void launcher::addDataToReceive(int slave, void* data, int size){
	memcpy(dataToReceive[slave]+dataToReceiveCnt[slave], data, size);
	dataToReceiveCnt[slave] += size;
}

void launcher::addUINT32ToSend(int slave, UINT32 val){
	dataToSend[slave][dataToSendCnt[slave]] = val;
	dataToSendCnt[slave]++;
}
void launcher::addUINT32ToReceive(int slave, UINT32 val){
	dataToReceive[slave][dataToReceiveCnt[slave]] = val;
	dataToReceiveCnt[slave]++;
}

void launcher::flushDataToSend(){
	for(int i=0; i<MAX_SLAVES; i++){
		dataToSendCnt[i]=0;
	}
}

void launcher::flushDataToReceive(){
	for(int i=0; i<MAX_SLAVES; i++){
		dataToReceiveCnt[i]=0;
	}
}
