/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2017) :
 *
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2017)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013 - 2014)
 *
 * Spider is a dataflow based runtime used to execute dynamic PiSDF
 * applications. The Preesm tool may be used to design PiSDF applications.
 *
 * This software is governed by the CeCILL  license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */
#include "spider.h"

#include <graphs/PiSDF/PiSDFCommon.h>
#include <graphs/SRDAG/SRDAGCommon.h>
#include <graphs/PiSDF/PiSDFGraph.h>
#include <graphs/SRDAG/SRDAGGraph.h>
#include <graphs/PiSDF/PiSDFVertex.h>
#include <graphs/Bipartite/BipartiteGraph.h>

#include <tools/StaticStack.h>
#include <tools/DynStack.h>

#include <graphs/Archi/Archi.h>
#include <graphs/Archi/SharedMemArchi.h>

#include <scheduling/MemAlloc.h>
#include <scheduling/MemAlloc/DummyMemAlloc.h>
#include <scheduling/MemAlloc/SpecialActorMemAlloc.h>
#include <scheduling/Scheduler.h>

#include <scheduling/Scheduler/ListScheduler.h>
#include <scheduling/Scheduler/ListSchedulerOnTheGo.h>
#include <scheduling/Scheduler/RoundRobin.h>
#include <scheduling/Scheduler/RoundRobinScattered.h>

#include <graphTransfo/GraphTransfo.h>

#include <monitor/StackMonitor.h>
#include <monitor/TimeMonitor.h>

#include <SpiderCommunicator.h>
#include <lrt.h>

#include <stdio.h>
#include <launcher/Launcher.h>

#include "platformPThread.h"

// #ifndef __k1__
// #include <HAL/hal/hal_ext.h>
// #endif

#ifdef __k1__
#define CHIP_FREQ ((float)(__bsp_frequency)/(1000*1000))
#endif

#ifndef CHIP_FREQ
#define CHIP_FREQ (1)
#endif

static Archi* archi_;
static PiSDFGraph* pisdf_;
static SRDAGGraph* srdag_;

static MemAlloc* memAlloc_;
static Scheduler* scheduler_;
//static PlatformMPPA* platform;
static PlatformPThread* platform;

static bool verbose_;
static bool useGraphOptim_;
static bool useActorPrecedence_;
static bool traceEnabled_;

void Spider::init(SpiderConfig cfg){

	setGraphOptim(cfg.useGraphOptim);

	setMemAllocType(cfg.memAllocType, (long)cfg.memAllocStart, cfg.memAllocSize);
	setSchedulerType(cfg.schedulerType);

	setActorPrecedence(cfg.useActorPrecedence);
	setVerbose(cfg.verbose);
	setTraceEnabled(cfg.traceEnabled);


	platform = new PlatformPThread(
			cfg.platform.nLrt,
			cfg.platform.shMemSize,
			cfg.platform.fcts,
			cfg.platform.nLrtFcts,
			cfg.archiStack,
			cfg.lrtStack,
			cfg.pisdfStack,
			cfg.srdagStack,
			cfg.transfoStack
	);
}

void Spider::clean(){

	if(srdag_ != 0)
		delete srdag_;
	if(memAlloc_ != 0)
		delete memAlloc_;
	if(scheduler_ != 0)
		delete scheduler_;
		
	if(platform != 0)
		delete platform;

	//StackMonitor::cleanAllStack();
}

void Spider::idle(){
	for(int lrt=0; lrt<archi_->getNPE(); lrt++){
		if(lrt != archi_->getSpiderPeIx()){
			archi_->desactivatePE(lrt);
		}
	}
}

void Spider::iterate(){
	Platform::get()->rstTime();
	/** Set all slave jobIx to 0 */

	delete srdag_;
	StackMonitor::freeAll(SRDAG_STACK);
	memAlloc_->reset();

	srdag_ = new SRDAGGraph();


	jit_ms(pisdf_, archi_, srdag_, memAlloc_, scheduler_);

	//Mise à zéro compteur job
	Platform::get()->rstJobIx();
}

void Spider::setGraphOptim(bool useGraphOptim){
	useGraphOptim_ = useGraphOptim;
}

void Spider::setVerbose(bool verbose){
	verbose_ = verbose;
}

void Spider::setActorPrecedence(bool useActorPrecedence){
	useActorPrecedence_ = useActorPrecedence;
}

void Spider::setTraceEnabled(bool traceEnabled){
	traceEnabled_ = traceEnabled;
}

bool Spider::getVerbose(){
	return verbose_;
}

bool Spider::getGraphOptim(){
	return useGraphOptim_;
}

bool Spider::getActorPrecedence(){
	return useActorPrecedence_;
}

bool Spider::getTraceEnabled(){
	return traceEnabled_;
}

void Spider::setArchi(Archi* archi){
	archi_ = archi;
}

void Spider::setGraph(PiSDFGraph* graph){
	pisdf_ = graph;
}

PiSDFGraph* Spider::getGraph(){
	return pisdf_;
}

Archi* Spider::getArchi(){
	return archi_;
}

void Spider::setMemAllocType(MemAllocType type, int start, int size){
	if(memAlloc_ != 0){
		delete memAlloc_;
	}
	switch(type){
	case MEMALLOC_DUMMY:
		memAlloc_ = new DummyMemAlloc(start, size);
		break;
	case MEMALLOC_SPECIAL_ACTOR:
		memAlloc_ = new SpecialActorMemAlloc(start, size);
		break;
	}
}

void Spider::setSchedulerType(SchedulerType type){
	if(scheduler_ != 0){
		delete scheduler_;
	}
	switch(type){
	case SCHEDULER_LIST:
		scheduler_ = new ListScheduler();
		break;
	case SCHEDULER_LIST_ON_THE_GO:
		scheduler_ = new ListSchedulerOnTheGo();
		break;
	case ROUND_ROBIN:
		scheduler_ = new RoundRobin();
		break;
	case ROUND_ROBIN_SCATTERED:
		scheduler_ = new RoundRobinScattered();
		break;
	}
}

void Spider::printSRDAG(const char* srdagPath){
	return srdag_->print(srdagPath);
}

void Spider::printPiSDF(const char* pisdfPath){
	return pisdf_->print(pisdfPath);
}

void Spider::printActorsStat(ExecutionStat* stat){
	printf("\t%15s:\n", "Actors");
	for(int j=0; j<stat->nPiSDFActor; j++){
		printf("\t%15s:", stat->actors[j]->getName());
		for(int k=0; k<archi_->getNPETypes(); k++)
			if(stat->actorIterations[j][k])
				printf("\t%ld (x%ld)",
						stat->actorTimes[j][k]/stat->actorIterations[j][k],
						stat->actorIterations[j][k]);
			else
				printf("\t%d (x%d)", 0, 0);
		printf("\n");
	}
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

static inline void printGrantt_SRDAGVertex(FILE *ganttFile, FILE *latexFile, Archi* archi, SRDAGVertex* vertex, Time start, Time end, int lrtIx, float latexScaling){
	static char name[200];
	static int i=0;
	vertex->toString(name, 100);

	char* temp_str = (char*) malloc(300*sizeof(char));


	sprintf(temp_str,
		"\t<event\n"
		"\t\tstart=\"%llu\"\n"
		"\t\tend=\"%llu\"\n"
		"\t\ttitle=\"%s_%d_%d\"\n"
		"\t\tmapping=\"%s\"\n"
		"\t\tcolor=\"%s\"\n"
		"\t\t>Step_%d.</event>\n",
		start,
		end,
		name, vertex->getIterId(), vertex->getRefId(),
		archi->getPEName(lrtIx),
		regenerateColor(i++),
		name);

	Platform::get()->fprintf(ganttFile,"%s", temp_str);

	/*
	Platform::get()->fprintf(ganttFile, "\t<event\n");
	Platform::get()->fprintf(ganttFile, "\t\tstart=\"%llu\"\n", 	start);
	Platform::get()->fprintf(ganttFile, "\t\tend=\"%llu\"\n",		end);
	Platform::get()->fprintf(ganttFile, "\t\ttitle=\"%s_%d_%d\"\n", 	name, vertex->getIterId(), vertex->getRefId());
	Platform::get()->fprintf(ganttFile, "\t\tmapping=\"%s\"\n", 	archi->getPEName(lrtIx));
	Platform::get()->fprintf(ganttFile, "\t\tcolor=\"%s\"\n", regenerateColor(i++));
	Platform::get()->fprintf(ganttFile, "\t\t>Step_%d.</event>\n", name);
	*/

	sprintf(temp_str,
		"%f,"
		"%f,"
		"%d,",
		start/latexScaling,
		end/latexScaling,
		lrtIx);

	if(vertex->getFctId() == 7)	sprintf(temp_str + strlen(temp_str), "color%d\n", vertex->getIterId());
	else sprintf(temp_str + strlen(temp_str), "c\n");

	Platform::get()->fprintf(latexFile,"%s", temp_str);

	/* Latex File */
	// Platform::get()->fprintf(latexFile, "%f,", start/latexScaling); /* Start */
	// Platform::get()->fprintf(latexFile, "%f,", end/latexScaling); /* Duration */
	// Platform::get()->fprintf(latexFile, "%d,", lrtIx); /* Core index */

	// if(vertex->getFctId() == 7){
	// 	Platform::get()->fprintf(latexFile, "color%d\n", vertex->getIterId()); /* Color */
	// }else Platform::get()->fprintf(latexFile, "c\n"); /* Color */





// Was already commented
//	if(vertex->getFctId() != -1)
//		Platform::get()->fprintf(latexFile, "color%d\n", vertex->getFctId()); /* Color */
//	else
//		Platform::get()->fprintf(latexFile, "color%d\n", 15); /* Color */

	free(temp_str);

}

void Spider::printGantt(const char* ganttPath, const char* latexPath, ExecutionStat* stat){
	FILE *ganttFile = Platform::get()->fopen(ganttPath);
	if(ganttFile == NULL) throw "Error opening ganttFile";

	FILE *latexFile = Platform::get()->fopen(latexPath);
	if(latexFile == NULL) throw "Error opening latexFile";

	float latexScaling = 1000;

	// Writing header
	Platform::get()->fprintf(ganttFile, "<data>\n");
	Platform::get()->fprintf(latexFile, "start,end,core,color\n");

	// Popping data from Trace queue.
	stat->mappingTime = 0;
	stat->graphTime = 0;
	stat->optimTime = 0;
	stat->schedTime = 0;
	stat->globalEndTime = 0;

	stat->forkTime = 0;
	stat->joinTime = 0;
	stat->rbTime = 0;
	stat->brTime = 0;
	stat->nExecSRDAGActor = 0;
	stat->nSRDAGActor = srdag_->getNVertex();
	stat->nSRDAGEdge = srdag_->getNEdge();
	stat->nPiSDFActor = 0;

	stat->memoryUsed = memAlloc_->getMemUsed();

	TraceMsg* traceMsg;
	int n = Launcher::get()->getNLaunched();
	while(n){
		if(Platform::get()->getSpiderCommunicator()->trace_start_recv((void**)&traceMsg)){
			switch (traceMsg->msgIx) {
				case TRACE_JOB:{
					SRDAGVertex* vertex = srdag_->getVertexFromIx(traceMsg->srdagIx);

					traceMsg->start /= CHIP_FREQ;
					traceMsg->end /= CHIP_FREQ;
					
					Time execTime = traceMsg->end - traceMsg->start;

					static Time baseTime=0;
					// if(strcmp(vertex->getReference()->getName(),"src") == 0){
					// 	baseTime = traceMsg->start;
					// }


					printGrantt_SRDAGVertex(
							ganttFile,
							latexFile,
							archi_,
							vertex,
							traceMsg->start-baseTime,
							traceMsg->end-baseTime,
							traceMsg->lrtIx,
							latexScaling);

					/* Update Stats */
					stat->globalEndTime = std::max(traceMsg->end-baseTime, stat->globalEndTime);
					stat->nExecSRDAGActor++;

					switch(vertex->getType()){
						case SRDAG_NORMAL:{
							int i;
							int lrtType = archi_->getPEType(traceMsg->lrtIx);
							for(i=0; i<stat->nPiSDFActor; i++){
								if(stat->actors[i] == vertex->getReference()){
									stat->actorTimes[i][lrtType] += execTime;
									stat->actorIterations[i][lrtType]++;

									stat->actorFisrt[i] = std::min(stat->actorFisrt[i], traceMsg->start);
									stat->actorLast[i] = std::max(stat->actorLast[i], traceMsg->end);
									break;
								}
							}
							if(i == stat->nPiSDFActor){
								stat->actors[stat->nPiSDFActor] = vertex->getReference();

								memset(stat->actorTimes[stat->nPiSDFActor], 0, MAX_STATS_PE_TYPES*sizeof(Time));
								memset(stat->actorIterations[stat->nPiSDFActor], 0, MAX_STATS_PE_TYPES*sizeof(Time));

								stat->actorTimes[stat->nPiSDFActor][lrtType] += execTime;
								stat->actorIterations[i][lrtType]++;
								stat->nPiSDFActor++;

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
//					break;

					static int i=0;

					traceMsg->start /= CHIP_FREQ;
					traceMsg->end /= CHIP_FREQ;

					/* Gantt File */
					Platform::get()->fprintf(ganttFile, "\t<event\n");
					Platform::get()->fprintf(ganttFile, "\t\tstart=\"%llu\"\n", 	traceMsg->start);
					Platform::get()->fprintf(ganttFile, "\t\tend=\"%llu\"\n",		traceMsg->end);
					Platform::get()->fprintf(ganttFile, "\t\ttitle=\"%s\"\n", 	TimeMonitor::getTaskName((TraceSpiderType)traceMsg->spiderTask));
					Platform::get()->fprintf(ganttFile, "\t\tmapping=\"%s\"\n", archi_->getPEName(traceMsg->lrtIx));
					Platform::get()->fprintf(ganttFile, "\t\tcolor=\"%s\"\n", 	regenerateColor(i++));
					Platform::get()->fprintf(ganttFile, "\t\t>Step_%d.</event>\n", traceMsg->spiderTask);

					stat->schedTime = std::max(traceMsg->end, stat->schedTime);

					switch(traceMsg->spiderTask){
					case TRACE_SPIDER_GRAPH:
						stat->graphTime += traceMsg->end - traceMsg->start;
						break;
					case TRACE_SPIDER_ALLOC:
						throw "Unhandle trace";
						break;
					case TRACE_SPIDER_SCHED:
						stat->mappingTime += traceMsg->end - traceMsg->start;
						break;
					case TRACE_SPIDER_OPTIM:
						stat->optimTime += traceMsg->end - traceMsg->start;
						break;
					}

					/* Latex File */
					Platform::get()->fprintf(latexFile, "%f,", traceMsg->start/latexScaling); /* Start */
					Platform::get()->fprintf(latexFile, "%f,", traceMsg->end/latexScaling); /* Duration */
					Platform::get()->fprintf(latexFile, "%d,", 0); /* Core index */
					Platform::get()->fprintf(latexFile, "colorSched\n",15); /* Color */
					break;}
				default:
					printf("msgIx %d\n",traceMsg->msgIx);
					throw "Unhandled trace msg";
					break;
			}
			Platform::get()->getSpiderCommunicator()->trace_end_recv();
			n--;
		}
	}
	Launcher::get()->rstNLaunched();

	Platform::get()->fprintf(ganttFile, "</data>\n");

	Platform::get()->fclose(ganttFile);
	Platform::get()->fclose(latexFile);

	stat->execTime = stat->globalEndTime - stat->schedTime;
}

PiSDFGraph* Spider::createGraph(
		int nEdges,
		int nParams,
		int nInIfs,
		int nOutIfs,
		int nConfigs,
		int nBodies){
	return CREATE(PISDF_STACK, PiSDFGraph)(
			/*Edges*/    nEdges,
			/*Params*/   nParams,
			/*InputIf*/  nInIfs,
			/*OutputIf*/ nOutIfs,
			/*Config*/   nConfigs,
			/*Body*/     nBodies);
}

PiSDFVertex* Spider::addBodyVertex(
			PiSDFGraph* graph,
			const char* vertexName, int fctId,
			int nInEdge, int nOutEdge,
			int nInParam){
	return graph->addBodyVertex(
			vertexName,
			fctId,
			nInEdge,
			nOutEdge,
			nInParam);
}

PiSDFVertex* Spider::addHierVertex(
		PiSDFGraph* graph,
		const char* vertexName,
		PiSDFGraph* subgraph,
		int nInEdge, int nOutEdge,
		int nInParam){
	return graph->addHierVertex(
			vertexName,
			subgraph,
			nInEdge,
			nOutEdge,
			nInParam);
}

PiSDFVertex* Spider::addSpecialVertex(
		PiSDFGraph* graph,
		PiSDFSubType subType,
		int nInEdge, int nOutEdge,
		int nInParam){
	return graph->addSpecialVertex(
			subType,
			nInEdge,
			nOutEdge,
			nInParam);
}

PiSDFVertex* Spider::addConfigVertex(
		PiSDFGraph* graph,
		const char* vertexName, int fctId,
		PiSDFSubType subType,
		int nInEdge, int nOutEdge,
		int nInParam, int nOutParam){
	return graph->addConfigVertex(
			vertexName,
			fctId,
			subType,
			nInEdge,
			nOutEdge,
			nInParam,
			nOutParam);
}

PiSDFVertex* Spider::addInputIf(
		PiSDFGraph* graph,
		const char* name,
		int nInParam){
	return graph->addInputIf(
			name,
			nInParam);
}

PiSDFVertex* Spider::addOutputIf(
		PiSDFGraph* graph,
		const char* name,
		int nInParam){
	return graph->addOutputIf(
			name,
			nInParam);
}

PiSDFParam* Spider::addStaticParam(
		PiSDFGraph* graph,
		const char* name,
		const char* expr){
	return graph->addStaticParam(
			name,
			expr);
}

PiSDFParam* Spider::addStaticParam(
		PiSDFGraph* graph,
		const char* name,
		int value){
	return graph->addStaticParam(
			name,
			value);
}

PiSDFParam* Spider::addHeritedParam(
		PiSDFGraph* graph,
		const char* name,
		int parentId){
	return graph->addHeritedParam(
			name,
			parentId);
}

PiSDFParam* Spider::addDynamicParam(
		PiSDFGraph* graph,
		const char* name){
	return graph->addDynamicParam(name);
}

PiSDFParam* Spider::addStaticDependentParam(
		PiSDFGraph* graph,
		const char* name,
		const char* expr){
	return graph->addStaticDependentParam(name, expr);
}

PiSDFParam* Spider::addDynamicDependentParam(
		PiSDFGraph* graph,
		const char* name,
		const char* expr){
	return graph->addDynamicDependentParam(name, expr);
}

PiSDFEdge* Spider::connect(
		PiSDFGraph* graph,
		PiSDFVertex* source, int sourcePortId, const char* production,
		PiSDFVertex* sink, int sinkPortId, const char* consumption,
		const char* delay, PiSDFVertex* setter, PiSDFVertex* getter){
	return graph->connect(
			source, sourcePortId, production,
			sink, sinkPortId, consumption,
			delay, setter, getter);
}

void Spider::addInParam(PiSDFVertex* vertex, int ix, PiSDFParam* param){
	vertex->addInParam(ix, param);
}
void Spider::addOutParam(PiSDFVertex* vertex, int ix, PiSDFParam* param){
	vertex->addOutParam(ix, param);
}

void Spider::setTimingOnType(PiSDFVertex* vertex, int peType, const char* timing){
	vertex->setTimingOnType(peType, timing);
}

void Spider::isExecutableOnAllPE(PiSDFVertex* vertex){
	vertex->isExecutableOnAllPE();
}

void Spider::isExecutableOnPE(PiSDFVertex* vertex, int pe){
	vertex->isExecutableOnPE(pe);
}

void Spider::isExecutableOnPEType(PiSDFVertex* vertex, int peType){
	for (int pe = 0; pe < archi_->getNPE(); pe++){
		if (archi_->getPEType(pe) == peType) vertex->isExecutableOnPE(pe);
	}
}

void Spider::cleanPiSDF(){
	PiSDFGraph* graph = pisdf_;
	if(graph != 0){
		graph->~PiSDFGraph();
		StackMonitor::free(PISDF_STACK, graph);
		StackMonitor::freeAll(PISDF_STACK);
	}
}

