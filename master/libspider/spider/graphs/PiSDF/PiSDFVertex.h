/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2017) :
 *
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2016)
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
#ifndef PISDF_VERTEX_H
#define PISDF_VERTEX_H

#include <tools/SetElement.h>

#include <graphs/PiSDF/PiSDFCommon.h>
#include <graphs/Archi/Archi.h>
#include <parser/Expression.h>

#include <cstring>

class PiSDFVertex : public SetElement {
public:
    PiSDFVertex(
            const char *name, int fctId,
            int typeId,
            PiSDFType type, PiSDFSubType subType,
            PiSDFGraph *graph, PiSDFGraph *subGraph,
            int nInEdge, int nOutEdge,
            int nInParam, int nOutParam);

    ~PiSDFVertex();

    /** Parameters getters */
    inline int getNInParam() const;

    inline int getNOutParam() const;

    inline const PiSDFParam *getInParam(int ix) const;

    inline const PiSDFParam *getOutParam(int ix) const;

    inline const PiSDFParam *const *getInParams() const;

    inline const PiSDFParam *const *getOutParams() const;

    /** Data edge getters */
    inline int getNInEdge() const;

    inline int getNOutEdge() const;

    inline PiSDFEdge *getInEdge(int ix);

    inline PiSDFEdge *getOutEdge(int ix);

    /** Connect Fcts */
    inline void connectInEdge(int ix, PiSDFEdge *edge);

    inline void connectOutEdge(int ix, PiSDFEdge *edge);

    /** Add Param Fcts */
    inline void addInParam(int ix, PiSDFParam *param);

    inline void addOutParam(int ix, PiSDFParam *param);

    /** General getters */
    inline int getId() const;

    inline int getTypeId() const;

    inline int getFctId() const;

    inline const char *getName() const;

    inline PiSDFType getType() const;

    inline PiSDFSubType getSubType() const;

    inline PiSDFGraph *getGraph() const;

    inline PiSDFGraph *getSubGraph() const;

    inline bool isHierarchical() const;

    /** General setter */
    inline void setSubGraph(PiSDFGraph *subGraph);

    /** Constraints/timings */
    inline bool canExecuteOn(int pe);

    inline Time getTimingOnType(int peType, const int *vertexParamValues, int nParam);

    inline const bool *getConstraints() const;

    inline void setTimingOnType(int peType, const char *timing);

    inline void isExecutableOnAllPE();

    inline void isExecutableOnPE(int pe);

private:
    static int globalId;

    int id_;
    int typeId_;
    int fctId_;
    const char *name_;

    PiSDFType type_;
    PiSDFSubType subType_;

    PiSDFGraph *graph_;
    PiSDFGraph *subGraph_;

    int nInEdge_, nOutEdge_;
    PiSDFEdge **inEdges_, **outEdges_;

    int nInParam_, nOutParam_;
    PiSDFParam **inParams_, **outParams_;

    int nPeMax_, nPeTypeMax_;
    bool *constraints_;
    Expression **timings_;
};

/** Inlines Fcts */
/** Parameters getters */
inline int PiSDFVertex::getNInParam() const {
    return nInParam_;
}

inline int PiSDFVertex::getNOutParam() const {
    return nOutParam_;
}

inline const PiSDFParam *PiSDFVertex::getInParam(int ix) const {
    if (ix < nInParam_ && ix >= 0)
        return inParams_[ix];
    else
        throw std::runtime_error("PiSDFVertex: Bad ix in getInParam");
}

inline const PiSDFParam *PiSDFVertex::getOutParam(int ix) const {
    if (ix < nOutParam_ && ix >= 0)
        return outParams_[ix];
    else
        throw std::runtime_error("PiSDFVertex: Bad ix in getOutParam");
}

inline const PiSDFParam *const *PiSDFVertex::getInParams() const {
    return inParams_;
}

inline const PiSDFParam *const *PiSDFVertex::getOutParams() const {
    return outParams_;
}

/** Data edge getters */
inline int PiSDFVertex::getNInEdge() const {
    return nInEdge_;
}

inline int PiSDFVertex::getNOutEdge() const {
    return nOutEdge_;
}

inline PiSDFEdge *PiSDFVertex::getInEdge(int ix) {
    if (ix < nInEdge_ && ix >= 0)
        return inEdges_[ix];
    else
        throw std::runtime_error("PiSDFVertex: Bad ix in getInEdge");
}

inline PiSDFEdge *PiSDFVertex::getOutEdge(int ix) {
    if (ix < nOutEdge_ && ix >= 0)
        return outEdges_[ix];
    else
        throw std::runtime_error("PiSDFVertex: Bad ix in getOutEdge");
}

/** Connect Fcts */
inline void PiSDFVertex::connectInEdge(int ix, PiSDFEdge *edge) {
    if (ix >= nInEdge_ || ix < 0)
        throw std::runtime_error("PiSDFVertex: Bad ix in connectInEdge");
    else if (inEdges_[ix] != 0)
        throw std::runtime_error("PiSDFVertex: Try to erase already connected input edge");
    else
        inEdges_[ix] = edge;
}

inline void PiSDFVertex::connectOutEdge(int ix, PiSDFEdge *edge) {
    if (ix >= nOutEdge_ || ix < 0)
        throw std::runtime_error("PiSDFVertex: Bad ix in connectOutEdge");
    else if (outEdges_[ix] != 0)
        throw std::runtime_error("PiSDFVertex: Try to erase already connected output edge");
    else
        outEdges_[ix] = edge;
}

/** Add Param Fcts */
inline void PiSDFVertex::addInParam(int ix, PiSDFParam *param) {
    if (ix >= nInParam_ || ix < 0)
        throw std::runtime_error("PiSDFVertex: Bad ix in addInParam");
    else if (inParams_[ix] != 0)
        throw std::runtime_error("PiSDFVertex: Try to erase already connected input param");
    else
        inParams_[ix] = param;
}

inline void PiSDFVertex::addOutParam(int ix, PiSDFParam *param) {
    if (ix >= nOutParam_ || ix < 0)
        throw std::runtime_error("PiSDFVertex: Bad ix in addOutParam");
    else if (outParams_[ix] != 0)
        throw std::runtime_error("PiSDFVertex: Try to erase already connected output param");
    else
        outParams_[ix] = param;
}

/** General getters */
inline int PiSDFVertex::getId() const {
    return id_;
}

inline int PiSDFVertex::getTypeId() const {
    return typeId_;
}

inline const char *PiSDFVertex::getName() const {
    switch (subType_) {
        case PISDF_SUBTYPE_INPUT_IF:
        case PISDF_SUBTYPE_OUTPUT_IF:
        case PISDF_SUBTYPE_NORMAL:
            return name_;
        case PISDF_SUBTYPE_BROADCAST:
            return "Broadcast";
        case PISDF_SUBTYPE_FORK:
            return "Fork";
        case PISDF_SUBTYPE_JOIN:
            return "Join";
        case PISDF_SUBTYPE_END:
            return "End";
        case PISDF_SUBTYPE_DELAY:
            return "DelayActor";
        case PISDF_SUBTYPE_ROUNDBUFFER:
            return "Roundbuffer";
    }
    return "NA";
}

inline int PiSDFVertex::getFctId() const {
    return fctId_;
}

inline PiSDFType PiSDFVertex::getType() const {
    return type_;
}

inline PiSDFSubType PiSDFVertex::getSubType() const {
    return subType_;
}

inline PiSDFGraph *PiSDFVertex::getGraph() const {
    return graph_;
}

inline PiSDFGraph *PiSDFVertex::getSubGraph() const {
    return subGraph_;
}

inline bool PiSDFVertex::isHierarchical() const {
    return subGraph_ != 0;
}

inline void PiSDFVertex::setSubGraph(PiSDFGraph *subGraph) {
    subGraph_ = subGraph;
}

/** Constraints/timings */
inline bool PiSDFVertex::canExecuteOn(int pe) {
    if (pe < 0 || pe >= nPeMax_)
        throw std::runtime_error("PiSDFVertex: accessing bad PE ix\n");
    return constraints_[pe];
}

inline Time PiSDFVertex::getTimingOnType(int peType, const int *vertexParamValues, int nParam) {
    if (peType < 0 || peType >= nPeTypeMax_) {
        printf("PiSDFVertex: get accessing bad PE type ix %d\n", peType);
        throw std::runtime_error("PiSDFVertex: accessing bad PE type ix\n");
    }

    if (timings_[peType] == NULL) return 0;
    return timings_[peType]->evaluate(vertexParamValues, nParam);
}

inline const bool *PiSDFVertex::getConstraints() const {
    return constraints_;
}

inline void PiSDFVertex::setTimingOnType(int peType, const char *timing) {
    if (peType < 0 || peType >= nPeTypeMax_) {
        printf("PiSDFVertex: set accessing bad PE type ix %d\n", peType);
        throw std::runtime_error("PiSDFVertex: accessing bad PE type ix\n");
    }

    if (timings_[peType] != 0) {
        timings_[peType]->~Expression();
        StackMonitor::free(PISDF_STACK, timings_[peType]);
        timings_[peType] = 0;
    }
    timings_[peType] = CREATE(PISDF_STACK, Expression)(timing, this->getInParams(), this->getNInParam());
}

inline void PiSDFVertex::isExecutableOnAllPE() {
    memset(constraints_, true, nPeMax_ * sizeof(bool));
}

inline void PiSDFVertex::isExecutableOnPE(int pe) {
    if (pe < 0)
        throw std::runtime_error("PiSDFVertex: accessing bad PE ix\n");
    else if (pe >= nPeMax_) return;
    constraints_[pe] = true;
}

#endif/*PISDF_VERTEX_H*/
