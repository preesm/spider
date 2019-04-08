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
#include "SRDAGLessIR.h"

void SRDAGLessIR::computeDependenciesIxFromInputIF(PiSDFVertex *vertex,
                                                   std::int32_t *edgeIx,
                                                   const std::int32_t *instancesArray,
                                                   PiSDFVertex **producer,
                                                   std::int32_t *deltaStart,
                                                   std::int32_t *deltaEnd) {
    auto *edge = vertex->getInEdge(*edgeIx);
    Param cons = 0;
    Param delay = 0;
    std::int32_t vertexIx = vertex->getId() - 1;
    std::int32_t vertexInstance = instancesArray[vertexIx];
    std::int32_t inputIfIx = 0;
    auto *srcVertex = edge->getSrc();
    while (srcVertex->getType() == PISDF_TYPE_IF) {
        /** Compute values here (little optim) **/
        cons = edge->resolveCons();
        delay = edge->resolveDelay();
        /** We hit a delay **/
        if ((vertexInstance * cons) < delay) {
            break;
        }
        /** Let's forward direct inheritence of dependencies **/
        vertex = vertex->getGraph()->getParentVertex();
        inputIfIx = srcVertex->getTypeId();
        /** Update values **/
        edge = vertex->getInEdge(inputIfIx);
        srcVertex = edge->getSrc();
        vertexIx = vertex->getId() - 1;
        vertexInstance = instancesArray[vertexIx];
    }
    /** We have reached top level of dependency **/
    (*edgeIx) = edge->getSrcPortIx();
    if (deltaStart) {
        // (*deltaStart) = computeFirstDependencyIxRelaxed(edge, vertexInstance, producer);
        (*deltaStart) = computeFirstDependencyIx(vertex, inputIfIx, vertexInstance);
    }
    if (deltaEnd) {
        // (*deltaEnd) = computeLastDependencyIxRelaxed(edge, vertexInstance, producer);
        (*deltaEnd) = computeLastDependencyIx(vertex, inputIfIx, vertexInstance);
    }
    if (producer) {
        (*producer) = srcVertex;
    }
}

std::int32_t
SRDAGLessIR::computeFirstDependencyIxRelaxed(PiSDFVertex *vertex,
                                             std::int32_t edgeIx,
                                             std::int32_t vertexInstance,
                                             int32_t /*level*/) {
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
    auto *srcVertex = edge->getSrc();
    while (srcVertex->isHierarchical()) {
        auto *subGraph = srcVertex->getSubGraph();
        auto *outputIf = subGraph->getOutputIf(edge->getSrcPortIx());
        edge = outputIf->getInEdge(0);
        srcVertex = edge->getSrc();
        /** Fetching P_n and q_{p_n} **/
        nextProd = edge->resolveProd();
        currentRep = srcVertex->getBRVValue();
        /** Computing (\delta^{0, n - 1}_{j, k} + 1) * P_{n - 1} **/
        auto shiftDep = (currentDep + 1) * currentProd;
        /** Computing \delta^{0, n}_{j, k} **/
        currentDep = currentRep -
                     fastCeilIntDiv(shiftDep - consumedTokens - tokenOffsetK, nextProd);
        /** Updating K^{0}_{n} **/
        tokenOffsetK = currentRep * nextProd - shiftDep - tokenOffsetK;
        /** Updating P_{n - 1} **/
        currentProd = nextProd;
    }
    /** Return  \delta^{0, level - 1}_{j, k} **/
    return currentDep;
}

std::int32_t
SRDAGLessIR::computeLastDependencyIxRelaxed(PiSDFVertex *vertex,
                                            std::int32_t edgeIx,
                                            std::int32_t vertexInstance,
                                            std::int32_t /*level*/,
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
    auto *srcVertex = edge->getSrc();
    while (srcVertex->isHierarchical()) {
        auto *subGraph = srcVertex->getSubGraph();
        auto *outputIf = subGraph->getOutputIf(edge->getSrcPortIx());
        edge = outputIf->getInEdge(0);
        srcVertex = edge->getSrc();
        /** Fetching P_n and q_{p_n} **/
        nextProd = edge->resolveProd();
        currentRep = srcVertex->getBRVValue();
        /** Computing (\delta^{1, n - 1}_{j, k} + 1) * P_{n - 1} **/
        auto shiftDep = (currentDep + 1) * currentProd;
        /** Computing \delta^{1, n}_{j, k} **/
        currentDep = currentRep -
                     fastCeilIntDiv(shiftDep - consumedTokens - tokenOffsetK, nextProd);
        /** Updating K^{1}_{n} **/
        tokenOffsetK = currentRep * nextProd - shiftDep - tokenOffsetK;
        /** Updating P_{n - 1} **/
        currentProd = nextProd;
    }
    /** Return  \delta^{1, level - 1}_{j, k} **/
    return currentDep;
}

std::int32_t SRDAGLessIR::computeFirstDependencyIxRelaxed(PiSDFEdge *edge,
                                                          std::int32_t vertexInstance,
                                                          PiSDFVertex **producer,
                                                          int32_t /*level*/) {
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
    auto *srcVertex = edge->getSrc();
    while (srcVertex->isHierarchical()) {
        auto *subGraph = srcVertex->getSubGraph();
        auto *outputIf = subGraph->getOutputIf(edge->getSrcPortIx());
        edge = outputIf->getInEdge(0);
        srcVertex = edge->getSrc();
        /** Fetching P_n and q_{p_n} **/
        nextProd = edge->resolveProd();
        currentRep = srcVertex->getBRVValue();
        /** Computing (\delta^{0, n - 1}_{j, k} + 1) * P_{n - 1} **/
        auto shiftDep = (currentDep + 1) * currentProd;
        /** Computing \delta^{0, n}_{j, k} **/
        currentDep = currentRep -
                     fastCeilIntDiv(shiftDep - consumedTokens - tokenOffsetK, nextProd);
        /** Updating K^{0}_{n} **/
        tokenOffsetK = currentRep * nextProd - shiftDep - tokenOffsetK;
        /** Updating P_{n - 1} **/
        currentProd = nextProd;
    }
    if (producer) {
        (*producer) = srcVertex;
    }
    /** Return  \delta^{0, level - 1}_{j, k} **/
    return currentDep;
}

std::int32_t SRDAGLessIR::computeLastDependencyIxRelaxed(PiSDFEdge *edge,
                                                         std::int32_t vertexInstance,
                                                         PiSDFVertex **producer,
                                                         int32_t /*level*/,
                                                         int32_t rho,
                                                         int32_t rv) {
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
    auto *srcVertex = edge->getSrc();
    while (srcVertex->isHierarchical()) {
        auto *subGraph = srcVertex->getSubGraph();
        auto *outputIf = subGraph->getOutputIf(edge->getSrcPortIx());
        edge = outputIf->getInEdge(0);
        srcVertex = edge->getSrc();
        /** Fetching P_n and q_{p_n} **/
        nextProd = edge->resolveProd();
        currentRep = srcVertex->getBRVValue();
        /** Computing (\delta^{1, n - 1}_{j, k} + 1) * P_{n - 1} **/
        auto shiftDep = (currentDep + 1) * currentProd;
        /** Computing \delta^{1, n}_{j, k} **/
        currentDep = currentRep -
                     fastCeilIntDiv(shiftDep - consumedTokens - tokenOffsetK, nextProd);
        /** Updating K^{1}_{n} **/
        tokenOffsetK = currentRep * nextProd - shiftDep - tokenOffsetK;
        /** Updating P_{n - 1} **/
        currentProd = nextProd;
    }
    if (producer) {
        (*producer) = srcVertex;
    }
    /** Return  \delta^{1, level - 1}_{j, k} **/
    return currentDep;
}

PiSDFEdge *SRDAGLessIR::computeDependenciesIx(PiSDFVertex *vertex,
                                              std::int32_t edgeIx,
                                              const std::int32_t *instancesArray,
                                              PiSDFVertex **producer,
                                              std::vector<std::int32_t> &indexesStart,
                                              std::vector<std::int32_t> &indexesLast) {
    auto *edge = vertex->getInEdge(edgeIx);
    Param cons = 0;
    Param delay = 0;
    std::int32_t vertexIx = vertex->getId() - 1;
    std::int32_t vertexInstance = instancesArray[vertexIx];
    std::int32_t inputIfIx = 0;
    auto *srcVertex = edge->getSrc();
    while (srcVertex->getType() == PISDF_TYPE_IF) {
        /** Compute values here (little optim) **/
        cons = edge->resolveCons();
        delay = edge->resolveDelay();
        /** We hit a delay **/
        if ((vertexInstance * cons) < delay) {
            indexesStart.push_back(-1);
            return edge;
        }
        /** Let's forward direct inheritence of dependencies **/
        vertex = vertex->getGraph()->getParentVertex();
        inputIfIx = srcVertex->getTypeId();
        /** Update values **/
        edge = vertex->getInEdge(inputIfIx);
        srcVertex = edge->getSrc();
        /** Update values **/
        edge = vertex->getInEdge(inputIfIx);
        srcVertex = edge->getSrc();
        vertexIx = vertex->getId() - 1;
        vertexInstance = instancesArray[vertexIx];
    }
    /** We have reached top level of dependency **/
    SRDAGLessIR::computeFirstDependencyIxRelaxed(edge,
                                                 vertexInstance,
                                                 producer,
                                                 indexesStart);
    SRDAGLessIR::computeLastDependencyIxRelaxed(edge,
                                                vertexInstance,
                                                producer,
                                                indexesLast);
    return edge;

}

void SRDAGLessIR::computeFirstDependencyIxRelaxed(PiSDFEdge *edge,
                                                  std::int32_t vertexInstance,
                                                  PiSDFVertex **producer,
                                                  std::vector<std::int32_t> &indexes,
                                                  int32_t /*level*/) {
    auto cons = edge->resolveCons();
    auto prod = edge->resolveProd();
    auto delay = edge->resolveDelay();
    std::int32_t tokenOffsetK = 0;
    /** Computing \delta^{0}_{j, k} **/
    std::int32_t currentDep = computeFirstDependencyIx(cons, prod, delay, vertexInstance);
    /** Save top level dependency **/
    indexes.push_back(currentDep);
    auto currentProd = prod;
    auto currentRep = 0;
    auto nextProd = 0;
    /** k * c_j - d_j only need to be computed once **/
    auto consumedTokens = vertexInstance * cons - delay;
    auto *srcVertex = edge->getSrc();
    while (srcVertex->isHierarchical()) {
        auto *subGraph = srcVertex->getSubGraph();
        auto *outputIf = subGraph->getOutputIf(edge->getSrcPortIx());
        edge = outputIf->getInEdge(0);
        srcVertex = edge->getSrc();
        /** Fetching P_n and q_{p_n} **/
        nextProd = edge->resolveProd();
        currentRep = srcVertex->getBRVValue();
        /** Computing (\delta^{0, n - 1}_{j, k} + 1) * P_{n - 1} **/
        auto shiftDep = (currentDep + 1) * currentProd;
        /** Computing \delta^{0, n}_{j, k} **/
        currentDep = currentRep -
                     fastCeilIntDiv(shiftDep - consumedTokens - tokenOffsetK, nextProd);
        /** Updating K^{0}_{n} **/
        tokenOffsetK = currentRep * nextProd - shiftDep - tokenOffsetK;
        /** Updating P_{n - 1} **/
        currentProd = nextProd;
        /** Save  \delta^{0, n}_{j, k} **/
        indexes.push_back(currentDep);
    }
    /** Set the last final true producer **/
    if (producer) {
        (*producer) = srcVertex;
    }
}

void SRDAGLessIR::computeLastDependencyIxRelaxed(PiSDFEdge *edge,
                                                 std::int32_t vertexInstance,
                                                 PiSDFVertex **producer,
                                                 std::vector<std::int32_t> &indexes,
                                                 int32_t /*level*/,
                                                 int32_t rho,
                                                 int32_t rv) {
    auto cons = edge->resolveCons();
    auto prod = edge->resolveProd();
    auto delay = edge->resolveDelay();
    std::int32_t tokenOffsetK = 0;
    /** Computing \delta^{1}_{j, k} **/
    std::int32_t currentDep = computeLastDependencyIx(cons, prod, delay, vertexInstance, rho, rv);
    /** Save top level dependency **/
    indexes.push_back(currentDep);
    auto currentProd = prod;
    auto currentRep = 0;
    auto nextProd = 0;
    /** (k + 1) * c_j -  d_j - 1 only need to be computed once **/
    auto consumedTokens = (vertexInstance + 1) * cons - 1 - delay;
    auto *srcVertex = edge->getSrc();
    while (srcVertex->isHierarchical()) {
        auto *subGraph = srcVertex->getSubGraph();
        auto *outputIf = subGraph->getOutputIf(edge->getSrcPortIx());
        edge = outputIf->getInEdge(0);
        srcVertex = edge->getSrc();
        /** Fetching P_n and q_{p_n} **/
        nextProd = edge->resolveProd();
        currentRep = srcVertex->getBRVValue();
        /** Computing (\delta^{1, n - 1}_{j, k} + 1) * P_{n - 1} **/
        auto shiftDep = (currentDep + 1) * currentProd;
        /** Computing \delta^{1, n}_{j, k} **/
        currentDep = currentRep -
                     fastCeilIntDiv(shiftDep - consumedTokens - tokenOffsetK, nextProd);
        /** Updating K^{1}_{n} **/
        tokenOffsetK = currentRep * nextProd - shiftDep - tokenOffsetK;
        /** Updating P_{n - 1} **/
        currentProd = nextProd;
        /** Save  \delta^{1, n}_{j, k} **/
        indexes.push_back(currentDep);
    }
    /** Set the last final true producer **/
    if (producer) {
        (*producer) = srcVertex;
    }
}




