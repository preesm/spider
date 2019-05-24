/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018 - 2019)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018 - 2019)
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


PiSDFEdge* SRDAGLessIR::computeDependenciesIxRelaxed(PiSDFVertex *vertex,
                                               std::int32_t edgeIx,
                                               std::int32_t vertexGlobInst,
                                               PiSDFVertex **producer,
                                               std::int32_t *firstDep,
                                               std::int32_t *lastDep) {
    auto *edge = vertex->getInEdge(edgeIx);
    auto cons = edge->resolveCons();
    auto prod = edge->resolveProd();
    auto delay = edge->resolveDelay();
    auto vertexLocInst = vertexGlobInst % vertex->getBRVValue();
    auto *srcVertex = edge->getSrc();
    auto prevLocalProd = prod;
    auto localBRV = 0;
    auto localProd = 0;
    /** Computing \delta^{0}_{j, k} **/
    std::int32_t depLocInstFirst = computeFirstDependencyIx(cons, prod, delay, vertexLocInst);
    std::int32_t depGlobInstFirst = depLocInstFirst;
    /** Computing \delta^{1}_{j, k} **/
    std::int32_t depLocInstLast = computeLastDependencyIx(cons, prod, delay, vertexLocInst);
    std::int32_t depGlobInstLast = depLocInstLast;
    /**  k * c_j -  d_j only need to be computed once **/
    auto firstAnchor = vertexLocInst * cons - delay;
    /** (k + 1) * c_j -  d_j - 1 only need to be computed once **/
    auto lastAnchor = (vertexLocInst + 1) * cons - 1 - delay;
    /** K^{0}_{0} = 0 **/
    std::int32_t firstK = 0;
    /** K^{1}_{0} = 0 **/
    std::int32_t lastK = 0;
    while (srcVertex->isHierarchical()) {
        /** Updating values **/
        auto *subGraph = srcVertex->getSubGraph();
        auto *outputIf = subGraph->getOutputIf(edge->getSrcPortIx());
        edge = outputIf->getInEdge(0);
        srcVertex = edge->getSrc();
        /** Fetching P_n and q_{p_n} **/
        localProd = edge->resolveProd();
        localBRV = srcVertex->getBRVValue();

        /** Computing (\delta^{0, n - 1}_{j, k} + 1) * P_{n - 1} **/
        auto shiftFirstDep = (depLocInstFirst + 1) * prevLocalProd;
        /** Computing \delta^{0, n}_{j, k} **/
        depLocInstFirst = localBRV - fastCeilIntDiv(shiftFirstDep - firstAnchor - firstK, localProd);
        /** Updating K^{0}_{n} **/
        firstK = localBRV * localProd - shiftFirstDep - firstK;
        /** Update depGlobInstFirst **/
        depGlobInstFirst = depGlobInstFirst * localBRV + depLocInstFirst;

        /** Computing (\delta^{1, n - 1}_{j, k} + 1) * P_{n - 1} **/
        auto shiftLastDep = (depLocInstLast + 1) * prevLocalProd;
        /** Computing \delta^{1, n}_{j, k} **/
        depLocInstLast = localBRV - fastCeilIntDiv(shiftLastDep - lastAnchor - lastK, localProd);
        /** Updating K^{1}_{n} **/
        lastK = localBRV * localProd - shiftLastDep - lastK;
        /** Update depGlobInstFirst **/
        depGlobInstLast = depGlobInstLast * localBRV + depLocInstLast;

        /** Updating P_{n - 1} **/
        prevLocalProd = localProd;
    }
    if (producer) {
        (*producer) = srcVertex;
    }
    if (firstDep) {
        (*firstDep) = depGlobInstFirst;
    }
    if (lastDep) {
        (*lastDep) = depGlobInstLast;
    }
    return edge;
}

std::int32_t SRDAGLessIR::computeFirstDependencyIxRelaxed(PiSDFVertex *vertex,
                                                          std::int32_t edgeIx,
                                                          std::int32_t vertexGlobInst,
                                                          PiSDFVertex **producer) {
    auto *edge = vertex->getInEdge(edgeIx);
    auto cons = edge->resolveCons();
    auto prod = edge->resolveProd();
    auto delay = edge->resolveDelay();
    auto vertexLocInst = vertexGlobInst % vertex->getBRVValue();
    std::int32_t tokenOffsetK = 0;
    /** Computing \delta^{0}_{j, k} **/
    std::int32_t depLocInst = computeFirstDependencyIx(cons, prod, delay, vertexLocInst);
    std::int32_t depGlobInst = depLocInst;
    auto prevLocalProd = prod;
    auto localBRV = 0;
    auto localProd = 0;
    /** (k) * c_j - d_j only need to be computed once **/
    auto consumedTokens = (vertexLocInst) * cons - delay;
    auto *srcVertex = edge->getSrc();
    while (srcVertex->isHierarchical()) {
        auto *subGraph = srcVertex->getSubGraph();
        auto *outputIf = subGraph->getOutputIf(edge->getSrcPortIx());
        edge = outputIf->getInEdge(0);
        srcVertex = edge->getSrc();
        /** Fetching P_n and q_{p_n} **/
        localProd = edge->resolveProd();
        localBRV = srcVertex->getBRVValue();
        /** Computing (\delta^{0, n - 1}_{j, k} + 1) * P_{n - 1} **/
        auto shiftDep = (depLocInst + 1) * prevLocalProd;
        /** Computing \delta^{0, n}_{j, k} **/
        depLocInst = localBRV - fastCeilIntDiv(shiftDep - consumedTokens - tokenOffsetK, localProd);
        /** Updating K^{0}_{n} **/
        tokenOffsetK = localBRV * localProd - shiftDep + tokenOffsetK;
        /** Updating P_{n - 1} **/
        prevLocalProd = localProd;
        /** Update depGlobInst **/
        depGlobInst = depGlobInst * localBRV + depLocInst;
    }
    if (producer) {
        (*producer) = srcVertex;
    }
    return depGlobInst;
}


std::int32_t SRDAGLessIR::computeLastDependencyIxRelaxed(PiSDFVertex *vertex,
                                                         std::int32_t edgeIx,
                                                         std::int32_t vertexGlobInst,
                                                         PiSDFVertex **producer) {
    auto *edge = vertex->getInEdge(edgeIx);
    auto cons = edge->resolveCons();
    auto prod = edge->resolveProd();
    auto delay = edge->resolveDelay();
    auto vertexLocInst = vertexGlobInst % vertex->getBRVValue();
    std::int32_t tokenOffsetK = 0;
    /** Computing \delta^{1}_{j, k} **/
    std::int32_t depLocInst = computeLastDependencyIx(cons, prod, delay, vertexLocInst);
    std::int32_t depGlobInst = depLocInst;
    auto prevLocalProd = prod;
    auto localBRV = 0;
    auto localProd = 0;
    /** (k + 1) * c_j -  d_j - 1 only need to be computed once **/
    auto consumedTokens = (vertexLocInst + 1) * cons - 1 - delay;
    auto *srcVertex = edge->getSrc();
    while (srcVertex->isHierarchical()) {
        auto *subGraph = srcVertex->getSubGraph();
        auto *outputIf = subGraph->getOutputIf(edge->getSrcPortIx());
        edge = outputIf->getInEdge(0);
        srcVertex = edge->getSrc();
        /** Fetching P_n and q_{p_n} **/
        localProd = edge->resolveProd();
        localBRV = srcVertex->getBRVValue();
        /** Computing (\delta^{1, n - 1}_{j, k} + 1) * P_{n - 1} **/
        auto shiftDep = (depLocInst + 1) * prevLocalProd;
        /** Computing \delta^{1, n}_{j, k} **/
        depLocInst = localBRV - fastCeilIntDiv(shiftDep - consumedTokens - tokenOffsetK, localProd);
        /** Updating K^{1}_{n} **/
        tokenOffsetK = localBRV * localProd - shiftDep + tokenOffsetK;
        /** Updating P_{n - 1} **/
        prevLocalProd = localProd;
        /** Update depGlobInst **/
        depGlobInst = depGlobInst * localBRV + depLocInst;
    }
    if (producer) {
        (*producer) = srcVertex;
    }
    return depGlobInst;
}







