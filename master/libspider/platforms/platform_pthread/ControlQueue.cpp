/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2017) :
 *
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2016)
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
#include "ControlQueue.h"

#include <platform.h>

ControlQueue::ControlQueue(std::uint64_t msgSizeMax, bool isCircular) : spiderQueue_(isCircular) {
    msgSizeMax_ = msgSizeMax;

    msgBufferRecv_ = (void *) CREATE_MUL(ARCHI_STACK, msgSizeMax, std::uint8_t);
    curMsgSizeRecv_ = 0;

    msgBufferSend_ = (void *) CREATE_MUL(ARCHI_STACK, msgSizeMax, std::uint8_t);
    curMsgSizeSend_ = 0;
}

ControlQueue::~ControlQueue() {
    StackMonitor::free(ARCHI_STACK, msgBufferRecv_);
    StackMonitor::free(ARCHI_STACK, msgBufferSend_);
}

void *ControlQueue::push_start(std::uint64_t size) {
    if (curMsgSizeSend_) {
        throw std::runtime_error("ERROR: Trying to send a msg when previous one is not sent.");
    }
    curMsgSizeSend_ = size;
    return msgBufferSend_;
}

void ControlQueue::push_end(std::uint64_t /*size*/) {
    /** Push data to the queue */
    spiderQueue_.push(curMsgSizeSend_, msgBufferSend_);

    /** Reset curMsgSizeSend_ to allow sending new data */
    curMsgSizeSend_ = 0;
}

void ControlQueue::rst() {
    spiderQueue_.clear();
}

std::uint64_t ControlQueue::pop_start(void **data, bool blocking) {
    /** Read data from the queue */
    curMsgSizeRecv_ = spiderQueue_.pop(&msgBufferRecv_, blocking, msgSizeMax_);
    *data = msgBufferRecv_;
    return curMsgSizeRecv_;
}

void ControlQueue::pop_end() {
    curMsgSizeRecv_ = 0;
}
