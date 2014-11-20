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

#include "ComputeBRV.h"
#include "GraphTransfo.h"
#include "topologyMatrix.h"

#include <graphs/PiSDF/PiSDFGraph.h>
#include <graphs/PiSDF/PiSDFVertex.h>
#include <graphs/PiSDF/PiSDFEdge.h>

#include <graphs/SRDAG/SRDAGGraph.h>
#include <graphs/SRDAG/SRDAGVertex.h>
#include <graphs/SRDAG/SRDAGEdge.h>

#include <cstring>
#include <cstdio>
#include <cmath>
#include <algorithm>

void computeBRV(SRDAGGraph *topSrdag, transfoJob *job, int* brv, Stack* stack){
	int i;
	int nbVertices = job->graph->getNBody();

	PiSDFEdgeIterator edgeIt = job->graph->getEdgeIterator();
	PiSDFVertexIterator vertexIt;

	/* Compute nbEdges */
	int nbEdges = 0;
	FOR_IT(edgeIt){
		if(edgeIt.current()->getSrc() != edgeIt.current()->getSnk()
			&& edgeIt.current()->getSrc()->getType() ==  PISDF_TYPE_BODY
			&& edgeIt.current()->getSnk()->getType() ==  PISDF_TYPE_BODY){
			nbEdges++;
		}
	}

    int* topo_matrix = sAlloc(stack, nbEdges*nbVertices, int);
	memset(topo_matrix, 0, nbEdges*nbVertices*sizeof(int));

	/* Fill the topology matrix(nbEdges x nbVertices) */
	nbEdges = 0; // todo do better with the nbEdges var
	FOR_IT(edgeIt){
		if(edgeIt.current()->getSrc() != edgeIt.current()->getSnk()
			&& edgeIt.current()->getSrc()->getType() ==  PISDF_TYPE_BODY
			&& edgeIt.current()->getSnk()->getType() ==  PISDF_TYPE_BODY){
			int prod = edgeIt.current()->resolveProd(job->paramValues, job->graph->getNParam());
			int cons = edgeIt.current()->resolveCons(job->paramValues, job->graph->getNParam());

			if(prod <= 0 || cons <= 0 )
				throw "Error Bad prod/cons resolved\n";

			topo_matrix[nbEdges*nbVertices + edgeIt.current()->getSrc()->getTypeId()] = prod;
			topo_matrix[nbEdges*nbVertices + edgeIt.current()->getSnk()->getTypeId()] = -cons;
			nbEdges++;
		}
	}

	printf("topoMatrix:\n");
	for(i=0; i<nbEdges; i++){
		int j;
		for(j=0; j<nbVertices; j++){
			printf("%4d ", topo_matrix[i*nbVertices+j]);
		}
		printf("\n");
	}

	/* Compute nullSpace */
	nullSpace(topo_matrix, brv, nbEdges, nbVertices, stack);

	/* Updating the productions of the round buffer vertices. */
	int coef=1;

	/* Looking on interfaces */
	vertexIt = job->graph->getInputIfIterator();
	FOR_IT(vertexIt){
		PiSDFEdge* edge = vertexIt.current()->getOutEdge(0);
		/* Only if IF<->Body edge */
		if(edge->getSnk()->getType() == PISDF_TYPE_BODY){
			float prod = edge->resolveProd(job->paramValues, job->graph->getNParam());
			float cons = edge->resolveCons(job->paramValues, job->graph->getNParam());
			float nbRepet = brv[edge->getSnk()->getTypeId()];
			coef = std::max(coef, (int)std::ceil(prod/(cons*nbRepet)));
		}
	}
	vertexIt = job->graph->getOutputIfIterator();
	FOR_IT(vertexIt){
		PiSDFEdge* edge = vertexIt.current()->getInEdge(0);
		/* Only if IF<->Body edge */
		if(edge->getSrc()->getType() == PISDF_TYPE_BODY){
			float prod = edge->resolveProd(job->paramValues, job->graph->getNParam());
			float cons = edge->resolveCons(job->paramValues, job->graph->getNParam());
			float nbRepet = brv[edge->getSrc()->getTypeId()];
			coef = std::max(coef, (int)std::ceil(cons/(prod*nbRepet)));
		}
	}
	/* Looking on implicit RB between Config and Body */
	vertexIt = job->graph->getConfigIterator();
	FOR_IT(vertexIt){
		for(int i=0; i<vertexIt.current()->getNOutEdge(); i++){
			PiSDFEdge* edge = vertexIt.current()->getOutEdge(i);
			/* Only if Config<->Body edge */
			if(edge->getSnk()->getType() == PISDF_TYPE_BODY){
				float prod = edge->resolveProd(job->paramValues, job->graph->getNParam());
				float cons = edge->resolveCons(job->paramValues, job->graph->getNParam());
				float nbRepet = brv[edge->getSnk()->getTypeId()];
				coef = std::max(coef, (int)std::ceil(prod/(cons*nbRepet)));
			}
		}
	}

	for(i=0; i<nbVertices; i++){
		brv[i] *= coef;
	}

	printf("brv:\n");
	for(i=0; i<nbVertices; i++){
		printf("%4d ", brv[i]);
	}
	printf("\n");
}
