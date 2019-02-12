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

#include "SRDAGLessScheduler.h"
#include <graphs/PiSDF/PiSDFEdge.h>
#include <graphs/PiSDF/PiSDFVertex.h>
#include <graphs/PiSDF/PiSDFGraph.h>
#include <cmath>

#define SCHEDULE_SIZE 10000

SRDAGLessScheduler::SRDAGLessScheduler(PiSDFGraph *graph, const std::int32_t *brv) {
    graph_ = graph;
    nVertices_ = graph->getNBody();
    archi_ = Spider::getArchi();
    schedule_ = CREATE(TRANSFO_STACK, Schedule)(archi_->getNPE(), SCHEDULE_SIZE);

    /** 0. Creates the array of ScheduleVertex **/
    scheduleVertexArray_ = CREATE_MUL(TRANSFO_STACK, nVertices_, ScheduleVertex);
    /** 1. Initialize properties **/
    for (int i = 0; i < nVertices_; ++i) {
        auto *vertex = graph->getBody(i);
        auto &scheduleVertex = scheduleVertexArray_[i];
        scheduleVertex.vertex_ = vertex;            // Pointer to the original PiSDFVertex
        scheduleVertex.vertexCount_ = brv[i];       // Number of instance to schedule
        scheduleVertex.vertexRhoValue_ = 1;         // Default value of rho
        scheduleVertex.vertexScheduledCount_ = 0;   // Number of scheduled instance of the vertex (default = 0)
        scheduleVertex.nDependencies_ = vertex->getNInEdge(); // TODO: take into account self loop
        scheduleVertex.vertexDependenciesArray_ = CREATE_MUL(TRANSFO_STACK, scheduleVertex.nDependencies_,
                                                             VertexDependency);
        for (int e = 0; e < vertex->getNInEdge(); ++e) {
            auto *edge = vertex->getInEdge(e);
            auto prod = edge->resolveProd();
            auto cons = edge->resolveCons();
            auto *srcVertex = edge->getSrc();
            scheduleVertex.vertexDependenciesArray_[e].vertex_ = &scheduleVertexArray_[srcVertex->getTypeId()];
            scheduleVertex.vertexDependenciesArray_[e].cons_ = cons;
            scheduleVertex.vertexDependenciesArray_[e].prod_ = prod;
        }
        scheduleVertex.endTimeArray_ = CREATE_MUL(TRANSFO_STACK, scheduleVertex.vertexCount_, Time);
    }
    /** 2. Compute the Rho values **/
    computeRhoValues();
}

SRDAGLessScheduler::~SRDAGLessScheduler() {
    if (schedule_) {
        schedule_->~Schedule();
        StackMonitor::free(TRANSFO_STACK, schedule_);
    }
    for (int i = 0; i < nVertices_; ++i) {
        auto &scheduleVertex = scheduleVertexArray_[i];
        scheduleVertex.vertex_ = nullptr;
        StackMonitor::free(TRANSFO_STACK, scheduleVertex.vertexDependenciesArray_);
        StackMonitor::free(TRANSFO_STACK, scheduleVertex.endTimeArray_);
    }
    StackMonitor::free(TRANSFO_STACK, scheduleVertexArray_);
}

//static bool computeMinRVNeeded(ScheduleVertex *const scheduleVertex) {
//    bool changedValue = true;
//    for (int i = 0; i < scheduleVertex->nDependencies_; ++i) {
//        auto &vertexDependency = scheduleVertex->vertexDependenciesArray_[i];
//        auto cons = vertexDependency.cons_;
//        auto prod = vertexDependency.prod_;
//        /** Compute raw rho value **/
//        auto currentMinExec = static_cast<int32_t>(cons / prod + (cons % prod != 0));
//        auto *srcScheduleVertex = vertexDependency.vertex_;
//        /** Take maximum between current rho value and raw value**/
////        currentMinExec = std::max(currentMinExec, scheduleVertex->vertexRhoValue_);
////        currentMinExec = std::min(currentMinExec, srcScheduleVertex->vertexCount_);
//        /** Set the rho value of previous actor **/
//        auto currentValue = srcScheduleVertex->vertexRhoValue_;
//        srcScheduleVertex->vertexRhoValue_ = std::max(currentValue, currentMinExec);
//        changedValue &= (srcScheduleVertex->vertexRhoValue_ == currentValue);
//    }
//    return changedValue;
//}

void SRDAGLessScheduler::computeRhoValues() {
    /** Compute current value of rho for every actor **/
    for (int v = 0; v < nVertices_; ++v) {
        auto &scheduleVertex = scheduleVertexArray_[v];
        for (int i = 0; i < scheduleVertex.nDependencies_; ++i) {
            auto &vertexDependency = scheduleVertex.vertexDependenciesArray_[i];
            auto cons = vertexDependency.cons_;
            auto prod = vertexDependency.prod_;
            /** Compute raw rho value **/
            auto currentMinExec = static_cast<int32_t>(cons / prod + (cons % prod != 0));
            auto *srcScheduleVertex = vertexDependency.vertex_;
            /** Take maximum between current rho value and raw value**/
//        currentMinExec = std::max(currentMinExec, scheduleVertex->vertexRhoValue_);
//        currentMinExec = std::min(currentMinExec, srcScheduleVertex->vertexCount_);
            /** Set the rho value of previous actor **/
            auto currentValue = srcScheduleVertex->vertexRhoValue_;
            srcScheduleVertex->vertexRhoValue_ = std::max(currentValue, currentMinExec);
        }
    }
}


static inline bool
isVertexSchedulable(ScheduleVertex *const scheduleVertex) {
    bool dependenciesStatisfied = true;
    for (int i = 0; i < scheduleVertex->nDependencies_ && dependenciesStatisfied; ++i) {
        auto &vertexDependency = scheduleVertex->vertexDependenciesArray_[i];
        auto cons = vertexDependency.cons_;
        auto availableData = vertexDependency.availableData_ - cons * scheduleVertex->vertexScheduledCount_;
        dependenciesStatisfied &= availableData >= cons;
    }
    return dependenciesStatisfied;
}

static inline int updateAvailableData(ScheduleVertex *const scheduleVertex) {
    Param numberSchedulable = std::min(scheduleVertex->vertexRhoValue_, scheduleVertex->vertexCount_);
    bool dependenciesStatisfied = numberSchedulable > 0;
    for (int i = 0; i < scheduleVertex->nDependencies_ && dependenciesStatisfied; ++i) {
        auto &vertexDependency = scheduleVertex->vertexDependenciesArray_[i];
        auto cons = vertexDependency.cons_;
        auto prod = vertexDependency.prod_;
        auto *srcScheduleVertex = vertexDependency.vertex_;
        auto availableData = prod * srcScheduleVertex->vertexScheduledCount_ -
                             cons * scheduleVertex->vertexScheduledCount_;
        numberSchedulable = std::min(numberSchedulable, availableData / cons);
    }
    return static_cast<int>(numberSchedulable);
}

void SRDAGLessScheduler::mapVertex(ScheduleVertex *const scheduleVertex) {
    int bestSlave = -1;
    Time bestStartTime = 0;
    Time bestEndTime = UINT64_MAX;
    Time bestWaitTime = 0;
    Time minimumStartTime = 0; // TODO: set this in function of other jobs dependencies
    for (int ix = 0; ix < scheduleVertex->nDependencies_; ++ix) {
        auto &vertexDependency = scheduleVertex->vertexDependenciesArray_[ix];
        auto *srcScheduleVertex = vertexDependency.vertex_;
        auto startIndex = static_cast<int>(scheduleVertex->vertexScheduledCount_ * vertexDependency.cons_ /
                                           vertexDependency.prod_);
        auto endIndex = startIndex + srcScheduleVertex->vertexRhoValue_;
        for (int i = startIndex; i < endIndex; ++i) {
            minimumStartTime = std::max(minimumStartTime, srcScheduleVertex->endTimeArray_[i]);
        }
    }
    auto *vertex = scheduleVertex->vertex_;
    for (int pe = 0; pe < archi_->getNPE(); ++pe) {
        /** Skip disabled processing elements **/
        if (!archi_->isActivated(pe)) {
            continue;
        }
        /** Search for best candidate **/
        if (vertex->canExecuteOn(pe)) {
            Time startTime = std::max(schedule_->getReadyTime(pe), minimumStartTime);
            Time waitTime = startTime - schedule_->getReadyTime(pe);
            auto peType = archi_->getPEType(pe);
            Time execTime = vertex->getTimingOnPE(peType);
            // TODO: add communication time in the balance
            Time endTime = startTime + execTime;
            if ((endTime < bestEndTime) || (endTime == bestEndTime && waitTime < bestWaitTime)) {
                bestStartTime = startTime;
                bestWaitTime = waitTime;
                bestEndTime = endTime;
                bestSlave = pe;
            }
        }
    }
    if (bestSlave < 0) {
        throwSpiderException("No slave found for vertex [%s].", vertex->getName());
    }
    auto *job = CREATE(TRANSFO_STACK, ScheduleJob)(vertex, bestSlave, bestSlave);
    job->setStartTime(bestStartTime);
    job->setEndTime(bestEndTime);
    schedule_->addJob(job);
    scheduleVertex->endTimeArray_[scheduleVertex->vertexScheduledCount_] = bestEndTime;
}

const Schedule *SRDAGLessScheduler::schedule() {
    bool done = false;
    while (!done) {
        done = true;
        /** Schedule **/
        for (int ix = 0; ix < nVertices_; ++ix) {
            auto *scheduleVertex = &scheduleVertexArray_[ix];
//            auto numberToSchedule = std::min(scheduleVertex->vertexRhoValue_, scheduleVertex->vertexCount_);
            auto numberSchedulable = updateAvailableData(scheduleVertex);
            for (int i = 0; i < numberSchedulable; ++i) {
//                /** Check if vertex can be scheduled **/
//                if (!isVertexSchedulable(scheduleVertex)) {
//                    break;
//                }
                /** Map the vertex **/
                mapVertex(scheduleVertex);
                /** Updating values **/
                scheduleVertex->vertexCount_--;
                scheduleVertex->vertexScheduledCount_++;
            }
            /** Test condition for ending **/
            done &= (scheduleVertex->vertexCount_ == 0);
        }
    }

    /** Execute and run the obtained schedule **/
//    schedule->execute();
//    Platform::get()->getLrt()->runUntilNoMoreJobs();
    return schedule_;
}

void SRDAGLessScheduler::printRhoValues() {
    fprintf(stderr, "\nINFO: Rho values:\n");
    for (int i = 0; i < nVertices_; i++) {
        fprintf(stderr, "INFO: >> Vertex: %s -- Rho: %d\n", graph_->getBody(i)->getName(),
                scheduleVertexArray_[i].vertexRhoValue_);
    }
}

