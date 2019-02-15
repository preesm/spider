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
    schedule_ = CREATE_NA(TRANSFO_STACK, Schedule)(archi_->getNPE(), SCHEDULE_SIZE);

    /** 0. Creates the array of ScheduleVertex **/
    rhoValueArray_ = CREATE_MUL_NA(TRANSFO_STACK, nVertices_, std::int32_t);
    instanceAvlCountArray_ = CREATE_MUL_NA(TRANSFO_STACK, nVertices_, std::int32_t);
    instanceSchCountArray_ = CREATE_MUL_NA(TRANSFO_STACK, nVertices_, std::int32_t);
    memset(instanceSchCountArray_, 0, nVertices_ * sizeof(std::int32_t));
    dependenciesArray_ = CREATE_MUL_NA(TRANSFO_STACK, nVertices_, VertexDependency*);

    /** 1. Initialize properties **/
    // TODO: take into account self loop
    for (int ix = 0; ix < nVertices_; ++ix) {
        auto *vertex = graph->getBody(ix);
        vertex->createScheduleJob(brv[ix]);
        rhoValueArray_[ix] = 1;
        instanceAvlCountArray_[ix] = brv[ix];
        dependenciesArray_[ix] = CREATE_MUL_NA(TRANSFO_STACK, vertex->getNInEdge(), VertexDependency);
        for (int i = 0; i < vertex->getNInEdge(); ++i) {
            auto *edge = vertex->getInEdge(i);
            dependenciesArray_[ix][i].vertex_ = edge->getSrc();
            dependenciesArray_[ix][i].cons_ = edge->resolveCons();
            dependenciesArray_[ix][i].prod_ = edge->resolveProd();
            dependenciesArray_[ix][i].delay_ = edge->resolveDelay();
            if (edge->getSrc() == vertex && dependenciesArray_[ix][i].delay_ == 0) {
                throwSpiderException("Graph not schedulable. Vertex [%s] has a self loop with no delay.",
                                     vertex->getName());
            }
        }
    }
    /** 2. Compute the Rho values **/
    computeRhoValues();
}

SRDAGLessScheduler::~SRDAGLessScheduler() {
    if (schedule_) {
        schedule_->~Schedule();
        StackMonitor::free(TRANSFO_STACK, schedule_);
    }
    StackMonitor::free(TRANSFO_STACK, rhoValueArray_);
    StackMonitor::free(TRANSFO_STACK, instanceAvlCountArray_);
    StackMonitor::free(TRANSFO_STACK, instanceSchCountArray_);
    for (int ix = 0; ix < graph_->getNBody(); ++ix) {
        StackMonitor::free(TRANSFO_STACK, dependenciesArray_[ix]);
    }
    StackMonitor::free(TRANSFO_STACK, dependenciesArray_);
}

void SRDAGLessScheduler::computeRhoValues() {
    /** Compute current value of rho for every actor **/
    for (int v = 0; v < nVertices_; ++v) {
        auto *vertex = graph_->getBody(v);
        auto *dependencies = dependenciesArray_[v];
        for (int i = 0; i < vertex->getNInEdge(); ++i) {
            auto cons = dependencies[i].cons_;
            auto prod = dependencies[i].prod_;
            auto *srcVertex = dependencies[i].vertex_;
            /** Compute raw rho value **/
            auto currentMinExec = static_cast<int32_t>(cons / prod + (cons % prod != 0));
            /** Take maximum between current rho value and raw value**/
//        currentMinExec = std::max(currentMinExec, scheduleVertex->vertexRhoValue_);
//        currentMinExec = std::min(currentMinExec, srcScheduleVertex->vertexCount_);
            /** Set the rho value of previous actor **/
            int index = srcVertex->getTypeId();
            rhoValueArray_[index] = std::max(rhoValueArray_[index], currentMinExec);
        }
    }
}

inline int SRDAGLessScheduler::updateAvailableData(PiSDFVertex *const vertex) {
    int index = vertex->getTypeId();
    Param numberSchedulable = std::min(rhoValueArray_[index], instanceAvlCountArray_[index]);
    bool dependenciesStatisfied = numberSchedulable > 0;
    auto *dependencies = dependenciesArray_[vertex->getTypeId()];
    for (int i = 0; i < vertex->getNInEdge() && dependenciesStatisfied; ++i) {
        auto cons = dependencies[i].cons_;
        auto prod = dependencies[i].prod_;
        auto *srcVertex = dependencies[i].vertex_;
        auto availableData = dependencies[i].delay_ +
                             prod * instanceSchCountArray_[srcVertex->getTypeId()] -
                             cons * instanceSchCountArray_[index];
        numberSchedulable = std::min(numberSchedulable, availableData / cons);
    }
    return static_cast<int>(numberSchedulable);
}

Time SRDAGLessScheduler::computeMinimumStartTime(const PiSDFVertex *vertex, Time minimumStartTime, ScheduleJob *job,
                                                 int32_t vertexSchCount, int32_t currentInstance,
                                                 const JobConstrain *jobConstrains) const {
    auto *dependencies = dependenciesArray_[vertex->getTypeId()];
    for (int ix = 0; ix < vertex->getNInEdge(); ++ix) {
        auto cons = dependencies[ix].cons_;
        auto prod = dependencies[ix].prod_;
        auto *srcVertex = dependencies[ix].vertex_;
        auto srcVertexIndex = srcVertex->getTypeId();
        /** Computing the range of instances we depend on **/
        auto totalCons = vertexSchCount * cons;
        auto delay = dependencies[ix].delay_;
        auto startIndex = static_cast<int>(totalCons / prod) - static_cast<int>(delay / cons);
        auto endIndex = startIndex + rhoValueArray_[srcVertexIndex];
        if (startIndex < 0) {
            continue;
        }
        auto *srcJob = srcVertex->getScheduleJob();
        for (int i = startIndex; i < endIndex; ++i) {
            /** Compute the minimal start time **/
            minimumStartTime = std::max(minimumStartTime, srcJob->getMappingEndTime(i));
            /** Compute the minimal dependency we need **/
            auto pe = srcJob->getMappedPE(i);
            auto currentValue = jobConstrains[pe].jobId_;
            auto srcInstanceJobID = srcJob->getJobID(i);
            if (srcInstanceJobID > currentValue) {
                job->setScheduleConstrain(currentInstance, pe, srcVertexIndex, srcInstanceJobID, i);
            }
        }
    }
    return minimumStartTime;
}

void SRDAGLessScheduler::mapVertex(PiSDFVertex *const vertex) {
    Time minimumStartTime = 0; // TODO: set this in function of other jobs dependencies
    auto *job = vertex->getScheduleJob();
    auto vertexSchCount = instanceSchCountArray_[vertex->getTypeId()];
    auto currentInstance = vertexSchCount;
    auto *jobConstrains = job->getScheduleConstrain(currentInstance);

    minimumStartTime = computeMinimumStartTime(vertex, minimumStartTime, job, vertexSchCount, currentInstance,
                                               jobConstrains);
    int bestSlave = -1;
    Time bestStartTime = 0;
    Time bestEndTime = UINT64_MAX;
    Time bestWaitTime = 0;
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
            Time execTime = vertex->getTimingOnPEType(peType);
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
    job->setMappedPE(currentInstance, bestSlave);
    job->setMappingStartTime(currentInstance, &bestStartTime);
    job->setMappingEndTime(currentInstance, &bestEndTime);
    schedule_->addJob(job, currentInstance);
}



const Schedule *SRDAGLessScheduler::schedule() {
    bool done = false;
    while (!done) {
        done = true;
        bool scheduledAtLeastOne = false;
        /** Schedule **/
        for (int ix = 0; ix < nVertices_; ++ix) {
            auto *vertex = graph_->getBody(ix);
            auto numberSchedulable = updateAvailableData(vertex);
            scheduledAtLeastOne = scheduledAtLeastOne | (numberSchedulable > 0);
            for (int i = 0; i < numberSchedulable; ++i) {
                /** Map the vertex **/
                mapVertex(vertex);
                /** Updating values **/
                instanceAvlCountArray_[ix]--;
                instanceSchCountArray_[ix]++;
            }
            /** Test condition for ending **/
            done &= (instanceAvlCountArray_[ix] == 0);
        }
        if (!scheduledAtLeastOne) {
            throwSpiderException("Failed to schedule at least one vertex.");
        }
    }
    return schedule_;
}

void SRDAGLessScheduler::printRhoValues() {
    fprintf(stderr, "\nINFO: Rho values:\n");
    for (int i = 0; i < nVertices_; i++) {
        fprintf(stderr, "INFO: >> Vertex: %s -- Rho: %d\n", graph_->getBody(i)->getName(),
                rhoValueArray_[i]);
    }
}

