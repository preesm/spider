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
		if(join0->getType() == SRDAG_JOIN){
			SRDAGVertex* join1 = join0->getOutEdge(0)->getSnk();
			if(join1->getType() == SRDAG_JOIN){
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
		if(secFork->getType() == SRDAG_FORK){
			SRDAGVertex* firFork = secFork->getInEdge(0)->getSrc();
			if(firFork->getType() == SRDAG_FORK){
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
		if(br->getType() == SRDAG_BROADCAST){
			for(int j=0; j<br->getNConnectedOutEdge(); j++){
				SRDAGEdge* delEdge = br->getOutEdge(j);
				SRDAGVertex* endVertex = delEdge->getSnk();
				if(endVertex->getType() == SRDAG_END){
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

			if(br->getNConnectedOutEdge() == 1){
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

void optims(SRDAGGraph *topDag, Stack* stack){
	while(reduceJoinJoin(topDag));
	while(reduceForkFork(topDag));
	while(removeBr(topDag));
}

