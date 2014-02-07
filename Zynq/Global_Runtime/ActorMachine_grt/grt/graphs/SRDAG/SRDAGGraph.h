
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/

#ifndef SRDAG_GRAPH
#define SRDAG_GRAPH

#include <types.h>
#include "SRDAGVertex.h"
#include "SRDAGEdge.h"
#include "../Base/BaseVertex.h"

#include <grt_definitions.h>
#include "../../tools/SchedulingError.h"

/**
 * A SRDAG graph. It contains SRDAG vertices and edges. It has a bigger table for vertices and edges than DAG.
 * Each edge production and consumption must be equal. There is no repetition vector for the vertices.
 * 
 * @author mpelcat
 */
class SRDAGGraph {

	private :
		/**
		 number of SRDAG vertices
		*/
		int nbVertices;

		/**
		 table of SRDAG vertices
		*/
		SRDAGVertex vertices[MAX_SRDAG_VERTICES];

		/**
		 number of SRDAG edges
		*/
		int nbEdges;

		/**
		 table of SRDAG edges
		*/
		SRDAGEdge edges[MAX_SRDAG_EDGES];

	public : 
		/**
		 Constructor
		*/
		SRDAGGraph();

		/**
		 Destructor
		*/
		~SRDAGGraph();

		/**
		 Adding a vertex to the graph. Vertices and edges must be added in topological order.
		 There is no initial token on edges
		 
		 @return the new vertex
		*/
		SRDAGVertex* addVertex();

		/**
		 Adding an edge to the graph. Vertices and edges must be added in topological order.
		 There is no initial token on edges
		 
		 @param source: The source vertex of the edge
		 @param tokenRate: number of tokens (chars) produced by the source and consumed by the sink
		 @param sink: The sink vertex of the edge
		 @return the created edge
		*/
		SRDAGEdge* addEdge(SRDAGVertex* source, int tokenRate, SRDAGVertex* sink, BaseEdge* refEdge = NULL);


		void appendAnnex(SRDAGGraph* annex);


		/**
		 Removes the last added edge
		*/
		void removeLastEdge();

		SRDAGVertex* findMatch(BaseVertex* refVx);

		SRDAGVertex* findUnplugIF(VERTEX_TYPE ifType);

		SRDAGVertex* findUnplugRB();




		/**
		 Removes all edges and vertices
		*/
		void flush();

		/**
		 Gets the actor at the given index
		 
		 @param index: index of the actor in the actor list
		 @return actor
		*/
		SRDAGVertex* getVertex(int index);

		/**
		 Gets the srDag vertices that share the given DAG reference
		 
		 @param ref: the DAG reference
		 @param output: the output SRDAG vertices
		 @return the number of found references
		*/
		int getVerticesFromCSDAGReference(CSDAGVertex* ref, SRDAGVertex** output);

		UINT32 getVerticesFromReference(BaseVertex* ref, SRDAGVertex** output);

		/**
		 Gets the index of the given actor
		 
		 @param vertex: actor vertex
		 @return index of the actor in the actor list
		*/
		int getVertexIndex(SRDAGVertex* vertex);

		/**
		 Gets the index of the given edge
		 
		 @param edge: edge
		 @return index of the edge in the edge list
		*/
		int getEdgeIndex(SRDAGEdge* edge);

		/**
		 Gets the actor number
		 
		 @return number of vertices
		*/
		int getNbVertices();

		/**
		 Gets the edge at the given index
		 
		 @param index: index of the edge in the edge list
		 @return edge
		*/
		SRDAGEdge* getEdge(int index);

		/**
		 Gets the edge number
		 
		 @return number of edges
		*/
		int getNbEdges();

		/**
		 Reset the "visited" status of all vertices
		*/
		void resetVisited();

		/**
		 Gets the input edges of a given vertex. Careful!! Slow!
		 
		 @param vertex: input vertex
		 @param output: table to store the edges
		 @return the number of input edges
		*/
		int getInputEdges(SRDAGVertex* vertex, SRDAGEdge** output);

		/**
		 Gets the output edges of a given vertex. Careful!! Slow!
		 
		 @param vertex: input vertex
		 @param output: table to store the edges
		 @return the number of output edges
		*/
		int getOutputEdges(SRDAGVertex* vertex, SRDAGEdge** output);

		int getMaxTime();

		int getCriticalPath();

		SRDAGEdge* getEdgeByRef(SRDAGVertex* srDagVx, BaseEdge* refEdge, VERTEX_TYPE inOut);

		SRDAGVertex* getVxByRefAndIx(BaseVertex* reference, int refIndex);

//		CSDAGVertex* getExplodeVertex();


		/*
		 * TODO: comments..
		 */
		void merge(SRDAGGraph* localDag);

		void removeVx(SRDAGVertex* Vx);

		/**
		 Gets pointers on the edges of the graph in the order of their source or sink.
		 Accelerates getting the input or output edges

		 @param sourceOrSink: 1 for sorting in source order, 0 for sink order
		*/
		void sortEdges(int startIndex);
};

/**
 Gets the actor at the given index
 
 @param index: index of the actor in the actor list
 @return actor
*/
inline SRDAGVertex* SRDAGGraph::getVertex(int index){
	return &vertices[index];
}

/**
 Gets the index of the given actor
 
 @param vertex: actor vertex
 @return index of the actor in the actor list
*/
inline int SRDAGGraph::getVertexIndex(SRDAGVertex* vertex){
	int index;
	for(index=0;index<nbVertices;index++){
		if(vertex == &vertices[index]){
			return index;
		}
	}
	return -1;
}

/**
 Gets the index of the given edge
 
 @param edge: edge
 @return index of the edge in the edge list
*/
inline int SRDAGGraph::getEdgeIndex(SRDAGEdge* edge){
	int index;
	for(index=0;index<nbEdges;index++){
		if(edge == &edges[index]){
			return index;
		}
	}
	return -1;
}

/**
 Gets the actor number
 
 @return number of vertices
*/
inline int SRDAGGraph::getNbVertices(){
	return nbVertices;
}

/**
 Gets the edge at the given index
 
 @param index: index of the edge in the edge list
 @return edge
*/
inline SRDAGEdge* SRDAGGraph::getEdge(int index){
	return &edges[index];
}

/**
 Gets the edge number
 
 @return number of edges
*/
inline int SRDAGGraph::getNbEdges(){
	return nbEdges;
}

/**
 Reset the "visited" status of all vertices
*/
inline void SRDAGGraph::resetVisited(){
	for(int i = 0; i < nbVertices; i++)
		vertices[i].setVisited(0);
}

/**
 Gets the srDag vertices that share the given DAG reference
 
 @param ref: the DAG reference
 @param output: the output SRDAG vertices
 @return the number of found references
*/
inline int SRDAGGraph::getVerticesFromCSDAGReference(CSDAGVertex* ref, SRDAGVertex** output){
	int size = 0;
	for(int i=0; i<nbVertices; i++){
		SRDAGVertex* vertex = &vertices[i];
		if(vertex->getCsDagReference() == ref){
			output[size] = vertex;
			size++;
		}
	}
	return size;
}


inline UINT32 SRDAGGraph::getVerticesFromReference(BaseVertex* ref, SRDAGVertex** output){
	UINT32 size = 0;
	for(int i=0; i<nbVertices; i++){
		SRDAGVertex* vertex = &vertices[i];
		if(vertex->getReference() == ref){
			output[size] = vertex;
			size++;
		}
	}
	return size;
}

/**
 Gets the input edges of a given vertex. Careful!! Slow!
 
 @param vertex: input vertex
 @param output: table to store the edges
 @return the number of input edges
*/
inline int SRDAGGraph::getInputEdges(SRDAGVertex* vertex, SRDAGEdge** output){
	int nbEdges = 0;
	for(int i=0; i<this->nbEdges; i++){
		SRDAGEdge* edge = &edges[i];
		SRDAGVertex* sink = edge->getSink();
		if(sink == vertex){
			output[nbEdges] = edge;
			nbEdges++;
		}
	}
	return nbEdges;
}

/**
 Gets the output edges of a given vertex. Careful!! Slow!
 
 @param vertex: input vertex
 @param output: table to store the edges
 @return the number of output edges
*/
inline int SRDAGGraph::getOutputEdges(SRDAGVertex* vertex, SRDAGEdge** output){
	int nbEdges = 0;
	for(int i=0; i<this->nbEdges; i++){
		SRDAGEdge* edge = &edges[i];
		SRDAGVertex* source = edge->getSource();
		if(source == vertex){
			output[nbEdges] = edge;
			nbEdges++;
		}
	}
	return nbEdges;
}


inline SRDAGVertex* SRDAGGraph::getVxByRefAndIx(BaseVertex* reference, int refIndex){
	for(int i=0; i< nbVertices; i++){
		if((vertices[i].getReference() == reference) &&
		  (vertices[i].getReferenceIndex() == refIndex))
			return &vertices[i];
	}
	return (SRDAGVertex*)0;
}



/**
 Adding a vertex to the graph
 
 @param vertexName: the name of the new vertex

 @return the new vertex
*/
inline SRDAGVertex* SRDAGGraph::addVertex(){
	SRDAGVertex* vertex;
#ifdef _DEBUG
	if(nbVertices >= MAX_SRDAG_VERTICES){
		// Adding a vertex while the graph is already full
		exitWithCode(1000);
	}
#endif
	vertex = &vertices[nbVertices];
	vertex->setBase(this);
	vertex->setId(nbVertices);
	nbVertices++;
	return vertex;
}


inline SRDAGVertex* SRDAGGraph::findMatch(BaseVertex* refVx){
	for (int i = 0; i < nbVertices; i++) {
		if((vertices[i].getNbInputEdge() == 0) &&
		   (vertices[i].getReference() == refVx)){
			return &vertices[i];
		}
	}
	return (SRDAGVertex*)0;
}

inline SRDAGVertex* SRDAGGraph::findUnplugIF(VERTEX_TYPE ifType){
	for (int i = 0; i < nbVertices; i++) {
		if((vertices[i].getNbInputEdge() == 0) &&
		   (vertices[i].getReference()->getType() == input_vertex)){
			return &vertices[i];
		}
	}
	return (SRDAGVertex*)0;
}

inline SRDAGVertex* SRDAGGraph::findUnplugRB(){
	for (int i = 0; i < nbVertices; i++) {
		if((vertices[i].getNbOutputEdge() == 0) &&
		   (vertices[i].getReference()->getType() == roundBuff_vertex)){
			return &vertices[i];
		}
	}
	return (SRDAGVertex*)0;
}




#endif
