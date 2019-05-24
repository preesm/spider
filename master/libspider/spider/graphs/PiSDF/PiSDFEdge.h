/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018 - 2019)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018 - 2019)
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
    explicit PiSDFEdge(PiSDFGraph *graph);

    ~PiSDFEdge() override;

    /** Getters */
    inline int getId() const;

    inline PiSDFVertex *getSrc() const;

    inline PiSDFVertex *getSnk() const;

    inline int getSrcPortIx() const;

    inline int getSnkPortIx() const;

    inline std::int32_t getDelayAlloc() const;

    inline std::int32_t getAlloc() const;

    /** Setters */
    inline void setDelay(const char *delay,
                         PiSDFVertex *setter,
                         PiSDFVertex *getter,
                         bool isDelayPersistent);

    inline void setMemoryDelayAlloc(std::int32_t memDelayAlloc);

    inline void setMemoryAlloc(std::int32_t memoryAlloc);

    /** Connections Fcts */
    void connectSrc(PiSDFVertex *src, int srcPortId, const char *prod);

    void connectSrc(PiSDFVertex *src, int srcPortId, Expression *prod);

    void connectSnk(PiSDFVertex *snk, int snkPortId, const char *cons);

    void connectSnk(PiSDFVertex *snk, int snkPortId, Expression *cons);

    inline void disconnectSnk();

    inline void disconnectSrc();

    /** Add Param Fcts */
    inline void addInParam(int ix, PiSDFParam *param);

    /** Compute Fcts */
    inline Param resolveProd(transfoJob *job) const;

    inline Param resolveCons(transfoJob *job) const;

    inline Param resolveProd() const;

    inline Param resolveCons() const;

    inline Param resolveDelay(transfoJob *job);

    inline Param resolveDelay();

    inline void getProdExpr(char *out, int sizeOut);

    inline void getConsExpr(char *out, int sizeOut);

    inline Expression *getConsExpr();

    inline Expression *getProdExpr();

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
    Expression *delayExpression_;
    PiSDFVertex *setter_;
    PiSDFVertex *getter_;
    PiSDFVertex *virtual_;
    bool isDelayPersistent_;
    std::int32_t memDelayAlloc_;
    std::int32_t alloc_;
};

inline void PiSDFEdge::disconnectSnk() {
    if (snk_ == nullptr) {
        throwSpiderException("Trying to disconnect sink to non connected edge.");
    }
    snk_ = nullptr;
    snkPortIx_ = -1;
    cons_->~Expression();
    StackMonitor::free(PISDF_STACK, cons_);
    cons_ = nullptr;
}

inline void PiSDFEdge::disconnectSrc() {
    if (src_ == nullptr) {
        throwSpiderException("Trying to disconnect sink to non connected edge.");
    }
    src_ = nullptr;
    srcPortIx_ = -1;
    prod_->~Expression();
    StackMonitor::free(PISDF_STACK, prod_);
    prod_ = nullptr;
}

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

inline std::int32_t PiSDFEdge::getDelayAlloc() const {
    return memDelayAlloc_;
}

inline std::int32_t PiSDFEdge::getAlloc() const {
    return alloc_;
}

inline void PiSDFEdge::setDelay(const char *delay,
                                PiSDFVertex *setter,
                                PiSDFVertex *getter,
                                bool isDelayPersistent) {
    if (delayExpression_) {
        delayExpression_->~Expression();
        StackMonitor::free(PISDF_STACK, delayExpression_);
        delayExpression_ = nullptr;
    }
    delayExpression_ = CREATE(PISDF_STACK, Expression)(delay, graph_->getParams(), graph_->getNParam());
    setter_ = setter;
    getter_ = getter;
    if (setter || getter) {
        virtual_ = graph_->addBodyVertex("delay",
                                         PISDF_SUBTYPE_DELAY,
                                         1,
                                         1,
                                         0);
    }
    isDelayPersistent_ = isDelayPersistent;
}

inline void PiSDFEdge::setMemoryDelayAlloc(std::int32_t memDelayAlloc) {
    memDelayAlloc_ = memDelayAlloc;
}

inline void PiSDFEdge::setMemoryAlloc(std::int32_t memoryAlloc) {
    alloc_ = memoryAlloc;
}

inline Param PiSDFEdge::resolveProd(transfoJob *job) const {
    return prod_->evaluate(src_->getInParams(), job);
}

inline Param PiSDFEdge::resolveCons(transfoJob *job) const {
    return cons_->evaluate(snk_->getInParams(), job);
}

inline Param PiSDFEdge::resolveProd() const {
    return prod_->evaluate();
}

inline Param PiSDFEdge::resolveCons() const {
    return cons_->evaluate();
}

inline Param PiSDFEdge::resolveDelay(transfoJob *job) {
    return delayExpression_->evaluate(graph_->getParams(), job);
}

inline Param PiSDFEdge::resolveDelay() {
    return delayExpression_->evaluate();
}

/** TODO take care of prod_ cons_ delay_ != 0 */

inline void PiSDFEdge::getProdExpr(char *out, int sizeOut) {
    prod_->toString(src_->getInParams(), src_->getNInParam(), out, sizeOut);
}

inline void PiSDFEdge::getConsExpr(char *out, int sizeOut) {
    cons_->toString(snk_->getInParams(), snk_->getNInParam(), out, sizeOut);
}

Expression *PiSDFEdge::getConsExpr() {
    return cons_;
}

Expression *PiSDFEdge::getProdExpr() {
    return prod_;
}

inline void PiSDFEdge::getDelayExpr(char *out, int sizeOut) {
    delayExpression_->toString(graph_->getParams(), graph_->getNParam(), out, sizeOut);
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
