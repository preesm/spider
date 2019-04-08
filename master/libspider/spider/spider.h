/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2018)
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
#ifndef SPIDER_H
#define SPIDER_H

#include <map>
#include <vector>
#include <cstdint>

class Archi;

class PiSDFVertex;

class PiSDFParam;

class PiSDFGraph;

class PiSDFEdge;

class SRDAGGraph;

class MemAlloc;

class Scheduler;


typedef enum PiSDFSubType {
    PISDF_SUBTYPE_NORMAL,
    PISDF_SUBTYPE_BROADCAST,
    PISDF_SUBTYPE_ROUNDBUFFER,
    PISDF_SUBTYPE_FORK,
    PISDF_SUBTYPE_JOIN,
    PISDF_SUBTYPE_END,
    PISDF_SUBTYPE_DELAY,
    PISDF_SUBTYPE_INPUT_IF,
    PISDF_SUBTYPE_OUTPUT_IF
} PiSDFSubType;

#define MAX_STATS_VERTICES 1000
#define MAX_STATS_PE_TYPES 3

using Time = std::uint64_t;
using Param = std::int64_t;

using lrtFct = void (*)(void **, void **, Param *, Param *);

typedef enum {
    MEMALLOC_DUMMY,
    MEMALLOC_SPECIAL_ACTOR
} MemAllocType;

typedef enum {
    SCHEDULER_GREEDY,
    SCHEDULER_LIST,
    SCHEDULER_LIST_ON_THE_GO,
    SCHEDULER_ROUND_ROBIN,
    SCHEDULER_ROUND_ROBIN_SCATTERED
} SchedulerType;

typedef enum {
    STACK_STATIC,
    STACK_DYNAMIC
} StackType;

typedef struct {
    StackType type;
    const char *name;

    void *start;
    int size;
} StackConfig;

typedef struct {
    int nLrt;
    int shMemSize;
    lrtFct *fcts;
    int nLrtFcts;
    int **coreAffinities;
    int *pesPerPeType;
    int nPeType;
} PlatformConfig;

typedef struct {
    const char *peType_;
    long long peID_;
    const char *actorName_;
    int eventSize_;
    std::vector<const char *> monitoredEvents_;
    bool isTiming_;
    int eventSetID_;
} PapifyConfig;

typedef struct {
    MemAllocType memAllocType;
    int memAllocStart;
    int memAllocSize;

    SchedulerType schedulerType;

    StackConfig archiStack;
    StackConfig pisdfStack;
    StackConfig srdagStack;
    StackConfig transfoStack;
    StackConfig lrtStack;

    bool useGraphOptim;
    bool verbose;
    bool traceEnabled;
    bool usePapify;

    std::map<lrtFct, PapifyConfig *> papifyJobInfo;

    PlatformConfig platform;
} SpiderConfig;

typedef struct {
    Time globalEndTime;

    Time execTime;
    Time schedTime;

    Time mappingTime;
    Time optimTime;
    Time graphTime;

    Time actorTimes[MAX_STATS_VERTICES][MAX_STATS_PE_TYPES];
    Time actorFisrt[MAX_STATS_VERTICES];
    Time actorLast[MAX_STATS_VERTICES];
    Time actorIterations[MAX_STATS_VERTICES][MAX_STATS_PE_TYPES];

    PiSDFVertex *actors[MAX_STATS_VERTICES];
    int nPiSDFActor;

    int nSRDAGActor;
    int nSRDAGEdge;
    int nExecSRDAGActor;

    Time forkTime;
    Time joinTime;
    Time rbTime;
    Time brTime;

    int memoryUsed;

    Time latencies;
} ExecutionStat;

namespace Spider {
    void init(SpiderConfig &cfg);

    void initReservedMemory();

    void clean();

    void setMemAllocType(MemAllocType type, int start, int size);

    void setSchedulerType(SchedulerType type);

    void setVerbose(bool verbose);

    void setGraphOptim(bool useGraphOptim);

    void setActorPrecedence(bool useActorPrecedence);

    void setTraceEnabled(bool traceEnabled);

    bool getVerbose();

    bool getGraphOptim();

    bool getActorPrecedence();

    bool getTraceEnabled();

    void setArchi(Archi *archi);

    void setGraph(PiSDFGraph *pisdf);

    void iterate();

    void printGantt(
            const char *ganttPath,
            const char *latexPath,
            ExecutionStat *stat);

    void printSRDAG(const char *srdagPath);

    void printPiSDF(const char *pisdfPath);

    void printActorsStat(ExecutionStat *stat);

    PiSDFGraph *getGraph();

    Archi *getArchi();

    /* PiSDF Graph Generation */
    PiSDFGraph *createGraph(
            int nEdges,
            int nParams,
            int nInIfs,
            int nOutIfs,
            int nConfigs,
            int nBodies);

    PiSDFVertex *addBodyVertex(
            PiSDFGraph *graph,
            const char *vertexName, int fctId,
            int nInEdge, int nOutEdge,
            int nInParam);

    PiSDFVertex *addSpecialVertex(
            PiSDFGraph *graph,
            const char *vertexName,
            PiSDFSubType subType,
            int nInEdge, int nOutEdge,
            int nInParam);

    PiSDFVertex *addConfigVertex(
            PiSDFGraph *graph,
            const char *vertexName, int fctId,
            PiSDFSubType subType,
            int nInEdge, int nOutEdge,
            int nInParam, int nOutParam);

    PiSDFVertex *addInputIf(
            PiSDFGraph *graph,
            const char *name,
            int nInParam);

    PiSDFVertex *addOutputIf(
            PiSDFGraph *graph,
            const char *name,
            int nInParam);

    void addSubGraph(PiSDFVertex *hierVertex, PiSDFGraph *subgraph);

    PiSDFParam *addStaticParam(PiSDFGraph *graph,
                               const char *name,
                               Param value);

    PiSDFParam *addStaticDependentParam(PiSDFGraph *graph,
                                        const char *name,
                                        const char *expr,
                                        std::initializer_list<PiSDFParam *> dependencies);

    PiSDFParam *addInheritedParam(PiSDFGraph *graph,
                                  const char *name,
                                  int parentId);

    PiSDFParam *addDynamicParam(PiSDFGraph *graph,
                                const char *name);

    PiSDFParam *addDynamicDependentParam(PiSDFGraph *graph,
                                         const char *name,
                                         const char *expr,
                                         std::initializer_list<PiSDFParam *> dependencies);


    PiSDFEdge *connect(
            PiSDFGraph *graph,
            PiSDFVertex *source, int sourcePortId, const char *production,
            PiSDFVertex *sink, int sinkPortId, const char *consumption,
            const char *delay,
            PiSDFVertex *setter = nullptr,
            PiSDFVertex *getter = nullptr,
            PiSDFVertex *delayActor = nullptr,
            bool isDelayPersistent = false);

    void addInParam(PiSDFVertex *vertex, int ix, PiSDFParam *param);

    void addOutParam(PiSDFVertex *vertex, int ix, PiSDFParam *param);

    void setTimingOnType(PiSDFVertex *vertex, int peType, const char *timing);

    void isExecutableOnAllPE(PiSDFVertex *vertex);

    void isExecutableOnPE(PiSDFVertex *vertex, int pe);

    void isExecutableOnPEType(PiSDFVertex *vertex, int peType);

    void cleanPiSDF();
};

#endif//SPIDER_H
