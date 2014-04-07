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

#include "SRDAGGraph.h"
#include "SRDAGVertex.h"
#include "../PiSDF/PiSDFIfVertex.h"
#include "SRDAGEdge.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "debuggingOptions.h"
#include <tools/DotWriter.h>

/**
 Constructor
*/
SRDAGGraph::SRDAGGraph()
{
	// There is no dynamic allocation of graph members
	vertices.reset();
	edges.reset();
	hierVertex.reset();
}

void SRDAGGraph::reset(){
	vertices.reset();
	edges.reset();
	hierVertex.reset();
}

/**
 Destructor
*/
SRDAGGraph::~SRDAGGraph()
{
}

/**
 Adding an edge to the graph
 
 @param source: The source vertex of the edge
 @param production: number of tokens (chars) produced by the source
 @param sink: The sink vertex of the edge
 @param consumption: number of tokens (chars) consumed by the sink
 @return the created edge
*/
SRDAGEdge* SRDAGGraph::addEdge(SRDAGVertex* source, UINT32 sourcePortIx, int tokenRate, SRDAGVertex* sink, UINT32 sinkPortIx, PiSDFEdge* refEdge){
	SRDAGEdge* edge;
	edge = edges.add();
	edge->reset();
	edge->setSource(source);
	edge->setTokenRate(tokenRate);
	edge->setSink(sink);
	edge->setRefEdge(refEdge);
	source->setOutputEdge(edge, sourcePortIx);
	sink->setInputEdge(edge, sinkPortIx);
	return edge;
}

/**
 Removes the last added edge
*/
void SRDAGGraph::removeLastEdge(){
	if(edges.getNb() > 0){
		edges.remove(edges.getNb()-1);
	}
	else{
		// Removing an edge from an empty graph
		exitWithCode(1007);
	}
}

/**
 Removes all edges and vertices
*/
void SRDAGGraph::flush(){
	vertices.reset();
	edges.reset();
	SRDAGEdge::firstInSinkOrder = (SRDAGEdge*) NULL;
}

int SRDAGGraph::getMaxTime(){
//	int sum=0;
//	for(int i=0; i< vertices.getNb(); i++){
//		sum += vertices[i].getCsDagReference()->getIntTiming(0);
//	}
//	return sum;
	return 0;
}


static void iterateCriticalPath(SRDAGVertex* curVertex, int curLenght, int* max){
//	curLenght += curVertex->getCsDagReference()->getIntTiming(0);
//	if(curVertex->getNbOutputEdge() == 0){
//		if(curLenght > *max) *max = curLenght;
//		return;
//	}
//	for(int i=0; i<curVertex->getNbOutputEdge(); i++){
//		iterateCriticalPath(curVertex->getOutputEdge(i)->getSink(), curLenght, max);
//	}
}

int SRDAGGraph::getCriticalPath(){
	int max;
	iterateCriticalPath(this->getVertex(0), 0, &max);
	return max;
}

SRDAGEdge* SRDAGGraph::getEdgeByRef(SRDAGVertex* hSrDagVx, PiSDFEdge* refEdge, VERTEX_TYPE inOut){
	if(inOut == input_vertex){
		for (UINT32 i = 0; i < hSrDagVx->getNbInputEdge(); i++) {
			SRDAGEdge* inSrDagEdge = hSrDagVx->getInputEdge(i);
			if(inSrDagEdge->getRefEdge() == refEdge) return inSrDagEdge;
		}
	}
	else if(inOut == output_vertex){
		for (UINT32 i = 0; i < hSrDagVx->getNbOutputEdge(); i++) {
			SRDAGEdge* outSrDagEdge = hSrDagVx->getOutputEdge(i);
			if(outSrDagEdge->getRefEdge() == refEdge) return outSrDagEdge;
		}
	}
	return 0;
}

void SRDAGGraph::removeVx(SRDAGVertex* Vx){
	Vx->setState(SrVxStDeleted);
}

