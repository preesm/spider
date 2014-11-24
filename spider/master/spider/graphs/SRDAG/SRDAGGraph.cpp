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

#include <graphs/SRDAG/SRDAGCommon.h>
#include <graphs/SRDAG/SRDAGEdge.h>
#include <graphs/SRDAG/SRDAGGraph.h>
#include <graphs/SRDAG/SRDAGVertex.h>
#include <tools/Set.h>
#include <platform_file.h>

#include <cstdio>

#define MAX_VERTEX 10000
#define MAX_EDGE 10000

SRDAGGraph::SRDAGGraph() {
	stack_ = 0;
}

SRDAGGraph::SRDAGGraph(Stack *stack){
	stack_ = stack;

	edges_ = SRDAGEdgeSet(MAX_EDGE, stack);
	vertices_ = SRDAGVertexSet(MAX_VERTEX, stack);
}

SRDAGGraph::~SRDAGGraph() {
}

SRDAGVertex* SRDAGGraph::addVertex(PiSDFVertex* reference){
	SRDAGVertex* vertex = sAlloc(stack_, 1, SRDAGVertex);
	*vertex = SRDAGVertex(
			SRDAG_NORMAL, this,
			reference,
			reference->getNInEdge(),
			reference->getNOutEdge(),
			reference->getNInParam(),
			reference->getNOutParam(),
			stack_);
	vertices_.add(vertex);
	return vertex;
}

SRDAGVertex* SRDAGGraph::addBroadcast(int nOutput){
	SRDAGVertex* vertex = sAlloc(stack_, 1, SRDAGVertex);
	*vertex = SRDAGVertex(
			SRDAG_BROADCAST, this,
			0 /*Ref*/,
			1 /*nInEdge*/,
			nOutput /*nOutEdge*/,
			0 /*nInParam*/,
			0 /*nOutParam*/,
			stack_);
	vertices_.add(vertex);
	return vertex;
}

SRDAGVertex* SRDAGGraph::addFork(int nOutput){
	SRDAGVertex* vertex = sAlloc(stack_, 1, SRDAGVertex);
	*vertex = SRDAGVertex(
			SRDAG_FORK, this,
			0 /*Ref*/,
			1 /*nInEdge*/,
			nOutput /*nOutEdge*/,
			0 /*nInParam*/,
			0 /*nOutParam*/,
			stack_);
	vertices_.add(vertex);
	return vertex;
}

SRDAGVertex* SRDAGGraph::addJoin(int nInput){
	SRDAGVertex* vertex = sAlloc(stack_, 1, SRDAGVertex);
	*vertex = SRDAGVertex(
			SRDAG_JOIN, this,
			0 /*Ref*/,
			nInput /*nInEdge*/,
			1 /*nOutEdge*/,
			0 /*nInParam*/,
			0 /*nOutParam*/,
			stack_);
	vertices_.add(vertex);
	return vertex;
}

SRDAGVertex* SRDAGGraph::addInit(){
	SRDAGVertex* vertex = sAlloc(stack_, 1, SRDAGVertex);
	*vertex = SRDAGVertex(
			SRDAG_INIT, this,
			0 /*Ref*/,
			0 /*nInEdge*/,
			1 /*nOutEdge*/,
			0 /*nInParam*/,
			0 /*nOutParam*/,
			stack_);
	vertices_.add(vertex);
	return vertex;
}

SRDAGVertex* SRDAGGraph::addEnd(){
	SRDAGVertex* vertex = sAlloc(stack_, 1, SRDAGVertex);
	*vertex = SRDAGVertex(
			SRDAG_END, this,
			0 /*Ref*/,
			1 /*nInEdge*/,
			0 /*nOutEdge*/,
			0 /*nInParam*/,
			0 /*nOutParam*/,
			stack_);
	vertices_.add(vertex);
	return vertex;
}

SRDAGVertex* SRDAGGraph::addRoundBuffer(){
	SRDAGVertex* vertex = sAlloc(stack_, 1, SRDAGVertex);
	*vertex = SRDAGVertex(
			SRDAG_ROUNDBUFFER, this,
			0 /*Ref*/,
			1 /*nInEdge*/,
			1 /*nOutEdge*/,
			0 /*nInParam*/,
			0 /*nOutParam*/,
			stack_);
	vertices_.add(vertex);
	return vertex;
}

SRDAGEdge* SRDAGGraph::addEdge() {
	SRDAGEdge* edge = sAlloc(stack_, 1, SRDAGEdge);
	*edge = SRDAGEdge(this);
	edges_.add(edge);
	return edge;
}

void SRDAGGraph::delVertex(SRDAGVertex* vertex){
	for(int i=0; i<vertex->getNInEdge(); i++)
		if(vertex->getInEdge(i) != 0)
			vertex->getInEdge(i)->disconnectSnk();
	for(int i=0; i<vertex->getNOutEdge(); i++)
		if(vertex->getOutEdge(i) != 0)
			vertex->getOutEdge(i)->disconnectSrc();
	vertices_.del(vertex);
}

void SRDAGGraph::delEdge(SRDAGEdge* edge){
	edges_.del(edge);
}

/** Print Fct */
void SRDAGGraph::print(const char *path){

	if(!check())
		throw "Errors in the SRDAG Graph\n";

	int maxId;
	int file = platform_fopen (path);
	if(file == -1){
		printf("cannot open %s\n", path);
		return;
	}

	// Writing header
	platform_fprintf (file, "digraph csdag {\n");
	platform_fprintf (file, "\tnode [color=\"#433D63\"];\n");
	platform_fprintf (file, "\tedge [color=\"#9262B6\" arrowhead=\"empty\"];\n");
	platform_fprintf (file, "\trankdir=LR;\n\n");

	/** Declare Iterator */
	SRDAGEdgeIterator edgeIt		= edges_.getIterator();
	SRDAGVertexIterator vertexIt	= vertices_.getIterator();

	// Drawing vertices.
	platform_fprintf (file, "\t# Vertices\n");
	for (vertexIt.first(); vertexIt.finished(); vertexIt.next()){
		char name[100];
		vertexIt.current()->toString(name, 100);
		platform_fprintf (file, "\t%d [shape=ellipse,label=\"%d\\n%s (%d)",
				vertexIt.current()->getId(),
				vertexIt.current()->getId(),
				name,
				vertexIt.current()->getFctId());
		platform_fprintf (file, "\",color=");
		switch (vertexIt.current()->getState()){
			case SRDAG_EXEC:
				platform_fprintf (file, "blue");
				break;
			case SRDAG_RUN:
				platform_fprintf (file, "gray");
				break;
			case SRDAG_NEXEC:
				if(vertexIt.current()->isHierarchical())
					platform_fprintf (file, "red");
				else
					platform_fprintf (file, "black");
				break;
		}
		platform_fprintf (file, "];\n");

		maxId = (vertexIt.current()->getId() > maxId) ? vertexIt.current()->getId() : maxId;
	}

	// Drawing edges.
	platform_fprintf (file, "\t# Edges\n");
	for (edgeIt.first(); edgeIt.finished(); edgeIt.next()) {
		int snkIx, srcIx;

		if(edgeIt.current()->getSrc())
			srcIx = edgeIt.current()->getSrc()->getId();
		else{
			platform_fprintf (file, "\t%d [shape=point];\n", ++maxId);
			srcIx = maxId;
		}
		if(edgeIt.current()->getSnk())
			snkIx = edgeIt.current()->getSnk()->getId();
		else{
			platform_fprintf (file, "\t%d [shape=point];\n", ++maxId);
			snkIx = maxId;
		}

//		switch(mode){
//		case DataRates:
			platform_fprintf (file, "\t%d->%d [label=\"%d\",taillabel=\"%d\",headlabel=\"%d\"];\n",
					srcIx, snkIx,
					edgeIt.current()->getRate(),
					edgeIt.current()->getSrcPortIx(),
					edgeIt.current()->getSnkPortIx());
//			break;
//		case Allocation:
//			platform_fprintf (file, "\t%d->%d [label=\"%d: %#x (%#x)\"];\n",
//					srcIx, snkIx,
//					edgeIt.current()->fifo.id,
//					edgeIt.current()->fifo.add,
//					edgeIt.current()->fifo.size);
//			break;
//		}

	}


	platform_fprintf (file, "}\n");
	platform_fclose(file);
}

bool SRDAGGraph::check(){
	bool result = true;

	/** Declare Iterator */
	SRDAGEdgeIterator edgeIt		= edges_.getIterator();
	SRDAGVertexIterator vertexIt	= vertices_.getIterator();

	/* Check vertices */
	for (vertexIt.first(); vertexIt.finished(); vertexIt.next()){
		int j;

		// Check input edges
		for(j=0; j<vertexIt.current()->getNInEdge(); j++){
			const SRDAGEdge* edge = vertexIt.current()->getInEdge(j);
			if(edge == NULL){
				printf("V%d Input%d: not connected\n", vertexIt.current()->getId(), j);
				result = false;
			}else if(edge->getSnk() != vertexIt.current()){
				printf("V%d E%d: connection mismatch\n", vertexIt.current()->getId(), edge->getId());
				result = false;
			}
		}

		// Check output edges
		for(j=0; j<vertexIt.current()->getNOutEdge(); j++){
			const SRDAGEdge* edge = vertexIt.current()->getOutEdge(j);
			if(edge == NULL){
				printf("V%d Output%d: not connected\n", vertexIt.current()->getId(), j);
				result = false;
			}else if(edge->getSrc() != vertexIt.current()){
				printf("V%d E%d: connection mismatch\n", vertexIt.current()->getId(), edge->getId());
				result = false;
			}
		}
	}

	/* Check edges */


	return result;
}
