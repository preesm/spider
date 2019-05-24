/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2018 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2019)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018 - 2019)
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
#include "CommonBRV.h"
#include "graphs/PiSDF/PiSDFEdge.h"


static void updateFromIF(PiSDFVertex *vertex, const int *brv, long &scaleFactor) {
    PiSDFEdge *edge = vertex->getAllEdges()[0];
    PiSDFVertex *source = edge->getSrc();
    PiSDFVertex *sink = edge->getSnk();
    auto prod = edge->resolveProd();
    auto cons = edge->resolveCons();
    long tmp = 0;
    long cmp = 0;
    if (vertex->getSubType() == PISDF_SUBTYPE_INPUT_IF && sink->getType() == PISDF_TYPE_BODY) {
        int sinkRV = brv[sink->getTypeId()];
        tmp = cons * sinkRV * scaleFactor;
        cmp = prod;
    } else if (vertex->getSubType() == PISDF_SUBTYPE_OUTPUT_IF && source->getType() == PISDF_TYPE_BODY) {
        int sourceRV = brv[source->getTypeId()];
        tmp = prod * sourceRV * scaleFactor;
        cmp = cons;
    } else {
        return;
    }
    if (tmp != 0 && tmp < cmp) {
        long scaleScaleFactor = cmp / tmp + (cmp % tmp != 0); // ceil(cmp / tmp)
//        if ((scaleScaleFactor * tmp) < cmp) {
//            scaleScaleFactor++;
//        }
        scaleFactor *= scaleScaleFactor;
    }
}

static void updateFromCFG(PiSDFVertex *vertex, const int *brv, long &scaleFactor) {
    for (int i = 0; i < vertex->getNOutEdge(); ++i) {
        PiSDFEdge *edge = vertex->getOutEdge(i);
        PiSDFVertex *sink = edge->getSnk();
        auto prod = edge->resolveProd();
        auto cons = edge->resolveCons();
        if (sink->getType() == PISDF_TYPE_BODY) {
            int sinkRV = brv[sink->getTypeId()];
            long tmp = cons * sinkRV * scaleFactor;
            if (tmp != 0 && tmp < prod) {
                long scaleScaleFactor = prod / tmp;
                if ((scaleScaleFactor * tmp) < prod) {
                    scaleScaleFactor++;
                }
                scaleFactor *= scaleScaleFactor;
            }
        }
    }
}

void updateBRV(long nVertices, int *brv, PiSDFVertex *const *vertices) {
    long scaleFactor = 1;
    // 5.1 Get scale factor
    for (long i = 0; i < nVertices; ++i) {
        PiSDFVertex *vertex = vertices[i];
        if (vertex->getType() == PISDF_TYPE_IF) {
            updateFromIF(vertex, brv, scaleFactor);
        } else if (vertex->getType() == PISDF_TYPE_CONFIG) {
            updateFromCFG(vertex, brv, scaleFactor);
        }
    }
    // 5.2 Apply scale factor
    if (scaleFactor != 1) {
        auto *graph = vertices[0]->getGraph();
        nVertices = nVertices - graph->getNInIf() - graph->getNOutIf();
        for (long i = 0; i < nVertices; ++i) {
            PiSDFVertex *vertex = vertices[i];
            brv[vertex->getTypeId()] *= scaleFactor;
        }
    }
}

void fillEdgeSet(PiSDFEdgeSet &edgeSet, PiSDFVertex *const *vertices, long nVertices) {
    for (long v = 0; v < nVertices; ++v) {
        // We only need to go though the output edges of every vertices
        for (int e = 0; e < vertices[v]->getNOutEdge(); ++e) {
            auto *edge = vertices[v]->getOutEdge(e);
            edgeSet.add(edge);
        }
    }
}
