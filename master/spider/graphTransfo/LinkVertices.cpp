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

#include "LinkVertices.h"
#include "GraphTransfo.h"

#include <graphs/PiSDF/PiSDFGraph.h>
#include <graphs/PiSDF/PiSDFVertex.h>
#include <graphs/PiSDF/PiSDFEdge.h>

#include <graphs/SRDAG/SRDAGGraph.h>
#include <graphs/SRDAG/SRDAGVertex.h>
#include <graphs/SRDAG/SRDAGEdge.h>

#include <stdio.h>
#include <cmath>
#include <algorithm>

#define MAX_VERTEX_REPETITION 1000

typedef enum {VERTEX, EDGE} nodeType;

typedef struct node{
	int nb;
	int rates[MAX_VERTEX_REPETITION];
    nodeType type;
	//todo union ?
		struct {
			SRDAGVertex* ptr;
			int portIx;
		} vertex;
		SRDAGEdge* edge;
} node;

void linkCAVertices(SRDAGGraph *topSrdag, transfoJob *job){
	for(int sr_ca_Ix=0; sr_ca_Ix<job->graph->getNConfig(); sr_ca_Ix++){
		SRDAGVertex *sr_ca = job->configs[sr_ca_Ix];
		PiSDFVertex *pi_ca = sr_ca->getReference();

		for(int inEdgeIx=0; inEdgeIx<pi_ca->getNInEdge(); inEdgeIx++){
			if(sr_ca->getInEdge(inEdgeIx) != 0){
				/* Edge already present, do nothing */
			}else{
				PiSDFEdge* pi_edge = pi_ca->getInEdge(inEdgeIx);
				PiSDFVertex* pi_src = pi_edge->getSrc();
				int cons = pi_edge->resolveCons(job);
				int prod = pi_edge->resolveProd(job);

				switch(pi_src->getType()){
				case PISDF_TYPE_IF:{
					if(pi_src->getSubType() != PISDF_SUBTYPE_INPUT_IF)
						throw "Error in graph transfo, Output IF connected to Config input\n";

					SRDAGEdge* srcEdge = job->inputIfs[pi_src->getTypeId()];
					if(srcEdge->getRate() == cons){
						srcEdge->connectSnk(sr_ca, inEdgeIx);
					}else{
						/* For now create RB, can do better to reduce RBs */
						SRDAGVertex* sr_rb = topSrdag->addRoundBuffer();
						srcEdge->connectSnk(sr_rb, 0);

						SRDAGEdge* newEdge = topSrdag->addEdge();
						newEdge->connectSrc(sr_rb, 0);
						newEdge->connectSnk(sr_ca, inEdgeIx);
						newEdge->setRate(cons);
					}
					break;}
				case PISDF_TYPE_CONFIG:{
					SRDAGVertex* sr_src = job->configs[pi_src->getTypeId()];
					SRDAGEdge* sr_edge = sr_src->getOutEdge(pi_edge->getSrcPortIx());

					if(sr_edge == 0){
						sr_edge = topSrdag->addEdge();
						sr_edge->connectSrc(sr_src, pi_edge->getSrcPortIx());
						sr_edge->setRate(prod);
					}

					sr_edge->connectSnk(sr_ca, inEdgeIx);
					if(prod != cons)
						throw "Error in graph transfo, cons/prod between Configs doesn't match\n";
					break;}
				default:
					throw "Error in graph transfo, Normal vertex precede config one\n";
				}
			}
		}

		for(int outEdgeIx=0; outEdgeIx<pi_ca->getNOutEdge(); outEdgeIx++){
			if(sr_ca->getOutEdge(outEdgeIx) != 0){
				/* Edge already present, do nothing */
			}else{
				PiSDFEdge* pi_edge = pi_ca->getOutEdge(outEdgeIx);
				PiSDFVertex* pi_snk = pi_edge->getSnk();
				int cons = pi_edge->resolveCons(job);
				int prod = pi_edge->resolveProd(job);

				switch(pi_snk->getType()){
				case PISDF_TYPE_IF:{
					if(pi_snk->getSubType() != PISDF_SUBTYPE_OUTPUT_IF)
						throw "Error in graph transfo, Input IF connected to Config output\n";

					SRDAGEdge* snkEdge = job->outputIfs[pi_snk->getTypeId()];
					if(snkEdge->getRate() == prod){
						snkEdge->connectSnk(sr_ca, outEdgeIx);
					}else{
						/* For now create RB, can do better to reduce RBs */
						SRDAGVertex* sr_rb = topSrdag->addRoundBuffer();
						snkEdge->connectSrc(sr_rb, 0);

						SRDAGEdge* newEdge = topSrdag->addEdge();
						newEdge->connectSrc(sr_ca, outEdgeIx);
						newEdge->connectSnk(sr_rb, 0);
						newEdge->setRate(prod);
					}
					break;}
				case PISDF_TYPE_CONFIG:{
					SRDAGVertex* sr_snk = job->configs[pi_snk->getTypeId()];
					SRDAGEdge* sr_edge = sr_snk->getInEdge(pi_edge->getSnkPortIx());

					if(sr_edge == 0){
						sr_edge = topSrdag->addEdge();
						sr_edge->connectSnk(sr_snk, pi_edge->getSnkPortIx());
						sr_edge->setRate(cons);
					}

					sr_edge->connectSrc(sr_ca, outEdgeIx);

					if(cons == prod)
						throw "Error in graph transfo, cons/prod between configs doesn't match\n";
					break;}
				case PISDF_TYPE_BODY:{
					SRDAGEdge* sr_edge = topSrdag->addEdge();
					sr_edge->connectSrc(sr_ca, outEdgeIx);
					sr_edge->setRate(prod);
					break;}
				default:
					throw "Error in graph transfo, Unhandled case\n";
				}
			}
		}
	}
}

void linkSRVertices(SRDAGGraph *topSrdag, transfoJob *job, int *brv, Stack* stack){
	PiSDFGraph* currentPiSDF = job->graph;

	for (int i = 0; i < currentPiSDF->getNEdge(); i++) {
		PiSDFEdge *edge = currentPiSDF->getEdge(i);

		// Already treated edge
		if(edge->getSnk()->getType() == PISDF_TYPE_CONFIG)
			continue;

		int nbDelays = edge->resolveDelay(job);
		int piSrcIx = edge->getSrcPortIx();

		int nbSourceRepetitions, nbSinkRepetitions;

		if(edge->getSrc()->getType() == PISDF_TYPE_BODY)
			nbSourceRepetitions = brv[edge->getSrc()->getTypeId()];
		else
			nbSourceRepetitions = 1;

		if(edge->getSnk()->getType() == PISDF_TYPE_BODY)
			nbSinkRepetitions = brv[edge->getSnk()->getTypeId()];
		else
			nbSinkRepetitions = 1;

		int sourceProduction = edge->resolveProd(job);
		int sinkConsumption = edge->resolveCons(job);

//		if(edge->getSrc()->getType() != PISDF_TYPE_BODY){
//			sourceProduction = sinkConsumption*nbSinkRepetitions;
//		}
//		if(edge->getSnk()->getType() != PISDF_TYPE_BODY){
//			sinkConsumption = sourceProduction*nbSourceRepetitions;
//		}

		int sourceIndex = 0, sinkIndex = 0;
		int curSourceToken, curSinkToken;

		SRDAGVertex** srcRepetitions;
		SRDAGEdge** snkRepetitions;

		bool sinkNeedEnd = false;
		int beforelastCons, firstPiSrcIx;
		int lastCons;

		int brIx = -1;
		int forkIx = -1;
		int joinIx = -1;

		// Fill source/sink repetition list
		switch(edge->getSrc()->getType()){
		case PISDF_TYPE_CONFIG:
			if(sourceProduction*1 == sinkConsumption*nbSinkRepetitions){
				// No need of Broadcast
				srcRepetitions = CREATE_MUL(stack, 1, SRDAGVertex*);
				*srcRepetitions = job->configs[edge->getSrc()->getTypeId()];
				curSourceToken = sourceProduction;
				firstPiSrcIx = piSrcIx;
			}else{
				bool perfectBr = sinkConsumption*nbSinkRepetitions%sourceProduction == 0;
				int nBr = sinkConsumption*nbSinkRepetitions/sourceProduction;
				if(!perfectBr) nBr++;

				curSourceToken = sourceProduction;
				nbSourceRepetitions = nBr;

				if(perfectBr){
					firstPiSrcIx = piSrcIx;
					SRDAGVertex* broadcast = topSrdag->addBroadcast(MAX_IO_EDGES);
					SRDAGEdge* configEdge = job->configs[edge->getSrc()->getTypeId()]->getOutEdge(piSrcIx);

					brIx = broadcast->getId();
					configEdge->connectSnk(broadcast, 0);

					srcRepetitions = CREATE_MUL(stack, nBr, SRDAGVertex*);
					for(int i=0; i<nBr; i++)
						srcRepetitions[i] = broadcast;
				}else{
					firstPiSrcIx = piSrcIx;
					lastCons = sourceProduction - sinkConsumption*nbSinkRepetitions;
					sinkNeedEnd = true;

					SRDAGVertex* broadcast = topSrdag->addBroadcast(MAX_IO_EDGES);
					SRDAGEdge* configEdge = job->configs[edge->getSrc()->getTypeId()]->getOutEdge(piSrcIx);

					brIx = broadcast->getId();
					configEdge->connectSnk(broadcast, 0);

					srcRepetitions = CREATE_MUL(stack, nBr, SRDAGVertex*);
					for(int i=0; i<nBr; i++)
						srcRepetitions[i] = broadcast;
				}

			}
			break;
		case PISDF_TYPE_IF:
			if(sourceProduction*1 == sinkConsumption*nbSinkRepetitions){
				// No need of Broadcast
				srcRepetitions = CREATE_MUL(stack, 1, SRDAGVertex*);
				srcRepetitions[0] = job->inputIfs[edge->getSrc()->getTypeId()]->getSrc();
				if(srcRepetitions[0] == 0){
					srcRepetitions[0] = topSrdag->addRoundBuffer();
					job->inputIfs[edge->getSrc()->getTypeId()]->connectSnk(srcRepetitions[0], 0);
					piSrcIx = 0;
					firstPiSrcIx = piSrcIx;
				}else{
					piSrcIx = job->inputIfs[edge->getSrc()->getTypeId()]->getSrcPortIx();
					firstPiSrcIx = piSrcIx;
				}
				curSourceToken = sourceProduction;
			}else{
				bool perfectBr = sinkConsumption*nbSinkRepetitions%sourceProduction == 0;
				int nBr = sinkConsumption*nbSinkRepetitions/sourceProduction;
				if(!perfectBr) nBr++;

				curSourceToken = sourceProduction;
				nbSourceRepetitions = nBr;

				if(perfectBr){
					SRDAGVertex* broadcast = topSrdag->addBroadcast(MAX_IO_EDGES);
					SRDAGEdge* interfaceEdge = job->inputIfs[edge->getSrc()->getTypeId()];
					piSrcIx = job->inputIfs[edge->getSrc()->getTypeId()]->getSrcPortIx();
					firstPiSrcIx = piSrcIx;
					brIx = broadcast->getId();

					interfaceEdge->connectSnk(broadcast, 0);

					srcRepetitions = CREATE_MUL(stack, nBr, SRDAGVertex*);
					for(int i=0; i<nBr; i++)
						srcRepetitions[i] = broadcast;
				}else{
					lastCons = sourceProduction - sinkConsumption*nbSinkRepetitions;
					sinkNeedEnd = true;

					SRDAGVertex* broadcast = topSrdag->addBroadcast(MAX_IO_EDGES);
					SRDAGEdge* interfaceEdge = job->inputIfs[edge->getSrc()->getTypeId()];
					piSrcIx = job->inputIfs[edge->getSrc()->getTypeId()]->getSrcPortIx();
					firstPiSrcIx = piSrcIx;
					brIx = broadcast->getId();

					interfaceEdge->connectSnk(broadcast, 0);

					srcRepetitions = CREATE_MUL(stack, nBr, SRDAGVertex*);
					for(int i=0; i<nBr; i++)
						srcRepetitions[i] = broadcast;
				}

			}
			break;
		case PISDF_TYPE_BODY:
			if(nbDelays == 0){
				srcRepetitions = CREATE_MUL(stack, nbSourceRepetitions, SRDAGVertex*);
				memcpy(srcRepetitions, job->bodies[edge->getSrc()->getTypeId()], nbSourceRepetitions*sizeof(SRDAGVertex*));
				curSourceToken = sourceProduction;
				firstPiSrcIx = piSrcIx;
			}else{
				srcRepetitions = CREATE_MUL(stack, nbSourceRepetitions+1, SRDAGVertex*);

				if(edge->getDelaySetter()){
					PiSDFVertex* ifDelaySetter = edge->getDelaySetter();
					SRDAGEdge* setterEdge = job->inputIfs[ifDelaySetter->getTypeId()];
					if(setterEdge->getRate() == nbDelays){
						srcRepetitions[0] = setterEdge->getSrc();
						if(srcRepetitions[0] == 0){
							srcRepetitions[0] = topSrdag->addRoundBuffer();
							job->inputIfs[edge->getSrc()->getTypeId()]->connectSnk(srcRepetitions[0], 0);
							firstPiSrcIx = 0;
						}else{
							firstPiSrcIx = setterEdge->getSrcPortIx();
						}
					}else{
						throw "Setter of a delay must be of the same rate than delay";
					}
				}else{
					srcRepetitions[0] = topSrdag->addInit();
					firstPiSrcIx = piSrcIx;
				}

				memcpy(srcRepetitions+1, job->bodies[edge->getSrc()->getTypeId()], nbSourceRepetitions*sizeof(SRDAGVertex*));

				nbSourceRepetitions++;
				curSourceToken = nbDelays;
			}
			break;
		}

		switch(edge->getSnk()->getType()){
		case PISDF_TYPE_CONFIG:
			throw "Should be impossible";
			break;
		case PISDF_TYPE_IF:
			if(sinkConsumption*1 == sourceProduction*nbSourceRepetitions){
				// No need of specific thing
				snkRepetitions = CREATE_MUL(stack, 1, SRDAGEdge*);
				*snkRepetitions = job->outputIfs[edge->getSnk()->getTypeId()];
				curSinkToken = sinkConsumption;
				beforelastCons = sinkConsumption;
				lastCons = sinkConsumption;
			}else{
				float nDroppedTokens = sourceProduction*nbSourceRepetitions-sinkConsumption;
				int nEnd = std::ceil(nDroppedTokens/sourceProduction);

				snkRepetitions = CREATE_MUL(stack, nEnd+1, SRDAGEdge*);
				for(int i=0; i<nEnd; i++){
					snkRepetitions[i] = topSrdag->addEdge();
					snkRepetitions[i]->connectSnk(topSrdag->addEnd(), 0);
				}
				snkRepetitions[nEnd] = job->outputIfs[edge->getSnk()->getTypeId()];

				nbSinkRepetitions = nEnd+1;

				beforelastCons = nDroppedTokens-(nEnd-1)*sourceProduction;
				lastCons = sinkConsumption;
				sinkConsumption = sourceProduction;

				if(nEnd == 1)
					curSinkToken = beforelastCons;
				else
					curSinkToken = sinkConsumption;
			}
			break;
		case PISDF_TYPE_BODY:
			if(nbDelays == 0){
				if(sinkNeedEnd){
					snkRepetitions = CREATE_MUL(stack, nbSinkRepetitions+1, SRDAGEdge*);

					for(int i=0; i<nbSinkRepetitions; i++){
						snkRepetitions[i] = topSrdag->addEdge();
						snkRepetitions[i]->connectSnk(job->bodies[edge->getSnk()->getTypeId()][i], edge->getSnkPortIx());
					}
					snkRepetitions[nbSinkRepetitions] = topSrdag->addEdge();
					snkRepetitions[nbSinkRepetitions]->connectSnk(topSrdag->addEnd(), 0);
					nbSinkRepetitions++;
				}else{
					snkRepetitions = CREATE_MUL(stack, nbSinkRepetitions, SRDAGEdge*);

					for(int i=0; i<nbSinkRepetitions; i++){
						snkRepetitions[i] = topSrdag->addEdge();
						snkRepetitions[i]->setRate(sinkConsumption);
						snkRepetitions[i]->connectSnk(job->bodies[edge->getSnk()->getTypeId()][i], edge->getSnkPortIx());
					}
				}

				curSinkToken   = sinkConsumption;
				beforelastCons = sinkConsumption;
				lastCons = sinkConsumption;
			}else{
				snkRepetitions = CREATE_MUL(stack, nbSinkRepetitions+1, SRDAGEdge*);

				for(int i=0; i<nbSinkRepetitions; i++){
					snkRepetitions[i] = topSrdag->addEdge();
					snkRepetitions[i]->connectSnk(job->bodies[edge->getSnk()->getTypeId()][i], edge->getSnkPortIx());
				}
				snkRepetitions[nbSinkRepetitions] = topSrdag->addEdge();
				snkRepetitions[nbSinkRepetitions]->connectSnk(topSrdag->addEnd(), 0);

				nbSinkRepetitions++;
				curSinkToken   = sinkConsumption;
				beforelastCons = sinkConsumption;
				lastCons = nbDelays;
			}
			break;
		}

		// Iterating until all consumptions are "satisfied".
		while (sourceIndex < nbSourceRepetitions
				|| sinkIndex < nbSinkRepetitions) {
			SRDAGVertex* sourceVertex;
			SRDAGEdge* sinkEdge;
			int sourcePortId;

			// Production/consumption rate for the current source/target.
			int rest = std::min(curSourceToken, curSinkToken); // Minimum.

			/*
			 * Adding explode/implode vertices if required.
			 */

			if (rest < curSourceToken
					&& (srcRepetitions[sourceIndex]->getId() != forkIx)){
				// Adding an explode vertex.
				SRDAGVertex *fork_vertex = topSrdag->addFork(MAX_IO_EDGES);
				forkIx = fork_vertex->getId();

				// Replacing the source vertex by the explode vertex in the array of sources.
				SRDAGVertex *origin_vertex = srcRepetitions[sourceIndex];
				srcRepetitions[sourceIndex] = fork_vertex;

				SRDAGVertex* sourceVertex;
				int sourcePortId;

				// TODO check !!!!
				switch(origin_vertex->getType()){
				case SRDAG_NORMAL:
				case SRDAG_FORK:
				case SRDAG_JOIN:
					sourceVertex = origin_vertex;
					sourcePortId = piSrcIx;
					break;
				case SRDAG_ROUNDBUFFER:
					sourceVertex = origin_vertex;
					sourcePortId = 0;
					break;
				case SRDAG_BROADCAST:
					if(origin_vertex->getId() == brIx){
						sourceVertex = origin_vertex;
						sourcePortId = origin_vertex->getNConnectedOutEdge();
					}else{
						sourceVertex = origin_vertex;
						sourcePortId = piSrcIx;
					}
					break;
				case SRDAG_INIT:
					sourceVertex = origin_vertex;
					sourcePortId = 0;
					break;
				default:
				case SRDAG_END:
					throw "Unexpected case in pisdf transfo";
				}

				if(sourceVertex->getOutEdge(sourcePortId) != 0)
					sourceVertex->getOutEdge(sourcePortId)->connectSnk(fork_vertex, 0);
				else
					// Adding an edge between the source and the fork.
					topSrdag->addEdge(
						sourceVertex, sourcePortId,
						fork_vertex, 0,
						curSourceToken
					);
			}

			if (rest < curSinkToken &&
					snkRepetitions[sinkIndex]->getSnk() &&
					(snkRepetitions[sinkIndex]->getSnk()->getId() != joinIx)){ // Type == 0 indicates it is a normal vertex.

				// Adding an join vertex.
				SRDAGVertex *join_vertex = topSrdag->addJoin(MAX_IO_EDGES);
				joinIx = join_vertex->getId();

				// Replacing the sink vertex by the join vertex in the array of sources.
				snkRepetitions[sinkIndex]->connectSrc(join_vertex, 0);
				snkRepetitions[sinkIndex] = topSrdag->addEdge();
				snkRepetitions[sinkIndex]->connectSnk(join_vertex, 0);

			}else if(snkRepetitions[sinkIndex]->getSnk()
					&& snkRepetitions[sinkIndex]->getSnk()->getId() == joinIx){
				/* Adding the new edge in join*/
				SRDAGVertex *join_vertex = snkRepetitions[sinkIndex]->getSnk();
				snkRepetitions[sinkIndex] = topSrdag->addEdge();
				snkRepetitions[sinkIndex]->connectSnk(join_vertex, join_vertex->getNConnectedInEdge());
			}
			sinkEdge = snkRepetitions[sinkIndex];

			//Creating the new edge between normal vertices or between a normal and an explode/implode one.

			sourceVertex = srcRepetitions[sourceIndex];
			switch(sourceVertex->getType()){
			case SRDAG_BROADCAST:
				if(sourceVertex->getId() == brIx){
					sourcePortId = sourceVertex->getNConnectedOutEdge();
				}else{
					if(sourceIndex == 0)
						sourcePortId = firstPiSrcIx;
					else
						sourcePortId = piSrcIx;
				}
				break;
			case SRDAG_FORK:
				if(sourceVertex->getId() == forkIx)
					sourcePortId = sourceVertex->getNConnectedOutEdge();
				else
					if(sourceIndex == 0)
						sourcePortId = firstPiSrcIx;
					else
						sourcePortId = piSrcIx;
				break;
			case SRDAG_INIT:
				sourcePortId = 0;
				break;
			default:
				if(sourceIndex == 0)
					sourcePortId = firstPiSrcIx;
				else
					sourcePortId = piSrcIx;
				break;
			}

			if(sourceVertex->getOutEdge(sourcePortId) != 0){
				SRDAGEdge* srcEdge = sourceVertex->getOutEdge(sourcePortId);
				sinkEdge->setAlloc(srcEdge->getAlloc());
				sinkEdge->setAllocIx(srcEdge->getAllocIx());
				sinkEdge->setRate(srcEdge->getRate());

				topSrdag->delEdge(srcEdge);
				sinkEdge->connectSrc(sourceVertex, sourcePortId);
			}else{
				sinkEdge->connectSrc(sourceVertex, sourcePortId);
				sinkEdge->setRate(rest);
			}

			// Update the number of token produced/consumed by the current source/target.
			curSourceToken -= rest;
			curSinkToken -= rest;

			if(curSourceToken == 0){
				sourceIndex++;
				curSourceToken = sourceProduction;
			}

			if(curSinkToken == 0){
				sinkIndex++;
				if(sinkIndex == nbSinkRepetitions-2)
					curSinkToken = beforelastCons;
				else if(sinkIndex == nbSinkRepetitions-1)
					curSinkToken = lastCons;
				else
					curSinkToken = sinkConsumption;
			}
		}

		stack->free(srcRepetitions);
		stack->free(snkRepetitions);
	}
}
