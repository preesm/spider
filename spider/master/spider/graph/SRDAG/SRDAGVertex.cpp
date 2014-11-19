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


#include <graph/SRDAG/SRDAGVertex.h>
#include <cstdio>
#include <cstring>

/** Static Var def */
int SRDAGVertex::globalId = 0;

/** Constructor */
SRDAGVertex::SRDAGVertex(){
	id_ = -1;

	type_ = SRDAG_NORMAL;
	state_ = SRDAG_NEXEC;
	graph_ = 0;
	reference_ = 0;

	nInEdge_ = nOutEdge_ = 0;
	inEdges_ = outEdges_ = 0;

	nInParam_ = nOutParam_ = 0;
	inParams_ = 0;
	outParams_ = 0;
}

SRDAGVertex::SRDAGVertex(
		SRDAGType type,	SRDAGGraph* graph,
		PiSDFVertex* reference,
		int nInEdge, int nOutEdge,
		int nInParam, int nOutParam,
		Stack* stack){
	id_ = globalId++;

	type_ = type;
	state_ = SRDAG_NEXEC;
	graph_ = graph;
	reference_ = reference;

	nInEdge_ = nInEdge;
	inEdges_ = sAlloc(stack, nInEdge_, SRDAGEdge*);
	memset(inEdges_, 0, nInEdge_*sizeof(SRDAGEdge*));

	nOutEdge_ = nOutEdge;
	outEdges_ = sAlloc(stack, nOutEdge_, SRDAGEdge*);
	memset(outEdges_, 0, nOutEdge_*sizeof(SRDAGEdge*));

	nInParam_ = nInParam;
	inParams_ = sAlloc(stack, nInParam, int);
	memset(inParams_, 0, nInParam*sizeof(int));

	nOutParam_ = nOutParam;
	outParams_ = sAlloc(stack, nOutParam, const int*);
	memset(outParams_, 0, nOutParam*sizeof(int**));

//	memset(constraints, false, MAX_SLAVES*sizeof(bool));
}

void SRDAGVertex::toString(char* name, int sizeMax) const{
	switch(type_){
	case SRDAG_NORMAL:
		snprintf(name, sizeMax, "%s_%d", reference_->getName(), id_);
		break;
	case SRDAG_FORK:
		snprintf(name, sizeMax, "Fork_%d", id_);
		break;
	case SRDAG_JOIN:
		snprintf(name, sizeMax, "Join_%d", id_);
		break;
	case SRDAG_ROUNDBUFFER:
		snprintf(name, sizeMax, "RB_%d", id_);
		break;
	case SRDAG_BROADCAST:
		snprintf(name, sizeMax, "BR_%d", id_);
		break;
	case SRDAG_INIT:
		snprintf(name, sizeMax, "Init_%d", id_);
		break;
	case SRDAG_END:
		snprintf(name, sizeMax, "End_%d", id_);
		break;
	}
}
