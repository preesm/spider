/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018 - 2019)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018 - 2019)
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
#include <graphs/SRDAG/SRDAGCommon.h>
#include <graphs/SRDAG/SRDAGVertex.h>

/** Static Var def */
//int SRDAGEdge::globalId = 0;

SRDAGEdge::SRDAGEdge() {
    id_ = -1;
    graph_ = nullptr;

    src_ = nullptr;
    srcPortIx_ = -1;
    snk_ = nullptr;
    snkPortIx_ = -1;

    rate_ = -1;
    alloc_ = -1;
}

SRDAGEdge::SRDAGEdge(SRDAGGraph *graph, int globalId) {

    //globalID_ = globalId++;
    id_ = globalId;

    graph_ = graph;

    src_ = nullptr;
    srcPortIx_ = -1;
    snk_ = nullptr;
    snkPortIx_ = -1;

    rate_ = -1;
    alloc_ = -1;
}

SRDAGEdge::~SRDAGEdge() = default;

void SRDAGEdge::connectSrc(SRDAGVertex *src, int srcPortId) {
    if (src_ != nullptr) {
        throwSpiderException("Try to connect to an already connected edge.");
    }
    src_ = src;
    srcPortIx_ = srcPortId;
    src_->connectOutEdge(this, srcPortIx_);
}

void SRDAGEdge::connectSnk(SRDAGVertex *snk, int snkPortId) {
    if (snk_ != nullptr) {
        throwSpiderException("Try to connect to an already connected edge.");
    }
    snk_ = snk;
    snkPortIx_ = snkPortId;
    snk_->connectInEdge(this, snkPortIx_);
}

void SRDAGEdge::disconnectSrc() {
    if (src_ == nullptr) {
        throwSpiderException("Try to disconnect to an already disconnected edge.");
    }
    src_->disconnectOutEdge(srcPortIx_);
    src_ = nullptr;
    srcPortIx_ = -1;
}

void SRDAGEdge::disconnectSnk() {
    if (snk_ == nullptr) {
        throwSpiderException("Try to disconnect to an already disconnected edge.");
    }
    snk_->disconnectInEdge(snkPortIx_);
    snk_ = nullptr;
    snkPortIx_ = -1;
}
