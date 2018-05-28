/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2016) :
 *
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2016)
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

typedef enum {VERTEX, EDGE} nodeType;

typedef struct node{
	int nb;
	int rates[MAX_IO_EDGES];
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
						throw std::runtime_error("Error in graph transfo, Output IF connected to Config input\n");

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
						throw std::runtime_error("Error in graph transfo, cons/prod between Configs doesn't match\n");
					break;}
				default:
					throw std::runtime_error("Error in graph transfo, Normal vertex precede config one\n");
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
						throw std::runtime_error("Error in graph transfo, Input IF connected to Config output\n");

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
						throw std::runtime_error("Error in graph transfo, cons/prod between configs doesn't match\n");
					break;}
				case PISDF_TYPE_BODY:{
					SRDAGEdge* sr_edge = topSrdag->addEdge();
					sr_edge->connectSrc(sr_ca, outEdgeIx);
					sr_edge->setRate(prod);
					break;}
				default:
					throw std::runtime_error("Error in graph transfo, Unhandled case\n");
				}
			}
		}
	}
}

void linkSRVertices(SRDAGGraph *topSrdag, transfoJob *job, int *brv){
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

		typedef struct SrcConnection{
			SRDAGVertex* src;
			int prod;
			int portIx;
		} SrcConnection;

		typedef struct SnkConnection{
			SRDAGEdge* edge;
			int cons;
		} SnkConnection;

		SrcConnection* srcConnections = 0;
		SnkConnection* snkConnections = 0;
		bool sinkNeedEnd = false;

		int forkIx = -1;
		int joinIx = -1;

		// Fill source/sink repetition list
		switch(edge->getSrc()->getType()){
		case PISDF_TYPE_CONFIG:
			if(sourceProduction*1 == sinkConsumption*nbSinkRepetitions){
				// No need of Broadcast
				srcConnections = CREATE_MUL(TRANSFO_STACK, 1, SrcConnection);

				srcConnections->src = job->configs[edge->getSrc()->getTypeId()];
				srcConnections->prod = sourceProduction;
				srcConnections->portIx = piSrcIx;
			}else{
				bool perfectBr = sinkConsumption*nbSinkRepetitions%sourceProduction == 0;
				int nBr = sinkConsumption*nbSinkRepetitions/sourceProduction;
				if(!perfectBr) nBr++;

				curSourceToken = sourceProduction;
				nbSourceRepetitions = nBr;

				if(perfectBr){
					SRDAGVertex* broadcast = topSrdag->addBroadcast(MAX_IO_EDGES);
					SRDAGEdge* configEdge = job->configs[edge->getSrc()->getTypeId()]->getOutEdge(piSrcIx);

					configEdge->connectSnk(broadcast, 0);

					srcConnections = CREATE_MUL(TRANSFO_STACK, nBr, SrcConnection);
					for(int i=0; i<nBr; i++){
						srcConnections[i].src = broadcast;
						srcConnections[i].portIx = i;
						srcConnections[i].prod = sourceProduction;
					}
				}else{
//					lastCons = sourceProduction - sinkConsumption*nbSinkRepetitions;
					sinkNeedEnd = true;

					SRDAGVertex* broadcast = topSrdag->addBroadcast(MAX_IO_EDGES);
					SRDAGEdge* configEdge = job->configs[edge->getSrc()->getTypeId()]->getOutEdge(piSrcIx);

					configEdge->connectSnk(broadcast, 0);

					srcConnections = CREATE_MUL(TRANSFO_STACK, nBr, SrcConnection);
					for(int i=0; i<nBr; i++){
						srcConnections[i].src = broadcast;
						srcConnections[i].portIx = i;
						srcConnections[i].prod = sourceProduction;
					}
				}
			}
			break;
		case PISDF_TYPE_IF:
			if(sourceProduction*1 == sinkConsumption*nbSinkRepetitions){
				// No need of Broadcast
				srcConnections = CREATE_MUL(TRANSFO_STACK, 1, SrcConnection);
				srcConnections[0].src = job->inputIfs[edge->getSrc()->getTypeId()]->getSrc();
				if(srcConnections[0].src == 0){
					srcConnections[0].src = topSrdag->addRoundBuffer();
					job->inputIfs[edge->getSrc()->getTypeId()]->connectSnk(srcConnections[0].src, 0);
					srcConnections[0].portIx = 0;
					srcConnections[0].prod = sourceProduction;
				}else{
					srcConnections[0].portIx = job->inputIfs[edge->getSrc()->getTypeId()]->getSrcPortIx();
					srcConnections[0].prod = sourceProduction;
				}
			}else{
				bool perfectBr = sinkConsumption*nbSinkRepetitions%sourceProduction == 0;
				int nBr = sinkConsumption*nbSinkRepetitions/sourceProduction;
				if(!perfectBr) nBr++;

				nbSourceRepetitions = nBr;
				sinkNeedEnd = !perfectBr;
//					lastCons = sourceProduction - sinkConsumption*nbSinkRepetitions;

				SRDAGVertex* broadcast = topSrdag->addBroadcast(MAX_IO_EDGES);
				job->inputIfs[edge->getSrc()->getTypeId()]->connectSnk(broadcast, 0);

				srcConnections = CREATE_MUL(TRANSFO_STACK, nBr, SrcConnection);
				for(int i=0; i<nBr; i++){
					srcConnections[i].src = broadcast;
					srcConnections[i].portIx = i;
					srcConnections[i].prod = sourceProduction;
				}
			}
			break;
		case PISDF_TYPE_BODY:
			if(nbDelays == 0){
				srcConnections = CREATE_MUL(TRANSFO_STACK, nbSourceRepetitions, SrcConnection);
				for(int i=0; i<nbSourceRepetitions; i++){
					srcConnections[i].src = job->bodies[edge->getSrc()->getTypeId()][i];
					srcConnections[i].portIx = piSrcIx;
					srcConnections[i].prod = sourceProduction;
				}
			}else{
				nbSourceRepetitions++;
				srcConnections = CREATE_MUL(TRANSFO_STACK, nbSourceRepetitions, SrcConnection);

				if(edge->getDelaySetter()){
					PiSDFVertex* ifDelaySetter = edge->getDelaySetter();
					SRDAGEdge* setterEdge = job->inputIfs[ifDelaySetter->getTypeId()];
					if(setterEdge->getRate() == nbDelays){
						srcConnections[0].src = setterEdge->getSrc();
						if(srcConnections[0].src == 0){
							srcConnections[0].src = topSrdag->addRoundBuffer();
							job->inputIfs[edge->getSrc()->getTypeId()]->connectSnk(srcConnections[0].src, 0);
							srcConnections[0].portIx = 0;
						}else{
							srcConnections[0].portIx = setterEdge->getSrcPortIx();
						}
					}else{
						throw std::runtime_error("Setter of a delay must be of the same rate than delay");
					}
				}else{
					srcConnections[0].src = topSrdag->addInit();
					srcConnections[0].portIx = 0;
				}
				srcConnections[0].prod = nbDelays;

				for(int i=1; i<nbSourceRepetitions; i++){
					srcConnections[i].src = job->bodies[edge->getSrc()->getTypeId()][i-1];
					srcConnections[i].portIx = piSrcIx;
					srcConnections[i].prod = sourceProduction;
				}
			}
			break;
		}

		switch(edge->getSnk()->getType()){
		case PISDF_TYPE_CONFIG:
			throw std::runtime_error("Should be impossible");
			break;
		case PISDF_TYPE_IF:
			if(sinkConsumption*1 == sourceProduction*nbSourceRepetitions){
				// No need of specific thing
				snkConnections = CREATE_MUL(TRANSFO_STACK, 1, SnkConnection);
				snkConnections[0].edge = job->outputIfs[edge->getSnk()->getTypeId()];
				snkConnections[0].cons = sinkConsumption;
			}else{
				float nDroppedTokens = sourceProduction*nbSourceRepetitions-sinkConsumption;
				int nEnd = std::ceil(nDroppedTokens/sourceProduction);

				nbSinkRepetitions = nEnd+1;
				snkConnections = CREATE_MUL(TRANSFO_STACK, nbSinkRepetitions, SnkConnection);
				for(int i=0; i<nEnd; i++){
					snkConnections[i].edge = topSrdag->addEdge();
					snkConnections[i].edge->connectSnk(topSrdag->addEnd(), 0);
					snkConnections[i].cons = sourceProduction;
				}
				snkConnections[nEnd-1].cons = nDroppedTokens-(nEnd-1)*sourceProduction;

				snkConnections[nEnd].edge = job->outputIfs[edge->getSnk()->getTypeId()];
				snkConnections[nEnd].cons = sinkConsumption;
			}
			break;
		case PISDF_TYPE_BODY:
			if(nbDelays == 0){
				if(sinkNeedEnd){
					snkConnections = CREATE_MUL(TRANSFO_STACK, nbSinkRepetitions+1, SnkConnection);

					for(int i=0; i<nbSinkRepetitions; i++){
						snkConnections[i].edge = topSrdag->addEdge();
						snkConnections[i].edge->connectSnk(job->bodies[edge->getSnk()->getTypeId()][i], edge->getSnkPortIx());
						snkConnections[i].cons = sinkConsumption;
					}
					snkConnections[nbSinkRepetitions].edge = topSrdag->addEdge();
					snkConnections[nbSinkRepetitions].edge->connectSnk(topSrdag->addEnd(), 0);
					snkConnections[nbSinkRepetitions].cons = sourceProduction - sinkConsumption*nbSinkRepetitions;
					nbSinkRepetitions++;
				}else{
					snkConnections = CREATE_MUL(TRANSFO_STACK, nbSinkRepetitions, SnkConnection);

					for(int i=0; i<nbSinkRepetitions; i++){
						snkConnections[i].edge = topSrdag->addEdge();
						snkConnections[i].edge->connectSnk(job->bodies[edge->getSnk()->getTypeId()][i], edge->getSnkPortIx());
						snkConnections[i].cons = sinkConsumption;
					}
				}
			}else{
				snkConnections = CREATE_MUL(TRANSFO_STACK, nbSinkRepetitions+1, SnkConnection);

				for(int i=0; i<nbSinkRepetitions; i++){
					snkConnections[i].edge = topSrdag->addEdge();
					snkConnections[i].edge->connectSnk(job->bodies[edge->getSnk()->getTypeId()][i], edge->getSnkPortIx());
					snkConnections[i].cons = sinkConsumption;
				}

				if(edge->getDelayGetter()){
					PiSDFVertex* ifDelayGetter = edge->getDelayGetter();
					SRDAGEdge* getterEdge = job->outputIfs[ifDelayGetter->getTypeId()];
					if(getterEdge->getRate() == nbDelays){
						snkConnections[nbSinkRepetitions].edge = getterEdge;
						snkConnections[nbSinkRepetitions].cons = nbDelays;
					}else{
						throw std::runtime_error("Getter of a delay must be of the same rate than delay");
					}
				}else{
					snkConnections[nbSinkRepetitions].edge = topSrdag->addEdge();
					snkConnections[nbSinkRepetitions].edge->connectSnk(topSrdag->addEnd(), 0);
					snkConnections[nbSinkRepetitions].cons = nbDelays;
				}

				nbSinkRepetitions++;
			}
			break;
		}

		curSourceToken = srcConnections[0].prod;
		curSinkToken = snkConnections[0].cons;

		// Iterating until all consumptions are "satisfied".
		while (sourceIndex < nbSourceRepetitions
				|| sinkIndex < nbSinkRepetitions) {
			// Production/consumption rate for the current source/target.
			int rest = std::min(curSourceToken, curSinkToken); // Minimum.

			/*
			 * Adding explode/implode vertices if required.
			 */

			if (rest < curSourceToken
					&& (srcConnections[sourceIndex].src->getId() != forkIx)){
				// Adding an explode vertex.
				SRDAGVertex *fork_vertex = topSrdag->addFork(MAX_IO_EDGES);
				forkIx = fork_vertex->getId();

				if(srcConnections[sourceIndex].src->getOutEdge(srcConnections[sourceIndex].portIx) != 0)
					srcConnections[sourceIndex].src->getOutEdge(srcConnections[sourceIndex].portIx)->connectSnk(fork_vertex, 0);
				else
					// Adding an edge between the source and the fork.
					topSrdag->addEdge(
						srcConnections[sourceIndex].src,
						srcConnections[sourceIndex].portIx,
						fork_vertex,
						0,
						curSourceToken
					);

				// Replacing the source vertex by the explode vertex in the array of sources.
				srcConnections[sourceIndex].src = fork_vertex;
				srcConnections[sourceIndex].portIx = 0;
			}

			if (rest < curSinkToken &&
					snkConnections[sinkIndex].edge->getSnk() &&
					(snkConnections[sinkIndex].edge->getSnk()->getId() != joinIx)){ // Type == 0 indicates it is a normal vertex.

				// Adding an join vertex.
				SRDAGVertex *join_vertex = topSrdag->addJoin(MAX_IO_EDGES);
				joinIx = join_vertex->getId();

				// Replacing the sink vertex by the join vertex in the array of sources.
				snkConnections[sinkIndex].edge->connectSrc(join_vertex, 0);
				snkConnections[sinkIndex].edge->setRate(snkConnections[sinkIndex].cons);
				snkConnections[sinkIndex].edge = topSrdag->addEdge();
				snkConnections[sinkIndex].edge->connectSnk(join_vertex, 0);

			}else if(snkConnections[sinkIndex].edge->getSnk()
					&& snkConnections[sinkIndex].edge->getSnk()->getId() == joinIx){
				/* Adding the new edge in join*/
				SRDAGVertex *join_vertex = snkConnections[sinkIndex].edge->getSnk();
				snkConnections[sinkIndex].edge = topSrdag->addEdge();
				snkConnections[sinkIndex].edge->connectSnk(join_vertex, join_vertex->getNConnectedInEdge());
			}

			//Creating the new edge between normal vertices or between a normal and an explode/implode one.
			SRDAGEdge* srcEdge;
			if((srcEdge = srcConnections[sourceIndex].src->getOutEdge(srcConnections[sourceIndex].portIx)) != 0){
				snkConnections[sinkIndex].edge->setAlloc(srcEdge->getAlloc());
				snkConnections[sinkIndex].edge->setAllocIx(srcEdge->getAllocIx());
				snkConnections[sinkIndex].edge->setRate(srcEdge->getRate());

				topSrdag->delEdge(srcEdge);
				snkConnections[sinkIndex].edge->connectSrc(
						srcConnections[sourceIndex].src,
						srcConnections[sourceIndex].portIx
						);
			}else{
				snkConnections[sinkIndex].edge->connectSrc(
						srcConnections[sourceIndex].src,
						srcConnections[sourceIndex].portIx
						);
				snkConnections[sinkIndex].edge->setRate(rest);
			}

			// Update the number of token produced/consumed by the current source/target.
			curSourceToken -= rest;
			curSinkToken -= rest;

			if(curSourceToken == 0){
				sourceIndex++;
				curSourceToken = srcConnections[sourceIndex].prod;
			}else{
				srcConnections[sourceIndex].portIx++;
			}

			if(curSinkToken == 0){
				sinkIndex++;
				curSinkToken = snkConnections[sinkIndex].cons;
			}
		}

		StackMonitor::free(TRANSFO_STACK, srcConnections);
		StackMonitor::free(TRANSFO_STACK, snkConnections);
	}
}
