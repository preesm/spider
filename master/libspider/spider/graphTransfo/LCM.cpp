/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2015)
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
#include "tools/Rational.h"
#include "graphs/PiSDF/PiSDFEdge.h"
#include "LCM.h"
#include "CommonBRV.h"


/**
 * Fill an array of Rational relations between the different vertices of a given connected components based on their
 * production / consumption data rates.
 *
 * @param job     Pointer to the transfoJob
 * @param edgeSet Edge set
 * @param reps    Array of Rational to be filled
 * @param offset  Offset of the current connected components in the global set of vertices
 */
static void fillReps(transfoJob *job, PiSDFEdgeSet &edgeSet, Rational *reps, long offset) {
    Rational n(1);
    for (int i = 0; i < edgeSet.getN(); ++i) {
        PiSDFEdge *edge = edgeSet.getArray()[i];
        PiSDFVertex *source = edge->getSrc();
        PiSDFVertex *sink = edge->getSnk();
        // Interfaces are taken into account in the repetition vector at first
        if (source->getType() == PISDF_TYPE_IF || sink->getType() == PISDF_TYPE_IF) {
            continue;
        }
        int cons = edge->resolveCons(job);
        int prod = edge->resolveProd(job);
        // Check if the edge is valid
        if ((prod == 0 && cons != 0) || (cons == 0 && prod != 0)) {
            std::string errorMsg =
                    std::string("ERROR: Non valid edge prod / cons. From source [") + std::string(source->getName()) +
                    std::string("] with prod = [" + std::to_string(prod) + std::string("] to sink [") +
                                std::string(sink->getName()) + std::string("] with cons = [") + std::to_string(cons) +
                                std::string("]."));
            throw std::runtime_error(errorMsg);
        }
        long sinkIx = sink->getSetIx() - offset;
        Rational &fa = reps[sinkIx];
        long sourceIx = source->getSetIx() - offset;
        Rational &sa = reps[sourceIx];
        if (fa.getNominator() == 0 && cons != 0) {
            if (sa.getNominator() != 0) {
                n = reps[sourceIx];
            }
            Rational tmp(prod, cons);
            reps[sinkIx] = n * tmp;
        }
        if (sa.getNominator() == 0 && prod != 0) {
            if (fa.getNominator() != 0) {
                n = reps[sinkIx];
            }
            Rational tmp(cons, prod);
            reps[sourceIx] = n * tmp;
        }
    }
}


/**
 * Compute the LCM factor based on all Rational.
 *
 * @param reps      Array of Rational;
 * @param nVertices Number of vertices in the connected components;
 * @return Value of the LCM
 */
static long computeLCMFactor(Rational *reps, long nVertices) {
    long lcm = 1;
    for (long i = 0; i < nVertices; ++i) {
        lcm = Rational::compute_lcm(lcm, reps[i].getDenominator());
    }
    return lcm;
}

/**
 * Compute the repetition vector values of the vertices of a given connected components.
 *
 * @param reps       Array of Rational;
 * @param vertices   Vertices of the connected components;
 * @param lcm        LCM value between the different vertices;
 * @param nVertices  Number of vertices in the connected components;
 * @param brv        Array of BRV values to be filled;
 */
static void computeBRVValues(Rational *reps, PiSDFVertex *const *vertices, long lcm, long nVertices, int *brv) {
    for (long i = 0; i < nVertices; ++i) {
        PiSDFVertex *vertex = vertices[i];
        // Ignore interfaces for now
        if (vertex->getType() == PISDF_TYPE_IF) {
            continue;
        }
        Rational &r = reps[i];
        long nom = r.getNominator();
        long denom = r.getDenominator();
        brv[vertex->getTypeId()] = ((nom * lcm) / denom);
    }
}


/**
 * Check the consistency of the connected components graph.
 *
 * @param job      Pointer to the transfoJob;
 * @param edgeSet  Edge set of the connected components;
 * @param brv      BRV values;
 */
static void checkConsistency(transfoJob *job, PiSDFEdgeSet &edgeSet, int *brv) {
    for (int i = 0; i < edgeSet.getN(); ++i) {
        PiSDFEdge *edge = edgeSet.getArray()[i];
        PiSDFVertex *source = edge->getSrc();
        PiSDFVertex *sink = edge->getSnk();
        if ((source->getType() == PISDF_TYPE_IF) || (sink->getType() == PISDF_TYPE_IF)) {
            continue;
        } else if ((source->getType() == PISDF_TYPE_CONFIG) && brv[source->getTypeId()] > 1) {
            throw std::runtime_error(std::string("ERROR: config actor [") + std::string(source->getName()) +
                                     std::string("] can not have an RV value > 1."));
        }
        int prod = edge->resolveProd(job);
        int cons = edge->resolveCons(job);
        int sourceRV = brv[source->getTypeId()];
        int sinkRV = brv[sink->getTypeId()];
        if ((prod * sourceRV) != (cons * sinkRV)) {
            throw std::runtime_error("Graph is not consistent: edge from [" + std::string(source->getName()) + "] "
                                                                                                               "with production [" +
                                     std::to_string(prod * sourceRV) + "] != [" + std::to_string(cons * sinkRV) +
                                     "].");
        }
    }
}


void
lcmBasedBRV(transfoJob *job, PiSDFVertexSet &vertexSet, long nDoneVertices, long nVertices, long nEdges, int *brv) {
    // 0. Get vertices and edges set
    PiSDFVertex *const *vertices = vertexSet.getArray() + nDoneVertices;
    PiSDFEdgeSet edgeSet(nEdges, TRANSFO_STACK);
    fillEdgeSet(edgeSet, vertices, nVertices);
    // 1. Initialize the reps map
    Rational *reps = CREATE_MUL(TRANSFO_STACK, nVertices, Rational);
    for (long i = 0; i < nVertices; ++i) {
        // Init a rational with num: 0, den: 1
        reps[i] = Rational();
    }

    fillReps(job, edgeSet, reps, nDoneVertices);

    // 2. Compute lcm
    long lcm = computeLCMFactor(reps, nVertices);

    // 3. Compute and set BRV values
    computeBRVValues(reps, vertices, lcm, nVertices, brv);

    // 4. Check consistency
    checkConsistency(job, edgeSet, brv);

    // 5. Update RV values with interfaces
    updateBRV(job, nVertices, brv, vertices);

    // 6. Free the memory
    StackMonitor::free(TRANSFO_STACK, reps);
    while (edgeSet.getN() > 0) {
        edgeSet.del(edgeSet[0]);
    }
}