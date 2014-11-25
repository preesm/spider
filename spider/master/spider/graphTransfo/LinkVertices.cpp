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

#include <cstdio>
#include <algorithm>

#define MAX_VERTEX_REPETITION 1000
#define MAX_IO_EDGES 100

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
	PiSDFVertexIterator vertexIt = job->graph->getConfigIterator();

	FOR_IT(vertexIt){
		SRDAGVertex *sr_ca = job->configs[vertexIt.currentIx()];
		PiSDFVertex *pi_ca = sr_ca->getReference();

		for(int inEdgeIx=0; inEdgeIx<pi_ca->getNInEdge(); inEdgeIx++){
			if(sr_ca->getInEdge(inEdgeIx) != 0){
				/* Edge already present, do nothing */
			}else{
				PiSDFEdge* pi_edge = pi_ca->getInEdge(inEdgeIx);
				PiSDFVertex* pi_src = pi_edge->getSrc();
				int cons = pi_edge->resolveCons(job->paramValues, job->graph->getNConfig());
				int prod = pi_edge->resolveProd(job->paramValues, job->graph->getNConfig());

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
				int cons = pi_edge->resolveCons(job->paramValues, job->graph->getNConfig());
				int prod = pi_edge->resolveProd(job->paramValues, job->graph->getNConfig());

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

void linkSRVertices(SRDAGGraph *topSrdag, transfoJob *job, int *brv){
	PiSDFEdgeIterator edgeIt = job->graph->getEdgeIterator();
	FOR_IT(edgeIt){
		int j;
		PiSDFEdge *pi_edge  = edgeIt.current();
		PiSDFVertex *pi_src = pi_edge->getSrc();
		PiSDFVertex *pi_snk = pi_edge->getSnk();
		int nbSrcs, nbSnks;
		int srcIx = 0, snkIx = 0;
		int curSrcTk, curSnkTk;

		int pi_cons = pi_edge->resolveCons(job->paramValues, job->graph->getNParam());
		int pi_prod = pi_edge->resolveProd(job->paramValues, job->graph->getNParam());
		int delay = pi_edge->resolveDelay(job->paramValues, job->graph->getNParam());

		typedef struct{
			SRDAGVertex *vertex;
			int port;
		} nodeConnection;

		int nbSrcEdges=0, nbSnkVertices=0;
		SRDAGEdge* srcEdges[MAX_VERTEX_REPETITION];
		nodeConnection snkVertices[MAX_VERTEX_REPETITION];

        if(pi_snk->getType() == PISDF_TYPE_CONFIG)
            continue;

		if(pi_snk->getType() != PISDF_TYPE_BODY
				&& pi_src->getType() != PISDF_TYPE_BODY)
			throw "Unexpected case in LinkVertices\n";

		switch(pi_src->getType()){
		case PISDF_TYPE_BODY:
			nbSrcs 	= brv[pi_src->getTypeId()];
			break;
		case PISDF_TYPE_CONFIG:
		case PISDF_TYPE_IF:
			break;
		}

		switch(pi_snk->getType()){
		case PISDF_TYPE_BODY:
			nbSnks 	= brv[pi_snk->getTypeId()];
			break;
		case PISDF_TYPE_CONFIG:
			/* Skip already treated edge */
			continue;
		case PISDF_TYPE_IF:
			nbSnks = 1;
			pi_cons = pi_prod*nbSrcs;
			break;
		}

		switch(pi_src->getType()){
		case PISDF_TYPE_BODY:
			break;
		case PISDF_TYPE_CONFIG:
			if(pi_snk->getType() == PISDF_TYPE_IF)
				/* Skip already treated edge */
				continue;
			nbSrcs = 1;
			pi_prod = pi_cons*nbSnks;
			break;
		case PISDF_TYPE_IF:
			nbSrcs = 1;
			pi_prod = pi_cons*nbSnks;
			break;
		}

		if(delay){
			curSrcTk = delay;
			nbSrcs++;
			nbSnks++;
		}else{
			curSrcTk = pi_prod;
		}

		curSnkTk = pi_cons;

		node srcConnections[MAX_VERTEX_REPETITION];
		node snkConnections[MAX_VERTEX_REPETITION];

		for(j=0; j<MAX_VERTEX_REPETITION; j++){
			srcConnections[j].nb = 0;
			snkConnections[j].nb = 0;
		}

		// Iterating until all src or all snk are "satisfied".
		while (srcIx < nbSrcs && snkIx < nbSnks) {
			// Production/consumption rate for the current source/target.
			int curRate = std::min(curSrcTk, curSnkTk);

			/* Init src if first connection */
			if(srcConnections[srcIx].nb == 0){
				if(srcIx == 0 && delay){
					srcConnections[srcIx].type = VERTEX;
					srcConnections[srcIx].vertex.ptr = topSrdag->addInit();
					srcConnections[srcIx].vertex.portIx = 0;
				}else{
					switch(pi_src->getType()){
					case PISDF_TYPE_BODY:
						srcConnections[srcIx].type = VERTEX;
						if(delay)
							srcConnections[srcIx].vertex.ptr = job->bodies[pi_src->getTypeId()][srcIx-1];
						else
							srcConnections[srcIx].vertex.ptr = job->bodies[pi_src->getTypeId()][srcIx];
						srcConnections[srcIx].vertex.portIx = pi_edge->getSrcPortIx();
						break;
					case PISDF_TYPE_CONFIG:
						srcConnections[srcIx].type = EDGE;
						srcConnections[srcIx].edge = job->configs[pi_src->getTypeId()]->getOutEdge(pi_edge->getSrcPortIx());
						break;
					case PISDF_TYPE_IF:
						srcConnections[srcIx].type = EDGE;
						srcConnections[srcIx].edge = job->inputIfs[pi_src->getTypeId()];
						break;
					}
				}
			}
			srcConnections[srcIx].rates[srcConnections[srcIx].nb++] = curRate;

			/* Init snk if first connection */
			if(snkConnections[snkIx].nb == 0){
				if(snkIx == nbSnks-1 && delay){
					snkConnections[snkIx].type = VERTEX;
					snkConnections[snkIx].vertex.ptr = 0;//SRDAGGraph_AddEnd(topDag, 0, 0);
					snkConnections[snkIx].vertex.portIx = -1;//0;
				}else{
					switch(pi_snk->getType()){
					case PISDF_TYPE_BODY:
						snkConnections[snkIx].type = VERTEX;
						snkConnections[snkIx].vertex.ptr = job->bodies[pi_snk->getTypeId()][snkIx];
						snkConnections[snkIx].vertex.portIx = pi_edge->getSnkPortIx();
						break;
					case PISDF_TYPE_CONFIG:
						throw "Unexpected case in LinkVertices\n";
						break;
					case PISDF_TYPE_IF:
						snkConnections[snkIx].type = EDGE;
						snkConnections[snkIx].edge = job->outputIfs[pi_snk->getTypeId()];
						break;
					}
				}
			}
			snkConnections[snkIx].rates[snkConnections[snkIx].nb++] = curRate;

			// Update the number of token produced/consumed by the current source/target.
			curSrcTk -= curRate;
			curSnkTk -= curRate;

			if(curSrcTk == 0){
				/* Do not increase index for interface or configs
				 * as their are executed only once.
				 */
				if(pi_src->getType() == PISDF_TYPE_BODY)
					srcIx++;
				curSrcTk += pi_prod;
			}

			if(curSnkTk == 0){
				if(pi_snk->getType() == PISDF_TYPE_BODY)
					snkIx++;
				if(snkIx == nbSnks-1 && delay != 0)
					curSnkTk += delay;
				else
					curSnkTk += pi_cons;
			}
		}
//		printf("edge %d:\n\tSource:", pi_edge->getId());
//		for(j=0; j<nbSrcs; j++){
//			int k;
//			printf(" { ");
//			for(k=0; k<srcConnections[j].nb; k++){
//				printf("%d(%c) ",
//						srcConnections[j].rates[k],
//						(srcConnections[j].type == EDGE)?'E':'V');
//			}
//			printf("}");
//		}
//		printf("\n\tSink:  ");
//		for(j=0; j<nbSnks; j++){
//			int k;
//			printf(" { ");
//			for(k=0; k<snkConnections[j].nb; k++){
//				printf("%d(%c) ",
//						snkConnections[j].rates[k],
//						(snkConnections[j].type == EDGE)?'E':'V');
//			}
//			printf("}");
//		}
//		printf("\n");

		/* Transform all src nodes to src edges.
		 * Basicaly create edges and maybe Im/Ex/Rb/Br
		 */
		for(j=0; j<nbSrcs; j++){
			node* srcNode = &(srcConnections[j]);
			switch(srcNode->type){
			case EDGE: /* Interfaces and Config */
				if(srcNode->nb == 1){
					if(srcNode->rates[0] > srcNode->edge->getRate())/* RB needed */{
						SRDAGVertex *rb = topSrdag->addRoundBuffer();
						SRDAGEdge *rb_edge = topSrdag->addEdge();
						srcNode->edge->connectSnk(rb, 0);
						rb_edge->connectSrc(rb, 0);
						rb_edge->setRate(srcNode->rates[0]);
						srcEdges[nbSrcEdges++] = rb_edge;
					}else
						srcEdges[nbSrcEdges++] = srcNode->edge;
				}else{
					int k;
					/* Todo complicate this (handle BR, RB?, ...)*/
					/* Check if can be replace with BR */
					bool isBroadcast = true;
					int sum = 0;
					for(k = 0; k<srcNode->nb; k++){
						isBroadcast = srcNode->rates[k] == srcNode->edge->getRate();
						sum += srcNode->rates[k];
					}
					bool needRB = sum > srcNode->edge->getRate();

					if(isBroadcast){
						SRDAGVertex *br = topSrdag->addBroadcast(srcNode->nb);
						srcNode->edge->connectSnk(br, 0);
						for(k=0; k<srcNode->nb; k++){
							SRDAGEdge* edge = topSrdag->addEdge();
							edge->connectSrc(br, k);
							edge->setRate(srcNode->rates[k]);
							srcEdges[nbSrcEdges++] = edge;
						}
						break;
					}

					if(needRB){
						SRDAGVertex *rb = topSrdag->addRoundBuffer();
						SRDAGVertex *fork = topSrdag->addFork(srcNode->nb);
						SRDAGEdge *rb_edge = topSrdag->addEdge();
						srcNode->edge->connectSnk(rb, 0);
						rb_edge->connectSrc(rb, 0);
						rb_edge->connectSnk(fork, 0);
						rb_edge->setRate(0);
						for(k=0; k<srcNode->nb; k++){
							SRDAGEdge* edge = topSrdag->addEdge();
							edge->connectSrc(fork, k);
							edge->setRate(srcNode->rates[k]);
							rb_edge->setRate(rb_edge->getRate()+edge->getRate());
							srcEdges[nbSrcEdges++] = edge;
						}
					}else{
						SRDAGVertex *fork = topSrdag->addFork(srcNode->nb);
						srcNode->edge->connectSnk(fork, 0);
						for(k=0; k<srcNode->nb; k++){
							SRDAGEdge* edge = topSrdag->addEdge();
							edge->connectSrc(fork, k);
							edge->setRate(srcNode->rates[k]);
							srcEdges[nbSrcEdges++] = edge;
						}
					}
					break;
				}
				break;
			case VERTEX: /* Normal vertices */
				if(srcNode->nb == 1){
					SRDAGEdge* edge = topSrdag->addEdge();
					edge->connectSrc(srcNode->vertex.ptr, srcNode->vertex.portIx);
					edge->setRate(srcNode->rates[0]);
					srcEdges[nbSrcEdges++] = edge;
				}else{
					int k;
					/* Todo complicate this */
					SRDAGVertex *fork = topSrdag->addFork(srcNode->nb);
					SRDAGEdge* edge_fork = topSrdag->addEdge();
					edge_fork->connectSrc(srcNode->vertex.ptr, srcNode->vertex.portIx);
					edge_fork->connectSnk(fork, 0);
					edge_fork->setRate(0);
					for(k=0; k<srcNode->nb; k++){
						SRDAGEdge* edge = topSrdag->addEdge();
						edge->connectSrc(fork, k);
						edge->setRate(srcNode->rates[k]);
						edge_fork->setRate(edge_fork->getRate() + edge->getRate());
						srcEdges[nbSrcEdges++] = edge;
					}
				}
				break;
			}
		}

		/* Transform all snk edges to snk nodes.
		 * This may lead to delete some edges,
		 * but not critical as they are not allocated yet
		 * as they are snk edges of a hierarchical graph.
		 */
		for(j=0; j<nbSnks; j++){
			node* snkNode = &(snkConnections[j]);
			switch(snkNode->type){
			case EDGE: /* Only Interface */
				if(snkNode->nb == 1){
					/* Direct link */
					snkVertices[nbSnkVertices].vertex = snkNode->edge->getSnk();
					snkVertices[nbSnkVertices].port = snkNode->edge->getSnkPortIx();
					snkNode->edge->disconnectSnk();
					topSrdag->delEdge(snkNode->edge);
					nbSnkVertices++;
				}else{
					int outCons = snkNode->edge->getRate();
					int outProd = 0;
					for(int k = 0; k<snkNode->nb; k++){
						outProd += snkNode->rates[k];
					}

					/* Remove some edges as they are not needed (or put End) */
					if(outCons < outProd){
						// To musch data keep last (only RB for now)
						SRDAGVertex *rb = topSrdag->addRoundBuffer();
						SRDAGVertex *join = topSrdag->addJoin(snkNode->nb);
						SRDAGEdge* rb_edge = topSrdag->addEdge();
						rb_edge->connectSnk(rb, 0);
						rb_edge->connectSrc(join, 0);
						rb_edge->setRate(outProd);
						snkNode->edge->connectSrc(rb, 0);
						for(int start=0; start<snkNode->nb; start++){
							snkVertices[nbSnkVertices].vertex = join;
							snkVertices[nbSnkVertices].port = start;
							nbSnkVertices++;
						}
//						throw "Unhandled case in Snk Resolution\n"; // todo
					}else if(outCons == outProd){
						/* Normal join scheme */
						SRDAGVertex *join = topSrdag->addJoin(snkNode->nb);
						snkNode->edge->connectSrc(join, 0);
						for(int start=0; start<snkNode->nb; start++){
							snkVertices[nbSnkVertices].vertex = join;
							snkVertices[nbSnkVertices].port = start;
							nbSnkVertices++;
						}
					}else{
						throw "Unexpected case in Snk Resolution\n";
					}
				}
				break;
			case VERTEX: /* Normal Vertices */
				if(snkNode->nb == 1){
					snkVertices[nbSnkVertices].vertex = snkNode->vertex.ptr;
					snkVertices[nbSnkVertices].port = snkNode->vertex.portIx;
					nbSnkVertices++;
				}else{
					int k;
					/* Todo complicate this */
					SRDAGVertex *join = topSrdag->addJoin(snkNode->nb);
					SRDAGEdge* edge_join = topSrdag->addEdge();
					edge_join->connectSrc(join, 0);
					edge_join->connectSnk(snkNode->vertex.ptr, snkNode->vertex.portIx);
					edge_join->setRate(0);
					for(k=0; k<snkNode->nb; k++){
						edge_join->setRate(edge_join->getRate() + snkNode->rates[k]);
						snkVertices[nbSnkVertices].vertex = join;
						snkVertices[nbSnkVertices].port = k;
						nbSnkVertices++;
					}
				}
				break;
			}
		}

		/* Connect src edges to snk vertices */
		if(nbSrcEdges != nbSnkVertices){
			throw "Nb src and snk connections mismatch\n";
		}

		for(j=0; j<nbSrcEdges; j++){
			if(snkVertices[j].vertex == 0){
				if(srcEdges[j]->getSrc()->getType() == SRDAG_BROADCAST){
					int k;
					SRDAGVertex *br = srcEdges[j]->getSrc();

					/* Remove edge of Broadcast */
					int portIx = srcEdges[j]->getSrcPortIx();
					srcEdges[j]->disconnectSrc();
					topSrdag->delEdge(srcEdges[j]);
					for(k=portIx; k<br->getNOutEdge()-1; k++){
						SRDAGEdge *edge = br->getOutEdge(k+1);
						edge->disconnectSrc();
						edge->connectSrc(br, k);
					}
					/* No link needed */
					continue;
				}else{
					snkVertices[j].vertex = topSrdag->addEnd();
					snkVertices[j].port = 0;
				}
			}
			srcEdges[j]->connectSnk(snkVertices[j].vertex, snkVertices[j].port);
		}
	}

	/* Optimizations */
	/* Remove Unused broadcasts */
	SRDAGVertexIterator vertexIt = topSrdag->getVertexIterator();
	FOR_IT(vertexIt){
		SRDAGVertex *vertex = vertexIt.current();
		if(vertex->getType() == SRDAG_BROADCAST && vertex->getNConnectedOutEdge() == 1){
			/* Remove Broadcast */
			/* TODO check if kill BR can cause troubles for other edges */
			SRDAGEdge *edge_in  = vertex->getInEdge(0);
			SRDAGEdge *edge_out = 0;
			for(int i=0; i<vertex->getNOutEdge(); i++){
				if(vertex->getOutEdge(i) != 0){
					edge_out = vertex->getOutEdge(i);
				}
			}
			SRDAGVertex *out = edge_out->getSnk();
			int outPrt = edge_out->getSnkPortIx();

			edge_out->disconnectSnk();
			edge_in->disconnectSnk();
			edge_in->connectSnk(out, outPrt);

			edge_out->disconnectSrc();
			topSrdag->delEdge(edge_out);
			topSrdag->delVertex(vertex);

			vertexIt.first(); // minus 1 can be enough
		}
	}
}
