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

#ifndef SRDAG_VERTEX
#define SRDAG_VERTEX

class DAGVertex;
//class CSDAGVertex;
class SRDAGGraph;
//class SRDAGEdge;
#include <cstring>

#include <grt_definitions.h>
#include "../../tools/SchedulingError.h"
#include "../PiSDF/PiSDFAbstractVertex.h"
#include "../PiSDF/PiSDFVertex.h"
#include "SRDAGEdge.h"
#include <tools/Array.h>

typedef enum{
	SrVxStExecutable,
	SrVxStExecuted,
	SrVxStNoExecuted,
	SrVxStDeleted
}SrVxSTATUS_FLAG;


typedef enum{
	Normal,
	ConfigureActor,
	Explode,
	Implode,
	RoundBuffer,
	Broadcast,
	Init,
	End
}SrVxTYPE;

/**
 * A vertex in a SRDAG graph
 * 
 * @author mpelcat
 */

class SRDAGVertex {

	private :
		/**
		 The PiSDF, i.e. the graph in which current vertex is included
		*/
		SRDAGGraph* graph;
		
		/**
		 Integer solved parameters. Retrieved while solving the edges
		*/
		int paramValues[MAX_PARAM];

		int relatedParamValues[MAX_PARAM];

		/**
		 The reference PiSDF vertex (if generated from a PiSDF)
		*/
		PiSDFAbstractVertex* Reference;

		/**
		 * Edge Reference (for round buffer)
		 */
		PiSDFEdge* EdgeReference;

		/**
		 * Reference to the parent vertex.
		 */
		SRDAGVertex* parent;

		/**
		 The vertex top level
		*/
		int schedLevel;

		/**
		 The vertex implementation slave index
		*/
		int slaveIndex;

		/**
		 The duplication index of the vertex to distinguish it from other vertices created from dagReference.
		*/
		int referenceIndex;
		int iterationIndex;

		Array<SRDAGEdge*,MAX_SRDAG_OUTPUT_EDGES> outputEdges;
		Array<SRDAGEdge*,MAX_SRDAG_INPUT_EDGES> inputEdges;

		// The index of the vertex in a schedule. -1 if the vertex have not been scheduled.
		int scheduleIndex;

		SrVxSTATUS_FLAG state;

		/**
		 For graph traversal
		*/
		bool visited;

		int mergeIx;	// Index in the merged graph.

		// Indicates the type of vertex normal (0), explode (1) or implode (2). Normal (0) by default.
		SrVxTYPE type;

		// Distinguishes among several explode/implode vertices.
		int expImpId;

		UINT32 execTime;
		int minStartTime;

		UINT32 id;

//		char name[MAX_VERTEX_NAME_SIZE];

		// Identifies the function implementing the actor's action(s).
		UINT64 functIx;
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
		 Setting the PiSDF, i.e. the graph in which current vertex is included

		 @param PiSDF: the PiSDF
		*/
		void setGraph(SRDAGGraph* graph);

		void reset();

		/**
		 Getting the PiSDF, i.e. the graph in which current vertex is included

		 @param PiSDF: the PiSDF
		*/
		SRDAGGraph* getGraph();

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

		int getRelatedParamValue(int paramIndex);
		void setRelatedParamValue(int paramIndex, int value);

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

		void setIterationIndex(int index){
			iterationIndex = index;
		}

		UINT32 getIterationIndex(){
			return iterationIndex;
		}


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

		int getSchedLevel();

		void setExecTime(UINT32 exec){
			execTime = exec;
		}

		UINT32 getExecTime(){
			return execTime;
		}

		void setMinStartTime(int t){
			minStartTime = t;
		}

		int getMinStartTime(){
			return minStartTime;
		}

		void setSchedLevel(int value);


		SRDAGEdge* getInputEdge(int id);
		SRDAGEdge* getOutputEdge(int id);

		UINT32 getNbInputEdge();
		UINT32 getNbOutputEdge();

		/*
		 * Getter and setter for the type.
		 */
		SrVxTYPE getType();
		void setType(SrVxTYPE type);

		/*
		 * Getter and setter for the expImpId.
		 */
		UINT32 getExpImpId();
		void setExpImpId(UINT32 id);

	    PiSDFAbstractVertex *getReference() const
	    {
	        return Reference;
	    }

	    void setReference(PiSDFAbstractVertex *Reference)
	    {
	        this->Reference = Reference;
	        Reference->addChildVertex(this);
	    }

	    void setEdgeReference(PiSDFEdge *Reference)
		{
			this->EdgeReference = Reference;
		}


	    UINT32 getId() const
	    {
	        return id;
	    }

	    void setId(UINT32 id)
	    {
	        this->id = id;
	    }

	    SrVxSTATUS_FLAG getState() const
	    {
	        return state;
	    }

	    void setState(SrVxSTATUS_FLAG state)
	    {
	        this->state = state;
	    }

	    int getMergeIx() const
	    {
	        return mergeIx;
	    }

	    void setMergeIx(int ix)
	    {
	        this->mergeIx = ix;
	    }

	    SRDAGVertex *getParent() const
	    {
	        return parent;
	    }

	    void setParent(SRDAGVertex *parent)
	    {
	        this->parent = parent;
	    }

	    char* getName()
	    {
//	        return name;
	    	return "";
	    }

	    void setName(char* name)
	    {
//	    	strcpy(this->name,name);
	    }

		UINT64 getFunctIx() const {
			return functIx;
		}

		void setFunctIx(UINT64 functIx) {
			this->functIx = functIx;
		}


		void setInputEdge(SRDAGEdge* edge, UINT32 id);
		void setOutputEdge(SRDAGEdge* edge, UINT32 id);
	    bool checkForExecution();

	    void updateState();


		int getInputEdgeId(SRDAGEdge* edge);
		int getOutputEdgeId(SRDAGEdge* edge);

		void removeInputEdge(SRDAGEdge* edge);
		void removeOutputEdge(SRDAGEdge* edge);

		void removeInputEdgeIx(UINT32 ix);
		void removeOutputEdgeIx(UINT32 ix);

		BOOL isHierarchical(){
			return Reference
					&& Reference->getType() == pisdf_vertex
					&& ((PiSDFVertex*)Reference)->hasSubGraph()
					&& type == Normal;
		}

		PiSDFGraph* getHierarchy(){
			return ((PiSDFVertex*)Reference)->getSubGraph();
		}

		void getName(char* name, UINT32 sizeMax);


	PiSDFEdge* getEdgeReference(){
		return EdgeReference;
	}
};


/**
 Setting the PiSDF, i.e. the graph in which current vertex is included

 @param PiSDF: the PiSDF
*/
inline
void SRDAGVertex::setGraph(SRDAGGraph* graph){
	this->graph = graph;
}

/**
 Getting the PiSDF, i.e. the graph in which current vertex is included

 @param PiSDF: the PiSDF
*/
inline
SRDAGGraph* SRDAGVertex::getGraph(){
	return this->graph;
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

inline
int SRDAGVertex::getRelatedParamValue(int paramIndex){
	return relatedParamValues[paramIndex];
}

inline
void SRDAGVertex::setRelatedParamValue(int paramIndex, int value){
	relatedParamValues[paramIndex] = value;
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

inline
int SRDAGVertex::getSchedLevel(){
	return schedLevel;
}

inline
void SRDAGVertex::setSchedLevel(int value){
	schedLevel = value;
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
void SRDAGVertex::setInputEdge(SRDAGEdge* edge, UINT32 id){
	inputEdges.add(edge,id);
}

inline
void SRDAGVertex::setOutputEdge(SRDAGEdge* edge, UINT32 id){
	outputEdges.add(edge,id);
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
UINT32 SRDAGVertex::getNbInputEdge(){
	return inputEdges.getNb();
}

inline
UINT32 SRDAGVertex::getNbOutputEdge(){
	return outputEdges.getNb();
}

inline
SrVxTYPE SRDAGVertex::getType(){
	return type;
}

inline
void SRDAGVertex::setType(SrVxTYPE type){
	this->type = type;
}

inline
UINT32 SRDAGVertex::getExpImpId(){
	return expImpId;
}

inline
void SRDAGVertex::setExpImpId(UINT32 id){
	this->expImpId = id;
}

#endif
