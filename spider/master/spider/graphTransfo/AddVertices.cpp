 /****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

#include "AddVertices.h"
#include "GraphTransfo.h"

#include <graphs/PiSDF/PiSDFGraph.h>
#include <graphs/PiSDF/PiSDFVertex.h>
#include <graphs/PiSDF/PiSDFEdge.h>

#include <graphs/SRDAG/SRDAGGraph.h>
#include <graphs/SRDAG/SRDAGVertex.h>
#include <graphs/SRDAG/SRDAGEdge.h>

void addSRVertices(SRDAGGraph *topSrdag, transfoJob *job, int *brv, Stack* stack){
    job->bodies = sAlloc(stack, job->graph->getNBody(), SRDAGVertex**);
	PiSDFVertexIterator bodyIt = job->graph->getBodyIterator();
	FOR_IT(bodyIt){
        job->bodies[bodyIt.currentIx()] = sAlloc(stack, brv[bodyIt.currentIx()], SRDAGVertex*);;
		PiSDFVertex* pi_vertex = job->graph->getBody(bodyIt.currentIx());
		switch(pi_vertex->getSubType()){
		case PISDF_SUBTYPE_NORMAL:
			for(int j=0; j<brv[bodyIt.currentIx()]; j++){
				job->bodies[bodyIt.currentIx()][j] = topSrdag->addVertex(pi_vertex);

				for(int i=0; i<bodyIt.current()->getNInParam(); i++){
					job->bodies[bodyIt.currentIx()][j]->addInParam(i, job->paramValues[pi_vertex->getInParam(i)->getTypeIx()]);
				}
			}
			break;
		case PISDF_SUBTYPE_BROADCAST:
			for(int j=0; j<brv[bodyIt.currentIx()]; j++){
				job->bodies[bodyIt.currentIx()][j] = topSrdag->addBroadcast(pi_vertex->getNOutEdge());
			}
			break;
		case PISDF_SUBTYPE_JOIN:
			for(int j=0; j<brv[bodyIt.currentIx()]; j++){
				job->bodies[bodyIt.currentIx()][j] = topSrdag->addJoin(pi_vertex->getNInEdge());
			}
			break;
		case PISDF_SUBTYPE_FORK:
			for(int j=0; j<brv[bodyIt.currentIx()]; j++){
				job->bodies[bodyIt.currentIx()][j] = topSrdag->addFork(pi_vertex->getNOutEdge());
			}
			break;
		default:
			throw "Unexpected Interface vertex in AddVertices\n";
		}
	}
}

void addCAVertices(SRDAGGraph *topSrdag, transfoJob *job, Stack* stack){
    job->configs = sAlloc(stack, job->graph->getNConfig(), SRDAGVertex*);
	PiSDFVertexIterator configIt = job->graph->getConfigIterator();
    FOR_IT(configIt){
		job->configs[configIt.currentIx()] = topSrdag->addVertex(configIt.current());
		if(configIt.current()->getType() == PISDF_TYPE_CONFIG){
			for(int i=0; i<configIt.current()->getNOutParam(); i++){
				job->configs[configIt.currentIx()]->addOutParam(i, &(job->paramValues[configIt.current()->getOutParam(i)->getTypeIx()]));
			}
		}
	}
}
