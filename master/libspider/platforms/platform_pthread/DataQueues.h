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
#ifndef SPIDER_DATAQUEUES_H
#define SPIDER_DATAQUEUES_H

#if defined _WIN32 && !defined _MSC_VER
#include <mingw-std-threads/include/mingw.mutex.h>
#else
#include <mutex>
#endif
#include <semaphore.h>

/**
 * Handles LRT synchronisation for data communications.
 */
class DataQueues {

public:

    /**
     * Constructor.
     * @param nLrt Number of Lrt in the platform.
     */
    explicit DataQueues(int nLrt);

    /**
     * Destructor.
     */
    ~DataQueues();

    /**
     * Update the job stamp of a lrt.
     * @param lrtIx Lrt index.
     * @param jobStamp New job stamp value.
     */
    void updateLrtJobStamp(int lrtIx, int jobStamp);

    /**
     * Wait a lrt to be at a specific job stamp.
     * @param lrtIx Demanding lrt index.
     * @param waitingLrtIx Target lrt index.
     * @param jobStamp Job stamp to wait.
     * @param blocking True if the call is blocking.
     * @return 0 if lrt is at the specific job stamp, 1 otherwise.
     */
    int waitOnJobStamp(int lrtIx, int waitingLrtIx, int jobStamp, bool blocking);

private:

    int nLrt_;
    sem_t *waitingSems_;
    int **jobStamps_;
    std::mutex *jobStampMutex_;
};


#endif //SPIDER_DATAQUEUES_H
