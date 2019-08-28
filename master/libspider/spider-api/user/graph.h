/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2019)
 * Daniel Madroñal <daniel.madronal@upm.es> (2019)
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
#ifndef SPIDER_GRAPH_API_H
#define SPIDER_GRAPH_API_H

#include <cstdint>
#include <map>

/* === Forward declarations === */

class PiSDFVertex;

class PiSDFParam;

class PiSDFGraph;

class PiSDFEdge;

class SRDAGGraph;

/* === Type declarations === */

using Param = std::int64_t;

/* === API methods === */

namespace Spider {

    /* === Vertex creation API === */

    PiSDFGraph *createGraph(
            std::int32_t nEdges,
            std::int32_t nParams,
            std::int32_t nInIfs,
            std::int32_t nOutIfs,
            std::int32_t nConfigs,
            std::int32_t nBodies);

    PiSDFVertex *addBodyVertex(
            PiSDFGraph *graph,
            const char *vertexName,
            std::int32_t fctId,
            std::int32_t nInEdge,
            std::int32_t nOutEdge,
            std::int32_t nInParam);

    PiSDFVertex *addBroadcastVertex(
            PiSDFGraph *graph,
            const char *vertexName,
            std::int32_t nOutEdge,
            std::int32_t nInParam
    );

    PiSDFVertex *addRoundbufferVertex(
            PiSDFGraph *graph,
            const char *vertexName,
            std::int32_t nInEdge,
            std::int32_t nInParam
    );

    PiSDFVertex *addForkVertex(
            PiSDFGraph *graph,
            const char *vertexName,
            std::int32_t nOutEdge,
            std::int32_t nInParam
    );

    PiSDFVertex *addJoinVertex(
            PiSDFGraph *graph,
            const char *vertexName,
            std::int32_t nInEdge,
            std::int32_t nInParam
    );

    PiSDFVertex *addInitVertex(
            PiSDFGraph *graph,
            const char *vertexName,
            std::int32_t nInParam
    );

    PiSDFVertex *addEndVertex(
            PiSDFGraph *graph,
            const char *vertexName,
            std::int32_t nInParam
    );

    PiSDFVertex *addConfigVertex(
            PiSDFGraph *graph,
            const char *vertexName,
            std::int32_t fctId,
            std::int32_t nInEdge,
            std::int32_t nOutEdge,
            std::int32_t nInParam,
            std::int32_t nOutParam);

    PiSDFVertex *addInputIf(
            PiSDFGraph *graph,
            const char *name,
            std::int32_t nInParam);

    PiSDFVertex *addOutputIf(
            PiSDFGraph *graph,
            const char *name,
            std::int32_t nInParam);

    /* === Param creation API === */

    PiSDFParam *addStaticParam(PiSDFGraph *graph,
                               const char *name,
                               Param value);

    PiSDFParam *addStaticDependentParam(PiSDFGraph *graph,
                                        const char *name,
                                        const char *expr,
                                        std::initializer_list<PiSDFParam *> dependencies);

    PiSDFParam *addDynamicParam(PiSDFGraph *graph,
                                const char *name);

    PiSDFParam *addDynamicDependentParam(PiSDFGraph *graph,
                                         const char *name,
                                         const char *expr,
                                         std::initializer_list<PiSDFParam *> dependencies);

    PiSDFParam *addInheritedParam(PiSDFGraph *graph,
                                  const char *name,
                                  std::int32_t parentId);

    /* === PiSDF methods API === */

    void addSubGraph(PiSDFVertex *hierVertex, PiSDFGraph *subgraph);

    PiSDFEdge *addEdge(PiSDFGraph *graph,
                       PiSDFVertex *src,
                       std::int32_t srcPortId,
                       const char *prodExpression,
                       PiSDFVertex *snk,
                       std::int32_t snkPortId,
                       const char *consExpression);

    void addDelay(PiSDFEdge *edge,
                  const char *delayExpression,
                  PiSDFVertex *setter = nullptr,
                  PiSDFVertex *getter = nullptr,
                  bool isDelayPersistent = false);

    void addInParam(PiSDFVertex *vertex, std::int32_t ix, PiSDFParam *param);

    void addOutParam(PiSDFVertex *vertex, std::int32_t ix, PiSDFParam *param);

    void setTimingOnType(PiSDFVertex *vertex, std::int32_t peType, const char *timing);

    void setEnergyOnType(PiSDFVertex *vertex, std::int32_t peType, double energy);

    void isExecutableOnAllPE(PiSDFVertex *vertex);

    void isExecutableOnPE(PiSDFVertex *vertex, std::int32_t pe);

    void isExecutableOnPEType(PiSDFVertex *vertex, std::uint32_t peType);
}

#endif //SPIDER_GRAPH_H
