/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
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
#include "LinkVertices.h"

#include <graphs/PiSDF/PiSDFEdge.h>

#include <cmath>

typedef enum {
    VERTEX, EDGE
} nodeType;

typedef struct node {
    int nb;
    int rates[MAX_IO_EDGES];
    nodeType type;
    //todo union ?
    struct {
        SRDAGVertex *ptr;
        int portIx;
    } vertex;
    SRDAGEdge *edge;
} node;


void linkCAVertices(SRDAGGraph *topSrdag, transfoJob *job) {
    for (int sr_ca_Ix = 0; sr_ca_Ix < job->graph->getNConfig(); sr_ca_Ix++) {
        SRDAGVertex *sr_ca = job->configs[sr_ca_Ix];
        PiSDFVertex *pi_ca = sr_ca->getReference();

        for (int inEdgeIx = 0; inEdgeIx < pi_ca->getNInEdge(); inEdgeIx++) {
            if (sr_ca->getInEdge(inEdgeIx) != nullptr) {
                /* Edge already present, do nothing */
            } else {
                PiSDFEdge *pi_edge = pi_ca->getInEdge(inEdgeIx);
                PiSDFVertex *pi_src = pi_edge->getSrc();
                int cons = pi_edge->resolveCons(job);
                int prod = pi_edge->resolveProd(job);

                switch (pi_src->getType()) {
                    case PISDF_TYPE_IF: {
                        if (pi_src->getSubType() != PISDF_SUBTYPE_INPUT_IF) {
                            throwSpiderException("Output interface connected to ConfigActor input port.");
                        }

                        SRDAGEdge *srcEdge = job->inputIfs[pi_src->getTypeId()];
                        if (srcEdge->getRate() == cons) {
                            srcEdge->connectSnk(sr_ca, inEdgeIx);
                        } else {
                            /* For now create RB, can do better to reduce RBs */
                            SRDAGVertex *sr_rb = topSrdag->addRoundBuffer();
                            srcEdge->connectSnk(sr_rb, 0);

                            SRDAGEdge *newEdge = topSrdag->addEdge();
                            newEdge->connectSrc(sr_rb, 0);
                            newEdge->connectSnk(sr_ca, inEdgeIx);
                            newEdge->setRate(cons);
                        }
                        break;
                    }
                    case PISDF_TYPE_CONFIG: {
                        SRDAGVertex *sr_src = job->configs[pi_src->getTypeId()];
                        SRDAGEdge *sr_edge = sr_src->getOutEdge(pi_edge->getSrcPortIx());

                        if (sr_edge == nullptr) {
                            sr_edge = topSrdag->addEdge();
                            sr_edge->connectSrc(sr_src, pi_edge->getSrcPortIx());
                            sr_edge->setRate(prod);
                        }

                        sr_edge->connectSnk(sr_ca, inEdgeIx);
                        if (prod != cons) {
                            throwSpiderException("Prod/Cons between ConfigActors [%s] and [%s] mismatch.",
                                                 pi_src->getName(), pi_ca->getName());
                        }
                        break;
                    }
                    default:
                        throwSpiderException(
                                "Normal vertex [%s] can not be connected to ConfigActor [%s] input port(s).",
                                pi_src->getName(), pi_ca->getName());
                }
            }
        }

        for (int outEdgeIx = 0; outEdgeIx < pi_ca->getNOutEdge(); outEdgeIx++) {
            if (sr_ca->getOutEdge(outEdgeIx) != nullptr) {
                /* Edge already present, do nothing */
            } else {
                PiSDFEdge *pi_edge = pi_ca->getOutEdge(outEdgeIx);
                PiSDFVertex *pi_snk = pi_edge->getSnk();
                int cons = pi_edge->resolveCons(job);
                int prod = pi_edge->resolveProd(job);

                switch (pi_snk->getType()) {
                    case PISDF_TYPE_IF: {
                        if (pi_snk->getSubType() != PISDF_SUBTYPE_OUTPUT_IF) {
                            throwSpiderException("Input interface connected to ConfigActor output port.");
                        }

                        SRDAGEdge *snkEdge = job->outputIfs[pi_snk->getTypeId()];
                        if (snkEdge->getRate() == prod) {
                            snkEdge->connectSnk(sr_ca, outEdgeIx);
                        } else {
                            /* For now create RB, can do better to reduce RBs */
                            SRDAGVertex *sr_rb = topSrdag->addRoundBuffer();
                            snkEdge->connectSrc(sr_rb, 0);

                            SRDAGEdge *newEdge = topSrdag->addEdge();
                            newEdge->connectSrc(sr_ca, outEdgeIx);
                            newEdge->connectSnk(sr_rb, 0);
                            newEdge->setRate(prod);
                        }
                        break;
                    }
                    case PISDF_TYPE_CONFIG: {
                        SRDAGVertex *sr_snk = job->configs[pi_snk->getTypeId()];
                        SRDAGEdge *sr_edge = sr_snk->getInEdge(pi_edge->getSnkPortIx());

                        if (sr_edge == nullptr) {
                            sr_edge = topSrdag->addEdge();
                            sr_edge->connectSnk(sr_snk, pi_edge->getSnkPortIx());
                            sr_edge->setRate(cons);
                        }

                        sr_edge->connectSrc(sr_ca, outEdgeIx);

                        if (cons != prod) {
                            throwSpiderException("Prod/Cons between ConfigActors [%s] and [%s] mismatch.",
                                                 pi_ca->getName(), pi_snk->getName());
                        }
                        break;
                    }
                    case PISDF_TYPE_BODY: {
                        SRDAGEdge *sr_edge = topSrdag->addEdge();
                        sr_edge->connectSrc(sr_ca, outEdgeIx);
                        sr_edge->setRate(prod);
                        break;
                    }
                    default:
                        throwSpiderException("Unhandled case.");
                }
            }
        }
    }
}


typedef struct SrcConnection {
    SRDAGVertex *src;
    Param prod;
    int portIx;
} SrcConnection;

typedef struct SnkConnection {
    SRDAGEdge *edge;
    Param cons;
} SnkConnection;

/**
 * @brief Add all instances of the source vertex of a given edge to the SrcConnection structure
 *
 * @param job                 The job
 * @param edge                The original PiSDF edge
 * @param srcConnections      The srcConnections array of SrcConnection strutures
 * @param nbSourceRepetitions Repetition vector value of the source vertex
 * @param sourceProduction    Production value of the source
 */
static inline void addSRSourceVertices(transfoJob *job,
                                       PiSDFEdge *edge,
                                       SrcConnection *srcConnections,
                                       int nbSourceRepetitions,
                                       int sourceProduction) {
    int srcID = edge->getSrc()->getTypeId();
    int srcPortIx = edge->getSrcPortIx();
    for (int i = 0; i < nbSourceRepetitions; i++) {
        srcConnections[i].src = job->bodies[srcID][i];
        srcConnections[i].portIx = srcPortIx;
        srcConnections[i].prod = sourceProduction;
    }
}

/**
 * @brief Add all instances of the setter vertex of a delay to the single rate top graph
 *
 * @param topSrdag       The top sr-dag graph
 * @param job            The job
 * @param brv            The repetition vector values
 * @param edge           The original PiSDF edge
 * @param srcConnections The srcConnections array of SrcConnection strutures
 */
static void linkSRDelaySetterVertices(SRDAGGraph *topSrdag,
                                      transfoJob *job,
                                      int *brv,
                                      PiSDFEdge *edge,
                                      SrcConnection *srcConnections) {
    // Value of the delay
    int nbDelays = edge->resolveDelay(job);
    // Setter actor that initialize the data tokens of the delay
    PiSDFVertex *delaySetter = edge->getDelaySetter();
    // Virtual actor used for analyses purposes
    PiSDFVertex *delayVirtual = edge->getDelayVirtual();
    // Check consistency through the value of the BRV of the virtual actor
    if (brv[delayVirtual->getTypeId()] > 1) {
        throwSpiderException("Delay [%s] -- RV(%d) > 1.", delayVirtual->getName(), brv[delayVirtual->getTypeId()]);
    }
    // If the setter is an interface, we should just retrieve the connection
    if (delaySetter->getType() == PISDF_TYPE_IF) {
        SRDAGEdge *setterEdge = job->inputIfs[delaySetter->getTypeId()];
        if (setterEdge->getRate() == nbDelays) {
            srcConnections[0].src = setterEdge->getSrc();
            if (srcConnections[0].src == nullptr) {
                srcConnections[0].src = topSrdag->addRoundBuffer();
                job->inputIfs[edge->getSrc()->getTypeId()]->connectSnk(srcConnections[0].src, 0);
                srcConnections[0].portIx = 0;
            } else {
                srcConnections[0].portIx = setterEdge->getSrcPortIx();
            }
        } else {
            throwSpiderException("Setter [%d] of delay [%s] does not have proper rate.", delaySetter->getName(),
                                 delayVirtual->getName());
        }
    } else {
        int nbSetterRepetitions = brv[delaySetter->getTypeId()];
        int setterPortIx = delayVirtual->getInEdge(0)->getSrcPortIx();
        srcConnections[0].prod = nbDelays;
        if (nbSetterRepetitions > 1) {
            // We need to add a join
            srcConnections[0].src = topSrdag->addJoin(MAX_IO_EDGES);
            srcConnections[0].portIx = 0;

            int setterProd = nbDelays / nbSetterRepetitions;
            for (int j = 0; j < nbSetterRepetitions; ++j) {
                topSrdag->addEdge(job->bodies[delaySetter->getTypeId()][j],
                                  setterPortIx,
                                  srcConnections[0].src,
                                  j,
                                  setterProd);
            }
        } else {
            // We don't need to add a join
            srcConnections[0].src = job->bodies[delaySetter->getTypeId()][0];
            srcConnections[0].portIx = setterPortIx;
        }
    }
}

/**
 * @brief Add all instances of the sink vertex of a given edge to the SnkConnection structure
 *
 * @param job                 The job
 * @param edge                The original PiSDF edge
 * @param snkConnections      The snkConnections array of SnkConnection strutures
 * @param nbSourceRepetitions Repetition vector value of the sink vertex
 * @param sinkConsumption     Consumption value of the sink
 */
static inline void addSRSinkVertices(SRDAGGraph *topSrdag,
                                     transfoJob *job,
                                     PiSDFEdge *edge,
                                     SnkConnection *snkConnections,
                                     int nbSinkRepetitions,
                                     int sinkConsumption) {
    int snkID = edge->getSnk()->getTypeId();
    int snkPortIx = edge->getSnkPortIx();
    for (int i = 0; i < nbSinkRepetitions; i++) {
        snkConnections[i].edge = topSrdag->addEdge();
        snkConnections[i].edge->connectSnk(job->bodies[snkID][i], snkPortIx);
        snkConnections[i].cons = sinkConsumption;
    }
}


/**
 * @brief Add all instances of the getter vertex of a delay to the single rate top graph
 *
 * @param topSrdag       The top sr-dag graph
 * @param job            The job
 * @param brv            The repetition vector values
 * @param edge           The original PiSDF edge
 * @param snkConnections The snkConnections array of SnkConnection strutures
 */
static void linkSRDelayGetterVertices(SRDAGGraph *topSrdag,
                                      transfoJob *job,
                                      int *brv,
                                      PiSDFEdge *edge,
                                      SnkConnection *snkConnections) {
    // Value of the delay
    int nbDelays = edge->resolveDelay(job);
    // Setter actor that initialize the data tokens of the delay
    PiSDFVertex *delayGetter = edge->getDelayGetter();
    // Virtual actor used for analyses purposes
    PiSDFVertex *delayVirtual = edge->getDelayVirtual();
    // Check consistency through the value of the BRV of the virtual actor
    if (brv[delayVirtual->getTypeId()] > 1) {
        throwSpiderException("Delay [%s] -- RV(%d) > 1.", delayVirtual->getName(), brv[delayVirtual->getTypeId()]);
    }
    // If the setter is an interface, we should just retrieve the connection
    if (delayGetter->getType() == PISDF_TYPE_IF) {
        SRDAGEdge *getterEdge = job->outputIfs[delayGetter->getTypeId()];
        if (getterEdge->getRate() == nbDelays) {
            snkConnections[0].edge = getterEdge;
            snkConnections[0].cons = nbDelays;
        } else {
            throwSpiderException("Getter [%d] of delay [%s] does not have proper rate.", delayGetter->getName(),
                                 delayVirtual->getName());
        }
    } else {
        int nbGetterRepetitions = brv[delayGetter->getTypeId()];
        int getterPortIx = delayVirtual->getOutEdge(0)->getSnkPortIx();
        snkConnections[0].edge = topSrdag->addEdge();
        snkConnections[0].cons = nbDelays;
        if (nbGetterRepetitions > 1) {
            // We need to add a fork
            SRDAGVertex *forkVertex = topSrdag->addFork(MAX_IO_EDGES);
            snkConnections[0].edge->connectSnk(forkVertex, 0);

            int getterProd = nbDelays / nbGetterRepetitions;
            for (int j = 0; j < nbGetterRepetitions; ++j) {
                topSrdag->addEdge(forkVertex,
                                  j,
                                  job->bodies[delayGetter->getTypeId()][j],
                                  getterPortIx,
                                  getterProd);
            }
        } else {
            // We don't need to add a fork
            snkConnections[0].edge->connectSnk(job->bodies[delayGetter->getTypeId()][0], getterPortIx);
        }
    }
}


/**
 * @brief Link all vertices of current graph contained in the transformation job
 *
 * @param topSrdag The top sr-dag graph
 * @param job      The job
 * @param brv      The repetition vector values
 */
void linkSRVertices(SRDAGGraph *topSrdag,
                    transfoJob *job,
                    int *brv) {
    PiSDFGraph *currentPiSDF = job->graph;


    for (int edgeIx = 0; edgeIx < currentPiSDF->getNEdge(); edgeIx++) {
        PiSDFEdge *edge = currentPiSDF->getEdge(edgeIx);

        // Already treated edge
        if (edge->getSnk()->getType() == PISDF_TYPE_CONFIG)
            continue;

        // We deal with delay virtual actor specifically
        if (edge->getSnk()->getSubType() == PISDF_SUBTYPE_DELAY)
            continue;
        if (edge->getSrc()->getSubType() == PISDF_SUBTYPE_DELAY)
            continue;

        int nbDelays = edge->resolveDelay(job);
        int piSrcIx = edge->getSrcPortIx();

        int nbSourceRepetitions = 1;
        int nbSinkRepetitions = 1;

        // If source is an actor, get its repetition vector
        // Else if it is an interface, it is 1 by default
        if (edge->getSrc()->getType() == PISDF_TYPE_BODY) {
            nbSourceRepetitions = brv[edge->getSrc()->getTypeId()];
        }

        // If sink is an actor, get its repetition vector
        // Else if it is an interface, it is 1 by default
        if (edge->getSnk()->getType() == PISDF_TYPE_BODY) {
            nbSinkRepetitions = brv[edge->getSnk()->getTypeId()];
        }

        auto sourceProduction = edge->resolveProd(job);
        auto sinkConsumption = edge->resolveCons(job);

        // Unused edge
        if (nbSourceRepetitions == 0 && nbSinkRepetitions == 0)
            continue;

//        if (nbSourceRepetitions == 0) {
//            if (sinkConsumption != 0) {
//                throwSpiderException("Actor [%s] consumes tokens from non executed actor [%s].",
//                                     edge->getSnk()->getName(), edge->getSrc()->getName());
//            } else {
//                if (edge->getSnk()->getSubType() == PISDF_SUBTYPE_JOIN) {
//                    continue;
//                } else {
//                    /* Create an empty edge */
//                    for (int snkRep = 0; snkRep < nbSinkRepetitions; snkRep++) {
//                        SRDAGVertex *snk = job->bodies[edge->getSnk()->getTypeId()][snkRep];
//                        int portIx = edge->getSnkPortIx();
//                        SRDAGEdge *emptyEdge = topSrdag->addEdge();
//                        emptyEdge->setRate(0);
//                        emptyEdge->connectSnk(snk, portIx);
//                    }
//                    continue;
//                }
//            }
//        }
//        if (nbSinkRepetitions == 0) {
//            if (sourceProduction != 0) {
//                for (int srcRep = 0; srcRep < nbSourceRepetitions; srcRep++) {
//                    SRDAGVertex *src = job->bodies[edge->getSnk()->getTypeId()][srcRep];
//                    int portIx = edge->getSrcPortIx();
//                    SRDAGVertex *end = topSrdag->addEnd();
//                    SRDAGEdge *edge = topSrdag->addEdge();
//                    edge->setRate(sourceProduction);
//                    edge->connectSrc(src, portIx);
//                    edge->connectSnk(end, 0);
//                }
//            } else {
//                if (edge->getSrc()->getSubType() == PISDF_SUBTYPE_FORK
//                    || edge->getSrc()->getSubType() == PISDF_SUBTYPE_BROADCAST) {
//                    continue;
//                } else {
//                    /* Create an empty edge */
//                    for (int srcRep = 0; srcRep < nbSourceRepetitions; srcRep++) {
//                        SRDAGVertex *src = job->bodies[edge->getSnk()->getTypeId()][srcRep];
//                        int portIx = edge->getSrcPortIx();
//                        SRDAGEdge *emptyEdge = topSrdag->addEdge();
//                        emptyEdge->setRate(0);
//                        emptyEdge->connectSrc(src, portIx);
//                    }
//                    continue;
//                }
//            }
//        }

        if (sourceProduction == 0 && sinkConsumption == 0) {
            /* Put Empty Src Port */
            for (int srcRep = 0; srcRep < nbSourceRepetitions; srcRep++) {
                SRDAGVertex *src = job->bodies[edge->getSrc()->getTypeId()][srcRep];
                int portIx = edge->getSrcPortIx();
                SRDAGEdge *emptyEdge = topSrdag->addEdge();
                emptyEdge->setRate(0);
                emptyEdge->connectSrc(src, portIx);
            }
            /* Put Empty Snk Port */
            for (int snkRep = 0; snkRep < nbSinkRepetitions; snkRep++) {
                SRDAGVertex *snk = job->bodies[edge->getSnk()->getTypeId()][snkRep];
                int portIx = edge->getSnkPortIx();
                SRDAGEdge *emptyEdge = topSrdag->addEdge();
                emptyEdge->setRate(0);
                emptyEdge->connectSnk(snk, portIx);
            }
            continue;
        }

        int sourceIndex = 0;
        int sinkIndex = 0;
        int curSourceToken;
        int curSinkToken;
        SRDAGVertex *persistentInit = nullptr;
        SRDAGVertex *persistentEnd = nullptr;

        SrcConnection *srcConnections = nullptr;
        SnkConnection *snkConnections = nullptr;
        bool sinkNeedEnd = false;

        int forkIx = -1;
        int joinIx = -1;


        // Fill source/sink repetition list
        switch (edge->getSrc()->getType()) {
            case PISDF_TYPE_CONFIG:
                if (sourceProduction * 1 == sinkConsumption * nbSinkRepetitions) {
                    // No need of Broadcast
                    srcConnections = CREATE_MUL(TRANSFO_STACK, 1, SrcConnection);

                    srcConnections->src = job->configs[edge->getSrc()->getTypeId()];
                    srcConnections->prod = sourceProduction;
                    srcConnections->portIx = piSrcIx;
                } else {
                    bool perfectBr = sinkConsumption * nbSinkRepetitions % sourceProduction == 0;
                    int nBr = sinkConsumption * nbSinkRepetitions / sourceProduction;
                    if (!perfectBr) nBr++;

                    curSourceToken = sourceProduction;
                    nbSourceRepetitions = nBr;

                    if (perfectBr) {
                        SRDAGVertex *broadcast = topSrdag->addBroadcast(MAX_IO_EDGES);
                        SRDAGEdge *configEdge = job->configs[edge->getSrc()->getTypeId()]->getOutEdge(piSrcIx);

                        configEdge->connectSnk(broadcast, 0);

                        srcConnections = CREATE_MUL(TRANSFO_STACK, nBr, SrcConnection);
                        for (int i = 0; i < nBr; i++) {
                            srcConnections[i].src = broadcast;
                            srcConnections[i].portIx = i;
                            srcConnections[i].prod = sourceProduction;
                        }
                    } else {
//					lastCons = sourceProduction - sinkConsumption*nbSinkRepetitions;
                        sinkNeedEnd = true;

                        SRDAGVertex *broadcast = topSrdag->addBroadcast(MAX_IO_EDGES);
                        SRDAGEdge *configEdge = job->configs[edge->getSrc()->getTypeId()]->getOutEdge(piSrcIx);

                        configEdge->connectSnk(broadcast, 0);

                        srcConnections = CREATE_MUL(TRANSFO_STACK, nBr, SrcConnection);
                        for (int i = 0; i < nBr; i++) {
                            srcConnections[i].src = broadcast;
                            srcConnections[i].portIx = i;
                            srcConnections[i].prod = sourceProduction;
                        }
                    }
                }
                break;
            case PISDF_TYPE_IF:
                if (sourceProduction * 1 == sinkConsumption * nbSinkRepetitions) {
                    // No need of Broadcast
                    srcConnections = CREATE_MUL(TRANSFO_STACK, 1, SrcConnection);
                    srcConnections[0].src = job->inputIfs[edge->getSrc()->getTypeId()]->getSrc();
                    if (srcConnections[0].src == nullptr) {
                        srcConnections[0].src = topSrdag->addRoundBuffer();
                        job->inputIfs[edge->getSrc()->getTypeId()]->connectSnk(srcConnections[0].src, 0);
                        srcConnections[0].portIx = 0;
                        srcConnections[0].prod = sourceProduction;
                    } else {
                        srcConnections[0].portIx = job->inputIfs[edge->getSrc()->getTypeId()]->getSrcPortIx();
                        srcConnections[0].prod = sourceProduction;
                    }
                } else {
                    bool perfectBr = sinkConsumption * nbSinkRepetitions % sourceProduction == 0;
                    int nBr = sinkConsumption * nbSinkRepetitions / sourceProduction;
                    if (!perfectBr) nBr++;

                    nbSourceRepetitions = nBr;
                    sinkNeedEnd = !perfectBr;
//					lastCons = sourceProduction - sinkConsumption*nbSinkRepetitions;

                    SRDAGVertex *broadcast = topSrdag->addBroadcast(MAX_IO_EDGES);
                    job->inputIfs[edge->getSrc()->getTypeId()]->connectSnk(broadcast, 0);

                    srcConnections = CREATE_MUL(TRANSFO_STACK, nBr, SrcConnection);
                    for (int i = 0; i < nBr; i++) {
                        srcConnections[i].src = broadcast;
                        srcConnections[i].portIx = i;
                        srcConnections[i].prod = sourceProduction;
                    }
                }
                break;
            case PISDF_TYPE_BODY:
                if (nbDelays == 0) {
                    srcConnections = CREATE_MUL(TRANSFO_STACK, nbSourceRepetitions, SrcConnection);
                    // Add the source instances
                    addSRSourceVertices(job,                     /* Transformation job */
                                        edge,                    /* PiSDF original edge */
                                        &srcConnections[0],      /* Start from first position */
                                        nbSourceRepetitions,     /* BRV value of the source */
                                        sourceProduction);       /* Source production*/
                } else {
                    nbSourceRepetitions++;
                    srcConnections = CREATE_MUL(TRANSFO_STACK, nbSourceRepetitions, SrcConnection);
                    // 1. Deal with the delay initialization
                    if (edge->getDelaySetter()) {
                        if (edge->isDelayPersistent()) {
                            throwSpiderException("Delay with persistence can not have setter actor.");
                        }
                        // Add setter vertex instances
                        linkSRDelaySetterVertices(topSrdag,        /* Top graph */
                                                  job,             /* Transformation job */
                                                  brv,             /* Basic Repetition Vector values */
                                                  edge,            /* PiSDF original edge */
                                                  srcConnections); /* The SrcConnection structures */
                    } else {
                        // Add init vertex
                        srcConnections[0].src = topSrdag->addInit();
                        srcConnections[0].portIx = 0;
                        srcConnections[0].prod = nbDelays;
                        // Set persistence property
                        srcConnections[0].src->addInParam(0, edge->isDelayPersistent());
                        if (edge->isDelayPersistent()) {
                            // Set memory address of the delay
                            srcConnections[0].src->addInParam(1, edge->getDelayAlloc());
                            persistentInit = srcConnections[0].src;
                        }
                    }
                    // 2. Add the source instances
                    addSRSourceVertices(job,                     /* Transformation job */
                                        edge,                    /* PiSDF original edge */
                                        &srcConnections[1],      /* Start from second position */
                                        nbSourceRepetitions - 1, /* BRV value of the source */
                                        sourceProduction);       /* Source production*/
                }
                break;
        }

        switch (edge->getSnk()->getType()) {
            case PISDF_TYPE_CONFIG:
                throwSpiderException("Actor [%s] can not be connected to sink ConfigActor [%s].",
                                     edge->getSrc()->getName(), edge->getSnk()->getName());
            case PISDF_TYPE_IF:
                if (sinkConsumption * 1 == sourceProduction * nbSourceRepetitions) {
                    // No need of specific thing
                    snkConnections = CREATE_MUL(TRANSFO_STACK, 1, SnkConnection);
                    snkConnections[0].edge = job->outputIfs[edge->getSnk()->getTypeId()];
                    snkConnections[0].cons = sinkConsumption;
                } else {
                    float nDroppedTokens = sourceProduction * nbSourceRepetitions - sinkConsumption;
                    int nEnd = std::ceil(nDroppedTokens / sourceProduction);

                    nbSinkRepetitions = nEnd + 1;
                    snkConnections = CREATE_MUL(TRANSFO_STACK, nbSinkRepetitions, SnkConnection);
                    for (int i = 0; i < nEnd; i++) {
                        snkConnections[i].edge = topSrdag->addEdge();
                        snkConnections[i].edge->connectSnk(topSrdag->addEnd(), 0);
                        snkConnections[i].cons = sourceProduction;
                    }
                    snkConnections[nEnd - 1].cons = nDroppedTokens - (nEnd - 1) * sourceProduction;

                    snkConnections[nEnd].edge = job->outputIfs[edge->getSnk()->getTypeId()];
                    snkConnections[nEnd].cons = sinkConsumption;
                }
                break;
            case PISDF_TYPE_BODY:
                if (nbDelays == 0) {
                    if (sinkNeedEnd) {
                        snkConnections = CREATE_MUL(TRANSFO_STACK, nbSinkRepetitions + 1, SnkConnection);
                        // 1. Add sink instances
                        addSRSinkVertices(topSrdag,          /* Top graph */
                                          job,               /* Transformation job */
                                          edge,              /* PiSDF original edge */
                                          snkConnections,    /* Start from first position */
                                          nbSinkRepetitions, /* BRV value of the sink */
                                          sinkConsumption);  /* Sink consumption*/
                        // 2. Add end vertex
                        snkConnections[nbSinkRepetitions].edge = topSrdag->addEdge();
                        snkConnections[nbSinkRepetitions].edge->connectSnk(topSrdag->addEnd(), 0);
                        snkConnections[nbSinkRepetitions].cons = sourceProduction - sinkConsumption * nbSinkRepetitions;
                        nbSinkRepetitions++;
                    } else {
                        snkConnections = CREATE_MUL(TRANSFO_STACK, nbSinkRepetitions, SnkConnection);
                        // Add sink instances
                        addSRSinkVertices(topSrdag,          /* Top graph */
                                          job,               /* Transformation job */
                                          edge,              /* PiSDF original edge */
                                          snkConnections,    /* Start from first position */
                                          nbSinkRepetitions, /* BRV value of the sink */
                                          sinkConsumption);  /* Sink consumption*/
                    }
                } else {
                    snkConnections = CREATE_MUL(TRANSFO_STACK, nbSinkRepetitions + 1, SnkConnection);
                    // 1. Add sink instances
                    addSRSinkVertices(topSrdag,          /* Top graph */
                                      job,               /* Transformation job */
                                      edge,              /* PiSDF original edge */
                                      snkConnections,    /* Start from first position */
                                      nbSinkRepetitions, /* BRV value of the sink */
                                      sinkConsumption);  /* Sink consumption*/

                    // 2. Deal with the delay end
                    if (edge->getDelayGetter()) {
                        if (edge->isDelayPersistent()) {
                            throwSpiderException("Delay with persistence can not have getter actor.");
                        }
                        // Add getter vertex instances
                        linkSRDelayGetterVertices(topSrdag,                            /* Top graph */
                                                  job,                                 /* Transformation job */
                                                  brv,                                 /* Basic Repetition Vector values */
                                                  edge,                                /* PiSDF original edge */
                                                  &snkConnections[nbSinkRepetitions]); /* The SnkConnection structure */
                    } else {
                        // Add end vertex
                        snkConnections[nbSinkRepetitions].edge = topSrdag->addEdge();
                        SRDAGVertex *endVertex = topSrdag->addEnd();
                        snkConnections[nbSinkRepetitions].edge->connectSnk(endVertex, 0);
                        snkConnections[nbSinkRepetitions].cons = nbDelays;
                        // Set persistence property
                        endVertex->addInParam(0, edge->isDelayPersistent());
                        if (edge->isDelayPersistent()) {
                            // Set memory address of the delay
                            endVertex->addInParam(1, edge->getDelayAlloc());
                            persistentEnd = endVertex;
                        }
                    }
                    // Update the number of sink
                    nbSinkRepetitions++;
                }
                break;
        }

        curSourceToken = srcConnections[0].prod;
        curSinkToken = snkConnections[0].cons;
        // Iterating until all consumptions are "satisfied".
        while (sourceIndex < nbSourceRepetitions
               || sinkIndex < nbSinkRepetitions) {
            // Production/consumption rate for the current source/target.
            int rest = std::min(curSourceToken, curSinkToken); // Minimum.
            /*
             * Adding explode/implode vertices if required.
             */

            if (rest < curSourceToken
                && (srcConnections[sourceIndex].src->getId() != forkIx)) {
                // Adding an explode vertex.
                SRDAGVertex *fork_vertex = topSrdag->addFork(MAX_IO_EDGES);
                forkIx = fork_vertex->getId();

                if (srcConnections[sourceIndex].src->getOutEdge(srcConnections[sourceIndex].portIx) != nullptr)
                    srcConnections[sourceIndex].src->getOutEdge(srcConnections[sourceIndex].portIx)->connectSnk(
                            fork_vertex, 0);
                else
                    // Adding an edge between the source and the fork.
                    topSrdag->addEdge(
                            srcConnections[sourceIndex].src,
                            srcConnections[sourceIndex].portIx,
                            fork_vertex,
                            0,
                            curSourceToken
                    );

                // Replacing the source vertex by the explode vertex in the array of sources.
                srcConnections[sourceIndex].src = fork_vertex;
                srcConnections[sourceIndex].portIx = 0;
            }

            if (rest < curSinkToken &&
                snkConnections[sinkIndex].edge->getSnk() &&
                (snkConnections[sinkIndex].edge->getSnk()->getId() !=
                 joinIx)) { // Type == 0 indicates it is a normal vertex.

                // Adding an join vertex.
                SRDAGVertex *join_vertex = topSrdag->addJoin(MAX_IO_EDGES);
                joinIx = join_vertex->getId();

                // Replacing the sink vertex by the join vertex in the array of sources.
                snkConnections[sinkIndex].edge->connectSrc(join_vertex, 0);
                snkConnections[sinkIndex].edge->setRate(snkConnections[sinkIndex].cons);
                snkConnections[sinkIndex].edge = topSrdag->addEdge();
                snkConnections[sinkIndex].edge->connectSnk(join_vertex, 0);

            } else if (snkConnections[sinkIndex].edge->getSnk()
                       && snkConnections[sinkIndex].edge->getSnk()->getId() == joinIx) {
                /* Adding the new edge in join*/
                SRDAGVertex *join_vertex = snkConnections[sinkIndex].edge->getSnk();
                snkConnections[sinkIndex].edge = topSrdag->addEdge();
                snkConnections[sinkIndex].edge->connectSnk(join_vertex, join_vertex->getNConnectedInEdge());
            }

            //Creating the new edge between normal vertices or between a normal and an explode/implode one.
            SRDAGEdge *srcEdge;
            if ((srcEdge = srcConnections[sourceIndex].src->getOutEdge(srcConnections[sourceIndex].portIx)) !=
                nullptr) {
                snkConnections[sinkIndex].edge->setAlloc(srcEdge->getAlloc());
                snkConnections[sinkIndex].edge->setRate(srcEdge->getRate());

                topSrdag->delEdge(srcEdge);
                snkConnections[sinkIndex].edge->connectSrc(
                        srcConnections[sourceIndex].src,
                        srcConnections[sourceIndex].portIx
                );
            } else {
                snkConnections[sinkIndex].edge->connectSrc(
                        srcConnections[sourceIndex].src,
                        srcConnections[sourceIndex].portIx
                );
                snkConnections[sinkIndex].edge->setRate(rest);
            }

            // Update the number of token produced/consumed by the current source/target.
            curSourceToken -= rest;
            curSinkToken -= rest;

            if (curSourceToken == 0) {
                sourceIndex++;
                curSourceToken = srcConnections[sourceIndex].prod;
            } else {
                srcConnections[sourceIndex].portIx++;
            }

            if (curSinkToken == 0) {
                sinkIndex++;
                curSinkToken = snkConnections[sinkIndex].cons;
            }
        }

        if (persistentInit && persistentEnd) {
            topSrdag->addEdge(persistentInit, 1, persistentEnd, 1, 0);
        }

        StackMonitor::free(TRANSFO_STACK, srcConnections);
        StackMonitor::free(TRANSFO_STACK, snkConnections);
    }
}
