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
#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <spider.h>
#include "monitor/StackMonitor.h"

typedef enum {
    MSG_START_JOB = 1,
    MSG_PARAM_VALUE = 2,
    MSG_CLEAR_TIME = 3,
    MSG_RESET_LRT = 4,
    MSG_END_ITER = 5,
    MSG_STOP_LRT = 6
} CtrlMsgType;


typedef enum {
    TRACE_SPIDER_GRAPH = 1,
    TRACE_SPIDER_ALLOC = 2,
    TRACE_SPIDER_SCHED = 3,
    TRACE_SPIDER_OPTIM = 4,
    TRACE_SPIDER_TMP0,
    TRACE_SPIDER_TMP1,
    TRACE_SPIDER_TMP2,
    TRACE_SPIDER_TMP3
} TraceSpiderType;


class Fifo {
public:
    std::int32_t alloc;
    std::int32_t size;
    std::int32_t blkLrtIx;
    std::int32_t blkLrtJobIx;
};

class ClearTimeMessage {
public:
    std::uint32_t id_;
    struct timespec timespec_;
};


typedef enum {
    LRT_NOTIFICATION,         // Signal that notification is about LRT state
    TRACE_NOTIFICATION,       // Signal that notification is about TRACE system
    JOB_NOTIFICATION,         // Signal that notification is about JOB information
    UNDEFINED_NOTIFICATION    // Signal that notification is undefined
} NotificationType;

typedef enum {
    LRT_END_ITERATION,        // Cross-check signal sent after last JOB (if JOB_LAST_ID was not received)
    LRT_REPEAT_ITERATION_EN,  // Signal LRT to repeat its complete iteration (indefinitely)
    LRT_REPEAT_ITERATION_DIS, // Signal LRT to stop repeating iteration
    LRT_FINISHED_ITERATION,   // Signal that given LRT has finished its iteration
    LRT_RST_ITERATION,        // Signal LRT to restart current iteration
    LRT_STOP,                 // Signal LRT to stop
    LRT_PAUSE,                // Signal LRT to freeze
    LRT_RESUME,               // Signal LRT to un-freeze
} LRTNotificationType;

typedef enum {
    TRACE_ENABLE,    // Signal LRT to enable its trace
    TRACE_DISABLE,   // Signal LRT to disable its trace
    TRACE_RST,       // Signal LRT to reset its trace
    TRACE_SENT,      // Signal that a trace has been sent
    TRACE_LRT,
    TRACE_SPIDER
} TraceNotificationType;

typedef enum {
    JOB_ADD,                        // Signal LRT that a job is available in shared queue
    JOB_LAST_ID,                    // Signal LRT what is the last job ID
    JOB_CLEAR_QUEUE,                // Signal LRT to clear its job queue (if LRT_REPEAT_ITERATION_EN, signal is ignored)
    JOB_SENT_PARAM,                 // Signal that LRT sent a ParameterMessage
    JOB_BROADCAST_JOBSTAMP,         // Signal LRT to broadcast its job stamp to everybody
    JOB_DELAY_BROADCAST_JOBSTAMP,   // Signal LRT to broadcast its job stamp to everybody after last job has been done
} JobNotificationType;

/**
 * @brief Generic notification message class
 */
class NotificationMessage {
public:
    explicit NotificationMessage(std::uint16_t type = NotificationType::UNDEFINED_NOTIFICATION,
                                 std::uint16_t subType = NotificationType::UNDEFINED_NOTIFICATION,
                                 std::int32_t index = -1) {
        type_ = type;
        subType_ = subType;
        index_ = index;
    }

    inline std::uint16_t getType() {
        return type_;
    }

    inline std::uint16_t getSubType() {
        return subType_;
    }

    inline std::int32_t getIndex() {
        return index_;
    }

private:
    std::uint16_t type_;
    std::uint16_t subType_;
    std::int32_t index_;
};

/**
 * @brief JOB synchronization notification message class
 */
class JobNotificationMessage {
public:
    explicit JobNotificationMessage(std::int32_t lrtID = -1, std::int32_t jobStamp = -1) {
        id_ = lrtID;
        jobStamp_ = jobStamp;
    }

    inline std::int32_t getID() {
        return id_;
    }

    inline std::int32_t getJobStamp() {
        return jobStamp_;
    }

private:
    std::int32_t id_;
    std::int32_t jobStamp_;
};

/**
 * @brief Information message about JOB to run
 */
class JobInfoMessage {
public:
    JobInfoMessage() = default;

    bool specialActor_ = false;
    std::int32_t srdagID_ = 0;
    std::int32_t fctID_ = 0;
    std::int32_t nEdgeIN_ = 0;
    std::int32_t nEdgeOUT_ = 0;
    std::int32_t nParamIN_ = 0;
    std::int32_t nParamOUT_ = 0;
    Fifo *inFifos_ = nullptr;
    Fifo *outFifos_ = nullptr;
    Param *inParams_ = nullptr;

    ~JobInfoMessage() {
        StackMonitor::free(ARCHI_STACK, inFifos_);
        StackMonitor::free(ARCHI_STACK, outFifos_);
        StackMonitor::free(ARCHI_STACK, inParams_);
    }
};

/**
 * @brief message containing generated parameters
 */
class ParameterMessage {
public:
    explicit ParameterMessage(std::int32_t vertexID, std::int32_t nParam, Param *params = nullptr) {
        vertexID_ = vertexID;
        nParam_ = nParam;
        if (!params) {
            params_ = CREATE_MUL(ARCHI_STACK, nParam, Param);
        } else {
            params_ = params;
        }
    }

    ~ParameterMessage() {
        StackMonitor::free(ARCHI_STACK, params_);
    }

    inline std::int32_t getVertexID() {
        return vertexID_;
    }

    inline std::int32_t getNParam() {
        return nParam_;
    }

    inline Param *getParams() {
        return params_;
    }

private:
    std::int32_t vertexID_;
    std::int32_t nParam_;
    Param *params_;
};

/**
 * @brief message containing TRACE information
 */
class TraceMessage {
public:

    explicit TraceMessage(std::int32_t vertexID = -1, std::int32_t spiderTask = -1, std::int32_t lrtID = -1,
                          Time start = 0, Time end = 0) {
        vertexID_ = vertexID;
        spiderTask_ = spiderTask;
        lrtID_ = lrtID;
        startTime_ = start;
        endTime_ = end;
    }

    inline std::int32_t getLRTID() {
        return lrtID_;
    }

    inline Time getStartTime() {
        return startTime_;
    }

    inline Time getEndTime() {
        return endTime_;
    }

    inline Time getEllapsedTime() {
        return endTime_ - startTime_;
    }

    inline std::int32_t getVertexID() {
        return vertexID_;
    }

    inline std::int32_t getSpiderTask() {
        return spiderTask_;
    }

private:
    std::int32_t vertexID_;
    std::int32_t spiderTask_;
    std::int32_t lrtID_;
    Time startTime_;
    Time endTime_;
};

#endif/*MESSAGE_H*/
