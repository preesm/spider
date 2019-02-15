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
#include "ListScheduler.h"

#include <graphs/SRDAG/SRDAGGraph.h>
#include <launcher/Launcher.h>

ListScheduler::ListScheduler() {
    srdag_ = nullptr;
    schedule_ = nullptr;
    archi_ = nullptr;
    list_ = nullptr;
}

ListScheduler::~ListScheduler() = default;

static int compareSchedLevel(SRDAGVertex *vertexA, SRDAGVertex *vertexB) {
    return vertexB->getSchedLvl() - vertexA->getSchedLvl();
}

void ListScheduler::addPrevActors(SRDAGVertex *vertex, List<SRDAGVertex *> *list) {
    for (int i = 0; i < vertex->getNConnectedInEdge(); i++) {
        SRDAGVertex *prevVertex = vertex->getInEdge(i)->getSrc();
        if (!list->isPresent(prevVertex) && prevVertex->getState() == SRDAG_EXEC) {
            list->add(prevVertex);
            computeSchedLevel(prevVertex);
            addPrevActors(prevVertex, list);
        }
    }
}

void ListScheduler::scheduleOnlyConfig(
        SRDAGGraph *graph,
        MemAlloc *memAlloc,
        Schedule *schedule,
        Archi *archi) {
    srdag_ = graph;
    schedule_ = schedule;
    archi_ = archi;

    list_ = CREATE(TRANSFO_STACK, List<SRDAGVertex *>)(TRANSFO_STACK, srdag_->getNExecVertex());

    for (int i = 0; i < srdag_->getNVertex(); i++) {
        SRDAGVertex *vertex = srdag_->getVertex(i);
        if (vertex->getState() == SRDAG_EXEC && vertex->getNOutParam() > 0) {
            list_->add(vertex);
            vertex->setSchedLvl(-1);
            addPrevActors(vertex, list_);
        }
    }

    memAlloc->alloc(list_);

    for (int i = 0; i < list_->getNb(); i++) {
        computeSchedLevel((*list_)[i]);
    }

    list_->sort(compareSchedLevel);

    schedule_->setAllMinReadyTime(Platform::get()->getTime());
    schedule_->setReadyTime(
            /* Spider Pe */      archi->getSpiderPeIx(),
            /* End of Mapping */ Platform::get()->getTime() +
                                 archi->getMappingTimeFct()(list_->getNb(), archi_->getNPE()));


    for (int i = 0; i < list_->getNb(); i++) {
        this->scheduleVertex((*list_)[i]);
    }

    /** Send Broadcast notification **/
    Launcher::get()->sendBroadCastNotification(true);


    list_->~List();
    StackMonitor::free(TRANSFO_STACK, list_);
}

void ListScheduler::schedule(
        SRDAGGraph *graph,
        MemAlloc *memAlloc,
        Schedule *schedule,
        Archi *archi) {
    srdag_ = graph;
    schedule_ = schedule;
    archi_ = archi;

    list_ = CREATE(TRANSFO_STACK, List<SRDAGVertex *>)(TRANSFO_STACK, srdag_->getNExecVertex());

    // Fill the list_ with SRDAGVertices in scheduling order
    for (int i = 0; i < srdag_->getNVertex(); i++) {
        SRDAGVertex *vertex = srdag_->getVertex(i);
        if (vertex->getState() == SRDAG_EXEC) {
            list_->add(vertex);
            vertex->setSchedLvl(-1);
        }
    }

    memAlloc->alloc(list_);

    for (int i = 0; i < list_->getNb(); i++) {
        computeSchedLevel((*list_)[i]);
    }

    list_->sort(compareSchedLevel);
    schedule_->setAllMinReadyTime(Platform::get()->getTime());
    schedule_->setReadyTime(
            /* Spider Pe */     archi->getSpiderPeIx(),
            /* End of Mapping */Platform::get()->getTime() +
                                archi->getMappingTimeFct()(list_->getNb(), archi_->getNPE()));

    for (int i = 0; i < list_->getNb(); i++) {
        this->scheduleVertex((*list_)[i]);
    }

    list_->~List();
    StackMonitor::free(TRANSFO_STACK, list_);
}

int ListScheduler::computeSchedLevel(SRDAGVertex *vertex) {
    int lvl = 0;
    if (vertex->getSchedLvl() == -1) {
        for (int i = 0; i < vertex->getNConnectedOutEdge(); i++) {
            SRDAGVertex *succ = vertex->getOutEdge(i)->getSnk();
            if (succ && succ->getState() != SRDAG_NEXEC) {
                Time minExecTime = (unsigned int) -1;
                for (int j = 0; j < archi_->getNPE(); j++) {
                    if (succ->isExecutableOn(j)) {
                        Time execTime = succ->executionTimeOn(archi_->getPEType(j));
                        if (execTime == 0) {
                            throwSpiderException("Vertex: %s -- NULL execution time.", succ->toString());
                        }
                        minExecTime = std::min(minExecTime, execTime);
                    }
                }
                lvl = std::max(lvl, computeSchedLevel(succ) + (int) minExecTime);
            }
        }
        vertex->setSchedLvl(lvl);
        return lvl;
    }
    return vertex->getSchedLvl();
}

void ListScheduler::scheduleVertex(SRDAGVertex *vertex) {
    Time minimumStartTime = 0;
    auto *job = vertex->getScheduleJob();
    auto *jobConstrains = job->getScheduleConstrain(0);

    for (int i = 0; i < vertex->getNConnectedInEdge(); i++) {
        auto *edge = vertex->getInEdge(i);
        if (edge->getRate() != 0) {
            auto *srcVertex = edge->getSrc();
            auto *srcJob = srcVertex->getScheduleJob();
            auto pe = srcJob->getMappedPE(0);
            auto currentValue = jobConstrains[pe].jobId_;
            minimumStartTime = std::max(minimumStartTime, srcJob->getMappingEndTime(0));
            if (srcJob->getJobID(0) > currentValue) {
                job->setScheduleConstrain(0, pe, srcVertex->getSetIx(), srcJob->getJobID(0));
            }
        }
    }

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
    for (int pe = 0; pe < archi_->getNPE(); pe++) {
        int slaveType = archi_->getPEType(pe);

        if (!archi_->isActivated(pe)) continue;

        // checking the constraints
        if (vertex->isExecutableOn(pe)) {
            Time startTime = std::max(schedule_->getReadyTime(pe), minimumStartTime);
            Time waitTime = startTime - schedule_->getReadyTime(pe);
            Time execTime = vertex->executionTimeOn(slaveType);
            Time comInTime = 0, comOutTime = 0;// TODO: take into account com time
            Time endTime = startTime + execTime + comInTime + comOutTime;
            if (endTime < bestEndTime || (endTime == bestEndTime && waitTime < bestWaitTime)) {
                bestSlave = pe;
                bestEndTime = endTime;
                bestStartTime = startTime;
                bestWaitTime = waitTime;
            }
        }
    }
    if (bestSlave < 0) {
        throwSpiderException("No slave found to execute one instance of vertex [%s].", vertex->toString());
    }
    job->setMappedPE(0, bestSlave);
    job->setMappingStartTime(0, &bestStartTime);
    job->setMappingEndTime(0, &bestEndTime);
    schedule_->addJob(job, 0);
}
