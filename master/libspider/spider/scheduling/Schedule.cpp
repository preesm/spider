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

Schedule::Schedule(int nPE, int nJobMax) {
    nPE_ = nPE;
    nJobMax_ = nJobMax;
    nJobs_ = 0;
    nJobPerPE_ = CREATE_MUL(TRANSFO_STACK, nPE_, int);
    readyTime_ = CREATE_MUL(TRANSFO_STACK, nPE_, Time);
    schedules_ = CREATE_MUL(TRANSFO_STACK, nPE_ * nJobMax_, SRDAGVertex*);

    memset(nJobPerPE_, 0, nPE_ * sizeof(int));
    memset(readyTime_, 0, nPE_ * sizeof(Time));
}

Schedule::~Schedule() {
    StackMonitor::free(TRANSFO_STACK, nJobPerPE_);
    StackMonitor::free(TRANSFO_STACK, readyTime_);
    StackMonitor::free(TRANSFO_STACK, schedules_);
}

void Schedule::addJob(int pe, SRDAGVertex *job, Time start, Time end) {
    if (pe < 0 || pe >= nPE_) {
        throwSpiderException("Bad PE value. Value: %d -- Max: %d.", pe, nPE_);
    }
    if (nJobPerPE_[pe] >= nJobMax_) {
        throwSpiderException("PE: %d -> nJobs: %d > nJobMax: %d.", nJobPerPE_[pe], nJobMax_);
    }

    schedules_[pe * nJobMax_ + nJobPerPE_[pe]] = job;
    job->setSlaveJobIx(nJobPerPE_[pe]);
    nJobPerPE_[pe]++;
    nJobs_++;
    readyTime_[pe] = std::max(readyTime_[pe], end);
    job->setStartTime(start);
    job->setEndTime(end);
}

void Schedule::print(const char *path) {
    FILE *file = Platform::get()->fopen(path);
    char name[100];

    // Writing header
    Platform::get()->fprintf(file, "<data>\n");

    // Exporting for gantt display
    for (int pe = 0; pe < nPE_; pe++) {
        for (int job = 0; job < nJobPerPE_[pe]; job++) {
            SRDAGVertex *vertex = getJob(pe, job);

            vertex->toString(name, 100);
            Platform::get()->fprintf(file, "\t<event\n");
            Platform::get()->fprintf(file, "\t\tstart=\"%d\"\n", vertex->getStartTime());
            Platform::get()->fprintf(file, "\t\tend=\"%d\"\n", vertex->getEndTime());
            Platform::get()->fprintf(file, "\t\ttitle=\"%s\"\n", name);
            Platform::get()->fprintf(file, "\t\tmapping=\"PE%d\"\n", pe);

            int ired = (vertex->getId() & 0x3) * 50 + 100;
            int igreen = ((vertex->getId() >> 2) & 0x3) * 50 + 100;
            int iblue = ((vertex->getId() >> 4) & 0x3) * 50 + 100;
            Platform::get()->fprintf(file, "\t\tcolor=\"#%02x%02x%02x\"\n", ired, igreen, iblue);

            Platform::get()->fprintf(file, "\t\t>%s.</event>\n", name);
        }
    }
    Platform::get()->fprintf(file, "</data>\n");

    Platform::get()->fclose(file);
}

bool Schedule::check() {
    bool result = true;

    /* Check core concurrency */
    for (int pe = 0; pe < nPE_ && result; pe++) {
        for (int i = 0; i < nJobPerPE_[pe] - 1 && result; i++) {
            SRDAGVertex *vertex = getJob(pe, i);
            SRDAGVertex *nextVertex = getJob(pe, i + 1);

            if (vertex->getEndTime() > nextVertex->getStartTime()) {
                result = false;
                char name[100];
                vertex->toString(name, 100);
                printf("Schedule: Superposition: task %s ", name);
                nextVertex->toString(name, 100);
                printf("and %s\n", name);
            }
        }
    }

    /* Check Communications */
    for (int pe = 0; pe < nPE_ && result; pe++) {
        for (int i = 0; i < nJobPerPE_[pe] - 1 && result; i++) {
            SRDAGVertex *vertex = getJob(pe, i);

            for (int j = 0; j < vertex->getNConnectedInEdge() && result; j++) {
                SRDAGVertex *precVertex = vertex->getInEdge(j)->getSrc();

                if (vertex->getStartTime() < precVertex->getEndTime()) {
                    result = false;
                    char name[100];
                    vertex->toString(name, 100);
                    printf("Schedule: Communication: task %s ", name);
                    precVertex->toString(name, 100);
                    printf("and %s\n", name);
                }
            }
        }
    }
    return result;
}

void Schedule::execute() {
    TimeMonitor::startMonitoring();
    for (int pe = 0; pe < nPE_; pe++) {
        for (int job = 0; job < nJobPerPE_[pe]; job++) {
            SRDAGVertex *vertex = getJob(pe, job);
            vertex->setState(SRDAG_EXEC);
            Launcher::get()->launchVertex(vertex);
        }
    }
    Launcher::get()->sendEndNotification(this);
    TimeMonitor::endMonitoring(TRACE_SPIDER_SCHED);
    Platform::get()->getLrt()->runUntilNoMoreJobs();
}

