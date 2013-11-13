
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

#ifndef SRDAG_VERTEX
#define SRDAG_VERTEX

class DAGVertex;
//class CSDAGVertex;
class SRDAGGraph;
class SRDAGEdge;
#include <cstring>

#include "../../SchedulerDimensions.h"
#include "../../tools/SchedulingError.h"
#include "../CSDAG/CSDAGVertex.h"
#include "../Base/BaseVertex.h"

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
		 The reference PiSDF vertex (if generated from a PiSDF)
		*/
		BaseVertex* Reference;

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

		// The index of the vertex in a schedule. -1 if the vertex have not been scheduled.
		int scheduleIndex;

		/**
		 For graph traversal
		*/
		bool visited;

		// Indicates the type of vertex normal (0), explode (1) or implode (2). Normal (0) by default.
		int type;

		// Distinguishes among several explode/implode vertices.
		int expImpId;

		UINT32 id;
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


	    int getScheduleIndex() const
	    {
	        return scheduleIndex;
	    }

	    void setScheduleIndex(int scheduleIndex)
	    {
	        this->scheduleIndex = scheduleIndex;
	    }


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

		/*
		 * Getter and setter for the type.
		 */
		int getType();
		void setType(int type);

		/*
		 * Getter and setter for the expImpId.
		 */
		int getExpImpId();
		void setExpImpId(int id);

	    BaseVertex *getReference() const
	    {
	        return Reference;
	    }

	    void setReference(BaseVertex *Reference)
	    {
	        this->Reference = Reference;
	    }


	    UINT32 getId() const
	    {
	        return id;
	    }

	    void setId(UINT32 id)
	    {
	        this->id = id;
	    }
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

inline
int SRDAGVertex::getType(){
	return type;
}

inline
void SRDAGVertex::setType(int type){
	this->type = type;
}

inline
int SRDAGVertex::getExpImpId(){
	return expImpId;
}

inline
void SRDAGVertex::setExpImpId(int id){
	this->expImpId = id;
}
#endif
