/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2017) :
 *
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2016)
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

#include "DataQueues.h"
#include <monitor/StackMonitor.h>
#include <semaphore.h>
#include <string.h>

DataQueues::DataQueues(int nLrt) {
    nLrt_ = nLrt;
    jobStampMutex_ = CREATE_MUL(ARCHI_STACK, nLrt_, std::mutex);
    jobStamps_ = CREATE_MUL(ARCHI_STACK, nLrt_, int*);
    for (int i = 0; i < nLrt_; i++) {
        jobStamps_[i] = CREATE_MUL(ARCHI_STACK, nLrt_, int);
        memset(jobStamps_[i], 0, nLrt_ * sizeof(int));
    }
    waitingSems_ = CREATE_MUL(ARCHI_STACK, nLrt_, sem_t);
    for (int i = 0; i < nLrt_; i++) {
        sem_init(&waitingSems_[i], 0, 0);
    }
}

DataQueues::~DataQueues() {
    StackMonitor::free(ARCHI_STACK, waitingSems_);
    for (int i = 0; i < nLrt_; i++) {
        StackMonitor::free(ARCHI_STACK, jobStamps_[i]);
    }
    StackMonitor::free(ARCHI_STACK, jobStamps_);
    StackMonitor::free(ARCHI_STACK, jobStampMutex_);

}

void DataQueues::updateLrtJobStamp(int lrtIx, int jobStamp) {
    jobStampMutex_[lrtIx].lock();

    /** Update the job stamp */
    jobStamps_[lrtIx][lrtIx] = jobStamp;

    /** Unlock registered lrts  */
    for (int i = 0; i < nLrt_; i++) {
        if (i != lrtIx &&
            jobStamps_[lrtIx][i] != 0 &&
            jobStamps_[lrtIx][i] <= jobStamp) {
            /** Clear registered job stamp */
            jobStamps_[lrtIx][i] = 0;

            /** Unlock the lrt */
            sem_post(&waitingSems_[i]);
        }
    }

    jobStampMutex_[lrtIx].unlock();
}

int DataQueues::waitOnJobStamp(int lrtIx, int waitingLrtIx, int jobStamp, bool blocking) {
    jobStampMutex_[waitingLrtIx].lock();

    if (jobStamps_[waitingLrtIx][waitingLrtIx] >= jobStamp) {
        /** Lrt is already at job stamp, no need to wait */
        jobStampMutex_[waitingLrtIx].unlock();
        return 0;
    } else {
        if (!blocking) {
            /** If not blocking, return 1 */
            jobStampMutex_[waitingLrtIx].unlock();
            return 1;
        } else {
            /** Register to a specific jobStamp */
            jobStamps_[waitingLrtIx][lrtIx] = jobStamp;
            jobStampMutex_[waitingLrtIx].unlock();

            /** Wait on semaphore to be unlocked */
            sem_wait(&waitingSems_[lrtIx]);
            return 0;
        }
    }
}