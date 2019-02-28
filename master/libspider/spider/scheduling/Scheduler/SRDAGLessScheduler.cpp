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

void SRDAGLessScheduler::initiliazeVertexScheduleIR(PiSDFVertex *const vertex, std::int32_t rv) {
    auto vertexIx = vertex->getTypeId();
    vertex->createScheduleJob(rv);
    rhoValueArray_[vertexIx] = 1;
    instanceAvlCountArray_[vertexIx] = rv;
    auto &dependencies = dependenciesArray_[vertexIx];
    dependencies = CREATE_MUL_NA(TRANSFO_STACK, vertex->getNInEdge(), VertexDependency);
    for (int i = 0; i < vertex->getNInEdge(); ++i) {
        auto *edge = vertex->getInEdge(i);
        auto *srcVertex = edge->getSrc();
        if (srcVertex->getSubType() == PISDF_SUBTYPE_INPUT_IF) {
            continue;
        }
        auto cons = edge->resolveCons();
        auto prod = edge->resolveProd();
        dependencies[i].vertex_ = srcVertex;
        dependencies[i].cons_ = cons;
        dependencies[i].prod_ = prod;
        dependencies[i].delay_ = edge->resolveDelay();
        dependencies[i].nScheduled_ = &instanceSchCountArray_[srcVertex->getTypeId()];
        if (edge->getSrc() == vertex && dependencies[i].delay_ == 0) {
            throwSpiderException("Graph not schedulable. Vertex [%s] has a self loop with no delay.",
                                 vertex->getName());
        }
    }
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

void SRDAGLessScheduler::replaceOutputIfWithRoundbuffer(const std::int32_t *const brv) {
    for (int ix = 0; ix < graph_->getNOutIf(); ++ix) {
        auto *outputIf = graph_->getOutputIf(ix);
        outputIf->createScheduleJob(1);
        auto *edge = outputIf->getInEdge(0);
        auto *vertex = edge->getSrc();
        auto vertexIx = vertex->getTypeId();
        auto vertexRv = brv[vertexIx];
        auto edgeSrcIx = edge->getSrcPortIx();
        auto cons = edge->resolveCons();
        auto prod = edge->resolveProd();
        /** Adding roundbuffer if needed **/
        if (cons != prod * vertexRv) {
            auto *roundbuffer = graph_->addSpecialVertex(
                    /* SubType */ PISDF_SUBTYPE_ROUNDBUFFER,
                    /* InData  */ 1,
                    /* OutData */ 1,
                    /* InParam */ 0);
            specialActorsAdded_.push_back(roundbuffer);
            vertex->disconnectOutEdge(edgeSrcIx);
            edge->disconnectSrc();
            edge->connectSrc(roundbuffer, 0, std::to_string(cons).c_str());
            roundbuffer->connectOutEdge(0, edge);
            graph_->connect(vertex, edgeSrcIx, edge->getProdExpr()->toString(),
                            roundbuffer, 0, std::to_string(prod * brv[vertexIx]).c_str(),
                            "0", nullptr, nullptr, nullptr, false);
            nVertices_++;
        }
    }
}

void SRDAGLessScheduler::replaceInputIfWithBroadcast(const std::int32_t *const brv) {
    for (int ix = 0; ix < graph_->getNInIf(); ++ix) {
        auto *inputIf = graph_->getInputIf(ix);
        inputIf->createScheduleJob(1);
        auto *edge = inputIf->getOutEdge(0);
        auto edgeSnkIx = edge->getSnkPortIx();
        auto *vertex = edge->getSnk();
        auto vertexIx = vertex->getTypeId();
        auto vertexRv = brv[vertexIx];
        auto cons = edge->resolveCons();
        auto prod = edge->resolveProd();
        /** Adding broadcast if needed **/
        if (prod != cons * vertexRv) {
            auto *broadcast = graph_->addSpecialVertex(
                    /* SubType */ PISDF_SUBTYPE_BROADCAST,
                    /* InData  */ 1,
                    /* OutData */ 1,
                    /* InParam */ 0);
            specialActorsAdded_.push_back(broadcast);
            vertex->disconnectInEdge(edgeSnkIx);
            edge->disconnectSnk();
            edge->connectSnk(broadcast, 0, std::to_string(prod).c_str());
            broadcast->connectInEdge(0, edge);
            graph_->connect(broadcast, 0, std::to_string(cons * brv[vertexIx]).c_str(),
                            vertex, edgeSnkIx, edge->getConsExpr()->toString(),
                            "0", nullptr, nullptr, nullptr, false);
            nVertices_++;
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

static void removeRoundBuffer(PiSDFVertex *vertex) {
    auto *edgeIn = vertex->getInEdge(0);
    auto *src = edgeIn->getSrc();
    auto srcPortIx = edgeIn->getSrcPortIx();
    auto *srcExpr = edgeIn->getProdExpr()->toString();
    auto *edgeOut = vertex->getOutEdge(0);
    /** Disconnect / Reconnect source on the edge **/
    edgeOut->disconnectSrc();
    edgeOut->connectSrc(src, srcPortIx, srcExpr);
    /** Disconnect / Reconnect edge on the vertex **/
    src->disconnectOutEdge(srcPortIx);
    src->connectInEdge(srcPortIx, edgeOut);
    /** Remove the vertex and the edge **/
    vertex->getGraph()->delVertex(vertex);
    vertex->getGraph()->delEdge(edgeIn);
}


SRDAGLessScheduler::SRDAGLessScheduler(PiSDFGraph *graph, const std::int32_t *brv, PiSDFSchedule *schedule,
                                       SRDAGLessScheduler *parent) {
    graph_ = graph;
    nVertices_ = graph->getNBody();
    schedule_ = schedule;
    parent_ = parent;
    nChildren_ = 0;
    firstChildIx_ = -1;

    auto originalGraphSize = graph->getNBody();

    /** 0. Check IF **/
    replaceInputIfWithBroadcast(brv);
    replaceOutputIfWithRoundbuffer(brv);

    /** 1. Creates the array of ScheduleVertex **/
    rhoValueArray_ = CREATE_MUL_NA(TRANSFO_STACK, nVertices_, std::int32_t);
    instanceAvlCountArray_ = CREATE_MUL_NA(TRANSFO_STACK, nVertices_, std::int32_t);
    instanceSchCountArray_ = CREATE_MUL_NA(TRANSFO_STACK, nVertices_, std::int32_t);
    memset(instanceSchCountArray_, 0, nVertices_ * sizeof(std::int32_t));
    dependenciesArray_ = CREATE_MUL_NA(TRANSFO_STACK, nVertices_, VertexDependency*);

    /** 2. Initialize vertices IR **/
    for (int ix = 0; ix < nVertices_; ++ix) {
        auto *vertex = graph_->getBody(ix);
        initiliazeVertexScheduleIR(vertex, ix >= originalGraphSize ? 1 : brv[ix]);
        nChildren_ += vertex->isHierarchical();
    }
    /** 3. Initialize subgraph IR **/
    children_ = CREATE_MUL_NA(TRANSFO_STACK, nChildren_, SRDAGLessScheduler*);
    firstChildIx_ = nVertices_ - nChildren_;
    for (int ix = 0; ix < nChildren_; ++ix) {
        auto *vertex = graph_->getBody(ix + firstChildIx_);
        auto *subgraph = vertex->getSubGraph();
        auto *childBRV = CREATE_MUL_NA(TRANSFO_STACK, subgraph->getNBody(), std::int32_t);
        computeBRV(subgraph, childBRV);
        children_[ix] = CREATE_NA(TRANSFO_STACK, SRDAGLessScheduler)(subgraph, childBRV, schedule_, this);
        StackMonitor::free(TRANSFO_STACK, childBRV);

    }
    if (!parent_) {
        /** Update interface IR **/
        for (int ix = 0; ix < nChildren_; ++ix) {
            if (children_[ix]) {
                initiliazeInterfacesIR(children_[ix]);
            }
        }
        /** 4. Compute the Rho values **/
        computeRhoValues();
    }
}

SRDAGLessScheduler::~SRDAGLessScheduler() {
    for (int ix = 0; ix < nVertices_; ++ix) {
        StackMonitor::free(TRANSFO_STACK, dependenciesArray_[ix]);
    }
    for (int ix = 0; ix < nChildren_; ++ix) {
        auto &child = children_[ix];
        child->~SRDAGLessScheduler();
        StackMonitor::free(TRANSFO_STACK, child);
    }
    for (auto &v : specialActorsAdded_) {
        if (v->getSubType() == PISDF_SUBTYPE_BROADCAST) {
            removeBroadcast(v);
        } else if (v->getSubType() == PISDF_SUBTYPE_ROUNDBUFFER) {
            removeRoundBuffer(v);
        }
    }
    schedule_ = nullptr;
    StackMonitor::free(TRANSFO_STACK, rhoValueArray_);
    StackMonitor::free(TRANSFO_STACK, instanceAvlCountArray_);
    StackMonitor::free(TRANSFO_STACK, instanceSchCountArray_);
    StackMonitor::free(TRANSFO_STACK, dependenciesArray_);
    StackMonitor::free(TRANSFO_STACK, children_);
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

void SRDAGLessScheduler::scheduleSubgraph(PiSDFVertex *const vertex, MemAlloc *memAlloc) {
    auto vertexIx = vertex->getTypeId();
    auto instance = instanceSchCountArray_[vertexIx];
    auto *childScheduler = children_[vertexIx - firstChildIx_];
    /** Reset availcount value of subgraph vertices **/
    if (instance > 0) {
        for (int ix = 0; ix < childScheduler->nVertices_; ++ix) {
            auto &currentAvlValue = childScheduler->instanceAvlCountArray_[ix];
            auto &currentSchValue = childScheduler->instanceSchCountArray_[ix];
            currentAvlValue = currentSchValue / instance;
        }
    }
    /** Do the scheduling of the subgraph **/
    childScheduler->schedule(memAlloc);
    std::string name = std::string(vertex->getName()) + std::string(".pgantt");
}

Time SRDAGLessScheduler::computeMinimumStartTime(PiSDFVertex *const vertex) const {
    Time minimumStartTime = 0;
    auto *job = vertex->getScheduleJob();
    auto instance = instanceSchCountArray_[vertex->getTypeId()];
    auto *jobConstrains = job->getScheduleConstrain(instance);
    auto *dependencies = dependenciesArray_[vertex->getTypeId()];
    for (int ix = 0; ix < vertex->getNInEdge(); ++ix) {
        auto cons = dependencies[ix].cons_;
        auto prod = dependencies[ix].prod_;
        auto delay = dependencies[ix].delay_;
        auto delayOffset = static_cast<int32_t>(delay / cons);
        // TODO: add delay setter / getter test condition (delay && setter.prod * setter.nScheduled == delay)
        auto deltaStart = static_cast<int32_t>((cons * instance) / prod) - delayOffset;
        if (deltaStart < 0) {
            continue;
        }
        auto *vertexInSrc = dependencies[ix].vertex_;
        auto *vertexInSrcJob = vertexInSrc->getScheduleJob();
        auto deltaEnd = static_cast<int32_t>((cons * (instance + 1) - 1) / prod) - delayOffset;
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
    auto instance = instanceSchCountArray_[vertex->getTypeId()];
    job->setMappedPE(instance, bestSlave);
    job->setMappingStartTime(instance, &bestStartTime);
    job->setMappingEndTime(instance, &bestEndTime);
    schedule_->addJob(job, instance);
}

void SRDAGLessScheduler::map(PiSDFVertex *const vertex, MemAlloc *memAlloc) {
    if (vertex->isHierarchical()) {
        scheduleSubgraph(vertex, memAlloc);
    } else {
        mapVertex(vertex);
    }
}

bool SRDAGLessScheduler::isSchedulable(PiSDFVertex *const vertex, std::int32_t nInstances) {
    auto vertexIx = vertex->getTypeId();
    auto *vertexDependencies = dependenciesArray_[vertexIx];
    bool canRun = true;
    for (int32_t ix = 0; ix < vertex->getNInEdge() && canRun; ++ix) {
        auto cons = vertexDependencies[ix].cons_;
        auto prod = vertexDependencies[ix].prod_;
        auto delay = vertexDependencies[ix].delay_;
        auto ixNeeded = static_cast<int32_t>((cons * nInstances - 1) / prod) -
                        static_cast<int32_t>(delay / cons); // floor(num / prod)
        auto nScheduled = *(vertexDependencies[ix].nScheduled_);
        canRun &= (nScheduled > ixNeeded);
    }
    return canRun;
}

const PiSDFSchedule *SRDAGLessScheduler::schedule(MemAlloc *memAlloc) {
    /** Alloc memory **/
//    memAlloc->alloc(graph_);
    /** Initialize list **/
    LinkedList<PiSDFVertex *> list(TRANSFO_STACK, nVertices_);
    for (int ix = 0; ix < nVertices_; ++ix) {
        auto *vertex = graph_->getBody(ix);
        list.add(vertex);
    }
    /** Iterate on the list **/
    list.setOnFirst();
    auto *node = list.getCurrent();
    while (node) {
        auto *vertex = node->val_;
        auto vertexIx = vertex->getTypeId();
        auto numberSchedulable = std::min(rhoValueArray_[vertexIx], instanceAvlCountArray_[vertexIx]);
        auto instance = instanceSchCountArray_[vertexIx];
        numberSchedulable *= isSchedulable(vertex, numberSchedulable + instance);
        for (int i = 0; i < numberSchedulable; ++i) {
            /** Map the vertex **/
            map(vertex, memAlloc);
            /** Updating values **/
            instanceAvlCountArray_[vertexIx]--;
            instanceSchCountArray_[vertexIx]++;
        }
        if (!instanceAvlCountArray_[vertexIx]) {
            /** Remove node as we finished using it **/
            list.del(node);
            node = list.getCurrent();
        } else {
            /** Get on next node **/
            node = list.next();
        }
    }
    return schedule_;
}











