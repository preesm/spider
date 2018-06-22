/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2018)
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
#include "PThreadLrtCommunicator.h"

#ifndef _WIN32

#endif

#include <platform.h>

PThreadLrtCommunicator::PThreadLrtCommunicator(
        ControlQueue *spider2LrtQueue,
        ControlQueue *lrt2SpiderQueue,
        DataQueues *dataQueues,
        TraceQueue *traceQueue
) {
    spider2LrtQueue_ = spider2LrtQueue;
    lrt2SpiderQueue_ = lrt2SpiderQueue;
    dataQueues_ = dataQueues;
    traceQueue_ = traceQueue;
}

PThreadLrtCommunicator::~PThreadLrtCommunicator() {
}

void *PThreadLrtCommunicator::ctrl_start_send(int size) {
    return lrt2SpiderQueue_->push_start(size);
}

void PThreadLrtCommunicator::ctrl_end_send(int /*size*/) {
    return lrt2SpiderQueue_->push_end(size);
}

int PThreadLrtCommunicator::ctrl_start_recv(void **data) {
    return spider2LrtQueue_->pop_start(data, false);
}

void PThreadLrtCommunicator::ctrl_start_recv_block(void **data) {
    spider2LrtQueue_->pop_start(data, true);
}

void PThreadLrtCommunicator::ctrl_end_recv() {
    return spider2LrtQueue_->pop_end();
}

void *PThreadLrtCommunicator::trace_start_send(int size) {
    return traceQueue_->push_start(Platform::get()->getLrtIx(), size);
}

void PThreadLrtCommunicator::trace_end_send(int /*size*/) {
    return traceQueue_->push_end(Platform::get()->getLrtIx(), size);
}

void PThreadLrtCommunicator::data_end_send(Fifo */*f*/) {
    // Nothing to do
}

void *PThreadLrtCommunicator::data_recv(Fifo *f) {
    return (void *) Platform::get()->virt_to_phy((void *) (intptr_t) (f->alloc));
}

void *PThreadLrtCommunicator::data_start_send(Fifo *f) {
    return (void *) Platform::get()->virt_to_phy((void *) (intptr_t) (f->alloc));
}

void PThreadLrtCommunicator::setLrtJobIx(int lrtIx, int jobIx) {
    dataQueues_->updateLrtJobStamp(lrtIx, jobIx);
}

void PThreadLrtCommunicator::waitForLrtUnlock(int nbDependency, int *blkLrtIx, int *blkLrtJobIx, int /*jobIx*/) {
    for (int i = 0; i < nbDependency; i++) {
        dataQueues_->waitOnJobStamp(Platform::get()->getLrtIx(), blkLrtIx[i], blkLrtJobIx[i], true);
    }
}
