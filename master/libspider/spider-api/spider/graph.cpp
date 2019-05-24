/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2019)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2019)
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
#include <spider-api/user/graph.h>
#include <spider.h>
#include <spider/monitor/StackMonitor.h>
#include <spider/graphs/PiSDF/PiSDFCommon.h>
#include <spider/graphs/PiSDF/PiSDFGraph.h>
#include <spider/graphs/PiSDF/PiSDFParam.h>
#include <spider/graphs/PiSDF/PiSDFVertex.h>
#include <spider/graphs/PiSDF/PiSDFEdge.h>

/* === Vertex creation API === */

PiSDFGraph *Spider::createGraph(
        std::int32_t nEdges,
        std::int32_t nParams,
        std::int32_t nInIfs,
        std::int32_t nOutIfs,
        std::int32_t nConfigs,
        std::int32_t nBodies) {
    return CREATE(PISDF_STACK, PiSDFGraph)(
            /*Edges*/    nEdges,
            /*Params*/   nParams,
            /*InputIf*/  nInIfs,
            /*OutputIf*/ nOutIfs,
            /*Config*/   nConfigs,
            /*Body*/     nBodies);
}

PiSDFVertex *Spider::addBodyVertex(
        PiSDFGraph *graph,
        const char *vertexName,
        std::int32_t fctId,
        std::int32_t nInEdge,
        std::int32_t nOutEdge,
        std::int32_t nInParam) {
    return graph->addBodyVertex(
            vertexName,
            fctId,
            nInEdge,
            nOutEdge,
            nInParam);
}

PiSDFVertex *Spider::addBroadcastVertex(PiSDFGraph *graph,
                                        const char *vertexName,
                                        std::int32_t nOutEdge,
                                        std::int32_t nInParam) {
    return graph->addBodyVertex(
            vertexName,
            PISDF_SUBTYPE_BROADCAST,
            1,
            nOutEdge,
            nInParam);
}

PiSDFVertex *Spider::addRoundbufferVertex(PiSDFGraph *graph,
                                          const char *vertexName,
                                          std::int32_t nInEdge,
                                          std::int32_t nInParam) {
    return graph->addBodyVertex(
            vertexName,
            PISDF_SUBTYPE_ROUNDBUFFER,
            nInEdge,
            1,
            nInParam);
}

PiSDFVertex *Spider::addForkVertex(PiSDFGraph *graph,
                                   const char *vertexName,
                                   std::int32_t nOutEdge,
                                   std::int32_t nInParam) {
    return graph->addBodyVertex(
            vertexName,
            PISDF_SUBTYPE_FORK,
            1,
            nOutEdge,
            nInParam);
}

PiSDFVertex *Spider::addJoinVertex(PiSDFGraph *graph,
                                   const char *vertexName,
                                   std::int32_t nInEdge,
                                   std::int32_t nInParam) {
    return graph->addBodyVertex(
            vertexName,
            PISDF_SUBTYPE_JOIN,
            nInEdge,
            1,
            nInParam);
}

PiSDFVertex *Spider::addInitVertex(PiSDFGraph *graph,
                                   const char *vertexName,
                                   std::int32_t nInParam) {
    return graph->addBodyVertex(
            vertexName,
            PISDF_SUBTYPE_INIT,
            0,
            1,
            nInParam);
}

PiSDFVertex *Spider::addEndVertex(PiSDFGraph *graph,
                                  const char *vertexName,
                                  std::int32_t nInParam) {
    return graph->addBodyVertex(
            vertexName,
            PISDF_SUBTYPE_END,
            1,
            0,
            nInParam);
}

PiSDFVertex *Spider::addConfigVertex(
        PiSDFGraph *graph,
        const char *vertexName,
        std::int32_t fctId,
        std::int32_t nInEdge,
        std::int32_t nOutEdge,
        std::int32_t nInParam,
        std::int32_t nOutParam) {
    return graph->addConfigVertex(
            vertexName,
            fctId,
            PISDF_SUBTYPE_NORMAL,
            nInEdge,
            nOutEdge,
            nInParam,
            nOutParam);
}

PiSDFVertex *Spider::addInputIf(
        PiSDFGraph *graph,
        const char *name,
        std::int32_t nInParam) {
    return graph->addInputIf(
            name,
            nInParam);
}

PiSDFVertex *Spider::addOutputIf(
        PiSDFGraph *graph,
        const char *name,
        std::int32_t nInParam) {
    return graph->addOutputIf(
            name,
            nInParam);
}

/* === Static Param method === */

static inline PiSDFParam *addDependentParam(PiSDFGraph *graph,
                                            const char *name,
                                            const char *expr,
                                            std::initializer_list<PiSDFParam *> dependencies,
                                            PiSDFParamType type) {
    auto *param = CREATE(PISDF_STACK, PiSDFParam)(name, expr, type, graph, graph->getNParam(), dependencies);
    graph->addPiSDFParam(param);
    return param;
}

/* === Param creation API === */

PiSDFParam *Spider::addStaticParam(PiSDFGraph *graph,
                                   const char *name,
                                   Param value) {
    auto *param = CREATE(PISDF_STACK, PiSDFParam)(name, std::to_string(value).c_str(), PISDF_PARAM_STATIC, graph,
                                                  graph->getNParam());
    param->setValue(value);
    graph->addPiSDFParam(param);
    return param;
}

PiSDFParam *Spider::addStaticDependentParam(PiSDFGraph *graph,
                                            const char *name,
                                            const char *expr,
                                            std::initializer_list<PiSDFParam *> dependencies) {
    return addDependentParam(graph, name, expr, dependencies, PISDF_PARAM_STATIC);
}

PiSDFParam *Spider::addDynamicParam(PiSDFGraph *graph,
                                    const char *name) {
    auto *param = CREATE(PISDF_STACK, PiSDFParam)(name, "", PISDF_PARAM_DYNAMIC, graph, graph->getNParam());
    graph->addPiSDFParam(param);
    return param;
}

PiSDFParam *Spider::addDynamicDependentParam(PiSDFGraph *graph,
                                             const char *name,
                                             const char *expr,
                                             std::initializer_list<PiSDFParam *> dependencies) {
    return addDependentParam(graph, name, expr, dependencies, PISDF_PARAM_DYNAMIC);
}

PiSDFParam *Spider::addInheritedParam(PiSDFGraph *graph,
                                      const char *name,
                                      std::int32_t parentId) {
    auto *parentVertex = graph->getParentVertex();
    auto *heritedParam = (PiSDFParam *) parentVertex->getInParam(parentId);
    auto *param = CREATE(PISDF_STACK, PiSDFParam)(name, "", PISDF_PARAM_HERITED, graph, graph->getNParam());
    param->setInheritedParameter(heritedParam);
    graph->addPiSDFParam(param);
    return param;
}

/* === PiSDF methods API === */

void Spider::addSubGraph(PiSDFVertex *hierVertex, PiSDFGraph *subgraph) {
    hierVertex->setSubGraph(subgraph);
    subgraph->setParentVertex(hierVertex);
}

PiSDFEdge *Spider::addEdge(PiSDFGraph *graph,
                           PiSDFVertex *src,
                           std::int32_t srcPortId,
                           const char *prodExpression,
                           PiSDFVertex *snk,
                           std::int32_t snkPortId,
                           const char *consExpression) {
    auto *edge = graph->addEdge();
    edge->connectSrc(src, srcPortId, prodExpression);
    edge->connectSnk(snk, snkPortId, consExpression);
    src->connectOutEdge(srcPortId, edge);
    snk->connectInEdge(snkPortId, edge);
    return edge;
}

void Spider::addDelay(PiSDFEdge *edge,
                      const char *delayExpression,
                      PiSDFVertex *setter,
                      PiSDFVertex *getter,
                      bool isDelayPersistent) {
    edge->setDelay(delayExpression, setter, getter, isDelayPersistent);
}

void Spider::addInParam(PiSDFVertex *vertex, std::int32_t ix, PiSDFParam *param) {
    vertex->addInParam(ix, param);
}

void Spider::addOutParam(PiSDFVertex *vertex, std::int32_t ix, PiSDFParam *param) {
    vertex->addOutParam(ix, param);
}

void Spider::setTimingOnType(PiSDFVertex *vertex, std::int32_t peType, const char *timing) {
    vertex->setTimingOnType(peType, timing);
}

void Spider::isExecutableOnAllPE(PiSDFVertex *vertex) {
    vertex->isExecutableOnAllPE();
}

void Spider::isExecutableOnPE(PiSDFVertex *vertex, std::int32_t pe) {
    vertex->isExecutableOnPE(pe);
}

void Spider::isExecutableOnPEType(PiSDFVertex *vertex, std::uint32_t peType) {
    const auto *archi = Spider::getArchi();
    for (std::uint32_t pe = 0; pe < archi->getNPE(); pe++) {
        if (archi->getPEFromSpiderID(pe)->getHardwareType() == peType) vertex->isExecutableOnPE(pe);
    }
}


