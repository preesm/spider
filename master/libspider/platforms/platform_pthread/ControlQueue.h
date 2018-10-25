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
#ifndef SPIDER_CONTROLQUEUE_H
#define SPIDER_CONTROLQUEUE_H


#include <mutex>
#include <queue>
#include <semaphore.h>

/**
 * Thread safe mono directional queue with only one reader and one writer.
 */
class ControlQueue {

public:
    /**
     * Constructor.
     * @param msgSizeMax largest possible message size.
     */
    ControlQueue(int msgSizeMax);

    /**
     * Destructor.
     */
    virtual ~ControlQueue();

    /**
     * Prepare a message to be send.
     * @param size Size needed by the message.
     * @return Ptr to data were to write the message.
     */
    void *push_start(int size);

    /**
     * Actually send the message prepared in @push_start.
     * @param size Size needed by the message.
     */
    void push_end(int size);

    /**
     * Receive a message from the queue.
     * @param data Ptr to the message will be store in this argument.
     * @param blocking True if the @pop_start should wait for a message if none is available.
     * @return 0 if no message have been received, size of the message otherwise.
     */
    int pop_start(void **data, bool blocking);

    /**
     * Free the data to allow the reception of a new message.
     * data from @pop_start should not be used after this call.
     */
    void pop_end();

private:
    std::queue<unsigned char> queue_;
    std::mutex queue_mutex_;
    sem_t queue_sem_;

    int msgSizeMax_;

    void *msgBufferSend_;
    int curMsgSizeSend_;

    void *msgBufferRecv_;
    int curMsgSizeRecv_;
};


#endif //SPIDER_CONTROLQUEUE_H
