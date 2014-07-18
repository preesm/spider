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
#include "../../tools/SchedulingError.h"
#include "../PiSDF/PiSDFAbstractVertex.h"
#include "../PiSDF/PiSDFVertex.h"
#include "SRDAGEdge.h"
#include <tools/IndexedArray.h>

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

	SRDAGVertexAbstract* parent;

	int refIx;
	int itrIx;

	int schedLevel;
	int slaveIndex;
	int scheduleIndex;
	int minStartTime;

	virtual void connectInputEdge(SRDAGEdge* edge, int ix)	= 0;
	virtual void connectOutputEdge(SRDAGEdge* edge, int ix)	= 0;
	virtual void disconnectInputEdge(int ix)	= 0;
	virtual void disconnectOutputEdge(int ix)	= 0;

public:
	SRDAGVertexAbstract(){}
	SRDAGVertexAbstract(
				int			 	_id,
				SRDAGGraph* 	_graph,
				SRDAGVertexType _type,
				SRDAGVertexAbstract* _parent,
				int 			_refIx,
				int 			_itrIx);

	virtual ~SRDAGVertexAbstract(){}

	int getId() const;
	SRDAGVertexType  getType()  const;
	SRDAGVertexState getState() const;
	void setState(SRDAGVertexState st);
	virtual int getFctIx() const = 0;

	virtual int getNbInputEdge()  const = 0;
	virtual int getNbOutputEdge() const = 0;
	virtual SRDAGEdge* getInputEdge(int id)  = 0;
	virtual SRDAGEdge* getOutputEdge(int id) = 0;

	virtual int getParamNb() const = 0;
	virtual int getParamValue(int paramIndex) = 0;

	virtual UINT32 getExecTime(int slaveType) const = 0;
	virtual bool getConstraint(int slaveType) const = 0;

	int getReferenceIndex() const;
	int getIterationIndex() const;

	virtual void updateState();

	int getSchedLevel() const;
	void setSchedLevel(int level);

	int getScheduleIndex() const;
	void setScheduleIndex(int index);

	int getMinStartTime() const;
	void setMinStartTime(UINT32 time);

	virtual BOOL isHierarchical() const = 0;
	virtual PiSDFGraph* getHierarchy() const = 0;

	virtual void getName(char* name, UINT32 sizeMax) = 0;

//	friend class SRDAGGraph;
	friend class SRDAGEdge;
};

inline int SRDAGVertexAbstract::getId() const
	{return id;}

inline SRDAGVertexType  SRDAGVertexAbstract::getType() const
	{return type;}

inline SRDAGVertexState SRDAGVertexAbstract::getState() const
	{return state;}

inline void SRDAGVertexAbstract::setState(SRDAGVertexState st)
	{state = st;}

inline int SRDAGVertexAbstract::getReferenceIndex() const
	{return refIx;}

inline int SRDAGVertexAbstract::getIterationIndex() const
	{return itrIx;}

inline void SRDAGVertexAbstract::updateState(){
	if(state != SRDAG_Executed){
		switch(type){
		case ConfigureActor:
			state = SRDAG_Executable;
			break;
		case RoundBuffer:
			if(getNbInputEdge() == 1 && getNbOutputEdge() == 1)
				state = SRDAG_Executable;
			else
				state = SRDAG_NotExecuted;
			break;
		default:
			for (int i = 0; i < getNbInputEdge(); i++){
				SRDAGVertexAbstract* predecessor = getInputEdge(i)->getSource();

				if(predecessor->state == SRDAG_NotExecuted)
					predecessor->updateState();

				if(predecessor->state == SRDAG_NotExecuted
						|| predecessor->isHierarchical()){
					state = SRDAG_NotExecuted;
					return;
				}
			}
			state = SRDAG_Executable;
		}
	}
}

inline int SRDAGVertexAbstract::getSchedLevel() const
	{return schedLevel;}

inline void SRDAGVertexAbstract::setSchedLevel(int level)
	{schedLevel = level;}

inline int SRDAGVertexAbstract::getScheduleIndex() const
	{return scheduleIndex;}

inline void SRDAGVertexAbstract::setScheduleIndex(int index)
	{scheduleIndex = index;}

inline int SRDAGVertexAbstract::getMinStartTime() const
	{return minStartTime;}

inline void SRDAGVertexAbstract::setMinStartTime(UINT32 time)
	{minStartTime = time;}

#endif
