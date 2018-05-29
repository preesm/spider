/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2015) :
 *
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
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
#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <platform.h>
#include <tools/Stack.h>
#include <graphs/SRDAG/SRDAGVertex.h>

#include <algorithm>

class Schedule {
public:
    Schedule();

    Schedule(int nPE, int nJobMax);

    ~Schedule();

    inline void setAllMinReadyTime(Time time);

    inline void setReadyTime(int pe, Time time);

    inline Time getReadyTime(int pe) const;

    void addJob(int pe, SRDAGVertex *job, Time start, Time end);

    void print(const char *path);

    bool check();

private:
    int nPE_;
    int nJobMax_;
    int *nJobPerPE_;
    Time *readyTime_;
    SRDAGVertex **schedules_;

    inline int getNJobs(int pe) const;

    inline SRDAGVertex *getJob(int pe, int ix) const;
};

inline void Schedule::setAllMinReadyTime(Time time) {
    for (int i = 0; i < nPE_; i++) {
        readyTime_[i] = std::max(time, readyTime_[i]);
    }
}

inline void Schedule::setReadyTime(int pe, Time time) {
    if (pe < 0 || pe >= nPE_)
        throw std::runtime_error("Schedule: Accessing bad PE\n");
    readyTime_[pe] = time;
}

inline Time Schedule::getReadyTime(int pe) const {
    if (pe < 0 || pe >= nPE_)
        throw std::runtime_error("Schedule: Accessing bad PE\n");
    return readyTime_[pe];
}

inline int Schedule::getNJobs(int pe) const {
    if (pe < 0 || pe >= nPE_)
        throw std::runtime_error("Schedule: Accessing bad PE\n");
    return nJobPerPE_[pe];
}

inline SRDAGVertex *Schedule::getJob(int pe, int ix) const {
    if (pe < 0 || pe >= nPE_)
        throw std::runtime_error("Schedule: Accessing bad PE\n");
    if (ix < 0 || ix >= nJobPerPE_[pe])
        throw std::runtime_error("Schedule: Accessing bad Job\n");

    return schedules_[pe * nJobMax_ + ix];
}

#endif/*SCHEDULE_H*/
