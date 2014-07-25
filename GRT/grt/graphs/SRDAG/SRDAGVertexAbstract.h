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

class SRDAGGraph;
#include <cstring>
#include <cstdlib>

#include <grt_definitions.h>
#include <tools/SchedulingError.h>

#include "SRDAGEdgeArray.h"

class SRDAGEdge;
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

class SRDAGVertexAbstract{
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

	SRDAGEdgeArray inputs, outputs;

public:
	SRDAGVertexAbstract();
	SRDAGVertexAbstract(
				SRDAGGraph* 	_graph,
				SRDAGVertexType _type,
				PiSDFAbstractVertex* _ref,
				int 			_refIx,
				int 			_itrIx);

	virtual ~SRDAGVertexAbstract(){}

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

	virtual int getParamNb() const = 0;
	virtual int* getParamArray() = 0;

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

	virtual BOOL isHierarchical() const = 0;
	virtual PiSDFGraph* getHierarchy() const = 0;

	virtual void getName(char* name, UINT32 sizeMax) = 0;

	int getSetIx() const;
	void setSetIx(int setIx);

//	friend class SRDAGGraph;
	friend class SRDAGEdge;
};

inline void SRDAGVertexAbstract::connectInputEdge(SRDAGEdge* edge, int ix)
	{inputs.setValue(ix, edge);}

inline void SRDAGVertexAbstract::connectOutputEdge(SRDAGEdge* edge, int ix)
	{outputs.setValue(ix, edge);}

inline void SRDAGVertexAbstract::disconnectInputEdge(int ix)
	{inputs.resetValue(ix);}

inline void SRDAGVertexAbstract::disconnectOutputEdge(int ix)
	{outputs.resetValue(ix);}

inline int SRDAGVertexAbstract::getId() const
	{return id;}

inline SRDAGVertexType  SRDAGVertexAbstract::getType() const
	{return type;}

inline SRDAGVertexState SRDAGVertexAbstract::getState() const
	{return state;}

inline void SRDAGVertexAbstract::setState(SRDAGVertexState st)
	{state = st;}

inline int SRDAGVertexAbstract::getFctIx() const
	{return fctIx;}

inline int SRDAGVertexAbstract::getReferenceIndex() const
	{return refIx;}

inline int SRDAGVertexAbstract::getIterationIndex() const
	{return itrIx;}

inline PiSDFAbstractVertex* SRDAGVertexAbstract::getReference() const
	{return reference;}

inline int SRDAGVertexAbstract::getNbInputEdge() const
	{return inputs.getNb();}

inline int SRDAGVertexAbstract::getNbOutputEdge() const
	{return outputs.getNb();}

inline SRDAGEdge* SRDAGVertexAbstract::getInputEdge(int id)
	{return inputs[id];}

inline SRDAGEdge* SRDAGVertexAbstract::getOutputEdge(int id)
	{return outputs[id];}

inline int SRDAGVertexAbstract::getSchedLevel() const
	{return schedLevel;}

inline void SRDAGVertexAbstract::setSchedLevel(int level)
	{schedLevel = level;}

inline int SRDAGVertexAbstract::getSetIx() const
	{return setIx;}

inline void SRDAGVertexAbstract::setSetIx(int setIx)
	{this->setIx = setIx;}

inline UINT32 SRDAGVertexAbstract::getExecTime(int slaveType) const
	{return execTime[slaveType];}

inline bool SRDAGVertexAbstract::getConstraint(int slaveType) const
	{return constraints[slaveType];}

inline UINT32 SRDAGVertexAbstract::getStartTime() const
	{return startTime;}

inline void SRDAGVertexAbstract::setStartTime(UINT32 time)
	{startTime = time;}

inline UINT32 SRDAGVertexAbstract::getEndTime() const
	{return endTime;}

inline void SRDAGVertexAbstract::setEndTime(UINT32 time)
	{endTime = time;}

#endif
