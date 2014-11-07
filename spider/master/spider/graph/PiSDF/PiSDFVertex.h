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

#ifndef PISDF_VERTEX_H
#define PISDF_VERTEX_H

#include "PiSDFCommon.h"

class PiSDFVertex {
public:
	PiSDFVertex();
	PiSDFVertex(
			const char* name, int fctId,
			PiSDFType type, PiSDFSubType subType,
			PiSDFGraph* graph, PiSDFGraph* subGraph,
			int nInEdge, int nOutEdge,
			int nInParam, int nOutParam,
			Stack* stack);

	/** Parameters getters */
	inline int getNInParam() const;
	inline int getNOutParam() const;
	inline const PiSDFParam* getInParam(int ix) const;
	inline const PiSDFParam* getOutParam(int ix) const;

	/** Data edge getters */
	inline int getNInEdge() const;
	inline int getNOutEdge() const;
	inline const PiSDFEdge* getInEdge(int ix) const;
	inline const PiSDFEdge* getOutEdge(int ix) const;

	/** Connect Fcts */
	inline void connectInEdge(int ix, PiSDFEdge* edge);
	inline void connectOutEdge(int ix, PiSDFEdge* edge);

	/** Add Param Fcts */
	inline void addInParam(int ix, PiSDFParam* param);
	inline void addOutParam(int ix, PiSDFParam* param);

	/** General getters */
	inline int getId() const;
	inline int getFctId() const;
	inline const char* getName() const;
	inline PiSDFType getType() const;
	inline PiSDFSubType getSubType() const;
	inline PiSDFGraph *getGraph() const;
	inline PiSDFGraph *getSubGraph() const;
	inline bool isHierarchical() const;

	/** General setter */
	inline void setSubGraph(PiSDFGraph* subGraph);

private:
	static int globalId;

	int id_;
	int fctId_;
	const char* name_;

	PiSDFType type_;
	PiSDFSubType subType_;

	PiSDFGraph* graph_;
	PiSDFGraph* subGraph_;

	int nInEdge_, nOutEdge_;
	PiSDFEdge **inEdges_, **outEdges_;

	int nInParam_, nOutParam_;
	PiSDFParam **inParams_, **outParams_;

//	BOOL constraints[MAX_SLAVES];
//	variable timings[MAX_SLAVE_TYPES];
};

/** Inlines Fcts */
/** Parameters getters */
inline int PiSDFVertex::getNInParam() const{
	return nInParam_;
}
inline int PiSDFVertex::getNOutParam() const{
	return nOutParam_;
}
inline const PiSDFParam* PiSDFVertex::getInParam(int ix) const{
#if	DEBUG
	if(ix < nInParam_ && ix >= 0)
#endif
		return inParams_[ix];
#if	DEBUG
	else
		throw "PiSDFVertex: Bad ix in getInParam";
#endif
}
inline const PiSDFParam* PiSDFVertex::getOutParam(int ix) const{
#if	DEBUG
	if(ix < nOutParam_ && ix >= 0)
#endif
		return outParams_[ix];
#if	DEBUG
	else
		throw "PiSDFVertex: Bad ix in getOutParam";
#endif
}

/** Data edge getters */
inline int PiSDFVertex::getNInEdge() const{
	return nInEdge_;
}
inline int PiSDFVertex::getNOutEdge() const{
	return nOutEdge_;
}
inline const PiSDFEdge* PiSDFVertex::getInEdge(int ix) const{
#if	DEBUG
	if(ix < nInEdge_ && ix >= 0)
#endif
		return inEdges_[ix];
#if	DEBUG
	else
		throw "PiSDFVertex: Bad ix in getInEdge";
#endif
}
inline const PiSDFEdge* PiSDFVertex::getOutEdge(int ix) const{
#if	DEBUG
	if(ix < nOutEdge_ && ix >= 0)
#endif
		return outEdges_[ix];
#if	DEBUG
	else
		throw "PiSDFVertex: Bad ix in getOutEdge";
#endif
}

/** Connect Fcts */
inline void PiSDFVertex::connectInEdge(int ix, PiSDFEdge* edge){
#if	DEBUG
	if(ix >= nInEdge_ && ix < 0)
		throw "PiSDFVertex: Bad ix in connectInEdge";
	else if(inEdges_[ix] != 0)
		throw "PiSDFVertex: Try to erase already connected input edge";
	else
#endif
		inEdges_[ix] = edge;
}
inline void PiSDFVertex::connectOutEdge(int ix, PiSDFEdge* edge){
#if	DEBUG
	if(ix >= nOutEdge_ && ix < 0)
		throw "PiSDFVertex: Bad ix in connectOutEdge";
	else if(outEdges_[ix] != 0)
		throw "PiSDFVertex: Try to erase already connected output edge";
	else
#endif
		outEdges_[ix] = edge;
}

/** Add Param Fcts */
inline void PiSDFVertex::addInParam(int ix, PiSDFParam* param){
#if	DEBUG
	if(ix >= nInParam_ && ix < 0)
		throw "PiSDFVertex: Bad ix in addInParam";
	else if(inParams_[ix] != 0)
		throw "PiSDFVertex: Try to erase already connected input param";
	else
#endif
		inParams_[ix] = param;
}
inline void PiSDFVertex::addOutParam(int ix, PiSDFParam* param){
#if	DEBUG
	if(ix >= nOutParam_ && ix < 0)
		throw "PiSDFVertex: Bad ix in addOutParam";
	else if(outParams_[ix] != 0)
		throw "PiSDFVertex: Try to erase already connected output param";
	else
#endif
		outParams_[ix] = param;
}

/** General getters */
inline int PiSDFVertex::getId() const{
	return id_;
}
inline const char* PiSDFVertex::getName() const{
	return name_;
}
inline int PiSDFVertex::getFctId() const{
	return fctId_;
}
inline PiSDFType PiSDFVertex::getType() const{
	return type_;
}
inline PiSDFSubType PiSDFVertex::getSubType() const{
	return subType_;
}
inline PiSDFGraph *PiSDFVertex::getGraph() const{
	return graph_;
}
inline PiSDFGraph *PiSDFVertex::getSubGraph() const{
	return subGraph_;
}
inline bool PiSDFVertex::isHierarchical() const{
	return subGraph_ != 0;
}

inline void PiSDFVertex::setSubGraph(PiSDFGraph* subGraph){
	subGraph_ = subGraph;
}

#endif/*PISDF_VERTEX_H*/
