/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018 - 2019)
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
#include "GreedyScheduler.h"
#include <graphs/SRDAG/SRDAGGraph.h>
#include <graphs/SRDAG/SRDAGVertex.h>
#include <cinttypes>
#include <tools/LinkedList.h>

GreedyScheduler::GreedyScheduler() {
    srdag_ = nullptr;
    schedule_ = nullptr;
    archi_ = nullptr;
    list_ = nullptr;
}

GreedyScheduler::~GreedyScheduler() = default;

void GreedyScheduler::mapVertex(SRDAGVertex *vertex) {
    Time minimumStartTime = computeMinimumStartTime(vertex);

    if (vertex->getState() == SRDAG_RUN) {
        vertex->setStartTime(minimumStartTime);
        vertex->setEndTime(minimumStartTime);
        return;
    }

    int bestSlave = -1;
    Time bestStartTime = 0;
    Time bestWaitTime = 0;
    auto bestEndTime = (Time) -1; // Very high value.

    // Getting a slave for the vertex.
    for (std::uint32_t peIx = 0; peIx < archi_->getNPE(); peIx++) {
        auto *pe = archi_->getPEFromSpiderID(peIx);
        int slaveType = pe->getHardwareType();

        if (!pe->isEnabled()) continue;

        // checking the constraints
        if (vertex->isExecutableOn(peIx)) {
            Time startTime = std::max(schedule_->getReadyTime(peIx), minimumStartTime);
            Time waitTime = startTime - schedule_->getReadyTime(peIx);
            Time execTime = vertex->executionTimeOn(slaveType);
            Time comInTime = 0, comOutTime = 0;// TODO: take into account com time
            Time endTime = startTime + execTime + comInTime + comOutTime;
            if (endTime < bestEndTime || (endTime == bestEndTime && waitTime < bestWaitTime)) {
                bestSlave = peIx;
                bestEndTime = endTime;
                bestStartTime = startTime;
                bestWaitTime = waitTime;
            }
        }
    }
    if (bestSlave < 0) {
        throwSpiderException("No slave found to execute one instance of vertex [%s].", vertex->toString());
    }
    Scheduler::addJobToSchedule(schedule_, vertex, bestSlave, &bestStartTime, &bestEndTime);
}

bool GreedyScheduler::isSchedulable(SRDAGVertex *vertex) {
    if (!vertex->getNConnectedInEdge()) {
        return true;
    }
    bool canRun = true;
    for (int i = 0; i < vertex->getNConnectedInEdge() && canRun; ++i) {
        auto *edge = vertex->getInEdge(i);
        if (!edge) {
            continue;
        }
        auto *vertexSrc = edge->getSrc();
        auto *scheduleJobSrc = vertexSrc->getScheduleJob();
        canRun &= (scheduleJobSrc->getMappedPE() >= 0);
    }
    return canRun;
}

void GreedyScheduler::schedule(SRDAGGraph *graph, MemAlloc *memAlloc, SRDAGSchedule *schedule, Archi *archi) {
    srdag_ = graph;
    schedule_ = schedule;
    archi_ = archi;
    /* Initialize list */
    LinkedList<SRDAGVertex *> list(TRANSFO_STACK, graph->getNVertex());
    for (int ix = 0; ix < graph->getNVertex(); ++ix) {
        auto *vertex = graph->getVertex(ix);
        list.add(vertex);
    }

    /* Pre-schedule memalloc */
    memAlloc->alloc(&list);

    /* Set time (maybe not that useful ?) */
    schedule_->setAllMinReadyTime(Platform::get()->getTime());
    schedule_->setReadyTime(
            /* Spider Pe */     archi->getSpiderGRTID(),
            /* End of Mapping */Platform::get()->getTime() +
                                archi->getScheduleTimeRoutine()(list.size(), archi_->getNPE()));


    /* Iterate on the list */
    list.setOnFirst();

    auto *node = list.current();
    while (node) {
        auto *vertex = node->val_;
        if (isSchedulable(vertex)) {
            /** Map the vertex **/
            mapVertex(vertex);
            /** Remove node as we finished using it **/
            list.del(node);
            node = list.current();
        } else {
            /** Get on next node **/
            node = list.next();
        }
    }
}

