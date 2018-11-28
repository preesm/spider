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

#ifndef SPIDER_NOTIFICATIONQUEUE_H
#define SPIDER_NOTIFICATIONQUEUE_H

#include <queue>
#include <mutex>
#include <semaphore.h>
#include <monitor/StackMonitor.h>

/**
 * @brief Thread safe implementation of std::queue
 */
class NotificationQueue {
public:
    /**
     * @brief Constructor
     */
    NotificationQueue();

    /**
     * @brief Destructor
     */
    ~NotificationQueue();


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
    std::uint8_t  pop(std::uint8_t *data, bool blocking);


    /**
     * @brief Clear the queue (thread safe)
     */
    void clear(void);

private:
    std::queue<std::uint8_t> queue_;
    std::mutex queueMutex_;
    sem_t queueCounter_;
};

#endif //SPIDER_NOTIFICATIONQUEUE_H
