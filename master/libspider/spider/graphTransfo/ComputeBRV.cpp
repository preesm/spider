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
#include "ComputeBRV.h"
#include "topologyMatrix.h"

#include <graphs/PiSDF/PiSDFEdge.h>

#include <cmath>
#include <algorithm>
#include <tools/Rational.h>

static bool isBodyExecutable(PiSDFVertex *body, transfoJob *job) {
    bool notExec = true;

    /* Test if all In/Out is equal to 0 */
    for (int j = 0; notExec && j < body->getNInEdge(); j++) {
        PiSDFEdge *inEdge = body->getInEdge(j);
        int cons = inEdge->resolveCons(job);
        notExec = notExec && (cons == 0);
    }
    for (int j = 0; notExec && j < body->getNOutEdge(); j++) {
        PiSDFEdge *outEdge = body->getOutEdge(j);
        int prod = outEdge->resolveProd(job);
        notExec = notExec && (prod == 0);
    }

    return !notExec;
}

static bool isEdgeValid(PiSDFEdge *edge, transfoJob *job) {
    int prod = edge->resolveProd(job);
    int cons = edge->resolveCons(job);

    if ((prod == 0 && cons != 0) || (cons == 0 && prod != 0))
        throw std::runtime_error("Bad Edge Prod/Cons, One is =0 et other is !=0\n");

    if (edge->getSrc() != edge->getSnk()
        && edge->getSrc()->getType() == PISDF_TYPE_BODY && isBodyExecutable(edge->getSrc(), job)
        && edge->getSnk()->getType() == PISDF_TYPE_BODY && isBodyExecutable(edge->getSnk(), job)
            )
        return true;

    return false;
}

void computeBRV(SRDAGGraph */*topSrdag*/, transfoJob *job, int *brv) {
    int *vertexIxs = CREATE_MUL(TRANSFO_STACK, job->graph->getNBody(), int);

    /* Compute nbVertices */
    int nbVertices = 0;
    for (int i = 0; i < job->graph->getNBody(); i++) {
        PiSDFVertex *body = job->graph->getBody(i);
        if (isBodyExecutable(body, job)) {
            vertexIxs[body->getTypeId()] = nbVertices++;
        } else
            vertexIxs[body->getTypeId()] = -1;
    }

    /* Compute nbEdges */
    int nbEdges = 0;
    for (int i = 0; i < job->graph->getNEdge(); i++) {
        PiSDFEdge *edge = job->graph->getEdge(i);
        if (isEdgeValid(edge, job)) {
            nbEdges++;
        }
    }

    int *topo_matrix = CREATE_MUL(TRANSFO_STACK, nbEdges * nbVertices, int);
    memset(topo_matrix, 0, nbEdges * nbVertices * sizeof(int));

    /* Fill the topology matrix(nbEdges x nbVertices) */
    nbEdges = 0; // todo do better with the nbEdges var
    for (int i = 0; i < job->graph->getNEdge(); i++) {
        PiSDFEdge *edge = job->graph->getEdge(i);
        if (isEdgeValid(edge, job)) {
            int prod = edge->resolveProd(job);
            int cons = edge->resolveCons(job);

            if (prod < 0 || cons < 0) {
                char name[100];
                edge->getProdExpr(name, 100);
                printf("Prod : %s = %d\n", name, prod);
                edge->getConsExpr(name, 100);
                printf("Cons : %s = %d\n", name, cons);
                throw std::runtime_error("Error Bad prod/cons resolved\n");
            }

            topo_matrix[nbEdges * nbVertices + vertexIxs[edge->getSrc()->getTypeId()]] = prod;
            topo_matrix[nbEdges * nbVertices + vertexIxs[edge->getSnk()->getTypeId()]] = -cons;
            nbEdges++;
        }
    }

    printf("topoMatrix:\n");
    for (int i = 0; i < nbEdges; i++) {
        for (int j = 0; j < nbVertices; j++) {
            printf("%4d ", topo_matrix[i * nbVertices + j]);
        }
        printf("\n");
    }

    int *smallBrv = CREATE_MUL(TRANSFO_STACK, nbVertices, int);

    /* Compute nullSpace */
    nullSpace(topo_matrix, smallBrv, nbEdges, nbVertices);

    /* Convert Small Brv to Complete One */
    for (int i = 0; i < job->graph->getNBody(); i++) {
        if (vertexIxs[i] == -1)
            brv[i] = 0;
        else
            brv[i] = smallBrv[vertexIxs[i]];
    }

    /* Updating the productions of the round buffer vertices. */
    int coef = 1;

    /* Looking on interfaces */
    for (int i = 0; i < job->graph->getNInIf(); i++) {
        PiSDFVertex *inIf = job->graph->getInputIf(i);
        PiSDFEdge *edge = inIf->getOutEdge(0);
        /* Only if IF<->Body edge */
        if (edge->getSnk()->getType() == PISDF_TYPE_BODY) {
            float prod = edge->resolveProd(job);
            float cons = edge->resolveCons(job);
            float nbRepet = brv[edge->getSnk()->getTypeId()];
            if (nbRepet != 0)
                coef = std::max(coef, (int) std::ceil(prod / (cons * nbRepet)));
        }
    }
    for (int i = 0; i < job->graph->getNOutIf(); i++) {
        PiSDFVertex *outIf = job->graph->getOutputIf(i);
        PiSDFEdge *edge = outIf->getInEdge(0);
        /* Only if Body<->IF edge */
        if (edge->getSrc()->getType() == PISDF_TYPE_BODY) {
            float prod = edge->resolveProd(job);
            float cons = edge->resolveCons(job);
            float nbRepet = brv[edge->getSrc()->getTypeId()];
            if (nbRepet != 0)
                coef = std::max(coef, (int) std::ceil(cons / (prod * nbRepet)));
        }
    }
    /* Looking on implicit RB between Config and Body */
    for (int i = 0; i < job->graph->getNConfig(); i++) {
        PiSDFVertex *config = job->graph->getConfig(i);
        for (int i = 0; i < config->getNOutEdge(); i++) {
            PiSDFEdge *edge = config->getOutEdge(i);
            /* Only if Config<->Body edge */
            if (edge->getSnk()->getType() == PISDF_TYPE_BODY) {
                float prod = edge->resolveProd(job);
                float cons = edge->resolveCons(job);
                float nbRepet = brv[edge->getSnk()->getTypeId()];
                if (nbRepet != 0)
                    coef = std::max(coef, (int) std::ceil(prod / (cons * nbRepet)));
            }
        }
    }

    for (int i = 0; i < nbVertices; i++) {
        brv[i] *= coef;
    }

    StackMonitor::free(TRANSFO_STACK, topo_matrix);
    StackMonitor::free(TRANSFO_STACK, vertexIxs);
    StackMonitor::free(TRANSFO_STACK, smallBrv);

//	printf("brv:\n");
//	for(int i=0; i<nbVertices; i++){
//		printf("%4d ", brv[i]);
//	}
//	printf("\n");
}

static void fillVertexSet(PiSDFVertexSet &vertexSet, PiSDFVertex *vertex) {
    // 0. Do the output edges
    for (int i = 0; i < vertex->getNOutEdge(); ++i) {
        PiSDFEdge *edge = vertex->getOutEdge(i);
        if (!edge) {
            throw new std::runtime_error("Null edge detected on vertex [" + std::string(vertex->getName()) + "].");
        }
        PiSDFVertex *targetVertex = edge->getSnk();
        if (!vertexSet.contains(targetVertex)) {
            vertexSet.add(targetVertex);
            fillVertexSet(vertexSet, targetVertex);
        }
    }
    // 1. Do the input edges
    for (int i = 0; i < vertex->getNInEdge(); ++i) {
        PiSDFEdge *edge = vertex->getInEdge(i);
        if (!edge) {
            throw new std::runtime_error("Null edge detected on vertex [" + std::string(vertex->getName()) + "].");
        }
        PiSDFVertex *sourceVertex = edge->getSrc();
        if (!vertexSet.contains(sourceVertex)) {
            vertexSet.add(sourceVertex);
            fillVertexSet(vertexSet, sourceVertex);
        }
    }
}

static void fillReps(transfoJob *job, PiSDFVertex &vertex, Rational &n, std::map<std::string, Rational > &reps) {
    // Update value in the HashMap
    reps[std::string(vertex.getName())] = n;

    // 0. Do the output edges
    for (int i = 0; i < vertex.getNOutEdge(); ++i) {
        PiSDFEdge *edge = vertex.getOutEdge(i);
        if (!edge) {
            throw new std::runtime_error("Null edge detected on vertex [" + std::string(vertex.getName()) + "].");
        }
        PiSDFVertex *targetVertex = edge->getSnk();
        if (targetVertex->getType() == PISDF_TYPE_IF) {
            continue;
        }
        Rational &fa = reps[std::string(targetVertex->getName())];
        if (fa.getNominator() == 0) {
            int prod = edge->resolveProd(job);
            int cons = edge->resolveCons(job);
            Rational tmp(prod, cons);
            Rational r = n * tmp;
            fillReps(job, *targetVertex, r, reps);
        }
    }
    // 1. Do the input edges
    for (int i = 0; i < vertex.getNInEdge(); ++i) {
        PiSDFEdge *edge = vertex.getInEdge(i);
        if (!edge) {
            throw new std::runtime_error("Null edge detected on vertex [" + std::string(vertex.getName()) + "].");
        }
        PiSDFVertex *sourceVertex = edge->getSrc();
        if (sourceVertex->getType() == PISDF_TYPE_IF) {
            continue;
        }
        Rational &fa = reps[std::string(sourceVertex->getName())];
        if (fa.getNominator() == 0) {
            int prod = edge->resolveProd(job);
            int cons = edge->resolveCons(job);
            Rational tmp(cons, prod);
            Rational r = n * tmp;
            fillReps(job, *sourceVertex, r, reps);
        }
    }
}

static inline long long compute_gcd(long long a, long long b) {
    long long t;
    while (b != 0) {
        t = b;
        b = a % b;
        a = t;
    }
    return a;
}

static inline long long compute_lcm(long long a, long long b) {
    if (a * b == 0) {
        return 1;
    }
    long long tmp = a * b;
    tmp = tmp < 0 ? -tmp : tmp;
    return tmp / compute_gcd(a, b);
}


static void fillEdgeSet(PiSDFEdgeSet &edgeSet, PiSDFVertex *const *vertices, int size) {
    for (int i = 0; i < size; ++i) {
        // 0. Do input edges
        for (int j = 0; j < vertices[i]->getNInEdge(); ++j) {
            PiSDFEdge *edge = vertices[i]->getInEdge(j);
            if (!edgeSet.contains(edge)) {
                edgeSet.add(edge);
            }
        }
        // 1. Do output edges
        for (int j = 0; j < vertices[i]->getNOutEdge(); ++j) {
            PiSDFEdge *edge = vertices[i]->getOutEdge(j);
            if (!edgeSet.contains(edge)) {
                edgeSet.add(edge);
            }
        }
    }
}


static void updateFromInputIF(transfoJob *job, PiSDFVertex *vertex, int *brv, long &scaleFactor) {
    PiSDFEdge *edge = vertex->getOutEdge(0);
    PiSDFVertex *sink = edge->getSnk();
    if (sink->getType() == PISDF_TYPE_BODY) {
        int sinkRV = brv[sink->getTypeId()];
        int prod = edge->resolveProd(job);
        int cons = edge->resolveCons(job);
        long tmp = cons * sinkRV * scaleFactor;
        if (tmp < prod) {
            long scaleScaleFactor = prod / tmp;
            if ((scaleScaleFactor * tmp) < prod) {
                scaleScaleFactor++;
            }
            scaleFactor *= scaleScaleFactor;
        }
    }
}

static void updateFromOutputIF(transfoJob *job, PiSDFVertex *vertex, int *brv, long &scaleFactor) {
    PiSDFEdge *edge = vertex->getInEdge(0);
    PiSDFVertex *source = edge->getSrc();
    if (source->getType() == PISDF_TYPE_BODY) {
        int sourceRV = brv[source->getTypeId()];
        int prod = edge->resolveProd(job);
        int cons = edge->resolveCons(job);
        long tmp = prod * sourceRV * scaleFactor;
        if (tmp < cons) {
            long scaleScaleFactor = cons / tmp;
            if ((scaleScaleFactor * tmp) < cons) {
                scaleScaleFactor++;
            }
            scaleFactor *= scaleScaleFactor;
        }
    }
}


static int getEdgeSetSize(PiSDFVertex *const *vertices, int size) {
    int maxSize = 0;
    for (int i = 0; i < size; ++i) {
        printf("%s -- %d \n", vertices[i]->getName(), maxSize);
        maxSize += vertices[i]->getNInEdge();
        maxSize += vertices[i]->getNOutEdge();
    }
    return maxSize;
}

static void lcmBRV(transfoJob *job, PiSDFVertexSet &vertexSet, long offset, long size, int *brv) {
    // 0. Get vertices set
    PiSDFVertex *const *vertices = vertexSet.getArray() + offset;
    // 1. Initialize the reps map
    std::map<std::string, Rational > reps;
    for (long i = 0; i < size; ++i) {
        reps.insert(std::make_pair(std::string(vertices[i]->getName()), Rational(0, 1)));
    }

    Rational initRational(1, 1);
    fillReps(job, *vertices[0], initRational, reps);
    // 2. Compute lcm
    long long lcm = 1;
    for (auto const &r : reps) {
        lcm = compute_lcm(lcm, r.second.getDenominator());
    }

    // 3. Compute and set BRV values
    for (long i = 0; i < size; ++i) {
        PiSDFVertex *vertex = vertices[i];
        // Ignore interfaces for now
        if (vertex->getType() == PISDF_TYPE_IF) {
            continue;
        }
        Rational &r = reps.at(vertex->getName());
        long nom = r.getNominator();
        long denom = r.getDenominator();
        brv[vertex->getTypeId()] = ((nom * lcm) / denom);
    }

    // 4. Check consistency
    int maxSize = getEdgeSetSize(vertices, size);
    PiSDFEdgeSet edgeSet(maxSize, TRANSFO_STACK);
    fillEdgeSet(edgeSet, vertices, size);
    for (int i = 0; i < edgeSet.getN(); ++i) {
        PiSDFEdge *edge = edgeSet.getArray()[i];
        PiSDFVertex *source = edge->getSrc();
        PiSDFVertex *sink = edge->getSnk();
        if ((source->getType() == PISDF_TYPE_IF) || (sink->getType() == PISDF_TYPE_IF)) {
            continue;
        }
        int prod = edge->resolveProd(job);
        int cons = edge->resolveCons(job);
        int sourceRV = brv[source->getTypeId()];
        int sinkRV = brv[sink->getTypeId()];
        if ((prod * sourceRV) != (cons * sinkRV)) {
            throw new std::runtime_error("Graph is not consistent: edge from [" + std::string(source->getName()) + "] "
                                                                                                                   "with production [" +
                                         std::to_string(prod * sourceRV) + "] != [" + std::to_string(cons * sinkRV) +
                                         "].");
        }
    }

    // 5. Update rv values with interfaces
    long scaleFactor = 1;
    // 5.1 Get scale factor
    for (long i = 0; i < size; ++i) {
        PiSDFVertex *vertex = vertices[i];
        if (vertex->getType() == PISDF_TYPE_IF && vertex->getSubType() == PISDF_SUBTYPE_INPUT_IF) {
            updateFromInputIF(job, vertex, brv, scaleFactor);
        } else if (vertex->getType() == PISDF_TYPE_IF && vertex->getSubType() == PISDF_SUBTYPE_OUTPUT_IF) {
            updateFromOutputIF(job, vertex, brv, scaleFactor);
        }
    }
    // 5.2 Apply scale factor
    if (scaleFactor != 1) {
        for (long i = 0; i < size; ++i) {
            PiSDFVertex *vertex = vertices[i];
            brv[vertex->getTypeId()] *= scaleFactor;
        }
    }
}

void computeBRV(transfoJob *job, int *brv) {
    // Retrieve the graph
    PiSDFGraph *graph = job->graph;
    PiSDFVertexSet vertexSet(graph->getNBody() + graph->getNInIf() + graph->getNOutIf(), TRANSFO_STACK);

    // 0. First we need to get all different connected components
    long verticesOffset = 0;
    for (int i = 0; i < graph->getNBody(); i++) {
        PiSDFVertex *vertex = graph->getBody(i);
        if (!vertexSet.contains(vertex)) {
            vertexSet.add(vertex);
            // 1. Fill up the vertexSet
            fillVertexSet(vertexSet, vertex);
            // 1.1 Update the offset in the vertexSet
            long size = vertexSet.getN() - verticesOffset;
            // 2. Compute the BRV of current set
            lcmBRV(job, vertexSet, verticesOffset, size, brv);
            verticesOffset = vertexSet.getN();
        }
    }
}




