/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2017) :
 *
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2015)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013 - 2014)
 *
 * Spider is a dataflow based runtime used to execute dynamic PiSDF
 * applications. The Preesm tool may be used to design PiSDF applications.
 *
 * This software is governed by the CeCILL  license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */
#include "DummyMemAlloc.h"

#include <cmath>

void DummyMemAlloc::reset() {
    currentMem_ = this->memStart_ + this->memReserved_;
}

static inline int getAlignSize(int size) {
    //return std::ceil(size/1.0/getpagesize())*getpagesize();
    float minAlloc = (float) Platform::get()->getMinAllocSize();
    return (int) std::ceil(((float) size) / minAlloc) * minAlloc;
}


void DummyMemAlloc::allocEdge(SRDAGEdge *edge) {
    int alloc;
    int size = edge->getRate();
    size = getAlignSize(size);
    if (currentMem_ + size > memStart_ + memSize_)
        throw std::runtime_error("Not Enough Shared Memory\n");
    edge->setAlloc(currentMem_);
    currentMem_ += size;
}

void DummyMemAlloc::alloc(List<SRDAGVertex *> *listOfVertices) {
    for (int i = 0; i < listOfVertices->getNb(); i++) {
        SRDAGVertex *vertex = listOfVertices->operator[](i);
        if (vertex->getState() == SRDAG_EXEC) {
            for (int j = 0; j < vertex->getNConnectedOutEdge(); j++) {
                allocEdge(vertex->getOutEdge(j));
            }
        }
    }
}

int DummyMemAlloc::getReservedAlloc(int size) {
    int alignedSize = getAlignSize(size);
    if (currentMem_ + alignedSize > memStart_ + memSize_) {
        throw std::runtime_error("Not Enough Shared Memory\n");
    }
    currentMem_ += alignedSize;
    return alignedSize;
}

int DummyMemAlloc::getMemUsed() {
    return currentMem_ - memStart_;
}
