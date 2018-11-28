/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2018)
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
#include <graphs/PiSDF/PiSDFEdge.h>
#include <graphs/SRDAG/SRDAGGraph.h>

#include <scheduling/MemAlloc.h>
#include <scheduling/MemAlloc/DummyMemAlloc.h>
#include <scheduling/MemAlloc/SpecialActorMemAlloc.h>
#include <scheduling/Scheduler.h>

#include <scheduling/Scheduler/ListScheduler.h>
#include <scheduling/Scheduler/ListSchedulerOnTheGo.h>
#include <scheduling/Scheduler/RoundRobin.h>
#include <scheduling/Scheduler/RoundRobinScattered.h>

#include <graphTransfo/GraphTransfo.h>

#include <monitor/TimeMonitor.h>

#include <SpiderCommunicator.h>

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

static Archi *archi_ = nullptr;
static PiSDFGraph *pisdf_ = nullptr;
static SRDAGGraph *srdag_ = nullptr;

static MemAlloc *memAlloc_ = nullptr;
static Scheduler *scheduler_ = nullptr;
//static PlatformMPPA* platform_;
static PlatformPThread *platform_ = nullptr;
static Schedule *schedule_ = nullptr;

static bool verbose_;
static bool useGraphOptim_;
static bool useActorPrecedence_;
static bool traceEnabled_;
static bool isStatic_;

static bool isGraphStatic(PiSDFGraph *const graph) {
    for (int i = 0; i < graph->getNParam(); ++i) {
        auto param = graph->getParam(i);
        switch (param->getType()) {
            case PISDF_PARAM_DYNAMIC:
                return false;
            default:
                break;
        }
    }
    for (int j = 0; j < graph->getNBody(); ++j) {
        PiSDFVertex *vertex = graph->getBody(j);
        if (vertex->isHierarchical()) {
            auto isChildStatic = isGraphStatic(vertex->getSubGraph());
            if (!isChildStatic) {
                return false;
            }
        }
    }
    return true;
}

void Spider::init(SpiderConfig &cfg) {

    setGraphOptim(cfg.useGraphOptim);

    setMemAllocType(cfg.memAllocType, (long) cfg.memAllocStart, cfg.memAllocSize);
    setSchedulerType(cfg.schedulerType);

    setVerbose(cfg.verbose);
    setTraceEnabled(cfg.traceEnabled);

    platform_ = new PlatformPThread(cfg);
}

void Spider::iterate() {
    /** Set all slave jobIx to 0 */

    if (isStatic_) {
        Time start = Platform::get()->getTime();
        if (!srdag_) {
//            Platform::get()->rstTime();
            srdag_ = new SRDAGGraph();
            schedule_ = static_scheduler(srdag_, memAlloc_, scheduler_);
//            Platform::get()->rstJobIxSend();
            Platform::get()->rstJobIxRecv();
        } else {
            schedule_->execute();
            Platform::get()->rstJobIxRecv();
        }
        Time end = Platform::get()->getTime();
        fprintf(stderr, "Execution Time: %lf\n", (end - start) / 1000000.0);
    } else {
        Platform::get()->rstTime();
        Time start = Platform::get()->getTime();
        delete srdag_;
        StackMonitor::freeAll(SRDAG_STACK);
        memAlloc_->reset();
        srdag_ = new SRDAGGraph();
        jit_ms(pisdf_, archi_, srdag_, memAlloc_, scheduler_);
        Time end = Platform::get()->getTime();
        fprintf(stderr, "Execution Time: %lf\n", (end - start) / 1000000.0);
        //Mise à zéro compteur job
        Platform::get()->rstJobIx();
    }
}


static int getReservedMemoryForGraph(PiSDFGraph *graph, int currentMemReserved) {
    transfoJob *job = CREATE(TRANSFO_STACK, transfoJob);
    memset(job, 0, sizeof(transfoJob));
    job->graph = graph;
    job->paramValues = CREATE_MUL(TRANSFO_STACK, job->graph->getNParam(), int);
    for (int paramIx = 0; paramIx < job->graph->getNParam(); paramIx++) {
        PiSDFParam *param = job->graph->getParam(paramIx);
        switch (param->getType()) {
            case PISDF_PARAM_STATIC:
                job->paramValues[paramIx] = param->getStaticValue();
                break;
            case PISDF_PARAM_HERITED:
                job->paramValues[paramIx] = graph->getParentVertex()->getInParam(param->getParentId())->getValue();
                break;
            case PISDF_PARAM_DYNAMIC:
                // Do nothing, cannot be evaluated yet
                job->paramValues[paramIx] = -1;
                break;
            case PISDF_PARAM_DEPENDENT_STATIC:
                job->paramValues[paramIx] = param->getExpression()->evaluate(job->graph->getParams(), job);
                break;
            case PISDF_PARAM_DEPENDENT_DYNAMIC:
                job->paramValues[paramIx] = -1;
                break;
        }
    }
    int memReserved = currentMemReserved;
    // Compute the total memory allocation needed for delays in current graph
    for (int i = 0; i < graph->getNEdge(); i++) {
        PiSDFEdge *edge = graph->getEdge(i);
        int nbDelays = edge->resolveDelay(job);
        if (nbDelays > 0 && edge->isDelayPersistent()) {
            // Compute memory offset
            int memAllocAddr = memAlloc_->getMemUsed();
            edge->setMemoryDelayAlloc(memAllocAddr);
            // Get reserved aligned size
            memReserved += memAlloc_->getReservedAlloc(nbDelays);
        }
    }
    StackMonitor::free(TRANSFO_STACK, job->paramValues);
    StackMonitor::free(TRANSFO_STACK, job);
    // Compute the total memory allocation needed for delays in subgraph
    for (int j = 0; j < graph->getNBody(); ++j) {
        PiSDFVertex *vertex = graph->getBody(j);
        if (vertex->isHierarchical()) {
            memReserved += getReservedMemoryForGraph(vertex->getSubGraph(), currentMemReserved);
        }
    }
    return memReserved;
}

void Spider::initReservedMemory() {
    PiSDFVertex *root = pisdf_->getBody(0);
    PiSDFGraph *graph = root->getSubGraph();
    // Compute the needed reserved memory for delays
    memAlloc_->reset();
    int memReserved = memAlloc_->getReservedAlloc(1);
    // Recursively go through the hierarchy
    memReserved += getReservedMemoryForGraph(graph, 0);
    fprintf(stderr, "INFO: Reserved ");
    if (memReserved < 1024) {
        fprintf(stderr, "%5.1f B", memReserved * 1.);
    } else if (memReserved < 1024 * 1024) {
        fprintf(stderr, "%5.1f KB", memReserved / 1024.);
    } else if (memReserved < 1024 * 1024 * 1024) {
        fprintf(stderr, "%5.1f MB", memReserved / (1024. * 1024.));
    } else {
        fprintf(stderr, "%5.1f GB", memReserved / (1024. * 1024. * 1024.));
    }
    fprintf(stderr, "(%#x) / ", memReserved);
    memAlloc_->printMemAllocSizeFormatted();
    fprintf(stderr, " of the shared memory\n");
    fprintf(stderr, "for delays.\n");

    memAlloc_->setReservedSize(memReserved);
    memAlloc_->reset();
}

void Spider::clean() {

    if (schedule_) {
        schedule_->~Schedule();
        StackMonitor::free(TRANSFO_STACK, schedule_);
        StackMonitor::freeAll(TRANSFO_STACK);
    }

    delete srdag_;
    delete memAlloc_;
    delete scheduler_;
    delete platform_;
    //StackMonitor::cleanAllStack();
}


void Spider::setGraphOptim(bool useGraphOptim) {
    useGraphOptim_ = useGraphOptim;
}

void Spider::setVerbose(bool verbose) {
    verbose_ = verbose;
}

void Spider::setActorPrecedence(bool useActorPrecedence) {
    useActorPrecedence_ = useActorPrecedence;
}

void Spider::setTraceEnabled(bool traceEnabled) {
    traceEnabled_ = traceEnabled;
}

bool Spider::getVerbose() {
    return verbose_;
}

bool Spider::getGraphOptim() {
    return useGraphOptim_;
}

bool Spider::getActorPrecedence() {
    return useActorPrecedence_;
}

bool Spider::getTraceEnabled() {
    return traceEnabled_;
}

void Spider::setArchi(Archi *archi) {
    archi_ = archi;
}

void Spider::setGraph(PiSDFGraph *graph) {
    pisdf_ = graph;

    // Detect the static property of the graph
    isStatic_ = isGraphStatic(pisdf_);
    if (isStatic_) {
        Platform::get()->fprintf(stderr, "Graph [%s] is static.\n", pisdf_->getBody(0)->getName());
    } else {
        Platform::get()->fprintf(stderr, "Graph [%s] is not fully static.\n", pisdf_->getBody(0)->getName());
    }
}

PiSDFGraph *Spider::getGraph() {
    return pisdf_;
}

Archi *Spider::getArchi() {
    return archi_;
}

void Spider::setMemAllocType(MemAllocType type, int start, int size) {
    if (memAlloc_ != 0) {
        delete memAlloc_;
    }
    switch (type) {
        case MEMALLOC_DUMMY:
            memAlloc_ = new DummyMemAlloc(start, size);
            break;
        case MEMALLOC_SPECIAL_ACTOR:
            memAlloc_ = new SpecialActorMemAlloc(start, size);
            break;
    }
}

void Spider::setSchedulerType(SchedulerType type) {
    if (scheduler_ != 0) {
        delete scheduler_;
    }
    switch (type) {
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

void Spider::printSRDAG(const char *srdagPath) {
    return srdag_->print(srdagPath);
}

void Spider::printPiSDF(const char *pisdfPath) {
    return pisdf_->print(pisdfPath);
}

void Spider::printActorsStat(ExecutionStat *stat) {
    Platform::get()->fprintf(stdout, "\t%15s:\n", "Actors");
    for (int j = 0; j < stat->nPiSDFActor; j++) {
        Platform::get()->fprintf(stdout, "\t%15s:", stat->actors[j]->getName());
        for (int k = 0; k < archi_->getNPETypes(); k++) {
            if (stat->actorIterations[j][k]) {
                Platform::get()->fprintf(stdout, "\t%lld (x%lld)",
                                         stat->actorTimes[j][k] / stat->actorIterations[j][k],
                                         stat->actorIterations[j][k]);
            } else {
                Platform::get()->fprintf(stdout, "\t%d (x%d)", 0, 0);
            }
        }
        Platform::get()->fprintf(stdout, "\n");
    }
}

static char *regenerateColor(int refInd) {
    static char color[8];
    color[0] = '\0';

    int ired = (refInd & 0x3) * 50 + 100;
    int igreen = ((refInd >> 2) & 0x3) * 50 + 100;
    int iblue = ((refInd >> 4) & 0x3) * 50 + 100;
    char red[5];
    char green[5];
    char blue[5];
    if (ired <= 0xf) {
        sprintf(red, "0%x", ired);
    } else {
        sprintf(red, "%x", ired);
    }

    if (igreen <= 0xf) {
        sprintf(green, "0%x", igreen);
    } else {
        sprintf(green, "%x", igreen);
    }

    if (iblue <= 0xf) {
        sprintf(blue, "0%x", iblue);
    } else {
        sprintf(blue, "%x", iblue);
    }

    strcpy(color, "#");
    strcat(color, red);
    strcat(color, green);
    strcat(color, blue);

    return color;
}

static inline void printGantt_SRDAGVertex(FILE *ganttFile, FILE *latexFile, Archi *archi, SRDAGVertex *vertex,
                                          Time start, Time end, int lrtIx, float latexScaling) {
    static char name[200];
    static int i = 0;
    vertex->toString(name, 100);

    char *temp_str = (char *) malloc(300 * sizeof(char));


    sprintf(temp_str,
            "\t<event\n"
            "\t\tstart=\"%lu\"\n"
            "\t\tend=\"%lu\"\n"
            "\t\ttitle=\"%s_%d_%d\"\n"
            "\t\tmapping=\"%s\"\n"
            "\t\tcolor=\"%s\"\n"
            "\t\t>Step_%d.</event>\n",
            start,
            end,
            name, vertex->getIterId(), vertex->getRefId(),
            archi->getPEName(lrtIx),
            regenerateColor(i++),
            lrtIx);

    Platform::get()->fprintf(ganttFile, "%s", temp_str);

    sprintf(temp_str,
            "%f,"
            "%f,"
            "%d,",
            start / latexScaling,
            end / latexScaling,
            lrtIx);

    if (vertex->getFctId() == 7) {
        sprintf(temp_str + strlen(temp_str), "color%d\n", vertex->getIterId());
    } else {
        sprintf(temp_str + strlen(temp_str), "c\n");
    }

    Platform::get()->fprintf(latexFile, "%s", temp_str);

    /* Latex File */
    // Platform::get()->fprintf(latexFile, "%f,", start/latexScaling); /* Start */
    // Platform::get()->fprintf(latexFile, "%f,", end/latexScaling); /* Duration */
    // Platform::get()->fprintf(latexFile, "%d,", lrtIx); /* Core index */

    // if(vertex->getFctId() == 7){
    // 	Platform::get()->fprintf(latexFile, "color%d\n", vertex->getIterId()); /* Color */
    // }else Platform::get()->fprintf(latexFile, "c\n"); /* Color */

    free(temp_str);
}

void Spider::printGantt(const char *ganttPath, const char *latexPath, ExecutionStat *stat) {
    FILE *ganttFile = Platform::get()->fopen(ganttPath);
    if (ganttFile == nullptr) throw std::runtime_error("Error opening ganttFile");

    FILE *latexFile = Platform::get()->fopen(latexPath);
    if (latexFile == nullptr) throw std::runtime_error("Error opening latexFile");

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

    TraceMessage *traceMsg;
    int n = Launcher::get()->getNLaunched();
    while (n) {
        Platform::get()->getSpiderCommunicator()->trace_start_recv_block((void **) &traceMsg);
        switch (traceMsg->id_) {
            case TRACE_JOB: {
                SRDAGVertex *vertex = srdag_->getVertexFromIx(traceMsg->srdagID_);

                traceMsg->start_ /= CHIP_FREQ;
                traceMsg->end_ /= CHIP_FREQ;

                Time execTime = traceMsg->end_ - traceMsg->start_;

                static Time baseTime = 0;
                // if(strcmp(vertex->getReference()->getName(),"src") == 0){
                // 	baseTime = traceMsg->start;
                // }


                printGantt_SRDAGVertex(
                        ganttFile,
                        latexFile,
                        archi_,
                        vertex,
                        traceMsg->start_ - baseTime,
                        traceMsg->end_ - baseTime,
                        traceMsg->lrtID_,
                        latexScaling);

                /* Update Stats */
                stat->globalEndTime = std::max(traceMsg->end_ - baseTime, stat->globalEndTime);
                stat->nExecSRDAGActor++;

                switch (vertex->getType()) {
                    case SRDAG_NORMAL: {
                        int i;
                        int lrtType = archi_->getPEType(traceMsg->lrtID_);
                        for (i = 0; i < stat->nPiSDFActor; i++) {
                            if (stat->actors[i] == vertex->getReference()) {
                                stat->actorTimes[i][lrtType] += execTime;
                                stat->actorIterations[i][lrtType]++;

                                stat->actorFisrt[i] = std::min(stat->actorFisrt[i], traceMsg->start_);
                                stat->actorLast[i] = std::max(stat->actorLast[i], traceMsg->end_);
                                break;
                            }
                        }
                        if (i == stat->nPiSDFActor) {
                            stat->actors[stat->nPiSDFActor] = vertex->getReference();

                            memset(stat->actorTimes[stat->nPiSDFActor], 0, MAX_STATS_PE_TYPES * sizeof(Time));
                            memset(stat->actorIterations[stat->nPiSDFActor], 0, MAX_STATS_PE_TYPES * sizeof(Time));

                            stat->actorTimes[stat->nPiSDFActor][lrtType] += execTime;
                            stat->actorIterations[i][lrtType]++;
                            stat->nPiSDFActor++;

                            stat->actorFisrt[i] = traceMsg->start_;
                            stat->actorLast[i] = traceMsg->end_;
                        }
                        break;
                    }
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

                break;
            }
            case TRACE_SPIDER: {

                static int i = 0;

                traceMsg->start_ /= CHIP_FREQ;
                traceMsg->end_ /= CHIP_FREQ;

                /* Gantt File */
                Platform::get()->fprintf(ganttFile, "\t<event\n");
                Platform::get()->fprintf(ganttFile, "\t\tstart=\"%llu\"\n", traceMsg->start_);
                Platform::get()->fprintf(ganttFile, "\t\tend=\"%llu\"\n", traceMsg->end_);
                Platform::get()->fprintf(ganttFile, "\t\ttitle=\"%s\"\n",
                                         TimeMonitor::getTaskName((TraceSpiderType) traceMsg->spiderTask_));
                Platform::get()->fprintf(ganttFile, "\t\tmapping=\"%s\"\n", archi_->getPEName(traceMsg->lrtID_));
                Platform::get()->fprintf(ganttFile, "\t\tcolor=\"%s\"\n", regenerateColor(i++));
                Platform::get()->fprintf(ganttFile, "\t\t>Step_%lu.</event>\n", traceMsg->spiderTask_);

                stat->schedTime = std::max(traceMsg->end_, stat->schedTime);

                switch (traceMsg->spiderTask_) {
                    case TRACE_SPIDER_GRAPH:
                        stat->graphTime += traceMsg->end_ - traceMsg->start_;
                        break;
                    case TRACE_SPIDER_SCHED:
                        stat->mappingTime += traceMsg->end_ - traceMsg->start_;
                        break;
                    case TRACE_SPIDER_OPTIM:
                        stat->optimTime += traceMsg->end_ - traceMsg->start_;
                        break;
                    case TRACE_SPIDER_ALLOC:
                    default:
                        throw std::runtime_error("Unhandle trace");
                }

                /* Latex File */
                Platform::get()->fprintf(latexFile, "%f,", traceMsg->start_ / latexScaling); /* Start */
                Platform::get()->fprintf(latexFile, "%f,", traceMsg->end_ / latexScaling); /* Duration */
                Platform::get()->fprintf(latexFile, "%d,", 0); /* Core index */
                Platform::get()->fprintf(latexFile, "colorSched\n", 15); /* Color */
                break;
            }
            default:
                printf("msgIx %u\n", traceMsg->id_);
                throw std::runtime_error("Unhandled trace msg");
        }
        Platform::get()->getSpiderCommunicator()->trace_end_recv();
        n--;
    }
    Launcher::get()->rstNLaunched();

    Platform::get()->fprintf(ganttFile, "</data>\n");

    Platform::get()->fclose(ganttFile);
    Platform::get()->fclose(latexFile);

    stat->execTime = stat->globalEndTime - stat->schedTime;
}

PiSDFGraph *Spider::createGraph(
        int nEdges,
        int nParams,
        int nInIfs,
        int nOutIfs,
        int nConfigs,
        int nBodies) {
    return CREATE(PISDF_STACK, PiSDFGraph)(
            /*Edges*/    nEdges,
            /*Params*/   nParams,
            /*InputIf*/  nInIfs,
            /*OutputIf*/ nOutIfs,
            /*Config*/   nConfigs,
            /*Body*/     nBodies);
}

PiSDFVertex *Spider::addBodyVertex(
        PiSDFGraph *graph,
        const char *vertexName, int fctId,
        int nInEdge, int nOutEdge,
        int nInParam) {
    return graph->addBodyVertex(
            vertexName,
            fctId,
            nInEdge,
            nOutEdge,
            nInParam);
}

PiSDFVertex *Spider::addHierVertex(
        PiSDFGraph *graph,
        const char *vertexName,
        PiSDFGraph *subgraph,
        int nInEdge, int nOutEdge,
        int nInParam) {
    return graph->addHierVertex(
            vertexName,
            subgraph,
            nInEdge,
            nOutEdge,
            nInParam);
}

PiSDFVertex *Spider::addSpecialVertex(
        PiSDFGraph *graph,
        PiSDFSubType subType,
        int nInEdge, int nOutEdge,
        int nInParam) {
    return graph->addSpecialVertex(
            subType,
            nInEdge,
            nOutEdge,
            nInParam);
}

PiSDFVertex *Spider::addConfigVertex(
        PiSDFGraph *graph,
        const char *vertexName, int fctId,
        PiSDFSubType subType,
        int nInEdge, int nOutEdge,
        int nInParam, int nOutParam) {
    return graph->addConfigVertex(
            vertexName,
            fctId,
            subType,
            nInEdge,
            nOutEdge,
            nInParam,
            nOutParam);
}

PiSDFVertex *Spider::addInputIf(
        PiSDFGraph *graph,
        const char *name,
        int nInParam) {
    return graph->addInputIf(
            name,
            nInParam);
}

PiSDFVertex *Spider::addOutputIf(
        PiSDFGraph *graph,
        const char *name,
        int nInParam) {
    return graph->addOutputIf(
            name,
            nInParam);
}

PiSDFParam *Spider::addStaticParam(
        PiSDFGraph *graph,
        const char *name,
        const char *expr) {
    return graph->addStaticParam(
            name,
            expr);
}

PiSDFParam *Spider::addStaticParam(
        PiSDFGraph *graph,
        const char *name,
        Param value) {
    return graph->addStaticParam(
            name,
            value);
}

PiSDFParam *Spider::addHeritedParam(
        PiSDFGraph *graph,
        const char *name,
        int parentId) {
    return graph->addHeritedParam(
            name,
            parentId);
}

PiSDFParam *Spider::addDynamicParam(
        PiSDFGraph *graph,
        const char *name) {
    return graph->addDynamicParam(name);
}

PiSDFParam *Spider::addStaticDependentParam(
        PiSDFGraph *graph,
        const char *name,
        const char *expr) {
    return graph->addStaticDependentParam(name, expr);
}

PiSDFParam *Spider::addDynamicDependentParam(
        PiSDFGraph *graph,
        const char *name,
        const char *expr) {
    return graph->addDynamicDependentParam(name, expr);
}

PiSDFEdge *Spider::connect(
        PiSDFGraph *graph,
        PiSDFVertex *source, int sourcePortId, const char *production,
        PiSDFVertex *sink, int sinkPortId, const char *consumption,
        const char *delay, PiSDFVertex *setter, PiSDFVertex *getter, PiSDFVertex *delayActor, bool isDelayPersistent) {
    return graph->connect(
            source, sourcePortId, production,
            sink, sinkPortId, consumption,
            delay, setter, getter, delayActor, isDelayPersistent);
}

void Spider::addInParam(PiSDFVertex *vertex, int ix, PiSDFParam *param) {
    vertex->addInParam(ix, param);
}

void Spider::addOutParam(PiSDFVertex *vertex, int ix, PiSDFParam *param) {
    vertex->addOutParam(ix, param);
}

void Spider::setTimingOnType(PiSDFVertex *vertex, int peType, const char *timing) {
    vertex->setTimingOnType(peType, timing);
}

void Spider::isExecutableOnAllPE(PiSDFVertex *vertex) {
    vertex->isExecutableOnAllPE();
}

void Spider::isExecutableOnPE(PiSDFVertex *vertex, int pe) {
    vertex->isExecutableOnPE(pe);
}

void Spider::isExecutableOnPEType(PiSDFVertex *vertex, int peType) {
    for (int pe = 0; pe < archi_->getNPE(); pe++) {
        if (archi_->getPEType(pe) == peType) vertex->isExecutableOnPE(pe);
    }
}

void Spider::cleanPiSDF() {
    PiSDFGraph *graph = pisdf_;
    if (graph != 0) {
        graph->~PiSDFGraph();
        StackMonitor::free(PISDF_STACK, graph);
        StackMonitor::freeAll(PISDF_STACK);
    }
}

