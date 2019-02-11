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

static int computeMinRVNeeded(PiSDFVertex *const vertex) {
    int finalMinExec = 1;
    for (int i = 0; i < vertex->getNOutEdge(); ++i) {
        auto *edge = vertex->getOutEdge(i);
        auto cons = edge->resolveCons();
        auto prod = edge->resolveProd();
        float alpha = 1.f;
        if (cons > prod) {
            alpha = static_cast<float>(cons) / static_cast<float>(prod);
        }
        int minExec = static_cast<int>(std::ceil(alpha));
        finalMinExec = std::max(minExec, finalMinExec);
    }
    return finalMinExec;
}

static void computeRhoValues(PiSDFGraph *root, std::int32_t *rhoValues) {
    bool converged;
    int iter = 0;
    do {
        converged = true;
        /** Compute current value of rho for every actor **/
        for (int i = 0; i < root->getNBody(); ++i) {
            auto *vertex = root->getBody(i);
            int precValue = rhoValues[i];
            rhoValues[i] = computeMinRVNeeded(vertex);
            converged &= (precValue == rhoValues[i]);
        }
        /** Ensure that we did at least two passes **/
        converged &= (iter > 0);
        /** Update iteration counter **/
        iter++;
    } while (!converged);
}

SRDAGLessScheduler::SRDAGLessScheduler(PiSDFGraph *graph, const std::int32_t *brv) {
    graph_ = graph;
    nVertices_ = graph->getNBody();
    archi_ = Spider::getArchi();
    schedule_ = nullptr;

    /** Number of instances of each vertex remaining in the graph (initialized as BRV) **/
    verticesCount_ = CREATE_MUL(TRANSFO_STACK, nVertices_, std::int32_t);
    memcpy(verticesCount_, brv, nVertices_ * sizeof(std::int32_t));
    /** Minimal value of number of instances of a given vertex that has to be scheduled w.r.t vertex successor **/
    verticesRhoValues_ = CREATE_MUL(TRANSFO_STACK, nVertices_, std::int32_t);
    for (int i = 0; i < nVertices_; ++i) {
        verticesRhoValues_[i] = 1;
    }
    computeRhoValues(graph_, verticesRhoValues_);
    /** Number of instances of each vertex already scheduled **/
    verticesScheduledCount_ = CREATE_MUL(TRANSFO_STACK, nVertices_, std::int32_t);
    memset(verticesScheduledCount_, 0, graph->getNBody() * sizeof(std::int32_t));
    /** End time of each instance of each vertex of **/
    verticesEndTime_ = CREATE_MUL(TRANSFO_STACK, nVertices_, std::vector<Time>);
}

SRDAGLessScheduler::~SRDAGLessScheduler() {
    StackMonitor::free(TRANSFO_STACK, verticesCount_);
    StackMonitor::free(TRANSFO_STACK, verticesEndTime_);
    StackMonitor::free(TRANSFO_STACK, verticesRhoValues_);
    StackMonitor::free(TRANSFO_STACK, verticesScheduledCount_);
    if (schedule_) {
        schedule_->~Schedule();
        StackMonitor::free(TRANSFO_STACK, schedule_);
    }
}

static inline bool
isVertexSchedulable(PiSDFVertex *const vertex, const int *countVertex, const int *countScheduledVertex) {
    bool dependenciesStatisfied = countVertex[vertex->getTypeId()] != 0;
    auto nScheduledVertex = countScheduledVertex[vertex->getTypeId()];
    for (int i = 0; i < vertex->getNInEdge() && dependenciesStatisfied; ++i) {
        auto *edge = vertex->getInEdge(i);
        auto prod = edge->resolveProd();
        auto cons = edge->resolveCons();
        auto *vertexIN = edge->getSrc();
        auto nScheduledVertexIN = countScheduledVertex[vertexIN->getTypeId()];
        auto availableData = prod * nScheduledVertexIN - cons * nScheduledVertex;
        dependenciesStatisfied &= availableData >= cons;
    }
    return dependenciesStatisfied;
}

void SRDAGLessScheduler::mapVertex(PiSDFVertex *const vertex) {
    int bestSlave = -1;
    Time bestStartTime = 0;
    Time bestEndTime = UINT64_MAX;
    Time bestWaitTime = 0;
    Time minimumStartTime = 0; // TODO: set this in function of other jobs dependencies
    auto nScheduledVertex = verticesScheduledCount_[vertex->getTypeId()];
    for (int i = 0; i < vertex->getNInEdge(); ++i) {
        auto *edge = vertex->getInEdge(i);
        auto *vertexIN = edge->getSrc();
        auto &vertexINEndTimes = verticesEndTime_[vertexIN->getTypeId()];
        Time maxTime = 0;
        Param prod = edge->resolveProd();
        Param cons = edge->resolveCons();
        Param startIndex = cons * nScheduledVertex / prod;
        Param endIndex = startIndex + std::max(1, static_cast<int>(cons / prod));
        for (Param ix = startIndex; ix < endIndex; ++ix) {
            auto time = vertexINEndTimes[ix];
            maxTime = std::max(maxTime, time);
        }
        minimumStartTime = std::max(minimumStartTime, maxTime);
    }
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
    verticesEndTime_[vertex->getTypeId()].push_back(bestEndTime);
}

const Schedule *SRDAGLessScheduler::schedule() {
    schedule_ = CREATE(TRANSFO_STACK, Schedule)(archi_->getNPE(), SCHEDULE_SIZE);

    bool done = false;
    while (!done) {
        done = true;
        /** Schedule **/
        for (int ix = 0; ix < nVertices_; ++ix) {
            auto *vertex = graph_->getBody(ix);
            for (int i = 0; i < verticesRhoValues_[ix]; ++i) {
                /** Check if vertex can be scheduled **/
                if (!isVertexSchedulable(vertex, verticesCount_, verticesScheduledCount_)) {
                    break;
                }
                /** Map the vertex **/
                auto currentCount = verticesCount_[ix];
                mapVertex(vertex);
                /** Updating values **/
                verticesCount_[ix] = verticesCount_[ix] - 1;
                verticesRhoValues_[ix] = std::min(verticesRhoValues_[ix], verticesCount_[ix]);
                verticesScheduledCount_[ix] += (currentCount - verticesCount_[ix]);
            }
            /** Test condition for ending **/
            done &= (verticesCount_[ix] == 0);
        }
    }

    schedule_->print("schedule-new.pgantt");
    /** Execute and run the obtained schedule **/
//    schedule->execute();
//    Platform::get()->getLrt()->runUntilNoMoreJobs();
    return schedule_;
}

void SRDAGLessScheduler::printRhoValues() {
    fprintf(stderr, "\nINFO: Rho values:\n");
    for (int i = 0; i < nVertices_; i++) {
        fprintf(stderr, "INFO: >> Vertex: %s -- Rho: %d\n", graph_->getBody(i)->getName(), verticesRhoValues_[i]);
    }
}
