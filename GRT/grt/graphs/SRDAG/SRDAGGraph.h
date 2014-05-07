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
#include <tools/List.h>

/**
 * A SRDAG graph. It contains SRDAG vertices and edges. It has a bigger table for vertices and edges than DAG.
 * Each edge production and consumption must be equal. There is no repetition vector for the vertices.
 * 
 * @author mpelcat
 */
class SRDAGGraph {

	private :
		List<SRDAGVertex,MAX_SRDAG_VERTICES> vertices;
//		List<SRDAGVertex*,MAX_SRDAG_VERTICES> hierVertex;
		List<SRDAGEdge,MAX_SRDAG_EDGES> edges;

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
		 * Reset the graph as at initialization
		 */
		void reset();

		/**
		 Adding a vertex to the graph. Vertices and edges must be added in topological order.
		 There is no initial token on edges
		 
		 @return the new vertex
		*/
		SRDAGVertex* addVertex();

		void storeHierVertex(SRDAGVertex* vertex){
//			hierVertex.add(vertex);
		}
		SRDAGVertex* getExHierVertex(){
//			for(UINT32 i=0; i<hierVertex.getNb(); i++){
//				if(hierVertex[i]->getState() == SrVxStExecutable)
//					return hierVertex[i];
//			}
			for(UINT32 i=0; i<vertices.getNb(); i++){
				if(vertices[i].isHierarchical() &&
						vertices[i].getState() == SrVxStExecutable)
					return &(vertices[i]);
			}
			return (SRDAGVertex*)NULL;
		}

		/**
		 Adding an edge to the graph. Vertices and edges must be added in topological order.
		 There is no initial token on edges
		 
		 @param source: The source vertex of the edge
		 @param tokenRate: number of tokens (chars) produced by the source and consumed by the sink
		 @param sink: The sink vertex of the edge
		 @return the created edge
		*/
		SRDAGEdge* addEdge(SRDAGVertex* source, UINT32 sourcePortIx, int tokenRate, SRDAGVertex* sink, UINT32 sinkPortIx, PiSDFEdge* refEdge = (PiSDFEdge*)NULL);


		void appendAnnex(SRDAGGraph* annex);


		/**
		 Removes the last added edge
		*/
		void removeLastEdge();

		void changeEdgeSource(SRDAGEdge* edge, SRDAGVertex* vertex);
		void changeEdgeSink(SRDAGEdge* edge, SRDAGVertex* vertex);



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
		UINT32 getVerticesFromReference(PiSDFAbstractVertex* ref, UINT32 iteration, SRDAGVertex** output);

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
		UINT32 getNbVertices();

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

		SRDAGEdge* getEdgeByRef(SRDAGVertex* srDagVx, PiSDFEdge* refEdge, VERTEX_TYPE inOut);

		SRDAGVertex* getVxByRefAndIx(PiSDFAbstractVertex* reference, int refIndex);

//		CSDAGVertex* getExplodeVertex();

		void merge(SRDAGGraph* localDag, bool intraLevel, UINT32 level, UINT8 step);

		void removeVx(SRDAGVertex* Vx);

		/**
		 Gets pointers on the edges of the graph in the order of their source or sink.
		 Accelerates getting the input or output edges

		 @param sourceOrSink: 1 for sorting in source order, 0 for sink order
		*/
		void sortEdges(int startIndex);

		/*
		 * Sets all the executable vx' states to executed.
		 */
		void updateExecuted();
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
	return vertices.getId(vertex);
}

/**
 Gets the index of the given edge
 
 @param edge: edge
 @return index of the edge in the edge list
*/
inline int SRDAGGraph::getEdgeIndex(SRDAGEdge* edge){
	return edges.getId(edge);
}

/**
 Gets the actor number
 
 @return number of vertices
*/
inline UINT32 SRDAGGraph::getNbVertices(){
	return vertices.getNb();
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
	return edges.getNb();
}

/**
 Reset the "visited" status of all vertices
*/
inline void SRDAGGraph::resetVisited(){
	for(int i = 0; i < vertices.getNb(); i++)
		vertices[i].setVisited(0);
}

/**
 Gets the srDag vertices that share the given DAG reference
 
 @param ref: the DAG reference
 @param output: the output SRDAG vertices
 @return the number of found references
*/
inline UINT32 SRDAGGraph::getVerticesFromReference(PiSDFAbstractVertex* ref, UINT32 iteration, SRDAGVertex** output){
	UINT32 size = 0;
	for(int i=0; i<vertices.getNb(); i++){
		SRDAGVertex* vertex = &vertices[i];
		if(vertex->getReference() == ref && vertex->getIterationIndex() == iteration){
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
	for(int i=0; i<edges.getNb(); i++){
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
	for(int i=0; i<edges.getNb(); i++){
		SRDAGEdge* edge = &edges[i];
		SRDAGVertex* source = edge->getSource();
		if(source == vertex){
			output[nbEdges] = edge;
			nbEdges++;
		}
	}
	return nbEdges;
}


inline SRDAGVertex* SRDAGGraph::getVxByRefAndIx(PiSDFAbstractVertex* reference, int refIndex){
	for(int i=0; i< vertices.getNb(); i++){
		if((vertices[i].getReference() == reference) &&
		   (vertices[i].getState() != SrVxStDeleted) &&
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
	vertex = vertices.add();
	vertex->reset();
	vertex->setGraph(this);
	vertex->setId(vertices.getId(vertex));
	return vertex;
}

inline void SRDAGGraph::updateExecuted(){
	for (int i = 0; i < vertices.getNb(); i++) {
		if(vertices[i].getState() == SrVxStExecutable) vertices[i].setState(SrVxStExecuted);
	}
}


#endif
