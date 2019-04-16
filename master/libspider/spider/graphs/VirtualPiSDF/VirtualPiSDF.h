/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2015)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013 - 2014)
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
#ifndef SPIDER_VIRTUALPISDF_H
#define SPIDER_VIRTUALPISDF_H

#include <graphs/PiSDF/PiSDFGraph.h>
#include <graphs/PiSDF/PiSDFVertex.h>
#include <scheduling/Scheduler/SRDAGLessIR.h>

class VirtualPiSDFGraph {
public:
    typedef struct VirtualPiSDFVertex {
        PiSDFVertex *vertex_ = nullptr;
        std::int32_t instance_ = -1;
    } VirtualPiSDFVertex;

    explicit VirtualPiSDFGraph(PiSDFGraph *graph);

    ~VirtualPiSDFGraph();

    /** General getter **/
    inline std::int32_t getNVertex() const;

    inline VirtualPiSDFVertex *getVertex(std::int32_t ix);

    inline std::int32_t getNEdgesIN(VirtualPiSDFVertex *vertex);

    inline VirtualPiSDFVertex *getFirstSource(VirtualPiSDFVertex *vertex, std::int32_t edgeIx);

    inline VirtualPiSDFVertex *getLastSource(VirtualPiSDFVertex *vertex, std::int32_t edgeIx);

    inline std::int32_t getNEdgesOUT(VirtualPiSDFVertex *vertex);

private:
    PiSDFGraph *originalGraph_;
    VirtualPiSDFVertex *vertexSet_;
    std::int32_t *pi2VirtMap_;

    void initVertexSet(PiSDFGraph *graph, std::int32_t *pi2VirtMap);

    inline void delVertex(VirtualPiSDFVertex *vertex);
};

inline std::int32_t VirtualPiSDFGraph::getNVertex() const {
    //return vertexSet_.getN();
    return 0;
}

inline VirtualPiSDFGraph::VirtualPiSDFVertex *VirtualPiSDFGraph::getVertex(std::int32_t ix) {
//    if (ix < 0 || ix >= vertexSet_.getN()) {
//        throwSpiderException("Bad index value: %d -- max: %d", ix, vertexSet_.getN());
//    }
    return &vertexSet_[ix];
}

inline std::int32_t VirtualPiSDFGraph::getNEdgesIN(VirtualPiSDFGraph::VirtualPiSDFVertex *vertex) {
    return vertex->vertex_->getNInEdge();
}

inline std::int32_t VirtualPiSDFGraph::getNEdgesOUT(VirtualPiSDFGraph::VirtualPiSDFVertex *vertex) {
    return vertex->vertex_->getNOutEdge();
}

inline void VirtualPiSDFGraph::delVertex(VirtualPiSDFGraph::VirtualPiSDFVertex *vertex) {
//    vertex->vertex_ = nullptr;
//    StackMonitor::free(TRANSFO_STACK, vertex);
//    vertexSet_.del(vertex);
}

inline VirtualPiSDFGraph::VirtualPiSDFVertex *
VirtualPiSDFGraph::getFirstSource(VirtualPiSDFGraph::VirtualPiSDFVertex *vertex, std::int32_t edgeIx) {
    auto *pisdfVertex = vertex->vertex_;
    auto firstInstance = SRDAGLessIR::computeFirstDependencyIx(pisdfVertex, edgeIx, vertex->instance_);
    auto virtIndex = pi2VirtMap_[pisdfVertex->getId() - 1] + firstInstance;
    return &vertexSet_[virtIndex];
}

inline VirtualPiSDFGraph::VirtualPiSDFVertex *
VirtualPiSDFGraph::getLastSource(VirtualPiSDFGraph::VirtualPiSDFVertex *vertex, std::int32_t edgeIx) {
    auto *pisdfVertex = vertex->vertex_;
    auto lastInstance = SRDAGLessIR::computeLastDependencyIx(pisdfVertex, edgeIx, vertex->instance_);
    auto virtIndex = pi2VirtMap_[pisdfVertex->getId() - 1] + lastInstance;
    return &vertexSet_[virtIndex];
}

#endif //SPIDER_VIRTUALPISDF_H
