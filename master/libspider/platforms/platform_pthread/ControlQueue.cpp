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

ControlQueue::ControlQueue(int msgSizeMax) {
    msgSizeMax_ = msgSizeMax;

    msgBufferRecv_ = (void *) CREATE_MUL(ARCHI_STACK, msgSizeMax, char);
    curMsgSizeRecv_ = 0;

    msgBufferSend_ = (void *) CREATE_MUL(ARCHI_STACK, msgSizeMax, char);
    curMsgSizeSend_ = 0;

    sem_init(&queue_sem_, 0, 0);
}

ControlQueue::~ControlQueue() {
    sem_destroy(&queue_sem_);

    StackMonitor::free(ARCHI_STACK, msgBufferRecv_);
    StackMonitor::free(ARCHI_STACK, msgBufferSend_);
}

void *ControlQueue::push_start(int size) {
    if (curMsgSizeSend_)
        throw std::runtime_error("LrtCommunicator: Try to send a msg when previous one is not sent");
    curMsgSizeSend_ = size;
    return msgBufferSend_;
}

void ControlQueue::push_end(int size) {
    unsigned long s = curMsgSizeSend_;

    /** Take Mutex protecting the Queue */
    queue_mutex_.lock();

    /** First bytes correspond to message size */
    for (unsigned int i = 0; i < sizeof(unsigned long); i++)
        queue_.push(s >> (sizeof(unsigned long) - 1 - i) * 8 & 0xFF);

    /** Send the message */
    for (int i = 0; i < curMsgSizeSend_; i++)
        queue_.push((*(((char *) msgBufferSend_) + i)) & 0xFF);

    /** Relax Mutex protecting the Queue */
    queue_mutex_.unlock();

    /** Post 1 token in semaphore representing a message */
    sem_post(&queue_sem_);

    curMsgSizeSend_ = 0;
}

int ControlQueue::pop_start(void **data, bool blocking) {
    unsigned long size = 0;

    /** Take sem (representing 1 message in the queue */
    if (blocking)
        sem_wait(&queue_sem_);
    else if (sem_trywait(&queue_sem_))
        return 0;

    /** Take Mutex protecting the Queue */
    queue_mutex_.lock();

    /** Retrieve message size */
    for (unsigned int nb = 0; nb < sizeof(unsigned long); nb++) {
        size = size << 8;
        size += queue_.front();
        queue_.pop();
    }

    /** Check size */
    if (size > (unsigned long) msgSizeMax_)
        throw std::runtime_error("Msg too big\n");

    curMsgSizeRecv_ = size;

    /** Retrieve the message */
    for (unsigned int recv = 0; recv < size; recv++) {
        *(((char *) msgBufferRecv_) + recv) = queue_.front();
        queue_.pop();
    }

    /** Relax Mutex protecting the Queue */
    queue_mutex_.unlock();

    *data = msgBufferRecv_;
    return curMsgSizeRecv_;
}

void ControlQueue::pop_end() {
    curMsgSizeRecv_ = 0;
}
