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

#ifndef SPIDER_SPIDERQUEUE_H
#define SPIDER_SPIDERQUEUE_H

#include <queue>
#include <mutex>
#include <semaphore.h>
#include <monitor/StackMonitor.h>
#include <SpiderException.h>
#include <cinttypes>

/**
 * @brief Thread safe implementation of std::queue with circular pop capability
 */
template<typename T>
class SpiderQueue {
public:
    /**
     * @brief Constructor
     * @param maxSize Maximum size of the queue
     * @param stackID ID of the stack to use to store data of the queue (ARCHI_STACK by default)
     */
    SpiderQueue();

    SpiderQueue(bool isCircular);

    /**
     * @brief Destructor
     */
    ~SpiderQueue();

    /**
     * @brief Push a value to the queue
     *
     * @param value Value to be pushed
     */
    void push(T &value);

    /**
     * @brief Push bufferSize values to the queue with the bufferSize
     *
     * @param bufferSize Size of the buffer to push
     * @param buffer     Buffer containing the bufferSize <T> values to push
     *
     * @remark The queue semaphore is only posted once. It is user responsability to know what is inside the queue.
     */
    void push(std::uint64_t &bufferSize, void *buffer);

    /**
     * @brief Pop data from the queue if it contains any. This method consider buffer data in the queue.
     * Method reads buffer size from first N bytes (depending on sizeof(T)) and then reads buffer into data.
     *
     * @param data      Buffer to be filled with buffer data
     * @param blocking  Flag to wait (true) if queue is empty or return (false).
     * @param maxSize   Maximum size allowed for the read buffer.
     *
     * @return Size of the read buffer, 0 if queue is empty and blocking is set to false.
     */
    std::uint64_t pop(void **data, bool blocking, std::uint64_t &maxSize);

    /**
     * @brief Read one value in the queue if it contains any.
     *
     * @param data      Pointer to data to be filled with queue content.
     * @param blocking  Flag to wait (true) if queue is empty or return (false).
     */
    void pop(T *data, bool blocking);

    /**
     * @brief Clear the queue (thread safe)
     */
    void clear();

private:
    std::vector<T> queue_;
    std::mutex queueMutex_;
    sem_t queueCounter_;
    std::uint8_t queueBufferSizeNBytes_;
    std::uint32_t queueIndex_;
    std::uint32_t queueSize_;

    bool isCircular_;
};

template<typename T>
SpiderQueue<T>::SpiderQueue() : queueIndex_(0), queueSize_(0), isCircular_(false) {
    if (sizeof(std::uint64_t) % sizeof(T)) {
        throwSpiderException("SpiderQueue item is not byte aligned with sizeof(std::uint64_t).");
    } else if (sizeof(T) > sizeof(std::uint64_t)) {
        throwSpiderException("Byte size of SpiderQueue item should be <= sizeof(std::uint64_t).");
    }
    queueBufferSizeNBytes_ = sizeof(std::uint64_t) / sizeof(T);
    sem_init(&queueCounter_, 0, 0);
}

template<typename T>
SpiderQueue<T>::SpiderQueue(bool isCircular) : queueIndex_(0), queueSize_(0), isCircular_(isCircular) {
    if (sizeof(std::uint64_t) % sizeof(T)) {
        throwSpiderException("SpiderQueue item is not byte aligned with sizeof(std::uint64_t).");
    } else if (sizeof(T) > sizeof(std::uint64_t)) {
        throwSpiderException("Byte size of SpiderQueue item should be <= sizeof(std::uint64_t).");
    }
    queueBufferSizeNBytes_ = sizeof(std::uint64_t) / sizeof(T);
    sem_init(&queueCounter_, 0, 0);
}

template<typename T>
SpiderQueue<T>::~SpiderQueue() {
    sem_destroy(&queueCounter_);
}

template<typename T>
void SpiderQueue<T>::push(T &value) {
    {
        /** Locking mutex with guard (in case of exception) */
        std::lock_guard<std::mutex> lock(queueMutex_);

        /** Pushing value */
        //queue_.push(value);
        queue_.push_back(value);

        /** Increase queueSize_ */
        queueSize_++;
    }
    /** Posting queue semaphore to signal item is added inside */
    sem_post(&queueCounter_);
}

template<typename T>
void SpiderQueue<T>::push(std::uint64_t &bufferSize, void *buffer) {
    /** Creating a scope for lock_guard */
    {
        /** Locking mutex with guard (in case of exception) */
        std::lock_guard<std::mutex> lock(queueMutex_);

        /** Filling queue with buffer size bytes */
        auto bufferSizeAsArray = (T *) (&bufferSize);
        for (std::uint8_t i = 0; i < queueBufferSizeNBytes_; ++i) {
            //queue_.push(bufferSizeAsArray[(queueBufferSizeNBytes_ - 1) - i]);
            queue_.push_back(bufferSizeAsArray[(queueBufferSizeNBytes_ - 1) - i]);
        }

        /** Filling queue with buffer data */
        auto convertedBuffer = (T *) buffer;
        for (std::uint64_t i = 0; i < bufferSize; i++) {
            auto currentValue = convertedBuffer[i];
            //queue_.push(currentValue);
            queue_.push_back(currentValue);
        }

        /** Increase queueSize_ */
        queueSize_++;
    }

    /** Posting queue semaphore to signal item is added inside */
    sem_post(&queueCounter_);
}

template<typename T>
static inline T popCircular(std::vector<T> &queue, std::uint32_t &index) {
    return queue[index++];
}

template<typename T>
static inline T popNonCircular(std::vector<T> &queue, std::uint32_t &/*index*/) {
    auto value = queue.front();
    queue.erase(queue.begin());
    return value;
}


template<typename T>
std::uint64_t SpiderQueue<T>::pop(void **data, bool blocking, std::uint64_t &maxSize) {
    /** Wait until a item is pushed in the queue */
    if (blocking) {
        sem_wait(&queueCounter_);
    } else if (sem_trywait(&queueCounter_)) {
        /** If queue is empty return */
        return 0;
    }
    /** Locking mutex with guard (in case of exception) */
    std::lock_guard<std::mutex> lock(queueMutex_);

    /** If a thread has cleared the queue */
    if (queue_.empty()) {
        return 0;
    }

    T (*pop_fct)(std::vector<T> &, std::uint32_t &) = popNonCircular;
    if (isCircular_) {
        pop_fct = popCircular;
    }
    /** Retrieve buffer size */
    std::uint64_t bufferSize = 0;
    for (std::uint8_t nb = 0; nb < queueBufferSizeNBytes_; nb++) {
        bufferSize = bufferSize << queueBufferSizeNBytes_;
        //auto front = queue_.front();
        //bufferSize += front;
        bufferSize += pop_fct(queue_, queueIndex_);
        //queue_.pop();
    }

    /** Check size */
    if (bufferSize > maxSize) {
        throwSpiderException("Trying to read a message too big. Message size: %"
                                     PRIu64
                                     " -- Max size: %"
                                     PRIu64
                                     ".",
                             bufferSize, maxSize);
    }

    /** Retrieve the item from the queue */
    auto convertedBufferRcv = (T *) (*data);
    for (std::uint64_t recv = 0; recv < bufferSize; recv++) {
        //auto front = queue_.front();
        //convertedBufferRcv[recv] = front;
        //queue_.pop();
        convertedBufferRcv[recv] = pop_fct(queue_, queueIndex_);
    }
    return bufferSize;
}

template<typename T>
void SpiderQueue<T>::pop(T *data, bool blocking) {
    /** Wait until a item is pushed in the queue */
    if (blocking) {
        sem_wait(&queueCounter_);
    } else if (sem_trywait(&queueCounter_)) {
        /** If queue is empty return */
        return;
    }

    /** Locking mutex with guard (in case of exception) */
    std::lock_guard<std::mutex> lock(queueMutex_);

    /** Retrieving data */
    //(*data) = queue_.front();
    (*data) = queue_[queueIndex_++];
    /** Removing the element from the queue */
    // queue_.pop();
}

template<typename T>
void SpiderQueue<T>::clear() {
    std::lock_guard<std::mutex> lock(queueMutex_);
    queueIndex_ = 0;
    if (!isCircular_) {
        queue_.clear();
        queueSize_ = 0;
    } else {
        for (std::uint32_t i = 0; i < queueSize_; ++i) {
            sem_post(&queueCounter_);
        }
    }
}

#endif //SPIDER_SPIDERQUEUE_H
