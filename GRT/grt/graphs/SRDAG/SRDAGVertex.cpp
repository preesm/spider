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

#include "SRDAGVertex.h"
#include <cstdio>
#include <cstring>

/**
 Constructor
*/
SRDAGVertex::SRDAGVertex(){
	graph=0;
	scheduleIndex = -1;
	visited=0;
	mergeIx = -1;
	referenceIndex=-1;
	iterationIndex=-1;
	schedLevel=-1;
	slaveIndex=-1;
	expImpId=-1;
	Reference = NULL;
	EdgeReference = NULL;
	id=-1;
	functIx=-1;
	minStartTime =-1;
	parent=NULL;
	type = Normal; // Normal type by default.
	state = SrVxStNoExecuted;
	execTime = 0;
	inputEdges.reset();
	outputEdges.reset();
}

void SRDAGVertex::reset(){
	graph=0;
	scheduleIndex = -1;
	visited=0;
	mergeIx = -1;
	referenceIndex=-1;
	iterationIndex=-1;
	schedLevel=-1;
	slaveIndex=-1;
	expImpId=-1;
	Reference = NULL;
	EdgeReference = NULL;
	id=-1;
	functIx=-1;
	minStartTime =-1;
	parent=NULL;
	type = Normal; // Normal type by default.
	state = SrVxStNoExecuted;
	execTime = 0;
	inputEdges.reset();
	outputEdges.reset();
}

/**
 Destructor
*/
SRDAGVertex::~SRDAGVertex()
{
}


void SRDAGVertex::removeInputEdge(SRDAGEdge* edge){
	inputEdges.remove(edge);
}

void SRDAGVertex::removeInputEdgeIx(UINT32 ix){
	inputEdges.remove(ix);
}

void SRDAGVertex::removeOutputEdge(SRDAGEdge* edge){
	outputEdges.remove(edge);
}

void SRDAGVertex::removeOutputEdgeIx(UINT32 ix){
	outputEdges.remove(ix);
}

int SRDAGVertex::getInputEdgeId(SRDAGEdge* edge){
	return inputEdges.getIdOf(edge);
}

int SRDAGVertex::getOutputEdgeId(SRDAGEdge* edge){
	return outputEdges.getIdOf(edge);
}

void SRDAGVertex::updateState(){
	if(state != SrVxStExecuted && state != SrVxStDeleted){
		switch(type){
		case ConfigureActor:
			state = SrVxStExecutable;
			break;
		case RoundBuffer:
			if(inputEdges.getNb() == 1 && outputEdges.getNb() == 1)
				state = SrVxStExecutable;
			else
				state = SrVxStNoExecuted;
			break;
		default:
			for (UINT32 i = 0; i < inputEdges.getNb(); i++){
				SRDAGVertex* predecessor = inputEdges[i]->getSource();

				if(predecessor->state == SrVxStNoExecuted)
					predecessor->updateState();

				if(predecessor->state == SrVxStNoExecuted
						|| predecessor->isHierarchical()){
					state = SrVxStNoExecuted;
					return;
				}
			}
			state = SrVxStExecutable;
		}
	}
}

void SRDAGVertex::getName(char* name, UINT32 sizeMax){
	UINT32 len;
	switch (type) {
		case Normal: // Normal vertex.
		case ConfigureActor:
			// TODO: Handle this and below lines for CSDAG vertices : len = snprintf(name,MAX_VERTEX_NAME_SIZE,"%s_%d",vertex->getCsDagReference()->getName(),vertex->getReferenceIndex());
			len = snprintf(name,MAX_VERTEX_NAME_SIZE,"%s_%d_%d",Reference->getName(),iterationIndex, referenceIndex);
			if(len > MAX_VERTEX_NAME_SIZE)
				exitWithCode(1075);
			break;
		case Explode: // Explode vertex.
			len = snprintf(name,MAX_VERTEX_NAME_SIZE,"%s_%d","Exp", expImpId);
			if(len > MAX_VERTEX_NAME_SIZE)
				exitWithCode(1075);
			break;
		case Implode: // Implode vertex.
			len = snprintf(name,MAX_VERTEX_NAME_SIZE,"%s_%d","Imp", expImpId);
			if(len > MAX_VERTEX_NAME_SIZE)
				exitWithCode(1075);
			break;
		case RoundBuffer: // Round buffer
			len = snprintf(name,MAX_VERTEX_NAME_SIZE,"RB");
			if(len > MAX_VERTEX_NAME_SIZE)
				exitWithCode(1075);
			break;
		case Init:
			len = snprintf(name,MAX_VERTEX_NAME_SIZE,"Init");
			if(len > MAX_VERTEX_NAME_SIZE)
				exitWithCode(1075);
			break;
		case End:
			len = snprintf(name,MAX_VERTEX_NAME_SIZE,"End");
			if(len > MAX_VERTEX_NAME_SIZE)
				exitWithCode(1075);
			break;
		case Broadcast:
			len = snprintf(name,MAX_VERTEX_NAME_SIZE,"Broadcast");
			if(len > MAX_VERTEX_NAME_SIZE)
				exitWithCode(1075);
			break;
		default:
			len = snprintf(name,MAX_VERTEX_NAME_SIZE,"undef");
			if(len > MAX_VERTEX_NAME_SIZE)
				exitWithCode(1075);
			break;
	}
}
