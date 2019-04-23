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
#include <cinttypes>
#include <graphs/PiSDF/PiSDFVertex.h>
#include <graphs/PiSDF/PiSDFEdge.h>
#include "ScheduleJob.h"

ScheduleJob::ScheduleJob(std::int32_t nInstances, std::int32_t nPEs) {
    nInstances_ = nInstances;
    nLaunchedInstance_ = 0;
    nPEs_ = nPEs;
    /** Creating matrices and vectors **/
    mappingVector_ = CREATE_MUL_NA(TRANSFO_STACK, nInstances_, std::int32_t);
    jobIDVector_ = CREATE_MUL_NA(TRANSFO_STACK, nInstances_, std::int32_t);
    mappingStartTimeVector_ = CREATE_MUL_NA(TRANSFO_STACK, nInstances_, Time);
    mappingEndTimeVector_ = CREATE_MUL_NA(TRANSFO_STACK, nInstances_, Time);
    scheduleConstrainsMatrix_ = CREATE_MUL_NA(TRANSFO_STACK, nPEs_ * nInstances_, JobConstrain);
    peDependenciesMatrix_ = CREATE_MUL_NA(TRANSFO_STACK, nPEs_ * nInstances_, bool);
    /** Initializing **/
    for (int i = 0; i < nInstances; ++i) {
        mappingVector_[i] = -1;
        jobIDVector_[i] = 0;
        mappingStartTimeVector_[i] = UINT64_MAX;
        mappingEndTimeVector_[i] = UINT64_MAX;
        for (int j = 0; j < nPEs_; ++j) {
            peDependenciesMatrix_[i * nPEs_ + j] = false;
            scheduleConstrainsMatrix_[i * nPEs_ + j] = JobConstrain();
        }
    }

    vertex_ = nullptr;
}

ScheduleJob::~ScheduleJob() {
    vertex_ = nullptr;
    StackMonitor::free(TRANSFO_STACK, mappingVector_);
    StackMonitor::free(TRANSFO_STACK, jobIDVector_);
    StackMonitor::free(TRANSFO_STACK, mappingStartTimeVector_);
    StackMonitor::free(TRANSFO_STACK, mappingEndTimeVector_);
    StackMonitor::free(TRANSFO_STACK, scheduleConstrainsMatrix_);
    StackMonitor::free(TRANSFO_STACK, peDependenciesMatrix_);
}


void ScheduleJob::print(FILE *file, int instance) {
    auto vertexName = vertex_->getName();
    auto vertexId = vertex_->getId() + instance;

    int red = (static_cast<unsigned>(vertexId) & 3u) * 50 + 100;
    int green = ((static_cast<unsigned>(vertexId) >> 2u) & 3u) * 50 + 100;
    int blue = ((static_cast<unsigned>(vertexId) >> 4u) & 3u) * 50 + 100;
    Platform::get()->fprintf(file, "\t<event\n");
    Platform::get()->fprintf(file, "\t\tstart=\"%" PRId64"\"\n", mappingStartTimeVector_[instance]);
    Platform::get()->fprintf(file, "\t\tend=\"%" PRId64"\"\n", mappingEndTimeVector_[instance]);
    Platform::get()->fprintf(file, "\t\ttitle=\"%s\"\n", vertexName);
    Platform::get()->fprintf(file, "\t\tmapping=\"PE%d\"\n", mappingVector_[instance]);
    Platform::get()->fprintf(file, "\t\tcolor=\"#%02x%02x%02x\"\n", red, green, blue);
    Platform::get()->fprintf(file, "\t\t>%s.</event>\n", vertexName);
}

static inline void createParamINArray(JobInfoMessage *const job, std::int32_t nParamIN) {
    job->nParamIN_ = nParamIN;
    job->inParams_ = CREATE_MUL_NA(ARCHI_STACK, nParamIN, Param);
}

//static inline void setParamINDelayProperties(JobInfoMessage *const job, SRDAGVertex *const vertex) {
//    // Set persistence property
//    job->inParams_[1] = vertex->getInParam(0);
//    // Set memory address
//    job->inParams_[2] = vertex->getInParam(1);
//}
//
//static inline void setParamINFork(JobInfoMessage *const job, SRDAGVertex *const vertex) {
//    createParamINArray(job, 2 + vertex->getNConnectedInEdge() + vertex->getNConnectedOutEdge());
//    job->inParams_[0] = vertex->getNConnectedInEdge();
//    job->inParams_[1] = vertex->getNConnectedOutEdge();
//    job->inParams_[2] = vertex->getInEdge(0)->getRate();
//    for (int i = 0; i < vertex->getNConnectedOutEdge(); i++) {
//        job->inParams_[3 + i] = vertex->getOutEdge(i)->getRate();
//    }
//}
//
//static inline void setParamINJoin(JobInfoMessage *const job, SRDAGVertex *const vertex) {
//    createParamINArray(job, 2 + vertex->getNConnectedInEdge() + vertex->getNConnectedOutEdge());
//    job->inParams_[0] = vertex->getNConnectedInEdge();
//    job->inParams_[1] = vertex->getNConnectedOutEdge();
//    job->inParams_[2] = vertex->getOutEdge(0)->getRate();
//    for (int i = 0; i < vertex->getNConnectedInEdge(); i++) {
//        job->inParams_[3 + i] = vertex->getInEdge(i)->getRate();
//    }
//}

JobInfoMessage *ScheduleJob::createJobMessage(int instance) {
    auto *jobInfoMessage = CREATE_NA(ARCHI_STACK, JobInfoMessage);
    /** Set basic properties **/
    jobInfoMessage->nEdgeIN_ = vertex_->getNInEdge();
    jobInfoMessage->nEdgeOUT_ = vertex_->getNOutEdge();
    jobInfoMessage->nParamIN_ = vertex_->getNInParam();
    jobInfoMessage->nParamOUT_ = vertex_->getNOutParam();
    jobInfoMessage->srdagID_ = vertex_->getId(); // TODO: add instance number
    jobInfoMessage->fctID_ = vertex_->getFctId();
//    jobInfoMessage->specialActor_ = vertex_->getType() != SRDAG_NORMAL;
//
    /** Set jobs 2 wait and notify properties **/
    auto nPE = Spider::getArchi()->getNActivatedPE();
    jobInfoMessage->lrts2Notify_ = CREATE_MUL_NA(ARCHI_STACK, nPE, bool);
    jobInfoMessage->jobs2Wait_ = CREATE_MUL_NA(ARCHI_STACK, nPE, std::int32_t);
    auto *constrains = getScheduleConstrain(instance);
    auto *dependencies = getInstanceDependencies(instance);
    for (std::uint32_t i = 0; i < nPE; ++i) {
        /** Set jobs to wait **/
        auto &jobConstrain = constrains[i];
        jobInfoMessage->jobs2Wait_[i] = jobConstrain.jobId_;
        /** Set value of the LRTs to notify **/
        auto &peDependency = dependencies[i];
        jobInfoMessage->lrts2Notify_[i] = peDependency;
    }

    /** Creates FIFOs and Param vector **/
    jobInfoMessage->inFifos_ = CREATE_MUL_NA(ARCHI_STACK, jobInfoMessage->nEdgeIN_, Fifo);
    jobInfoMessage->outFifos_ = CREATE_MUL_NA(ARCHI_STACK, jobInfoMessage->nEdgeOUT_, Fifo);
    /** Set IN FIFOs properties **/
    for (int i = 0; i < jobInfoMessage->nEdgeIN_; ++i) {
        auto *edge = vertex_->getInEdge(i);
        jobInfoMessage->inFifos_[i].alloc = static_cast<int32_t>(edge->getAlloc() + edge->resolveCons() * instance);
        jobInfoMessage->inFifos_[i].size = static_cast<int32_t>(edge->resolveCons());
    }
    /** Set OUT FIFOs properties**/
    for (int i = 0; i < jobInfoMessage->nEdgeOUT_; ++i) {
        auto *edge = vertex_->getOutEdge(i);
        jobInfoMessage->outFifos_[i].alloc = static_cast<int32_t>(edge->getAlloc() + edge->resolveProd() * instance);
        jobInfoMessage->outFifos_[i].size = static_cast<int32_t>(edge->resolveProd());
    }
    /** Set Param properties **/
    createParamINArray(jobInfoMessage, vertex_->getNInParam());
    for (int i = 0; i < vertex_->getNInParam(); ++i) {
        jobInfoMessage->inParams_[i] = vertex_->getInParamValue(i);
    }
//    switch (vertex_->getType()) {
//        case SRDAG_NORMAL:
//            createParamINArray(jobInfoMessage, vertex_->getNInParam());
//            memcpy(jobInfoMessage->inParams_, vertex_->getInParams(), jobInfoMessage->nParamIN_ * sizeof(Param));
//            break;
//        case SRDAG_FORK:
//            setParamINFork(jobInfoMessage, vertex_);
//            break;
//        case SRDAG_JOIN:
//            setParamINJoin(jobInfoMessage, vertex_);
//            break;
//        case SRDAG_ROUNDBUFFER:
//            createParamINArray(jobInfoMessage, 2);
//            jobInfoMessage->inParams_[0] = vertex_->getInEdge(0)->getRate();
//            jobInfoMessage->inParams_[1] = vertex_->getOutEdge(0)->getRate();
//            break;
//        case SRDAG_BROADCAST:
//            createParamINArray(jobInfoMessage, 2);
//            jobInfoMessage->inParams_[0] = vertex_->getInEdge(0)->getRate();
//            jobInfoMessage->inParams_[1] = vertex_->getNConnectedOutEdge();
//            break;
//        case SRDAG_INIT:
//            createParamINArray(jobInfoMessage, 3);
//            setParamINDelayProperties(jobInfoMessage, vertex_);
//            jobInfoMessage->inParams_[0] = vertex_->getOutEdge(0)->getRate();
//            break;
//        case SRDAG_END:
//            createParamINArray(jobInfoMessage, 3);
//            setParamINDelayProperties(jobInfoMessage, vertex_);
//            jobInfoMessage->inParams_[0] = vertex_->getInEdge(0)->getRate();
//            break;
//    }
    return jobInfoMessage;
}











