
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

#include <time.h>
#include "Memory.h"
#include "../graphs/SRDAG/SRDAGGraph.h"
#include "launcher.h"
#include "messages/CreateFifoMsg.h"
#include "messages/ClearFifoMsg.h"
#include "messages/CreateTaskMsg.h"
#include "messages/StopTaskMsg.h"
#include "messages/StartMsg.h"
#include <tools/ExecutionStat.h>
#include <hwQueues.h>
#include <algorithm>

#define PRINT_ACTOR_IN_DOT_FILE		0


static char* regenerateColor(int refInd){
	static char color[8];
	color[0] = '\0';

	int ired = (refInd & 0x3)*50 + 100;
	int igreen = ((refInd >> 2) & 0x3)*50 + 100;
	int iblue = ((refInd >> 4) & 0x3)*50 + 100;
	char red[5];
	char green[5];
	char blue[5];
	if(ired <= 0xf){
		sprintf(red,"0%x",ired);
	}
	else{
		sprintf(red,"%x",ired);
	}

	if(igreen <= 0xf){
		sprintf(green,"0%x",igreen);
	}
	else{
		sprintf(green,"%x",igreen);
	}

	if(iblue <= 0xf){
		sprintf(blue,"0%x",iblue);
	}
	else{
		sprintf(blue,"%x",iblue);
	}

	strcpy(color,"#");
	strcat(color,red);
	strcat(color,green);
	strcat(color,blue);

	return color;
}


launcher::launcher(): sharedMem(Memory(0x0, 0xffffffff)){

	nbFIFOs = 0;
	launchedSlaveNb=0;
};

void launcher::init(int nbSlaves){
	RTQueuesInit(nbSlaves);
	flushDataToSend();
	flushDataToReceive();
}

void launcher::clear(){
	flushDataToSend();
	flushDataToReceive();
}

void launcher::createRealTimeGantt(Architecture *arch, SRDAGGraph *dag, const char *filePathName){
	// Creating the Gantt with real times.
	FILE * pFile;
	pFile = fopen(filePathName, "w+");
	if(pFile != NULL){
		// Writing header
		fprintf (pFile, "<data>\n");

		// Writing execution data for each slave.
		for(UINT32 i=0; i<arch->getNbSlaves(); i++){
			UINT32 data[MAX_CTRL_DATA];
			UINT32 nbTasks = popExecInfo(i, data);

			for (UINT32 j=0 ; j<nbTasks; j++){
				SRDAGVertex* vertex = dag->getVertex(data[j + j*2]); // data[0] contains the vertex's id.
				fprintf (pFile, "\t<event\n");
				UINT32 hour = data[(1+j) + j*2];
				UINT32 min = hour * 60 + data[(2+j) + j*2];
				UINT32 sec = min * 60 + data[(3+j) + j*2];
				UINT32 mili = sec * 1000 + data[(4+j) + j*2];
				UINT32 execTime = (data[(5+j) + j*2]/CLOCKS_PER_SEC) * 1000;
//				UINT32 startTime = data[(1+j) + j*2];
//				UINT32 execTime = data[(2+j) + j*2];
				fprintf (pFile, "\t\tstart=\"%d\"\n", mili);
				fprintf (pFile, "\t\tend=\"%d\"\n",	mili + execTime);
//				fprintf (pFile, "\t\tend=\"%d\"\n",	mktime(&execTime) + mktime(&startTime));
				fprintf (pFile, "\t\ttitle=\"%s\"\n", vertex->getReference()->getName());
				fprintf (pFile, "\t\tmapping=\"%s\"\n", arch->getSlaveName(i));
				fprintf (pFile, "\t\tcolor=\"%s\"\n", regenerateColor(vertex->getId()));
				fprintf (pFile, "\t\t>%s.</event>\n", vertex->getReference()->getName());

				printf("task %d started at %d:%d:%d:%d ended at +%d\n",
						j, hour, min, sec, mili, (execTime/CLOCKS_PER_SEC) * 1000);
			}
		}
		fprintf (pFile, "</data>\n");
		fclose (pFile);
	}
}


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
//	if(nbAM == MAX_NB_AM) exitWithCode(1058);
//	AMGraph* am = &AMGraphTbl[nbAM++];
//	for(int i=0; i<graph->getNbVertices(); i++){
//		CreateTaskMsg msg = CreateTaskMsg(graph, graph->getVertex(i), am);
//		char name[20];
//		sprintf(name, "%s_%d.gv", graph->getVertex(i)->getCsDagReference()->getName(), i);
//		msg.toDot(name);
//		msg.send(graph->getVertex(i)->getSlaveIndex());
//	}

}

void launcher::initTasks(SRDAGGraph* graph, Schedule* schedule, int nbSlaves){
//	if(nbAM == MAX_NB_AM) exitWithCode(1058);
//	AMGraph* am = &AMGraphTbl[nbAM++];
//	for(int i=0; i<nbSlaves; i++){
//		CreateTaskMsg msg = CreateTaskMsg(graph, schedule, i, am);
//		char name[20];
//		sprintf(name, "Slave%d.gv", i);
//		msg.toDot(name);
//		msg.send(i);
//	}
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


void launcher::launchWaitAck(int nbSlaves){
	UINT32 data[MAX_CTRL_DATA];
	launchedSlaveNb = nbSlaves;

	// Sending FIFO flushing and task creation messages.
	for(int i=0; i<nbSlaves; i++){
		RTQueuePush(i, RTCtrlQueue, dataToSend[i], dataToSendCnt[i]*sizeof(UINT32));
	}

	// Waiting for acknowledge from LRT 0.
	RTQueuePop(0, RTCtrlQueue, &data, dataToReceiveCnt[0]*sizeof(UINT32));
	for(int j=0; (UINT32)j< dataToReceiveCnt[0]; j++)
		if(dataToReceive[0][j] != data[j]){
			printf("Unattended ack message from Slave %d (%d instead of %d)\n", 0, data[j], dataToReceive[0][j]);
			abort();
		}

	// Starting executions.
	for(int i=0; i<nbSlaves; i++){
		StartMsg().send(i);
	}

}

void launcher::launch(int nbSlaves, bool clearAfterCompletion){
	StartMsg msg;
	launchedSlaveNb = nbSlaves;

	// Sending FIFO flushing and task creation messages.
	for(int i=0; i<nbSlaves; i++){
		if(dataToSendCnt[i] > 0){
			RTQueuePush(i, RTCtrlQueue, dataToSend[i], dataToSendCnt[i]*sizeof(UINT32));
			// Starting executions.
			msg.setClearAfterCompletion(clearAfterCompletion);
			msg.send(i);
		}
	}
}

//void launcher::launchJobs(UINT16 nbSlaves){
//	UINT32 data[MAX_JOB_DATA];
////	launchedSlaveNb = nbSlaves;
//	for(UINT16 i=0; i<nbSlaves; i++){
//		RTQueuePush(i, RTJobQueue, jobDataToSend[i], jobDataToSendCnt[i]*sizeof(UINT32));
////		RTQueuePop(i, RTCtrlQueue, &data, dataToReceiveCnt[i]*sizeof(UINT32));
////		for(int j=0; (UINT32)j< dataToReceiveCnt[i]; j++){
////			if(dataToReceive[i][j] != data[j]){
////				printf("Unattended ack message from Slave %d (%d instead of %d)\n", i, data[j], dataToReceive[i][j]);
////				abort();
////			}
////		}
//	}
//}

void launcher::prepare(SRDAGGraph* graph, Architecture *archi, Schedule* schedule, ExecutionStat* execStat){
	CreateFifoMsg msg_createFifo;
	ClearFifoMsg msg_clearFifo;
//	CreateTaskMsg msg_createTask;

	flushDataToSend();
	flushDataToReceive();

	/* Creating fifos */
	for(int i=0; i<graph->getNbEdges(); i++){
//		msg_createFifo = CreateFifoMsg(graph, graph->getEdge(i), &sharedMem);
		msg_createFifo = CreateFifoMsg(graph, graph->getEdge(i), DEFAULT_FIFO_SIZE, &sharedMem);
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

	// An acknowledge message (i.e. MSG_CLEAR_FIFO) from LRT 0 must be received.
	addUINT32ToReceive(0, MSG_CLEAR_FIFO);

	/* Creating Tasks */
//	for(int i=0; i<archi->getNbActiveSlaves(); i++){
//		if(nbAM == MAX_NB_AM) exitWithCode(1058);
//		AMGraph* am = &AMGraphTbl[nbAM++];
//		CreateTaskMsg msg_createTask = CreateTaskMsg(graph, schedule, i, am);
//		char name[20];
//		sprintf(name, "Slave%d.gv", i);
//		msg_createTask.toDot(name);
//		execStat->nbAMVertices[i]	= am->getNbVertices();
//		execStat->nbAMConds[i]		= am->getNbConds();
//		execStat->nbAMActions[i]	= am->getNbActions();
////		dataToSendCnt[i] += msg_createTask.prepare(dataToSend[i], dataToSendCnt[i]);
////		msg_createTask.prepare(i, this);
//	}

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

void launcher::prepareFIFOsInfo(SRDAGGraph* graph, Architecture* arch){
	/* Creating fifos for executable vxs.*/
	for (UINT32 i = 0; i < graph->getNbVertices(); i++) {
		SRDAGVertex* vx = graph->getVertex(i);
		if(vx->getState() == SrVxStExecutable){
			for (UINT32 j = 0; j < vx->getNbInputEdge(); j++){
				SRDAGEdge* edge = vx->getInputEdge(j);
				if(edge->getFifoId() == -1){
					addFIFO(edge);
				}
			}
			for (UINT32 j = 0; j < vx->getNbOutputEdge(); j++){
				SRDAGEdge* edge = vx->getOutputEdge(j);
				if(edge->getFifoId() == -1){
					addFIFO(edge);
				}
			}
		}
	}


//	for(int i=0; i<graph->getNbEdges(); i++){
////		msg_createFifo = CreateFifoMsg(graph, graph->getEdge(i), &sharedMem);
//		msg_createFifo = CreateFifoMsg(graph, graph->getEdge(i), DEFAULT_FIFO_SIZE, &sharedMem);
//		for(int j=0; j<archi->getNbActiveSlaves(); j++)
//			msg_createFifo.prepare(j,this);
////			dataToSendCnt[j] += msg_createFifo.prepare(dataToSend[j], dataToSendCnt[j]);
//	}
//	execStat->memAllocated = sharedMem.getTotalAllocated();
//	execStat->fifoNb = graph->getNbEdges();

//	/* Clearing fifos */
//	ClearFifoMsg(-1).prepare(0, this);
//	for(int i=0; i<graph->getNbEdges(); i++){
//		msg_clearFifo = ClearFifoMsg(i);
////		int j = i%archi->getNbActiveSlaves();
////		dataToSendCnt[j] += msg_clearFifo.prepare(dataToSend[j], dataToSendCnt[j]);
//		msg_clearFifo.prepare(0, this);
//	}

	// Setting the type of acknowledge message that should be received from LRT 0.
//	addUINT32ToReceive(0, MSG_CLEAR_FIFO);

}

void launcher::prepareTasksInfo(SRDAGGraph* graph, UINT32 nbSlaves, BaseSchedule* schedule, bool isAM, ExecutionStat* execStat){
//	CreateFifoMsg msg_createFifo;
//	ClearFifoMsg msg_clearFifo;
	CreateTaskMsg msg_createTask;
	static UINT16 stepsCntr = 0;

	/* Creating Tasks */
	for(int i=0; i < nbSlaves; i++){
		if(isAM){
			// Creating an actor machine.
			msg_createTask = CreateTaskMsg(graph, schedule, i, this);
			msg_createTask.setIsAM(1);

			// Writing AM into .dot file.
			char name[20];
			sprintf(name, "Slave%d_%d.gv", i, stepsCntr);
			msg_createTask.toDot(name);

			// Copying task and AM data into the chunk of data that will be sent.
			msg_createTask.prepare(i, this);
			msg_createTask.getAM()->prepare(i, this);
		}
		else
		{
#if PRINT_ACTOR_IN_DOT_FILE == 1
			char name[20];
			sprintf(name, "Slave%d_%d.gv", i, stepsCntr);
			FILE * pFile = fopen (name,"w");
			if(pFile != NULL){
				// Writing header
				fprintf (pFile, "digraph Actors {\n");
				fprintf (pFile, "node [color=Black];\n");
				fprintf (pFile, "edge [color=Black];\n");
//				fprintf (pFile, "rankdir=LR;\n");
			}
#endif
			// Creating single actors.
			for (UINT32 j = 0; j < schedule->getNbVertices(i); j++) {
//				msg_createTask = CreateTaskMsg(graph, (SRDAGVertex*)(schedule->getSchedule(i, j)->vertex), this);
//				msg_createTask.setIsAM(0);
//
//				// Copying task and actor data into the chunk of data that will be sent
//				msg_createTask.prepare(i, this);
//				msg_createTask.getLRTActor()->prepare(i, this);

				SRDAGVertex* vertex = (SRDAGVertex*)(schedule->getSchedule(i, j)->vertex);
				if (vertex->getState() == SrVxStExecutable){
//					LRTActor actor = LRTActor(graph, vertex, this);
//					actor.prepare(i, this);

					addUINT32ToSend(i, MSG_CREATE_TASK);
					addUINT32ToSend(i, vertex->getFunctIx());
					addUINT32ToSend(i, vertex->getId());
					addUINT32ToSend(i, 0); // Not an actor machine.
					addUINT32ToSend(i, vertex->getNbInputEdge());
					addUINT32ToSend(i, vertex->getNbOutputEdge());

					UINT32 nbParams = 0;
					UINT32 params[MAX_NB_ARGS];
					if(vertex->getType() == 0){
						VERTEX_TYPE refType = vertex->getReference()->getType();
						if((refType == roundBuff_vertex)||
							(refType == input_vertex)||
							(refType == output_vertex)){
							nbParams = 2;
							params[0] = vertex->getInputEdge(0)->getTokenRate();
							params[1] = vertex->getOutputEdge(0)->getTokenRate();
						}
						else if (refType == broad_vertex){
							nbParams = 2;
							params[0] = vertex->getNbOutputEdge();
							params[1] = vertex->getInputEdge(0)->getTokenRate();
						}
						else{
							nbParams = vertex->getReference()->getNbParameters();

							for(UINT32 i=0; i<nbParams; i++){
								params[i] = vertex->getReference()->getParameter(i)->getValue();
							}
						}
					}
					else{// Implode/Explode vertices.
						nbParams = vertex->getNbInputEdge() + vertex->getNbOutputEdge() + 2;
						params[0] = vertex->getNbInputEdge();
						params[1] = vertex->getNbOutputEdge();

						// Setting number of tokens going through each input/output.
						for(UINT32 i=0; i<vertex->getNbInputEdge(); i++){
							params[i + 2] = vertex->getInputEdge(i)->getTokenRate();
						}
						for(UINT32 i=0; i<vertex->getNbOutputEdge(); i++){
							params[i + 2 + vertex->getNbInputEdge()] = vertex->getOutputEdge(i)->getTokenRate();
						}
					}

					addUINT32ToSend(i, nbParams);

					for (UINT32 k = 0; k < vertex->getNbInputEdge(); k++) {
						addUINT32ToSend(i, getFIFO(vertex->getInputEdge(k)->getFifoId())->id);
						addUINT32ToSend(i, getFIFO(vertex->getInputEdge(k)->getFifoId())->addr);
						// TODO: see if the FIFO' size is required.
					}
					for (UINT32 k = 0; k < vertex->getNbOutputEdge(); k++) {
						addUINT32ToSend(i, getFIFO(vertex->getOutputEdge(k)->getFifoId())->id);
						addUINT32ToSend(i, getFIFO(vertex->getOutputEdge(k)->getFifoId())->addr);
						// TODO: see if the FIFO' size is required.
					}
					for(UINT32 k = 0; k < nbParams; k++)
						addUINT32ToSend(i, params[k]);

					if(vertex->getReference()->getType() == config_vertex){
						UINT32 dataCnt = 0;
						dataCnt++;			// MSG_PARAM_VALUE must be received as first word.
						dataCnt++;			// The id of the vertex must be received as second word.
						// Storing the number of parameter values that must be received.
						dataCnt += ((PiSDFConfigVertex*)(vertex->getReference()))->getNbRelatedParams();

						dataToReceiveCnt[i] = dataCnt*sizeof(UINT32); // Number of bytes to be received.
					}

#if PRINT_ACTOR_IN_DOT_FILE == 1
					sprintf(name, "%s_%d", vertex->getReference()->getName(), vertex->getReferenceIndex());
					actor.toDot(pFile, vertex->getName(), j);
#endif
				}
			}
#if PRINT_ACTOR_IN_DOT_FILE == 1
			fprintf (pFile, "}\n");
			fclose (pFile);
#endif
		}

//		execStat->nbAMVertices[i]	= am->getNbVertices();
//		execStat->nbAMConds[i]		= am->getNbConds();
//		execStat->nbAMActions[i]	= am->getNbActions();
//		dataToSendCnt[i] += msg_createTask.prepare(dataToSend[i], dataToSendCnt[i]);

//		if(true){
//			this->addUINT32ToSend(i, true);	// An AM is present.
//		}
	}
	stepsCntr++;
	/* Launch schedule */
//	for(int j=0; j<archi->getNbActiveSlaves(); j++)
////		dataToSendCnt[j] += StartMsg().prepare(dataToSend[j], dataToSendCnt[j]);
//		 StartMsg().prepare(j, this);
//
//	for(int j=0; j<archi->getNbActiveSlaves(); j++){
//		execStat->msgLength[j] = dataToSendCnt[j]*4;
//		if(dataToSendCnt[j]>MAX_CTRL_DATA){
//			printf("dataToSendCnt>MAX_CTRL_DATA\n");
//			abort();
//		}
//	}
//
//	execStat->listMakespan = 0;
//	for(int j=0; j<archi->getNbActiveSlaves(); j++){
//		execStat->listMakespan = std::max(execStat->listMakespan, schedule->getReadyTime(j));
//	}
//
//	sharedMem.exportMem("mem.csv");
}


void launcher::prepareConfigExec(
		BaseVertex** configVertices,
		UINT32 nbConfigVertices,
		Architecture *archi,
		BaseSchedule* schedule,
		ExecutionStat* execStat)
{
	CreateFifoMsg msg_createFifo;
	ClearFifoMsg msg_clearFifo;
//	CreateTaskMsg msg_createTask;

	flushDataToSend();
	flushDataToReceive();

	for(UINT32 i=0; i < nbConfigVertices; i++){
		PiSDFConfigVertex* vertex = (PiSDFConfigVertex*)configVertices[i];

//		/* Creating input fifos */
//		for (UINT32 j = 0; j < vertex->getNbInputEdges(); j++) {
//			PiSDFEdge* edge = vertex->getInputEdge(j);
//			msg_createFifo = CreateFifoMsg(vertex-edge, DEFAULT_FIFO_SIZE, &sharedMem);
//
//			for(int k = 0; k < archi->getNbActiveSlaves(); k++){
//				msg_createFifo.prepare(k, this);
//			}
//		}

//		/*
//		 * Creating the output fifo to receive the parameter's value.
//		 */
//		PiSDFParameter* param = vertex->getRelatedParameter();
//		msg_createFifo = CreateFifoMsg(param->getId(), DEFAULT_FIFO_SIZE, &sharedMem);
//
//		// A create fifo msg will be sent to each active local RT.
//		for(int k = 0; k < archi->getNbActiveSlaves(); k++){
//			msg_createFifo.prepare(k, this);
//		}


//
//
//
//
//
//
//		/* Clearing fifos */
//		for (UINT32 j = 0; j < vertex->getNbOutputEdges(); j++) {
//			PiSDFEdge* edge = vertex->getOutputEdge(j);
//			msg_clearFifo = ClearFifoMsg(edge->getId());
//
//			for(int k = 0; k < archi->getNbActiveSlaves(); k++){
//				msg_clearFifo.prepare(0, this);
//			}
//		}
//
//		execStat->fifoNb += vertex->getNbOutputEdges();
//	}
//	execStat->memAllocated = sharedMem.getTotalAllocated();

	/* Creating single tasks (not Actor Machines) */
		// TODO: Creating tasks on selected slaves.
//		for(int i=0; i<archi->getNbActiveSlaves(); i++){
//			if(nbAM == MAX_NB_AM) exitWithCode(1058);
//			AMGraph* am = &AMGraphTbl[nbAM++];
//			CreateTaskMsg msg_createTask = CreateTaskMsg(vertex, am, 1);
//		//		char name[20];
//		//		sprintf(name, "Slave%d.gv", j);
//		//		msg_createTask.toDot(name);
//		//		execStat->nbAMVertices[i]	= msg_createTask.getAm()->getNbVertices();
//		//		execStat->nbAMConds[i]		= msg_createTask.getAm()->getNbConds();
//		//		execStat->nbAMActions[i]	= msg_createTask.getAm()->getNbActions();
//		//		dataToSendCnt[i] += msg_createTask.prepare(dataToSend[i], dataToSendCnt[i]);
//			msg_createTask.prepare(i, this);
//
//			// Creating start message.
//			StartMsg().prepare(i, this);
//
////			// Creating stop message for the actor executes only once.
////			StopTaskMsg().prepare(i, this);
//		}
	}
}

UINT32 launcher::rcvData(UINT32 slave, UINT32 msgType, UINT32* data){
	UINT32 bytesRcvd = 0;
	UINT32 bytesToRcv = dataToReceiveCnt[slave];
	if(bytesToRcv > 0){
		bytesRcvd = RTQueuePop(slave, RTCtrlQueue, data, bytesToRcv);
		if(bytesRcvd != bytesToRcv) exitWithCode(1067);
		if(data[0] != msgType) exitWithCode(1068);
		dataToReceiveCnt[slave] -= bytesRcvd;
	}
	return bytesRcvd;
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

void launcher::toDot(const char* path, UINT32 slaveId){
	char name[20];
	sprintf(name, "%s_%d.gv", path, slaveId);
	FILE * pFile = fopen (name,"w");
	if(pFile != NULL){
		// Writing header
		fprintf (pFile, "digraph Actors {\n");
		fprintf (pFile, "node [color=Black];\n");
		fprintf (pFile, "edge [color=Black];\n");
//		fprintf (pFile, "rankdir=LR;\n");


	}
	fprintf (pFile, "}\n");
	fclose (pFile);

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

void launcher::addUINT32ToSend(int slave, UINT32 val, RTQueueType queue){
	switch (queue) {
		case RTCtrlQueue:
			dataToSend[slave][dataToSendCnt[slave]] = val;
			dataToSendCnt[slave]++;
			break;
		case RTJobQueue:
			jobDataToSend[slave][jobDataToSendCnt[slave]] = val;
			jobDataToSendCnt[slave]++;
			break;
		default:
			break;
	}
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


void launcher::resolveParameters(SRDAGGraph* dag, UINT32 nbSlaves){
	UINT32 data[MAX_CTRL_DATA];
	for (UINT32 i = 0; i < nbSlaves; i++) {
		UINT32 nbBytesRcvd = rcvData(i, MSG_PARAM_VALUE, data);
		if(nbBytesRcvd > 0){
			UINT32 vxId = data[1];
			PiSDFConfigVertex* refConfigVx = (PiSDFConfigVertex*)(dag->getVertex(vxId)->getReference());
			for(UINT32 j = 0; j < refConfigVx->getNbRelatedParams(); j++){
				refConfigVx->getRelatedParam(j)->setValue(data[j + 2]);
			}
		}
	}
}


UINT32 launcher::popExecInfo(UINT32 slaveId, UINT32* data){
	UINT32 dataHdr[MAX_CTRL_DATA], nbTasks;

	UINT32 nbHdrBytes = RTQueuePop(slaveId, RTCtrlQueue, dataHdr, 2*sizeof(UINT32));
	if(dataHdr[0] != MSG_EXEC_TIMES) exitWithCode(1068);

//	UINT32 clocksPerSec = dataHdr[1];
	UINT32 nbBytes = dataHdr[1];

	RTQueuePop(slaveId, RTCtrlQueue, data, nbBytes - nbHdrBytes);


	/*
	 * For each task there is 6 sizeof(UINT32) words.
	 * For details see the 'sendExecData' function at 'lrt_taskMngr.c'.
	 */
	nbTasks = (nbBytes/sizeof(UINT32))/6;

	return nbTasks;
}


void launcher::sendClearTasks(int nbSlaves){
	for(int i=0; i<nbSlaves; i++){
		RTQueuePush_UINT32(i, RTCtrlQueue, MSG_CLEAR_TASKS);
	}
}
