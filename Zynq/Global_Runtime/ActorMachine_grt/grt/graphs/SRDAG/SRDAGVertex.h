/*********************************************************
Copyright or � or Copr. IETR/INSA: Maxime Pelcat

Contact mpelcat for more information:
mpelcat@insa-rennes.fr

This software is a computer program whose purpose is to execute
parallel applications.

 *********************************************************/
 
#ifndef SRDAG_VERTEX
#define SRDAG_VERTEX

class CSDAGVertex;
class SRDAGGraph;
class SRDAGEdge;
#include <cstring>

#include "../../SchedulerDimensions.h"
#include "../../tools/SchedulingError.h"
#include "../CSDAG/CSDAGVertex.h"

/**
 * A vertex in a SRDAG graph
 * 
 * @author mpelcat
 */

class SRDAGVertex {

	private :
		/**
		 The base, i.e. the graph in which current vertex is included
		*/
		SRDAGGraph* base;
		
		/**
		 Integer solved parameters. Retrieved while solving the edges
		*/
		int paramValues[MAX_PARAM];

		/**
		 The reference DAG graph (if generated from a CSDAG)
		*/
		CSDAGVertex* csDagReference;

		/**
		 The vertex top level
		*/
		int tLevel;

		/**
		 The vertex implementation slave index
		*/
		int slaveIndex;

		/**
		 The duplication index of the vertex to distinguish it from other vertices created from dagReference.
		*/
		int referenceIndex;

		SRDAGEdge *outputEdges[MAX_SRDAG_OUTPUT_EDGES];
		SRDAGEdge *inputEdges[MAX_SRDAG_INPUT_EDGES];

		int nbInputEdges;
		int nbOutputEdges;

		/**
		 For graph traversal
		*/
		bool visited;
	public : 
		/**
		 Constructor
		*/
		SRDAGVertex();

		/**
		 Destructor
		*/
		~SRDAGVertex();

		/**
		 Setting the base, i.e. the graph in which current vertex is included

		 @param base: the base
		*/
		void setBase(SRDAGGraph* graph);

		/**
		 Getting the base, i.e. the graph in which current vertex is included

		 @param base: the base
		*/
		SRDAGGraph* getBase();

		/**
		 Getting the value of a parameter

		 @param paramIndex: the parameter index
		 @return the parameter value
		*/
		int getParamValue(int paramIndex);

		/**
		 Setting the value of a parameter

		 @param paramIndex: the parameter index
		 @param value: the parameter value
		*/
		void setParamValue(int paramIndex, int value);

		/**
		 Getting the CSDAG vertex that generated the current SRDAG vertex

		 @return the CSDAG reference vertex
		*/
		CSDAGVertex* getCsDagReference();

		/**
		 Setting the value of a parameter

		 @param:vertex the CSDAG reference vertex
		*/
		void setCsDagReference(CSDAGVertex* vertex);

		/**
		 Getting the duplication index of the vertex that distinguishes
		 it from other vertices created from dagReference.

		 @return the CSDAG reference index
		*/
		int getReferenceIndex();

		int getGlobalIndex();

		/**
		 Setting the duplication index of the vertex to distinguish 
		 it from other vertices created from dagReference.

		 @param index: the vertex reference index
		*/
		void setReferenceIndex(int index);

		void setGlobalIndex(int index);

		/**
		 Getting the 'visited' status of the vertex

		 @return the status
		*/
		bool getVisited();

		/**
		 Setting the 'visited' status of the vertex

		 @param value: the status
		*/
		void setVisited(bool value);

		/**
		 Getter of the implementation information giving the slave that will execute the vertex.

		 @return the slave index
		*/
		int getSlaveIndex();

		/**
		 Setter of the implementation information giving the slave that will execute the vertex.

		 @param slaveIndex: the slave index
		*/
		void setSlaveIndex(int slaveIndex);

		/**
		 Getter of the vertex top level: the time between the loop execution beginning and the vertex execution beginning.

		 @return the t level of the vertex
		*/
		int getTLevel();

		/**
		 Setter of the vertex top level: the time between the loop execution beginning and the vertex execution beginning.

		 @param value: the t level of the vertex
		*/
		void setTLevel(int value);


		void addInputEdge(SRDAGEdge* edge);
		void addOutputEdge(SRDAGEdge* edge);

		SRDAGEdge* getInputEdge(int id);
		SRDAGEdge* getOutputEdge(int id);

		int getNbInputEdge();
		int getNbOutputEdge();
};


/**
 Setting the base, i.e. the graph in which current vertex is included

 @param base: the base
*/
inline
void SRDAGVertex::setBase(SRDAGGraph* graph){
	this->base = graph;
}

/**
 Getting the base, i.e. the graph in which current vertex is included

 @param base: the base
*/
inline
SRDAGGraph* SRDAGVertex::getBase(){
	return this->base;
}

/**
 Getting the value of a parameter

 @param paramIndex: the parameter index
 @return the parameter value
*/
inline
int SRDAGVertex::getParamValue(int paramIndex){
	return paramValues[paramIndex];
}

/**
 Setting the value of a parameter

 @param paramIndex: the parameter index
 @param value: the parameter value
*/
inline
void SRDAGVertex::setParamValue(int paramIndex, int value){
	paramValues[paramIndex] = value;
}

/**
 Getting the CSDAG vertex that generated the current SRDAG vertex

 @return the CSDAG reference vertex
*/
inline
CSDAGVertex* SRDAGVertex::getCsDagReference(){
	return csDagReference;
}

/**
 Setting the value of a parameter

 @param vertex: the CSDAG reference vertex
*/
inline
void SRDAGVertex::setCsDagReference(CSDAGVertex* vertex){
	csDagReference = vertex;
}

/**
 Getting the duplication index of the vertex that distinguishes
 it from other vertices created from dagReference.

 @return the CSDAG reference index
*/
inline
int SRDAGVertex::getReferenceIndex(){
	return referenceIndex;
}

/**
 Setting the duplication index of the vertex to distinguish 
 it from other vertices created from dagReference.

 @param index: the vertex reference index
*/
inline
void SRDAGVertex::setReferenceIndex(int index){
	referenceIndex = index;
}


/**
 Getter of the implementation information giving the slave that will execute the vertex.

 @return the slave index
*/
inline
int SRDAGVertex::getSlaveIndex(){
	return slaveIndex;
}

/**
 Setter of the implementation information giving the slave that will execute the vertex.

 @param slaveIndex: the slave index
*/

inline
void SRDAGVertex::setSlaveIndex(int index){
	slaveIndex = index;
}

/**
 Getter of the vertex top level: the time between the loop execution beginning and the vertex execution beginning.

 @return the t level of the vertex
*/
inline
int SRDAGVertex::getTLevel(){
	return tLevel;
}

/**
 Setter of the vertex top level: the time between the loop execution beginning and the vertex execution beginning.

 @param value: the t level of the vertex
*/
inline
void SRDAGVertex::setTLevel(int value){
	tLevel = value;
}

/**
 Getting the 'visited' status of the vertex

 @return the status
*/
inline
bool SRDAGVertex::getVisited(){
	return visited;
}

/**
 Setting the 'visited' status of the vertex

 @param value: the status
*/
inline
void SRDAGVertex::setVisited(bool value){
	visited = value;
}

inline
void SRDAGVertex::addInputEdge(SRDAGEdge* edge){
	if(nbInputEdges > MAX_SRDAG_INPUT_EDGES){
		exitWithCode(1047);
	}
	inputEdges[nbInputEdges++] = edge;
}

inline
void SRDAGVertex::addOutputEdge(SRDAGEdge* edge){
	if(nbOutputEdges > MAX_SRDAG_OUTPUT_EDGES){
		exitWithCode(1048);
	}
	outputEdges[nbOutputEdges++] = edge;
}

inline
SRDAGEdge* SRDAGVertex::getInputEdge(int id){
	return inputEdges[id];
}

inline
SRDAGEdge* SRDAGVertex::getOutputEdge(int id){
	return outputEdges[id];
}

inline
int SRDAGVertex::getNbInputEdge(){
	return nbInputEdges;
}

inline
int SRDAGVertex::getNbOutputEdge(){
	return nbOutputEdges;
}

#endif
