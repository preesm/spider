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
#include "SRDAGLessListScheduler.h"
#include <graphs/PiSDF/PiSDFGraph.h>
#include <graphs/PiSDF/PiSDFEdge.h>
#include <Logger.h>

static inline std::int32_t computeTotalExpandedNVertices(PiSDFGraph *graph) {
    std::int32_t totalNVertices = 0;
    for (int i = 0; i < graph->getNBody(); ++i) {
        auto *vertex = graph->getBody(i);
        if (vertex->isHierarchical()) {
            auto subGraphNVertices = computeTotalExpandedNVertices(vertex->getSubGraph());
            subGraphNVertices *= vertex->getBRVValue();
            totalNVertices += subGraphNVertices;
        }
        totalNVertices += vertex->getBRVValue();
    }
    return totalNVertices;
}

void SRDAGLessListScheduler::initListOfVertex(PiSDFGraph *graph) {
    for (int i = 0; i < graph->getNBody(); ++i) {
        auto *vertex = graph->getBody(i);
        for (int j = 0; j < vertex->getBRVValue(); ++j) {
            auto *listSchdVertex = CREATE_NA(TRANSFO_STACK, ListSchdVertex);
            listSchdVertex->schedLvl_ = -1;
            listSchdVertex->vertex_ = vertex;
            listSchdVertex->instance_ = j;
            list_->add(listSchdVertex);
        }
        if (vertex->isHierarchical()) {
            initListOfVertex(vertex->getSubGraph());
        }
    }
}

SRDAGLessListScheduler::SRDAGLessListScheduler(PiSDFGraph *graph, PiSDFSchedule *schedule) : SRDAGLessScheduler(graph,
                                                                                                                schedule) {
    totalNVertices_ = computeTotalExpandedNVertices(graph);
    list_ = CREATE_NA(TRANSFO_STACK, List<ListSchdVertex *>)(TRANSFO_STACK, totalNVertices_);
    /** Init schedule level **/
    initListOfVertex(graph);
}

SRDAGLessListScheduler::~SRDAGLessListScheduler() {
    for (int i = 0; i < totalNVertices_; ++i) {
        auto *listSchdVertex = (*list_)[i];
        StackMonitor::free(TRANSFO_STACK, listSchdVertex);
    }
    list_->~List();
    StackMonitor::free(TRANSFO_STACK, list_);
//    for (int i = 0; i < nVertices_; ++i) {
//        StackMonitor::free(TRANSFO_STACK, schedLvl_[i]);
//    }
//    StackMonitor::free(TRANSFO_STACK, schedLvl_);
}

//static Time computeMinExecTime(PiSDFVertex *const vertex, Archi *const archi) {
//    Time minExecTime = UINT64_MAX;
//    for (int j = 0; j < archi->getNPE(); j++) {
//        if (vertex->canExecuteOn(j)) {
//            Time execTime = vertex->getTimingOnPEType(archi->getPEType(j));
//            if (execTime == 0) {
//                throwSpiderException("Vertex: %s -- NULL execution time.", vertex->getName());
//            }
//            minExecTime = std::min(minExecTime, execTime);
//        }
//    }
//    return minExecTime;
//}

std::int32_t SRDAGLessListScheduler::computeScheduleLevel(ListSchdVertex *const listSchdVertex) {
//    int lvl = 0;
//    auto *archi = Spider::getArchi();
//    if (schedLvl_[vertex->getTypeId()][0] == -1) {
//        for (int i = 0; i < vertex->getNOutEdge(); i++) {
//            auto *edge = vertex->getOutEdge(i);
//            auto *successor = edge->getSnk();
//            if (successor && successor != vertex) {
//                lvl = std::max(lvl, computeScheduleLevel(successor) + (int) computeMinExecTime(successor, archi));
//            }
//        }
//        for (int i = 0; i < instanceAvlCountArray_[vertex->getTypeId()]; ++i) {
//            schedLvl_[vertex->getTypeId()][i] = lvl;
//        }
//        return lvl;
//    }
//    auto vertexIx = vertex->getTypeId();
//    for (std::int32_t ix = 0; ix < instanceAvlCountArray_[vertex->getTypeId()]; ++ix) {
//        lvl = schedLvl_[vertexIx][ix] + (int) computeMinExecTime(vertex, archi);
//        for (int i = 0; i < vertex->getNInEdge(); i++) {
//            auto *edge = vertex->getInEdge(i);
//            auto *source = edge->getSrc();
//            auto cons = edge->resolveCons();
//            auto prod = edge->resolveProd();
//            auto deltaStart = (cons * ix) / prod;
//            auto deltaEnd = (cons * (ix + 1) - 1) / prod;
//            for (auto j = deltaStart; j <= deltaEnd; ++j) {
//                schedLvl_[source->getTypeId()][j] = std::max(schedLvl_[source->getTypeId()][j], lvl);
//            }
//        }
//    }
//    auto *vertex = listSchdVertex->vertex_;
//    for (int i = 0; i < vertex->getNOutEdge(); i++) {
//        auto *edge = vertex->getOutEdge(i);
//        auto *successor = edge->getSnk();
//        if (successor && successor != vertex) {
//            lvl = std::max(lvl, computeScheduleLevel(successor) + (int) computeMinExecTime(successor, archi));
//        }
//    }
    return listSchdVertex->schedLvl_;
}

int SRDAGLessListScheduler::compareScheduleLevels(ListSchdVertex *vertexA, ListSchdVertex *vertexB) {
    return vertexB->schedLvl_ - vertexA->schedLvl_;
}

inline void SRDAGLessListScheduler::sort() {
    myqsort(0, list_->getNb() - 1);
}

inline void SRDAGLessListScheduler::myqsort(int p, int r) {
    int q;
    if (p < r) {
        q = myqsort_part(p, r);
        myqsort(p, q);
        myqsort(q + 1, r);
    }
}

inline int SRDAGLessListScheduler::myqsort_part(int p, int r) {
    auto *pivot = (*list_)[p];
    int i = p - 1, j = r + 1;
    ListSchdVertex *temp;
    while (true) {
        do
            j--;
        while (compareScheduleLevels((*list_)[j], pivot) > 0);
        do
            i++;
        while (compareScheduleLevels((*list_)[i], pivot) < 0);
        if (i < j) {
            temp = (*list_)[i];
            (*list_)[i] = (*list_)[j];
            (*list_)[j] = temp;
        } else
            return j;
    }
}

const PiSDFSchedule *SRDAGLessListScheduler::schedule(PiSDFGraph *const /*graph*/, MemAlloc *memAlloc) {
    /** Compute schedule level **/
    for (int i = 0; i < nVertices_; ++i) {
        computeScheduleLevel((*list_)[i]);
    }

//    Logger::print(LOG_GENERAL, LOG_INFO, "Finished computing schedule levels.\n");

    /** Sort the list **/
    sort();

//    Logger::print(LOG_GENERAL, LOG_INFO, "Finished sorting list.\n");

    /** Map the vertices **/
    for (int i = 0; i < list_->getNb(); i++) {
        auto *listSchdVertex = (*list_)[i];
        SRDAGLessScheduler::map(listSchdVertex->vertex_, memAlloc);
        /** Update schedule count **/
        instanceSchCountArray_[getVertexIx(listSchdVertex->vertex_)]++;
    }
    return schedule_;
}


