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
#include <graphs/AbstractGraph/AbstractVertex.h>

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

typedef IndexedArray<SRDAGEdge*, MAX_SRDAG_IO_EDGES> edgeArray;

class SRDAGVertex{

private :
	int id;
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

	edgeArray outputEdges;
	edgeArray inputEdges;

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

	UINT32 execTime[MAX_SLAVE_TYPES];
	bool constraint[MAX_SLAVE_TYPES];

	int minStartTime;

	// Identifies the function implementing the actor's action(s).
	UINT64 functIx;
public :
	/** Constructor */
	SRDAGVertex();

	/** Destructor */
	~SRDAGVertex();

	void reset();

	void setGraph(SRDAGGraph* graph);
	SRDAGGraph* getGraph();

	int getNbInputEdge();
	int getNbOutputEdge();

	int getId() const;

	int getParamValue(int paramIndex) const;
	void setParamValue(int paramIndex, int value);

	int getRelatedParamValue(int paramIndex) const;
	void setRelatedParamValue(int paramIndex, int value);

	int getReferenceIndex() const;
	void setReferenceIndex(int index);

	int getGlobalIndex() const;
	void setGlobalIndex(int index);

	int getIterationIndex() const;
	void setIterationIndex(int index);

	int getSchedLevel() const;
	void setSchedLevel(int level);

	int getScheduleIndex() const;
	void setScheduleIndex(int index);

	UINT32 getExecTime(int slaveType);
	void setExecTime(int slaveType, UINT32 exec);

	bool getConstraint(int slaveType);
	void setConstraint(int slaveType, bool constr);

	int getMinStartTime() const;
	void setMinStartTime(int time);

	SrVxTYPE getType() const;
	void setType(SrVxTYPE type);

	PiSDFAbstractVertex *getReference() const;
	void setReference(PiSDFAbstractVertex *Reference);

	void setEdgeReference(PiSDFEdge *Reference);
	PiSDFEdge* getEdgeReference() const;

	SrVxSTATUS_FLAG getState() const;
	void setState(SrVxSTATUS_FLAG state);

	UINT64 getFunctIx() const;
	void setFunctIx(UINT64 functIx);

	SRDAGEdge* getInputEdge(int id);
	SRDAGEdge* getOutputEdge(int id);

	void updateState();

	BOOL isHierarchical();
	PiSDFGraph* getHierarchy();

	void getName(char* name, UINT32 sizeMax);

	friend class SRDAGGraph;
	friend class SRDAGEdge;
};

inline int SRDAGVertex::getNbInputEdge(){
	return inputEdges.getNb();
}

inline int SRDAGVertex::getNbOutputEdge(){
	return outputEdges.getNb();
}

inline int SRDAGVertex::getId() const{
	return id;
}

inline
SRDAGGraph* SRDAGVertex::getGraph(){
	return graph;
}

inline
int SRDAGVertex::getParamValue(int paramIndex) const{
	return paramValues[paramIndex];
}

inline
void SRDAGVertex::setParamValue(int paramIndex, int value){
	paramValues[paramIndex] = value;
}

inline
int SRDAGVertex::getRelatedParamValue(int paramIndex) const{
	return relatedParamValues[paramIndex];
}

inline
void SRDAGVertex::setRelatedParamValue(int paramIndex, int value){
	relatedParamValues[paramIndex] = value;
}

inline
int SRDAGVertex::getReferenceIndex() const{
	return referenceIndex;
}

inline
void SRDAGVertex::setReferenceIndex(int index){
	referenceIndex = index;
}

inline
int SRDAGVertex::getIterationIndex() const{
	return iterationIndex;
}

inline
void SRDAGVertex::setIterationIndex(int index){
	iterationIndex = index;
}

inline
int SRDAGVertex::getSchedLevel() const{
	return schedLevel;
}

inline
void SRDAGVertex::setSchedLevel(int level){
	schedLevel = level;
}

inline
int SRDAGVertex::getScheduleIndex() const{
	return scheduleIndex;
}

inline
void SRDAGVertex::setScheduleIndex(int index){
	scheduleIndex = index;
}

inline
int SRDAGVertex::getExecTime() const{
	return execTime;
}

inline
void SRDAGVertex::setExecTime(int time){
	execTime = time;
}

inline
int SRDAGVertex::getMinStartTime() const{
	return minStartTime;
}

inline
void SRDAGVertex::setMinStartTime(int time){
	minStartTime = time;
}

inline
SrVxTYPE SRDAGVertex::getType() const{
	return type;
}

inline
void SRDAGVertex::setType(SrVxTYPE _type){
	type = _type;
}

inline
PiSDFAbstractVertex *SRDAGVertex::getReference() const{
	return Reference;
}

inline
void SRDAGVertex::setReference(PiSDFAbstractVertex *ref){
	Reference = ref;
	ref->addChildVertex(this);
}

inline
void SRDAGVertex::setOutputEdge(SRDAGEdge* edge, UINT32 id){
	outputEdges.add(edge,id);
}

inline
UINT32 SRDAGVertex::getExecTime(int slaveType){
	return execTime[slaveType];
}

inline
void SRDAGVertex::setExecTime(int slaveType, UINT32 exec){
	execTime[slaveType] = exec;
}

inline 
bool SRDAGVertex::getConstraint(int slaveType){
	return constraints[slaveType];
}

inline 
void SRDAGVertex::setConstraint(int slaveType, bool constr){
	constraints[slaveType] = constr;
}

inline
PiSDFEdge* SRDAGVertex::getEdgeReference() const{
	return EdgeReference;
}

inline
void SRDAGVertex::setEdgeReference(PiSDFEdge *ref){
	EdgeReference = ref;
}

inline
SrVxSTATUS_FLAG SRDAGVertex::getState() const{
	return state;
}

inline
void SRDAGVertex::setState(SrVxSTATUS_FLAG _state){
	state = _state;
}

inline
UINT64 SRDAGVertex::getFunctIx() const{
	return functIx;
}

inline
void SRDAGVertex::setFunctIx(UINT64 fIx){
	functIx = fIx;
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
BOOL SRDAGVertex::isHierarchical(){
	return Reference
			&& Reference->getType() == pisdf_vertex
			&& ((PiSDFVertex*)Reference)->hasSubGraph()
			&& type == Normal;
}

inline
PiSDFGraph* SRDAGVertex::getHierarchy(){
	return ((PiSDFVertex*)Reference)->getSubGraph();
}

#endif
