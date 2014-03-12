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
#include "../CSDAG/CSDAGVertex.h"
#include "../PiSDF/PiSDFIfVertex.h"
#include "SRDAGEdge.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "debuggingOptions.h"
#include <tools/DotWriter.h>

extern DotWriter dotWriter;
static char file[MAX_FILE_NAME_SIZE];

/**
 Constructor
*/
SRDAGGraph::SRDAGGraph()
{
	// There is no dynamic allocation of graph members
	nbVertices = 0;
//	memset(vertices,'\0',MAX_SRDAG_VERTICES*sizeof(SRDAGVertex));
	nbEdges = 0;
//	memset(edges,'\0',MAX_SRDAG_EDGES*sizeof(SRDAGEdge));
}

void SRDAGGraph::reset(){
	nbVertices = 0;
	nbEdges = 0;
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
SRDAGEdge* SRDAGGraph::addEdge(SRDAGVertex* source, int tokenRate, SRDAGVertex* sink, BaseEdge* refEdge){
	SRDAGEdge* edge;
#ifdef _DEBUG
	if(nbEdges >= MAX_SRDAG_EDGES){
		// Adding an edge while the graph is already full
		exitWithCode(1001);
	}
#endif
	edge = &edges[nbEdges];
	edge->reset();
	edge->setSource(source);
	edge->setTokenRate(tokenRate);
	edge->setSink(sink);
	edge->setRefEdge(refEdge);
	source->addOutputEdge(edge);
	sink->addInputEdge(edge);
	nbEdges++;
	return edge;
}


void SRDAGGraph::appendAnnex(SRDAGGraph* annex){
	// Adding vxs
	for (int i = 0; i < annex->getNbVertices(); i++) {
		SRDAGVertex* annexVx = annex->getVertex(i);

		SRDAGVertex* vx = addVertex();
		vx->setName(annexVx->getName());
		vx->setFunctIx(annexVx->getFunctIx());
		vx->setReference(annexVx->getReference());
		vx->setReferenceIndex(annexVx->getReferenceIndex());
		vx->setState(annexVx->getState());
		vx->setType(annexVx->getType());
		vx->setExpImpId(annexVx->getExpImpId());
		vx->setParent(annexVx->getParent());

		// Storing the index in the merged graph.
		annexVx->setMergeIx(vx->getId());
	}

	// Adding edges.
	for (int i = 0; i < annex->getNbEdges(); i++) {
		SRDAGEdge* edge = annex->getEdge(i);
		SRDAGVertex* source;
		SRDAGVertex* sink;
		int srcMergeIx = edge->getSource()->getMergeIx();
		int snkMergeIx = edge->getSink()->getMergeIx();
		if(srcMergeIx != -1)
			source = getVertex(srcMergeIx); // The source has already be added.
		else
		{
//			// Adding a new source vx.
//			source = addVertex();
//			source->setName(edge->getSource()->getName());
//			source->setFunctIx(edge->getSource()->getFunctIx());
//			source->setReference(edge->getSource()->getReference());
//			source->setReferenceIndex(edge->getSource()->getReferenceIndex());
//			source->setState(edge->getSource()->getState());
//			source->setType(edge->getSource()->getType());
//			source->setExpImpId(edge->getSource()->getExpImpId());
//			source->setParent(edge->getSource()->getParent());
//
//			edge->getSource()->setMergeIx(source->getId());
		}

		if(snkMergeIx != -1)
			sink = getVertex(snkMergeIx);
		else
		{
//			sink = addVertex();
//			sink->setName(edge->getSink()->getName());
//			sink->setFunctIx(edge->getSink()->getFunctIx());
//			sink->setReference(edge->getSink()->getReference());
//			sink->setReferenceIndex(edge->getSink()->getReferenceIndex());
//			sink->setState(edge->getSink()->getState());
//			sink->setType(edge->getSink()->getType());
//			sink->setExpImpId(edge->getSink()->getExpImpId());
//			sink->setParent(edge->getSink()->getParent());
//
//			edge->getSink()->setMergeIx(sink->getId());
		}

		addEdge(source, edge->getTokenRate(), sink, edge->getRefEdge());
//		newEdge->setFifoId(edge->getFifoId());
	}
}


/**
 Removes the last added edge
*/
void SRDAGGraph::removeLastEdge(){
	if(nbEdges > 0){
		nbEdges--;
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
	nbVertices = nbEdges = 0;
	SRDAGEdge::firstInSinkOrder = (SRDAGEdge*) NULL;
}

#if 0
/**
 Pivot function for Quick Sort
*/
int partition(SRDAGEdge* edgePointers, int p, int r, char sourceOrSink) {
  SRDAGEdge* x = &edgePointers[r];
  static SRDAGEdge temp;
  int j = p - 1;
  for (int i = p; i < r; i++) {
	  if (((sourceOrSink != 0) && (x->source >= edgePointers[i].source)) ||
		  ((sourceOrSink == 0) && (x->sink >= edgePointers[i].sink))) {
      j = j + 1;
      memcpy(&temp,&edgePointers[j],sizeof(SRDAGEdge));
      memcpy(&edgePointers[j],&edgePointers[i],sizeof(SRDAGEdge));
      memcpy(&edgePointers[i],&temp,sizeof(SRDAGEdge));
    }
  }

  memcpy(&edgePointers[r],&edgePointers[j + 1],sizeof(SRDAGEdge));
  memcpy(&edgePointers[j + 1],x,sizeof(SRDAGEdge));

  return (j + 1);
}

/**
 quick Sort recursive algorithm 
*/
void quickSort(SRDAGEdge* edgePointers, int p, int r, char sourceOrSink) {
  if (p < r) {
    int q = partition(edgePointers, p, r, sourceOrSink);
    quickSort(edgePointers, p, q - 1, sourceOrSink);
    quickSort(edgePointers, q + 1, r, sourceOrSink);
  }
}

/**
 quick Sort algorithm used for sorting edges pointers from their source or sink address
*/
void quickSort(SRDAGEdge* edgePointers, int length, char sourceOrSink) {
  quickSort(edgePointers, 0, length-1, sourceOrSink);
}
#endif


int SRDAGGraph::getMaxTime(){
	int sum=0;
	for(int i=0; i< nbVertices; i++){
		sum += vertices[i].getCsDagReference()->getIntTiming(0);
	}
	return sum;
}


static void iterateCriticalPath(SRDAGVertex* curVertex, int curLenght, int* max){
	curLenght += curVertex->getCsDagReference()->getIntTiming(0);
	if(curVertex->getNbOutputEdge() == 0){
		if(curLenght > *max) *max = curLenght;
		return;
	}
	for(int i=0; i<curVertex->getNbOutputEdge(); i++){
		iterateCriticalPath(curVertex->getOutputEdge(i)->getSink(), curLenght, max);
	}
}

int SRDAGGraph::getCriticalPath(){
	int max;
	iterateCriticalPath(this->getVertex(0), 0, &max);
	return max;
}

SRDAGEdge* SRDAGGraph::getEdgeByRef(SRDAGVertex* hSrDagVx, BaseEdge* refEdge, VERTEX_TYPE inOut){
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




/*
 * Merges one DAG into the current DAG object.
 * -"annex" is the DAG that will be annexed into the current DAG object.
 * -"intralevel" indicates the type of merging.
 *
 * There are two types of merging :
 * 	1. When "intralevel" is true, it indicates that both DAGs correspond to the same hierarchical level.
 * 	This is the case of one sub-graph which contains configure vertices, and requires
 * 	to be executed in two times. At the first time, all the configure vertices and its
 * 	predecessors are executed. At the second time, before the rest of the sub-graph can be executed,
 * 	an INTRA-level merging must be done. Like this, the successor vertices are plugged to the output edges
 * 	of the executed configure vertices.
 *
 * 	2. When "intralevel" is false, it indicates that the DAGs correspond to different hierarchical levels.
 * 	This is the case of the first execution of a sub-graph with configure vertices, or a sub-graph with
 * 	no configure vertex at all. In both cases an INTER-level merging is done. I.e. the "annex" sub-graph
 * 	replaces its parent vertex in the current DAG object.
 *
 * To do the merging, it adds all vertices and edges from "annex" into the same global DAG.
 * (let's call the vertices from the global DAG the left side, and those of the "annex", the right side).
 * For both types of merging, the left side connectors are round buffer or input vertices without output edges.
 * The right side connectors are round buffer or input vertices without input edges.
 *
 * INTER-level:
 */
void SRDAGGraph::merge(SRDAGGraph* annex, bool intraLevel, UINT32 level, UINT8 step){
#if PRINT_GRAPH
//	// Printing the global dag before merging.
//	UINT32 len = snprintf(file, MAX_FILE_NAME_SIZE, "%s_%d_%d.gv", PRE_SRDAG_FILE_NAME, level, step);
//	if(len > MAX_FILE_NAME_SIZE){
//		exitWithCode(1072);
//	}
//	dotWriter.write(this, file, 1, 1);
//
//	// Printing the annexing dag.
//	len = snprintf(file, MAX_FILE_NAME_SIZE, "%s_%d_%d.gv", SUB_SRDAG_FILE_NAME, level, step);
//	if(len > MAX_FILE_NAME_SIZE){
//		exitWithCode(1072);
//	}
//	dotWriter.write(annex, file, 1, 1);
#endif

	// Adding all (annexing vertices and edges) to the global DAG.
	appendAnnex(annex);

	// Doing connections.
	if(intraLevel){
		// Finding an unplugged vx to the left (RoundBuffer/Input vertex).
		SRDAGVertex* leftVx;
		leftVx = findUnplug();
		while(leftVx){
			// Finding matching unplugged vertex to the right.
			SRDAGVertex* rightVx;
			if (!(rightVx = findMatch(leftVx->getReference()))) exitWithCode(1064);
			// Connecting them.
//			addEdge(leftVx, leftVx->getInputEdge(0)->getTokenRate(), rightVx, leftVx->getInputEdge(0)->getRefEdge());
//			leftVx->getInputEdge(0)->setSink(rightVx);
			rightVx->getOutputEdge(0)->setSource(leftVx);
			leftVx->addOutputEdge(rightVx->getOutputEdge(0));
			// Deleting right Vx.
			rightVx->setState(SrVxStDeleted);

			leftVx = findUnplug();
		}

		// Connecting output interface(s) in the annex.
		SRDAGVertex* outputVx;
		outputVx = findUnplugIF(output_vertex);
		while(outputVx){
			// Getting the output edge from the higher level in the PiSDF.
			BaseEdge* refEdge = ((PiSDFIfVertex*)outputVx->getReference())->getParentEdge();
			// Getting the corresponding edge in the global DAG.
			SRDAGEdge* outSrDagEdge = getEdgeByRef(outputVx->getParent(), refEdge, output_vertex);
			// Changing the source of the DAG edge to the output of the lower level.
			outSrDagEdge->setSource(outputVx);
			outputVx->addOutputEdge(outSrDagEdge);
//			if(outputVx->getNbOutputEdge()>0) outputVx->setState(SrVxStExecutable);

			outputVx = findUnplugIF(output_vertex);
		}
	}
	else{
		// INTER-level merging.

		// Finding an unplugged input connector vertex in the right side.
		SRDAGVertex* inputVx;
		inputVx = findUnplugIF(input_vertex);

		while(inputVx){
			// Getting the input edge from the parent vertex in the PiSDF.
			BaseEdge* refEdge = ((PiSDFIfVertex*)inputVx->getReference())->getParentEdge();
			// Getting the associated edge in the global DAG.
			SRDAGEdge* inSrDagEdge = getEdgeByRef(inputVx->getParent(), refEdge, input_vertex);
			// Replacing the parent vertex by its descendant vertex as sink of the input DAG edge.
			inSrDagEdge->setSink(inputVx);
			inputVx->addInputEdge(inSrDagEdge);
//			if(inputVx->getNbOutputEdge()>0) inputVx->setState(SrVxStExecutable);

			// Finding an unplugged connector vertex in the right side.
			inputVx = findUnplugIF(input_vertex);
		}

		// Finding an unplugged output connector vertex in the right side.
		SRDAGVertex* outputVx;
		outputVx = findUnplugIF(output_vertex);
		while(outputVx){
			// Getting the output edge from the higher level in the PiSDF.
			BaseEdge* refEdge = ((PiSDFIfVertex*)outputVx->getReference())->getParentEdge();
			// Getting the corresponding edge in the global DAG.
			SRDAGEdge* outSrDagEdge = getEdgeByRef(outputVx->getParent(), refEdge, output_vertex);
			// Changing the source of the DAG edge to the output of the lower level.
			outSrDagEdge->setSource(outputVx);
			outputVx->addOutputEdge(outSrDagEdge);
//			if(outputVx->getNbOutputEdge()>0) outputVx->setState(SrVxStExecutable);

			outputVx = findUnplugIF(output_vertex);
		}
	}


#if PRINT_GRAPH
//	// Printing the global dag after merging.
//	len = snprintf(file, MAX_FILE_NAME_SIZE, "%s_%d_%d.gv", POST_SRDAG_FILE_NAME, level, step);
//	if(len > MAX_FILE_NAME_SIZE){
//		exitWithCode(1072);
//	}
//	dotWriter.write(this, file, 1, 1);
#endif
}


void SRDAGGraph::removeVx(SRDAGVertex* Vx){

}


/**
 Gets pointers on the edges of the graph in the order of their source or sink.
 Accelerates getting the input or output edges. The edges are linked together
 to constitute a linked list.

 @param startIndex: only the edges after this index are reordered
*/
void SRDAGGraph::sortEdges(int startIndex){
	/*int length = this->nbEdges - startIndex;
	if(length>1){
		quickSort((&edges[startIndex]), length, 0);
	}*/
	SRDAGEdge* currentNewEdge, *currentOldEdge;
	SRDAGVertex* currentNewSink;

	for(int i=startIndex; i<nbEdges; i++){
		currentNewEdge = &edges[i];

		// Adding the first edge
		if(SRDAGEdge::firstInSinkOrder == (SRDAGEdge*)NULL){
			SRDAGEdge::firstInSinkOrder = currentNewEdge;
			SRDAGEdge::lastInSinkOrder = currentNewEdge;
			currentNewEdge->prevInSinkOrder = (SRDAGEdge*)NULL;
			currentNewEdge->nextInSinkOrder = (SRDAGEdge*)NULL;
		}
		else{
			currentNewSink = currentNewEdge->getSink();

			/*currentOldEdge = SRDAGEdge::firstInSinkOrder;
			// Going through the already ordered edges
			// while the edge has a next one and must be before the one we add, we go to the next one
			while((currentOldEdge->nextInSinkOrder != NULL) && (currentOldEdge->getSink() < currentNewSink)){
				currentOldEdge = currentOldEdge->nextInSinkOrder;
			}*/

			currentOldEdge = SRDAGEdge::lastInSinkOrder;
			// Going through the already ordered edges in reverse order
			// while the edge has a next one and must be before the one we add, we go to the next one
			while((currentOldEdge->prevInSinkOrder != NULL) && (currentOldEdge->getSink() > currentNewSink)){
				currentOldEdge = currentOldEdge->prevInSinkOrder;
			}

			// The next is null and we need to add the new edge after the old one
			if(currentOldEdge->getSink() <= currentNewSink){
				currentNewEdge->prevInSinkOrder = currentOldEdge;
				currentNewEdge->nextInSinkOrder = (SRDAGEdge*)NULL;
				SRDAGEdge::lastInSinkOrder = currentNewEdge;
				currentOldEdge->nextInSinkOrder = currentNewEdge;
			}
			else{
				// We need to add the new edge before the old one
				currentNewEdge->prevInSinkOrder = currentOldEdge->prevInSinkOrder;
				if(currentOldEdge->prevInSinkOrder != (SRDAGEdge*)NULL){
					currentOldEdge->prevInSinkOrder->nextInSinkOrder = currentNewEdge;
				}
				else{
					//New start vertex
					SRDAGEdge::firstInSinkOrder = currentNewEdge;
				}
				currentNewEdge->nextInSinkOrder = currentOldEdge;
				currentOldEdge->prevInSinkOrder = currentNewEdge;
			}
		}
	}
}
