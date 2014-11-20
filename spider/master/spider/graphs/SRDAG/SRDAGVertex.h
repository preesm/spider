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

#ifndef SRDAG_VERTEX_H
#define SRDAG_VERTEX_H

#include <graphs/PiSDF/PiSDFVertex.h>
#include <graphs/SRDAG/SRDAGCommon.h>

class SRDAGVertex {
public:
	friend class SRDAGEdge;

	SRDAGVertex();
	SRDAGVertex(
			SRDAGType type, SRDAGGraph* graph,
			PiSDFVertex* reference,
			int nInEdge, int nOutEdge,
			int nInParam, int nOutParam,
			Stack* stack);

	/** Parameters getters */
	inline int getNInParam() const;
	inline int getNOutParam() const;
	inline int getInParam(int ix) const;
	inline const int* getOutParam(int ix) const;
	inline const int * getInParams() const;
	inline const int * const * getOutParams() const;

	/** Data edge getters */
	inline int getNInEdge() const;
	inline int getNOutEdge() const;
	inline const SRDAGEdge* getInEdge(int ix) const;
	inline const SRDAGEdge* getOutEdge(int ix) const;


	/** Add Param Fcts */
	inline void addInParam(int ix, int param);
	inline void addOutParam(int ix, const int* param);

	/** General getters */
	inline int getId() const;
	inline int getFctId() const;
	inline SRDAGType getType() const;
	inline SRDAGState getState() const;
	inline SRDAGGraph *getGraph() const;
	inline PiSDFGraph *getSubGraph() const;
	inline bool isHierarchical() const;

	inline void setState(SRDAGState state);

	void toString(char* name, int sizeMax) const;

protected:
	/** Connect Fcts */
	inline void connectInEdge(SRDAGEdge* edge, int ix);
	inline void connectOutEdge(SRDAGEdge* edge, int ix);
	inline void disconnectInEdge(int ix);
	inline void disconnectOutEdge(int ix);

private:
	static int globalId;

	int id_;
	SRDAGType type_;
	SRDAGState state_;
	SRDAGGraph* graph_;
	PiSDFVertex* reference_;

	int nInEdge_, nOutEdge_;
	SRDAGEdge **inEdges_, **outEdges_;

	int nInParam_, nOutParam_;
	int *inParams_;
	const int **outParams_;

//	BOOL constraints[MAX_SLAVES];
//	variable timings[MAX_SLAVE_TYPES];
};

/** Inlines Fcts */
/** Parameters getters */
inline int SRDAGVertex::getNInParam() const{
	return nInParam_;
}
inline int SRDAGVertex::getNOutParam() const{
	return nOutParam_;
}
inline int SRDAGVertex::getInParam(int ix) const{
#if	DEBUG
	if(ix < nInParam_ && ix >= 0)
#endif
		return inParams_[ix];
#if	DEBUG
	else
		throw "SRDAGVertex: Bad ix in getInParam";
#endif
}
inline const int* SRDAGVertex::getOutParam(int ix) const{
#if	DEBUG
	if(ix < nOutParam_ && ix >= 0)
#endif
		return outParams_[ix];
#if	DEBUG
	else
		throw "SRDAGVertex: Bad ix in getOutParam";
#endif
}
inline const int* SRDAGVertex::getInParams() const{
	return inParams_;
}
inline const int* const * SRDAGVertex::getOutParams() const{
	return outParams_;
}

/** Data edge getters */
inline int SRDAGVertex::getNInEdge() const{
	return nInEdge_;
}
inline int SRDAGVertex::getNOutEdge() const{
	return nOutEdge_;
}
inline const SRDAGEdge* SRDAGVertex::getInEdge(int ix) const{
#if	DEBUG
	if(ix < nInEdge_ && ix >= 0)
#endif
		return inEdges_[ix];
#if	DEBUG
	else
		throw "SRDAGVertex: Bad ix in getInEdge";
#endif
}
inline const SRDAGEdge* SRDAGVertex::getOutEdge(int ix) const{
#if	DEBUG
	if(ix < nOutEdge_ && ix >= 0)
#endif
		return outEdges_[ix];
#if	DEBUG
	else
		throw "SRDAGVertex: Bad ix in getOutEdge";
#endif
}

/** Connect Fcts */
inline void SRDAGVertex::connectInEdge(SRDAGEdge* edge, int ix){
#if	DEBUG
	if(ix >= nInEdge_ && ix < 0)
		throw "SRDAGVertex: Bad ix in connectInEdge";
	else if(inEdges_[ix] != 0)
		throw "SRDAGVertex: Try to overwrite already connected input edge";
	else
#endif
		inEdges_[ix] = edge;
}
inline void SRDAGVertex::connectOutEdge(SRDAGEdge* edge, int ix){
#if	DEBUG
	if(ix >= nOutEdge_ && ix < 0)
		throw "SRDAGVertex: Bad ix in connectOutEdge";
	else if(outEdges_[ix] != 0)
		throw "SRDAGVertex: Try to overwrite already connected output edge";
	else
#endif
		outEdges_[ix] = edge;
}
inline void SRDAGVertex::disconnectInEdge(int ix){
#if	DEBUG
	if(ix >= nInEdge_ && ix < 0)
		throw "SRDAGVertex: Bad ix in disconnectInEdge";
	else if(inEdges_[ix] == 0)
		throw "SRDAGVertex: Try to disconnect empty input edge";
	else
#endif
		inEdges_[ix] = 0;
}
inline void SRDAGVertex::disconnectOutEdge(int ix){
#if	DEBUG
	if(ix >= nOutEdge_ && ix < 0)
		throw "SRDAGVertex: Bad ix in disconnectOutEdge";
	else if(outEdges_[ix] == 0)
		throw "SRDAGVertex: Try to disconnect empty output edge";
	else
#endif
		outEdges_[ix] = 0;
}

/** Add Param Fcts */
inline void SRDAGVertex::addInParam(int ix, int param){
#if	DEBUG
	if(ix >= nInParam_ && ix < 0)
		throw "SRDAGVertex: Bad ix in addInParam";
	else if(inParams_[ix] != 0)
		throw "SRDAGVertex: Try to erase already connected input param";
	else
#endif
		inParams_[ix] = param;
}
inline void SRDAGVertex::addOutParam(int ix, const int* param){
#if	DEBUG
	if(ix >= nOutParam_ && ix < 0)
		throw "SRDAGVertex: Bad ix in addOutParam";
	else if(outParams_[ix] != 0)
		throw "SRDAGVertex: Try to erase already connected output param";
	else
#endif
		outParams_[ix] = param;
}

/** General getters */
inline int SRDAGVertex::getId() const{
	return id_;
}
inline int SRDAGVertex::getFctId() const{
	switch(type_){
	case SRDAG_NORMAL:
		return reference_->getFctId();
		break;
	case SRDAG_BROADCAST:
		return BROADCAST_F_IX;
		break;
	case SRDAG_JOIN:
		return IMPLODE_F_IX;
		break;
	case SRDAG_FORK:
		return EXPLODE_F_IX;
		break;
	case SRDAG_ROUNDBUFFER:
		return ROUNDBUFFER_F_IX;
		break;
	case SRDAG_INIT:
		return INIT_F_IX;
		break;
	case SRDAG_END:
		return END_F_IX;
		break;
	default:
		throw "Unhandled case in SRDAGVertex::getFctId\n";
	}
}
inline SRDAGType SRDAGVertex::getType() const{
	return type_;
}
inline SRDAGState SRDAGVertex::getState() const{
	return state_;
}
inline SRDAGGraph *SRDAGVertex::getGraph() const{
	return graph_;
}
inline PiSDFGraph *SRDAGVertex::getSubGraph() const{
	if(type_ == SRDAG_NORMAL)
		return reference_->getSubGraph();
	else
		return 0;
}
inline bool SRDAGVertex::isHierarchical() const{
	return type_ == SRDAG_NORMAL && reference_->isHierarchical();
}

inline void SRDAGVertex::setState(SRDAGState state){
	state_ = state;
}

#endif/*SRDAG_VERTEX_H*/
