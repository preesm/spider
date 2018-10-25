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

#include <string.h>

#ifndef _WIN32

#endif

#ifdef __k1__
#define CHIP_FREQ ((float)(__bsp_frequency)/1000)
#else
#define CHIP_FREQ (1)
#endif

#include "specialActors/specialActors.h"

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
    fcts_ = 0;
    nFct_ = 0;
    ix_ = ix;
    run_ = false;
    jobIx_ = -1;
    usePapify_ = false;

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
    /* Nothing to Unalloc */
#ifdef VERBOSE
    printf("LRT %3d did %d jobs\n",ix_,jobIxTotal_);
#endif

    //printf("LRT %3d did %d jobs\n",ix_,jobIxTotal_);

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

void LRT::setFctTbl(const lrtFct fct[], int nFct) {
    fcts_ = fct;
    nFct_ = nFct;
}

void LRT::sendTrace(int srdagIx, Time start, Time end) {
    TraceMsg *msgTrace = (TraceMsg *) Platform::get()->getLrtCommunicator()->trace_start_send(sizeof(TraceMsg));

    msgTrace->msgIx = TRACE_JOB;
    msgTrace->srdagIx = srdagIx;
    msgTrace->spiderTask = (unsigned long) -1;
    msgTrace->start = start;
    msgTrace->end = end;
    msgTrace->lrtIx = ix_;

    Platform::get()->getLrtCommunicator()->trace_end_send(sizeof(TraceMsgType));
}

inline void LRT::runReceivedJob(void *msg) {

#ifdef VERBOSE_TIME
    time_waiting_job += Platform::get()->getTime() - start_waiting_job;
#endif

    Platform::get()->getLrtCommunicator()->unlockLrt(getJobIx());

    switch (((UndefinedMsg *) msg)->msgIx) {
        case MSG_START_JOB: {

            StartJobMsg *jobMsg = (StartJobMsg *) msg;
            Fifo *inFifos = (Fifo *) ((char *) jobMsg + 1 * sizeof(StartJobMsg));
            Fifo *outFifos = (Fifo *) ((char *) inFifos + jobMsg->nbInEdge * sizeof(Fifo));
            Param *inParams = (Param *) ((char *) outFifos + jobMsg->nbOutEdge * sizeof(Fifo));

            void **inFifosAlloc = CREATE_MUL(LRT_STACK, jobMsg->nbInEdge, void*);
            void **outFifosAlloc = CREATE_MUL(LRT_STACK, jobMsg->nbOutEdge, void*);
            Param *outParams = CREATE_MUL(LRT_STACK, jobMsg->nbOutParam, Param);

            Time start;

#ifdef VERBOSE_TIME
            start = Platform::get()->getTime();
#endif

            for (int i = 0; i < (int) jobMsg->nbInEdge; i++) {
                tabBlkLrtIx[i] = inFifos[i].blkLrtIx; // lrt to wait
                tabBlkLrtJobIx[i] = inFifos[i].blkLrtJobIx; // total job ticket for this lrt to wait
            }

            Platform::get()->getLrtCommunicator()->waitForLrtUnlock((int) jobMsg->nbInEdge, tabBlkLrtIx,
                                                                    tabBlkLrtJobIx, getJobIx());

#ifdef VERBOSE_TIME
            time_waiting_prev_actor += Platform::get()->getTime() - start;
#endif


#ifdef VERBOSE_TIME
            start = Platform::get()->getTime();
#endif

            Platform::get()->getLrtCommunicator()->allocateDataBuffer(jobMsg->nbInEdge, inFifos, jobMsg->nbOutEdge,
                                                                      outFifos);

#ifdef VERBOSE_TIME
            time_alloc_data += Platform::get()->getTime() - start;
#endif


            for (int i = 0; i < (int) jobMsg->nbInEdge; i++) {
#ifdef VERBOSE_TIME
                Time start = Platform::get()->getTime();
#endif

                inFifosAlloc[i] = Platform::get()->getLrtCommunicator()->data_recv(&inFifos[i]); // in com

                if (inFifos[i].size == 0)
                    inFifosAlloc[i] = NULL;

#ifdef VERBOSE_TIME
                time_waiting_input_comm += Platform::get()->getTime() - start;
#endif
            }


            for (int i = 0; i < (int) jobMsg->nbOutEdge; i++) {
#ifdef VERBOSE_TIME
                Time start = Platform::get()->getTime();
#endif

                outFifosAlloc[i] = Platform::get()->getLrtCommunicator()->data_start_send(&outFifos[i]); // in com

                if (outFifos[i].size == 0)
                    outFifosAlloc[i] = NULL;

#ifdef VERBOSE_TIME
                time_waiting_input_comm += Platform::get()->getTime() - start;
#endif
            }


            start = Platform::get()->getTime();

            if (jobMsg->specialActor && jobMsg->fctIx < 6) {
                specialActors[jobMsg->fctIx](inFifosAlloc, outFifosAlloc, inParams, outParams); // compute
            } else if ((int) jobMsg->fctIx < nFct_) {
                if (usePapify_) {
#ifdef PAPI_AVAILABLE
                    // TODO, find better way to do that
                    try {
                        // We can monitor the events
                        PapifyAction *papifyAction = nullptr;
                        papifyAction = jobPapifyActions_.at(fcts_[jobMsg->fctIx]);
                        // Start monitoring
                        papifyAction->startMonitor();
                        // Do the monitored job
                        fcts_[jobMsg->fctIx](inFifosAlloc, outFifosAlloc, inParams, outParams);
                        // Stop monitoring
                        papifyAction->stopMonitor();
                        // Writes the monitoring results
                        papifyAction->writeEvents();
                    } catch (std::out_of_range &e) {
                        // This job does not have papify events associated with  it
                        fcts_[jobMsg->fctIx](inFifosAlloc, outFifosAlloc, inParams, outParams);
                    }
#endif
                } else {
                    // We don't use papify
                    fcts_[jobMsg->fctIx](inFifosAlloc, outFifosAlloc, inParams, outParams);
                }
            } else {
                printf("Cannot find actor function\n");
                while (1);
            }

            Time end = Platform::get()->getTime();

#ifdef VERBOSE_TIME
            time_compute += end - start;
#endif

            if (jobMsg->traceEnabled)
                sendTrace(jobMsg->srdagIx, start, end);


            for (int i = 0; i < (int) jobMsg->nbOutEdge; i++) {
#ifdef VERBOSE_TIME
                Time start = Platform::get()->getTime();
#endif

                Platform::get()->getLrtCommunicator()->data_end_send(&outFifos[i]); // out com

#ifdef VERBOSE_TIME
                time_waiting_output_comm += Platform::get()->getTime() - start;
#endif
            }


            Platform::get()->getLrtCommunicator()->freeDataBuffer(jobMsg->nbInEdge, jobMsg->nbOutEdge);


            if (jobMsg->nbOutParam != 0) {
                int size = sizeof(ParamValueMsg) + jobMsg->nbOutParam * sizeof(Param);

#ifdef VERBOSE_TIME
                Time start = Platform::get()->getTime();
#endif

                ParamValueMsg *msgParam = (ParamValueMsg *) Platform::get()->getLrtCommunicator()->ctrl_start_send(
                        size); // out com

#ifdef VERBOSE_TIME
                time_waiting_output_comm += Platform::get()->getTime() - start;
#endif

                Param *params = (Param *) (msgParam + 1);

                msgParam->msgIx = MSG_PARAM_VALUE;
                msgParam->srdagIx = jobMsg->srdagIx;
                memcpy(params, outParams, jobMsg->nbOutParam * sizeof(Param));

#ifdef VERBOSE_TIME
                start = Platform::get()->getTime();
#endif

                Platform::get()->getLrtCommunicator()->ctrl_end_send(size); // out com

#ifdef VERBOSE_TIME
                time_waiting_output_comm += Platform::get()->getTime() - start;
#endif
            }

            jobIx_++;
//            printf("LRT%d finished SR%d\n", ix_, jobMsg->srdagIx);
            Platform::get()->getLrtCommunicator()->setLrtJobIx(getIx(), jobIx_);

            StackMonitor::free(LRT_STACK, inFifosAlloc);
            StackMonitor::free(LRT_STACK, outFifosAlloc);
            StackMonitor::free(LRT_STACK, outParams);
            StackMonitor::freeAll(LRT_STACK);

            break;
        }
        case MSG_CLEAR_TIME: {
            ClearTimeMsg *timeMsg = (ClearTimeMsg *) msg;
            Platform::get()->rstTime(timeMsg);
            break;
        }
        case MSG_END_ITER: {
#ifdef VERBOSE_TIME
            nb_iter++;
#endif

            EndIterMsg *msg = (EndIterMsg *) Platform::get()->getLrtCommunicator()->ctrl_start_send(
                    sizeof(EndIterMsg));
            msg->msgIx = MSG_END_ITER;
            Platform::get()->getLrtCommunicator()->ctrl_end_send(sizeof(EndIterMsg));
            break;
        }
        case MSG_RESET_LRT: {

            rstJobIx();

            Platform::get()->getLrtCommunicator()->rstLrtJobIx(getIx());
            ResetLrtMsg *msg = (ResetLrtMsg *) Platform::get()->getLrtCommunicator()->ctrl_start_send(
                    sizeof(ResetLrtMsg));
            msg->msgIx = MSG_RESET_LRT;
            Platform::get()->getLrtCommunicator()->ctrl_end_send(sizeof(ResetLrtMsg));
            break;
        }
        case MSG_STOP_LRT:
            run_ = false;
            break;
        case MSG_PARAM_VALUE:
        default:
            printf("Unexpected message received\n");
            while (1);
    }
    Platform::get()->getLrtCommunicator()->ctrl_end_recv();

#ifdef VERBOSE_TIME
    start_waiting_job = Platform::get()->getTime();
#endif
}

void LRT::runUntilNoMoreJobs() {
    void *msg;

    while (Platform::get()->getLrtCommunicator()->ctrl_start_recv(&msg)) {
        runReceivedJob(msg);
    }
}

void LRT::runInfinitly() {
    void *msg;
    run_ = true;

#ifdef VERBOSE_TIME
    Time start = Platform::get()->getTime();
#endif

#ifdef VERBOSE_TIME
    start_waiting_job = Platform::get()->getTime();
#endif

    while (run_) {
        Platform::get()->getLrtCommunicator()->ctrl_start_recv_block(&msg);
        runReceivedJob(msg);
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
