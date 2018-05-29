/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2017) :
 *
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2015)
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
#ifndef SRDAG_EDGE_H
#define SRDAG_EDGE_H

#include <tools/SetElement.h>

#include <graphs/SRDAG/SRDAGCommon.h>
//#include "SRDAGVertex.h"
//#include "SRDAGGraph.h"
//#include <parser/Expression.h>

class SRDAGEdge : public SetElement {
public:
    /** Constructors */
    SRDAGEdge();

    SRDAGEdge(SRDAGGraph *graph, int globalId);

    ~SRDAGEdge();

    /** Getters */
    inline int getId() const;

    inline SRDAGVertex *getSrc() const;

    inline SRDAGVertex *getSnk() const;

    inline int getSrcPortIx() const;

    inline int getSnkPortIx() const;

    inline int getRate() const;

    inline int getAlloc() const;

    inline int getAllocIx() const;

    inline int getNToken() const;

    /** Setters */
    inline void setRate(int rate);

    inline void setAlloc(int rate);

    inline void setAllocIx(int allocIx);

    inline void setNToken(int nToken);

    /** Connections Fcts */
    void connectSrc(SRDAGVertex *src, int srcPortId);

    void connectSnk(SRDAGVertex *snk, int snkPortId);

    void disconnectSrc();

    void disconnectSnk();

private:
    //static int globalId;

    int id_;
    SRDAGGraph *graph_;

    SRDAGVertex *src_;
    int srcPortIx_;
    SRDAGVertex *snk_;
    int snkPortIx_;

    int rate_;
    int alloc_;
    int allocIx_;
    int nToken_;
};

inline int SRDAGEdge::getId() const {
    return id_;
}

inline SRDAGVertex *SRDAGEdge::getSrc() const {
    return src_;
}

inline SRDAGVertex *SRDAGEdge::getSnk() const {
    return snk_;
}

inline int SRDAGEdge::getSrcPortIx() const {
    return srcPortIx_;
}

inline int SRDAGEdge::getSnkPortIx() const {
    return snkPortIx_;
}

inline int SRDAGEdge::getRate() const {
    return rate_;
}

inline int SRDAGEdge::getAlloc() const {
    return alloc_;
}

inline int SRDAGEdge::getAllocIx() const {
    return allocIx_;
}

inline int SRDAGEdge::getNToken() const {
    return nToken_;
}

inline void SRDAGEdge::setRate(int rate) {
    rate_ = rate;
}

inline void SRDAGEdge::setAlloc(int alloc) {
    alloc_ = alloc;
}

inline void SRDAGEdge::setAllocIx(int allocIx) {
    allocIx_ = allocIx;
}

inline void SRDAGEdge::setNToken(int nToken) {
    nToken_ = nToken;
}


#endif/*SRDAG_EDGE_H*/
