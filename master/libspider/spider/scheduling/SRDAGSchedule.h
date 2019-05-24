/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018 - 2019)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018 - 2019)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2015)
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
#ifndef SRDAGSCHEDULE_H
#define SRDAGSCHEDULE_H

#include <platform.h>
#include <tools/Stack.h>

#include <algorithm>
#include "SRDAGScheduleJob.h"
#include "Schedule.h"

class SRDAGSchedule : public Schedule {
public:
    SRDAGSchedule() = default;

    SRDAGSchedule(int nPE, int nJobMax);

    ~SRDAGSchedule() override;

    void addJob(SRDAGScheduleJob *job);

    inline SRDAGScheduleJob *getJob(int id) {
        return jobs_[id];
    }

    inline void restartSchedule() override {
        nSentJobs_ = 0;
        if (nJobs_) {
            for (int i = 0; i < nJobs_; ++i) {
                jobs_[i]->resetLaunchInstances();
                auto *vertex = jobs_[i]->getVertex();
                vertex->setState(SRDAG_EXEC);
            }
        }
    }

    inline std::vector<SRDAGScheduleJob *> &getJobs() {
        return jobs_;
    }

    inline void setAllMinReadyTime(Time time) override {
        Schedule::setAllMinReadyTime(time);
    }

    inline void setReadyTime(int pe, Time time) override {
        Schedule::setReadyTime(pe, time);
    }

    inline Time getReadyTime(int pe) const override {
        return Schedule::getReadyTime(pe);
    }

    void print(const char *path) override;

    bool check() override;

    void execute() override;

private:
    std::vector<SRDAGScheduleJob *> jobs_;
};

#endif/*SCHEDULE_H*/
