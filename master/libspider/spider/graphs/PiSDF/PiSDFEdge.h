/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
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
#ifndef PISDF_EDGE_H
#define PISDF_EDGE_H

#include <tools/SetElement.h>

#include <graphs/PiSDF/PiSDFCommon.h>
#include <graphs/PiSDF/PiSDFGraph.h>
#include <graphs/PiSDF/PiSDFVertex.h>
#include <parser/Expression.h>

class PiSDFEdge : public SetElement {
public:
    /** Constructors */
    PiSDFEdge(PiSDFGraph *graph);

    ~PiSDFEdge() override;

    /** Getters */
    inline int getId() const;

    inline PiSDFVertex *getSrc() const;

    inline PiSDFVertex *getSnk() const;

    inline int getSrcPortIx() const;

    inline int getSnkPortIx() const;

    inline int getDelayAlloc() const;

    /** Setters */
    inline void setDelay(const char *delay,
                         PiSDFVertex *setter,
                         PiSDFVertex *getter,
                         PiSDFVertex *delayActor,
                         bool isDelayPersistent);

    inline void setMemoryDelayAlloc(int memDelayAlloc);

    /** Connections Fcts */
    void connectSrc(PiSDFVertex *src, int srcPortId, const char *prod);

    void connectSnk(PiSDFVertex *snk, int snkPortId, const char *cons);

    /** Add Param Fcts */
    inline void addInParam(int ix, PiSDFParam *param);

    /** Compute Fcts */
    inline Param resolveProd(transfoJob *job) const;

    inline Param resolveCons(transfoJob *job) const;

    inline Param resolveDelay(transfoJob *job);

    inline void getProdExpr(char *out, int sizeOut);

    inline void getConsExpr(char *out, int sizeOut);

    inline void getDelayExpr(char *out, int sizeOut);

    inline PiSDFVertex *getDelaySetter();

    inline PiSDFVertex *getDelayGetter();

    inline PiSDFVertex *getDelayVirtual();

    inline bool isDelayPersistent();

private:
    static int globalId;

    int id_;
    PiSDFGraph *graph_;

    PiSDFVertex *src_;
    int srcPortIx_;
    PiSDFVertex *snk_;
    int snkPortIx_;

    /* Production and Consumption */
    Expression *prod_;
    Expression *cons_;

    /* Parameterized Delays */
    Expression *delay_;
    PiSDFVertex *setter_;
    PiSDFVertex *getter_;
    PiSDFVertex *virtual_;
    bool isDelayPersistent_;
    int memDelayAlloc_;
};

inline int PiSDFEdge::getId() const {
    return id_;
}

inline PiSDFVertex *PiSDFEdge::getSrc() const {
    return src_;
}

inline PiSDFVertex *PiSDFEdge::getSnk() const {
    return snk_;
}

inline int PiSDFEdge::getSrcPortIx() const {
    return srcPortIx_;
}

inline int PiSDFEdge::getSnkPortIx() const {
    return snkPortIx_;
}

inline int PiSDFEdge::getDelayAlloc() const {
    return memDelayAlloc_;
}

inline void PiSDFEdge::setDelay(const char *delay,
                                PiSDFVertex *setter,
                                PiSDFVertex *getter,
                                PiSDFVertex *delayActor,
                                bool isDelayPersistent) {
    if (delay_ != nullptr) {
        delay_->~Expression();
        StackMonitor::free(PISDF_STACK, delay_);
        delay_ = nullptr;
    }
    delay_ = CREATE(PISDF_STACK, Expression)(delay, graph_->getParams(), graph_->getNParam());

    if ((setter || getter) && !delayActor) {
        throwSpiderException("Delay can not have setter nor getter without special delay actor vertex.");
    }

    if (setter) {
        setter_ = setter;
        virtual_ = delayActor;
        //setter->connectOutEdge(0, this);
    }
    if (getter) {
        getter_ = getter;
        virtual_ = delayActor;
        //getter->connectInEdge(0, this);
    }
    isDelayPersistent_ = isDelayPersistent;
}

inline void PiSDFEdge::setMemoryDelayAlloc(int memDelayAlloc) {
    memDelayAlloc_ = memDelayAlloc;
}

inline Param PiSDFEdge::resolveProd(transfoJob *job) const {
    return prod_->evaluate(src_->getInParams(), job);
}

inline Param PiSDFEdge::resolveCons(transfoJob *job) const {
    return cons_->evaluate(snk_->getInParams(), job);
}

inline Param PiSDFEdge::resolveDelay(transfoJob *job) {
    return delay_->evaluate(graph_->getParams(), job);
}

/** TODO take care of prod_ cons_ delay_ != 0 */

inline void PiSDFEdge::getProdExpr(char *out, int sizeOut) {
    prod_->toString(src_->getInParams(), src_->getNInParam(), out, sizeOut);
}

inline void PiSDFEdge::getConsExpr(char *out, int sizeOut) {
    cons_->toString(snk_->getInParams(), snk_->getNInParam(), out, sizeOut);
}

inline void PiSDFEdge::getDelayExpr(char *out, int sizeOut) {
    delay_->toString(graph_->getParams(), graph_->getNParam(), out, sizeOut);
}

inline PiSDFVertex *PiSDFEdge::getDelaySetter() {
    return setter_;
}

inline PiSDFVertex *PiSDFEdge::getDelayGetter() {
    return getter_;
}

inline PiSDFVertex *PiSDFEdge::getDelayVirtual() {
    return virtual_;
}

inline bool PiSDFEdge::isDelayPersistent() {
    return isDelayPersistent_;
}

#endif/*PISDF_EDGE_H*/
