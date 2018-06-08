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

#include "TraceQueue.h"

#include <lrt.h>

TraceQueue::TraceQueue(int msgSizeMax, int nLrt) {
    msgSizeMax_ = msgSizeMax;
    nLrt_ = nLrt;

    msgBufferRecv_ = (void *) CREATE_MUL(ARCHI_STACK, msgSizeMax, char);
    curMsgSizeRecv_ = 0;

    msgBufferSend_ = CREATE_MUL(ARCHI_STACK, nLrt_ + 1, void*);
    curMsgSizeSend_ = CREATE_MUL(ARCHI_STACK, nLrt_ + 1, int);
    for (int i = 0; i < nLrt_ + 1; i++) {
        msgBufferSend_[i] = (void *) CREATE_MUL(ARCHI_STACK, msgSizeMax, char);
        curMsgSizeSend_[i] = 0;
    }

    sem_init(&queue_sem_, 0, 0);
}

TraceQueue::~TraceQueue() {
    sem_destroy(&queue_sem_);

    StackMonitor::free(ARCHI_STACK, msgBufferRecv_);

    for (int i = 0; i < nLrt_ + 1; i++) {
        StackMonitor::free(ARCHI_STACK, msgBufferSend_[i]);
    }
    StackMonitor::free(ARCHI_STACK, msgBufferSend_);
    StackMonitor::free(ARCHI_STACK, curMsgSizeSend_);
}

void *TraceQueue::push_start(int lrtIx, int size) {
    if (curMsgSizeSend_[lrtIx])
        throw std::runtime_error("LrtCommunicator: Try to send a msg when previous one is not sent");
    curMsgSizeSend_[lrtIx] = size;
    return msgBufferSend_[lrtIx];
}

void TraceQueue::push_end(int lrtIx, int size) {
    unsigned long s = curMsgSizeSend_[lrtIx];

    /** Take Mutex protecting the Queue */
    queue_mutex_.lock();

    /** First bytes correspond to message size */
    for (unsigned int i = 0; i < sizeof(unsigned long); i++)
        queue_.push(s >> (sizeof(unsigned long) - 1 - i) * 8 & 0xFF);

    /** Send the message */
    for (int i = 0; i < curMsgSizeSend_[lrtIx]; i++)
        queue_.push(*(((char *) msgBufferSend_[lrtIx]) + i) & 0xFF);

    /** Relax Mutex protecting the Queue */
    queue_mutex_.unlock();

    /** Post 1 token in semaphore representing a message */
    sem_post(&queue_sem_);

    curMsgSizeSend_[lrtIx] = 0;
}

int TraceQueue::pop_start(void **data, bool blocking) {
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

void TraceQueue::pop_end() {
    curMsgSizeRecv_ = 0;
}