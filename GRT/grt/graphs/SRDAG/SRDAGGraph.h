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

#include "SRDAGVertexAbstract.h"
#include "SRDAGVertexBroadCast.h"
#include "SRDAGVertexConfig.h"
#include "SRDAGVertexInitEnd.h"
#include "SRDAGVertexNormal.h"
#include "SRDAGVertexRB.h"
#include "SRDAGVertexXplode.h"

/**
 * A SRDAG graph. It contains SRDAG vertices and edges. It has a bigger table for vertices and edges than DAG.
 * Each edge production and consumption must be equal. There is no repetition vector for the vertices.
 */
class SRDAGGraph{
	private :
		Pool<SRDAGVertexNormal,NORMAL_POOL_SIZE> 		vertexNoPool;
		Pool<SRDAGVertexBroadcast,BROADCAST_POOL_SIZE> 	vertexBrPool;
		Pool<SRDAGVertexConfig,CONFIG_POOL_SIZE> 		vertexCfPool;
		Pool<SRDAGVertexInitEnd,INITEND_POOL_SIZE> 		vertexIEPool;
		Pool<SRDAGVertexRB,RB_POOL_SIZE> 				vertexRBPool;
		Pool<SRDAGVertexXplode,XPLODE_POOL_SIZE> 		vertexXpPool;

		Pool<SRDAGEdge,EDGE_POOL_SIZE> 					ePool;

		Set<SRDAGVertexAbstract, MAX_SRDAG_VERTICES> 	vertices;
		Set<SRDAGVertexXplode, XPLODE_POOL_SIZE> 		implodes;
		Set<SRDAGVertexRB, RB_POOL_SIZE> 				rbs;
		Set<SRDAGVertexBroadcast, BROADCAST_POOL_SIZE>	brs;
//		Set<SRDAGEdge, EDGE_POOL_SIZE>					edges;

	public : 
		SRDAGGraph();
		~SRDAGGraph();
		void reset();

		SRDAGVertexNormal* 		createVertexNo(int refIx, int itrIx, PiSDFVertex* ref);
		SRDAGVertexBroadcast* 	createVertexBr(int refIx, int itrIx, PiSDFVertex* ref);
		SRDAGVertexConfig* 		createVertexCf(int refIx, int itrIx, PiSDFConfigVertex* ref);
		SRDAGVertexInitEnd* 	createVertexIn(int refIx, int itrIx);
		SRDAGVertexInitEnd* 	createVertexEn(int refIx, int itrIx);
		SRDAGVertexRB* 			createVertexRB(int refIx, int itrIx, PiSDFAbstractVertex* ref);
		SRDAGVertexXplode* 		createVertexIm(int refIx, int itrIx);
		SRDAGVertexXplode* 		createVertexEx(int refIx, int itrIx);

		SRDAGEdge* 				createEdge(PiSDFEdge* ref);

		void removeVertex(SRDAGVertexAbstract* vertex);
		void removeEdge(SRDAGEdge* edge);

		int getNbVertices();
		int getNbEdges();

		SRDAGVertexAbstract* getNextHierVertex();

		SetIterator<SRDAGVertexAbstract,MAX_SRDAG_VERTICES>	getVertexIterator();
		PoolIterator<SRDAGEdge, EDGE_POOL_SIZE> 				getEdgeIterator();

		SetIterator<SRDAGVertexXplode,XPLODE_POOL_SIZE> 	getImplodeIterator();
		SetIterator<SRDAGVertexRB,RB_POOL_SIZE> 			getRBIterator();
		SetIterator<SRDAGVertexBroadcast,BROADCAST_POOL_SIZE> getBrIterator();

		int getVerticesFromReference(PiSDFAbstractVertex* ref, int iteration, SRDAGVertexAbstract** output);

		SRDAGVertexAbstract* getVertexFromIx(int ix){
			SetIterator<SRDAGVertexAbstract,MAX_SRDAG_VERTICES> iter = vertices.getIterator();
			SRDAGVertexAbstract* vertex;
			while((vertex = iter.next()) != NULL){
				if(vertex->getId() == ix)
					return vertex;
			}
			return (SRDAGVertexAbstract*) NULL;
		}

		void updateState();

		void print(const char* name, bool displayNames, bool displayRates);
};


inline void SRDAGGraph::print(const char* name, bool displayNames, bool displayRates){
	// Printing the topDag
	DotWriter::write(this, name, displayNames, displayRates);
}

inline SRDAGVertexNormal* SRDAGGraph::createVertexNo(
		int refIx,
		int itrIx,
		PiSDFVertex* ref){
	SRDAGVertexNormal* vertex = vertexNoPool.alloc();
	*vertex = SRDAGVertexNormal(this, refIx, itrIx, ref);
	if(ref != NULL)	ref->addChildVertex(vertex, itrIx);
	vertices.add(vertex);
	return vertex;
}

inline SRDAGVertexConfig* SRDAGGraph::createVertexCf(
		int refIx,
		int itrIx,
		PiSDFConfigVertex* ref){
	SRDAGVertexConfig* vertex = vertexCfPool.alloc();
	*vertex = SRDAGVertexConfig(this, refIx, itrIx, ref);
	if(ref != NULL)	ref->addChildVertex(vertex, itrIx);
	vertices.add(vertex);
	return vertex;
}

inline SRDAGVertexBroadcast* SRDAGGraph::createVertexBr(
		int refIx,
		int itrIx,
		PiSDFVertex* ref){
	SRDAGVertexBroadcast* vertex = vertexBrPool.alloc();
	*vertex = SRDAGVertexBroadcast(this, refIx, itrIx, ref);
	if(ref != NULL)	ref->addChildVertex(vertex, itrIx);
	vertices.add(vertex);
	brs.add(vertex);
	return vertex;
}

inline SRDAGVertexInitEnd* SRDAGGraph::createVertexIn(
		int refIx,
		int itrIx){
	SRDAGVertexInitEnd* vertex = vertexIEPool.alloc();
	*vertex = SRDAGVertexInitEnd(this, Init, refIx, itrIx);
	vertices.add(vertex);
	return vertex;
}

inline SRDAGVertexInitEnd* SRDAGGraph::createVertexEn(
		int refIx,
		int itrIx){
	SRDAGVertexInitEnd* vertex = vertexIEPool.alloc();
	*vertex = SRDAGVertexInitEnd(this, End, refIx, itrIx);
	vertices.add(vertex);
	return vertex;
}

inline SRDAGVertexRB* SRDAGGraph::createVertexRB(
		int refIx,
		int itrIx,
		PiSDFAbstractVertex* ref){
	SRDAGVertexRB* vertex = vertexRBPool.alloc();
	*vertex = SRDAGVertexRB(this, refIx, itrIx, ref);
	if(ref != NULL)	ref->addChildVertex(vertex, itrIx);
	vertices.add(vertex);
	rbs.add(vertex);
	return vertex;
}

inline SRDAGVertexXplode* SRDAGGraph::createVertexIm(
		int refIx,
		int itrIx){
	SRDAGVertexXplode* vertex = vertexXpPool.alloc();
	*vertex = SRDAGVertexXplode(this, Implode, refIx, itrIx);
	vertices.add(vertex);
	implodes.add(vertex);
	return vertex;
}

inline SRDAGVertexXplode* SRDAGGraph::createVertexEx(
		int refIx,
		int itrIx){
	SRDAGVertexXplode* vertex = vertexXpPool.alloc();
	*vertex = SRDAGVertexXplode(this, Explode, refIx, itrIx);
	vertices.add(vertex);
	return vertex;
}

inline
SRDAGEdge* SRDAGGraph::createEdge(PiSDFEdge* ref){
	SRDAGEdge* edge = ePool.alloc();
	*edge = SRDAGEdge(this);
//	edges.add(edge);
	return edge;
}

inline
SRDAGVertexAbstract* SRDAGGraph::getNextHierVertex(){
	SetIterator<SRDAGVertexAbstract,MAX_SRDAG_VERTICES> iter = vertices.getIterator();
	SRDAGVertexAbstract* vertex;
	while((vertex = iter.next()) != NULL){
		if(vertex->isHierarchical() &&
				vertex->getState() == SRDAG_Executable)
			return vertex;
	}
	return (SRDAGVertexAbstract*)NULL;
}

inline SetIterator<SRDAGVertexAbstract,MAX_SRDAG_VERTICES> SRDAGGraph::getVertexIterator(){
	return vertices.getIterator();
}

inline SetIterator<SRDAGVertexXplode,XPLODE_POOL_SIZE> SRDAGGraph::getImplodeIterator(){
	return implodes.getIterator();
}

inline SetIterator<SRDAGVertexRB,RB_POOL_SIZE> SRDAGGraph::getRBIterator(){
	return rbs.getIterator();
}

inline SetIterator<SRDAGVertexBroadcast,BROADCAST_POOL_SIZE> SRDAGGraph::getBrIterator(){
	return brs.getIterator();
}

/**
 Gets the edge at the given index
 
 @param index: index of the edge in the edge list
 @return edge
*/
inline PoolIterator<SRDAGEdge, EDGE_POOL_SIZE>  SRDAGGraph::getEdgeIterator(){
	return ePool.getIterator();
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

inline int SRDAGGraph::getVerticesFromReference(PiSDFAbstractVertex* ref, int iteration, SRDAGVertexAbstract** output){
	int size = 0;
	SetIterator<SRDAGVertexAbstract,MAX_SRDAG_VERTICES> iter = vertices.getIterator();
	SRDAGVertexAbstract* vertex;
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
	return ePool.getNb();
}

inline void SRDAGGraph::updateState(){
	// Updating all input, round-buffer and hierarchical vxs.
	SetIterator<SRDAGVertexAbstract,MAX_SRDAG_VERTICES> iter = this->getVertexIterator();
	SRDAGVertexAbstract *vertex;
	while((vertex = iter.next()) != NULL){
		vertex->updateState();
	}
}



#endif
