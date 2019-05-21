/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018 - 2019)
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
#include <graphs/PiSDF/PiSDFEdge.h>
#include <graphTransfo/ComputeBRV.h>
#include <graphTransfo/LCM.h>
#include "TopologyMatrix.h"

static inline std::int32_t getVertexIx(PiSDFVertex *const vertex) {
    std::int32_t offsetInputIF = ((vertex->getType() == PISDF_TYPE_IF) * vertex->getGraph()->getNBody());
    std::int32_t offsetOutputIF = ((vertex->getSubType() == PISDF_SUBTYPE_OUTPUT_IF) * vertex->getGraph()->getNInIf());
    return vertex->getTypeId() + offsetInputIF + offsetOutputIF;
}

static void fillVertexSet(PiSDFVertexSet &vertexSet, long &sizeEdgeSet, PiSDFVertex **keyVertexSet) {
    int currentSize = 0;
    int n = vertexSet.size() - 1;
    do {
        currentSize = vertexSet.size();
        auto *current = vertexSet.getArray()[n];
        // 0. Do the output edges
        for (int i = 0; i < current->getNOutEdge(); ++i) {
            auto *edge = current->getOutEdge(i);
            if (!edge) {
                throwSpiderException("Vertex [%s] has NULL edge", current->getName());
            }
            auto *vertex = edge->getSnk();
            auto vertexIx = getVertexIx(vertex);
            if (!keyVertexSet[vertexIx]) {
                vertexSet.add(vertex);
                keyVertexSet[vertexIx] = vertex;
            }
            sizeEdgeSet++;
        }
        // 1. Do the input edges
        for (int i = 0; i < current->getNInEdge(); ++i) {
            auto *edge = current->getInEdge(i);
            if (!edge) {
                throwSpiderException("Vertex [%s] has NULL edge", current->getName());
            }
            auto *vertex = edge->getSrc();
            auto vertexIx = getVertexIx(vertex);
            if (!keyVertexSet[vertexIx]) {
                vertexSet.add(vertex);
                keyVertexSet[vertexIx] = vertex;
            }
        }
        n++;
    } while ((vertexSet.size() != currentSize) || (n != vertexSet.size()));
}

void computeBRV(PiSDFGraph *const graph, int *brv) {
    // Retrieve the graph
    int nTotalVertices = graph->getNBody() + graph->getNInIf() + graph->getNOutIf();
    PiSDFVertexSet vertexSet(nTotalVertices, TRANSFO_STACK);
    auto **keyVertexSet = CREATE_MUL_NA(TRANSFO_STACK, nTotalVertices, PiSDFVertex*);
    for (int ix = 0; ix < nTotalVertices; ++ix) {
        keyVertexSet[ix] = nullptr;
    }

    // 0. First we need to get all different connected components
    long nDoneVertices = 0;
    for (int i = 0; i < graph->getNBody(); i++) {
        auto *vertex = graph->getBody(i);
        auto vertexIx = getVertexIx(vertex);
        if (!keyVertexSet[vertexIx]) {
            long nEdges = 0;
            vertexSet.add(vertex);
            keyVertexSet[vertexIx] = vertex;
            // 1. Fill up the vertexSet
            fillVertexSet(vertexSet, nEdges, keyVertexSet);
            // 1.1 Update the offset in the vertexSet
            long nVertices = vertexSet.size() - nDoneVertices;
            // 2. Compute the BRV of current set
            lcmBasedBRV(vertexSet, nDoneVertices, nVertices, nEdges, brv);
            // 3. Update the number of treated vertices
            nDoneVertices = vertexSet.size();
        }
        vertex->setBRVValue(brv[i]);
    }
    for (std::int32_t i = 0; i < graph->getNInIf(); ++i) {
        auto *vertex = graph->getInputIf(i);
        vertex->setBRVValue(1);
    }
    for (std::int32_t i = 0; i < graph->getNOutIf(); ++i) {
        auto *vertex = graph->getOutputIf(i);
        vertex->setBRVValue(1);
    }
    while (vertexSet.size() > 0) {
        vertexSet.del(vertexSet[0]);
    }
    StackMonitor::free(TRANSFO_STACK, keyVertexSet);
}


void computeHierarchicalBRV(PiSDFGraph *const graph) {
    auto *brv = CREATE_MUL(TRANSFO_STACK, graph->getNBody(), std::int32_t);
    computeBRV(graph, brv);
    StackMonitor::free(TRANSFO_STACK, brv);
    for (std::int32_t i = 0; i < graph->getNBody(); ++i) {
        auto *vertex = graph->getBody(i);
        if (vertex->isHierarchical()){
            computeHierarchicalBRV(vertex->getSubGraph());
        }
    }
}




