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

#include <cstdio>
#include <cstring>

#include "SRDAGVertex.h"

#include <graphs/PiSDF/PiSDFConfigVertex.h>
#include <graphs/PiSDF/PiSDFVertex.h>

int SRDAGVertex::creationIx = 0;

SRDAGVertex::SRDAGVertex(){
	id 				= -1;
	graph 			= NULL;
	type 			= Normal;
	state 			= SRDAG_NotExecuted;
	reference		= NULL;
	refIx 			= -1;
	itrIx 			= -1;
	schedLevel 		= -1;
	slaveIndex 		= -1;
	setIx 			= -1;
	startTime		= -1;
	endTime			= -1;
	fctIx 			= -1;
	haveSubGraph	= false;
	subGraph		= NULL;
}

SRDAGVertex::SRDAGVertex(
			SRDAGGraph* 	_graph,
			SRDAGVertexType _type,
			PiSDFAbstractVertex* _ref,
			int 			_refIx,
			int 			_itrIx){
	id 				= creationIx++;
	graph 			= _graph;
	type 			= _type;
	state 			= SRDAG_NotExecuted;
	reference		= _ref;
	refIx 			= _refIx;
	itrIx 			= _itrIx;
	schedLevel 		= -1;
	slaveIndex 		= -1;
	setIx 			= -1;
	startTime		= -1;
	endTime			= -1;
	fctIx 			= -1;
	haveSubGraph	= false;
	subGraph		= NULL;

	inputs = FitedArray<SRDAGEdge*,MAX_EDGE_ARRAY>();
	outputs = FitedArray<SRDAGEdge*,MAX_EDGE_ARRAY>();
	params = FitedArray<int,MAX_PARAM_ARRAY>();
	relatedParamValues = FitedArray<int,MAX_PARAM_ARRAY>();

	switch(type){
	case Normal:
		inputs = FitedArray<SRDAGEdge*,MAX_EDGE_ARRAY>(MAX_SRDAG_IO_EDGES);
		outputs = FitedArray<SRDAGEdge*,MAX_EDGE_ARRAY>(MAX_SRDAG_IO_EDGES);
		params = FitedArray<int,MAX_PARAM_ARRAY>(MAX_PARAM);
		fctIx = reference->getFunction_index();

		haveSubGraph = reference
				&& reference->getType() == normal_vertex
				&& ((PiSDFVertex*)reference)->hasSubGraph()
				&& type == Normal;

		if(haveSubGraph)
			subGraph = ((PiSDFVertex*)reference)->getSubGraph();

		for(int i=0; i<MAX_SLAVE_TYPES; i++){
			if(reference->getConstraints(i)){
				constraints[i] = true;
				execTime[i] =  reference->getResolvedTiming(i);
			}else{
				constraints[i] = false;
			}
		}

		for(int i=0; i<reference->getNbParameters(); i++)
			params.setValue(i, reference->getParameter(i)->getValue());
		break;
	case ConfigureActor:
		inputs = FitedArray<SRDAGEdge*,MAX_EDGE_ARRAY>(MAX_SRDAG_IO_EDGES);
		outputs = FitedArray<SRDAGEdge*,MAX_EDGE_ARRAY>(MAX_SRDAG_IO_EDGES);
		params = FitedArray<int,MAX_PARAM_ARRAY>(MAX_PARAM);
		relatedParamValues = FitedArray<int,MAX_PARAM_ARRAY>(MAX_PARAM);
		fctIx = reference->getFunction_index();
		haveSubGraph	= false;

		for(int i=0; i<MAX_SLAVE_TYPES; i++){
			if(reference->getConstraints(i)){
				constraints[i] = true;
				execTime[i] =  reference->getResolvedTiming(i);
			}else{
				constraints[i] = false;
			}
		}

		for(int i=0; i<reference->getNbParameters(); i++)
			params.setValue(i, reference->getParameter(i)->getValue());
		break;
	case Explode:
		inputs = FitedArray<SRDAGEdge*,MAX_EDGE_ARRAY>(1);
		outputs = FitedArray<SRDAGEdge*,MAX_EDGE_ARRAY>(MAX_SRDAG_XPLODE_EDGES);
		fctIx = XPLODE_FUNCT_IX;
		haveSubGraph	= false;
		for(int i=0; i<MAX_SLAVE_TYPES; i++){
			constraints[i] = true;
			execTime[i] = SYNC_TIME;
		}
		break;
	case Implode:
		inputs = FitedArray<SRDAGEdge*,MAX_EDGE_ARRAY>(MAX_SRDAG_XPLODE_EDGES);
		outputs = FitedArray<SRDAGEdge*,MAX_EDGE_ARRAY>(1);
		fctIx = XPLODE_FUNCT_IX;
		haveSubGraph	= false;
		for(int i=0; i<MAX_SLAVE_TYPES; i++){
			constraints[i] = true;
			execTime[i] = SYNC_TIME;
		}
		break;
	case RoundBuffer:
		inputs = FitedArray<SRDAGEdge*,MAX_EDGE_ARRAY>(1);
		outputs = FitedArray<SRDAGEdge*,MAX_EDGE_ARRAY>(1);
		fctIx = RB_FUNCT_IX;

		haveSubGraph	= false;
		for(int i=0; i<MAX_SLAVE_TYPES; i++){
			constraints[i] = true;
			execTime[i] = SYNC_TIME;
		}
		break;
	case Broadcast:
		inputs = FitedArray<SRDAGEdge*,MAX_EDGE_ARRAY>(1);
		outputs = FitedArray<SRDAGEdge*,MAX_EDGE_ARRAY>(MAX_SRDAG_XPLODE_EDGES);
		fctIx = BROADCAST_FUNCT_IX;
		haveSubGraph	= false;
		for(int i=0; i<MAX_SLAVE_TYPES; i++){
			constraints[i] = true;
			execTime[i] = SYNC_TIME;
		}
		break;
	case Init:
		outputs = FitedArray<SRDAGEdge*,MAX_EDGE_ARRAY>(1);
		fctIx = INIT_FUNCT_IX;
		haveSubGraph	= false;
		for(int i=0; i<MAX_SLAVE_TYPES; i++){
			constraints[i] = true;
			execTime[i] = SYNC_TIME;
		}
		break;
	case End:
		inputs = FitedArray<SRDAGEdge*,MAX_EDGE_ARRAY>(1);
		fctIx = END_FUNCT_IX;
		haveSubGraph	= false;
		for(int i=0; i<MAX_SLAVE_TYPES; i++){
			constraints[i] = true;
			execTime[i] = SYNC_TIME;
		}
		break;
	}
}

void SRDAGVertex::updateState(){
	if(state == SRDAG_NotExecuted){
		switch(type){
		case ConfigureActor:
			state = SRDAG_Executable;
			break;
		case RoundBuffer:
			if(getNbInputEdge() == 1 && getNbOutputEdge() == 1)
				state = SRDAG_Executable;
			else
				state = SRDAG_NotExecuted;
			break;
		default:
			for (int i = 0; i < getNbInputEdge(); i++){
				SRDAGVertex* predecessor = getInputEdge(i)->getSource();

				if(predecessor->isHierarchical()){
					state = SRDAG_NotExecuted;
					return;
				}

				if(predecessor->state == SRDAG_NotExecuted){
					predecessor->updateState();
					if(predecessor->state == SRDAG_NotExecuted){
						state = SRDAG_NotExecuted;
						return;
					}
				}
			}
			state = SRDAG_Executable;
		}
	}
}

void SRDAGVertex::getName(char* name, UINT32 sizeMax){
	int len;
	switch(type){
	case Normal:
	case ConfigureActor:
		len = snprintf(name,MAX_VERTEX_NAME_SIZE,"%s_%d_%d",reference->getName(),itrIx, refIx);
		break;
	case Explode:
		len = snprintf(name,MAX_VERTEX_NAME_SIZE,"Exp_%d", id);
		break;
	case Implode:
		len = snprintf(name,MAX_VERTEX_NAME_SIZE,"Imp_%d", id);
		break;
	case RoundBuffer:
		len = snprintf(name,MAX_VERTEX_NAME_SIZE,"RB_%d", id);
		break;
	case Broadcast:
		len = snprintf(name,MAX_VERTEX_NAME_SIZE,"BR_%d", id);
		break;
	case Init:
		len = snprintf(name,MAX_VERTEX_NAME_SIZE,"Init_%d", id);
		break;
	case End:
		len = snprintf(name,MAX_VERTEX_NAME_SIZE,"End_%d", id);
		break;
	}
	if(len > MAX_VERTEX_NAME_SIZE)
		exitWithCode(1075);
}
