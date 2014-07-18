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

#ifndef SRDAG_VERTEX_XPLODE
#define SRDAG_VERTEX_XPLODE

class SRDAGGraph;
#include <cstring>

#include <grt_definitions.h>
#include "../../tools/SchedulingError.h"
#include "../PiSDF/PiSDFAbstractVertex.h"
#include "../PiSDF/PiSDFVertex.h"
#include "SRDAGEdge.h"
#include "SRDAGVertexAbstract.h"
#include <tools/IndexedArray.h>

class SRDAGVertexXplode : public SRDAGVertexAbstract{

private :
	IndexedArray<SRDAGEdge*, 1> gatherEdges;
	IndexedArray<SRDAGEdge*, MAX_SRDAG_XPLODE_EDGES> scatterEdges;

	void connectInputEdge(SRDAGEdge* edge, int ix);
	void connectOutputEdge(SRDAGEdge* edge, int ix);
	void disconnectInputEdge(int ix);
	void disconnectOutputEdge(int ix);

public :
	SRDAGVertexXplode(){}
	SRDAGVertexXplode(
			int			 	_id,
			SRDAGGraph* 	_graph,
			SRDAGVertexType _type,
			SRDAGVertexAbstract* 	_parent,
			int 			_refIx,
			int 			_itrIx);
	~SRDAGVertexXplode(){}

	int getNbInputEdge() const;
	int getNbOutputEdge() const;
	SRDAGEdge* getInputEdge(int id);
	SRDAGEdge* getOutputEdge(int id);

	int getParamNb() const;
	int getParamValue(int paramIndex);
	UINT32 getExecTime(int slaveType) const;
	bool getConstraint(int slaveType) const;

	virtual int getFctIx() const;

	BOOL isHierarchical() const;
	PiSDFGraph* getHierarchy() const;

	void getName(char* name, UINT32 sizeMax);
};

inline int SRDAGVertexXplode::getNbInputEdge() const{
	switch(type){
	case Implode:
		return scatterEdges.getNb();
	case Explode:
		return gatherEdges.getNb();
	default:
		return 0;
	}
}

inline int SRDAGVertexXplode::getNbOutputEdge() const{
	switch(type){
	case Implode:
		return gatherEdges.getNb();
	case Explode:
		return scatterEdges.getNb();
	default:
		return 0;
	}
}

inline SRDAGEdge* SRDAGVertexXplode::getInputEdge(int id){
	switch(type){
	case Implode:
		return scatterEdges[id];
	case Explode:
		return gatherEdges[id];
	default:
		return NULL;
	}
}

inline SRDAGEdge* SRDAGVertexXplode::getOutputEdge(int id){
	switch(type){
	case Implode:
		return gatherEdges[id];
	case Explode:
		return scatterEdges[id];
	default:
		return NULL;
	}
}

inline void SRDAGVertexXplode::connectInputEdge(SRDAGEdge* edge, int ix){
	switch(type){
	case Implode:
		scatterEdges.setValue(ix, edge);
		break;
	case Explode:
		gatherEdges.setValue(ix, edge);
		break;
	default:
		return;
	}
}

inline void SRDAGVertexXplode::connectOutputEdge(SRDAGEdge* edge, int ix){
	switch(type){
	case Implode:
		gatherEdges.setValue(ix, edge);
		break;
	case Explode:
		scatterEdges.setValue(ix, edge);
		break;
	default:
		return;
	}
}

inline void SRDAGVertexXplode::disconnectInputEdge(int ix){
	switch(type){
	case Implode:
		scatterEdges.resetValue(ix);
		break;
	case Explode:
		gatherEdges.resetValue(ix);
		break;
	default:
		return;
	}
}

inline void SRDAGVertexXplode::disconnectOutputEdge(int ix){
	switch(type){
	case Implode:
		gatherEdges.resetValue(ix);
		break;
	case Explode:
		scatterEdges.resetValue(ix);
		break;
	default:
		return;
	}
}

inline int SRDAGVertexXplode::getParamNb() const{
	return 2+getNbInputEdge()+getNbOutputEdge();
}

inline int SRDAGVertexXplode::getParamValue(int paramIndex){
	if(paramIndex == 0)
		return getNbInputEdge();
	else if (paramIndex == 1)
		return getNbOutputEdge();
	else if (paramIndex < 2 + getNbInputEdge())
		return getInputEdge(paramIndex-2)->getTokenRate();
	else if (paramIndex < 2 + getNbInputEdge() + getNbOutputEdge())
		return getOutputEdge(paramIndex-2-getNbInputEdge())->getTokenRate();
	else
		return -1;
}

inline UINT32 SRDAGVertexXplode::getExecTime(int slaveType) const
	{return SYNC_TIME;}

inline bool SRDAGVertexXplode::getConstraint(int slaveType) const
	{return true;}

inline int SRDAGVertexXplode::getFctIx() const
	{return XPLODE_FUNCT_IX;}

inline BOOL SRDAGVertexXplode::isHierarchical() const{
	return false;
}

inline PiSDFGraph* SRDAGVertexXplode::getHierarchy() const{
	return NULL;
}

inline void SRDAGVertexXplode::getName(char* name, UINT32 sizeMax){
	int len = snprintf(name,MAX_VERTEX_NAME_SIZE,"%s_%d", (type == Implode) ? "Imp" : "Exp", id);
	if(len > MAX_VERTEX_NAME_SIZE)
		exitWithCode(1075);
}

#endif
