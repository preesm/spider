/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet,            *
 * Hugo Miomandre                                                           *
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

#include "DummyMemAlloc.h"

#include <graphs/SRDAG/SRDAGCommon.h>
#include <graphs/SRDAG/SRDAGGraph.h>
#include <graphs/SRDAG/SRDAGVertex.h>

#include <platform.h>

#include <cmath>


void DummyMemAlloc::reset(){
	currentMem_ = this->memStart_;
	nbFifos_ = 0;
}

static inline int getAlignSize(int size){
	//return std::ceil(size/1.0/getpagesize())*getpagesize();
	float minAlloc = (float) Platform::get()->getMinAllocSize();
	return (int)ceil(((float)size)/minAlloc)*minAlloc;
}

void DummyMemAlloc::alloc(List<SRDAGVertex*>* listOfVertices){
	for(int i=0; i<listOfVertices->getNb(); i++){
		SRDAGVertex* vertex = listOfVertices->operator [](i);
		if(vertex->getState() == SRDAG_EXEC){
			for(int j=0; j<vertex->getNConnectedOutEdge(); j++){
				SRDAGEdge* edge = vertex->getOutEdge(j);
				if(edge->getAlloc() == -1){
					int size = edge->getRate();
					size = getAlignSize(size);
					if(currentMem_+size > memStart_ + memSize_)
						throw "Not Enough Shared Memory\n";
					edge->setAlloc(currentMem_);
					edge->setAllocIx(nbFifos_++);
					currentMem_ += size;
				}
			}
		}
	}
}

int DummyMemAlloc::getMemUsed(){
	return currentMem_ - memStart_;
}
