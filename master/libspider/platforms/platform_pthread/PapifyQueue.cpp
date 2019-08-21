/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2018 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2019)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2018)
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
#include "PapifyQueue.h"

PapifyQueue::PapifyQueue(std::uint64_t msgSizeMax, int nLrt) {
    msgSizeMax_ = msgSizeMax;
    nLrt_ = nLrt;

    msgBufferRecv_ = (void *) CREATE_MUL(ARCHI_STACK, msgSizeMax, std::uint8_t);
    curMsgSizeRecv_ = 0;

    msgBufferSend_ = CREATE_MUL(ARCHI_STACK, nLrt_ + 1, void*);
    curMsgSizeSend_ = CREATE_MUL(ARCHI_STACK, nLrt_ + 1, std::uint64_t);
    for (int i = 0; i < nLrt_ + 1; i++) {
        msgBufferSend_[i] = (void *) CREATE_MUL(ARCHI_STACK, msgSizeMax, std::uint8_t);
        curMsgSizeSend_[i] = 0;
    }
}

PapifyQueue::~PapifyQueue() {
    StackMonitor::free(ARCHI_STACK, msgBufferRecv_);

    for (int i = 0; i < nLrt_ + 1; i++) {
        StackMonitor::free(ARCHI_STACK, msgBufferSend_[i]);
    }
    StackMonitor::free(ARCHI_STACK, msgBufferSend_);
    StackMonitor::free(ARCHI_STACK, curMsgSizeSend_);
}

void *PapifyQueue::push_start(int lrtIx, std::uint64_t size) {
    if (curMsgSizeSend_[lrtIx]) {
        throwSpiderException("Can not send new message while previous one is not sent.");
    }
    curMsgSizeSend_[lrtIx] = size;
    return msgBufferSend_[lrtIx];
}

void PapifyQueue::push_end(int lrtIx, std::uint64_t /*size*/) {
    if (lrtIx >= nLrt_ || lrtIx < 0) {
        throwSpiderException("Bad lrtIx for PapifyQueue. Value: %d -- Max: %d", lrtIx, nLrt_);
    }

    /** Push data to the queue */
    spiderQueue_.push(curMsgSizeSend_[lrtIx], msgBufferSend_);

    /** Reset curMsgSizeSend_ to allow sending new data */
    curMsgSizeSend_[lrtIx] = 0;
}

std::uint64_t PapifyQueue::pop_start(void **data, bool blocking) {
    /** Read data from the queue */
    curMsgSizeRecv_ = spiderQueue_.pop(&msgBufferRecv_, blocking, msgSizeMax_);
    *data = msgBufferRecv_;
    return curMsgSizeRecv_;
}

void PapifyQueue::pop_end() {
    curMsgSizeRecv_ = 0;
}
