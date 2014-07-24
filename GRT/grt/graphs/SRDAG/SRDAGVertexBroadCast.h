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

#ifndef SRDAG_VERTEX_BROADCAST
#define SRDAG_VERTEX_BROADCAST

class SRDAGGraph;
#include <cstring>

#include <grt_definitions.h>
#include "../../tools/SchedulingError.h"
#include "../PiSDF/PiSDFAbstractVertex.h"
#include "../PiSDF/PiSDFVertex.h"
#include "SRDAGEdge.h"
#include "SRDAGVertexAbstract.h"
#include <tools/IndexedArray.h>

class SRDAGVertexBroadcast : public SRDAGVertexAbstract{

private :
	IndexedArray<SRDAGEdge*, 1> inputEdges;
	IndexedArray<SRDAGEdge*, MAX_SRDAG_XPLODE_EDGES> outputEdges;

	void connectInputEdge(SRDAGEdge* edge, int ix);
	void connectOutputEdge(SRDAGEdge* edge, int ix);
	void disconnectInputEdge(int ix);
	void disconnectOutputEdge(int ix);

public :
	SRDAGVertexBroadcast(){}
	SRDAGVertexBroadcast(
				SRDAGGraph* 	_graph,
				int 			_refIx,
				int 			_itrIx,
				PiSDFVertex* ref);
	~SRDAGVertexBroadcast(){}

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

inline int SRDAGVertexBroadcast::getNbInputEdge() const
	{return inputEdges.getNb();}

inline int SRDAGVertexBroadcast::getNbOutputEdge() const
	{return outputEdges.getNb();}

inline SRDAGEdge* SRDAGVertexBroadcast::getInputEdge(int id)
	{return inputEdges[id];}

inline SRDAGEdge* SRDAGVertexBroadcast::getOutputEdge(int id)
	{return outputEdges[id];}

inline void SRDAGVertexBroadcast::connectInputEdge(SRDAGEdge* edge, int ix)
	{inputEdges.setValue(ix, edge);}

inline void SRDAGVertexBroadcast::connectOutputEdge(SRDAGEdge* edge, int ix)
	{outputEdges.setValue(ix, edge);}

inline void SRDAGVertexBroadcast::disconnectInputEdge(int ix)
	{inputEdges.resetValue(ix);}

inline void SRDAGVertexBroadcast::disconnectOutputEdge(int ix)
	{outputEdges.resetValue(ix);}

inline int SRDAGVertexBroadcast::getParamNb() const
	{return 0;}

inline int* SRDAGVertexBroadcast::getParamArray()
	{return (int*)NULL;}

inline int SRDAGVertexBroadcast::getFctIx() const
	{return BROADCAST_FUNCT_IX;}

inline BOOL SRDAGVertexBroadcast::isHierarchical() const
	{return false;}

inline PiSDFGraph* SRDAGVertexBroadcast::getHierarchy() const
	{return (PiSDFGraph*)NULL;}

inline void SRDAGVertexBroadcast::getName(char* name, UINT32 sizeMax){
	int len = snprintf(name,MAX_VERTEX_NAME_SIZE,"Br_%d", id);
	if(len > MAX_VERTEX_NAME_SIZE)
		exitWithCode(1075);
}

#endif
