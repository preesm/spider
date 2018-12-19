/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
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


static void fillVertexSet(PiSDFVertexSet &vertexSet, long &sizeEdgeSet) {
    int currentSize = 0;
    int n = vertexSet.getN() - 1;
    do {
        currentSize = vertexSet.getN();
        PiSDFVertex *current = vertexSet.getArray()[n];
        // 0. Do the output edges
        for (int i = 0; i < current->getNOutEdge(); ++i) {
            PiSDFEdge *edge = current->getOutEdge(i);
            if (!edge) {
                throwSpiderException("Vertex [%s] has NULL edge", current->getName());
            }
            PiSDFVertex *targetVertex = edge->getSnk();
            if (!vertexSet.contains(targetVertex)) {
                vertexSet.add(targetVertex);
            }
            sizeEdgeSet++;
        }
        // 1. Do the input edges
        for (int i = 0; i < current->getNInEdge(); ++i) {
            PiSDFEdge *edge = current->getInEdge(i);
            if (!edge) {
                throwSpiderException("Vertex [%s] has NULL edge", current->getName());
            }
            PiSDFVertex *sourceVertex = edge->getSrc();
            if (!vertexSet.contains(sourceVertex)) {
                vertexSet.add(sourceVertex);
            }
        }
        n++;
    } while ((vertexSet.getN() != currentSize) || (n != vertexSet.getN()));
}

void computeBRV(transfoJob *job, int *brv) {
    // Retrieve the graph
    PiSDFGraph *const graph = job->graph;
    int nTotalVertices = graph->getNBody() + graph->getNInIf() + graph->getNOutIf();
    PiSDFVertexSet vertexSet(nTotalVertices, TRANSFO_STACK);

    // 0. First we need to get all different connected components
    long nDoneVertices = 0;
    for (int i = 0; i < graph->getNBody(); i++) {
        PiSDFVertex *vertex = graph->getBody(i);
        if (!vertexSet.contains(vertex)) {
            long nEdges = 0;
            vertexSet.add(vertex);
            // 1. Fill up the vertexSet
            fillVertexSet(vertexSet, nEdges);
            // 1.1 Update the offset in the vertexSet
            long nVertices = vertexSet.getN() - nDoneVertices;
            // 2. Compute the BRV of current set
            lcmBasedBRV(job, vertexSet, nDoneVertices, nVertices, nEdges, brv);
            // 3. Update the number of treated vertices
            nDoneVertices = vertexSet.getN();
        }
    }
    while (vertexSet.getN() > 0) {
        vertexSet.del(vertexSet[0]);
    }
}




