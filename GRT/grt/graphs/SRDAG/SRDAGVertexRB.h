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

#ifndef SRDAG_VERTEX_RB
#define SRDAG_VERTEX_RB

class SRDAGGraph;
#include <cstring>

#include <grt_definitions.h>
#include "../../tools/SchedulingError.h"
#include "../PiSDF/PiSDFAbstractVertex.h"
#include "../PiSDF/PiSDFVertex.h"
#include "SRDAGEdge.h"
#include "SRDAGVertexAbstract.h"
#include <tools/IndexedArray.h>

class SRDAGVertexRB : public SRDAGVertexAbstract{

private :
	IndexedArray<SRDAGEdge*, 1> inputEdges;
	IndexedArray<SRDAGEdge*, 1> outputEdges;

	void connectInputEdge(SRDAGEdge* edge, int ix);
	void connectOutputEdge(SRDAGEdge* edge, int ix);
	void disconnectInputEdge(int ix);
	void disconnectOutputEdge(int ix);

public :
	SRDAGVertexRB(){}
	SRDAGVertexRB(
			int			 	_id,
			SRDAGGraph* 	_graph,
			SRDAGVertexAbstract* 	_parent,
			int 			_refIx,
			int 			_itrIx,
			PiSDFAbstractVertex* ref);
	~SRDAGVertexRB(){}

	int getNbInputEdge() const;
	int getNbOutputEdge() const;
	SRDAGEdge* getInputEdge(int id);
	SRDAGEdge* getOutputEdge(int id);

	int getParamNb() const;
	int getParamValue(int paramIndex);
	UINT32 getExecTime(int slaveType) const;
	bool getConstraint(int slaveType) const;

	int getFctIx() const;

	BOOL isHierarchical() const;
	PiSDFGraph* getHierarchy() const;

	void getName(char* name, UINT32 sizeMax);
};

inline int SRDAGVertexRB::getNbInputEdge() const
	{return inputEdges.getNb();}

inline int SRDAGVertexRB::getNbOutputEdge() const
	{return outputEdges.getNb();}

inline SRDAGEdge* SRDAGVertexRB::getInputEdge(int id)
	{return inputEdges[id];}

inline SRDAGEdge* SRDAGVertexRB::getOutputEdge(int id)
	{return outputEdges[id];}

inline void SRDAGVertexRB::connectInputEdge(SRDAGEdge* edge, int ix)
	{inputEdges.setValue(ix, edge);}

inline void SRDAGVertexRB::connectOutputEdge(SRDAGEdge* edge, int ix)
	{outputEdges.setValue(ix, edge);}

inline void SRDAGVertexRB::disconnectInputEdge(int ix)
	{inputEdges.resetValue(ix);}

inline void SRDAGVertexRB::disconnectOutputEdge(int ix)
	{outputEdges.resetValue(ix);}

inline int SRDAGVertexRB::getParamNb() const
	{return 2;}

inline int SRDAGVertexRB::getParamValue(int paramIndex){
	if(paramIndex == 0)
		return inputEdges[0]->getTokenRate();
	else if (paramIndex == 1)
		return outputEdges[0]->getTokenRate();
	else
		return -1;
}

inline UINT32 SRDAGVertexRB::getExecTime(int slaveType) const
	{return SYNC_TIME;}

inline bool SRDAGVertexRB::getConstraint(int slaveType) const
	{return true;}

inline int SRDAGVertexRB::getFctIx() const
	{return RB_FUNCT_IX;}

inline BOOL SRDAGVertexRB::isHierarchical() const
	{return false;}

inline PiSDFGraph* SRDAGVertexRB::getHierarchy() const
	{return NULL;}

inline void SRDAGVertexRB::getName(char* name, UINT32 sizeMax){
	int len = snprintf(name, MAX_VERTEX_NAME_SIZE, "RB_%d", id);
	if(len > MAX_VERTEX_NAME_SIZE)
		exitWithCode(1075);
}

#endif
