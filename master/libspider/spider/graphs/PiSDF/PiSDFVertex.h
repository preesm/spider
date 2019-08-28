/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018 - 2019)
 * Daniel Madroñal <daniel.madronal@upm.es> (2019)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018 - 2019)
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
#include <scheduling/PiSDFScheduleJob.h>

class PiSDFVertex : public SetElement {
public:
    PiSDFVertex(
            const char *name, int fctId,
            int typeId,
            PiSDFType type, PiSDFSubType subType,
            PiSDFGraph *graph, PiSDFGraph *subGraph,
            int nInEdge, int nOutEdge,
            int nInParam, int nOutParam);

    ~PiSDFVertex() override;

    /** Parameters getters */
    inline int getNInParam() const;

    inline int getNOutParam() const;

    inline const PiSDFParam *getInParam(int ix) const;

    inline Param getInParamValue(int ix) const;

    inline const PiSDFParam *getOutParam(int ix) const;

    inline const PiSDFParam *const *getInParams() const;

    inline const PiSDFParam *const *getOutParams() const;

    /** Data edge getters */
    inline int getNInEdge() const;

    inline int getNOutEdge() const;

    inline PiSDFEdge *getInEdge(int ix);

    inline PiSDFEdge *getOutEdge(int ix);

    inline PiSDFEdge *const *getAllEdges() const;

    /** Connect Fcts */
    inline void connectInEdge(int ix, PiSDFEdge *edge);

    inline void connectOutEdge(int ix, PiSDFEdge *edge);

    /** Disconnect Fcts **/
    inline void disconnectInEdge(int ix);

    inline void disconnectOutEdge(int ix);

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

    inline std::int32_t getBRVValue() const {
        return brvValue_;
    }

    /** General setter */
    inline void setSubGraph(PiSDFGraph *subGraph);

    inline void setBRVValue(std::int32_t value) {
        brvValue_ = value;
    }

    inline void setId(std::int32_t id) {
        id_ = id;
    }

    /** Constraints/timings */
    inline bool canExecuteOn(int pe);

    inline Time getTimingOnType(int peType, const Param *vertexParamValues, int nParam);

    inline Time getTimingOnPEType(int peType);

    inline double getEnergyOnPEType(int peType);

    inline const bool *getConstraints() const;

    inline void setTimingOnType(int peType, const char *timing);

    inline void setEnergyOnType(int peType, double energy);

    inline void isExecutableOnAllPE();

    inline void isExecutableOnPE(int pe);

    void createScheduleJob(int nInstance) {
        if (scheduleJob_) {
            scheduleJob_->~PiSDFScheduleJob();
            StackMonitor::free(TRANSFO_STACK, scheduleJob_);
        }
        scheduleJob_ = CREATE_NA(TRANSFO_STACK, PiSDFScheduleJob)(nInstance, Platform::get()->getNLrt());
        scheduleJob_->setVertex(this);
    }

    inline PiSDFScheduleJob *getScheduleJob() {
        return scheduleJob_;
    }

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

    int nEdge_;
    int nInEdge_;
    int nOutEdge_;
    PiSDFEdge **allEdges_;
    PiSDFEdge **inEdges_;
    PiSDFEdge **outEdges_;

    int nInParam_, nOutParam_;
    PiSDFParam **inParams_, **outParams_;

    int nPeMax_, nPeTypeMax_;
    bool *constraints_;
    Expression **timings_;
    double *energies_;

    PiSDFScheduleJob *scheduleJob_;

    std::int32_t brvValue_;
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
    if (ix < nInParam_ && ix >= 0) {
        return inParams_[ix];
    }
    throwSpiderException("Bad index. Value: %d -- Max: %d", ix, nInParam_);
}

inline Param PiSDFVertex::getInParamValue(int ix) const {
    if (ix < nInParam_ && ix >= 0) {
        return inParams_[ix]->getValue();
    }
    throwSpiderException("Bad index. Value: %d -- Max: %d", ix, nInParam_);
}

inline const PiSDFParam *PiSDFVertex::getOutParam(int ix) const {
    if (ix < nOutParam_ && ix >= 0)
        return outParams_[ix];
    throwSpiderException("Bad index. Value: %d -- Max: %d", ix, nOutParam_);
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
    throwSpiderException("Bad index. Value: %d -- Max: %d", ix, nInEdge_);
}

inline PiSDFEdge *PiSDFVertex::getOutEdge(int ix) {
    if (ix < nOutEdge_ && ix >= 0)
        return outEdges_[ix];
    throwSpiderException("Bad index. Value: %d -- Max: %d", ix, nOutEdge_);
}

inline PiSDFEdge *const *PiSDFVertex::getAllEdges() const {
    return allEdges_;
}

/** Connect Fcts */
inline void PiSDFVertex::connectInEdge(int ix, PiSDFEdge *edge) {
    if (ix >= nInEdge_ || ix < 0)
        throwSpiderException("Bad index. Vertex [%s] --> Value: %d -- Max: %d", name_, ix, nInEdge_);
    else if (inEdges_[ix] != nullptr)
        throwSpiderException("Vertex [%s] --> Trying to erase already connected input edge.", name_);
    else
        inEdges_[ix] = edge;
}

inline void PiSDFVertex::connectOutEdge(int ix, PiSDFEdge *edge) {
    if (ix >= nOutEdge_ || ix < 0)
        throwSpiderException("Bad index. Value: %d -- Max: %d", ix, nOutEdge_);
    else if (outEdges_[ix] != nullptr)
        throwSpiderException("Vertex [%s] --> Trying to erase already connected output edge.", name_);
    else
        outEdges_[ix] = edge;
}

/** Disconnect Fcts **/
inline void PiSDFVertex::disconnectInEdge(int ix) {
    if (ix >= nInEdge_ || ix < 0) {
        throwSpiderException("Bad index. Value: %d -- Max: %d", ix, nInEdge_);
    } else if (!inEdges_[ix]) {
        throwSpiderException("Trying to disconnect NULL input edge.");
    } else {
        inEdges_[ix] = nullptr;
    }
}

inline void PiSDFVertex::disconnectOutEdge(int ix) {
    if (ix >= nOutEdge_ || ix < 0) {
        throwSpiderException("Bad index. Value: %d -- Max: %d", ix, nOutEdge_);
    } else if (outEdges_[ix] == nullptr) {
        throwSpiderException("Trying to disconnect NULL output edge.");
    } else {
        outEdges_[ix] = nullptr;
    }
}

/** Add Param Fcts */
inline void PiSDFVertex::addInParam(int ix, PiSDFParam *param) {
    if (ix >= nInParam_ || ix < 0)
        throwSpiderException("Bad index. Value: %d -- Max: %d", ix, nInParam_);
    else if (inParams_[ix] != nullptr)
        throwSpiderException("Vertex [%s] --> Trying to erase already connected input param.", name_);
    else
        inParams_[ix] = param;
}

inline void PiSDFVertex::addOutParam(int ix, PiSDFParam *param) {
    if (ix >= nOutParam_ || ix < 0)
        throwSpiderException("Bad index. Value: %d -- Max: %d", ix, nOutParam_);
    else if (outParams_[ix] != nullptr)
        throwSpiderException("Vertex [%s] --> Trying to erase already connected output param.", name_);
    else {
        outParams_[ix] = param;
        param->setSetter(this, ix);
    }
}

/** General getters */
inline int PiSDFVertex::getId() const {
    return id_;
}

inline int PiSDFVertex::getTypeId() const {
    return typeId_;
}

inline const char *PiSDFVertex::getName() const {
    if (name_) {
        return name_;
    }
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
        case PISDF_SUBTYPE_INIT:
            return "Init";
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
    return subGraph_ != nullptr;
}

inline void PiSDFVertex::setSubGraph(PiSDFGraph *subGraph) {
    subGraph_ = subGraph;
}

/** Constraints/timings/energies */
inline bool PiSDFVertex::canExecuteOn(int pe) {
    if (pe < 0 || pe >= nPeMax_)
        throwSpiderException("Bad PE index. Value: %d -- Max: %d", pe, nPeMax_);
    return constraints_[pe];
}

inline Time PiSDFVertex::getTimingOnType(int peType, const Param *vertexParamValues, int nParam) {
    if (peType < 0 || peType >= nPeTypeMax_) {
        throwSpiderException("Bad PEType index. Value: %d -- Max: %d", peType, nPeTypeMax_ - 1);
    }

    if (timings_[peType] == nullptr) {
        return subType_ == PISDF_SUBTYPE_NORMAL ? 0 : 50;
    }
    return (Time) timings_[peType]->evaluate(vertexParamValues, nParam);
}

inline Time PiSDFVertex::getTimingOnPEType(int peType) {
    if (peType < 0 || peType >= nPeTypeMax_) {
        throwSpiderException("Bad PEType index. Value: %d -- Max: %d", peType, nPeTypeMax_ - 1);
    }
    if (timings_[peType] == nullptr) {
        return subType_ == PISDF_SUBTYPE_NORMAL ? 0 : 50;
    }
    return (Time) timings_[peType]->evaluate();
}

inline double PiSDFVertex::getEnergyOnPEType(int peType) {
    if (peType < 0 || peType >= nPeTypeMax_) {
        throwSpiderException("Bad PEType index. Value: %d -- Max: %d", peType, nPeTypeMax_ - 1);
    }
    return energies_[peType];
}

inline const bool *PiSDFVertex::getConstraints() const {
    return constraints_;
}

inline void PiSDFVertex::setTimingOnType(int peType, const char *timing) {
    if (peType < 0 || peType >= nPeTypeMax_) {
        throwSpiderException("Bad PEType index. Value: %d -- Max: %d", peType, nPeTypeMax_);
    }

    if (timings_[peType] != nullptr) {
        timings_[peType]->~Expression();
        StackMonitor::free(PISDF_STACK, timings_[peType]);
        timings_[peType] = nullptr;
    }
    timings_[peType] = CREATE(PISDF_STACK, Expression)(timing, this->getInParams(), this->getNInParam());
}

inline void PiSDFVertex::setEnergyOnType(int peType, double energy) {
    if (peType < 0 || peType >= nPeTypeMax_) {
        throwSpiderException("Bad PEType index. Value: %d -- Max: %d", peType, nPeTypeMax_);
    }
    energies_[peType] = energy;
}

inline void PiSDFVertex::isExecutableOnAllPE() {
    memset(constraints_, true, nPeMax_ * sizeof(bool));
}

inline void PiSDFVertex::isExecutableOnPE(int pe) {
    if (pe < 0 || pe > nPeMax_) {
        throwSpiderException("Bad PE index. Value: %d -- Max: %d", pe, nPeMax_);
    }
    constraints_[pe] = true;
}

#endif/*PISDF_VERTEX_H*/
