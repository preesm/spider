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

#include "Optims.h"
#include "GraphTransfo.h"

#include <graphs/PiSDF/PiSDFGraph.h>
#include <graphs/PiSDF/PiSDFVertex.h>
#include <graphs/PiSDF/PiSDFEdge.h>

#include <graphs/SRDAG/SRDAGGraph.h>
#include <graphs/SRDAG/SRDAGVertex.h>
#include <graphs/SRDAG/SRDAGEdge.h>

static int reduceJoinJoin(SRDAGGraph* topDag){
	for(int i=0; i<topDag->getNVertex(); i++){
		SRDAGVertex* join0 = topDag->getVertex(i);
		if(join0 && join0->getState() != SRDAG_RUN && join0->getType() == SRDAG_JOIN){
			SRDAGVertex* join1 = join0->getOutEdge(0)->getSnk();
			if(join1 && join1->getType() == SRDAG_JOIN){
				int nbToAdd = join0->getNConnectedInEdge();
				int ixToAdd = join0->getOutEdge(0)->getSnkPortIx();

				int nbTotEdges = join1->getNConnectedInEdge()+nbToAdd-1;

				// Shift edges after
				for(int k=nbTotEdges-1; k>ixToAdd+nbToAdd-1; k--){
					SRDAGEdge* edge = join1->getInEdge(k-nbToAdd+1);
					edge->disconnectSnk();
					edge->connectSnk(join1, k);
				}

				SRDAGEdge* delEdge = join1->getInEdge(ixToAdd);
				delEdge->disconnectSnk();
				topDag->delEdge(delEdge);

				// Add edges
				for(int k=0; k<nbToAdd; k++){
					SRDAGEdge* edge = join0->getInEdge(k);
					edge->disconnectSnk();
					edge->connectSnk(join1, k+ixToAdd);
				}

				topDag->delVertex(join0);
				return 1;
			}
		}
	}
	return 0;
}

static int reduceForkFork(SRDAGGraph* topDag){
	for(int i=0; i<topDag->getNVertex(); i++){
		SRDAGVertex* secFork = topDag->getVertex(i);
		if(secFork && secFork->getType() == SRDAG_FORK){
			SRDAGVertex* firFork = secFork->getInEdge(0)->getSrc();
			if(firFork && firFork->getState() != SRDAG_RUN && firFork->getType() == SRDAG_FORK){
				int nbToAdd = secFork->getNConnectedOutEdge();
				int ixToAdd = secFork->getInEdge(0)->getSrcPortIx();

				int nbTotEdges = firFork->getNConnectedOutEdge()+nbToAdd-1;

				// Shift edges after
				for(int k=nbTotEdges-1; k>ixToAdd+nbToAdd-1; k--){
					SRDAGEdge* edge = firFork->getOutEdge(k-nbToAdd+1);
					edge->disconnectSrc();
					edge->connectSrc(firFork, k);
				}

				SRDAGEdge* delEdge = firFork->getOutEdge(ixToAdd);
				delEdge->disconnectSrc();
				topDag->delEdge(delEdge);

				// Add edges
				for(int k=0; k<nbToAdd; k++){
					SRDAGEdge* edge = secFork->getOutEdge(k);
					edge->disconnectSrc();
					edge->connectSrc(firFork, k+ixToAdd);
				}

				topDag->delVertex(secFork);
				return 1;
			}
		}
	}
	return 0;
}

static int removeBr(SRDAGGraph* topDag){
	bool result = false;
	for(int i=0; i<topDag->getNVertex(); i++){
		SRDAGVertex* br = topDag->getVertex(i);
		if(br && br->getState() != SRDAG_RUN && br->getType() == SRDAG_BROADCAST){
			for(int j=0; j<br->getNConnectedOutEdge(); j++){
				SRDAGEdge* delEdge = br->getOutEdge(j);
				SRDAGVertex* endVertex = delEdge->getSnk();
				if(endVertex && endVertex->getType() == SRDAG_END){
					int nbOutput = br->getNConnectedOutEdge();

					delEdge->disconnectSrc();
					delEdge->disconnectSnk();
					topDag->delEdge(delEdge);
					topDag->delVertex(endVertex);

					for(int k=j+1; k<nbOutput; k++){
						SRDAGEdge *edge = br->getOutEdge(k);
						edge->disconnectSrc();
						edge->connectSrc(br, k-1);
					}

					result = true;
				}
			}

			if(br->getNConnectedOutEdge() == 1 && br->getOutEdge(0)->getSnk()){
				SRDAGEdge* inEdge = br->getInEdge(0);
				SRDAGVertex* nextVertex = br->getOutEdge(0)->getSnk();
				int edgeIx = br->getOutEdge(0)->getSnkPortIx();

				SRDAGEdge* delEdge = br->getOutEdge(0);
				delEdge->disconnectSrc();
				delEdge->disconnectSnk();
				topDag->delEdge(delEdge);

				inEdge->disconnectSnk();
				inEdge->connectSnk(nextVertex, edgeIx);
				topDag->delVertex(br);

				return true;
			}else if(br->getNConnectedOutEdge() == 0){
				SRDAGVertex* end = topDag->addEnd();
				SRDAGEdge* edge = br->getInEdge(0);

				edge->disconnectSnk();
				edge->connectSnk(end, 0);

				topDag->delVertex(br);
				return true;
			}

			if(result)
				return true;
		}
	}
	return result;
}

static int removeFork(SRDAGGraph* topDag){
	bool result = false;
	for(int i=0; i<topDag->getNVertex(); i++){
		SRDAGVertex* fork = topDag->getVertex(i);
		if(fork && fork->getState() != SRDAG_RUN && fork->getType() == SRDAG_FORK){
			for(int j=0; j<fork->getNConnectedOutEdge(); j++){
				SRDAGEdge* delEdge = fork->getOutEdge(j);
				SRDAGVertex* nextVertex = delEdge->getSnk();
				if(nextVertex && delEdge->getRate() == 0){
					switch (nextVertex->getType()) {
						case SRDAG_END:{
							int nbOutput = fork->getNConnectedOutEdge();
							delEdge->disconnectSrc();
							delEdge->disconnectSnk();
							topDag->delEdge(delEdge);
							topDag->delVertex(nextVertex);

							for(int k=j+1; k<nbOutput; k++){
								SRDAGEdge *edge = fork->getOutEdge(k);
								edge->disconnectSrc();
								edge->connectSrc(fork, k-1);
							}
							result = true;
							break;}
						case SRDAG_JOIN:{
							int forkNOut = fork->getNConnectedOutEdge();
							int joinPortIx = delEdge->getSnkPortIx();
							int joinNIn = nextVertex->getNConnectedInEdge();
							delEdge->disconnectSrc();
							delEdge->disconnectSnk();
							topDag->delEdge(delEdge);

							/* Shift on Fork */
							for(int k=j+1; k<forkNOut; k++){
								SRDAGEdge *edge = fork->getOutEdge(k);
								edge->disconnectSrc();
								edge->connectSrc(fork, k-1);
							}

							/* Shift on Join */
							for(int k=joinPortIx+1; k<joinNIn; k++){
								SRDAGEdge *edge = nextVertex->getInEdge(k);
								edge->disconnectSnk();
								edge->connectSnk(nextVertex, k-1);
							}

							break;}
					}
				}
			}

			if(fork->getNConnectedOutEdge() == 1 && fork->getOutEdge(0)->getSnk()){
				SRDAGEdge* inEdge = fork->getInEdge(0);
				SRDAGVertex* nextVertex = fork->getOutEdge(0)->getSnk();
				int edgeIx = fork->getOutEdge(0)->getSnkPortIx();

				SRDAGEdge* delEdge = fork->getOutEdge(0);
				delEdge->disconnectSrc();
				delEdge->disconnectSnk();
				topDag->delEdge(delEdge);

				inEdge->disconnectSnk();
				inEdge->connectSnk(nextVertex, edgeIx);
				topDag->delVertex(fork);

				return true;
			}else if(fork->getNConnectedOutEdge() == 0){
				SRDAGVertex* end = topDag->addEnd();
				SRDAGEdge* edge = fork->getInEdge(0);

				edge->disconnectSnk();
				edge->connectSnk(end, 0);

				topDag->delVertex(fork);
				return true;
			}

			if(result)
				return true;
		}
	}
	return result;
}

static int removeJoin(SRDAGGraph* topDag){
	bool result = false;
	for(int i=0; i<topDag->getNVertex(); i++){
		SRDAGVertex* join = topDag->getVertex(i);
		if(join && join->getState() != SRDAG_RUN && join->getType() == SRDAG_JOIN){
			if(join->getNConnectedOutEdge() == 1 && join->getOutEdge(0)->getSnk()){
				SRDAGEdge* inEdge = join->getInEdge(0);
				SRDAGVertex* nextVertex = join->getOutEdge(0)->getSnk();
				int edgeIx = join->getOutEdge(0)->getSnkPortIx();

				SRDAGEdge* delEdge = join->getOutEdge(0);
				delEdge->disconnectSrc();
				delEdge->disconnectSnk();
				topDag->delEdge(delEdge);

				inEdge->disconnectSnk();
				inEdge->connectSnk(nextVertex, edgeIx);
				topDag->delVertex(join);

				return true;
			}else if(join->getNConnectedOutEdge() == 0){
				SRDAGVertex* end = topDag->addEnd();
				SRDAGEdge* edge = join->getInEdge(0);

				edge->disconnectSnk();
				edge->connectSnk(end, 0);

				topDag->delVertex(join);
				return true;
			}

			if(result)
				return true;
		}
	}
	return result;
}

void optims(SRDAGGraph *topDag, Stack* stack){
	bool res;
	topDag->print("before.gv");
	do{
		res = false;
		res = res || reduceJoinJoin(topDag);
		res = res || reduceForkFork(topDag);
		res = res || removeBr(topDag);
		res = res || removeFork(topDag);
		res = res || removeJoin(topDag);
	}while(res);
	topDag->print("after.gv");
}

