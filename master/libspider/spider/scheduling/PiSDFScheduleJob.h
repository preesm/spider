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

#include <Message.h>

class PiSDFVertex;

typedef struct JobConstrain {
    PiSDFVertex *vertex_;           // ID of the vertex we are constrained on
    std::int32_t vertexInstance_;   // Instance of the vertex
    std::int32_t jobId_;            // Job ID we are constrained on

    ~JobConstrain() {
        vertex_ = nullptr;
    }

    JobConstrain() : vertex_{nullptr},
                     vertexInstance_{-1},
                     jobId_{-1} {
    }
} JobConstrain;

class ScheduleJob {
public:
    ScheduleJob(std::int32_t nInstances, std::int32_t nPEs);

    ~ScheduleJob();

    /** Methods **/
    JobInfoMessage *createJobMessage(int instance);

    void print(FILE *file, int instance);

    inline void resetLaunchInstances() {
        nLaunchedInstance_ = 0;
    }

    /** Setters **/

    inline void setVertex(PiSDFVertex *vertex) {
        vertex_ = vertex;
    }

    inline void setScheduleConstrain(int instance, int pe, PiSDFVertex *vertex, std::int32_t jobId,
                                     std::int32_t vertexInstance = 0) {
        checkInstance(instance);
        scheduleConstrainsMatrix_[instance * nPEs_ + pe].vertex_ = vertex;
        scheduleConstrainsMatrix_[instance * nPEs_ + pe].vertexInstance_ = vertexInstance;
        scheduleConstrainsMatrix_[instance * nPEs_ + pe].jobId_ = jobId;
    }

    inline void setJobID(int instance, int jobID) {
        checkInstance(instance);
        jobIDVector_[instance] = jobID;
    }

    inline void setMappedPE(int instance, int pe) {
        checkInstance(instance);
        mappingVector_[instance] = pe;
    }

    inline void setMappingStartTime(int instance, const Time *time) {
        checkInstance(instance);
        mappingStartTimeVector_[instance] = *time;
    }

    inline void setMappingEndTime(int instance, const Time *time) {
        checkInstance(instance);
        mappingEndTimeVector_[instance] = *time;
    }

    inline void setInstancePEDependency(int instance, int pe, bool shouldNotify) {
        checkInstance(instance);
        peDependenciesMatrix_[instance * nPEs_ + pe] = shouldNotify;
    }

    inline void launchNextInstance() {
        nLaunchedInstance_++;
        if (nLaunchedInstance_ > nInstances_) {
            throwSpiderException("Launched more instances of a job than available. %d -- %d", nLaunchedInstance_,
                                 nInstances_);
        }
    }

    /** Getters **/
    inline PiSDFVertex *getVertex() {
        return vertex_;
    }

    inline std::int32_t getJobID(int instance) {
        checkInstance(instance);
        return jobIDVector_[instance];
    }

    inline std::int32_t getNumberOfInstances() {
        return nInstances_;
    }

    inline std::int32_t getNumberOfLaunchedInstances() {
        return nLaunchedInstance_;
    }

    inline JobConstrain *getScheduleConstrain(int instance) {
        checkInstance(instance);
        return &scheduleConstrainsMatrix_[instance * nPEs_];
    }

    inline std::int32_t getMappedPE(int instance) {
        checkInstance(instance);
        return mappingVector_[instance];
    }

    inline Time getMappingStartTime(int instance) {
        checkInstance(instance);
        return mappingStartTimeVector_[instance];
    }

    inline Time getMappingEndTime(int instance) {
        checkInstance(instance);
        return mappingEndTimeVector_[instance];
    }

    inline const bool *getInstanceDependencies(int instance) {
        checkInstance(instance);
        return &peDependenciesMatrix_[instance * nPEs_];
    }

private:
    /**
     * @brief Vertex to which the job is attached
     */
    PiSDFVertex *vertex_;
    /**
     * @brief Total number of instances
     */
    std::int32_t nInstances_;
    /**
     * @brief Number of launched instances.
     */
    std::int32_t nLaunchedInstance_;
    /**
     * @brief Number of PEs.
     */
    std::int32_t nPEs_;
    /**
     * @brief Vector of Mapped PE for each instance of the job. Size = 1 * nInstances_
     */
    std::int32_t *mappingVector_;
    /**
     * @brief Job ID vector
     */
    std::int32_t *jobIDVector_;
    /**
     * @brief Vector of starting time of each instances. Size = 1 * nInstances_
     */
    Time *mappingStartTimeVector_;
    /**
     * @brief Vector of end time of each instances. Size = 1 * nInstances_
     */
    Time *mappingEndTimeVector_;
    /**
     * @brief Constrains for scheduling each instance. Size = nPEs_ * nInstances_
     */
    JobConstrain *scheduleConstrainsMatrix_;
    /**
     * @brief Dependencies to notify for each instance. Size = nPEs_ * nInstances_
     */
    bool *peDependenciesMatrix_;

    inline void checkInstance(std::int32_t &instance) const;
};

void ScheduleJob::checkInstance(std::int32_t &instance) const {
    if (instance < 0 || instance >= nInstances_) {
        throwSpiderException("Bad instance value: %d", instance);
    }
}

#endif //SPIDER_SCHEDULEJOB_H
