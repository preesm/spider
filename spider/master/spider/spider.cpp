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

#include "spider.h"

#include <cstdio>
#include <launcher/Launcher.h>

static SpiderCommunicator* spiderCom = 0;
static LRT* lrt = 0;

static Stack* srdagStack = 0;
static Stack* transfoStack = 0;

static SRDAGGraph* srdag = 0;

static MemAlloc* memAlloc;
static Scheduler* scheduler;

void spider_init(SpiderConfig cfg){
	spider_setMemAllocType(cfg.memAllocType, (long)cfg.memAllocStart, cfg.memAllocSize);
	spider_setSchedulerType(cfg.schedulerType);
	spider_setSrdagStack(cfg.srdagStack);
	spider_setTransfoStack(cfg.transfoStack);
}

void spider_free(){
	if(srdag != 0)
		delete srdag;
	if(memAlloc != 0)
		delete memAlloc;
	if(scheduler != 0)
		delete scheduler;
	if(srdagStack != 0)
		delete srdagStack;
	if(transfoStack != 0)
		delete transfoStack;
}

void spider_launch(
		Archi* archi,
		PiSDFGraph* pisdf){
	delete srdag;
	srdagStack->freeAll();
	memAlloc->reset();

	srdag = new SRDAGGraph(srdagStack);

	jit_ms(pisdf, archi, srdag, transfoStack, memAlloc, scheduler);

}

void spider_setMemAllocType(MemAllocType type, int start, int size){
	if(memAlloc != 0){
		delete memAlloc;
	}
	switch(type){
	case MEMALLOC_DUMMY:
		memAlloc = new DummyMemAlloc(start, size);
		break;
	}
}

void spider_setSchedulerType(SchedulerType type){
	if(scheduler != 0){
		delete scheduler;
	}
	switch(type){
	case SCHEDULER_LIST:
		scheduler = new ListScheduler();
		break;
	}
}

void spider_setSrdagStack(StackConfig cfg){
	if(srdagStack != 0){
		delete srdagStack;
	}
	switch(cfg.type){
	case STACK_DYNAMIC:
		srdagStack = new DynStack(cfg.name);
		break;
	case STACK_STATIC:
		srdagStack = new StaticStack(cfg.name, cfg.start, cfg.size);
		break;
	}
}

void spider_setTransfoStack(StackConfig cfg){
	if(transfoStack != 0){
		delete transfoStack;
	}
	switch(cfg.type){
	case STACK_DYNAMIC:
		transfoStack = new DynStack(cfg.name);
		break;
	case STACK_STATIC:
		transfoStack = new StaticStack(cfg.name, cfg.start, cfg.size);
		break;
	}
}

SRDAGGraph* spider_getLastSRDAG(){
	return srdag;
}

void setSpiderCommunicator(SpiderCommunicator* com){
	spiderCom = com;
}

SpiderCommunicator* getSpiderCommunicator(){
	return spiderCom;
}

void setLrt(LRT* l){
	lrt = l;
}

LRT* getLrt(){
	return lrt;
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

static inline void printGrantt_SRDAGVertex(int file, Archi* archi, SRDAGVertex* vertex, Time start, Time end, int lrtIx){
	char name[100];
	static int i=0;
	vertex->toString(name, 100);

	Platform::get()->fprintf(file, "\t<event\n");
	Platform::get()->fprintf(file, "\t\tstart=\"%u\"\n", 	start);
	Platform::get()->fprintf(file, "\t\tend=\"%u\"\n",		end);
	Platform::get()->fprintf(file, "\t\ttitle=\"%s\"\n", 	name);
	Platform::get()->fprintf(file, "\t\tmapping=\"%s\"\n", 	archi->getPEName(lrtIx));
	Platform::get()->fprintf(file, "\t\tcolor=\"%s\"\n", regenerateColor(i++));
	Platform::get()->fprintf(file, "\t\t>Step_%d.</event>\n", name);

}

void spider_printGantt(Archi* archi, SRDAGGraph* srdag, const char* ganttPath, const char* latexPath, ExecutionStat* stat){
	int ganttFile = Platform::get()->fopen(ganttPath);
	int latexFile = Platform::get()->fopen(latexPath);

	// Writing header
	Platform::get()->fprintf(ganttFile, "<data>\n");
	Platform::get()->fprintf(latexFile, "<!-- latex\n{");


	// Popping data from Trace queue.
	stat->taskOrderingTime = 0;

	TraceMsg* traceMsg;
	int n = Launcher::get()->getNLaunched();
	while(n){
		if(getSpiderCommunicator()->trace_start_recv((void**)&traceMsg)){
			switch (traceMsg->msgIx) {
				case TRACE_JOB:
					printGrantt_SRDAGVertex(
							ganttFile, archi,
							srdag->getVertexFromIx(traceMsg->srdagIx),
							traceMsg->start,
							traceMsg->end,
							traceMsg->lrtIx);
					getSpiderCommunicator()->trace_end_recv();
					n--;
					break;
				default:
					throw "Unhandled trace msg";
					break;
			}
		}
	}
	Launcher::get()->rstNLaunched();
//		platform_fprintf("master\t<event\n");
//		platform_fprintf("\t\tstart=\"%u\"\n", 	timeStartTaskOrdering[j]);
//		platform_fprintf("\t\tend=\"%u\"\n",	timeEndTaskOrdering[j]);
//		platform_fprintf("\t\ttitle=\"TaskOrdering_%d\"\n", j);
//		platform_fprintf("\t\tmapping=\"Master\"\n");
//		platform_fprintf("\t\tcolor=\"%s\"\n", regenerateColor(j));
//		platform_fprintf("\t\t>Step_%d.</event>\n", j);
//
//		if(latex){
//			fprintf(flatex, "%d/", timeStartTaskOrdering[j]/1000 );/*start*/
//			fprintf(flatex, "%d/", (timeEndTaskOrdering[j] - timeStartTaskOrdering[j])/1000);/*duration*/
//			fprintf(flatex, "%d/",	 0);/*core index*/
//			fprintf(flatex, "%s/",   "");/*name*/
//			fprintf(flatex, "color%d,\n",15); // color taskordering
//		}
//		stat->taskOrderingTime += timeEndTaskOrdering[j] - timeStartTaskOrdering[j];
//	}
//
//	UINT32 mappingNb = 0;
//	stat->mappingTime = 0;
//	for (UINT32 j=0 ; j<nbStepsMapping; j++){
//		platform_fprintf("\t<event\n");
//		platform_fprintf("\t\tstart=\"%u\"\n", 	timeStartMapping[j]);
//		platform_fprintf("\t\tend=\"%u\"\n",	timeEndMapping[j]);
//		platform_fprintf("\t\ttitle=\"Mapping_%d\"\n", j);
//		platform_fprintf("\t\tmapping=\"Master\"\n");
//		platform_fprintf("\t\tcolor=\"%s\"\n", regenerateColor(j));
//		platform_fprintf("\t\t>Step_%d.</event>\n", j);
//
//		if(latex){
//			fprintf(flatex, "%d/", timeStartMapping[j]/1000 );/*start*/
//			fprintf(flatex, "%d/", (timeEndMapping[j] - timeStartMapping[j])/1000);/*duration*/
//			fprintf(flatex, "%d/",	 0);/*core index*/
//			fprintf(flatex, "%s/",   "");/*name*/
//			fprintf(flatex, "color%d,\n",15); // color mapping
//		}
//
//		stat->mappingTime += timeEndMapping[j] - timeStartMapping[j];
//		mappingNb += actorsByStep[j];
//	}
//
//	if(mappingNb != 0)
//		printf("Mapped %d tasks in %d cycles (%d cycles/tasks)\n",
//			mappingNb,
//			stat->mappingTime,
//			stat->mappingTime/mappingNb);
//
//
//	stat->graphTransfoTime = 0;
//	for (UINT32 j=0 ; j<nbStepsGraph; j++){
//		platform_fprintf("\t<event\n");
//		platform_fprintf("\t\tstart=\"%u\"\n", 	timeStartGraph[j]);
//		platform_fprintf("\t\tend=\"%u\"\n",	timeEndGraph[j]);
//		platform_fprintf("\t\ttitle=\"Graph_%d\"\n", j);
//		platform_fprintf("\t\tmapping=\"Master\"\n");
//		platform_fprintf("\t\tcolor=\"%s\"\n", regenerateColor(j));
//		platform_fprintf("\t\t>Step_%d.</event>\n", j);
//
//		if(latex){
//			fprintf(flatex, "%d/", timeStartGraph[j]/1000 );/*start*/
//			fprintf(flatex, "%d/", (timeEndGraph[j] - timeStartGraph[j])/1000);/*duration*/
//			fprintf(flatex, "%d/",	 0);/*core index*/
//			fprintf(flatex, "%s/",   "");/*name*/
//			fprintf(flatex, "color%d,\n",15); // color graphtransfo
//		}
//		stat->graphTransfoTime += timeEndGraph[j] - timeStartGraph[j];
//	}
//
//	// Writing execution data for each slave.
//	for(int slave=0; slave<arch->getNbActiveSlaves(); slave++){
//		int nbTasks;
//
//		if(slave !=0){
//			SendInfoData::send(slave);
//			UINT32 msgType = platform_QPopUINT32(slave);
//			if(msgType != MSG_EXEC_TIMES)
//				exitWithCode(1068);
//			nbTasks = platform_QPopUINT32(slave);
//		}else{
//			nbTasks = Monitor_getNB();
//		}
//
//		for (int j=0 ; j<nbTasks; j++){
//			taskTime task;
//			UINT32 execTime;
//
//			if(slave !=0){
//				platform_QPop(slave, &task, sizeof(taskTime));
//			}else{
//				task = Monitor_get(j);
//			}
//
//			execTime = task.end - task.start;
//
//			int k;
//			switch(task.type){
//			case Normal:
//			case ConfigureActor:
//				for(k=0; k<stat->nbActor; k++){
//					if(stat->actors[k] == task.pisdfVertex){
//						stat->actorTimes[k] += execTime;
//						stat->actorIterations[k]++;
//						break;
//					}
//				}
//				if(k == stat->nbActor){
//					stat->actors[stat->nbActor] = task.pisdfVertex;
//					stat->actorTimes[stat->nbActor] = execTime;
//					stat->actorIterations[k] = 1;
//					stat->nbActor++;
//				}
//
//	//			if(stat->actors[k]->getFunction_index() == 3){
//	//				stat->latencies[t.end/PERIOD] = t.end%PERIOD + PERIOD;
//	//			}
//				break;
//			case Broadcast:
//				stat->broadcastTime += execTime;
//				break;
//			case Explode:
//				stat->explodeTime += execTime;
//				break;
//			case Implode:
//				stat->implodeTime += execTime;
//				break;
//			case RoundBuffer:
//				stat->roundBufferTime += execTime;
//				break;
//			case Init:
//			case End:
//				break;
//			}
//
//			getVertexName(name, MAX_VERTEX_NAME_SIZE, task);
//			platform_fprintf("\t<event\n");
//			platform_fprintf("\t\tstart=\"%lu\"\n", task.start);
//			platform_fprintf("\t\tend=\"%lu\"\n",	task.end);
//			platform_fprintf("\t\ttitle=\"%s\"\n", name);
//			platform_fprintf("\t\tmapping=\"%s\"\n", arch->getSlaveName(slave));
//			platform_fprintf("\t\tcolor=\"%s\"\n", regenerateColor(task.srdagIx));
//			platform_fprintf("\t\t>%s.</event>\n", name);
//
//			if(latex){
//				fprintf(flatex, "%d/", task.start/1000 );/*start*/
//				fprintf(flatex, "%d/", (task.end - task.start)/1000);/*duration*/
//				fprintf(flatex, "%d/",	 slave);/*core index*/
//				fprintf(flatex, "%s/",   "");/*name*/
//
//				if(task.pisdfVertex != 0 && task.pisdfVertex->getFunction_index() == 7)
//					fprintf(flatex, "color%d,\n", task.repet); // color Id
//				else
//					fprintf(flatex, "color%d,\n", 15); // color Id
//			}
//
//			if(task.start > task.end){
//				printf("Receive bad time\n");
//			}
//			if(stat->globalEndTime < task.end)
//				stat->globalEndTime = task.end;
//
//			if(task.globalIx >= ITER_MAX)
//				printf("Error : task.globalIx %d > ITER_MAX %d \n", task.globalIx, ITER_MAX);
//
//			if(stat->endTime[task.globalIx] < task.end)
//				stat->endTime[task.globalIx] = task.end;
//		}
//	}

	Platform::get()->fprintf(ganttFile, "</data>\n");
	Platform::get()->fclose(ganttFile);

	Platform::get()->fprintf(latexFile, "}\nlatex -->\n");
	Platform::get()->fclose(latexFile);
}
