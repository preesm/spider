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
#include "VirtualPiSDF.h"

static inline std::int32_t computeTotalExpandedNVertices(PiSDFGraph *graph) {
    std::int32_t totalNVertices = 0;
    for (int i = 0; i < graph->getNBody(); ++i) {
        auto *vertex = graph->getBody(i);
        if (vertex->isHierarchical()) {
            auto subGraphNVertices = computeTotalExpandedNVertices(vertex->getSubGraph());
            subGraphNVertices *= vertex->getBRVValue();
            totalNVertices += subGraphNVertices;
        }
        totalNVertices += vertex->getBRVValue();
    }
    return totalNVertices;
}

static int currentN = 0;

void VirtualPiSDFGraph::initVertexSet(PiSDFGraph *graph, std::int32_t *pi2VirtMap) {
    for (int i = 0; i < graph->getNBody(); ++i) {
        auto *vertex = graph->getBody(i);
        for (int j = 0; j < vertex->getBRVValue(); ++j) {
            auto *virtVertex = &vertexSet_[currentN++];
            virtVertex->vertex_ = vertex;
            virtVertex->instance_ = j;
            // vertexSet_.add(virtVertex);
        }
        /** Mapping index of PiSDFVertex to the one of the virtual vertex **/
        //pi2VirtMap_[vertex->getId() - 1] = vertexSet_[(vertexSet_.getN()) - vertex->getBRVValue()]->getSetIx();
        pi2VirtMap_[vertex->getId() - 1] = currentN - vertex->getBRVValue();
        if (vertex->isHierarchical()) {
            initVertexSet(vertex->getSubGraph(), pi2VirtMap);
        }
    }
}

static std::int32_t computeTotalNBodies(PiSDFGraph *g) {
    std::int32_t nBodies = g->getNBody();
    for (auto i = 0; i < g->getNBody(); ++i) {
        if (g->getBody(i)->isHierarchical()) {
            nBodies += computeTotalNBodies(g->getBody(i)->getSubGraph());
        }
    }
    return nBodies;
}

VirtualPiSDFGraph::VirtualPiSDFGraph(PiSDFGraph *graph) {
    originalGraph_ = graph;
    pi2VirtMap_ = CREATE_MUL_NA(TRANSFO_STACK, computeTotalNBodies(originalGraph_), std::int32_t);
    vertexSet_ = CREATE_MUL_NA(TRANSFO_STACK, computeTotalExpandedNVertices(originalGraph_), VirtualPiSDFVertex);
    /** Initialize vertex Set **/
    currentN = 0;
    initVertexSet(originalGraph_, pi2VirtMap_);
}

VirtualPiSDFGraph::~VirtualPiSDFGraph() {
//    while (vertexSet_.getN() > 0) {
//        delVertex(vertexSet_[0]);
//    }
    StackMonitor::free(TRANSFO_STACK, vertexSet_);
    StackMonitor::free(TRANSFO_STACK, pi2VirtMap_);
}




