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
#ifndef SPIDER_SRDAGLESSSCHEDULER_H
#define SPIDER_SRDAGLESSSCHEDULER_H

#include <vector>
#include <cstdint>
#include <scheduling/PiSDFSchedule.h>
#include "SRDAGLessIR.h"

class PiSDFGraph;

typedef struct VertexDependency {
    PiSDFVertex *vertex_ = nullptr;
    Param cons_ = 0;
    Param prod_ = 0;
    Param delay_ = 0;
    std::int32_t *nScheduled_ = nullptr;
} VertexDependency;

class SRDAGLessScheduler {
public:
    SRDAGLessScheduler(PiSDFGraph *graph, PiSDFSchedule *schedule, SRDAGLessScheduler *parent = nullptr);

    virtual ~SRDAGLessScheduler();

    virtual const PiSDFSchedule *schedule(PiSDFGraph *graph, MemAlloc *memAlloc);

    inline PiSDFSchedule *getSchedule() {
        return schedule_;
    }

    inline void printSchedule(const char *path) {
        if (schedule_) {
            schedule_->print(path);
        }
    }

    void printRhoValues();

protected:
    /** Implementation variables **/
    PiSDFGraph *graph_;
    PiSDFSchedule *schedule_;
    std::int32_t nVertices_;
    std::int32_t nChildren_;
    std::int32_t firstChildIx_;
    SRDAGLessScheduler *parent_;
    SRDAGLessScheduler **children_;
    std::vector<PiSDFVertex *> specialActorsAdded_;

    /** Schedule IR variables  **/
    std::int32_t *rhoValueArray_;
    std::int32_t *instanceAvlCountArray_;
    std::int32_t *instanceSchCountArray_;
    VertexDependency **dependenciesArray_;

    void initiliazeVertexScheduleIR(PiSDFVertex *vertex, std::int32_t rv);

    void initIR(PiSDFGraph *const graph);

    void initiliazeInterfacesIR(SRDAGLessScheduler *vertex);

    void replaceInputIfWithBroadcast(PiSDFGraph *graph);

    void replaceOutputIfWithRoundbuffer(const std::int32_t *brv);

    bool isSchedulable(PiSDFVertex *vertex, std::int32_t nInstances);

    void scheduleSubgraph(PiSDFVertex *vertex, MemAlloc *memAlloc);

    void mapVertex(PiSDFVertex *vertex);

    void map(PiSDFVertex *vertex, MemAlloc *memAlloc);

    void computeRhoValues();

    Time computeMinimumStartTime(PiSDFVertex *vertex) const;
};

#endif //SPIDER_SRDAGLESSSCHEDULER_H
