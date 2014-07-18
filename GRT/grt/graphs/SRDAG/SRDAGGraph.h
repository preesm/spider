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

typedef Pool<SRDAGVertexAbstract,MAX_SRDAG_VERTICES> vertexPool;
typedef Set<SRDAGVertexAbstract, MAX_SRDAG_VERTICES> vertexSet;
typedef SetIterator<SRDAGVertexAbstract,MAX_SRDAG_VERTICES> vertexSetIterator;

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

		Pool<SRDAGVertexNormal,MAX_SRDAG_VERTICES> 		vertexNoPool;
		Pool<SRDAGVertexBroadcast,MAX_SRDAG_VERTICES> 	vertexBrPool;
		Pool<SRDAGVertexConfig,MAX_SRDAG_VERTICES> 		vertexCfPool;
		Pool<SRDAGVertexInitEnd,MAX_SRDAG_VERTICES> 	vertexIEPool;
		Pool<SRDAGVertexRB,MAX_SRDAG_VERTICES> 			vertexRBPool;
		Pool<SRDAGVertexXplode,MAX_SRDAG_VERTICES> 		vertexXpPool;

		edgePool	ePool;

		Set<SRDAGVertexAbstract, MAX_SRDAG_VERTICES> 	vertices;
		Set<SRDAGVertexXplode, MAX_SRDAG_VERTICES> 		implodes;
		Set<SRDAGVertexRB, MAX_SRDAG_VERTICES> 			rbs;
		edgeSet		edges;

	public : 
		SRDAGGraph();
		~SRDAGGraph();
		void reset();

		SRDAGVertexNormal* 		createVertexNo(SRDAGVertexAbstract* parent, int refIx, int itrIx, PiSDFVertex* ref);
		SRDAGVertexBroadcast* 	createVertexBr(SRDAGVertexAbstract* parent, int refIx, int itrIx);
		SRDAGVertexConfig* 		createVertexCf(SRDAGVertexAbstract* parent, int refIx, int itrIx, PiSDFConfigVertex* ref);
		SRDAGVertexInitEnd* 	createVertexIn(SRDAGVertexAbstract* parent, int refIx, int itrIx);
		SRDAGVertexInitEnd* 	createVertexEn(SRDAGVertexAbstract* parent, int refIx, int itrIx);
		SRDAGVertexRB* 			createVertexRB(SRDAGVertexAbstract* parent, int refIx, int itrIx, PiSDFAbstractVertex* ref);
		SRDAGVertexXplode* 		createVertexIm(SRDAGVertexAbstract* parent, int refIx, int itrIx);
		SRDAGVertexXplode* 		createVertexEx(SRDAGVertexAbstract* parent, int refIx, int itrIx);

		SRDAGEdge* 				createEdge(PiSDFEdge* ref);

		void removeVertex(SRDAGVertexAbstract* vertex);
		void removeEdge(SRDAGEdge* edge);

		int getNbVertices();
		int getNbEdges();

		SRDAGVertexAbstract* getNextHierVertex();

		vertexSetIterator 	getVertexIterator();
		edgeSetIterator 	getEdgeIterator();

		SetIterator<SRDAGVertexXplode,MAX_SRDAG_VERTICES> getImplodeIterator();
		SetIterator<SRDAGVertexRB,MAX_SRDAG_VERTICES> getRBIterator();

		int getVerticesFromReference(PiSDFAbstractVertex* ref, int iteration, SRDAGVertexAbstract** output);

		SRDAGVertexAbstract* getVertexFromIx(int ix){
			vertexSetIterator iter = vertices.getIterator();
			SRDAGVertexAbstract* vertex;
			while((vertex = iter.next()) != NULL){
				if(vertex->getId() == ix)
					return vertex;
			}
			return NULL;
		}

		void updateExecuted();
		void print(const char* name, bool displayNames, bool displayRates);
};


inline void SRDAGGraph::print(const char* name, bool displayNames, bool displayRates){
	// Printing the topDag
	DotWriter::write(this, name, displayNames, displayRates);
}

inline SRDAGVertexNormal* SRDAGGraph::createVertexNo(
		SRDAGVertexAbstract* parent,
		int refIx,
		int itrIx,
		PiSDFVertex* ref){
	SRDAGVertexNormal* vertex = vertexNoPool.alloc();
	*vertex = SRDAGVertexNormal(vertexIxCount++, this, parent, refIx, itrIx, ref);
	vertices.add(vertex);
	return vertex;
}

inline SRDAGVertexConfig* SRDAGGraph::createVertexCf(
		SRDAGVertexAbstract* parent,
		int refIx,
		int itrIx,
		PiSDFConfigVertex* ref){
	SRDAGVertexConfig* vertex = vertexCfPool.alloc();
	*vertex = SRDAGVertexConfig(vertexIxCount++, this, parent, refIx, itrIx, ref);
	vertices.add(vertex);
	return vertex;
}

inline SRDAGVertexBroadcast* SRDAGGraph::createVertexBr(
		SRDAGVertexAbstract* parent,
		int refIx,
		int itrIx){
	SRDAGVertexBroadcast* vertex = vertexBrPool.alloc();
	*vertex = SRDAGVertexBroadcast(vertexIxCount++, this, parent, refIx, itrIx);
	vertices.add(vertex);
	return vertex;
}

inline SRDAGVertexInitEnd* SRDAGGraph::createVertexIn(
		SRDAGVertexAbstract* parent,
		int refIx,
		int itrIx){
	SRDAGVertexInitEnd* vertex = vertexIEPool.alloc();
	*vertex = SRDAGVertexInitEnd(vertexIxCount++, this, Init, parent, refIx, itrIx);
	vertices.add(vertex);
	return vertex;
}

inline SRDAGVertexInitEnd* SRDAGGraph::createVertexEn(
		SRDAGVertexAbstract* parent,
		int refIx,
		int itrIx){
	SRDAGVertexInitEnd* vertex = vertexIEPool.alloc();
	*vertex = SRDAGVertexInitEnd(vertexIxCount++, this, End, parent, refIx, itrIx);
	vertices.add(vertex);
	return vertex;
}

inline SRDAGVertexRB* SRDAGGraph::createVertexRB(
		SRDAGVertexAbstract* parent,
		int refIx,
		int itrIx,
		PiSDFAbstractVertex* ref){
	SRDAGVertexRB* vertex = vertexRBPool.alloc();
	*vertex = SRDAGVertexRB(vertexIxCount++, this, parent, refIx, itrIx, ref);
	vertices.add(vertex);
	rbs.add(vertex);
	return vertex;
}

inline SRDAGVertexXplode* SRDAGGraph::createVertexIm(
		SRDAGVertexAbstract* parent,
		int refIx,
		int itrIx){
	SRDAGVertexXplode* vertex = vertexXpPool.alloc();
	*vertex = SRDAGVertexXplode(vertexIxCount++, this, Implode, parent, refIx, itrIx);
	vertices.add(vertex);
	implodes.add(vertex);
	return vertex;
}

inline SRDAGVertexXplode* SRDAGGraph::createVertexEx(
		SRDAGVertexAbstract* parent,
		int refIx,
		int itrIx){
	SRDAGVertexXplode* vertex = vertexXpPool.alloc();
	*vertex = SRDAGVertexXplode(vertexIxCount++, this, Explode, parent, refIx, itrIx);
	vertices.add(vertex);
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
SRDAGVertexAbstract* SRDAGGraph::getNextHierVertex(){
	vertexSetIterator iter = vertices.getIterator();
	SRDAGVertexAbstract* vertex;
	while((vertex = iter.next()) != NULL){
		if(vertex->isHierarchical() &&
				vertex->getState() == SRDAG_Executable)
			return vertex;
	}
	return (SRDAGVertexAbstract*)NULL;
}

inline vertexSetIterator SRDAGGraph::getVertexIterator(){
	return vertices.getIterator();
}

inline SetIterator<SRDAGVertexXplode,MAX_SRDAG_VERTICES> SRDAGGraph::getImplodeIterator(){
	return implodes.getIterator();
}

inline SetIterator<SRDAGVertexRB,MAX_SRDAG_VERTICES> SRDAGGraph::getRBIterator(){
	return rbs.getIterator();
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

inline int SRDAGGraph::getVerticesFromReference(PiSDFAbstractVertex* ref, int iteration, SRDAGVertexAbstract** output){
	int size = 0;
	vertexSetIterator iter = vertices.getIterator();
	SRDAGVertexAbstract* vertex;
	while((vertex = iter.next()) != NULL){
		if(vertex->getIterationIndex() == iteration){
			if(vertex->getType() == ConfigureActor){
				if(((SRDAGVertexConfig*)vertex)->getReference() == ref){
					output[size] = vertex;
					size++;
				}
			}else if (vertex->getType() == Normal){
				if(((SRDAGVertexNormal*)vertex)->getReference() == ref){
					output[size] = vertex;
					size++;
				}
			}else if(vertex->getType() == RoundBuffer){
				if(((SRDAGVertexRB*)vertex)->getReference() == ref){
					output[size] = vertex;
					size++;
				}
			}
		}
	}
	return size;
}

inline int SRDAGGraph::getNbVertices(){
	return vertices.getNb();
}

inline int SRDAGGraph::getNbEdges(){
	return edges.getNb();
}

inline void SRDAGGraph::updateExecuted(){
	vertexSetIterator iter = vertices.getIterator();
	SRDAGVertexAbstract* vertex;
	while((vertex = iter.next()) != NULL){
		if(vertex->getState() == SRDAG_Executable)
			vertex->setState(SRDAG_Executed);
	}
}


#endif
