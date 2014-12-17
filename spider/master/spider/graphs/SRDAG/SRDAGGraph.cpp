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

#include <cstdio>

#define MAX_VERTEX 10000
#define MAX_EDGE 10000

SRDAGGraph::SRDAGGraph(Stack *stack){
	stack_ = stack;

	edges_ = SRDAGEdgeSet(MAX_EDGE, stack_);
	vertices_ = SRDAGVertexSet(MAX_VERTEX, stack_);
}

SRDAGGraph::~SRDAGGraph() {
}

SRDAGVertex* SRDAGGraph::addVertex(PiSDFVertex* reference){
	SRDAGVertex* vertex = CREATE(stack_, SRDAGVertex)(
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
	SRDAGVertex* vertex = CREATE(stack_, SRDAGVertex)(
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
	SRDAGVertex* vertex = CREATE(stack_, SRDAGVertex)(
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
	SRDAGVertex* vertex = CREATE(stack_, SRDAGVertex)(
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
	SRDAGVertex* vertex = CREATE(stack_, SRDAGVertex)(
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
	SRDAGVertex* vertex = CREATE(stack_, SRDAGVertex)(
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
	SRDAGVertex* vertex = CREATE(stack_, SRDAGVertex)(
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
	SRDAGEdge* edge = CREATE(stack_, SRDAGEdge)(this);
	edges_.add(edge);
	return edge;
}

SRDAGEdge* SRDAGGraph::addEdge(
		SRDAGVertex* src, int srcPortIx,
		SRDAGVertex* snk, int snkPortIx,
		int rate) {
	SRDAGEdge* edge = CREATE(stack_, SRDAGEdge)(this);
	edges_.add(edge);

	edge->connectSrc(src, srcPortIx);
	edge->connectSnk(snk, snkPortIx);
	edge->setRate(rate);

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

int SRDAGGraph::getNExecVertex(){
	int n=0;
	for(int i=0; i<vertices_.getN(); i++){
		if(vertices_[i]->getState() == SRDAG_EXEC)
			n++;
	}
	return n;
}

void SRDAGGraph::updateState(){
	for(int i=0; i<vertices_.getN(); i++){
		vertices_[i]->updateState();
	}
}

/** Print Fct */
void SRDAGGraph::print(const char *path){

	if(!check())
		printf("Errors in the SRDAG Graph\n");

	int maxId = -1;
	int file = Platform::get()->fopen(path);
	if(file == -1){
		printf("cannot open %s\n", path);
		return;
	}

	// Writing header
	Platform::get()->fprintf(file, "digraph csdag {\n");
	Platform::get()->fprintf(file, "\tnode [color=\"#433D63\"];\n");
	Platform::get()->fprintf(file, "\tedge [color=\"#9262B6\" arrowhead=\"empty\"];\n");
	Platform::get()->fprintf(file, "\trankdir=LR;\n\n");

	// Drawing vertices.
	Platform::get()->fprintf(file, "\t# Vertices\n");
	for (int i=0; i<vertices_.getN(); i++){
		char name[100];
		SRDAGVertex* vertex = vertices_[i];
		vertex->toString(name, 100);
		Platform::get()->fprintf(file, "\t%d [shape=ellipse,label=\"%d\\n%s (%d)",
				vertex->getId(),
				vertex->getId(),
				name,
				vertex->getFctId());
		Platform::get()->fprintf(file, "\",color=");
		switch (vertex->getState()){
			case SRDAG_EXEC:
				Platform::get()->fprintf(file, "blue");
				break;
			case SRDAG_RUN:
				Platform::get()->fprintf(file, "gray");
				break;
			case SRDAG_NEXEC:
				if(vertex->isHierarchical())
					Platform::get()->fprintf(file, "red");
				else
					Platform::get()->fprintf(file, "black");
				break;
		}
		Platform::get()->fprintf(file, "];\n");

		maxId = (vertex->getId() > maxId) ? vertex->getId() : maxId;
	}

	// Drawing edges.
	Platform::get()->fprintf(file, "\t# Edges\n");
	for (int i=0; i<edges_.getN(); i++) {
		SRDAGEdge* edge = edges_[i];
		int snkIx, srcIx;

		if(edge->getSrc())
			srcIx = edge->getSrc()->getId();
		else{
			Platform::get()->fprintf(file, "\t%d [shape=point];\n", ++maxId);
			srcIx = maxId;
		}
		if(edge->getSnk())
			snkIx = edge->getSnk()->getId();
		else{
			Platform::get()->fprintf(file, "\t%d [shape=point];\n", ++maxId);
			snkIx = maxId;
		}

//		switch(mode){
//		case DataRates:
			Platform::get()->fprintf(file, "\t%d->%d [label=\"%d\n%d\",taillabel=\"%d\",headlabel=\"%d\"];\n",
					srcIx, snkIx,
					edge->getRate(),
					edge->getAlloc(),
					edge->getSrcPortIx(),
					edge->getSnkPortIx());
//			break;
//		case Allocation:
//			Platform::get()->fprintf(file, "\t%d->%d [label=\"%d: %#x (%#x)\"];\n",
//					srcIx, snkIx,
//					edge->fifo.id,
//					edge->fifo.add,
//					edge->fifo.size);
//			break;
//		}

	}


	Platform::get()->fprintf(file, "}\n");
	Platform::get()->fclose(file);
}

bool SRDAGGraph::check(){
	bool result = true;

	/* Check vertices */
	for (int i=0; i<vertices_.getN(); i++){
		SRDAGVertex* vertex = vertices_[i];
		int j;

		// Check input edges
		for(j=0; j<vertex->getNInEdge(); j++){
			const SRDAGEdge* edge = vertex->getInEdge(j);
			if(edge == NULL){
				printf("V%d Input%d: not connected\n", vertex->getId(), j);
				result = false;
			}else if(edge->getSnk() != vertex){
				printf("V%d E%d: connection mismatch\n", vertex->getId(), edge->getId());
				result = false;
			}
		}

		// Check output edges
		for(j=0; j<vertex->getNOutEdge(); j++){
			const SRDAGEdge* edge = vertex->getOutEdge(j);
			if(edge == NULL){
				printf("V%d Output%d: not connected\n", vertex->getId(), j);
				result = false;
			}else if(edge->getSrc() != vertex){
				printf("V%d E%d: connection mismatch\n", vertex->getId(), edge->getId());
				result = false;
			}
		}
	}

	/* Check edges */


	return result;
}
