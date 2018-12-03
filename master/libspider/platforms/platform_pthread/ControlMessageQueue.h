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

#ifndef SPIDER_CONTROLMESSAGEQUEUE_H
#define SPIDER_CONTROLMESSAGEQUEUE_H


#include <cstdint>
#include <queue>
#include <Message.h>
#include <mutex>

/**
 * @brief Thread safe Message Queue using std::vector for message storage and std::queue for index management;
 *        This implementation is index based for retrieval of message.
 *        Index is returned at push and is needed for pop.
 * @tparam T Type of message
 */

template<class T>
class ControlMessageQueue {
public:
    ControlMessageQueue() = default;

    ~ControlMessageQueue() {
        msgQueueMutex_.lock();
        /** Clearing the message queue **/
        msgQueue_.clear();
        msgQueueMutex_.unlock();
        indexesQueueMutex_.lock();
        /** Clearing the indexes queue **/
        while (!indexesQueue_.empty()) {
            indexesQueue_.pop();
        }
        indexesQueueMutex_.unlock();
    }

    /**
     * @brief Retrieve the message of given id.
     * @param message  Message to be filled.
     * @param id       Id of the job.
     */
    bool pop(T *message, std::int32_t id);

    /**
     * @brief  Push a message in the queue
     * @param message message to push
     * @return ID of the JobMessage in the Queue
     */
    std::int32_t push(T *message);

private:
    /** Queue of the first available free space in queue */
    std::queue<std::int32_t> indexesQueue_;
    std::mutex indexesQueueMutex_;

    /**
     * @brief Thread safe function to get next free index in queue
     * @return next free index (if any), -1 if none are available
     */
    std::int32_t getNextFreeIndex();

    /**
     * @brief Thread safe function to set add a free index in queue
     * @param index Index to add
     */
    void setNextFreeIndex(std::int32_t index);

    /** Queue of JobMessage */
    std::vector<T> msgQueue_;
    std::mutex msgQueueMutex_;
};

/**
 * @brief Defining templates here due to some linker issues.
 */
template
class ControlMessageQueue<JobMessage *>;

template
class ControlMessageQueue<ParameterMessage *>;

#endif //SPIDER_CONTROLMESSAGEQUEUE_H
