/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2018)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013)
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
#ifdef _MSC_VER
#if (_MSC_VER < 1900)
#define snprintf _snprintf
#endif
#endif

#include <graphs/SRDAG/SRDAGVertex.h>

/** Static Var def */
//int SRDAGVertex::globalId = 0;

/** Constructor */
SRDAGVertex::SRDAGVertex(
        int globalId,
        SRDAGType type, SRDAGGraph *graph,
        PiSDFVertex *reference,
        int refId, int iterId,
        int nInEdge, int nOutEdge,
        int nInParam, int nOutParam) {


    //globalID_ = globalId++;
    id_ = globalId;

    type_ = type;
    state_ = SRDAG_NEXEC;
    graph_ = graph;
    reference_ = reference;
    refId_ = refId;
    iterId_ = iterId;

    nMaxInEdge_ = nInEdge;
    inEdges_ = CREATE_MUL(SRDAG_STACK, nMaxInEdge_, SRDAGEdge*);
    memset(inEdges_, 0, nMaxInEdge_ * sizeof(SRDAGEdge *));

    nMaxOutEdge_ = nOutEdge;
    outEdges_ = CREATE_MUL(SRDAG_STACK, nMaxOutEdge_, SRDAGEdge*);
    memset(outEdges_, 0, nMaxOutEdge_ * sizeof(SRDAGEdge *));

    nCurInEdge_ = 0;
    nCurOutEdge_ = 0;

    nInParam_ = nInParam;
    inParams_ = CREATE_MUL(SRDAG_STACK, nInParam_, Param);
    memset(inParams_, 0, nInParam * sizeof(Param));

    nOutParam_ = nOutParam;
    outParams_ = CREATE_MUL(SRDAG_STACK, nOutParam_, Param*);
    memset(outParams_, 0, nOutParam * sizeof(Param **));

    start_ = end_ = -1;
    schedLvl_ = -1;
    slave_ = -1;
    slaveJobIx_ = -1;
}

SRDAGVertex::~SRDAGVertex() {
    StackMonitor::free(SRDAG_STACK, inEdges_);
    StackMonitor::free(SRDAG_STACK, outEdges_);
    StackMonitor::free(SRDAG_STACK, inParams_);
    StackMonitor::free(SRDAG_STACK, outParams_);
}

void SRDAGVertex::toString(char *name, int sizeMax) const {

    switch (type_) {
        case SRDAG_NORMAL:
            snprintf(name, sizeMax, "%s", reference_->getName());
            break;
        case SRDAG_FORK:
            snprintf(name, sizeMax, "Fork");
            break;
        case SRDAG_JOIN:
            snprintf(name, sizeMax, "Join");
            break;
        case SRDAG_ROUNDBUFFER:
            snprintf(name, sizeMax, "RB");
            break;
        case SRDAG_BROADCAST:
            snprintf(name, sizeMax, "BR");
            break;
        case SRDAG_INIT:
            snprintf(name, sizeMax, "Init");
            break;
        case SRDAG_END:
            snprintf(name, sizeMax, "End");
            break;
    }
}

void SRDAGVertex::updateState() {
    if (state_ == SRDAG_NEXEC) {
        /* Check Input Edges */
        for (int i = 0; i < getNConnectedInEdge(); i++) {
            SRDAGEdge *edge = getInEdge(i);
            if (!edge) {
                continue;
            }
            SRDAGVertex *predecessor = getInEdge(i)->getSrc();

            /* Case when you don't wait any token from predecessor (Null Edge) */
            if (edge->getRate() == 0)
                continue;

            if (!predecessor || predecessor->isHierarchical()) {
                state_ = SRDAG_NEXEC;
                return;
            }

            if (predecessor->state_ == SRDAG_NEXEC) {
                predecessor->updateState();
                if (predecessor->state_ == SRDAG_NEXEC) {
                    state_ = SRDAG_NEXEC;
                    return;
                }
            }
        }
        state_ = SRDAG_EXEC;
    }
}
