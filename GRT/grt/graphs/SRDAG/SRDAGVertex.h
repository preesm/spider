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

#ifndef SRDAG_VERTEX_ABSTRACT
#define SRDAG_VERTEX_ABSTRACT

#include <cstring>
#include <cstdlib>

#include <grt_definitions.h>
#include <tools/SchedulingError.h>
#include <tools/FitedArray.h>

class SRDAGGraph;
class SRDAGEdge;
class SRDAGVertex;
class PiSDFGraph;
class PiSDFAbstractVertex;

typedef enum{
	SRDAG_Executable,
	SRDAG_Executed,
	SRDAG_NotExecuted
}SRDAGVertexState;


typedef enum{
	Normal,
	ConfigureActor,
	Explode,
	Implode,
	RoundBuffer,
	Broadcast,
	Init,
	End
}SRDAGVertexType;

class SRDAGVertex{
protected :
	int id;
	SRDAGGraph* graph;

	SRDAGVertexType		type;
	SRDAGVertexState 	state;

	PiSDFAbstractVertex* reference;
	int fctIx;

	int refIx;
	int itrIx;

	int schedLevel;
	int slaveIndex;
	UINT32 startTime;
	UINT32 endTime;

	void connectInputEdge(SRDAGEdge* edge, int ix);
	void connectOutputEdge(SRDAGEdge* edge, int ix);
	void disconnectInputEdge(int ix);
	void disconnectOutputEdge(int ix);

	static int creationIx;

	UINT32 execTime[MAX_SLAVE_TYPES];
	bool constraints[MAX_SLAVE_TYPES];

	int setIx;

	FitedArray<SRDAGEdge*,MAX_EDGE_ARRAY> inputs, outputs;
	FitedArray<int,MAX_PARAM_ARRAY> params;
	FitedArray<int,MAX_PARAM_ARRAY> relatedParamValues;

	bool haveSubGraph;
	PiSDFGraph* subGraph;
public:
	SRDAGVertex();
	SRDAGVertex(
				SRDAGGraph* 	_graph,
				SRDAGVertexType _type,
				PiSDFAbstractVertex* _ref,
				int 			_refIx,
				int 			_itrIx);

	virtual ~SRDAGVertex(){}

	int getId() const;
	SRDAGVertexType  getType()  const;
	SRDAGVertexState getState() const;
	void setState(SRDAGVertexState st);
	int getFctIx() const;
	PiSDFAbstractVertex* getReference() const;

	int getNbInputEdge() const;
	int getNbOutputEdge() const;
	SRDAGEdge* getInputEdge(int id);
	SRDAGEdge* getOutputEdge(int id);

	int getParamNb() const;
	int* getParamArray();

	UINT32 getExecTime(int slaveType) const;
	bool getConstraint(int slaveType) const;

	int getReferenceIndex() const;
	int getIterationIndex() const;

	virtual void updateState();

	int getSchedLevel() const;
	void setSchedLevel(int level);

	int getScheduleIndex() const;
	void setScheduleIndex(int index);

	UINT32 getStartTime() const;
	void setStartTime(UINT32 time);

	UINT32 getEndTime() const;
	void setEndTime(UINT32 time);

	bool isHierarchical() const;
	PiSDFGraph* getHierarchy() const;

	void getName(char* name, UINT32 sizeMax);

	int getSetIx() const;
	void setSetIx(int setIx);

	int getRelatedParamValue(int paramIndex) const;
	void setRelatedParamValue(int paramIndex, int value);

//	friend class SRDAGGraph;
	friend class SRDAGEdge;
};

inline void SRDAGVertex::connectInputEdge(SRDAGEdge* edge, int ix)
	{inputs.setValue(ix, edge);}

inline void SRDAGVertex::connectOutputEdge(SRDAGEdge* edge, int ix)
	{outputs.setValue(ix, edge);}

inline void SRDAGVertex::disconnectInputEdge(int ix)
	{inputs.resetValue(ix);}

inline void SRDAGVertex::disconnectOutputEdge(int ix)
	{outputs.resetValue(ix);}

inline int SRDAGVertex::getId() const
	{return id;}

inline SRDAGVertexType  SRDAGVertex::getType() const
	{return type;}

inline SRDAGVertexState SRDAGVertex::getState() const
	{return state;}

inline void SRDAGVertex::setState(SRDAGVertexState st)
	{state = st;}

inline int SRDAGVertex::getFctIx() const
	{return fctIx;}

inline int SRDAGVertex::getReferenceIndex() const
	{return refIx;}

inline int SRDAGVertex::getIterationIndex() const
	{return itrIx;}

inline PiSDFAbstractVertex* SRDAGVertex::getReference() const
	{return reference;}

inline int SRDAGVertex::getNbInputEdge() const
	{return inputs.getNb();}

inline int SRDAGVertex::getNbOutputEdge() const
	{return outputs.getNb();}

inline SRDAGEdge* SRDAGVertex::getInputEdge(int id)
	{return inputs[id];}

inline SRDAGEdge* SRDAGVertex::getOutputEdge(int id)
	{return outputs[id];}

inline int SRDAGVertex::getSchedLevel() const
	{return schedLevel;}

inline void SRDAGVertex::setSchedLevel(int level)
	{schedLevel = level;}

inline int SRDAGVertex::getSetIx() const
	{return setIx;}

inline void SRDAGVertex::setSetIx(int setIx)
	{this->setIx = setIx;}

inline int SRDAGVertex::getParamNb() const
	{return params.getNb();}

inline int* SRDAGVertex::getParamArray()
	{return params.getArray();}

inline UINT32 SRDAGVertex::getExecTime(int slaveType) const
	{return execTime[slaveType];}

inline bool SRDAGVertex::getConstraint(int slaveType) const
	{return constraints[slaveType];}

inline UINT32 SRDAGVertex::getStartTime() const
	{return startTime;}

inline void SRDAGVertex::setStartTime(UINT32 time)
	{startTime = time;}

inline UINT32 SRDAGVertex::getEndTime() const
	{return endTime;}

inline void SRDAGVertex::setEndTime(UINT32 time)
	{endTime = time;}

inline bool SRDAGVertex::isHierarchical() const
	{return haveSubGraph;}

inline PiSDFGraph* SRDAGVertex::getHierarchy() const
	{return subGraph;}

inline int SRDAGVertex::getRelatedParamValue(int paramIndex) const
	{return relatedParamValues[paramIndex];}

inline void SRDAGVertex::setRelatedParamValue(int paramIndex, int value)
	{relatedParamValues.setValue(paramIndex, value);}

#endif
