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
#ifndef LRT_H
#define LRT_H

#include <map>
#include <tools/Stack.h>
#include <platform.h>
#include <SpiderCommunicator.h>
#include <LrtCommunicator.h>

#ifdef PAPI_AVAILABLE
#include "../papify/PapifyAction.h"

#endif

#define NB_MAX_ACTOR (200)

using lrtFct = void (*)(void **, void **, Param *, Param *);

class LRT {
public:
    LRT(int ix);

    virtual ~LRT();

    void runUntilNoMoreJobs() {
        run(false);
    };

    void runInfinitly() {
        run(true);
    };

    inline void setFctTbl(const lrtFct fct[], int nFct);

    inline void setJobIx(int jobIx);

    inline void rstJobIx();

    inline int getIx() const;

    inline int getJobIx() const;

    inline void setUsePapify();

    inline void setCommunicators();

#ifdef PAPI_AVAILABLE

    void addPapifyJobInfo(lrtFct const &fct, PapifyAction *papifyAction);

#endif

protected:
    void sendTrace(int srdagIx, Time start, Time end);

private:
    int ix_;
    int nFct_;
    const lrtFct *fcts_;
    bool run_;
    bool usePapify_;
    int jobIx_;
    int jobIxTotal_;

    int tabBlkLrtIx[NB_MAX_ACTOR];
    int tabBlkLrtJobIx[NB_MAX_ACTOR];

#ifdef PAPI_AVAILABLE
    std::map<lrtFct, PapifyAction *> jobPapifyActions_;
#endif
#ifdef VERBOSE_TIME
    Time time_waiting_job;
    Time time_waiting_prev_actor;
    Time time_waiting_input_comm;
    Time time_compute;
    Time time_waiting_output_comm;
    Time time_global;

    Time time_alloc_data;

    Time time_other;

    Time start_waiting_job;

    int nb_iter;
#endif

    int nLrt_;
    bool repeatJobQueue_;
    bool freeze_;
    bool traceEnabled_;
    bool shouldBroadcast_;
    std::vector<JobInfoMessage *> jobQueue_;
    std::uint32_t jobQueueIndex_;
    std::uint32_t jobQueueSize_;
    std::int32_t lastJobID_;
    SpiderCommunicator *spiderCommunicator_;
    LrtCommunicator *lrtCommunicator_;
    std::vector<std::int32_t> jobStamps_;

    bool compareLRTJobStamps(std::vector<std::int32_t> &jobsToWait);

    void updateLRTJobStamp(std::int32_t lrtID, std::int32_t jobStamp);

    void notifyLRTJobStamp(std::int32_t lrtID, JobNotificationMessage *msg, std::vector<bool> &notifiedLRT);

    /**
     * @brief Fetch an LRT notification message
     * @param message message to fetch
     */
    void handleLRTNotification(NotificationMessage &message);

    /**
     * @brief Fetch a JOB notification message
     * @param message message to fetch
     */
    void handleJobNotification(NotificationMessage &message);

    /**
     * @brief Fetch a TRACE notification message
     * @param message message to fetch
     */
    void handleTraceNotification(NotificationMessage &message);

    /**
     * @brief Run a JOB message
     * @param message message of the JOB to run
     */
    void runJob(JobInfoMessage *message);

    /**
     * @brief Clear the JOB queue
     */
    void clearJobQueue();

    void run(bool loop);

    void broadcastJobStamp();
};

inline void LRT::setFctTbl(const lrtFct fct[], int nFct) {
    fcts_ = fct;
    nFct_ = nFct;
}

inline int LRT::getIx() const {
    return ix_;
}

inline int LRT::getJobIx() const {
    return jobIx_;
}

inline void LRT::setJobIx(int jobIx) {
    jobIx_ = jobIx;
}

inline void LRT::rstJobIx() {
    jobIx_ = -1;
}

inline void LRT::setUsePapify() {
    usePapify_ = true;
}

inline void LRT::setCommunicators() {
    lrtCommunicator_ = Platform::get()->getLrtCommunicator();
    spiderCommunicator_ = Platform::get()->getSpiderCommunicator();
}

#endif/*LRT_H*/
