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

#include "SRDAGEdge.h"
#include "../PiSDF/PiSDFAbstractVertex.h"
#include "../PiSDF/PiSDFConfigVertex.h"

#include <grt_definitions.h>
#include <platform_types.h>
#include "../../tools/SchedulingError.h"
#include <tools/Pool.h>
#include <tools/PoolIterator.h>
#include <tools/List.h>
#include <tools/Set.h>
#include <tools/SetIterator.h>
#include <tools/DotWriter.h>

#include "SRDAGVertex.h"
#include <graphs/PiSDF/PiSDFVertex.h>

/**
 * A SRDAG graph. It contains SRDAG vertices and edges. It has a bigger table for vertices and edges than DAG.
 * Each edge production and consumption must be equal. There is no repetition vector for the vertices.
 */
class SRDAGGraph{
	private :
		Pool<SRDAGVertex,MAX_SRDAG_VERTICES>	vertexPool;
		Pool<SRDAGEdge,MAX_SRDAG_EDGES>			edgePool;

		Set<SRDAGVertex, MAX_SRDAG_VERTICES> 	vertices;
		Set<SRDAGVertex, MAX_SRDAG_IMPLODES>	implodes;
		Set<SRDAGVertex, MAX_SRDAG_RBS>			rbs;
		Set<SRDAGVertex, MAX_SRDAG_BROADCASTS>	brs;

	public : 
		SRDAGGraph();
		~SRDAGGraph();
		void reset();

		SRDAGVertex* 	createVertexNo(int refIx, int itrIx, PiSDFVertex* ref);
		SRDAGVertex* 	createVertexBr(int refIx, int itrIx, PiSDFVertex* ref);
		SRDAGVertex* 	createVertexCf(int refIx, int itrIx, PiSDFConfigVertex* ref);
		SRDAGVertex* 	createVertexIn(int refIx, int itrIx);
		SRDAGVertex* 	createVertexEn(int refIx, int itrIx);
		SRDAGVertex* 	createVertexRB(int refIx, int itrIx, PiSDFAbstractVertex* ref);
		SRDAGVertex* 	createVertexIm(int refIx, int itrIx);
		SRDAGVertex* 	createVertexEx(int refIx, int itrIx);

		SRDAGEdge* 		createEdge(PiSDFEdge* ref);

		void removeVertex(SRDAGVertex* vertex);
		void removeEdge(SRDAGEdge* edge);

		int getNbVertices();
		int getNbEdges();

		SRDAGVertex* getNextHierVertex();

		SetIterator<SRDAGVertex,MAX_SRDAG_VERTICES>		getVertexIterator();
		PoolIterator<SRDAGEdge, MAX_SRDAG_EDGES> 		getEdgeIterator();

		SetIterator<SRDAGVertex,MAX_SRDAG_IMPLODES>		getImplodeIterator();
		SetIterator<SRDAGVertex,MAX_SRDAG_RBS> 			getRBIterator();
		SetIterator<SRDAGVertex,MAX_SRDAG_BROADCASTS> 	getBrIterator();

		int getVerticesFromReference(PiSDFAbstractVertex* ref, int iteration, SRDAGVertex** output);

		SRDAGVertex* getVertexFromIx(int ix){
			SetIterator<SRDAGVertex,MAX_SRDAG_VERTICES> iter = vertices.getIterator();
			SRDAGVertex* vertex;
			while((vertex = iter.next()) != NULL){
				if(vertex->getId() == ix)
					return vertex;
			}
			return (SRDAGVertex*) NULL;
		}

		void updateState();

		void print(const char* name, bool displayNames, bool displayRates);
};


inline void SRDAGGraph::print(const char* name, bool displayNames, bool displayRates){
	// Printing the topDag
	DotWriter::write(this, name, displayNames, displayRates);
}

inline SRDAGVertex* SRDAGGraph::createVertexNo(
		int refIx,
		int itrIx,
		PiSDFVertex* ref){
	SRDAGVertex* vertex = vertexPool.alloc();
	*vertex = SRDAGVertex(this, Normal, ref, refIx, itrIx);
	if(ref != NULL)	ref->addChildVertex(vertex, itrIx);
	vertices.add(vertex);
	return vertex;
}

inline SRDAGVertex* SRDAGGraph::createVertexCf(
		int refIx,
		int itrIx,
		PiSDFConfigVertex* ref){
	SRDAGVertex* vertex = vertexPool.alloc();
	*vertex = SRDAGVertex(this, ConfigureActor, ref, refIx, itrIx);
	if(ref != NULL)	ref->addChildVertex(vertex, itrIx);
	vertices.add(vertex);
	return vertex;
}

inline SRDAGVertex* SRDAGGraph::createVertexBr(
		int refIx,
		int itrIx,
		PiSDFVertex* ref){
	SRDAGVertex* vertex = vertexPool.alloc();
	*vertex = SRDAGVertex(this, Broadcast, ref, refIx, itrIx);
	if(ref != NULL)	ref->addChildVertex(vertex, itrIx);
	vertices.add(vertex);
	brs.add(vertex);
	return vertex;
}

inline SRDAGVertex* SRDAGGraph::createVertexIn(
		int refIx,
		int itrIx){
	SRDAGVertex* vertex = vertexPool.alloc();
	*vertex = SRDAGVertex(this, Init, (PiSDFAbstractVertex*)NULL, refIx, itrIx);
	vertices.add(vertex);
	return vertex;
}

inline SRDAGVertex* SRDAGGraph::createVertexEn(
		int refIx,
		int itrIx){
	SRDAGVertex* vertex = vertexPool.alloc();
	*vertex = SRDAGVertex(this, End, (PiSDFAbstractVertex*)NULL, refIx, itrIx);
	vertices.add(vertex);
	return vertex;
}

inline SRDAGVertex* SRDAGGraph::createVertexRB(
		int refIx,
		int itrIx,
		PiSDFAbstractVertex* ref){
	SRDAGVertex* vertex = vertexPool.alloc();
	*vertex = SRDAGVertex(this, RoundBuffer, ref, refIx, itrIx);
	if(ref != NULL)	ref->addChildVertex(vertex, itrIx);
	vertices.add(vertex);
	rbs.add(vertex);
	return vertex;
}

inline SRDAGVertex* SRDAGGraph::createVertexIm(
		int refIx,
		int itrIx){
	SRDAGVertex* vertex = vertexPool.alloc();
	*vertex = SRDAGVertex(this, Implode, (PiSDFAbstractVertex*)NULL, refIx, itrIx);
	vertices.add(vertex);
	implodes.add(vertex);
	return vertex;
}

inline SRDAGVertex* SRDAGGraph::createVertexEx(
		int refIx,
		int itrIx){
	SRDAGVertex* vertex = vertexPool.alloc();
	*vertex = SRDAGVertex(this, Explode, (PiSDFAbstractVertex*)NULL, refIx, itrIx);
	vertices.add(vertex);
	return vertex;
}

inline
SRDAGEdge* SRDAGGraph::createEdge(PiSDFEdge* ref){
	SRDAGEdge* edge = edgePool.alloc();
	*edge = SRDAGEdge(this);
//	edges.add(edge);
	return edge;
}

inline
SRDAGVertex* SRDAGGraph::getNextHierVertex(){
	SetIterator<SRDAGVertex,MAX_SRDAG_VERTICES> iter = vertices.getIterator();
	SRDAGVertex* vertex;
	while((vertex = iter.next()) != NULL){
		if(vertex->isHierarchical() &&
				vertex->getState() == SRDAG_Executable)
			return vertex;
	}
	return (SRDAGVertex*)NULL;
}

inline SetIterator<SRDAGVertex,MAX_SRDAG_VERTICES> SRDAGGraph::getVertexIterator(){
	return vertices.getIterator();
}

inline SetIterator<SRDAGVertex,MAX_SRDAG_IMPLODES> SRDAGGraph::getImplodeIterator(){
	return implodes.getIterator();
}

inline SetIterator<SRDAGVertex,MAX_SRDAG_RBS> SRDAGGraph::getRBIterator(){
	return rbs.getIterator();
}

inline SetIterator<SRDAGVertex,MAX_SRDAG_BROADCASTS> SRDAGGraph::getBrIterator(){
	return brs.getIterator();
}

/**
 Gets the edge at the given index
 
 @param index: index of the edge in the edge list
 @return edge
*/
inline PoolIterator<SRDAGEdge, MAX_SRDAG_EDGES>  SRDAGGraph::getEdgeIterator(){
	return edgePool.getIterator();
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
	SetIterator<SRDAGVertex,MAX_SRDAG_VERTICES> iter = vertices.getIterator();
	SRDAGVertex* vertex;
	while((vertex = iter.next()) != NULL){
		if(vertex->getIterationIndex() == iteration
				&& vertex->getReference() == ref){
			output[size] = vertex;
			size++;
		}
	}
	return size;
}

inline int SRDAGGraph::getNbVertices(){
	return vertices.getNb();
}

inline int SRDAGGraph::getNbEdges(){
//	return edges.getNb();
	return edgePool.getNb();
}

inline void SRDAGGraph::updateState(){
	// Updating all input, round-buffer and hierarchical vxs.
	SetIterator<SRDAGVertex,MAX_SRDAG_VERTICES> iter = this->getVertexIterator();
	SRDAGVertex *vertex;
	while((vertex = iter.next()) != NULL){
		vertex->updateState();
	}
}



#endif
