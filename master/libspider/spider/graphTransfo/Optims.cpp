/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2015 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2015 - 2018)
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
#include "Optims.h"

#include <cmath>

static bool joinjoinUsed = false;
static bool forkforkUsed = false;
static bool brbrUsed = false;
static bool brUsed = false;
static bool forkUsed = false;
static bool joinUsed = false;
static bool initendUsed = false;
static bool joinforkUsed = false;

void printOptimsStats() {
    if (!joinjoinUsed) printf("JoinJoin Unused\n");
    if (!forkforkUsed) printf("ForkFork Unused\n");
    if (!brbrUsed) printf("BrBr Unused\n");
    if (!brUsed) printf("Br Unused\n");
    if (!forkUsed) printf("Fork Unused\n");
    if (!joinUsed) printf("Join Unused\n");
    if (!initendUsed) printf("InitEnd Unused\n");
    if (!joinforkUsed) printf("JoinFork Unused\n");
}

static int removeNullEdge(SRDAGGraph *topDag) {
    for (int i = 0; i < topDag->getNEdge(); i++) {
        SRDAGEdge *nullEdge = topDag->getEdge(i);
        if (nullEdge->getRate() == 0) {
            SRDAGVertex *src = nullEdge->getSrc();
            SRDAGVertex *snk = nullEdge->getSnk();
            if (src && src->getType() == SRDAG_FORK && src->getState() == SRDAG_EXEC && snk == nullptr) {
                int forkNOut = src->getNConnectedOutEdge();
                int forkPortIx = nullEdge->getSrcPortIx();
                nullEdge->disconnectSrc();
                topDag->delEdge(nullEdge);

                /* Shift on Fork */
                for (int k = forkPortIx + 1; k < forkNOut; k++) {
                    SRDAGEdge *edge = src->getOutEdge(k);
                    edge->disconnectSrc();
                    edge->connectSrc(src, k - 1);
                }
                return true;
            }
            if (snk && snk->getType() == SRDAG_JOIN && snk->getState() == SRDAG_EXEC && src == nullptr) {
                int joinNIn = snk->getNConnectedInEdge();
                int joinPortIx = nullEdge->getSnkPortIx();
                nullEdge->disconnectSnk();
                topDag->delEdge(nullEdge);

                /* Shift on Fork */
                for (int k = joinPortIx + 1; k < joinNIn; k++) {
                    SRDAGEdge *edge = snk->getInEdge(k);
                    edge->disconnectSnk();
                    edge->connectSnk(snk, k - 1);
                }
                return true;
            }
        }
    }
    return false;
}

static int reduceRB(SRDAGGraph *topDag) {
    for (int i = 0; i < topDag->getNVertex(); i++) {
        SRDAGVertex *rb = topDag->getVertex(i);
        if (rb && rb->getState() == SRDAG_EXEC && rb->getType() == SRDAG_ROUNDBUFFER) {
            int inTkn = rb->getInEdge(0)->getRate();
            int outTkn = rb->getOutEdge(0)->getRate();
            if (inTkn == outTkn) {
                SRDAGEdge *inEdge = rb->getInEdge(0);
                SRDAGVertex *nextVertex = rb->getOutEdge(0)->getSnk();
                int edgeIx = rb->getOutEdge(0)->getSnkPortIx();

                SRDAGEdge *delEdge = rb->getOutEdge(0);
                delEdge->disconnectSrc();
                delEdge->disconnectSnk();
                topDag->delEdge(delEdge);

                inEdge->disconnectSnk();
                inEdge->connectSnk(nextVertex, edgeIx);
                topDag->delVertex(rb);
                return 1;
            } else if (inTkn < outTkn) {
                /* Duplicate Token Using Br/Join Pattern */
                auto nBrOut = (int) ceil(((float) outTkn) / inTkn);
                SRDAGVertex *br = topDag->addBroadcast(MAX_IO_EDGES);
                SRDAGVertex *join = topDag->addJoin(MAX_IO_EDGES);
                int rest = outTkn;

                br->setState(SRDAG_EXEC);
                join->setState(SRDAG_EXEC);

                SRDAGEdge *inEdge = rb->getInEdge(0);
                SRDAGEdge *outEdge = rb->getOutEdge(0);

                inEdge->disconnectSnk();
                inEdge->connectSnk(br, 0);
                outEdge->disconnectSrc();
                outEdge->connectSrc(join, 0);
                topDag->delVertex(rb);

                for (int j = 0; j < nBrOut; j++) {
                    if (rest >= inTkn) {
                        topDag->addEdge(br, j, join, j, inTkn);
                        rest -= inTkn;
                    } else {
                        SRDAGVertex *fork = topDag->addFork(MAX_IO_EDGES);
                        SRDAGVertex *end = topDag->addEnd();
                        fork->setState(SRDAG_EXEC);
                        end->setState(SRDAG_EXEC);
                        topDag->addEdge(br, j, fork, 0, inTkn);
                        topDag->addEdge(fork, 0, join, j, rest);
                        topDag->addEdge(fork, 1, end, 0, inTkn - rest);
                        rest -= rest;
                    }
                }
            } else { /* inTkn > outTkn */
                /* Discard Token Using Fork/End Pattern */
                SRDAGEdge *inEdge = rb->getInEdge(0);
                SRDAGEdge *outEdge = rb->getOutEdge(0);

                SRDAGVertex *fork = topDag->addFork(MAX_IO_EDGES);
                SRDAGVertex *end = topDag->addEnd();
                fork->setState(SRDAG_EXEC);
                end->setState(SRDAG_EXEC);
                topDag->addEdge(fork, 0, end, 0, inTkn - outTkn);

                inEdge->disconnectSnk();
                inEdge->connectSnk(fork, 0);

                outEdge->disconnectSrc();
                outEdge->connectSrc(fork, 1);

                topDag->delVertex(rb);
            }
        }
    }
    return 0;
}

static int reduceJoinJoin(SRDAGGraph *topDag) {
    for (int i = 0; i < topDag->getNVertex(); i++) {
        SRDAGVertex *join0 = topDag->getVertex(i);
        if (join0 && join0->getState() == SRDAG_EXEC && join0->getType() == SRDAG_JOIN) {
            SRDAGVertex *join1 = join0->getOutEdge(0)->getSnk();
            if (join1 && join1->getType() == SRDAG_JOIN) {
                int nbToAdd = join0->getNConnectedInEdge();
                int ixToAdd = join0->getOutEdge(0)->getSnkPortIx();

                int nbTotEdges = join1->getNConnectedInEdge() + nbToAdd - 1;

                // Shift edges after
                for (int k = nbTotEdges - 1; k > ixToAdd + nbToAdd - 1; k--) {
                    SRDAGEdge *edge = join1->getInEdge(k - nbToAdd + 1);
                    edge->disconnectSnk();
                    edge->connectSnk(join1, k);
                }

                SRDAGEdge *delEdge = join1->getInEdge(ixToAdd);
                delEdge->disconnectSnk();
                topDag->delEdge(delEdge);

                // Add edges
                for (int k = 0; k < nbToAdd; k++) {
                    SRDAGEdge *edge = join0->getInEdge(k);
                    edge->disconnectSnk();
                    edge->connectSnk(join1, k + ixToAdd);
                }

                topDag->delVertex(join0);

                joinjoinUsed = true;
                return 1;
            }
        }
    }
    return 0;
}

static int reduceForkFork(SRDAGGraph *topDag) {
    for (int i = 0; i < topDag->getNVertex(); i++) {
        SRDAGVertex *secFork = topDag->getVertex(i);
        if (secFork && secFork->getType() == SRDAG_FORK) {
            SRDAGVertex *firFork = secFork->getInEdge(0)->getSrc();
            if (firFork && firFork->getState() == SRDAG_EXEC && firFork->getType() == SRDAG_FORK) {
                int nbToAdd = secFork->getNConnectedOutEdge();
                int ixToAdd = secFork->getInEdge(0)->getSrcPortIx();

                int nbTotEdges = firFork->getNConnectedOutEdge() + nbToAdd - 1;

                // Shift edges after
                for (int k = nbTotEdges - 1; k > ixToAdd + nbToAdd - 1; k--) {
                    SRDAGEdge *edge = firFork->getOutEdge(k - nbToAdd + 1);
                    edge->disconnectSrc();
                    edge->connectSrc(firFork, k);
                }

                SRDAGEdge *delEdge = firFork->getOutEdge(ixToAdd);
                delEdge->disconnectSrc();
                topDag->delEdge(delEdge);

                // Add edges
                for (int k = 0; k < nbToAdd; k++) {
                    SRDAGEdge *edge = secFork->getOutEdge(k);
                    edge->disconnectSrc();
                    edge->connectSrc(firFork, k + ixToAdd);
                }

                topDag->delVertex(secFork);

                forkforkUsed = true;
                return 1;
            }
        }
    }
    return 0;
}

static int reduceBrBr(SRDAGGraph *topDag) {
    for (int i = 0; i < topDag->getNVertex(); i++) {
        SRDAGVertex *secBr = topDag->getVertex(i);
        if (secBr && secBr->getType() == SRDAG_BROADCAST) {
            SRDAGVertex *firBr = secBr->getInEdge(0)->getSrc();
            if (firBr && firBr->getState() == SRDAG_EXEC && firBr->getType() == SRDAG_BROADCAST) {
                int nbToAdd = secBr->getNConnectedOutEdge();
                int ixToAdd = secBr->getInEdge(0)->getSrcPortIx();

                SRDAGEdge *delEdge = firBr->getOutEdge(ixToAdd);
                delEdge->disconnectSrc();
                topDag->delEdge(delEdge);

                /* Add first edge where the previous one was */
                {
                    SRDAGEdge *edge = secBr->getOutEdge(0);
                    edge->disconnectSrc();
                    edge->connectSrc(firBr, ixToAdd);
                }

                /* Add others at the end of the firBr */
                ixToAdd = firBr->getNConnectedOutEdge();
                for (int k = 1; k < nbToAdd; k++) {
                    SRDAGEdge *edge = secBr->getOutEdge(k);
                    edge->disconnectSrc();
                    edge->connectSrc(firBr, k - 1 + ixToAdd);
                }

                topDag->delVertex(secBr);

                brbrUsed = true;
                return 1;
            }
        }
    }
    return 0;
}

static bool removeBr(SRDAGGraph *topDag) {
    bool result = false;
    for (int i = 0; i < topDag->getNVertex(); i++) {
        SRDAGVertex *br = topDag->getVertex(i);
        if (br && br->getState() == SRDAG_EXEC && br->getType() == SRDAG_BROADCAST) {
            for (int j = 0; j < br->getNConnectedOutEdge(); j++) {
                SRDAGEdge *delEdge = br->getOutEdge(j);
                SRDAGVertex *endVertex = delEdge->getSnk();
                if (endVertex && endVertex->getType() == SRDAG_END &&
                    endVertex->getInParam(0) == PISDF_DELAY_NONPERSISTENT) {
                    int nbOutput = br->getNConnectedOutEdge();

                    delEdge->disconnectSrc();
                    delEdge->disconnectSnk();
                    topDag->delEdge(delEdge);
                    topDag->delVertex(endVertex);

                    for (int k = j + 1; k < nbOutput; k++) {
                        SRDAGEdge *edge = br->getOutEdge(k);
                        edge->disconnectSrc();
                        edge->connectSrc(br, k - 1);
                    }

                    result = true;
                }
            }

            if (br->getNConnectedOutEdge() == 1 && br->getOutEdge(0)->getSnk()) {
                SRDAGEdge *inEdge = br->getInEdge(0);
                SRDAGVertex *nextVertex = br->getOutEdge(0)->getSnk();
                int edgeIx = br->getOutEdge(0)->getSnkPortIx();

                SRDAGEdge *delEdge = br->getOutEdge(0);
                delEdge->disconnectSrc();
                delEdge->disconnectSnk();
                topDag->delEdge(delEdge);

                inEdge->disconnectSnk();
                inEdge->connectSnk(nextVertex, edgeIx);
                topDag->delVertex(br);

                brUsed = true;
                return true;
            } else if (br->getNConnectedOutEdge() == 0) {
                SRDAGVertex *end = topDag->addEnd();
                SRDAGEdge *edge = br->getInEdge(0);
                end->setState(SRDAG_EXEC);

                edge->disconnectSnk();
                edge->connectSnk(end, 0);

                topDag->delVertex(br);
                brUsed = true;
                return true;
            }

            if (result) {
                brUsed = true;
                return true;
            }
        }
    }
    return result;
}

static bool removeFork(SRDAGGraph *topDag) {
    bool result = false;
    for (int i = 0; i < topDag->getNVertex(); i++) {
        SRDAGVertex *fork = topDag->getVertex(i);
        if (fork && fork->getState() == SRDAG_EXEC && fork->getType() == SRDAG_FORK) {
            for (int j = 0; j < fork->getNConnectedOutEdge(); j++) {
                SRDAGEdge *delEdge = fork->getOutEdge(j);
                SRDAGVertex *nextVertex = delEdge->getSnk();
                if (nextVertex && delEdge->getRate() == 0) {
                    switch (nextVertex->getType()) {
                        case SRDAG_END: {
                            int nbOutput = fork->getNConnectedOutEdge();
                            delEdge->disconnectSrc();
                            delEdge->disconnectSnk();
                            topDag->delEdge(delEdge);
                            topDag->delVertex(nextVertex);

                            for (int k = j + 1; k < nbOutput; k++) {
                                SRDAGEdge *edge = fork->getOutEdge(k);
                                edge->disconnectSrc();
                                edge->connectSrc(fork, k - 1);
                            }
                            result = true;
                            break;
                        }
                        case SRDAG_JOIN: {
                            int forkNOut = fork->getNConnectedOutEdge();
                            int joinPortIx = delEdge->getSnkPortIx();
                            int joinNIn = nextVertex->getNConnectedInEdge();
                            delEdge->disconnectSrc();
                            delEdge->disconnectSnk();
                            topDag->delEdge(delEdge);

                            /* Shift on Fork */
                            for (int k = j + 1; k < forkNOut; k++) {
                                SRDAGEdge *edge = fork->getOutEdge(k);
                                edge->disconnectSrc();
                                edge->connectSrc(fork, k - 1);
                            }

                            /* Shift on Join */
                            for (int k = joinPortIx + 1; k < joinNIn; k++) {
                                SRDAGEdge *edge = nextVertex->getInEdge(k);
                                edge->disconnectSnk();
                                edge->connectSnk(nextVertex, k - 1);
                            }

                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            if (fork->getNConnectedOutEdge() == 1 && fork->getOutEdge(0)->getSnk()) {
                SRDAGEdge *inEdge = fork->getInEdge(0);
                SRDAGVertex *nextVertex = fork->getOutEdge(0)->getSnk();
                int edgeIx = fork->getOutEdge(0)->getSnkPortIx();

                SRDAGEdge *delEdge = fork->getOutEdge(0);
                delEdge->disconnectSrc();
                delEdge->disconnectSnk();
                topDag->delEdge(delEdge);

                inEdge->disconnectSnk();
                inEdge->connectSnk(nextVertex, edgeIx);
                topDag->delVertex(fork);

                forkUsed = true;
                return true;
            } else if (fork->getNConnectedOutEdge() == 0) {
                SRDAGVertex *end = topDag->addEnd();
                SRDAGEdge *edge = fork->getInEdge(0);
                end->setState(SRDAG_EXEC);

                edge->disconnectSnk();
                edge->connectSnk(end, 0);

                topDag->delVertex(fork);
                forkUsed = true;
                return true;
            }

            if (result) {
                forkUsed = true;
                return true;
            }
        }
    }
    return result;
}

static bool removeJoin(SRDAGGraph *topDag) {
    bool result = false;
    for (int i = 0; i < topDag->getNVertex(); i++) {
        SRDAGVertex *join = topDag->getVertex(i);
        if (join && join->getState() == SRDAG_EXEC && join->getType() == SRDAG_JOIN) {
            if (join->getNConnectedInEdge() == 1 && join->getInEdge(0)->getSrc()) {
                SRDAGEdge *inEdge = join->getInEdge(0);
                SRDAGVertex *nextVertex = join->getOutEdge(0)->getSnk();
                int edgeIx = join->getOutEdge(0)->getSnkPortIx();

                SRDAGEdge *delEdge = join->getOutEdge(0);
                delEdge->disconnectSrc();

                if (delEdge->getSnk()) {
                    delEdge->disconnectSnk();
                }

                topDag->delEdge(delEdge);
                inEdge->disconnectSnk();

                if (nextVertex)
                    inEdge->connectSnk(nextVertex, edgeIx);

                topDag->delVertex(join);

                joinUsed = true;
                return true;
            } else if (join->getNConnectedInEdge() == 0) {
                SRDAGVertex *end = topDag->addEnd();
                SRDAGEdge *edge = join->getInEdge(0);
                end->setState(SRDAG_EXEC);

                edge->disconnectSnk();
                edge->connectSnk(end, 0);

                topDag->delVertex(join);
                joinUsed = true;
                return true;
            }

            if (result) {
                joinUsed = true;
                return true;
            }
        }
    }
    return result;
}

static int reduceInitEnd(SRDAGGraph *topDag) {
    for (int i = 0; i < topDag->getNVertex(); i++) {
        SRDAGVertex *init = topDag->getVertex(i);
        if (init && init->getState() == SRDAG_EXEC && init->getType() == SRDAG_INIT) {
            SRDAGVertex *end = init->getOutEdge(0)->getSnk();
            if (end && end->getState() == SRDAG_EXEC && end->getType() == SRDAG_END) {
                SRDAGEdge *edge = init->getOutEdge(0);
                edge->disconnectSrc();
                edge->disconnectSnk();
                topDag->delEdge(edge);
                topDag->delVertex(end);
                topDag->delVertex(init);
                initendUsed = true;
                return 1;
            }
        }
    }
    return 0;
}

static int reduceJoinEnd(SRDAGGraph *topDag) {
    for (int i = 0; i < topDag->getNVertex(); i++) {
        SRDAGVertex *join = topDag->getVertex(i);
        if (join && join->getState() == SRDAG_EXEC && join->getType() == SRDAG_JOIN) {
            SRDAGVertex *end = join->getOutEdge(0)->getSnk();
            if (end && end->getState() == SRDAG_EXEC && end->getType() == SRDAG_END) {
                int nEdges = join->getNConnectedInEdge();
                // Check if the End was linked to a persistent delay or not
                bool isPersistent = end->getInParam(0) == PISDF_DELAY_PERSISTENT;
                int memAllocStart = -1;
                if (isPersistent) {
                    memAllocStart = end->getInParam(1);
                }
                for (int j = 0; j < nEdges; j++) {
                    SRDAGEdge *edge = join->getInEdge(j);
                    SRDAGVertex *newEnd = topDag->addEnd();
                    newEnd->setState(SRDAG_EXEC);
                    edge->disconnectSnk();
                    edge->connectSnk(newEnd, 0);
                    newEnd->addInParam(0, isPersistent);
                    // If the End was not persistent, it will be useless info anyway
                    newEnd->addInParam(1, memAllocStart);
                    memAllocStart += edge->getRate();
                }

                SRDAGEdge *edge = join->getOutEdge(0);
                edge->disconnectSrc();
                edge->disconnectSnk();
                topDag->delEdge(edge);
                topDag->delVertex(end);
                topDag->delVertex(join);
                return 1;
            }
        }
    }
    return 0;
}

static int reduceJoinFork(SRDAGGraph *topDag) {
    for (int i = 0; i < topDag->getNVertex(); i++) {
        SRDAGVertex *join = topDag->getVertex(i);
        if (join && join->getType() == SRDAG_JOIN && join->getState() != SRDAG_RUN) {
            SRDAGVertex *fork = join->getOutEdge(0)->getSnk();
            if (fork && fork->getType() == SRDAG_FORK && fork->getState() != SRDAG_RUN) {
                int totalNbTokens = 0;
                int totProd = 0;

                int sourceIndex = 0, sinkIndex = 0;

                SRDAGVertex *sources[MAX_IO_EDGES];
                int sourceProds[MAX_IO_EDGES];
                int sourcePortIx[MAX_IO_EDGES];
                bool sourceExplode[MAX_IO_EDGES];
                int nbSourceRepetitions = join->getNConnectedInEdge();

                topDag->delEdge(join->getOutEdge(0));

                for (int k = 0; k < nbSourceRepetitions; k++) {
                    SRDAGEdge *edge = join->getInEdge(k);
                    sources[k] = edge->getSrc();
                    sourceProds[k] = edge->getRate();
                    sourcePortIx[k] = edge->getSrcPortIx();

                    topDag->delEdge(edge);
                    totalNbTokens += sourceProds[k];
                    sourceExplode[k] = false;
                }
                topDag->delVertex(join);

                SRDAGVertex *sinks[MAX_IO_EDGES];
                int sinkCons[MAX_IO_EDGES];
                int sinkPortIx[MAX_IO_EDGES];
                int nbSinkRepetitions = fork->getNConnectedOutEdge();
                bool sinkImplode[MAX_IO_EDGES];
                for (int k = 0; k < nbSinkRepetitions; k++) {
                    SRDAGEdge *edge = fork->getOutEdge(k);
                    sinks[k] = edge->getSnk();
                    sinkCons[k] = edge->getRate();
                    sinkPortIx[k] = edge->getSnkPortIx();

                    topDag->delEdge(edge);
                    sinkImplode[k] = false;
                }
                topDag->delVertex(fork);

                int curProd = sourceProds[0];
                int curCons = sinkCons[0];

                while (totProd < totalNbTokens) {
                    // Production/consumption rate for the current source/target.
                    int rest = (curProd > curCons) ? curCons : curProd;

                    /*
                     * Adding explode/implode vertices if required.
                     */

                    if (rest < curProd && !sourceExplode[sourceIndex]) {
                        // Adding an explode vertex.
                        SRDAGVertex *fork_vertex = topDag->addFork(MAX_IO_EDGES);
                        fork_vertex->setState(SRDAG_EXEC);

                        // Replacing the source vertex by the explode vertex in the array of sources.
                        SRDAGVertex *origin_vertex = sources[sourceIndex];
                        sources[sourceIndex] = fork_vertex;
                        sourceExplode[sourceIndex] = true;

                        // Adding an edge between the source and the explode.
                        topDag->addEdge(
                                origin_vertex, sourcePortIx[sourceIndex],
                                fork_vertex, 0,
                                sourceProds[sourceIndex]);
                    }

                    if (rest < curCons && !sinkImplode[sinkIndex]) {
                        // Adding an implode vertex.
                        SRDAGVertex *join_vertex = topDag->addJoin(MAX_IO_EDGES);
                        join_vertex->setState(SRDAG_EXEC);

                        // Replacing the sink vertex by the implode vertex in the array of sources.
                        SRDAGVertex *origin_vertex = sinks[sinkIndex];//	// Adding vxs
                        sinks[sinkIndex] = join_vertex;
                        sinkImplode[sinkIndex] = true;

                        // Adding an edge between the implode and the sink.
                        topDag->addEdge(
                                join_vertex, 0,
                                origin_vertex, sinkPortIx[sinkIndex],
                                sinkCons[sinkIndex]);
                    }

                    //Creating the new edge between normal vertices or between a normal and an explode/implode one.
                    int sourcePortId, sinkPortId;
                    if (sourceExplode[sourceIndex]) {
                        sourcePortId = sources[sourceIndex]->getNConnectedOutEdge();
                    } else {
                        sourcePortId = sourcePortIx[sourceIndex];
                    }

                    if (sinkImplode[sinkIndex])
                        sinkPortId = sinks[sinkIndex]->getNConnectedInEdge();
                    else
                        sinkPortId = sinkPortIx[sinkIndex];


                    topDag->addEdge(
                            sources[sourceIndex], sourcePortId,
                            sinks[sinkIndex], sinkPortId,
                            rest);

                    // Update the totProd for the current edge (totProd is used in the condition of the While loop)
                    totProd += rest;

                    curCons -= rest;
                    curProd -= rest;

                    if (curProd == 0) {
                        curProd += sourceProds[++sourceIndex];
                    }
                    if (curCons == 0) {
                        curCons += sinkCons[++sinkIndex];
                    }
                }

                if (sourceIndex != nbSourceRepetitions) {
                    /* Check Unhandled vertices */
                    /* Shift on Fork */
                    for (int i = sourceIndex; i < nbSourceRepetitions; i++) {
                        if (sources[i]->getType() == SRDAG_FORK) {
                            SRDAGVertex *rem_fork = sources[i];
                            for (int j = 0; j < rem_fork->getNConnectedOutEdge(); j++) {
                                if (rem_fork->getOutEdge(j) == nullptr) {
                                    for (int k = j + 1; k < rem_fork->getNOutEdge(); k++) {
                                        SRDAGEdge *edge = rem_fork->getOutEdge(k);
                                        if (edge) {
                                            edge->disconnectSrc();
                                            edge->connectSrc(rem_fork, k - 1);
                                        }
                                    }
                                }
                            }
                        } else {
                            throw std::runtime_error("A non-End vertex have a cons of 0\n");
                        }
                    }
                }
                if (sinkIndex != nbSinkRepetitions) {
                    /* Check Unhandled vertices */
                    for (int i = sinkIndex; i < nbSinkRepetitions; i++) {
                        if (sinks[i]->getType() == SRDAG_END) {
                            topDag->delVertex(sinks[i]);
                        } else {
                            throw std::runtime_error("A non-End vertex have a cons of 0\n");
                        }
                    }
                }

                joinforkUsed = true;
                return 1;
            }
        }
    }
    return 0;
}

void optims(SRDAGGraph *topDag) {
    bool res, resTotal;
    do {
        res = false;
        resTotal = false;

        do {
            res = removeNullEdge(topDag);
            resTotal = res || resTotal;
        } while (res);

        do {
            res = reduceRB(topDag);
            resTotal = res || resTotal;
        } while (res);

        do {
            res = reduceJoinEnd(topDag);
            resTotal = res || resTotal;
        } while (res);

        do {
            res = reduceJoinJoin(topDag);
            resTotal = res || resTotal;
        } while (res);

        do {
            res = reduceForkFork(topDag);
            resTotal = res || resTotal;
        } while (res);

        do {
            res = reduceBrBr(topDag);
            resTotal = res || resTotal;
        } while (res);

        do {
            res = reduceJoinFork(topDag);
            resTotal = res || resTotal;
        } while (res);

        do {
            res = removeBr(topDag);
            resTotal = res || resTotal;
        } while (res);

        do {
            res = removeFork(topDag);
            resTotal = res || resTotal;
        } while (res);

        do {
            res = removeJoin(topDag);
            resTotal = res || resTotal;
        } while (res);

        do {
            res = reduceInitEnd(topDag);
            resTotal = res || resTotal;
        } while (res);

    } while (res);
}

