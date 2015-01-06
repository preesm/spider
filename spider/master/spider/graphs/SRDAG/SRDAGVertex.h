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
#include <graphs/SRDAG/SRDAGEdge.h>

#include <platform.h>

class SRDAGVertex {
public:
	friend class SRDAGEdge;

	SRDAGVertex(
			SRDAGType type, SRDAGGraph* graph,
			PiSDFVertex* reference,
			int nInEdge, int nOutEdge,
			int nInParam, int nOutParam,
			Stack* stack);
	~SRDAGVertex();

	/** Parameters getters */
	inline int getNInParam() const;
	inline int getNOutParam() const;
	inline int getInParam(int ix) const;
	inline int* getOutParam(int ix);
	inline const int * getInParams() const;
	inline const int * const * getOutParams() const;

	/** Data edge getters */
	inline int getNInEdge() const;
	inline int getNOutEdge() const;
	inline int getNConnectedInEdge() const;
	inline int getNConnectedOutEdge() const;
	inline SRDAGEdge* getInEdge(int ix);
	inline SRDAGEdge* getOutEdge(int ix);
	inline SRDAGEdge* const * getInEdges();
	inline SRDAGEdge* const * getOutEdges();

	/** Add Param Fcts */
	inline void addInParam(int ix, int param);
	inline void addOutParam(int ix, int* param);

	/** General getters */
	inline int getId() const;
	inline int getFctId() const;
	inline SRDAGType getType() const;
	inline SRDAGState getState() const;
	inline SRDAGGraph *getGraph() const;
	inline PiSDFGraph *getSubGraph() const;
	inline PiSDFVertex *getReference() const;
	inline bool isHierarchical() const;
	inline Time getStartTime() const;
	inline Time getEndTime() const;

	inline void setState(SRDAGState state);
	void updateState();
	inline void setStartTime(Time start);
	inline void setEndTime(Time end);

	/** Comparison fcts */
	inline bool isEqual(SRDAGVertex* v2);
	inline bool match(SRDAGVertex* v2);

	void toString(char* name, int sizeMax) const;


	/** Constraints/Timings Fcts */
	inline bool isExecutableOn(int pe) const;
	inline Time executionTimeOn(int peType) const;
	inline int getSchedLvl() const;
	void setSchedLvl(int schedLvl);

protected:
	/** Connect Fcts */
	inline void connectInEdge(SRDAGEdge* edge, int ix);
	inline void connectOutEdge(SRDAGEdge* edge, int ix);
	inline void disconnectInEdge(int ix);
	inline void disconnectOutEdge(int ix);

private:
	static int globalId;

	int id_;
	Stack* stack_;
	SRDAGType type_;
	SRDAGState state_;
	SRDAGGraph* graph_;
	PiSDFVertex* reference_;

	int nInEdge_, nOutEdge_;
	SRDAGEdge **inEdges_, **outEdges_;

	int nInParam_, nOutParam_;
	int *inParams_;
	int ** outParams_;

	Time start_, end_;
	int schedLvl_;

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
inline int* SRDAGVertex::getOutParam(int ix){
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
inline int SRDAGVertex::getNConnectedInEdge() const{
	int nb=0;
	for(int i=0; i<nInEdge_; i++)
		if(inEdges_[i] != 0)
			nb++;
	return nb;
}
inline int SRDAGVertex::getNConnectedOutEdge() const{
	int nb=0;
	for(int i=0; i<nOutEdge_; i++)
		if(outEdges_[i] != 0)
			nb++;
	return nb;
}
inline SRDAGEdge* SRDAGVertex::getInEdge(int ix){
#if	DEBUG
	if(ix < nInEdge_ && ix >= 0)
#endif
		return inEdges_[ix];
#if	DEBUG
	else
		throw "SRDAGVertex: Bad ix in getInEdge";
#endif
}
inline SRDAGEdge* SRDAGVertex::getOutEdge(int ix){
#if	DEBUG
	if(ix < nOutEdge_ && ix >= 0)
#endif
		return outEdges_[ix];
#if	DEBUG
	else
		throw "SRDAGVertex: Bad ix in getOutEdge";
#endif
}
inline SRDAGEdge* const * SRDAGVertex::getInEdges(){
	return inEdges_;
}
inline SRDAGEdge* const * SRDAGVertex::getOutEdges(){
	return outEdges_;
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
	if(ix >= nInParam_ || ix < 0)
		throw "SRDAGVertex: Bad ix in addInParam";
	else if(inParams_[ix] != 0)
		throw "SRDAGVertex: Try to erase already connected input param";
	else
#endif
		inParams_[ix] = param;
}
inline void SRDAGVertex::addOutParam(int ix, int* param){
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
		return JOIN_F_IX;
		break;
	case SRDAG_FORK:
		return FORK_F_IX;
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
inline PiSDFVertex *SRDAGVertex::getReference() const{
	return reference_;
}
inline bool SRDAGVertex::isHierarchical() const{
	return type_ == SRDAG_NORMAL && reference_->isHierarchical();
}
inline Time SRDAGVertex::getStartTime() const{
	return start_;
}
inline Time SRDAGVertex::getEndTime() const{
	return end_;
}

inline void SRDAGVertex::setState(SRDAGState state){
	state_ = state;
}
inline void SRDAGVertex::setStartTime(Time start){
	start_ = start;
}
inline void SRDAGVertex::setEndTime(Time end){
	end_ = end;
}

inline bool SRDAGVertex::isEqual(SRDAGVertex* v2){
	bool equal = true;
	equal = equal && (this->type_ == v2->type_);
	equal = equal && (this->getFctId() == v2->getFctId());

//	equal = equal && (this->getNInEdge() == v2->getNInEdge());
	equal = equal && (this->getNConnectedInEdge() == v2->getNConnectedInEdge());
//	equal = equal && (this->getNOutEdge() == v2->getNOutEdge());
	equal = equal && (this->getNConnectedOutEdge() == v2->getNConnectedOutEdge());

	equal = equal && (this->nInParam_ == v2->nInParam_);
	for(int i=0; i<this->nInParam_; i++)
		equal = equal && (this->inParams_[i] == v2->inParams_[i]);

	return equal;
}
inline bool SRDAGVertex::match(SRDAGVertex* v2){
	bool match = this->isEqual(v2);

	for(int i=0; i<this->getNConnectedInEdge(); i++)
		match = match && this->getInEdge(i)->getSrc()->isEqual(v2->getInEdge(i)->getSrc())
						&& this->getInEdge(i)->getRate() == v2->getInEdge(i)->getRate()
						&& (this->getInEdge(i)->getSrcPortIx() == v2->getInEdge(i)->getSrcPortIx()
								|| this->getInEdge(i)->getSrc()->getType() == SRDAG_BROADCAST);

	for(int i=0; i<this->getNConnectedOutEdge(); i++)
		match = match && this->getOutEdge(i)->getSnk()->isEqual(v2->getOutEdge(i)->getSnk())
						&& this->getOutEdge(i)->getRate()  == v2->getOutEdge(i)->getRate()
						&& (this->getOutEdge(i)->getSnkPortIx() == v2->getOutEdge(i)->getSnkPortIx()
								|| this->getOutEdge(i)->getSnk()->getType() == SRDAG_BROADCAST);

	return match;
}

inline bool SRDAGVertex::isExecutableOn(int pe) const{
	switch(type_){
	case SRDAG_NORMAL:
		return reference_->canExecuteOn(pe);
	case SRDAG_BROADCAST:
	case SRDAG_JOIN:
	case SRDAG_FORK:
	case SRDAG_ROUNDBUFFER:
	case SRDAG_INIT:
	case SRDAG_END:
		return true;
	default:
		throw "Unhandled case in SRDAGVertex::isExecutableOn\n";
	}
}
inline Time SRDAGVertex::executionTimeOn(int peType) const{
	switch(type_){
	case SRDAG_NORMAL:
		return reference_->getTimingOnType(peType, inParams_, nInParam_);
	case SRDAG_BROADCAST:
	case SRDAG_JOIN:
	case SRDAG_FORK:
	case SRDAG_ROUNDBUFFER:
	case SRDAG_INIT:
	case SRDAG_END:
		return 1;
	default:
		throw "Unhandled case in SRDAGVertex::isExecutableOn\n";
	}
}
inline int SRDAGVertex::getSchedLvl() const {
	return schedLvl_;
}
inline void SRDAGVertex::setSchedLvl(int schedLvl) {
	schedLvl_ = schedLvl;
}

#endif/*SRDAG_VERTEX_H*/
