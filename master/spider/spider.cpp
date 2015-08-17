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

#include <stdio.h>
#include <launcher/Launcher.h>

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
	case MEMALLOC_SPECIAL_ACTOR:
		memAlloc = new SpecialActorMemAlloc(start, size);
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

static Time start = 0;

void spider_startMonitoring(){
	if(start != 0)
		throw "Try to monitor 2 different things in the same time";
	start = Platform::get()->getTime();
}

void spider_endMonitoring(TraceSpiderType type){
	if(start == 0)
		throw "End monitor with no starting point";
	Launcher::get()->sendTraceSpider(type, start, Platform::get()->getTime());
	start = 0;
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

static inline void printGrantt_SRDAGVertex(int ganttFile, int latexFile, Archi* archi, SRDAGVertex* vertex, Time start, Time end, int lrtIx){
	char name[100];
	static int i=0;
	vertex->toString(name, 100);

	Platform::get()->fprintf(ganttFile, "\t<event\n");
	Platform::get()->fprintf(ganttFile, "\t\tstart=\"%u\"\n", 	start);
	Platform::get()->fprintf(ganttFile, "\t\tend=\"%u\"\n",		end);
	Platform::get()->fprintf(ganttFile, "\t\ttitle=\"%s\"\n", 	name);
	Platform::get()->fprintf(ganttFile, "\t\tmapping=\"%s\"\n", 	archi->getPEName(lrtIx));
	Platform::get()->fprintf(ganttFile, "\t\tcolor=\"%s\"\n", regenerateColor(i++));
	Platform::get()->fprintf(ganttFile, "\t\t>Step_%d.</event>\n", name);

	/* Latex File */
	Platform::get()->fprintf(latexFile, "%d/", start/1000); /* Start */
	Platform::get()->fprintf(latexFile, "%d/", (end - start)/1000); /* Duration */
	Platform::get()->fprintf(latexFile, "%d/", lrtIx); /* Core index */
	Platform::get()->fprintf(latexFile, "%s/", ""); /* name */

	if(vertex->getFctId() != -1)
		Platform::get()->fprintf(latexFile, "color%d,\n", vertex->getFctId()); /* Color */
	else
		Platform::get()->fprintf(latexFile, "color%d,\n", 15); /* Color */

}

static const char* spiderTaskName[5] = {
		"",
		"Graph handling",
		"Memory Allocation",
		"Task Scheduling",
		"Graph Optimization"
};

void spider_printGantt(Archi* archi, SRDAGGraph* srdag, const char* ganttPath, const char* latexPath, ExecutionStat* stat){
	int ganttFile = Platform::get()->fopen(ganttPath);
	int latexFile = Platform::get()->fopen(latexPath);

	// Writing header
	Platform::get()->fprintf(ganttFile, "<data>\n");
	Platform::get()->fprintf(latexFile, "<!-- latex\n{");


	// Popping data from Trace queue.
	stat->taskOrderingTime = 0;
	stat->globalEndTime = 0;

	stat->forkTime = 0;
	stat->joinTime = 0;
	stat->rbTime = 0;
	stat->brTime = 0;
	stat->nbActor = 0;

	stat->memoryUsed = memAlloc->getMemUsed();

	TraceMsg* traceMsg;
	int n = Launcher::get()->getNLaunched();
	while(n){
		if(Platform::getSpiderCommunicator()->trace_start_recv((void**)&traceMsg)){
			switch (traceMsg->msgIx) {
				case TRACE_JOB:{
					SRDAGVertex* vertex = srdag->getVertexFromIx(traceMsg->srdagIx);
					Time execTime = traceMsg->end - traceMsg->start;

					printGrantt_SRDAGVertex(
							ganttFile,
							latexFile,
							archi,
							vertex,
							traceMsg->start,
							traceMsg->end,
							traceMsg->lrtIx);

					/* Update Stats */
					stat->globalEndTime = std::max(traceMsg->end, stat->globalEndTime);

					switch(vertex->getType()){
						case SRDAG_NORMAL:{
							int i;
							int lrtType = archi->getPEType(traceMsg->lrtIx);
							for(i=0; i<stat->nbActor; i++){
								if(stat->actors[i] == vertex->getReference()){
									stat->actorTimes[i][lrtType] += execTime;
									stat->actorIterations[i][lrtType]++;

									stat->actorFisrt[i] = std::min(stat->actorFisrt[i], traceMsg->start);
									stat->actorLast[i] = std::max(stat->actorLast[i], traceMsg->end);
									break;
								}
							}
							if(i == stat->nbActor){
								stat->actors[stat->nbActor] = vertex->getReference();

								memset(stat->actorTimes[stat->nbActor], 0, MAX_STATS_PE_TYPES*sizeof(Time));
								memset(stat->actorIterations[stat->nbActor], 0, MAX_STATS_PE_TYPES*sizeof(Time));

								stat->actorTimes[stat->nbActor][lrtType] += execTime;
								stat->actorIterations[i][lrtType]++;
								stat->nbActor++;

								stat->actorFisrt[i] = traceMsg->start;
								stat->actorLast[i] = traceMsg->end;
							}
							break;}
						case SRDAG_BROADCAST:
							stat->brTime += execTime;
							break;
						case SRDAG_FORK:
							stat->forkTime += execTime;
							break;
						case SRDAG_JOIN:
							stat->joinTime += execTime;
							break;
						case SRDAG_ROUNDBUFFER:
							stat->rbTime += execTime;
							break;
						case SRDAG_INIT:
						case SRDAG_END:
							break;
					}

					break;}
				case TRACE_SPIDER:{
					static int i=0;
					/* Gantt File */
					Platform::get()->fprintf(ganttFile, "\t<event\n");
					Platform::get()->fprintf(ganttFile, "\t\tstart=\"%u\"\n", 	traceMsg->start);
					Platform::get()->fprintf(ganttFile, "\t\tend=\"%u\"\n",		traceMsg->end);
					Platform::get()->fprintf(ganttFile, "\t\ttitle=\"%s\"\n", 	spiderTaskName[traceMsg->spiderTask]);
					Platform::get()->fprintf(ganttFile, "\t\tmapping=\"%s\"\n", archi->getPEName(traceMsg->lrtIx));
					Platform::get()->fprintf(ganttFile, "\t\tcolor=\"%s\"\n", 	regenerateColor(i++));
					Platform::get()->fprintf(ganttFile, "\t\t>Step_%d.</event>\n", spiderTaskName[traceMsg->spiderTask]);

					/* Latex File */
					Platform::get()->fprintf(latexFile, "%d/", traceMsg->start/1000); /* Start */
					Platform::get()->fprintf(latexFile, "%d/", (traceMsg->end - traceMsg->start)/1000); /* Duration */
					Platform::get()->fprintf(latexFile, "%d/", 0); /* Core index */
					Platform::get()->fprintf(latexFile, "%s/", ""); /* name */
					Platform::get()->fprintf(latexFile, "color%d,\n",15); /* Color */
					break;}
				default:
					throw "Unhandled trace msg";
					break;
			}
			Platform::getSpiderCommunicator()->trace_end_recv();
			n--;
		}
	}
	Launcher::get()->rstNLaunched();

	Platform::get()->fprintf(ganttFile, "</data>\n");
	Platform::get()->fclose(ganttFile);

	Platform::get()->fprintf(latexFile, "}\nlatex -->\n");
	Platform::get()->fclose(latexFile);
}
