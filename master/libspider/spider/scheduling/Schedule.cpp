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
#include <monitor/TimeMonitor.h>
#include <launcher/Launcher.h>
#include <lrt.h>
#include "Schedule.h"


Schedule::Schedule(int nPE, int nJobMax) {
    nPE_ = nPE;
    nJobMax_ = nJobMax;
    nJobs_ = 0;

    // Creates array of jobs
    jobs_ = CREATE_MUL(TRANSFO_STACK, nPE_, std::vector<ScheduleJob *>);
    nJobPerPE_.reserve((unsigned long) nPE_);
    readyTime_.reserve((unsigned long) nPE_);
    for (int i = 0; i < nPE_; ++i) {
        jobs_[i].reserve((unsigned long) nJobMax_);
        nJobPerPE_.push_back(0);
        readyTime_.push_back(0);
    }
}

Schedule::~Schedule() {
    clearJobs();
    StackMonitor::free(TRANSFO_STACK, jobs_);
}

void Schedule::clearJobs() {
    for (int i = 0; i < nPE_; ++i) {
        for (auto &job : jobs_[i]) {
            job->~ScheduleJob();
            StackMonitor::free(ARCHI_STACK, job);
        }
        jobs_[i].clear();
    }
}

ScheduleJob *Schedule::findJobFromVertex(SRDAGVertex *vertex) {
    if (vertex) {
        for (int i = 0; i < nPE_; ++i) {
            for (auto &job: jobs_[i]) {
                if (job->getVertex() == vertex) {
                    return job;
                }
            }
        }
    }
    return nullptr;
}

void Schedule::addJob(ScheduleJob *job) {
    int pe = job->getPE();
    if (pe < 0 || pe >= nPE_) {
        throwSpiderException("Bad PE value. Value: %d -- Max: %d.", pe, nPE_);
    }
    if ((int) jobs_[pe].size() >= nJobMax_) {
        throwSpiderException("PE: %d -> nJobs: %d > nJobMax: %d.", nJobPerPE_[pe], nJobMax_);
    }
    if (!jobs_[pe].empty()) {
        job->setPreviousJob(jobs_[pe].back());
    }
    jobs_[pe].push_back(job);
    nJobs_++;
    readyTime_[pe] = std::max(readyTime_[pe], job->getEndTime());
    auto *vertex = job->getVertex();
    if (vertex) {
        vertex->setSlave(job->getPE());
        vertex->setSlaveJobIx(nJobPerPE_[pe]++);
        vertex->setStartTime(job->getStartTime());
        vertex->setEndTime(job->getEndTime());

        // Update job predecessor info
        for (int i = 0; i < vertex->getNConnectedInEdge(); ++i) {
            auto *edge = vertex->getInEdge(i);
            auto *inVertex = edge->getSrc();
            auto *precJob = findJobFromVertex(inVertex);
            if (precJob) {
                job->addPredecessor(precJob);
                precJob->addSuccessor(job);
            }
        }
    }

    /** Update the jobs it needs to wait from other LRTs **/
    job->updateJobsToWait();
}

void Schedule::print(const char *path) {
    FILE *file = Platform::get()->fopen(path);

    // Writing header
    Platform::get()->fprintf(file, "<data>\n");

    // Exporting for gantt display
    for (int pe = 0; pe < nPE_; pe++) {
        for (auto &job : jobs_[pe]) {
            job->print(file);
        }
    }
    Platform::get()->fprintf(file, "</data>\n");

    Platform::get()->fclose(file);
}

bool Schedule::check() {

    /* Check core concurrency */
    for (int pe = 0; pe < nPE_; ++pe) {
        auto &jobsOfPE = jobs_[pe];
        auto *currentJob = jobsOfPE.front();
        auto *nextJob = currentJob->getNextJob();
        while (nextJob) {
            if (currentJob->getEndTime() > nextJob->getStartTime()) {
                auto *currentJobVertex = currentJob->getVertex();
                auto *nextJobVertex = nextJob->getVertex();
                throwSpiderException("Superposition of tasks [%s] and [%s] on PE [%d].", currentJobVertex->toString(),
                                     nextJobVertex->toString(), pe);
            }
            currentJob = nextJob;
            nextJob = currentJob->getNextJob();
        }
    }

    /* Check Communications */
    for (int pe = 0; pe < nPE_; ++pe) {
        for (auto &job : jobs_[pe]) {
            for (auto &precJob : job->getPredecessors()) {
                auto *currentJobVertex = job->getVertex();
                auto *precJobVertex = precJob->getVertex();
                if (precJob->getEndTime() > job->getStartTime()) {
                    throwSpiderException("Task [%s] (PE %d) depending on task [%s] (PE %d) overlaps in time.",
                                         currentJobVertex->toString(),
                                         job->getPE(), precJobVertex->toString(), precJob->getPE());
                }
            }
        }
    }
    return true;
}

void Schedule::execute() {
    TimeMonitor::startMonitoring();
    for (int pe = 0; pe < nPE_; pe++) {
        for (auto &job : jobs_[pe]) {
            Launcher::get()->sendJob(job);
        }
    }
    Launcher::get()->sendEndNotification(this);
    TimeMonitor::endMonitoring(TRACE_SPIDER_SCHED);
    Platform::get()->getLrt()->runUntilNoMoreJobs();
}

std::vector<ScheduleJob *> &Schedule::getPEJobs(int pe) {
    if (pe < 0 || pe >= nPE_) {
        throwSpiderException("Bad PE value. Value: %d -- Max: %d.", pe, nPE_);
    }
    return jobs_[pe];
}

