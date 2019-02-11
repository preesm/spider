/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2015)
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
#ifndef SPIDER_SCHEDULEJOB_H
#define SPIDER_SCHEDULEJOB_H

#include <graphs/SRDAG/SRDAGVertex.h>
#include <Message.h>

class ScheduleJob {
public:
    ScheduleJob(SRDAGVertex *vertex, int pe, int lrt);

    ScheduleJob(PiSDFVertex *vertex, int pe, int lrt);

    ~ScheduleJob();

    /** Setters **/
    inline void setPreviousJob(ScheduleJob *job) {
        previousJob_ = job;
        job->setNextJob(this);
    }

    inline void setNextJob(ScheduleJob *job) {
        nextJob_ = job;

    }

    inline void setStartTime(Time start) {
        startTime_ = start;
    }

    inline void setEndTime(Time end) {
        endTime_ = end;
    }

    /** Getters **/
    inline SRDAGVertex *getVertex() const {
        return vertex_;
    }

    inline int getLRT() {
        return lrt_;
    }

    inline int getPE() {
        return pe_;
    }

    inline ScheduleJob *getPreviousJob() {
        return previousJob_;
    }

    inline ScheduleJob *getNextJob() {
        return nextJob_;
    }

    inline Time getStartTime() {
        return startTime_;
    }

    inline Time getEndTime() {
        return endTime_;
    }

    inline std::vector<ScheduleJob *> &getSuccessors() {
        return successors_;
    }

    inline std::vector<ScheduleJob *> &getPredecessors() {
        return predecessors_;
    }

    inline std::int32_t *getJobs2Wait() const {
        return jobsToWait_;
    }

    /** Methods **/
    bool isBeforeJob(ScheduleJob *job);

    bool isAfterJob(ScheduleJob *job);

    inline void addSuccessor(ScheduleJob *job) {
        successors_.push_back(job);
    }

    inline void addPredecessor(ScheduleJob *job) {
        predecessors_.push_back(job);
    }

    void updateJobsToWait();

    JobInfoMessage *createJobMessage();

    void print(FILE *file);

private:
    /**
     * @brief Vertex associated to the job
     */
    SRDAGVertex *vertex_;
    PiSDFVertex *piSDFVertex_;
    /**
     * @brief PE on which the job is executed
     */
    int pe_;
    /**
     * @brief LRT handling the job
     */
    int lrt_;
    /**
     * @brief Start time of the job
     */
    Time startTime_;
    /**
     * @brief End time of the job
     */
    Time endTime_;
    /**
     * @brief Previous job on the same lrt
     */
    ScheduleJob *previousJob_;
    /**
     * @brief Next job on the same lrt
     */
    ScheduleJob *nextJob_;
    /**
     * @brief Job direct successor dependencies
     */
    std::vector<ScheduleJob *> successors_;
    /**
     * @brief Job direct predecessors dependencies
     */
    std::vector<ScheduleJob *> predecessors_;
    /**
     * @brief Indexes of jobs to wait
     */
    std::int32_t *jobsToWait_;
};

#endif //SPIDER_SCHEDULEJOB_H
