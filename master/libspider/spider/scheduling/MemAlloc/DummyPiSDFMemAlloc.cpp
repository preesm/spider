/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2018)
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
#include <cmath>
#include "DummyPiSDFMemAlloc.h"
#include <graphs/PiSDF/PiSDFGraph.h>
#include <graphs/PiSDF/PiSDFVertex.h>
#include <graphs/PiSDF/PiSDFEdge.h>

static inline int getAlignSize(Param size) {
    //return std::ceil(size/1.0/getpagesize())*getpagesize();
    auto minAlloc = (float) Platform::get()->getMinAllocSize();
    return static_cast<int>(std::ceil(((float) size) / minAlloc) * minAlloc);
}

void DummyPiSDFMemAlloc::reset() {
    currentMem_ = memStart_ + memReserved_;
}

void DummyPiSDFMemAlloc::allocEdge(PiSDFEdge *edge) {
    auto *vertex = edge->getSrc();
    auto *vertexJob = vertex->getScheduleJob();
    auto nInstance = vertexJob->getNumberOfInstances();
    auto size = edge->resolveProd() * nInstance;
    size = getAlignSize(size);
    if (currentMem_ + size > memStart_ + memSize_) {
        throwSpiderException("Not Enough Shared Memory. Want: %d -- Available: %d.", currentMem_ + size,
                             memStart_ + memSize_);
    }
    edge->setMemoryAlloc(currentMem_);
    currentMem_ += size;
}

void DummyPiSDFMemAlloc::alloc(PiSDFGraph *graph) {
    for (int ix = 0; ix < graph->getNBody(); ++ix) {
        auto *vertex = graph->getBody(ix);
        auto *vertexJob = vertex->getScheduleJob();
        auto nInstance = vertexJob->getNumberOfInstances();
        for (int i = 0; i < vertex->getNOutEdge(); ++i) {
            auto *edge = vertex->getOutEdge(i);
            auto size = edge->resolveProd() * nInstance;
            size = getAlignSize(size);
            if (currentMem_ + size > memStart_ + memSize_) {
                throwSpiderException("Not Enough Shared Memory. Want: %d -- Available: %d.", currentMem_ + size,
                                     memStart_ + memSize_);
            }
            edge->setMemoryAlloc(currentMem_);
            currentMem_ += size;
        }
    }
}

int DummyPiSDFMemAlloc::getReservedAlloc(int size) {
    int alignedSize = getAlignSize(size);
    if (currentMem_ + alignedSize > memStart_ + memSize_) {
        throwSpiderException("Not Enough Shared Memory. Want: %d -- Available: %d.", currentMem_ + size,
                             memStart_ + memSize_);
    }
    currentMem_ += alignedSize;
    return alignedSize;
}

int DummyPiSDFMemAlloc::getMemUsed() {
    return currentMem_ - memStart_;
}


