/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2019)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2019)
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
#ifndef SPIDER_SRDAGSCHEDULEJOB_H
#define SPIDER_SRDAGSCHEDULEJOB_H

#include <Message.h>

class SRDAGVertex;

typedef struct SRDAGJobConstrain {
    std::int32_t vertexId_ = -1;         // ID of the vertex we are constrained on
    std::int32_t jobId_ = -1;            // Job ID we are constrained on
} SRDAGJobConstrain;

class SRDAGScheduleJob {
public:
    explicit SRDAGScheduleJob();

    ~SRDAGScheduleJob();

    /** Methods **/
    JobInfoMessage *createJobMessage();

    void print(FILE *file, int instance);

    inline void resetLaunchInstances() {
        nLaunchedInstance_ = 0;
    }

    /** Setters **/
    inline void setVertex(SRDAGVertex *vertex) {
        vertex_ = vertex;
    }

    inline void setScheduleConstrain(int pe, std::int32_t vertexId, std::int32_t jobId) {
        scheduleConstrainsMatrix_[pe].vertexId_ = vertexId;
        scheduleConstrainsMatrix_[pe].jobId_ = jobId;
    }

    inline void setJobID(int jobID) {
        jobID_ = jobID;
    }

    inline void setMappedPE(int pe) {
        mappingPE_ = pe;
    }

    inline void setMappingStartTime(const Time *time) {
        mappingStartTime_ = *time;
    }

    inline void setMappingEndTime(const Time *time) {
        mappingEndTime_ = *time;
    }

    inline void setInstancePEDependency(int pe, bool shouldNotify) {
        peDependenciesMatrix_[pe] = shouldNotify;
    }

    inline void launchNextInstance() {
        nLaunchedInstance_++;
        if (nLaunchedInstance_ > nInstances_) {
            throwSpiderException("Launched more instances of a job than available. %d -- %d", nLaunchedInstance_,
                                 nInstances_);
        }
    }

    /** Getters **/
    inline SRDAGVertex *getVertex() {
        return vertex_;
    }

    inline std::int32_t getJobID() {
        return jobID_;
    }

    inline std::int32_t getNumberOfInstances() {
        return 1;
    }

    inline std::int32_t getNumberOfLaunchedInstances() {
        return nLaunchedInstance_;
    }

    inline SRDAGJobConstrain *getScheduleConstrain() {
        return scheduleConstrainsMatrix_;
    }

    inline std::int32_t getMappedPE() {
        return mappingPE_;
    }

    inline Time getMappingStartTime() {
        return mappingStartTime_;
    }

    inline Time getMappingEndTime() {
        return mappingEndTime_;
    }

    inline const bool *getInstanceDependencies() {
        return peDependenciesMatrix_;
    }

private:
    /**
     * @brief Vertex to which the job is attached
     */
    SRDAGVertex *vertex_;
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
    std::int32_t mappingPE_;
    /**
     * @brief Job ID vector
     */
    std::int32_t jobID_;
    /**
     * @brief Vector of starting time of each instances. Size = 1 * nInstances_
     */
    Time mappingStartTime_;
    /**
     * @brief Vector of end time of each instances. Size = 1 * nInstances_
     */
    Time mappingEndTime_;
    /**
     * @brief Constrains for scheduling each instance. Size = nPEs_ * nInstances_
     */
    SRDAGJobConstrain *scheduleConstrainsMatrix_;
    /**
     * @brief Dependencies to notify for each instance. Size = nPEs_ * nInstances_
     */
    bool *peDependenciesMatrix_;
};

#endif //SPIDER_PISDFSCHEDULEJOB_H
