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
#include "ScheduleJob.h"

ScheduleJob::ScheduleJob(SRDAGVertex *vertex, int pe, int lrt) {
    vertex_ = vertex;
    pe_ = pe;
    lrt_ = lrt;
    previousJob_ = nullptr;
    nextJob_ = nullptr;
    startTime_ = (Time) -1;
    endTime_ = (Time) -1;
    successors_.reserve((size_t) vertex->getNConnectedOutEdge());
    predecessors_.reserve((size_t) vertex->getNConnectedInEdge());
}

ScheduleJob::~ScheduleJob() {
    vertex_ = nullptr;
    previousJob_ = nullptr;
    nextJob_ = nullptr;
    successors_.clear();
    predecessors_.clear();
}

bool ScheduleJob::isBeforeJob(ScheduleJob *job) {
    auto *currentNextJob = nextJob_;
    while (currentNextJob) {
        if (currentNextJob == job) {
            return true;
        }
        currentNextJob = currentNextJob->getNextJob();
    }
    return false;
}

bool ScheduleJob::isAfterJob(ScheduleJob *job) {
    return job->isBeforeJob(this);
}

void ScheduleJob::print(FILE *file, const char *name) {
    int red = (vertex_->getId() & 0x3) * 50 + 100;
    int green = ((vertex_->getId() >> 2) & 0x3) * 50 + 100;
    int blue = ((vertex_->getId() >> 4) & 0x3) * 50 + 100;

    Platform::get()->fprintf(file, "\t<event\n");
    Platform::get()->fprintf(file, "\t\tstart=\"%" PRId64"\"\n", startTime_);
    Platform::get()->fprintf(file, "\t\tend=\"%" PRId64"\"\n", endTime_);
    Platform::get()->fprintf(file, "\t\ttitle=\"%s\"\n", name);
    Platform::get()->fprintf(file, "\t\tmapping=\"PE%d\"\n", pe_);
    Platform::get()->fprintf(file, "\t\tcolor=\"#%02x%02x%02x\"\n", red, green, blue);
    Platform::get()->fprintf(file, "\t\t>%s.</event>\n", name);
}


