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


#include <graphs/PiSDF/PiSDFVertex.h>
#include <string.h>

/** Static Var def */
int PiSDFVertex::globalId = 0;

/** Constructor */
PiSDFVertex::PiSDFVertex(
		const char* name, int fctId,
		int typeId,
		PiSDFType type, PiSDFSubType subType,
		PiSDFGraph* graph, PiSDFGraph* subGraph,
		int nInEdge, int nOutEdge,
		int nInParam, int nOutParam){

	id_ = globalId++;
	typeId_ = typeId;
	fctId_ = fctId;
	type_ = type;
	name_ = name;

	subType_ = subType;
	graph_ = graph;
	subGraph_ = subGraph;

	nInEdge_ = nInEdge;
	inEdges_ = CREATE_MUL(PISDF_STACK, nInEdge_, PiSDFEdge*);
	memset(inEdges_, 0, nInEdge_*sizeof(PiSDFEdge*));

	nOutEdge_ = nOutEdge;
	outEdges_ = CREATE_MUL(PISDF_STACK, nOutEdge_, PiSDFEdge*);
	memset(outEdges_, 0, nOutEdge_*sizeof(PiSDFEdge*));

	nInParam_ = nInParam;
	inParams_ = CREATE_MUL(PISDF_STACK, nInParam, PiSDFParam*);
	memset(inParams_, 0, nInParam*sizeof(PiSDFParam*));

	nOutParam_ = nOutParam;
	outParams_ = CREATE_MUL(PISDF_STACK, nOutParam, PiSDFParam*);
	memset(outParams_, 0, nOutParam*sizeof(PiSDFParam*));

	nPeMax_ = Spider::getArchi()->getNPE();
	nPeTypeMax_ = Spider::getArchi()->getNPETypes();

	constraints_ = CREATE_MUL(PISDF_STACK, nPeMax_, bool);
	memset(constraints_, false, nPeMax_*sizeof(bool));

	timings_ = CREATE_MUL(PISDF_STACK, nPeTypeMax_, Expression*);
	memset(timings_, 0, nPeTypeMax_*sizeof(Expression*));
}

PiSDFVertex::~PiSDFVertex(){
	StackMonitor::free(PISDF_STACK,inEdges_);
	StackMonitor::free(PISDF_STACK,outEdges_);
	StackMonitor::free(PISDF_STACK,inParams_);
	StackMonitor::free(PISDF_STACK,outParams_);
	StackMonitor::free(PISDF_STACK,constraints_);

	for(int i=0; i<nPeTypeMax_; i++){
		if(timings_[i]){
			timings_[i]->~Expression();
			StackMonitor::free(PISDF_STACK,timings_[i]);
			timings_[i] = 0;
		}
	}
	StackMonitor::free(PISDF_STACK,timings_);
}
