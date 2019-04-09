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
#ifndef SPIDER_SRDAGLESSIR_H
#define SPIDER_SRDAGLESSIR_H

#include <cstdint>
#include "graphs/PiSDF/PiSDFEdge.h"
#include "graphs/PiSDF/PiSDFVertex.h"

class SRDAGLessIR {
public:
    SRDAGLessIR() = default;

    ~SRDAGLessIR() = default;

    static inline std::int32_t computeFirstDependencyIx(PiSDFVertex *vertex,
                                                        std::int32_t edgeIx,
                                                        std::int32_t vertexInstance);

    static inline std::int32_t computeLastDependencyIx(PiSDFVertex *vertex,
                                                       std::int32_t edgeIx,
                                                       std::int32_t vertexInstance,
                                                       std::int32_t rho = 1);

    static inline std::int32_t computeFirstDependencyIx(std::int32_t cons,
                                                        std::int32_t prod,
                                                        std::int32_t delay,
                                                        std::int32_t vertexInstance);

    static inline std::int32_t computeLastDependencyIx(std::int32_t cons,
                                                       std::int32_t prod,
                                                       std::int32_t delay,
                                                       std::int32_t vertexInstance,
                                                       std::int32_t rho = 1,
                                                       std::int32_t rv = 1);

    static void computeDependenciesIxFromInputIF(PiSDFVertex *vertex,
                                                 std::int32_t *edgeIx,
                                                 const std::int32_t *instancesArray,
                                                 PiSDFVertex **producer,
                                                 std::int32_t *deltaStart,
                                                 std::int32_t *deltaEnd);

    static std::int32_t computeFirstDependencyIxRelaxed(PiSDFVertex *vertex,
                                                        std::int32_t edgeIx,
                                                        std::int32_t vertexInstance,
                                                        std::int32_t level = -1);

    static std::int32_t computeLastDependencyIxRelaxed(PiSDFVertex *vertex,
                                                       std::int32_t edgeIx,
                                                       std::int32_t vertexInstance,
                                                       std::int32_t level = -1,
                                                       std::int32_t rho = 1,
                                                       std::int32_t rv = 1);

    static std::int32_t computeFirstDependencyIxRelaxed(PiSDFEdge *edge,
                                                        std::int32_t vertexInstance,
                                                        PiSDFVertex **producer,
                                                        std::int32_t level = -1);

    static std::int32_t computeLastDependencyIxRelaxed(PiSDFEdge *edge,
                                                       std::int32_t vertexInstance,
                                                       PiSDFVertex **producer,
                                                       int32_t level = 1,
                                                       std::int32_t rho = 1,
                                                       std::int32_t rv = 1);

    static void computeFirstDependencyIxRelaxed(PiSDFEdge *edge,
                                                std::int32_t vertexInstance,
                                                PiSDFVertex **producer,
                                                std::vector<std::int32_t> &indexes,
                                                int32_t level = 1);

    static void computeLastDependencyIxRelaxed(PiSDFEdge *edge,
                                               std::int32_t vertexInstance,
                                               PiSDFVertex **producer,
                                               std::vector<std::int32_t> &indexes,
                                               int32_t level = 1,
                                               int32_t rho = -1,
                                               int32_t rv = -1);

    static inline PiSDFVertex *getProducer(PiSDFVertex *vertex,
                                           std::int32_t edgeIx,
                                           std::int32_t vertexInstance);

    /**
     * @brief Get the real producer of an actor on a given edge
     * @remark This function does not handle case where outputIF is connected to inputIF
     * @param vertex Actor
     * @param edgeIx Input edge
     * @return real producer (may be located in another subgraph)
     */
    static inline PiSDFVertex *getProducerRelaxed(PiSDFVertex *vertex, std::int32_t edgeIx, bool goDown = false);

    static inline PiSDFVertex *getTopParentVertex(PiSDFVertex *vertex,
                                                  std::int32_t edgeIx,
                                                  std::int32_t vertGlobInst,
                                                  std::int32_t *topVertGlobInst,
                                                  std::int32_t *topEdgeIx);

    static inline bool checkForInitIfInputIF(PiSDFVertex *vertex, std::int32_t edgeIx, std::int32_t vertGlobInst);

    static PiSDFEdge *computeDependenciesIxRelaxed(PiSDFVertex *vertex,
                                                   std::int32_t edgeIx,
                                                   std::int32_t vertexGlobInst,
                                                   PiSDFVertex **producer,
                                                   std::int32_t *firstDep,
                                                   std::int32_t *lastDep);

    static std::int32_t computeFirstDependencyIxRelaxed(PiSDFVertex *vertex,
                                                        std::int32_t edgeIx,
                                                        std::int32_t vertexGlobInst,
                                                        PiSDFVertex **producer);

    static std::int32_t computeLastDependencyIxRelaxed(PiSDFVertex *vertex,
                                                       std::int32_t edgeIx,
                                                       std::int32_t vertexGlobInst,
                                                       PiSDFVertex **producer);

    static inline std::int32_t fastCeilIntDiv(std::int32_t num, std::int32_t denom);

    static inline std::int32_t fastFloorIntDiv(std::int32_t num, std::int32_t denom);

private:

};

inline std::int32_t SRDAGLessIR::computeFirstDependencyIx(PiSDFVertex *vertex,
                                                          std::int32_t edgeIx,
                                                          std::int32_t vertexInstance) {
    auto *edge = vertex->getInEdge(edgeIx);
    auto cons = edge->resolveCons();
    auto prod = edge->resolveProd();
    auto delay = edge->resolveDelay();
    return computeFirstDependencyIx(cons, prod, delay, vertexInstance);
}

inline std::int32_t SRDAGLessIR::computeLastDependencyIx(PiSDFVertex *vertex,
                                                         std::int32_t edgeIx,
                                                         std::int32_t vertexInstance,
                                                         std::int32_t rho) {
    auto *edge = vertex->getInEdge(edgeIx);
    auto cons = edge->resolveCons();
    auto prod = edge->resolveProd();
    auto delay = edge->resolveDelay();
    return computeLastDependencyIx(cons, prod, delay, vertexInstance, rho, vertex->getBRVValue());
}

inline std::int32_t SRDAGLessIR::computeFirstDependencyIx(const std::int32_t cons,
                                                          const std::int32_t prod,
                                                          const std::int32_t delay,
                                                          std::int32_t vertexInstance) {
    return fastFloorIntDiv(cons * vertexInstance - delay, prod);
}

inline std::int32_t SRDAGLessIR::computeLastDependencyIx(const std::int32_t cons,
                                                         const std::int32_t prod,
                                                         const std::int32_t delay,
                                                         std::int32_t vertexInstance,
                                                         int32_t rho,
                                                         int32_t rv) {
    if (rho > 1) {
        return static_cast<int32_t>((cons * (vertexInstance + std::min(rho, rv - vertexInstance)) - delay - 1) / prod);
    }
    return fastFloorIntDiv(cons * (vertexInstance + 1) - delay - 1, prod);
}

inline PiSDFVertex *SRDAGLessIR::getProducer(PiSDFVertex *vertex,
                                             std::int32_t edgeIx,
                                             std::int32_t /*vertexInstance*/) {
    /** TODO: hand case of dynamic param **/
    return vertex->getInEdge(edgeIx)->getSrc();
}


inline std::int32_t SRDAGLessIR::fastCeilIntDiv(std::int32_t num, std::int32_t denom) {
    return static_cast<int32_t >(num / denom + (num % denom != 0));
}

std::int32_t SRDAGLessIR::fastFloorIntDiv(std::int32_t num, std::int32_t denom) {
    std::int32_t d = num / denom;
    std::int32_t r = num % denom;  /* optimizes into single division. */
    return r ? (d - ((num < 0) ^ (denom < 0))) : d;
}

inline PiSDFVertex *SRDAGLessIR::getProducerRelaxed(PiSDFVertex *vertex, std::int32_t edgeIx, bool goDown) {
    auto *edge = vertex->getInEdge(edgeIx);
    auto *srcVertex = edge->getSrc();
    /** First if needed we go though all input **/
    while (srcVertex->getType() == PISDF_TYPE_IF) {
        /** Get parent vertex **/
        vertex = vertex->getGraph()->getParentVertex();
        edge = vertex->getInEdge(srcVertex->getTypeId());
        srcVertex = edge->getSrc();
    }
    /** Secondly we down hierarchy if needed **/
    while (goDown && srcVertex->isHierarchical()) {
        /** Get child vertex **/
        vertex = srcVertex->getSubGraph()->getOutputIf(edge->getSrcPortIx());
        edge = vertex->getInEdge(0);
        srcVertex = edge->getSrc();
    }
    return srcVertex;
}

PiSDFVertex *SRDAGLessIR::getTopParentVertex(PiSDFVertex *vertex,
                                             std::int32_t edgeIx,
                                             std::int32_t vertGlobInst,
                                             std::int32_t *topVertGlobInst,
                                             std::int32_t *topEdgeIx) {
    auto *edge = vertex->getInEdge(edgeIx);
    auto *srcVertex = edge->getSrc();
    auto globInst = vertGlobInst;
    /** First if needed we go though all input **/
    while (srcVertex->getType() == PISDF_TYPE_IF) {
        globInst = globInst / vertex->getBRVValue();
        /** Get parent vertex **/
        vertex = vertex->getGraph()->getParentVertex();
        edge = vertex->getInEdge(srcVertex->getTypeId());
        srcVertex = edge->getSrc();
    }
    if (topVertGlobInst) {
        (*topVertGlobInst) = globInst;
    }
    if (topEdgeIx) {
        (*topEdgeIx) = edge->getSnkPortIx();
    }
    return vertex;
}

bool SRDAGLessIR::checkForInitIfInputIF(PiSDFVertex *vertex, std::int32_t edgeIx, std::int32_t vertGlobInst) {
    auto *edge = vertex->getInEdge(edgeIx);
    auto *srcVertex = edge->getSrc();
    if (srcVertex->getType() != PISDF_TYPE_IF) {
        return false;
    }
    auto globInst = vertGlobInst;
    auto locInst = vertGlobInst % vertex->getBRVValue();
    auto cons = edge->resolveCons();
    auto delay = edge->resolveDelay();
    while (srcVertex->getType() == PISDF_TYPE_IF) {
        if ((locInst * cons) > delay) {
            return true;
        }
        globInst = globInst / vertex->getBRVValue();
        /** Get parent vertex **/
        vertex = vertex->getGraph()->getParentVertex();
        /** Update value **/
        locInst = globInst % vertex->getBRVValue();
        edge = vertex->getInEdge(srcVertex->getTypeId());
        cons = edge->resolveCons();
        delay = edge->resolveDelay();
        srcVertex = edge->getSrc();
    }
    return false;
}


#endif //SPIDER_SRDAGLESSIR_H
