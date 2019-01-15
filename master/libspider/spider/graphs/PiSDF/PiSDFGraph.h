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
#ifndef PISDF_GRAPH_H
#define PISDF_GRAPH_H

#include <graphs/PiSDF/PiSDFCommon.h>
#include <graphs/Archi/Archi.h>

class PiSDFGraph {
public:
    PiSDFGraph(
            int nEdges, int nParams,
            int nInputIf, int nOutputIf,
            int nConfig, int nBody);

    virtual ~PiSDFGraph();

    PiSDFVertex *addBodyVertex(
            const char *vertexName, int fctId,
            int nInEdge, int nOutEdge,
            int nInParam);

    PiSDFVertex *addHierVertex(
            const char *vertexName,
            PiSDFGraph *graph,
            int nInEdge, int nOutEdge,
            int nInParam);

    PiSDFVertex *addSpecialVertex(
            PiSDFSubType subType,
            int nInEdge, int nOutEdge,
            int nInParam);

    PiSDFVertex *addConfigVertex(
            const char *vertexName, int fctId,
            PiSDFSubType subType,
            int nInEdge, int nOutEdge,
            int nInParam, int nOutParam);

    PiSDFVertex *addInputIf(
            const char *name,
            int nInParam);

    PiSDFVertex *addOutputIf(
            const char *name,
            int nInParam);


    PiSDFParam *addStaticParam(const char *name, const char *expr);

    PiSDFParam *addStaticParam(const char *name, int value);

    PiSDFParam *addHeritedParam(const char *name, int parentId);

    PiSDFParam *addDynamicParam(const char *name);

    PiSDFParam *addStaticDependentParam(const char *name, const char *expr);

    PiSDFParam *addDynamicDependentParam(const char *name, const char *expr);

    inline void addPiSDFParam(PiSDFParam *param);

    /** Element getters */
    inline PiSDFEdge *getEdge(int ix);

    inline PiSDFParam *getParam(int ix);

    inline PiSDFVertex *getBody(int ix);

    inline PiSDFVertex *getConfig(int ix);

    inline PiSDFVertex *getInputIf(int ix);

    inline PiSDFVertex *getOutputIf(int ix);

    inline const PiSDFParam *const *getParams() const;

    inline int getNParam() const;

    inline int getNEdge() const;

    inline int getNInIf() const;

    inline int getNOutIf() const;

    inline int getNConfig() const;

    inline int getNBody() const;

    /** General getters */
    inline PiSDFVertex *getParentVertex();

    /** General setters */
    inline void setParentVertex(PiSDFVertex *parent);

    /** Print Fct */
    void print(const char *path);

    /** Connect Fct */
    PiSDFEdge *connect(
            PiSDFVertex *source, int sourcePortId, const char *production,
            PiSDFVertex *sink, int sinkPortId, const char *consumption,
            const char *delay, PiSDFVertex *setter = nullptr, PiSDFVertex *getter = nullptr,
            PiSDFVertex *delayActor = nullptr,
            bool isDelayPersistent = false);

    void delVertex(PiSDFVertex *vertex);

    void delParam(PiSDFParam *param);

    void delEdge(PiSDFEdge *edge);

private:
    PiSDFVertex *parent_;

    PiSDFEdgeSet edges_;
    PiSDFParamSet params_;
    PiSDFVertexSet bodies_;
    PiSDFVertexSet configs_;
    PiSDFVertexSet inputIfs_;
    PiSDFVertexSet outputIfs_;

    PiSDFEdge *addEdge();
};

/** Inline Fcts */

/** Element getters */
inline PiSDFEdge *PiSDFGraph::getEdge(int ix) {
    return edges_[ix];
}

inline PiSDFParam *PiSDFGraph::getParam(int ix) {
    return params_[ix];
}

inline PiSDFVertex *PiSDFGraph::getBody(int ix) {
    return bodies_[ix];
}

inline PiSDFVertex *PiSDFGraph::getConfig(int ix) {
    return configs_[ix];
}

inline PiSDFVertex *PiSDFGraph::getInputIf(int ix) {
    return inputIfs_[ix];
}

inline PiSDFVertex *PiSDFGraph::getOutputIf(int ix) {
    return outputIfs_[ix];
}

inline const PiSDFParam *const *PiSDFGraph::getParams() const {
    return params_.getArray();
}

inline int PiSDFGraph::getNParam() const {
    return params_.getN();
}

inline int PiSDFGraph::getNEdge() const {
    return edges_.getN();
}

inline int PiSDFGraph::getNInIf() const {
    return inputIfs_.getN();
}

inline int PiSDFGraph::getNOutIf() const {
    return outputIfs_.getN();
}

inline int PiSDFGraph::getNConfig() const {
    return configs_.getN();
}

inline int PiSDFGraph::getNBody() const {
    return bodies_.getN();
}

/** General getters */
inline PiSDFVertex *PiSDFGraph::getParentVertex() {
    return parent_;
}

inline void PiSDFGraph::setParentVertex(PiSDFVertex *parent) {
    parent_ = parent;
}

inline void PiSDFGraph::addPiSDFParam(PiSDFParam *param) {
    params_.add(param);
}

#endif/*PISDF_GRAPH_H*/
