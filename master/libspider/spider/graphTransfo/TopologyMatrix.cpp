/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2018 - 2019) :
 *
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
#include <algorithm>
#include <cmath>
#include <graphTransfo/TopologyMatrix.h>
#include <graphs/PiSDF/PiSDFEdge.h>
#include <graphTransfo/CommonBRV.h>
#include <tools/Rational.h>


static int nullSpace(Param *topo_matrix, int *brv, int nbEdges, int nbVertices) {
    auto *ratioMatrix = CREATE_MUL(TRANSFO_STACK, nbVertices * nbEdges, Rational);
    auto *ratioResult = CREATE_MUL(TRANSFO_STACK, nbVertices, Rational);
    /* Copy matrix into ratioMatrix */
    for (int i = 0; i < nbEdges * nbVertices; i++) {
        ratioMatrix[i] = Rational(topo_matrix[i]);
    }

    for (int i = 0; i < nbEdges; i++) {
        Rational pivotMax(ratioMatrix[i * nbVertices + i].abs());
        int maxIndex = i;

        for (int t = i + 1; t < nbEdges; t++) {
            Rational newPivot(ratioMatrix[t * nbVertices + i].abs());
            if (newPivot > pivotMax) {
                maxIndex = t;
                pivotMax = newPivot;
            }
        }

        if (pivotMax != 0 && maxIndex != i) {
            /* Switch Rows */
            for (int t = 0; t < nbVertices; t++) {
                Rational tmp(ratioMatrix[maxIndex * nbVertices + t]);
                ratioMatrix[maxIndex * nbVertices + t] = Rational(ratioMatrix[i * nbVertices + t]);
                ratioMatrix[i * nbVertices + t] = Rational(tmp);
            }
        } else if (maxIndex == i && (pivotMax != 0)) {
            /* Do nothing */
        } else {
            break;
        }

        Rational odlPivot = ratioMatrix[i * nbVertices + i];
        for (int t = i; t < nbVertices; t++) {
            ratioMatrix[i * nbVertices + t] /= odlPivot;
        }

        for (int j = i + 1; j < nbEdges; j++) {
            if (ratioMatrix[j * nbVertices + i] != 0) {
                Rational oldji = ratioMatrix[j * nbVertices + i];

                for (int k = 0; k < nbVertices; k++) {
                    ratioMatrix[j * nbVertices + k] =
                            ratioMatrix[j * nbVertices + k] - (oldji * ratioMatrix[i * nbVertices + k]);
                }
            }
        }
    }

    for (int i = 0; i < nbVertices; i++) {
        ratioResult[i] = Rational(1);
    }

    for (int i = nbEdges - 1; i >= 0; i--) {
        Rational val = Rational();

        for (int k = i + 1; k < nbVertices; k++) {
            val += (ratioMatrix[i * nbVertices + k] * ratioResult[k]);
        }
        if (val != 0) {
            if (ratioMatrix[i * nbVertices + i] == 0) {
                throwSpiderException("Diagonal element of topology matrix [%d][%d] is null.", i, i);
            }
            ratioResult[i] = val.abs() / ratioMatrix[i * nbVertices + i];
        }
    }

    std::int64_t lcm = 1;
    for (int i = 0; i < nbVertices; i++) {
        lcm = Rational::compute_lcm(lcm, ratioResult[i].getDenominator());
    }
    for (int i = 0; i < nbVertices; i++) {
        brv[i] = Rational(ratioResult[i] * lcm).abs().toInt32();
    }

    StackMonitor::free(TRANSFO_STACK, ratioMatrix);
    StackMonitor::free(TRANSFO_STACK, ratioResult);

    return 0;
}

static bool isBodyExecutable(PiSDFVertex *body) {
    bool notExec = true;

    /* Test if all In/Out is equal to 0 */
    for (int j = 0; notExec && j < body->getNInEdge(); j++) {
        PiSDFEdge *inEdge = body->getInEdge(j);
        Param cons = inEdge->resolveCons();
        notExec = notExec && (cons == 0);
    }
    for (int j = 0; notExec && j < body->getNOutEdge(); j++) {
        PiSDFEdge *outEdge = body->getOutEdge(j);
        Param prod = outEdge->resolveProd();
        notExec = notExec && (prod == 0);
    }

    return !notExec;
}

static bool isEdgeValid(PiSDFEdge *edge) {
    Param prod = edge->resolveProd();
    Param cons = edge->resolveCons();

    if ((prod == 0 && cons != 0) || (cons == 0 && prod != 0)) {
        throwSpiderException("Bad Edge Prod/Cons, Prod: %d et Cons: %d.", prod, cons);
    }


    return edge->getSrc() != edge->getSnk()
           && edge->getSrc()->getType() == PISDF_TYPE_BODY && isBodyExecutable(edge->getSrc())
           && edge->getSnk()->getType() == PISDF_TYPE_BODY && isBodyExecutable(edge->getSnk());
}


void topologyBasedBRV(PiSDFVertexSet &vertexSet, long nDoneVertices, long nVertices, long nEdges,
                      int *brv) {
    PiSDFVertex *const *vertices = vertexSet.getArray() + nDoneVertices;
    auto *vertexIxs = CREATE_MUL(TRANSFO_STACK, nVertices, int);
    PiSDFEdgeSet edgeSet(static_cast<int>(nEdges), TRANSFO_STACK);
    fillEdgeSet(edgeSet, vertices, nVertices);

    /* Compute nbVertices */
    int nbVertices = 0;
    for (int i = 0; i < nVertices; i++) {
        PiSDFVertex *body = vertices[i];
        if (body->getType() == PISDF_TYPE_BODY) {
            long ix = body->getSetIx() - nDoneVertices;
            if (isBodyExecutable(body)) {
                vertexIxs[ix] = nbVertices++;
            } else {
                vertexIxs[ix] = -1;
            }
        }
    }

    /* Compute nbEdges */
    int nbEdges = 0;
    for (int i = 0; i < nEdges; i++) {
        PiSDFEdge *edge = edgeSet.getArray()[i];
        if (isEdgeValid(edge)) {
            nbEdges++;
        }
    }

    auto *topo_matrix = CREATE_MUL(TRANSFO_STACK, nbEdges * nbVertices, Param);
    memset(topo_matrix, 0, nbEdges * nbVertices * sizeof(int));

    /* Fill the topology matrix(nbEdges x nbVertices) */
    nbEdges = 0; // todo do better with the nbEdges var
    for (int i = 0; i < nEdges; i++) {
        PiSDFEdge *edge = edgeSet.getArray()[i];
        if (isEdgeValid(edge)) {
            Param prod = edge->resolveProd();
            Param cons = edge->resolveCons();

            if (prod < 0 || cons < 0) {
                char name[100];
                edge->getProdExpr(name, 100);
                fprintf(stderr, "Prod: %s = %li\n", name, prod);
                edge->getConsExpr(name, 100);
                fprintf(stderr, "Cons: %s = %li\n", name, cons);
                throwSpiderException("Error Bad prod/cons resolved.");
            }
            long sourceIx = edge->getSrc()->getSetIx() - nDoneVertices;
            long sinkIx = edge->getSnk()->getSetIx() - nDoneVertices;
            topo_matrix[nbEdges * nbVertices + vertexIxs[sourceIx]] = prod;
            topo_matrix[nbEdges * nbVertices + vertexIxs[sinkIx]] = -cons;
            nbEdges++;
        }
    }

    auto *smallBrv = CREATE_MUL(TRANSFO_STACK, nbVertices, int);

    /* Compute nullSpace */
    nullSpace(topo_matrix, smallBrv, nbEdges, nbVertices);

    /* Convert Small Brv to Complete One */
    for (int i = 0; i < nVertices; i++) {
        PiSDFVertex *vertex = vertices[i];
        if (vertex->getType() == PISDF_TYPE_BODY) {
            if (vertexIxs[i] == -1) {
                brv[vertex->getTypeId()] = 0;
            } else {
                brv[vertex->getTypeId()] = smallBrv[vertexIxs[i]];
            }
        }
    }

//    /* Updating the productions of the round buffer vertices. */
//    int coef = 1;
//
//    /* Looking on interfaces */
//    for (int i = 0; i < job->graph->getNInIf(); i++) {
//        PiSDFVertex *inIf = job->graph->getInputIf(i);
//        PiSDFEdge *edge = inIf->getOutEdge(0);
//        /* Only if IF<->Body edge */
//        if (edge->getSnk()->getType() == PISDF_TYPE_BODY) {
//            float prod = edge->resolveProd(job);
//            float cons = edge->resolveCons(job);
//            float nbRepet = brv[edge->getSnk()->getTypeId()];
//            if (nbRepet != 0)
//                coef = std::max(coef, (int) std::ceil(prod / (cons * nbRepet)));
//        }
//    }
//    for (int i = 0; i < job->graph->getNOutIf(); i++) {
//        PiSDFVertex *outIf = job->graph->getOutputIf(i);
//        PiSDFEdge *edge = outIf->getInEdge(0);
//        /* Only if Body<->IF edge */
//        if (edge->getSrc()->getType() == PISDF_TYPE_BODY) {
//            float prod = edge->resolveProd(job);
//            float cons = edge->resolveCons(job);
//            float nbRepet = brv[edge->getSrc()->getTypeId()];
//            if (nbRepet != 0)
//                coef = std::max(coef, (int) std::ceil(cons / (prod * nbRepet)));
//        }
//    }
//    /* Looking on implicit RB between Config and Body */
//    for (int i = 0; i < job->graph->getNConfig(); i++) {
//        PiSDFVertex *config = job->graph->getConfig(i);
//        for (int i = 0; i < config->getNOutEdge(); i++) {
//            PiSDFEdge *edge = config->getOutEdge(i);
//            /* Only if Config<->Body edge */
//            if (edge->getSnk()->getType() == PISDF_TYPE_BODY) {
//                float prod = edge->resolveProd(job);
//                float cons = edge->resolveCons(job);
//                float nbRepeat = brv[edge->getSnk()->getTypeId()];
//                if (nbRepeat != 0)
//                    coef = std::max(coef, (int) std::ceil(prod / (cons * nbRepeat)));
//            }
//        }
//    }
//
//    for (int i = 0; i < nbVertices; i++) {
//        brv[i] *= coef;
//    }

    // Update RV based on interfaces and config actors
    updateBRV(nVertices, brv, vertices);

    StackMonitor::free(TRANSFO_STACK, topo_matrix);
    StackMonitor::free(TRANSFO_STACK, vertexIxs);
    StackMonitor::free(TRANSFO_STACK, smallBrv);
    while (edgeSet.size() > 0) {
        edgeSet.del(edgeSet[0]);
    }
}
