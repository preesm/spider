/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018 - 2019)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018 - 2019)
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
#include <Logger.h>
#include <specialActors/specialActors.h>
#include <graphs/Archi/Archi.h>

#ifdef __USE_GNU

#include <csignal>

#endif


#ifdef __k1__
#define CHIP_FREQ ((float)(__bsp_frequency)/1000)
#else
#define CHIP_FREQ (1)
#endif

static lrtFct specialActors[6] = {
        &saBroadcast,
        &saFork,
        &saJoin,
        &saRoundbuffer,
        &saInit,
        &saEnd
};

extern int preesmStopThreads;

LRT::LRT(int ix) {
    /* TODO add some heapMemory */
    fcts_ = nullptr;
    nFct_ = 0;
    ix_ = ix;
    jobIx_ = -1;
    jobIxTotal_ = 0;
    run_ = false;
    usePapify_ = false;
    dumpPapifyInfo_ = false;
    feedbackPapifyInfo_ = false;

    traceEnabled_ = false;
    repeatJobQueue_ = false;
    freeze_ = false;
    shouldBroadcast_ = false;
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

void LRT::sendPapifyTrace(int srdagIx, PapifyAction *papifyAction) {
    // Push message
    auto *papifyMessage = CREATE(ARCHI_STACK, PapifyMessage)(srdagIx, -1, getIx(), papifyAction->getTimeStart(), papifyAction->getTimeStop());
    auto index = spiderCommunicator_->push_papify_message(&papifyMessage);

    // Push notification
    auto notificationMessage = NotificationMessage(PAPIFY_NOTIFICATION, PAPIFY_TIMING, getIx(), index);
    spiderCommunicator_->push_notification(Platform::get()->getNLrt(), &notificationMessage);
}

void LRT::sendTrace(int srdagIx, Time start, Time end) {
    // Push message
    auto *traceMessage = CREATE(ARCHI_STACK, TraceMessage)(srdagIx, -1, getIx(), start, end);
    auto index = spiderCommunicator_->push_trace_message(&traceMessage);

    // Push notification
    auto notificationMessage = NotificationMessage(TRACE_NOTIFICATION, TRACE_LRT, getIx(), index);
    spiderCommunicator_->push_notification(Platform::get()->getNLrt(), &notificationMessage);
}

bool LRT::compareLRTJobStamps(std::int32_t *jobsToWait) {
    if (jobsToWait[getIx()] > getJobIx()) {
        Logger::print(LOG_JOB, LOG_ERROR,
                      "LRT: %d -- waiting for future self job. Current jobStamp: %d -- waitedJob: %d",
                      getIx(),
                      getJobIx(),
                      jobsToWait[getIx()]);
        return false;
    }
    for (int i = 0; i < nLrt_; ++i) {
        if (jobsToWait[i] >= 0 && jobsToWait[i] > jobStamps_[i]) {
            Logger::print(LOG_JOB, LOG_INFO, "LRT: %d -- waiting for LRT: %d and Job: %d -- Current JobStamp: %d\n",
                          getIx(), i,
                          jobsToWait[i], jobStamps_[i]);
            return false;
        }
    }
    return true;
}

void LRT::updateLRTJobStamp(std::int32_t lrtID, std::int32_t jobStamp) {
    if (lrtID < 0 || lrtID >= nLrt_) {
        throwSpiderException("Bad LRT id. Value: %d -- Max: %d.", lrtID, nLrt_ - 1);
    }
    jobStamps_[lrtID] = jobStamp;
    Logger::print(LOG_JOB, LOG_INFO, "LRT: %d -- updating local jobStamp of LRT: %d -- new JobStamp: %d\n",
                  getIx(),
                  lrtID,
                  jobStamp);
}

void LRT::notifyLRTJobStamp(int lrt, bool notify) {
    if (lrt != getIx() && notify) {
        Logger::print(LOG_JOB, LOG_INFO, "LRT: %d -- notifying LRT: %d -- sent jobStamp: %d\n", getIx(), lrt, jobIx_);
        NotificationMessage message(JOB_NOTIFICATION, JOB_UPDATE_JOBSTAMP, getIx(), jobIx_);
        spiderCommunicator_->push_notification(lrt, &message);
    }
}

void LRT::runJob(JobInfoMessage *job) {
    Time start;

#ifdef VERBOSE_TIME
    time_waiting_prev_actor += Platform::get()->getTime() - start;
#endif


#ifdef VERBOSE_TIME
    start = Platform::get()->getTime();
#endif

    // TODO: alloc data (see other platforms)
    auto **inFifosAlloc = CREATE_MUL(LRT_STACK, job->nEdgeIN_, void*);
    auto **outFifosAlloc = CREATE_MUL(LRT_STACK, job->nEdgeOUT_, void*);
    auto *outParams = CREATE_MUL(ARCHI_STACK, job->nParamOUT_, Param);

#ifdef VERBOSE_TIME
    time_alloc_data += Platform::get()->getTime() - start;
#endif

    auto *peJob = Spider::getArchi()->getPEFromSpiderID(ix_);

    for (int i = 0; i < job->nEdgeIN_; i++) {
#ifdef VERBOSE_TIME
        Time start = Platform::get()->getTime();
#endif
        auto fifo = job->inFifos_[i];
//        inFifosAlloc[i] = lrtCommunicator_->data_recv(fifo.alloc); // in com
        inFifosAlloc[i] = peJob->getMemoryUnit()->receiveMemory(fifo.alloc, nullptr, fifo.alloc);
        if (fifo.size == 0) {
            inFifosAlloc[i] = nullptr;
        }

#ifdef VERBOSE_TIME
        time_waiting_input_comm += Platform::get()->getTime() - start;
#endif
    }


    for (int i = 0; i < job->nEdgeOUT_; i++) {
#ifdef VERBOSE_TIME
        Time start = Platform::get()->getTime();
#endif
        auto fifo = job->outFifos_[i];
//        outFifosAlloc[i] = lrtCommunicator_->data_start_send(fifo.alloc); // in com
        outFifosAlloc[i] = peJob->getMemoryUnit()->virtToPhy(fifo.alloc);
        if (fifo.size == 0) {
            outFifosAlloc[i] = nullptr;
        }

#ifdef VERBOSE_TIME
        time_waiting_input_comm += Platform::get()->getTime() - start;
#endif
    }


    start = Platform::get()->getTime();

    // Get inParams
    auto *inParams = job->inParams_;
    auto fctID = job->fctID_;
    if (job->specialActor_ && fctID < 6) {
        specialActors[fctID](inFifosAlloc, outFifosAlloc, inParams, outParams); // compute
    } else if (fctID < nFct_) {
        if (usePapify_) {
#ifdef PAPI_AVAILABLE
            // TODO, find better way to do that
            try {
                // We can monitor the events
                PapifyAction *papifyAction = nullptr;
                papifyAction = jobPapifyActions_.at(fcts_[fctID]);
                // Start monitoring
                papifyAction->startMonitor();
                // Do the monitored job
                fcts_[fctID](inFifosAlloc, outFifosAlloc, inParams, outParams);
                // Stop monitoring
                papifyAction->stopMonitor();
                // Writes the monitoring results
                if (dumpPapifyInfo_) {
                    papifyAction->writeEvents();
                }
                if(feedbackPapifyInfo_){
                    sendPapifyTrace(job->srdagID_, papifyAction);
                }
            } catch (std::out_of_range &e) {
                // This job does not have papify events associated with  it
                fcts_[fctID](inFifosAlloc, outFifosAlloc, inParams, outParams);
            }
#endif
        } else {
            fcts_[fctID](inFifosAlloc, outFifosAlloc, inParams, outParams);
        }
    } else {
        throwSpiderException("Invalid function id: %d -- Range=[0;%d[.\n", fctID, nFct_);
    }

    Time end = Platform::get()->getTime();

#ifdef VERBOSE_TIME
    time_compute += end - start;
#endif

    if (traceEnabled_) {
        sendTrace(job->srdagID_, start, end);
    }


    // TODO: data_end_send for other platforms
    for (int i = 0; i < job->nEdgeOUT_; i++) {
        auto fifo = job->outFifos_[i];
        peJob->getMemoryUnit()->sendMemory(fifo.alloc, nullptr, fifo.alloc);
    }

    /** Updating jobIx_ and notifying other LRT (if needed) **/
    jobIx_++;
    jobStamps_[getIx()] = jobIx_;
    for (int i = 0; i < nLrt_; ++i) {
#ifdef VERBOSE_TIME
        Time start = Platform::get()->getTime();
#endif
        notifyLRTJobStamp(i, job->lrts2Notify_[i]);
#ifdef VERBOSE_TIME
        time_waiting_output_comm += Platform::get()->getTime() - start;
#endif
    }


    if (job->nParamOUT_) {
        // TODO: change ARCHI_STACK to something not shared
        auto *parameterMessage = CREATE(ARCHI_STACK, ParameterMessage)(job->srdagID_, job->nParamOUT_, outParams);
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
        NotificationMessage parameterNotification(JOB_NOTIFICATION, JOB_SENT_PARAM, getIx(), index);
        spiderCommunicator_->push_notification(Platform::get()->getNLrt(), &parameterNotification);

#ifdef VERBOSE_TIME
        time_waiting_output_comm += Platform::get()->getTime() - start;
#endif
    } else {
        StackMonitor::free(ARCHI_STACK, outParams);
    }



    /** Freeing local memory **/
    StackMonitor::free(LRT_STACK, inFifosAlloc);
    StackMonitor::free(LRT_STACK, outFifosAlloc);
}

void LRT::broadcastJobStamp() {
    shouldBroadcast_ = false;
    if (jobIx_ < 0) {
        return;
    }
    NotificationMessage msg(JOB_NOTIFICATION, JOB_UPDATE_JOBSTAMP, getIx(), jobIx_);
    for (int i = 0; i < nLrt_; ++i) {
        if (i == getIx()) {
            continue;
        }
        spiderCommunicator_->push_notification(i, &msg);
    }
}

void LRT::clearJobQueue() {
    for (auto &job: jobQueue_) {
        job->~JobInfoMessage();
        StackMonitor::free(ARCHI_STACK, job);
    }
    jobQueue_.clear();
    jobQueueSize_ = 0;
    jobQueueIndex_ = 0;
    lastJobID_ = -1;
    jobIx_ = -1;
    Logger::print(LOG_JOB, LOG_INFO, "LRT: %d -- cleared jobQueue_.\n", Platform::get()->getLrtIx());
}

void LRT::handleLRTNotification(NotificationMessage &message) {
    switch (message.getSubType()) {
        case LRT_END_ITERATION:
            lastJobID_ = message.getIndex();
            Logger::print(LOG_JOB, LOG_INFO, "LRT: %d -- lastJobID: %d\n", getIx(), lastJobID_);
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
            break;
        default:
            throwSpiderException("Unhandled type of LRT notification: %u\n", message.getSubType());
    }

}

void LRT::handleJobNotification(NotificationMessage &message) {
    /** Get the ID of the job in the global queue */
    switch (message.getSubType()) {
        case JOB_ADD: {
            JobInfoMessage *job;
            // pop message from global queue
            lrtCommunicator_->pop_job_message(&job, message.getIndex());
            // push message in local queue (don't execute right now in case more important notification comes along
            jobQueue_.push_back(job);
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
        case JOB_DELAY_BROADCAST_JOBSTAMP:
            shouldBroadcast_ = true;
            break;
        case JOB_BROADCAST_JOBSTAMP:
            broadcastJobStamp();
            break;
        case JOB_UPDATE_JOBSTAMP:
            updateLRTJobStamp(message.getLRTID(), message.getIndex());
            break;
        default:
            throwSpiderException("Unhandled type of JOB notification: %u\n", message.getSubType());
    }
}

void LRT::handleTraceNotification(NotificationMessage &message) {
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

bool LRT::checkNotifications(bool shouldWait) {
#ifdef __USE_GNU
    sigset_t waiting_mask;
    sigpending(&waiting_mask);
    if (sigismember(&waiting_mask, SIGINT)) {
        preesmStopThreads = 1;
    }
#endif
    if (shouldWait) {
        Logger::print(LOG_JOB, LOG_INFO, "LRT: %d -- waiting for notification.\n", getIx());
    }
    NotificationMessage notificationMessage;
    if (lrtCommunicator_->pop_notification(&notificationMessage, shouldWait)) {
        switch (notificationMessage.getType()) {
            case LRT_NOTIFICATION:
                handleLRTNotification(notificationMessage);
                break;
            case TRACE_NOTIFICATION:
                handleTraceNotification(notificationMessage);
                break;
            case JOB_NOTIFICATION:
                handleJobNotification(notificationMessage);
                break;
            default:
                throwSpiderException("Unhandled type of notification: %d.", notificationMessage.getType());
        }
        return true;
    }
    return false;
}

static inline void jobQueueSanityCheck(std::int32_t lastJobID, std::uint32_t jobQueueSize, int lrtId) {
    if (lastJobID >= 0 && jobQueueSize > (std::uint32_t) (lastJobID + 1)) {
        throwSpiderException(
                "QueuSize mismatch with number of jobs to do. LRT: %d -- queueSize: %d -- lastJobID: %d", lrtId,
                jobQueueSize, lastJobID);
    }
}

void LRT::run(bool loop) {
#ifdef VERBOSE_TIME
    Time start = Platform::get()->getTime();
#endif

#ifdef VERBOSE_TIME
    start_waiting_job = Platform::get()->getTime();
#endif
    bool doneWithCurrentJobs = false;
    bool canRunJob = true;
    while (run_) {
        /** Should wait for notifications if no more jobs are available and that we are in loop mode **/
        bool blocking = (loop && doneWithCurrentJobs) || !canRunJob;
        /** 0. Check for the presence of notifications **/
        while (checkNotifications(blocking)) {
            blocking = false;
        }
        /** Check if we have to exit **/
        if (!run_) {
            break;
        }
        /** Sanity check **/
        jobQueueSanityCheck(lastJobID_, jobQueueSize_, getIx());
        /** 1. If JOB queue is not empty **/
        bool gotJobToDo = jobQueueIndex_ < jobQueueSize_;
        if (gotJobToDo) {
            Logger::print(LOG_JOB, LOG_INFO, "LRT: %d -- Got %d Jobs to do -- Got %d Jobs in queue -- Done %d.\n",
                          Platform::get()->getLrtIx(), lastJobID_ + 1,
                          jobQueueSize_, jobQueueIndex_);
            /** Pop the job **/
            auto *job = jobQueue_[jobQueueIndex_];
            /** Can Run the job ? **/
            canRunJob = compareLRTJobStamps(job->jobs2Wait_);
            if (canRunJob) {
                jobQueueIndex_++;
                Logger::print(LOG_JOB, LOG_INFO, "LRT: %d -- Running Job: %d\n", Platform::get()->getLrtIx(),
                              jobIx_ + 1);
                /** Run the job **/
                runJob(job);
                Logger::print(LOG_JOB, LOG_INFO, "LRT: %d -- Finished Job: %d\n", Platform::get()->getLrtIx(), jobIx_);
            }
        }
        /** 2. Check if end of iteration of end of current jobs **/
        bool doneWithIteration = lastJobID_ >= 0 && jobIx_ == lastJobID_;
        if (doneWithIteration) {
            if (loop) {
                /** Send finished iteration message **/
                NotificationMessage finishedMessage(LRT_NOTIFICATION, LRT_FINISHED_ITERATION, getIx());
                spiderCommunicator_->push_notification(Platform::get()->getNLrt(), &finishedMessage);
            }
            /** Reset local jobStamps **/
            jobStamps_.assign(jobStamps_.size(), -1);
            Logger::print(LOG_JOB, LOG_INFO, "LRT: %d -- finished iteration.\n", getIx());
            if (repeatJobQueue_) {
                jobIx_ = -1;
                jobQueueIndex_ = 0;
            } else {
                clearJobQueue();
            }
        }
        doneWithCurrentJobs = (jobQueueSize_ == jobQueueIndex_);
        if (doneWithCurrentJobs) {
            if (shouldBroadcast_) {
                broadcastJobStamp();
            }
            if (!loop) {
                Logger::print(LOG_JOB, LOG_INFO, "LRT: %d -- exiting iteration.\n", getIx());
                break;
            }
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
