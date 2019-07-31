/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018 - 2019)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018 - 2019)
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
#include <cinttypes>
#include "spider.h"

#include <graphs/PiSDF/PiSDFCommon.h>
#include <graphs/SRDAG/SRDAGCommon.h>
#include <graphs/SRDAG/SRDAGGraph.h>
#include <graphs/PiSDF/PiSDFGraph.h>
#include <graphs/PiSDF/PiSDFEdge.h>

#include <scheduling/MemAlloc/SpecialActorMemAlloc.h>
#include <scheduling/MemAlloc/DummyMemAlloc.h>
#include <scheduling/Scheduler.h>
#include <scheduling/MemAlloc.h>

#include <scheduling/Scheduler/ListSchedulerOnTheGo.h>
#include <scheduling/Scheduler/RoundRobinScattered.h>
#include <scheduling/Scheduler/ListScheduler.h>
#include <scheduling/Scheduler/RoundRobin.h>

#include <graphTransfo/GraphTransfo.h>

#include <SpiderCommunicator.h>

#include <monitor/TimeMonitor.h>
#include <launcher/Launcher.h>
#include <Logger.h>
#include <scheduling/MemAlloc/DummyPiSDFMemAlloc.h>
#include <scheduling/Scheduler/GreedyScheduler.h>
#include <graphs/Archi/Archi.h>

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
static SRDAGSchedule *schedule_ = nullptr;

static bool verbose_;
static bool useGraphOptim_;
static bool useActorPrecedence_;
static bool traceEnabled_;
static bool papifyFeedbackEnabled_;

static bool containsDynamicParam(PiSDFGraph *const graph) {
    for (int i = 0; i < graph->getNParam(); ++i) {
        auto *param = graph->getParam(i);
        if (param->isDynamic()) {
            return true;
        }
    }
    return false;
}

static bool isGraphStatic(PiSDFGraph *const graph) {
    bool isStatic = !containsDynamicParam(graph);
    for (int j = 0; j < graph->getNBody(); ++j) {
        PiSDFVertex *vertex = graph->getBody(j);
        if (vertex->isHierarchical()) {
            auto *subGraph = vertex->getSubGraph();
            subGraph->setGraphStaticProperty(isGraphStatic(subGraph));
            isStatic &= subGraph->isGraphStatic();
        }
    }
    return isStatic;
}

void Spider::initStacks(SpiderStackConfig &cfg) {
    StackMonitor::initStack(ARCHI_STACK, cfg.archiStack);
    StackMonitor::initStack(PISDF_STACK, cfg.pisdfStack);
    StackMonitor::initStack(SRDAG_STACK, cfg.srdagStack);
    StackMonitor::initStack(TRANSFO_STACK, cfg.transfoStack);
}

void Spider::init(SpiderConfig &cfg, SpiderStackConfig &stackConfig) {
    Logger::initializeLogger();

    setGraphOptim(cfg.useGraphOptim);

    setMemAllocType(cfg.memAllocType, cfg.memAllocStart, cfg.memAllocSize);
    setSchedulerType(cfg.schedulerType);

    setVerbose(cfg.verbose);
    setTraceEnabled(cfg.traceEnabled);

    //TODO: add a switch between the different platform
    platform_ = new PlatformPThread(cfg, stackConfig);

    setPapifyFeedbackEnabled(cfg.feedbackPapifyInfo);

    if (traceEnabled_) {
        Launcher::get()->sendEnableTrace(-1);
    }
//    Logger::enable(LOG_JOB);
}

void Spider::iterate() {
    Platform::get()->rstTime();
    if (pisdf_->isGraphStatic()) {
        if (!srdag_) {
            /* On first iteration, the schedule is created */
            srdag_ = new SRDAGGraph();
            schedule_ = static_scheduler(srdag_, memAlloc_, scheduler_);
        }
        /* Run the schedule */
        schedule_->executeAndRun();
        schedule_->restartSchedule();
    } else {
        delete srdag_;
        StackMonitor::freeAll(SRDAG_STACK);
        memAlloc_->reset();
        srdag_ = new SRDAGGraph();
        jit_ms(pisdf_, archi_, srdag_, memAlloc_, scheduler_);
    }
    /** Process PAPIFY feedback **/
    if(papifyFeedbackEnabled_){
        Platform::get()->processPapifyFeedback();
    }
    /** Wait for LRTs to finish **/
    Platform::get()->rstJobIxRecv();
}


static int getReservedMemoryForGraph(PiSDFGraph *graph, int currentMemReserved) {
    auto *job = CREATE(TRANSFO_STACK, transfoJob);
    memset(job, 0, sizeof(transfoJob));
    job->graph = graph;
    job->paramValues = CREATE_MUL(TRANSFO_STACK, job->graph->getNParam(), Param);
    for (int paramIx = 0; paramIx < job->graph->getNParam(); paramIx++) {
        PiSDFParam *param = job->graph->getParam(paramIx);
        job->paramValues[paramIx] = param->getValue();
    }
    int memReserved = currentMemReserved;
    // Compute the total memory allocation needed for delays in current graph
    for (int i = 0; i < graph->getNEdge(); i++) {
        PiSDFEdge *edge = graph->getEdge(i);
        auto nbDelays = edge->resolveDelay();
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
//        schedule_->~PiSDFSchedule();
        schedule_->~SRDAGSchedule();
        StackMonitor::free(TRANSFO_STACK, schedule_);
    }

    if (archi_) {
        archi_->~Archi();
        StackMonitor::free(ARCHI_STACK, archi_);
    }

    if (pisdf_) {
        pisdf_->~PiSDFGraph();
        StackMonitor::free(PISDF_STACK, pisdf_);
        StackMonitor::freeAll(PISDF_STACK);
    }

    delete srdag_;
    delete memAlloc_;
    delete scheduler_;
    delete platform_;

    /* === Checking stacks state === */

    StackMonitor::freeAll(ARCHI_STACK);
    StackMonitor::freeAll(TRANSFO_STACK);
    StackMonitor::freeAll(SRDAG_STACK);
    StackMonitor::freeAll(PISDF_STACK);

    /* === Cleaning the Stacks === */

    StackMonitor::clean(ARCHI_STACK);
    StackMonitor::clean(TRANSFO_STACK);
    StackMonitor::clean(SRDAG_STACK);
    StackMonitor::clean(PISDF_STACK);
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

void Spider::setPapifyFeedbackEnabled(bool papifyFeedbackEnabled) {
    papifyFeedbackEnabled_ = papifyFeedbackEnabled;
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
    pisdf_->setGraphStaticProperty(isGraphStatic(pisdf_->getBody(0)->getSubGraph()));

    if (pisdf_->isGraphStatic()) {
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
    /** If a memAlloc_ already existed, we delete it**/
    delete memAlloc_;
    switch (type) {
        case MEMALLOC_DUMMY:
            memAlloc_ = new DummyMemAlloc(start, size);
            break;
        case MEMALLOC_SPECIAL_ACTOR:
            memAlloc_ = new SpecialActorMemAlloc(start, size);
            break;
        default:
            throwSpiderException("Unsupported type of Memory Allocation.\n");
    }
}

void Spider::setSchedulerType(SchedulerType type) {
    /** If a scheduler_ already existed, we delete it**/
    delete scheduler_;
    switch (type) {
        case SCHEDULER_LIST:
            scheduler_ = new ListScheduler();
            break;
        case SCHEDULER_GREEDY:
            scheduler_ = new GreedyScheduler();
            break;
        case SCHEDULER_LIST_ON_THE_GO:
            scheduler_ = new ListSchedulerOnTheGo();
            break;
        case SCHEDULER_ROUND_ROBIN:
            scheduler_ = new RoundRobin();
            break;
        case SCHEDULER_ROUND_ROBIN_SCATTERED:
            scheduler_ = new RoundRobinScattered();
            break;
    }
}

void Spider::printSRDAG(const char *srdagPath) {
    return srdag_->print(srdagPath);
}

void Spider::printPiSDF(const char *pisdfPath) {
    pisdf_->getBody(0)->getSubGraph()->print(pisdfPath);
}

void Spider::printActorsStat(ExecutionStat *stat) {
    Platform::get()->fprintf(stdout, "\t%15s:\n", "Actors");
    for (int j = 0; j < stat->nPiSDFActor; j++) {
        Platform::get()->fprintf(stdout, "\t%15s:", stat->actors[j]->getName());
        for (std::uint32_t k = 0; k < archi_->getNPEType(); k++) {
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

static void printGantt_SRDAGVertex(FILE *ganttFile, FILE *latexFile, Archi *archi, SRDAGVertex *vertex,
                                   Time start, Time end, int lrtIx, float latexScaling) {
    static char name[200];
    static int i = 0;
    vertex->toString(name, 100);

    auto *temp_str = (char *) malloc(300 * sizeof(char));


    sprintf(temp_str,
            "\t<event\n"
            "\t\tstart=\"%" PRIu64"\"\n"
            "\t\tend=\"%" PRIu64"\"\n"
            "\t\ttitle=\"%s_%d_%d\"\n"
            "\t\tmapping=\"%s\"\n"
            "\t\tcolor=\"%s\"\n"
            "\t\t>Step_%d.</event>\n",
            start,
            end,
            name, vertex->getIterId(), vertex->getRefId(),
            archi->getPEFromSpiderID(lrtIx)->getName().c_str(),
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

static void writeGanttForVertex(TraceMessage *message, FILE *ganttFile, FILE *latexFile, ExecutionStat *stat) {
    SRDAGVertex *vertex = srdag_->getVertexFromIx(message->getVertexID());

    auto startTimeScaled = message->getStartTime() / CHIP_FREQ;
    auto endTimeScaled = message->getEndTime() / CHIP_FREQ;

    auto execTime = message->getEllapsedTime() / CHIP_FREQ;

    Time baseTime = 0;
    // if(strcmp(vertex->getReference()->getName(),"src") == 0){
    // 	baseTime = traceMsg->start;
    // }


    printGantt_SRDAGVertex(
            ganttFile,
            latexFile,
            archi_,
            vertex,
            startTimeScaled - baseTime,
            endTimeScaled - baseTime,
            message->getLRTID(),
            1000.f);


    /* Update Stats */
    stat->globalEndTime = std::max(endTimeScaled - baseTime, stat->globalEndTime);
    stat->nExecSRDAGActor++;

    switch (vertex->getType()) {
        case SRDAG_NORMAL: {
            int i;
            auto lrtType = archi_->getPEFromSpiderID(message->getLRTID())->getHardwareType();
            auto *pisdfVertexRef = vertex->getReference();
            // Update execution time of the PiSDF actor
            auto timingOnPe = std::to_string(execTime);
            pisdfVertexRef->setTimingOnType(lrtType, timingOnPe.c_str());
            // Update global stats
            for (i = 0; i < stat->nPiSDFActor; i++) {
                if (stat->actors[i] == pisdfVertexRef) {
                    stat->actorTimes[i][lrtType] += execTime;
                    stat->actorIterations[i][lrtType]++;

                    stat->actorFisrt[i] = std::min(stat->actorFisrt[i], startTimeScaled);
                    stat->actorLast[i] = std::max(stat->actorLast[i], endTimeScaled);
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

                stat->actorFisrt[i] = startTimeScaled;
                stat->actorLast[i] = endTimeScaled;
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
}

static void writeGanttForSpiderTasks(TraceMessage *message, FILE *ganttFile, FILE *latexFile, ExecutionStat *stat) {
    int i = 0;

    /** Scale the different values of measured time to chip time **/
    Time startTimeScaled = message->getStartTime() / CHIP_FREQ;
    Time endTimeScaled = message->getEndTime() / CHIP_FREQ;
    Time ellapsedTimeScaled = message->getEllapsedTime() / CHIP_FREQ;

    /* Gantt File */
    Platform::get()->fprintf(ganttFile, "\t<event\n");
    Platform::get()->fprintf(ganttFile, "\t\tstart=\"%" PRIu64"\"\n", startTimeScaled);
    Platform::get()->fprintf(ganttFile, "\t\tend=\"%" PRIu64"\"\n", endTimeScaled);
    Platform::get()->fprintf(ganttFile, "\t\ttitle=\"%s\"\n",
                             TimeMonitor::getTaskName((TraceSpiderType) message->getSpiderTask()));
    Platform::get()->fprintf(ganttFile, "\t\tmapping=\"%s\"\n",
                             archi_->getPEFromSpiderID(message->getLRTID())->getName().c_str());
    Platform::get()->fprintf(ganttFile, "\t\tcolor=\"%s\"\n", regenerateColor(i++));
    Platform::get()->fprintf(ganttFile, "\t\t>Step_%d.</event>\n", message->getSpiderTask());

    stat->schedTime = std::max(endTimeScaled, stat->schedTime);

    switch (message->getSpiderTask()) {
        case TRACE_SPIDER_GRAPH:
            stat->graphTime += ellapsedTimeScaled;
            break;
        case TRACE_SPIDER_SCHED:
            stat->mappingTime += ellapsedTimeScaled;
            break;
        case TRACE_SPIDER_OPTIM:
            stat->optimTime += ellapsedTimeScaled;
            break;
        case TRACE_SPIDER_ALLOC:
        default:
            throwSpiderException("Unhandle type of SpiderTrace: %d.", message->getSpiderTask());
    }

    /* Latex File */
    auto latexScaling = 1000.f;
    Platform::get()->fprintf(latexFile, "%f,", startTimeScaled / latexScaling); /* Start */
    Platform::get()->fprintf(latexFile, "%f,", endTimeScaled / latexScaling); /* Duration */
    Platform::get()->fprintf(latexFile, "%" PRIu32",", archi_->getSpiderGRTID()); /* Core index */
    Platform::get()->fprintf(latexFile, "colorSched\n", 15); /* Color */
}

void Spider::printGantt(const char *ganttPath, const char *latexPath, ExecutionStat *stat) {
    FILE *ganttFile = Platform::get()->fopen(ganttPath);
    if (ganttFile == nullptr) {
        throwSpiderException("Failed to open ganttFile.");
    }

    FILE *latexFile = Platform::get()->fopen(latexPath);
    if (latexFile == nullptr) {
        throwSpiderException("Failed to open latexFile.");
    }

    // Writing header
    Platform::get()->fprintf(ganttFile, "<data>\n");
    Platform::get()->fprintf(latexFile, "start,end,core,color\n");

    // Popping data from Trace queue.
    // TODO: change execution stat to proper class
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

    int n = Launcher::get()->getNLaunched();
    auto *spiderCommunicator = Platform::get()->getSpiderCommunicator();
    while (n) {
        NotificationMessage message;
        spiderCommunicator->pop_notification(Platform::get()->getNLrt(), &message, true);
        if (message.getType() != TRACE_NOTIFICATION) {
            // Push back notification for later
            // We should not have any other kind of notification here though
            spiderCommunicator->push_notification(Platform::get()->getNLrt(), &message);
        } else {
            TraceMessage *msg;
            spiderCommunicator->pop_trace_message(&msg, message.getIndex());
            if (message.getSubType() == TRACE_LRT) {
                writeGanttForVertex(msg, ganttFile, latexFile, stat);
            } else if (message.getSubType() == TRACE_SPIDER) {
                writeGanttForSpiderTasks(msg, ganttFile, latexFile, stat);
            } else {
                throwSpiderException("Unhandled type of Trace: %d", message.getSubType());
            }
            StackMonitor::free(ARCHI_STACK, msg);
        }
        n--;
    }
    Launcher::get()->rstNLaunched();

    Platform::get()->fprintf(ganttFile, "</data>\n");

    Platform::get()->fclose(ganttFile);
    Platform::get()->fclose(latexFile);

    stat->execTime = stat->globalEndTime - stat->schedTime;
}
