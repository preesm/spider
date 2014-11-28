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

#ifndef PISDF_EDGE_H
#define PISDF_EDGE_H

#include <graphs/PiSDF/PiSDFCommon.h>
#include <graphs/PiSDF/PiSDFGraph.h>
#include <graphs/PiSDF/PiSDFVertex.h>
#include <parser/Expression.h>

class PiSDFEdge {
public:
	/** Constructors */
	PiSDFEdge();
	PiSDFEdge(
			PiSDFGraph* graph,
			int nParam,
			Stack *stack);

	/** Getters */
	inline int getId() const;
	inline PiSDFVertex* getSrc() const;
	inline PiSDFVertex* getSnk() const;
	inline int getSrcPortIx() const;
	inline int getSnkPortIx() const;

	/** Setters */
	inline void setDelay(const char* delay, Stack* stack);

	/** Connections Fcts */
	void connectSrc(PiSDFVertex* src, int srcPortId, const char* prod, Stack* stack);
	void connectSnk(PiSDFVertex* snk, int snkPortId, const char* cons, Stack* stack);

	/** Add Param Fcts */
	inline void addInParam(int ix, PiSDFParam* param);

	/** Compute Fcts */
	inline int resolveProd(transfoJob* job) const;
	inline int resolveCons(transfoJob* job) const;
	inline int resolveDelay(transfoJob* job);

	inline void getProdExpr(char* out, int sizeOut);
	inline void getConsExpr(char* out, int sizeOut);
	inline void getDelayExpr(char* out, int sizeOut);

private:
	static int globalId;

	int id_;
	PiSDFGraph* graph_;
//	Stack *stack_;

	PiSDFVertex* src_;
	int srcPortIx_;
	PiSDFVertex* snk_;
	int snkPortIx_;

	/* Production and Consumption */
	Parser::Expression prod_;
	Parser::Expression cons_;

	/* Parameterized Delays */
	Parser::Expression delay_;
};

inline int PiSDFEdge::getId() const{
	return id_;
}
inline PiSDFVertex* PiSDFEdge::getSrc() const {
	return src_;
}
inline PiSDFVertex* PiSDFEdge::getSnk() const {
	return snk_;
}
inline int PiSDFEdge::getSrcPortIx() const {
	return srcPortIx_;
}
inline int PiSDFEdge::getSnkPortIx() const {
	return snkPortIx_;
}

inline void PiSDFEdge::setDelay(const char* expr, Stack* stack){
	delay_ = Parser::Expression(expr, graph_->getParams(), graph_->getNParam(), stack);
}

inline int PiSDFEdge::resolveProd(transfoJob* job) const {
	return prod_.evaluate(src_->getInParams(), job);
}
inline int PiSDFEdge::resolveCons(transfoJob* job) const {
	return cons_.evaluate(snk_->getInParams(), job);
}
inline int PiSDFEdge::resolveDelay(transfoJob* job){
	return delay_.evaluate(graph_->getParams(), job);
}

inline void PiSDFEdge::getProdExpr(char* out, int sizeOut){
	prod_.toString(src_->getGraph()->getParams(), src_->getGraph()->getNParam(), out, sizeOut);
}
inline void PiSDFEdge::getConsExpr(char* out, int sizeOut){
	cons_.toString(snk_->getGraph()->getParams(), snk_->getGraph()->getNParam(), out, sizeOut);
}
inline void PiSDFEdge::getDelayExpr(char* out, int sizeOut){
	delay_.toString(graph_->getParams(), graph_->getNParam(), out, sizeOut);
}

#endif/*PISDF_EDGE_H*/
