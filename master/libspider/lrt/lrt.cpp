/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2018)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013)
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
#include <lrt.h>
#include <LrtCommunicator.h>

#include <cstring>
#include <PThreadLrtCommunicator.h>
#include <PThreadSpiderCommunicator.h>

#ifndef _WIN32

#endif

#ifdef __k1__
#define CHIP_FREQ ((float)(__bsp_frequency)/1000)
#else
#define CHIP_FREQ (1)
#endif

#include "specialActors/specialActors.h"
#include "lrt.h"

//#define VERBOSE_JOBS

static lrtFct specialActors[6] = {
        &saBroadcast,
        &saFork,
        &saJoin,
        &saRoundbuffer,
        &saInit,
        &saEnd
};

LRT::LRT(int ix) {
    /* TODO add some heapMemory */
    fcts_ = nullptr;
    nFct_ = 0;
    ix_ = ix;
    jobIx_ = -1;
    jobIxTotal_ = 0;
    run_ = false;
    usePapify_ = false;

    traceEnabled_ = false;
    repeatJobQueue_ = false;
    freeze_ = false;
    lastJobID_ = -1;
    jobQueueIndex_ = 0;
    jobQueueSize_ = 0;
    nLrt_ = Platform::get()->getNLrt();
    for (int i = 0; i < nLrt_; ++i) {
        jobStamps_.push_back(-1);
    }

#ifdef VERBOSE_TIME
    time_waiting_job = 0;
    time_waiting_prev_actor = 0;
    time_waiting_input_comm = 0;
    time_compute = 0;
    time_waiting_output_comm = 0;
    time_global = 0;

    time_alloc_data = 0;

    time_other = 0;

    nb_iter = 0;
#endif
}

LRT::~LRT() {
    /** Clearing the jobQueue_ **/
    clearJobQueue();

    /** Reseting pointers **/
    // TODO: move free and destruction of stuff from platform to here ?
    spiderCommunicator_ = nullptr;
    lrtCommunicator_ = nullptr;

    /** Clearing the jobStamps **/
    jobStamps_.clear();

#ifdef VERBOSE_TIME
    time_other = time_global - time_waiting_job - time_waiting_prev_actor - time_alloc_data - time_waiting_input_comm - time_compute - time_waiting_output_comm;

    printf("LRT %3d, time_wait_job %.2fms (%.1f%%), time_wait_prev_actor %.2fms (%.1f%%), time_alloc_data %.2fms (%.1f%%), time_wait_in_com %.2fms (%.1f%%), time_compute %.2fms (%.1f%%), time_wait_out_com %.2fms (%.1f%%), time_other %.2fms (%.1f%%), time_global %.2fms\n",
    ix_,
    (float)time_waiting_job / CHIP_FREQ / nb_iter,
    (float)time_waiting_job*100/time_global,
    (float)time_waiting_prev_actor / CHIP_FREQ / nb_iter,
    (float)time_waiting_prev_actor*100/time_global,
    (float)time_alloc_data / CHIP_FREQ / nb_iter,
    (float)time_alloc_data*100/time_global,
    (float)time_waiting_input_comm / CHIP_FREQ / nb_iter,
    (float)time_waiting_input_comm*100/time_global,
    (float)time_compute / CHIP_FREQ / nb_iter,
    (float)time_compute*100/time_global,
    (float)time_waiting_output_comm / CHIP_FREQ / nb_iter,
    (float)time_waiting_output_comm*100/time_global,
    (float)time_other / CHIP_FREQ / nb_iter,
    (float)time_other*100/time_global,
    (float)time_global / CHIP_FREQ / nb_iter);
#endif
#ifdef PAPI_AVAILABLE
    if (usePapify_) {
        std::map<lrtFct, PapifyAction *>::iterator it;
        // Delete all actor monitors
        for (it = jobPapifyActions_.begin(); it != jobPapifyActions_.end(); ++it) {
            delete it->second;
        }
    }
#endif
}

void LRT::sendTrace(int srdagIx, Time start, Time end) {
//    auto msgTrace = (TraceMessage *) lrtCommunicator_->trace_start_send(sizeof(TraceMessage));
//
//    msgTrace->vertexID_ = srdagIx;
//    msgTrace->spiderTask_ = (std::uint32_t) -1;
//    msgTrace->startTime_ = start;
//    msgTrace->endTime_ = end;
//    msgTrace->lrtID_ = ix_;

//    lrtCommunicator_->trace_end_send(sizeof(TraceMsgType));

    // Push message
    auto *traceMessage = CREATE(ARCHI_STACK, TraceMessage)(srdagIx, -1, getIx(), start, end);
    auto *spiderCommunicator = Platform::get()->getSpiderCommunicator();
    auto index = spiderCommunicator->push_trace_message(&traceMessage);

    // Push notification
    auto notificationMessage = NotificationMessage(TRACE_NOTIFICATION, TRACE_LRT, index);
    spiderCommunicator->push_notification(Platform::get()->getNLrt(), &notificationMessage);
}

bool LRT::checkLRTJobStamps(std::vector<std::int32_t> &jobsToWait) {
    for (int i = 0; i < nLrt_; ++i) {
        auto jobToWait = jobsToWait[i];
        if (i == getIx() && jobToWait > getJobIx()) {
            throwSpiderException("waiting for future self job. Current jobStamp: %d -- waitedJob: %d", getJobIx(),
                                 jobToWait);
        } else if ((jobToWait >= 0) && jobToWait > jobStamps_[i]) {
#ifdef VERBOSE_JOBS
            fprintf(stderr, "INFO: LRT: %d -- waiting for LRT: %d and Job: %d -- Current JobStamp: %d\n", getIx(), i,
                    jobToWait, jobStamps_[i]);
#endif
            return false;
        }
    }
    return true;
}

void LRT::fetchLRTNotification(NotificationMessage &message) {
    switch (message.getSubType()) {
        case LRT_END_ITERATION:
            if ((lastJobID_ < 0) || (jobQueueSize_ - 1) > (std::uint32_t) lastJobID_) {
                lastJobID_ = jobQueueSize_ - 1;
#ifdef VERBOSE_JOBS
                fprintf(stderr, "INFO: LRT: %d -- lastJobID: %d\n", getIx(), lastJobID_);
#endif
            }
            break;
        case LRT_RST_ITERATION:
            jobIx_ = -1;
            jobQueueIndex_ = 0;
            break;
        case LRT_REPEAT_ITERATION_EN:
            repeatJobQueue_ = true;
            break;
        case LRT_REPEAT_ITERATION_DIS:
            repeatJobQueue_ = false;
            break;
        case LRT_PAUSE:
            freeze_ = true;
            break;
        case LRT_RESUME:
            freeze_ = false;
            break;
        case LRT_STOP:
            run_ = false;
            clearJobQueue();
            break;
        default:
            throwSpiderException("Unhandled type of LRT notification: %u\n", message.getSubType());
    }

}

void LRT::fetchJobNotification(NotificationMessage &message) {
    /** Get the ID of the job in the global queue */
    switch (message.getSubType()) {
        case JOB_ADD: {
            JobInfoMessage *msg;
            // pop message from global queue
            lrtCommunicator_->pop_job_message(&msg, message.getIndex());
            // push message in local queue (don't execute right now in case more important notification comes along
            jobQueue_.push_back(msg);
            jobQueueSize_++;
        }
            break;
        case JOB_CLEAR_QUEUE:
            if (!repeatJobQueue_) {
                clearJobQueue();
            }
            break;
        case JOB_LAST_ID:
            lastJobID_ = message.getIndex();
            break;
        default:
            throwSpiderException("Unhandled type of JOB notification: %u\n", message.getSubType());
    }
}

void LRT::fetchTraceNotification(NotificationMessage &message) {
    switch (message.getSubType()) {
        case TRACE_ENABLE:
            traceEnabled_ = true;
            break;
        case TRACE_DISABLE:
            traceEnabled_ = false;
            break;
        case TRACE_RST:
            break;
        default:
            throwSpiderException("Unhandled type of TRACE notification: %u\n", message.getSubType());
    }
}

void LRT::clearJobQueue() {
    for (auto &it : (jobQueue_)) {
        it->~JobInfoMessage();
        StackMonitor::free(ARCHI_STACK, it);
    }
    jobQueue_.clear();
    jobQueueSize_ = 0;
    jobQueueIndex_ = 0;
    jobIx_ = -1;
    lastJobID_ = -1;
#ifdef VERBOSE_JOBS
    fprintf(stderr, "INFO: LRT: %d -- cleared jobQueue_.\n", getIx());
#endif
}

void LRT::runJob(JobInfoMessage *message) {
    auto inFifos = message->inFifos_;
    auto outFifos = message->outFifos_;
    auto inParams = message->inParams_;

    auto **inFifosAlloc = CREATE_MUL(LRT_STACK, message->nEdgeIN_, void*);
    auto **outFifosAlloc = CREATE_MUL(LRT_STACK, message->nEdgeOUT_, void*);
    auto outParams = CREATE_MUL(ARCHI_STACK, message->nParamOUT_, Param);

    Time start;
    std::vector<std::int32_t> jobsToWait(nLrt_, -1);

    for (int i = 0; i < message->nEdgeIN_; i++) {
        jobsToWait[inFifos[i].blkLrtIx] = std::max(jobsToWait[inFifos[i].blkLrtIx], inFifos[i].blkLrtJobIx);
    }

    // Waiting for JobStamps to be updated
    while (!checkLRTJobStamps(jobsToWait)) {
        JobNotificationMessage msg;
        if (((PThreadSpiderCommunicator *) spiderCommunicator_)->pop_data_notification(getIx(), &msg)) {
            updateLRTJobStamp(msg.getID(), msg.getJobStamp());
        }
    }


#ifdef VERBOSE_TIME
    time_waiting_prev_actor += Platform::get()->getTime() - start;
#endif


#ifdef VERBOSE_TIME
    start = Platform::get()->getTime();
#endif

    lrtCommunicator_->allocateDataBuffer(message->nEdgeIN_, inFifos, message->nEdgeOUT_,
                                         outFifos);

#ifdef VERBOSE_TIME
    time_alloc_data += Platform::get()->getTime() - start;
#endif


    for (int i = 0; i < message->nEdgeIN_; i++) {
#ifdef VERBOSE_TIME
        Time start = Platform::get()->getTime();
#endif

        inFifosAlloc[i] = lrtCommunicator_->data_recv(&inFifos[i]); // in com

        if (inFifos[i].size == 0) {
            inFifosAlloc[i] = nullptr;
        }

#ifdef VERBOSE_TIME
        time_waiting_input_comm += Platform::get()->getTime() - start;
#endif
    }


    for (int i = 0; i < message->nEdgeOUT_; i++) {
#ifdef VERBOSE_TIME
        Time start = Platform::get()->getTime();
#endif

        outFifosAlloc[i] = lrtCommunicator_->data_start_send(&outFifos[i]); // in com

        if (outFifos[i].size == 0) {
            outFifosAlloc[i] = nullptr;
        }

#ifdef VERBOSE_TIME
        time_waiting_input_comm += Platform::get()->getTime() - start;
#endif
    }


    start = Platform::get()->getTime();

    if (message->specialActor_ && message->fctID_ < 6) {
        specialActors[message->fctID_](inFifosAlloc, outFifosAlloc, inParams, outParams); // compute
    } else if ((int) message->fctID_ < nFct_) {
        if (usePapify_) {
#ifdef PAPI_AVAILABLE
            // TODO, find better way to do that
            try {
                // We can monitor the events
                PapifyAction *papifyAction = nullptr;
                papifyAction = jobPapifyActions_.at(fcts_[message->fctID_]);
                // Start monitoring
                papifyAction->startMonitor();
                // Do the monitored job
                fcts_[message->fctID_](inFifosAlloc, outFifosAlloc, inParams, outParams);
                // Stop monitoring
                papifyAction->stopMonitor();
                // Writes the monitoring results
                papifyAction->writeEvents();
            } catch (std::out_of_range &e) {
                // This job does not have papify events associated with  it
                fcts_[message->fctID_](inFifosAlloc, outFifosAlloc, inParams, outParams);
            }
#endif
        } else {
            // We don't use papify
            fcts_[message->fctID_](inFifosAlloc, outFifosAlloc, inParams, outParams);
        }
    } else {
        throwSpiderException("Invalid function id: %d -- Range=[0;%d[.\n", message->fctID_, nFct_);
    }

    Time end = Platform::get()->getTime();

#ifdef VERBOSE_TIME
    time_compute += end - start;
#endif

    if (traceEnabled_) {
        sendTrace(message->srdagID_, start, end);
    }


    JobNotificationMessage msg(getIx(), jobIx_ + 1);

    std::vector<bool> notifiedLRT((size_t) nLrt_, false);

    for (int i = 0; i < message->nEdgeOUT_; i++) {
#ifdef VERBOSE_TIME
        Time start = Platform::get()->getTime();
#endif
        lrtCommunicator_->data_end_send(&outFifos[i]); // out com

        notifyLRTJobStamp(message->outFifos_[i].blkLrtIx, &msg, notifiedLRT);

#ifdef VERBOSE_TIME
        time_waiting_output_comm += Platform::get()->getTime() - start;
#endif
    }


    if (message->nParamOUT_) {
        // TODO: change ARCHI_STACK to something not shared
        auto *parameterMessage = CREATE(ARCHI_STACK, ParameterMessage)(message->srdagID_, message->nParamOUT_,
                                                                       outParams);
#ifdef VERBOSE_TIME
        Time start = Platform::get()->getTime();
#endif
        /** Sending the parameter message */
        auto index = spiderCommunicator_->push_parameter_message(&parameterMessage);
        parameterMessage = nullptr;

#ifdef VERBOSE_TIME
        time_waiting_output_comm += Platform::get()->getTime() - start;
#endif

#ifdef VERBOSE_TIME
        start = Platform::get()->getTime();
#endif
        /** Sending notification **/
        NotificationMessage parameterNotification(JOB_NOTIFICATION, JOB_SENT_PARAM, index);
        spiderCommunicator_->push_notification(Platform::get()->getNLrt(), &parameterNotification);

#ifdef VERBOSE_TIME
        time_waiting_output_comm += Platform::get()->getTime() - start;
#endif
    } else {
        StackMonitor::free(ARCHI_STACK, outParams);
    }


    jobIx_++;
    jobStamps_[getIx()] = jobIx_;
    /** Freeing local memory **/
    StackMonitor::free(LRT_STACK, inFifosAlloc);
    StackMonitor::free(LRT_STACK, outFifosAlloc);
    StackMonitor::freeAll(LRT_STACK);
}

void LRT::jobRunner() {
    auto message = jobQueue_[jobQueueIndex_++];
    // Run job
#ifdef VERBOSE_JOBS
    fprintf(stderr, "INFO: LRT: %d -- Running Job: %d\n", Platform::get()->getLrtIx(), jobIx_ + 1);
#endif

    runJob(message);

#ifdef VERBOSE_JOBS
    fprintf(stderr, "INFO: LRT: %d -- Finished Job: %d\n", Platform::get()->getLrtIx(), jobIx_);
#endif
    // Check if it is last job of current iteration
    if ((lastJobID_ >= 0) && jobIx_ == lastJobID_) {
        // Send finished iteration message
        NotificationMessage finishedMessage(LRT_NOTIFICATION, LRT_FINISHED_ITERATION, getIx());
        spiderCommunicator_->push_notification(Platform::get()->getNLrt(), &finishedMessage);
        /** Reset local jobStamps **/
        for (int i = 0; i < nLrt_; ++i) {
            jobStamps_[i] = -1;
        }
#ifdef VERBOSE_JOBS
        fprintf(stderr, "INFO: LRT: %d -- finished iteration.\n", getIx());
#endif
        if (repeatJobQueue_) {
            jobIx_ = -1;
            jobQueueIndex_ = 0;
        }
    }
}

void LRT::run(bool loop) {
#ifdef VERBOSE_TIME
    Time start = Platform::get()->getTime();
#endif

#ifdef VERBOSE_TIME
    start_waiting_job = Platform::get()->getTime();
#endif
    run_ = true;
    bool doneWithCurrentJobs = false;
    while (run_) {
        /** Should wait for notifications if no more jobs are available and that we are in loop mode **/
        bool blocking = loop && doneWithCurrentJobs;
        // TODO: change this specific behavior to all communicator
        auto communicator = (PThreadLrtCommunicator *) (Platform::get()->getLrtCommunicator());
        /** 0. Check for the presence of notification **/
        NotificationMessage notificationMessage;
        while (communicator->pop_notification(&notificationMessage, blocking)) {
            blocking = false;
            switch (notificationMessage.getType()) {
                case LRT_NOTIFICATION:
                    fetchLRTNotification(notificationMessage);
                    break;
                case TRACE_NOTIFICATION:
                    fetchTraceNotification(notificationMessage);
                    break;
                case JOB_NOTIFICATION:
                    fetchJobNotification(notificationMessage);
                    break;
                default:
                    throwSpiderException("Unhandled type of notification: %d.", notificationMessage.getType());
            }
        }
        /** 1. If no notification and JOB queue is not empty **/
        if (jobQueueIndex_ < jobQueueSize_) {
#ifdef VERBOSE_JOBS
            fprintf(stderr, "INFO: LRT: %d -- Got %d Jobs to do -- Got %d Jobs in queue -- Done %d.\n",
                    Platform::get()->getLrtIx(), lastJobID_ + 1,
                    jobQueueSize_, jobQueueIndex_);
#endif
            jobRunner();
        }
        doneWithCurrentJobs = (jobQueueSize_ == jobQueueIndex_);
        if (!loop && doneWithCurrentJobs) {
#ifdef VERBOSE_JOBS
            fprintf(stderr, "INFO: LRT: %d -- exiting iteration.\n", getIx());
#endif
            break;
        }
    }

#ifdef VERBOSE_TIME
    time_global += Platform::get()->getTime() - start;
#endif
}


#ifdef PAPI_AVAILABLE

void LRT::addPapifyJobInfo(lrtFct const &fct, PapifyAction *papifyAction) {
    this->jobPapifyActions_.insert(std::make_pair(fct, papifyAction));
}


#endif
