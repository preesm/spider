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

#include <time.h>
#include "Memory.h"
#include "../graphs/SRDAG/SRDAGGraph.h"
#include "launcher.h"
#include <launcher/messages/CreateTaskMsg.h>
#include <launcher/messages/SendInfoData.h>
#include <platform_queue.h>
#include <platform.h>
#include <algorithm>
#include <platform_file.h>
#include <platform_time.h>
#include <debuggingOptions.h>


#define PRINT_ACTOR_IN_DOT_FILE		0

static Memory memory = Memory(0x0, 0x003F8000);
#define END_MEM_SIZE 10000

static UINT32 nbFifo;
static UINT32 nbParamToRecv;

static UINT32 timeStartGraph[MAX_PISDF_STEPS];
static UINT32 timeEndGraph[MAX_PISDF_STEPS];
static UINT32 timeStartScheduling[MAX_PISDF_STEPS];
static UINT32 timeEndScheduling[MAX_PISDF_STEPS];
static UINT32 nbStepsSched=0;
static UINT32 nbStepsGraph=0;

static UINT32 endMem;

void Launcher::initGraphTime(){
	timeStartGraph[nbStepsGraph] = platform_time_getValue();
}
void Launcher::endGraphTime(){
	timeEndGraph[nbStepsGraph] = platform_time_getValue();
	nbStepsGraph++;
}

void Launcher::initSchedulingTime(){
	timeStartScheduling[nbStepsSched] = platform_time_getValue();
}
void Launcher::endSchedulingTime(){
	timeEndScheduling[nbStepsSched] = platform_time_getValue();
	nbStepsSched++;
}


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

void Launcher::assignFifo(SRDAGGraph* graph){
	/* Creating fifos for executable vxs.*/
	for (UINT32 i = 0; i < graph->getNbVertices(); i++) {
		SRDAGVertex* vx = graph->getVertex(i);
		if(vx->getState() == SrVxStExecutable){
			for (UINT32 j = 0; j < vx->getNbOutputEdge(); j++){
				SRDAGEdge* edge = vx->getOutputEdge(j);
				if(edge->getFifoId() == -1){
					edge->setFifoId(nbFifo++);
					edge->setFifoAddress(memory.alloc(edge->getTokenRate()));
				}
			}
		}
	}
}

void Launcher::launch(SRDAGGraph* graph, Architecture* arch, BaseSchedule* schedule){
	/* Creating Tasks */
	for(UINT32 slave=0; slave < arch->getNbActiveSlaves(); slave++){
		for(UINT32 i=0; i < schedule->getNbVertices(slave); i++){
			SRDAGVertex* vertex = (SRDAGVertex*)schedule->getVertex(slave,i);
			if(vertex->getState() == SrVxStExecutable){
				CreateTaskMsg::send(slave, vertex);
				if(vertex->getType() == ConfigureActor)
					nbParamToRecv += ((PiSDFConfigVertex*)(vertex->getReference()))->getNbRelatedParams();
			}
		}
	}
}

void Launcher::assignFifoVertex(SRDAGVertex* vertex){
	UINT32 i, base, offset;
	SRDAGEdge* edge;
	switch(vertex->getType()){
	 case Explode:
		base = vertex->getInputEdge(0)->getFifoAddress();
		offset = 0;
		for (i = 0; i < vertex->getNbOutputEdge(); i++){
			edge = vertex->getOutputEdge(i);
			edge->setFifoId(nbFifo++);
			edge->setFifoAddress(base+offset);
			offset += edge->getTokenRate();
		}
		break;
//	 case Implode:
//		 if(vertex->getOutputEdge(0)->getFifoId() == -1){
//			 base = memory.alloc(vertex->getOutputEdge(0)->getTokenRate());
//			 offset = 0;
//			 vertex->getOutputEdge(0)->setFifoId(nbFifo++);
//			 vertex->getOutputEdge(0)->setFifoAddress(base);
//			for (i = 0; i < vertex->getNbInputEdge(); i++){
//				edge = vertex->getInputEdge(i);
//				if(edge->getFifoId() == -1){
//					edge->setFifoId(nbFifo++);
//					edge->setFifoAddress(base+offset);
//				}
//				offset += edge->getTokenRate();
//			}
//		 }
//		 break;
	 case Broadcast:
		 for (i = 0; i < vertex->getNbOutputEdge(); i++){
			edge = vertex->getOutputEdge(i);
			if(edge->getFifoId() == -1){
				edge->setFifoId(nbFifo++);
				edge->setFifoAddress(vertex->getInputEdge(0)->getFifoAddress());
			}
		}
		 break;
	 case Implode:
	 default:
		 if(vertex->getFunctIx() == SWICTH_FUNCT_IX){
			 vertex->getOutputEdge(0)->setFifoId(nbFifo++);
			 vertex->getOutputEdge(0)->setFifoAddress(memory.alloc(vertex->getInputEdge(2)->getTokenRate()));
		 }

		for (i = 0; i < vertex->getNbOutputEdge(); i++){
			edge = vertex->getOutputEdge(i);
//			if(edge->getSink()->getType() == Implode){
//				assignFifoVertex(edge->getSink());
//			}

			if(edge->getFifoId() == -1){
				edge->setFifoId(nbFifo++);
				 if(edge->getSink()->getFunctIx() == END_FUNCT_IX){
					edge->setFifoAddress(endMem);
				 }else{
					 edge->setFifoAddress(memory.alloc(edge->getTokenRate()));
				}
			}
		}
		break;
	}
}

void Launcher::launchVertex(SRDAGVertex* vertex, UINT32 slave){
	Launcher::assignFifoVertex(vertex);
	CreateTaskMsg::send(slave, vertex);
	if(vertex->getType() == ConfigureActor)
		nbParamToRecv += ((PiSDFConfigVertex*)(vertex->getReference()))->getNbRelatedParams();
}

void Launcher::init(){
	nbStepsSched = nbStepsGraph = 0;
	nbFifo = nbParamToRecv = 0;
	memory = Memory(0x0, 0x003EC000);

	endMem = memory.alloc(END_MEM_SIZE);
}

void Launcher::reset(){
	nbFifo = nbParamToRecv = 0;
	memory = Memory(0x0, 0x003EC000);
	endMem = memory.alloc(END_MEM_SIZE);
}

//#define PRINT_GRAPH 1
void Launcher::createRealTimeGantt(Architecture *arch, SRDAGGraph *dag, const char *filePathName, ExecutionStat* stat){
	// Creating the Gantt with real times.
	memset(stat, 0, sizeof(ExecutionStat));
	UINT32 nbIter = 0;

#if PRINT_REAL_GANTT
	platform_fopen(filePathName);

	// Writing header
	platform_fprintf("<data>\n");

	char name[MAX_VERTEX_NAME_SIZE];
#endif

	// Writing execution data for the master.
	for (UINT32 j=0 ; j<nbStepsSched; j++){
#if PRINT_REAL_GANTT
		platform_fprintf("\t<event\n");
		platform_fprintf("\t\tstart=\"%u\"\n", 	timeStartScheduling[j]);
		platform_fprintf("\t\tend=\"%u\"\n",	timeEndScheduling[j]);
		platform_fprintf("\t\ttitle=\"Sched_%d\"\n", j);
		platform_fprintf("\t\tmapping=\"Master\"\n");
		platform_fprintf("\t\tcolor=\"%s\"\n", regenerateColor(j));
		platform_fprintf("\t\t>Step_%d.</event>\n", j);
#endif
		stat->schedulingTime += timeEndScheduling[j] - timeStartScheduling[j];
	}
	for (UINT32 j=0 ; j<nbStepsGraph; j++){
#if PRINT_REAL_GANTT
		platform_fprintf("\t<event\n");
		platform_fprintf("\t\tstart=\"%u\"\n", 	timeStartGraph[j]);
		platform_fprintf("\t\tend=\"%u\"\n",	timeEndGraph[j]);
		platform_fprintf("\t\ttitle=\"Graph_%d\"\n", j);
		platform_fprintf("\t\tmapping=\"Master\"\n");
		platform_fprintf("\t\tcolor=\"%s\"\n", regenerateColor(j));
		platform_fprintf("\t\t>Step_%d.</event>\n", j);
#endif
		stat->graphTransfoTime += timeEndGraph[j] - timeStartGraph[j];
	}

	// Writing execution data for each slave.
	for(UINT32 slave=0; slave<arch->getNbActiveSlaves(); slave++){
		SendInfoData::send(slave);

		UINT32 msgType = platform_QPopUINT32(slave, platformCtrlQ);
		if(msgType != MSG_EXEC_TIMES)
			exitWithCode(1068);

		UINT32 nbTasks = platform_QPopUINT32(slave, platformCtrlQ);

		for (UINT32 j=0 ; j<nbTasks; j++){
			SRDAGVertex* vertex = dag->getVertex(platform_QPopUINT32(slave, platformCtrlQ)); // data[0] contains the vertex's id.
			UINT32 startTime = platform_QPopUINT32(slave, platformCtrlQ);
			UINT32 endTime = platform_QPopUINT32(slave, platformCtrlQ);

			UINT32 k;
			switch(vertex->getType()){
			case Normal:
			case ConfigureActor:
				for(k=0; k<stat->nbActor; k++){
					if(stat->actors[k] == vertex->getReference()){
						stat->actorTimes[k] += endTime - startTime;
						stat->actorIterations[k]++;
						break;
					}
				}
				if(k == stat->nbActor){
					stat->actors[stat->nbActor] = vertex->getReference();
					stat->actorTimes[stat->nbActor] = endTime - startTime;
					stat->actorIterations[k] = 1;
					stat->nbActor++;
				}

				if(stat->actors[k]->getFunction_index() == 3){
					stat->latencies[nbIter++] = endTime - nbIter*PERIOD;
				}
				break;
			case Broadcast:
				stat->broadcastTime += endTime - startTime;
				break;
			case Explode:
				stat->explodeTime += endTime - startTime;
				break;
			case Implode:
				stat->implodeTime += endTime - startTime;
				break;
			case RoundBuffer:
				stat->roundBufferTime += endTime - startTime;
				break;
			}

#if PRINT_REAL_GANTT
			vertex->getName(name, MAX_VERTEX_NAME_SIZE);
			platform_fprintf("\t<event\n");
			platform_fprintf("\t\tstart=\"%lu\"\n", startTime);
			platform_fprintf("\t\tend=\"%lu\"\n",	endTime);
			platform_fprintf("\t\ttitle=\"%s\"\n", name);
			platform_fprintf("\t\tmapping=\"%s\"\n", arch->getSlaveName(slave));
			platform_fprintf("\t\tcolor=\"%s\"\n", regenerateColor(vertex->getId()));
			platform_fprintf("\t\t>%s.</event>\n", name);

			if(startTime > endTime){
				printf("Receive bad time\n");
			}
#endif
			if(stat->globalEndTime < endTime)
				stat->globalEndTime = endTime;
		}
	}
#if PRINT_REAL_GANTT
	platform_fprintf("</data>\n");
	platform_fclose();
#endif
}

void Launcher::resolveParameters(Architecture *arch, SRDAGGraph* topDag){
	UINT32 slave = 0;
	while(nbParamToRecv != 0){
		UINT32 msgType;
		if(platform_QNonBlockingPop(slave, platformCtrlQ, &msgType, sizeof(UINT32)) == sizeof(UINT32)){
			if(msgType != MSG_PARAM_VALUE) exitWithCode(1068);
			UINT32 vxId = platform_QPopUINT32(slave, platformCtrlQ);
			PiSDFConfigVertex* refConfigVx = (PiSDFConfigVertex*)(topDag->getVertex(vxId)->getReference());
			for(UINT32 j = 0; j < refConfigVx->getNbRelatedParams(); j++){
				topDag->getVertex(vxId)->setRelatedParamValue(j,platform_QPopUINT32(slave, platformCtrlQ));
			}
			nbParamToRecv -= refConfigVx->getNbRelatedParams();
		}
		slave = (slave+1)%arch->getNbActiveSlaves();
	}
}
