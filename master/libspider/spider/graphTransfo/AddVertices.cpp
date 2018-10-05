/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2017)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013)
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
#include "AddVertices.h"

#include <graphs/PiSDF/PiSDFEdge.h>

void addSRVertices(SRDAGGraph *topSrdag, transfoJob *job, int *brv) {
    job->bodies = CREATE_MUL(TRANSFO_STACK, job->graph->getNBody(), SRDAGVertex**);

    for (int bodyIx = 0; bodyIx < job->graph->getNBody(); bodyIx++) {
        PiSDFVertex *pi_vertex = job->graph->getBody(bodyIx);
        job->bodies[bodyIx] = CREATE_MUL(TRANSFO_STACK, brv[bodyIx], SRDAGVertex*);

        switch (pi_vertex->getSubType()) {
            case PISDF_SUBTYPE_NORMAL:
                for (int j = 0; j < brv[bodyIx]; j++) {
                    job->bodies[bodyIx][j] = topSrdag->addVertex(pi_vertex, j, job->graphIter);
                    for (int i = 0; i < pi_vertex->getNInParam(); i++) {
                        job->bodies[bodyIx][j]->addInParam(i, job->paramValues[pi_vertex->getInParam(i)->getTypeIx()]);
                    }
                }
                break;
            case PISDF_SUBTYPE_BROADCAST:
                for (int j = 0; j < brv[bodyIx]; j++) {
                    /* Check Broadcast use */
                    int cons = pi_vertex->getInEdge(0)->resolveCons(job);
                    for (int tmp = 0; tmp < pi_vertex->getNOutEdge(); tmp++) {
                        int prod = pi_vertex->getOutEdge(tmp)->resolveProd(job);
                        if (prod != cons) {
                            char prodExpr[100], consExpr[100];
                            pi_vertex->getInEdge(0)->getConsExpr(consExpr, 100);
                            pi_vertex->getOutEdge(tmp)->getProdExpr(prodExpr, 100);
                            printf("Warning: Broadcast have different production/consumption: prod: %d != cons (%d) \n",
                                   prod, cons);
                        }
                    }
                    job->bodies[bodyIx][j] = topSrdag->addBroadcast(MAX_IO_EDGES);
                }
                break;
            case PISDF_SUBTYPE_ROUNDBUFFER:
                for (int j = 0; j < brv[bodyIx]; j++) {
                    job->bodies[bodyIx][j] = topSrdag->addRoundBuffer();
                }
                break;
            case PISDF_SUBTYPE_JOIN:
                for (int j = 0; j < brv[bodyIx]; j++) {
                    job->bodies[bodyIx][j] = topSrdag->addJoin(MAX_IO_EDGES);
                }
                break;
            case PISDF_SUBTYPE_FORK:
                for (int j = 0; j < brv[bodyIx]; j++) {
                    job->bodies[bodyIx][j] = topSrdag->addFork(MAX_IO_EDGES);
                }
                break;
            case PISDF_SUBTYPE_END:
                for (int j = 0; j < brv[bodyIx]; j++) {
                    job->bodies[bodyIx][j] = topSrdag->addEnd();
                }
                break;
            default:
                throw std::runtime_error("Unexpected Interface vertex in AddVertices\n");
        }
    }
}

void addCAVertices(SRDAGGraph *topSrdag, transfoJob *job) {
    job->configs = CREATE_MUL(TRANSFO_STACK, job->graph->getNConfig(), SRDAGVertex*);
    for (int configIx = 0; configIx < job->graph->getNConfig(); configIx++) {
        PiSDFVertex *config = job->graph->getConfig(configIx);

        if (config->getSubType() == PISDF_SUBTYPE_BROADCAST)
            job->configs[configIx] = topSrdag->addBroadcast(MAX_IO_EDGES, config);
        else {
            job->configs[configIx] = topSrdag->addVertex(config, 0, job->graphIter);

            for (int i = 0; i < config->getNInParam(); i++) {
                job->configs[configIx]->addInParam(i, job->paramValues[config->getInParam(i)->getTypeIx()]);
            }
            for (int i = 0; i < config->getNOutParam(); i++) {
                job->configs[configIx]->addOutParam(i, &(job->paramValues[config->getOutParam(i)->getTypeIx()]));
            }
        }
    }
}
