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
#include <graphTransfo/ComputeBRV.h>
#include <cinttypes>
#include <tools/LinkedList.h>
#include <scheduling/MemAlloc.h>

#define SCHEDULE_SIZE 20000

static inline std::int32_t getVertexIx(const PiSDFVertex *const vertex) {
    return vertex->getId() - 1;
}

void SRDAGLessScheduler::initiliazeVertexScheduleIR(PiSDFVertex *const vertex, std::int32_t rv) {
    auto vertexIx = getVertexIx(vertex);
    vertex->createScheduleJob(rv);
    rhoValueArray_[vertexIx] = 1;
    instanceAvlCountArray_[vertexIx] = rv;
}

void SRDAGLessScheduler::initiliazeInterfacesIR(SRDAGLessScheduler *const scheduler) {
    auto *parent = scheduler->parent_;
    auto *graph = scheduler->graph_;
    auto *graphDependencies = parent->dependenciesArray_[graph->getParentVertex()->getTypeId()];
    /** Input If **/
    for (int ix = 0; ix < graph->getNInIf(); ++ix) {
        auto *inputIf = graph->getInputIf(ix);
        auto *edge = inputIf->getOutEdge(0);
        auto *sink = edge->getSnk();
        auto *snkDependency = &scheduler->dependenciesArray_[sink->getTypeId()][edge->getSnkPortIx()];
        snkDependency->vertex_ = graphDependencies[ix].vertex_;
        snkDependency->cons_ = edge->resolveCons();
        snkDependency->prod_ = graphDependencies[ix].prod_;
        snkDependency->delay_ = graphDependencies[ix].delay_ + edge->resolveDelay();
        snkDependency->nScheduled_ = graphDependencies[ix].nScheduled_;
    }
    /** Output If **/
    for (int ix = 0; ix < graph->getNOutIf(); ++ix) {
        auto *edge = graph->getParentVertex()->getOutEdge(ix);
        auto *sink = edge->getSnk();
        auto *snkDependency = &parent->dependenciesArray_[sink->getTypeId()][edge->getSnkPortIx()];
        auto *outputIf = graph->getOutputIf(ix);
        auto *edgeIf = outputIf->getInEdge(0);
        auto *vertexSrcIf = edgeIf->getSrc();
        auto vertexSrcIfIx = vertexSrcIf->getTypeId();
        snkDependency->vertex_ = vertexSrcIf;
        snkDependency->cons_ = snkDependency->cons_;
        snkDependency->prod_ = edgeIf->resolveProd();
        snkDependency->delay_ = snkDependency->delay_ + edgeIf->resolveDelay();
        snkDependency->nScheduled_ = &scheduler->instanceSchCountArray_[vertexSrcIfIx];
    }
    /** Go through children **/
    auto *children = scheduler->children_;
    for (int j = 0; j < scheduler->nChildren_; ++j) {
        for (int ix = 0; ix < scheduler->nChildren_; ++ix) {
            initiliazeInterfacesIR(children[ix]);
        }
    }

    /** Compute rho values **/
    scheduler->computeRhoValues();
}

void SRDAGLessScheduler::replaceInputIfWithBroadcast(PiSDFGraph *const graph) {
    for (int ix = 0; ix < graph->getNInIf(); ++ix) {
        auto *inputIf = graph->getInputIf(ix);
        auto *edge = inputIf->getOutEdge(0);
        auto edgeSnkIx = edge->getSnkPortIx();
        auto *snkVertex = edge->getSnk();
        auto cons = edge->resolveCons();
        auto prod = edge->resolveProd();
        /** Adding broadcast if needed **/
        auto totalCons = cons * snkVertex->getBRVValue();
        if (prod != totalCons) {
            auto *broadcast = graph->addSpecialVertex(
                    /* SubType */ PISDF_SUBTYPE_BROADCAST,
                    /* InData  */ 1,
                    /* OutData */ 1,
                    /* InParam */ 0);
            broadcast->setBRVValue(1);
            broadcast->createScheduleJob(1);
            broadcast->setId(nVertices_ + 1);
            broadcast->isExecutableOnPE(Spider::getArchi()->getSpiderPeIx());
            broadcast->setTimingOnType(Spider::getArchi()->getSpiderPeIx(), "100");
            specialActorsAdded_.push_back(broadcast);
            snkVertex->disconnectInEdge(edgeSnkIx);
            edge->disconnectSnk();
            edge->connectSnk(broadcast, 0, std::to_string(prod).c_str());
            broadcast->connectInEdge(0, edge);
            graph->connect(broadcast, 0, std::to_string(totalCons).c_str(),
                           snkVertex, edgeSnkIx, edge->getConsExpr()->toString(),
                           "0", nullptr, nullptr, nullptr, false);
            nVertices_++;
        }
    }
    for (int i = 0; i < graph->getNBody(); ++i) {
        auto *vertex = graph->getBody(i);
        if (vertex->isHierarchical()) {
            replaceInputIfWithBroadcast(vertex->getSubGraph());
        }
    }
}

static void removeBroadcast(PiSDFVertex *vertex) {
    auto *edgeOut = vertex->getOutEdge(0);
    auto *sink = edgeOut->getSnk();
    auto sinkPortIx = edgeOut->getSnkPortIx();
    auto *snkExpr = edgeOut->getConsExpr()->toString();
    auto *edgeIn = vertex->getInEdge(0);
    /** Disconnect / Reconnect sink on the edge **/
    edgeIn->disconnectSnk();
    edgeIn->connectSnk(sink, sinkPortIx, snkExpr);
    /** Disconnect / Reconnect edge on the vertex **/
    sink->disconnectInEdge(sinkPortIx);
    sink->connectInEdge(sinkPortIx, edgeIn);
    /** Remove the vertex and the edge **/
    vertex->getGraph()->delVertex(vertex);
    vertex->getGraph()->delEdge(edgeOut);
}

static std::int32_t computeTotalNBodies(PiSDFGraph *g) {
    std::int32_t nBodies = g->getNBody();
    for (auto i = 0; i < g->getNBody(); ++i) {
        if (g->getBody(i)->isHierarchical()) {
            nBodies += computeTotalNBodies(g->getBody(i)->getSubGraph());
        }
    }
    return nBodies;
}

void SRDAGLessScheduler::initIR(PiSDFGraph *const graph) {
    for (int ix = 0; ix < graph->getNBody(); ++ix) {
        auto *vertex = graph->getBody(ix);
        initiliazeVertexScheduleIR(vertex, vertex->getBRVValue());
//        fprintf(stderr, "INFO: Vertex [%s] -- BRV [%d] -- ID[%d]\n", vertex->getName(), vertex->getBRVValue(),
//                vertex->getId());
        if (vertex->isHierarchical()) {
            initIR(vertex->getSubGraph());
        }
    }
}

SRDAGLessScheduler::SRDAGLessScheduler(PiSDFGraph *graph, PiSDFSchedule *schedule, SRDAGLessScheduler *parent) {
    graph_ = graph;
    //nVertices_ = graph->getNBody();
    schedule_ = schedule;
    parent_ = parent;
    nChildren_ = 0;
    firstChildIx_ = -1;

    nVertices_ = computeTotalNBodies(graph);

    /** 0. Check IF **/
    replaceInputIfWithBroadcast(graph);

    /** 1. Creates the array of ScheduleVertex **/
    rhoValueArray_ = CREATE_MUL_NA(TRANSFO_STACK, nVertices_, std::int32_t);
    instanceAvlCountArray_ = CREATE_MUL_NA(TRANSFO_STACK, nVertices_, std::int32_t);
    instanceSchCountArray_ = CREATE_MUL_NA(TRANSFO_STACK, nVertices_, std::int32_t);
    memset(instanceSchCountArray_, 0, nVertices_ * sizeof(std::int32_t));

    /** 2. Initialize vertices IR **/
    initIR(graph);
//    fprintf(stderr, "INFO: finished init\n");
//    /** 4. Compute the Rho values **/
//    computeRhoValues();
}

SRDAGLessScheduler::~SRDAGLessScheduler() {
    for (int ix = 0; ix < nChildren_; ++ix) {
        auto &child = children_[ix];
        child->~SRDAGLessScheduler();
        StackMonitor::free(TRANSFO_STACK, child);
    }
    for (auto &v : specialActorsAdded_) {
        removeBroadcast(v);
    }
    schedule_ = nullptr;
    StackMonitor::free(TRANSFO_STACK, rhoValueArray_);
    StackMonitor::free(TRANSFO_STACK, instanceAvlCountArray_);
    StackMonitor::free(TRANSFO_STACK, instanceSchCountArray_);
//    StackMonitor::free(TRANSFO_STACK, dependenciesArray_);
//    StackMonitor::free(TRANSFO_STACK, children_);
}

void SRDAGLessScheduler::computeRhoValues() {
    /** Compute current value of rho for every actor **/
    for (int v = 0; v < graph_->getNBody(); ++v) {
        auto *vertex = graph_->getBody(v);
        auto *dependencies = dependenciesArray_[v];
        for (int i = 0; i < vertex->getNInEdge(); ++i) {
            auto *srcVertex = dependencies[i].vertex_;
            if (srcVertex->getGraph() != graph_) {
                /** Case of input if **/
                continue;
            }
            /** Compute raw rho value **/
            auto cons = dependencies[i].cons_;
            auto prod = dependencies[i].prod_;
            auto currentMinExec = static_cast<int32_t >(cons / prod + (cons % prod != 0));
            /** Take maximum between current rho value and raw value**/
//                currentMinExec = std::max(currentMinExec, rhoValueArray_[vertex->getTypeId()]);
//                currentMinExec = std::min(currentMinExec, instanceAvlCountArray_[srcVertex->getTypeId()]);
            /** Set the rho value of previous actor **/
            int index = srcVertex->getTypeId();
            // Case at least RHO execution of every successor
//                if (currentMinExec > rhoValueArray_[index]) {
//                    converged = false;
//                    rhoValueArray_[index] = currentMinExec;
//                }
            // Case at least ONE instance of every successor
            rhoValueArray_[index] = std::max(rhoValueArray_[index], currentMinExec);
//                //  Case at least ONE instance of ONE successor
//                if (currentMinExec > rhoValueArray_[index]) {
//                    rhoValueArray_[index] = std::max(rhoValueArray_[index], currentMinExec);
//                    break;
//                }
        }
    }
}

void SRDAGLessScheduler::printRhoValues() {
    fprintf(stderr, "\nINFO: Rho values:\n");
    for (int i = 0; i < nVertices_; i++) {
        fprintf(stderr, "INFO: >> Vertex: %s -- Rho: %d\n", graph_->getBody(i)->getName(),
                rhoValueArray_[i]);
    }
}

Time SRDAGLessScheduler::computeMinimumStartTime(PiSDFVertex *const vertex) const {
    Time minimumStartTime = 0;
    auto *job = vertex->getScheduleJob();
    auto instance = instanceSchCountArray_[getVertexIx(vertex)];
    auto *jobConstrains = job->getScheduleConstrain(instance);
    for (int ix = 0; ix < vertex->getNInEdge(); ++ix) {
        std::int32_t deltaStart = 0;
        std::int32_t deltaEnd = 0;
        auto *vertexInSrc = SRDAGLessIR::getProducer(vertex, ix, instance);
        if (vertexInSrc->getType() == PISDF_TYPE_IF) {
            SRDAGLessIR::computeDependenciesIxFromInputIF(vertex, ix, instanceSchCountArray_,
                                                          &vertexInSrc,
                                                          &deltaStart,
                                                          &deltaEnd);
            if (deltaStart < 0) {
                /*!< Case of init */
                continue;
            }
        } else {
            deltaStart = SRDAGLessIR::computeFirstDependencyIx(vertex, ix, instance);
            if (deltaStart < 0) {
                /*!< Case of init */
                continue;
            }
            deltaEnd = SRDAGLessIR::computeLastDependencyIx(vertex, ix, instance);
        }
        auto *vertexInSrcJob = vertexInSrc->getScheduleJob();
        for (int i = deltaStart; i <= deltaEnd; ++i) {
            /** Compute the minimal start time **/
            minimumStartTime = std::max(minimumStartTime, vertexInSrcJob->getMappingEndTime(i));
            /** Compute the minimal dependency we need **/
            auto pe = vertexInSrcJob->getMappedPE(i);
            auto currentValue = jobConstrains[pe].jobId_;
            auto srcInstanceJobID = vertexInSrcJob->getJobID(i);
            if (srcInstanceJobID > currentValue) {
                job->setScheduleConstrain(instance, pe, vertexInSrc, srcInstanceJobID, i);
            }
        }
    }
    return minimumStartTime;
}

void SRDAGLessScheduler::mapVertex(PiSDFVertex *const vertex) {
    auto *job = vertex->getScheduleJob();
    Time minimumStartTime = computeMinimumStartTime(vertex);
    int bestSlave = -1;
    Time bestStartTime = 0;
    Time bestEndTime = UINT64_MAX;
    Time bestWaitTime = 0;
    auto *archi = Spider::getArchi();
    for (int pe = 0; pe < archi->getNPE(); ++pe) {
        /** Skip disabled processing elements **/
        if (!archi->isActivated(pe)) {
            continue;
        }
        /** Search for best candidate **/
        if (vertex->canExecuteOn(pe)) {
            Time startTime = std::max(schedule_->getReadyTime(pe), minimumStartTime);
            Time waitTime = startTime - schedule_->getReadyTime(pe);
            auto peType = archi->getPEType(pe);
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
    auto instance = instanceSchCountArray_[getVertexIx(vertex)];
    job->setMappedPE(instance, bestSlave);
    job->setMappingStartTime(instance, &bestStartTime);
    job->setMappingEndTime(instance, &bestEndTime);
    schedule_->addJob(job, instance);
}

void SRDAGLessScheduler::map(PiSDFVertex *const vertex, MemAlloc *memAlloc) {
    auto vertexIx = getVertexIx(vertex);
    if (vertex->isHierarchical()) {
//        scheduleSubgraph(vertex, memAlloc);
//        /** Check if H actor is finished to be scheduled **/
//        bool isScheduled = true;
//        auto *subGraph = vertex->getSubGraph();
//        for (int i = 0; i < subGraph->getNBody() && isScheduled; ++i) {
//            auto *subVertex = subGraph->getBody(i);
//            auto globalInstance = instanceSchCountArray_[getVertexIx(subVertex)];
//            isScheduled &= (globalInstance % subVertex->getBRVValue() == 0);
//        }
//        if (isScheduled) {
//            /** Updating values **/
//            instanceAvlCountArray_[vertexIx]--;
//            instanceSchCountArray_[vertexIx]++;
//        }
        auto *subGraph = vertex->getSubGraph();
        schedule(subGraph, memAlloc);
        auto instance = instanceSchCountArray_[vertexIx];
        Time endTime = 0;
        for (int i = 0; i < subGraph->getNOutIf(); ++i) {
            auto *outputIf = subGraph->getOutputIf(i);
            auto *vertexSrc = outputIf->getInEdge(0)->getSrc();
            endTime = std::max(endTime, vertexSrc->getScheduleJob()->getMappingEndTime(vertexSrc->getBRVValue() - 1));
        }
        vertex->getScheduleJob()->setMappingEndTime(instance, &endTime);
    } else {
        mapVertex(vertex);
    }
    /** Updating values **/
    instanceAvlCountArray_[vertexIx]--;
    instanceSchCountArray_[vertexIx]++;
}

bool SRDAGLessScheduler::isSchedulable(PiSDFVertex *const vertex, std::int32_t /*nInstances*/) {
    auto vertexIx = getVertexIx(vertex);
    auto instance = instanceSchCountArray_[vertexIx];
//    auto *vertexDependencies = dependenciesArray_[vertexIx];
    bool canRun = true;
    for (int32_t ix = 0; ix < vertex->getNInEdge() && canRun; ++ix) {
        auto *vertexInSrc = SRDAGLessIR::getProducer(vertex, ix, instance);
        std::int32_t deltaIx = 0;
        if (vertexInSrc->getType() == PISDF_TYPE_IF) {
            continue;
        } else {
            deltaIx = SRDAGLessIR::computeLastDependencyIx(vertex, ix, instance);
        }
        canRun &= (instanceSchCountArray_[getVertexIx(vertexInSrc)] % (vertexInSrc->getBRVValue() + 1) > deltaIx);
    }
    return canRun;
}

static void buildNodeList(PiSDFGraph *const graph, LinkedList<PiSDFVertex *> &list) {
    for (int ix = 0; ix < graph->getNBody(); ++ix) {
        auto *vertex = graph->getBody(ix);
        list.add(vertex);
        if (vertex->isHierarchical()) {
            buildNodeList(vertex->getSubGraph(), list);
        }
    }
}

const PiSDFSchedule *SRDAGLessScheduler::schedule(PiSDFGraph *const graph, MemAlloc *memAlloc) {
    /** Alloc memory **/
//    memAlloc->alloc(graph_);
    /** Initialize list **/
    LinkedList<PiSDFVertex *> list(TRANSFO_STACK, nVertices_);
    for (int ix = 0; ix < graph->getNBody(); ++ix) {
        auto *vertex = graph->getBody(ix);
        list.add(vertex);
    }
    /** Iterate on the list **/
    list.setOnFirst();
    auto *node = list.current();
    LinkedList<std::int32_t> instanceList(TRANSFO_STACK, nVertices_);
    while (node) {
        auto *vertex = node->val_;
        auto vertexIx = getVertexIx(vertex);
//        auto numberSchedulable = std::min(rhoValueArray_[vertexIx], instanceAvlCountArray_[vertexIx]);
//        auto instance = instanceSchCountArray_[vertexIx];
//        numberSchedulable *= isSchedulable(vertex, numberSchedulable + instance);
        /*!< Check for all remaining instances if it is schedulable */
        auto numberSchedulable = isSchedulable(vertex, 1);
        for (int i = 0; i < numberSchedulable; ++i) {
            /** Map the vertex **/
            map(vertex, memAlloc);
        }
        if (!instanceAvlCountArray_[vertexIx]) {
            //  fprintf(stderr, "INFO: removing vertex[%s]\n", vertex->getName());
            /** Remove node as we finished using it **/
            list.del(node);
            node = list.current();
        } else {
            /** Get on next node **/
            node = list.next();
        }
    }
    return schedule_;
}











