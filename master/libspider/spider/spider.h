/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018 - 2019)
 * Daniel Madroñal <daniel.madronal@upm.es> (2019)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018 - 2019)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2018)
 * rlazcano <raquel.lazcano@upm.es> (2019)
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
#include <spider-api/user/archi.h>
#include <spider-api/user/graph.h>

#ifdef APOLLO_AVAILABLE

#include <apolloAPI.h>

#endif

#define MAX_STATS_VERTICES 1000
#define MAX_STATS_PE_TYPES 3

/* === Forward declaration === */

class MemAlloc;
class Scheduler;

/* === Type(s) === */

using Time = std::uint64_t;
using Param = std::int64_t;

/* === Enumeration(s) === */

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

enum class StackType : std::uint8_t {
    STATIC,  /*!< Static stack, size is fixed on init */
    DYNAMIC  /*!< Dynamic stack, size is limited by hardware available resources */
};

/* === Routine(s) === */

using lrtFct = void (*)(void **, void **, Param *, Param *);

/* === Structure(s) === */

typedef struct StackInfo {
    StackType type;
    const char *name;
    void *start;
    std::uint32_t size;
} StackInfo;

typedef struct SpiderStackConfig {
    StackInfo archiStack;
    StackInfo pisdfStack;
    StackInfo srdagStack;
    StackInfo transfoStack;
    StackInfo lrtStack;
} SpiderStackConfig;

typedef struct {
    const char *peType_;
    const char *peID_;
    const char *actorName_;
    int eventSize_;
    std::vector<const char *> monitoredEvents_;
    bool isTiming_;
    int eventSetID_;
} PapifyConfig;

typedef struct SpiderConfig {
    MemAllocType memAllocType;
    int memAllocStart;
    int memAllocSize;

    SchedulerType schedulerType;

    bool useGraphOptim;
    bool verbose;
    bool traceEnabled;
    bool usePapify;
    bool dumpPapifyInfo;
    bool feedbackPapifyInfo;
    bool apolloEnabled;
    bool apolloCompiled;

    std::map<lrtFct, std::map<const char *, PapifyConfig*>> papifyJobInfo;
    std::map<lrtFct, std::map<const char *, std::map<int, double>>> energyModelsInfo;
    bool energyAwareness;
    double performanceObjective;

    lrtFct *fcts;
    int nLrtFcts;
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
    void initStacks(SpiderStackConfig &cfg);

    void init(SpiderConfig &cfg, SpiderStackConfig &stackConfig);

    void initReservedMemory();

    void clean();

    void setMemAllocType(MemAllocType type, int start, int size);

    void setSchedulerType(SchedulerType type);

    void setVerbose(bool verbose);

    void setGraphOptim(bool useGraphOptim);

    void setActorPrecedence(bool useActorPrecedence);

    void setTraceEnabled(bool traceEnabled);

    void setPapifyFeedbackEnabled(bool papifyFeedbackEnabled);

    void setEnergyAwareness(bool energyAwareness);

    void setPerformanceObjective(double performanceObjective);

    void setApolloEnabled(bool apolloEnabled);

    void setApolloCompiled(bool apollloCompiled);

    bool getVerbose();

    bool getGraphOptim();

    bool getActorPrecedence();

    bool getTraceEnabled();

    bool getApolloEnabled();

    bool getApolloCompiled();

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

    bool getEnergyAwareness();

    double getPerformanceObjective();
}

#endif//SPIDER_H
