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

    static inline std::int32_t computeFirstDependencyIx(const std::int32_t &cons,
                                                        const std::int32_t &prod,
                                                        const std::int32_t &delay,
                                                        std::int32_t vertexInstance);

    static inline std::int32_t computeLastDependencyIx(PiSDFVertex *vertex,
                                                       std::int32_t edgeIx,
                                                       std::int32_t vertexInstance,
                                                       std::int32_t rho = 1,
                                                       std::int32_t rv = 1);

    static inline std::int32_t computeLastDependencyIx(const std::int32_t &cons,
                                                       const std::int32_t &prod,
                                                       const std::int32_t &delay,
                                                       std::int32_t vertexInstance,
                                                       std::int32_t rho = 1,
                                                       std::int32_t rv = 1);

    static inline std::int32_t computeFirstDependencyIxRelaxed(PiSDFVertex *vertex,
                                                               std::int32_t edgeIx,
                                                               std::int32_t vertexInstance,
                                                               std::int32_t level = -1);

    static inline std::int32_t computeLastDependencyIxRelaxed(PiSDFVertex *vertex,
                                                              std::int32_t edgeIx,
                                                              std::int32_t vertexInstance,
                                                              std::int32_t level = -1,
                                                              std::int32_t rho = 1,
                                                              std::int32_t rv = 1);

    static inline PiSDFVertex *getProducer(PiSDFVertex *vertex,
                                           std::int32_t edgeIx,
                                           std::int32_t vertexInstance);

    static inline std::int32_t fastCeilIntDiv(std::int32_t num, std::int32_t denom);

private:

};

std::int32_t SRDAGLessIR::computeFirstDependencyIx(PiSDFVertex *vertex,
                                                   std::int32_t edgeIx,
                                                   std::int32_t vertexInstance) {
    auto *edge = vertex->getInEdge(edgeIx);
    auto cons = edge->resolveCons();
    auto prod = edge->resolveProd();
    auto delay = edge->resolveDelay();
    return static_cast<int32_t>((cons * vertexInstance - delay) / prod);
}

std::int32_t
SRDAGLessIR::computeFirstDependencyIx(const std::int32_t &cons,
                                      const std::int32_t &prod,
                                      const std::int32_t &delay,
                                      std::int32_t vertexInstance) {
    return static_cast<int32_t>((cons * vertexInstance - delay) / prod);
}


std::int32_t SRDAGLessIR::computeLastDependencyIx(PiSDFVertex *vertex,
                                                  std::int32_t edgeIx,
                                                  std::int32_t vertexInstance,
                                                  std::int32_t rho,
                                                  std::int32_t rv) {
    auto *edge = vertex->getInEdge(edgeIx);
    auto cons = edge->resolveCons();
    auto prod = edge->resolveProd();
    auto delay = edge->resolveDelay();
    if (rho > 1) {
        return static_cast<int32_t>((cons * (vertexInstance + std::min(rho, rv - vertexInstance)) - delay - 1) / prod);
    }
    return static_cast<int32_t>((cons * (vertexInstance + 1) - delay - 1) / prod);
}

std::int32_t
SRDAGLessIR::computeLastDependencyIx(const std::int32_t &cons,
                                     const std::int32_t &prod,
                                     const std::int32_t &delay,
                                     std::int32_t vertexInstance,
                                     int32_t rho,
                                     int32_t rv) {
    if (rho > 1) {
        return static_cast<int32_t>((cons * (vertexInstance + std::min(rho, rv - vertexInstance)) - delay - 1) / prod);
    }
    return static_cast<int32_t>((cons * (vertexInstance + 1) - delay - 1) / prod);
}

PiSDFVertex *SRDAGLessIR::getProducer(PiSDFVertex *vertex, std::int32_t edgeIx, std::int32_t /*vertexInstance*/) {
    auto *edge = vertex->getInEdge(edgeIx);
    return edge->getSrc();
}

std::int32_t
SRDAGLessIR::computeFirstDependencyIxRelaxed(PiSDFVertex *vertex,
                                             std::int32_t edgeIx,
                                             std::int32_t vertexInstance,
                                             int32_t level) {
    auto *edge = vertex->getInEdge(edgeIx);
    auto cons = edge->resolveCons();
    auto prod = edge->resolveProd();
    auto delay = edge->resolveDelay();
    std::int32_t tokenOffsetK = 0;
    /** Computing \delta^{0}_{j, k} **/
    std::int32_t currentDep = computeFirstDependencyIx(cons, prod, delay, vertexInstance);
    auto currentProd = prod;
    auto currentRep = 0;
    auto nextProd = 0;
    /** k * c_j - d_j only need to be computed once **/
    auto consumedTokens = vertexInstance * cons - delay;
    for (int i = 0; i < level; ++i) {
        /** Fetching P_n and q_{p_n} **/
        nextProd = 0;
        currentRep = 0;
        /** Computing (\delta^{0, n - 1}_{j, k} + 1) * P_{n - 1} **/
        auto shiftDep = (currentDep + 1) * currentProd;
        /** Computing \delta^{0, n}_{j, k} **/
        currentDep = currentRep -
                     fastCeilIntDiv(shiftDep - consumedTokens - tokenOffsetK, nextProd);
        /** Updating K^{0}_{n} **/
        tokenOffsetK = currentRep * nextProd - shiftDep - tokenOffsetK;
    }
    /** Return  \delta^{0, level - 1}_{j, k} **/
    return currentDep;
}

std::int32_t
SRDAGLessIR::computeLastDependencyIxRelaxed(PiSDFVertex *vertex,
                                            std::int32_t edgeIx,
                                            std::int32_t vertexInstance,
                                            std::int32_t level,
                                            int32_t rho,
                                            int32_t rv) {
    auto *edge = vertex->getInEdge(edgeIx);
    auto cons = edge->resolveCons();
    auto prod = edge->resolveProd();
    auto delay = edge->resolveDelay();
    std::int32_t tokenOffsetK = 0;
    /** Computing \delta^{1}_{j, k} **/
    std::int32_t currentDep = computeLastDependencyIx(cons, prod, delay, vertexInstance, rho, rv);
    auto currentProd = prod;
    auto currentRep = 0;
    auto nextProd = 0;
    /** (k + 1) * c_j -  d_j - 1 only need to be computed once **/
    auto consumedTokens = (vertexInstance + 1) * cons - 1 - delay;
    for (int i = 0; i < level; ++i) {
        /** Fetching P_n and q_{p_n} **/
        nextProd = 0;
        currentRep = 0;
        /** Computing (\delta^{1, n - 1}_{j, k} + 1) * P_{n - 1} **/
        auto shiftDep = (currentDep + 1) * currentProd;
        /** Computing \delta^{1, n}_{j, k} **/
        currentDep = currentRep -
                     fastCeilIntDiv(shiftDep - consumedTokens - tokenOffsetK, nextProd);
        /** Updating K^{1}_{n} **/
        tokenOffsetK = currentRep * nextProd - shiftDep - tokenOffsetK;
    }
    /** Return  \delta^{1, level - 1}_{j, k} **/
    return currentDep;
}

inline std::int32_t SRDAGLessIR::fastCeilIntDiv(std::int32_t num, std::int32_t denom) {
    return static_cast<int32_t >(num / denom + (num % denom != 0));
}


#endif //SPIDER_SRDAGLESSIR_H
