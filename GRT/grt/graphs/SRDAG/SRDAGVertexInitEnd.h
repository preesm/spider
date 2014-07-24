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

#ifndef SRDAG_VERTEX_INIT_END
#define SRDAG_VERTEX_INIT_END

class SRDAGGraph;
#include <cstring>

#include <grt_definitions.h>
#include "../../tools/SchedulingError.h"
#include "../PiSDF/PiSDFAbstractVertex.h"
#include "../PiSDF/PiSDFVertex.h"
#include "SRDAGEdge.h"
#include "SRDAGVertexAbstract.h"
#include <tools/IndexedArray.h>

class SRDAGVertexInitEnd : public SRDAGVertexAbstract{

private :
	IndexedArray<SRDAGEdge*, 1> edges;

	void connectInputEdge(SRDAGEdge* edge, int ix);
	void connectOutputEdge(SRDAGEdge* edge, int ix);
	void disconnectInputEdge(int ix);
	void disconnectOutputEdge(int ix);

	int param;

public :
	SRDAGVertexInitEnd(){}
	SRDAGVertexInitEnd(
			SRDAGGraph* 	_graph,
			SRDAGVertexType _type,
			int 			_refIx,
			int 			_itrIx);
	~SRDAGVertexInitEnd(){}

	int getNbInputEdge() const;
	int getNbOutputEdge() const;
	SRDAGEdge* getInputEdge(int id);
	SRDAGEdge* getOutputEdge(int id);

	int getParamNb() const;
	int* getParamArray();

	virtual int getFctIx() const;

	BOOL isHierarchical() const;
	PiSDFGraph* getHierarchy() const;

	void getName(char* name, UINT32 sizeMax);
};

inline int SRDAGVertexInitEnd::getNbInputEdge() const{
	switch(type){
	case Init:
		return 0;
	case End:
		return edges.getNb();
	default:
		return 0;
	}
}

inline int SRDAGVertexInitEnd::getNbOutputEdge() const{
	switch(type){
	case Init:
		return edges.getNb();
	case End:
		return 0;
	default:
		return 0;
	}
}

inline SRDAGEdge* SRDAGVertexInitEnd::getInputEdge(int id){
	switch(type){
	case Init:
		return (SRDAGEdge*)NULL;
	case End:
		return edges[id];
	default:
		return (SRDAGEdge*)NULL;
	}
}

inline SRDAGEdge* SRDAGVertexInitEnd::getOutputEdge(int id){
	switch(type){
	case Init:
		return edges[id];
	case End:
		return (SRDAGEdge*)NULL;
	default:
		return (SRDAGEdge*)NULL;
	}
}

inline void SRDAGVertexInitEnd::connectInputEdge(SRDAGEdge* edge, int ix){
	switch(type){
	case Init:
		return edges.setValue(-1, edge);
	case End:
		return edges.setValue(ix, edge);
	default:
		return;
	}
}

inline void SRDAGVertexInitEnd::connectOutputEdge(SRDAGEdge* edge, int ix){
	switch(type){
	case Init:
		return edges.setValue(ix, edge);
	case End:
		return edges.setValue(-1, edge);
	default:
		return;
	}
}

inline void SRDAGVertexInitEnd::disconnectInputEdge(int ix){
	switch(type){
	case Init:
		return edges.resetValue(-1);
	case End:
		return edges.resetValue(ix);
	default:
		return;
	}
}

inline void SRDAGVertexInitEnd::disconnectOutputEdge(int ix){
	switch(type){
	case Init:
		return edges.resetValue(ix);
	case End:
		return edges.resetValue(-1);
	default:
		return;
	}
}

inline int SRDAGVertexInitEnd::getParamNb() const
	{return 1;}

inline int* SRDAGVertexInitEnd::getParamArray(){
	param = edges[0]->getTokenRate();
	return &param;
}

inline int SRDAGVertexInitEnd::getFctIx() const{
	switch(type){
	case Init: return INIT_FUNCT_IX;
	case End:  return END_FUNCT_IX;
	default: return -1;
	}
}

inline BOOL SRDAGVertexInitEnd::isHierarchical() const
	{return false;}

inline PiSDFGraph* SRDAGVertexInitEnd::getHierarchy() const
	{return (PiSDFGraph*)NULL;}

inline void SRDAGVertexInitEnd::getName(char* name, UINT32 sizeMax){
	int len = snprintf(name,MAX_VERTEX_NAME_SIZE,"%s_%d", (type == Init) ? "Init" : "End", id);
	if(len > MAX_VERTEX_NAME_SIZE)
		exitWithCode(1075);
}

#endif
