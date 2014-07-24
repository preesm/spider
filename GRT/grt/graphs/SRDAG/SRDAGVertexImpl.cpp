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

#include "SRDAGVertexAbstract.h"
#include "SRDAGVertexBroadCast.h"
#include "SRDAGVertexConfig.h"
#include "SRDAGVertexInitEnd.h"
#include "SRDAGVertexNormal.h"
#include "SRDAGVertexRB.h"
#include "SRDAGVertexXplode.h"

#include <graphs/PiSDF/PiSDFConfigVertex.h>

int SRDAGVertexAbstract::creationIx = 0;

SRDAGVertexAbstract::SRDAGVertexAbstract(){
	id 				= -1;
	graph 			= NULL;
	type 			= Normal;
	state 			= SRDAG_NotExecuted;
	reference		= NULL;
	refIx 			= -1;
	itrIx 			= -1;
	schedLevel 		= -1;
	slaveIndex 		= -1;
	scheduleIndex 	= -1;
	minStartTime 	= -1;
	setIx 			= -1;
}

SRDAGVertexAbstract::SRDAGVertexAbstract(
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
	scheduleIndex 	= -1;
	minStartTime 	= -1;
	setIx 			= -1;
}

void SRDAGVertexAbstract::updateState(){
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
				SRDAGVertexAbstract* predecessor = getInputEdge(i)->getSource();

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

SRDAGVertexBroadcast::SRDAGVertexBroadcast(
		SRDAGGraph* 	_graph,
		int 			_refIx,
		int 			_itrIx,
		PiSDFVertex* ref):
		SRDAGVertexAbstract(_graph, Broadcast, ref, _refIx,_itrIx){
	inputEdges.reset();
	outputEdges.reset();
}

SRDAGVertexConfig::SRDAGVertexConfig(
		SRDAGGraph* 	_graph,
		int 			_refIx,
		int 			_itrIx,
		PiSDFConfigVertex* ref):
		SRDAGVertexAbstract(_graph, ConfigureActor, ref, _refIx,_itrIx){
	inputEdges.reset();
	outputEdges.reset();

	for(int i=0; i<MAX_SLAVE_TYPES; i++){
		if(reference->getConstraints(i)){
			constraints[i] = true;
			execTime[i] =  reference->getResolvedTiming(i);
		}else{
			constraints[i] = false;
		}
	}

	for(int i=0; i<ref->getNbParameters(); i++)
		paramValues[i] = reference->getParameter(i)->getValue();
}

SRDAGVertexNormal::SRDAGVertexNormal(
		SRDAGGraph* 	_graph,
		int 			_refIx,
		int 			_itrIx,
		PiSDFVertex* ref):
		SRDAGVertexAbstract(_graph, Normal, ref, _refIx,_itrIx){
	inputEdges.reset();
	outputEdges.reset();

	for(int i=0; i<MAX_SLAVE_TYPES; i++){
		if(reference->getConstraints(i)){
			constraints[i] = true;
			execTime[i] =  reference->getResolvedTiming(i);
		}else{
			constraints[i] = false;
		}
	}

	for(int i=0; i<reference->getNbParameters(); i++)
		paramValues[i] = reference->getParameter(i)->getValue();
}

SRDAGVertexInitEnd::SRDAGVertexInitEnd(
		SRDAGGraph* 	_graph,
		SRDAGVertexType _type,
		int 			_refIx,
		int 			_itrIx):
		SRDAGVertexAbstract(_graph, _type, NULL, _refIx,_itrIx){
	edges.reset();
}

SRDAGVertexRB::SRDAGVertexRB(
		SRDAGGraph* 	_graph,
		int 			_refIx,
		int 			_itrIx,
		PiSDFAbstractVertex* ref):
		SRDAGVertexAbstract(_graph, RoundBuffer, ref, _refIx,_itrIx){
	outputEdges.reset();
	inputEdges.reset();
}

SRDAGVertexXplode::SRDAGVertexXplode(
		SRDAGGraph* 	_graph,
		SRDAGVertexType _type,
		int 			_refIx,
		int 			_itrIx):
		SRDAGVertexAbstract(_graph, _type, NULL, _refIx,_itrIx){
	gatherEdges.reset();
	scatterEdges.reset();
}
