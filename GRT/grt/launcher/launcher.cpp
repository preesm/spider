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
#include <grt_definitions.h>
#include <execution/monitor.h>

#include <platform_data_queue.h>

#define PRINT_ACTOR_IN_DOT_FILE		0

static Memory memory = Memory(0x0, 0);
#define END_MEM_SIZE 10000

static UINT32 nbFifo;
static UINT32 nbParamToRecv;

static UINT32 timeStartGraph[MAX_PISDF_STEPS];
static UINT32 timeEndGraph[MAX_PISDF_STEPS];
static UINT32 timeStartTaskOrdering[MAX_PISDF_STEPS];
static UINT32 timeEndTaskOrdering[MAX_PISDF_STEPS];
static UINT32 timeStartMapping[MAX_PISDF_STEPS];
static UINT32 timeEndMapping[MAX_PISDF_STEPS];
static UINT32 actorsByStep[MAX_PISDF_STEPS];
static UINT32 nbStepsTaskOrdering=0;
static UINT32 nbStepsMapping=0;
static UINT32 nbStepsGraph=0;

static UINT32 endMem;

void Launcher::initGraphTime(){
	timeStartGraph[nbStepsGraph] = platform_time_getValue();
}
void Launcher::endGraphTime(){
	timeEndGraph[nbStepsGraph] = platform_time_getValue();
	nbStepsGraph++;
}

void Launcher::initTaskOrderingTime(){
	timeStartTaskOrdering[nbStepsTaskOrdering] = platform_time_getValue();
}
void Launcher::endTaskOrderingTime(){
	timeEndTaskOrdering[nbStepsTaskOrdering] = platform_time_getValue();
	nbStepsTaskOrdering++;
}

void Launcher::initMappingTime(){
	timeStartMapping[nbStepsMapping] = platform_time_getValue();
}

void Launcher::setActorsNb(UINT32 nb){
	actorsByStep[nbStepsMapping] = nb;
}

void Launcher::endMappingTime(){
	timeEndMapping[nbStepsMapping] = platform_time_getValue();
	nbStepsMapping++;
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

void Launcher::launch(SRDAGGraph* graph, Architecture* arch, BaseSchedule* schedule){
	/* Creating Tasks */
	for(int slave=0; slave < arch->getNbActiveSlaves(); slave++){
		for(int i=0; i < schedule->getNbVertices(slave); i++){
			SRDAGVertex* vertex = schedule->getVertex(slave,i);
			if(vertex->getState() == SRDAG_Executable){
				CreateTaskMsg::send(slave, vertex);
				if(vertex->getType() == ConfigureActor)
					nbParamToRecv += ((PiSDFConfigVertex*)vertex)->getNbRelatedParams();
			}
		}
	}
}

void Launcher::assignFifoVertex(SRDAGVertex* vertex){
	UINT32 base, offset;
	int i;
	SRDAGEdge* edge;
	switch(vertex->getType()){
	 case Explode:
		base = vertex->getInputEdge(0)->getFifoAddress();
		offset = 0;
		for (i = 0; i < vertex->getNbOutputEdge(); i++){
			edge = vertex->getOutputEdge(i);
			edge->setFifoId(nbFifo);
			nbFifo = (nbFifo+1)%3000;
			edge->setFifoAddress(base+offset);
			offset += edge->getTokenRate();
		}
		break;
	 case Broadcast:
		 for (i = 0; i < vertex->getNbOutputEdge(); i++){
			edge = vertex->getOutputEdge(i);
			if(edge->getFifoId() == -1){
				edge->setFifoId(nbFifo);
				nbFifo = (nbFifo+1)%3000;
				edge->setFifoAddress(vertex->getInputEdge(0)->getFifoAddress());
			}
		}
		 break;
	 case Implode:
	 default:

		 for (i = 0; i < vertex->getNbOutputEdge(); i++){
			 SRDAGVertex* implode = vertex->getOutputEdge(i)->getSink();
			 if(implode->getType() == Implode){
				 BOOL suitable = (implode->getOutputEdge(0)->getFifoId() == -1);
				 for (int j=0; suitable && j < implode->getNbInputEdge(); j++){
					 SRDAGVertex* pred = implode->getInputEdge(j)->getSource();
					 if(pred->getType() != Normal){
						 suitable = FALSE;
					 }
				 }
				 if(suitable){
					 edge = implode->getOutputEdge(0);
					 UINT32 mem = memory.alloc(edge->getTokenRate());
					 implode->getOutputEdge(0)->setFifoId(nbFifo);
						nbFifo = (nbFifo+1)%3000;
					 implode->getOutputEdge(0)->setFifoAddress(mem);

					 UINT32 offset=0;
					 for (int j=0; j < implode->getNbInputEdge(); j++){
						 edge = implode->getInputEdge(j);
						 edge->setFifoId(nbFifo);
							nbFifo = (nbFifo+1)%3000;
						 edge->setFifoAddress(mem+offset);
						 offset += edge->getTokenRate();
					 }
				 }
			 }
		 }

		 if(vertex->getFctIx() == SWICTH_FUNCT_IX){
			 vertex->getOutputEdge(0)->setFifoId(nbFifo);
				nbFifo = (nbFifo+1)%3000;
			 vertex->getOutputEdge(0)->setFifoAddress(vertex->getInputEdge(2)->getFifoAddress());
		 }

		for (i = 0; i < vertex->getNbOutputEdge(); i++){
			edge = vertex->getOutputEdge(i);
//			if(edge->getSink()->getType() == Implode){
//				assignFifoVertex(edge->getSink());
//			}

			if(edge->getFifoId() == -1){
				edge->setFifoId(nbFifo);
				nbFifo = (nbFifo+1)%3000;
				 if(edge->getSink()->getFctIx() == END_FUNCT_IX){
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
#if EXEC == 1
	CreateTaskMsg::send(slave, vertex);
	if(vertex->getType() == ConfigureActor)
		nbParamToRecv += ((PiSDFConfigVertex*)(vertex->getReference()))->getNbRelatedParams();
#endif
	vertex->setState(SRDAG_Executed);
}

void Launcher::init(){
	nbStepsMapping = nbStepsTaskOrdering = nbStepsGraph = 0;
	nbFifo = nbParamToRecv = 0;
	memory = Memory(0x0, platform_getDataMemSize());

	endMem = memory.alloc(END_MEM_SIZE);
	nbFifo = 0;
}

void Launcher::reset(){
	nbParamToRecv = 0;
	memory = Memory(0x0, platform_getDataMemSize());
	endMem = memory.alloc(END_MEM_SIZE);
}

static void getVertexName(char* name, UINT32 sizeMax, taskTime task){
	int len;
	switch(task.type){
	case Normal:
	case ConfigureActor:
		len = snprintf(name,MAX_VERTEX_NAME_SIZE,"(%d)%s_%d_%d", task.globalIx, task.pisdfVertex->getName(), task.iter, task.repet);
		break;
	case Explode:
		len = snprintf(name,MAX_VERTEX_NAME_SIZE,"(%d)Exp_%d", task.globalIx, task.srdagIx);
		break;
	case Implode:
		len = snprintf(name,MAX_VERTEX_NAME_SIZE,"(%d)Imp_%d", task.globalIx, task.srdagIx);
		break;
	case RoundBuffer:
		len = snprintf(name,MAX_VERTEX_NAME_SIZE,"(%d)RB_%d", task.globalIx, task.srdagIx);
		break;
	case Broadcast:
		len = snprintf(name,MAX_VERTEX_NAME_SIZE,"(%d)BR_%d", task.globalIx, task.srdagIx);
		break;
	case Init:
		len = snprintf(name,MAX_VERTEX_NAME_SIZE,"(%d)Init_%d", task.globalIx, task.srdagIx);
		break;
	case End:
		len = snprintf(name,MAX_VERTEX_NAME_SIZE,"(%d)End_%d", task.globalIx, task.srdagIx);
		break;
	}
	if(len > MAX_VERTEX_NAME_SIZE)
		exitWithCode(1075);
}

//#define PRINT_GRAPH 1
void Launcher::createRealTimeGantt(Architecture *arch, SRDAGGraph *dag, const char *filePathName, ExecutionStat* stat, bool latex){
	// Creating the Gantt with real times.
	memset(stat, 0, sizeof(ExecutionStat));

#if PRINT_REAL_GANTT
	platform_fopen(filePathName);
	FILE* flatex;

	// Writing header
	platform_fprintf("<data>\n");

	char name[MAX_VERTEX_NAME_SIZE];

	if(latex){
		char file[MAX_FILE_NAME_SIZE+50];
		sprintf(file, "%s_latex", filePathName);
		flatex = fopen(file, "w+");
		fprintf(flatex, "<!-- latex\n");
		fprintf(flatex, "{");
	}
#endif

	// Writing execution data for the master.
	stat->taskOrderingTime = 0;
	for (UINT32 j=0 ; j<nbStepsTaskOrdering; j++){
#if PRINT_REAL_GANTT
		platform_fprintf("\t<event\n");
		platform_fprintf("\t\tstart=\"%u\"\n", 	timeStartTaskOrdering[j]);
		platform_fprintf("\t\tend=\"%u\"\n",	timeEndTaskOrdering[j]);
		platform_fprintf("\t\ttitle=\"TaskOrdering_%d\"\n", j);
		platform_fprintf("\t\tmapping=\"Master\"\n");
		platform_fprintf("\t\tcolor=\"%s\"\n", regenerateColor(j));
		platform_fprintf("\t\t>Step_%d.</event>\n", j);

		if(latex){
			fprintf(flatex, "%d/", timeStartTaskOrdering[j]/1000 );/*start*/
			fprintf(flatex, "%d/", (timeEndTaskOrdering[j] - timeStartTaskOrdering[j])/1000);/*duration*/
			fprintf(flatex, "%d/",	 0);/*core index*/
			fprintf(flatex, "%s/",   "");/*name*/
			fprintf(flatex, "color%d,\n",15); // color taskordering
		}
#endif
		stat->taskOrderingTime += timeEndTaskOrdering[j] - timeStartTaskOrdering[j];
	}

	UINT32 mappingNb = 0;
	stat->mappingTime = 0;
	for (UINT32 j=0 ; j<nbStepsMapping; j++){
#if PRINT_REAL_GANTT
		platform_fprintf("\t<event\n");
		platform_fprintf("\t\tstart=\"%u\"\n", 	timeStartMapping[j]);
		platform_fprintf("\t\tend=\"%u\"\n",	timeEndMapping[j]);
		platform_fprintf("\t\ttitle=\"Mapping_%d\"\n", j);
		platform_fprintf("\t\tmapping=\"Master\"\n");
		platform_fprintf("\t\tcolor=\"%s\"\n", regenerateColor(j));
		platform_fprintf("\t\t>Step_%d.</event>\n", j);

		if(latex){
			fprintf(flatex, "%d/", timeStartMapping[j]/1000 );/*start*/
			fprintf(flatex, "%d/", (timeEndMapping[j] - timeStartMapping[j])/1000);/*duration*/
			fprintf(flatex, "%d/",	 0);/*core index*/
			fprintf(flatex, "%s/",   "");/*name*/
			fprintf(flatex, "color%d,\n",15); // color mapping
		}

#endif
		stat->mappingTime += timeEndMapping[j] - timeStartMapping[j];
		mappingNb += actorsByStep[j];
	}

	if(mappingNb != 0)
		printf("Mapped %d tasks in %d cycles (%d cycles/tasks)\n",
			mappingNb,
			stat->mappingTime,
			stat->mappingTime/mappingNb);


	stat->graphTransfoTime = 0;
	for (UINT32 j=0 ; j<nbStepsGraph; j++){
#if PRINT_REAL_GANTT
		platform_fprintf("\t<event\n");
		platform_fprintf("\t\tstart=\"%u\"\n", 	timeStartGraph[j]);
		platform_fprintf("\t\tend=\"%u\"\n",	timeEndGraph[j]);
		platform_fprintf("\t\ttitle=\"Graph_%d\"\n", j);
		platform_fprintf("\t\tmapping=\"Master\"\n");
		platform_fprintf("\t\tcolor=\"%s\"\n", regenerateColor(j));
		platform_fprintf("\t\t>Step_%d.</event>\n", j);

		if(latex){
			fprintf(flatex, "%d/", timeStartGraph[j]/1000 );/*start*/
			fprintf(flatex, "%d/", (timeEndGraph[j] - timeStartGraph[j])/1000);/*duration*/
			fprintf(flatex, "%d/",	 0);/*core index*/
			fprintf(flatex, "%s/",   "");/*name*/
			fprintf(flatex, "color%d,\n",15); // color graphtransfo
		}
#endif
		stat->graphTransfoTime += timeEndGraph[j] - timeStartGraph[j];
	}

	// Writing execution data for each slave.
#if EXEC
	for(int slave=0; slave<arch->getNbActiveSlaves(); slave++){
		int nbTasks;

		if(slave !=0){
			SendInfoData::send(slave);
			UINT32 msgType = platform_QPopUINT32(slave);
			if(msgType != MSG_EXEC_TIMES)
				exitWithCode(1068);
			nbTasks = platform_QPopUINT32(slave);
		}else{
			nbTasks = Monitor_getNB();
		}

		for (int j=0 ; j<nbTasks; j++){
			taskTime task;
			UINT32 execTime;

			if(slave !=0){
				platform_QPop(slave, &task, sizeof(taskTime));
			}else{
				task = Monitor_get(j);
			}

			execTime = task.end - task.start;

			int k;
			switch(task.type){
			case Normal:
			case ConfigureActor:
				for(k=0; k<stat->nbActor; k++){
					if(stat->actors[k] == task.pisdfVertex){
						stat->actorTimes[k] += execTime;
						stat->actorIterations[k]++;
						break;
					}
				}
				if(k == stat->nbActor){
					stat->actors[stat->nbActor] = task.pisdfVertex;
					stat->actorTimes[stat->nbActor] = execTime;
					stat->actorIterations[k] = 1;
					stat->nbActor++;
				}

	//			if(stat->actors[k]->getFunction_index() == 3){
	//				stat->latencies[t.end/PERIOD] = t.end%PERIOD + PERIOD;
	//			}
				break;
			case Broadcast:
				stat->broadcastTime += execTime;
				break;
			case Explode:
				stat->explodeTime += execTime;
				break;
			case Implode:
				stat->implodeTime += execTime;
				break;
			case RoundBuffer:
				stat->roundBufferTime += execTime;
				break;
			case Init:
			case End:
				break;
			}

	#if PRINT_REAL_GANTT
			getVertexName(name, MAX_VERTEX_NAME_SIZE, task);
			platform_fprintf("\t<event\n");
			platform_fprintf("\t\tstart=\"%lu\"\n", task.start);
			platform_fprintf("\t\tend=\"%lu\"\n",	task.end);
			platform_fprintf("\t\ttitle=\"%s\"\n", name);
			platform_fprintf("\t\tmapping=\"%s\"\n", arch->getSlaveName(slave));
			platform_fprintf("\t\tcolor=\"%s\"\n", regenerateColor(task.srdagIx));
			platform_fprintf("\t\t>%s.</event>\n", name);

			if(latex){
				fprintf(flatex, "%d/", task.start/1000 );/*start*/
				fprintf(flatex, "%d/", (task.end - task.start)/1000);/*duration*/
				fprintf(flatex, "%d/",	 slave);/*core index*/
				fprintf(flatex, "%s/",   "");/*name*/

				if(task.pisdfVertex != 0 && task.pisdfVertex->getFunction_index() == 7)
					fprintf(flatex, "color%d,\n", task.repet); // color Id
				else
					fprintf(flatex, "color%d,\n", 15); // color Id
			}

			if(task.start > task.end){
				printf("Receive bad time\n");
			}
	#endif
			if(stat->globalEndTime < task.end)
				stat->globalEndTime = task.end;

			if(task.globalIx >= ITER_MAX)
				printf("Error : task.globalIx %d > ITER_MAX %d \n", task.globalIx, ITER_MAX);

			if(stat->endTime[task.globalIx] < task.end)
				stat->endTime[task.globalIx] = task.end;
		}
	}
#endif
#if PRINT_REAL_GANTT
	platform_fprintf("</data>\n");
	platform_fclose();

	if(latex){
		fprintf(flatex, "}\n");
		fprintf(flatex, "latex -->\n");
		fclose(flatex);
	}
#endif
}

void Launcher::resolveParameters(Architecture *arch, SRDAGGraph* topDag){
	int slave = 0;
	UINT32 paramValues[MAX_NB_PiSDF_PARAMS];
	while(nbParamToRecv != 0){
		if(slave == 0){
			int vxId, nbParam;
			if(popParam(&vxId, &nbParam, paramValues)){
				SRDAGVertex* cfgVertex = (SRDAGVertex*)(topDag->getVertexFromIx(vxId));
				PiSDFConfigVertex* refConfigVx = (PiSDFConfigVertex*)(cfgVertex->getReference());
				nbParamToRecv -= refConfigVx->getNbRelatedParams();
				for(int i=0; i<nbParam; i++)
					cfgVertex->setRelatedParamValue(i,paramValues[i]);
			}
		}else{
			UINT32 msgType;
			if(platform_QNonBlockingPop(slave, &msgType, sizeof(UINT32)) == sizeof(UINT32)){
				if(msgType != MSG_PARAM_VALUE) exitWithCode(1068);
				int vxId = platform_QPopUINT32(slave);
				SRDAGVertex* cfgVertex = (SRDAGVertex*)(topDag->getVertexFromIx(vxId));
				PiSDFConfigVertex* refConfigVx = (PiSDFConfigVertex*)(cfgVertex->getReference());
				for(UINT32 j = 0; j < refConfigVx->getNbRelatedParams(); j++){
					cfgVertex->setRelatedParamValue(j,platform_QPopUINT32(slave));
				}
				nbParamToRecv -= refConfigVx->getNbRelatedParams();
			}
		}
		slave = (slave+1)%arch->getNbActiveSlaves();
	}
}
