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

#ifndef SRDAG_GRAPH
#define SRDAG_GRAPH

#include "SRDAGVertex.h"
#include "SRDAGEdge.h"
#include "../PiSDF/PiSDFAbstractVertex.h"

#include <grt_definitions.h>
#include <platform_types.h>
#include "../../tools/SchedulingError.h"
#include <tools/Pool.h>
#include <tools/List.h>
#include <tools/Set.h>
#include <tools/SetIterator.h>

typedef Pool<SRDAGVertex,MAX_SRDAG_VERTICES> vertexPool;
typedef Set<SRDAGVertex, MAX_SRDAG_VERTICES> vertexSet;
typedef SetIterator<SRDAGVertex,MAX_SRDAG_VERTICES> vertexSetIterator;

typedef Pool<SRDAGEdge,MAX_SRDAG_EDGES> edgePool;
typedef Set<SRDAGEdge, MAX_SRDAG_EDGES> edgeSet;
typedef SetIterator<SRDAGEdge,MAX_SRDAG_EDGES> edgeSetIterator;

/**
 * A SRDAG graph. It contains SRDAG vertices and edges. It has a bigger table for vertices and edges than DAG.
 * Each edge production and consumption must be equal. There is no repetition vector for the vertices.
 */
class SRDAGGraph{
	private :
		int vertexIxCount, edgeIxCount;

		vertexPool 	vPool;
		edgePool	ePool;

		vertexSet 	vertices;
		edgeSet		edges;

	public : 
		SRDAGGraph();
		~SRDAGGraph();
		void reset();

		SRDAGVertex* createVertex();
		SRDAGEdge* createEdge(PiSDFEdge* ref);

		void removeVertex(SRDAGVertex* vertex);

		int getNbVertex();
		int getNbEdge();

		SRDAGVertex* getNextHierVertex();

		vertexSetIterator 	getVertexIterator();
		edgeSetIterator 	getEdgeIterator();

		int getVerticesFromReference(PiSDFAbstractVertex* ref, int iteration, SRDAGVertex** output);

		SRDAGVertex* getVertexFromIx(int ix){
			vertexSetIterator iter = vertices.getIterator();
			SRDAGVertex* vertex;
			while((vertex = iter.next()) != NULL){
				if(vertex->id == ix)
					return vertex;
			}
			return NULL;
		}

		void updateExecuted();
};

inline
SRDAGVertex* SRDAGGraph::createVertex(){
	SRDAGVertex* vertex = vPool.alloc();
	vertices.add(vertex);
	vertex->reset();
	vertex->id = vertexIxCount++;
	vertex->graph = this;

	char name[MAX_TOOL_NAME];
	snprintf(name, MAX_TOOL_NAME, "Input Edges of vertex %d", vertex->id);
	vertex->inputEdges.setName(name);

	return vertex;
}

inline
SRDAGEdge* SRDAGGraph::createEdge(PiSDFEdge* ref){
	SRDAGEdge* edge = ePool.alloc();
	edges.add(edge);
	edge->reset();
	edge->id = edgeIxCount++;
	edge->graph = this;
	edge->setRefEdge(ref);
	return edge;
}

inline
SRDAGVertex* SRDAGGraph::getNextHierVertex(){
	vertexSetIterator iter = vertices.getIterator();
	SRDAGVertex* vertex;
	while((vertex = iter.next()) != NULL){
		if(vertex->isHierarchical() &&
				vertex->getState() == SrVxStExecutable)
			return vertex;
	}
	return (SRDAGVertex*)NULL;
}

inline vertexSetIterator SRDAGGraph::getVertexIterator(){
	return vertices.getIterator();
}


/**
 Gets the edge at the given index
 
 @param index: index of the edge in the edge list
 @return edge
*/
inline edgeSetIterator SRDAGGraph::getEdgeIterator(){
	return edges.getIterator();
}

/**
 Gets the srDag vertices that share the given DAG reference
 
 @param ref: the DAG reference
 @param output: the output SRDAG vertices
 @return the number of found references
*/
//inline UINT32 SRDAGGraph::getVerticesFromReference(PiSDFAbstractVertex* ref, UINT32 iteration, SRDAGVertex** output){
//	UINT32 size = 0;
//	for(int i=0; i<ref->getChildNbVertices(); i++){
//		SRDAGVertex* vertex = ref->getChildVertex(i);
//		if(vertex != NULL && vertex->getReference() == ref && vertex->getIterationIndex() == iteration){
//			output[size] = vertex;
//			size++;
//		}
//	}
//	return size;
//}

inline int SRDAGGraph::getVerticesFromReference(PiSDFAbstractVertex* ref, int iteration, SRDAGVertex** output){
	int size = 0;
	vertexSetIterator iter = vertices.getIterator();
	SRDAGVertex* vertex;
	while((vertex = iter.next()) != NULL){
		if(vertex->Reference == ref && vertex->getIterationIndex() == iteration){
			output[size] = vertex;
			size++;
		}
	}
	return size;
}

inline int SRDAGGraph::getNbVertex(){
	return vertices.getNb();
}

inline int SRDAGGraph::getNbEdge(){
	return edges.getNb();
}

inline void SRDAGGraph::updateExecuted(){
	vertexSetIterator iter = vertices.getIterator();
	SRDAGVertex* vertex;
	while((vertex = iter.next()) != NULL){
		if(vertex->getState() == SrVxStExecutable)
			vertex->setState(SrVxStExecuted);
	}
}


#endif
