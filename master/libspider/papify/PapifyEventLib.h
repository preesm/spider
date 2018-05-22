/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2017) :
 *
 * Daniel Madronal <daniel.madronal@upm.es> (2017-2018)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 *
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
#ifndef SPIDER_PAPIFYEVENTLIB_H
#define SPIDER_PAPIFYEVENTLIB_H

#include <pthread.h>
#include <vector>
#include <papi.h>

class PapifyEventLib {
public:
    /**
     * Initializes the PAPI library and the different interfaces needed
     */
    PapifyEventLib();
    ~PapifyEventLib();

    /**
     * @brief Throw an error related to PapifyEventLib
     *
     * @param file          File in which the error happened
     * @param line          Line of the error
     * @param message The message
     */
    static void throwError(const char* file,int line, const char* message);

    /**
     * @brief Throw an error related to PapifyEventLib
     *
     * @param file          File in which the error happened
     * @param line          Line of the error
     * @param papiErrorCode The error code
     */
    static void throwError(const char* file, int line, int papiErrorCode);

    /**
     * @brief Returned the zero time of the initialization of PAPI
     *
     * @return the zero time value
     */
    long long getZeroTime(void);

    /**
     * @brief Check if a given event set has already been launched
     * @param eventSetID User id (not PAPI) of the event set
     * @return true if already launched, false else
     */
    bool isEventSetLaunched(int eventSetID);

    /**
     * @brief Init a PAPI event set corresponding to the event passed in parameter.
     * @param numberOfEvents     Number of events in the event set
     * @param moniteredEventSet  The event set
     * @param eventSetID         User id of the event set
     * @param PEType             The PE component type
     *
     * @return the id of the event set
     */
    int PAPIEventSetInit(int numberOfEvents,
                          std::vector<char *>& moniteredEventSet, int eventSetID, const char* PEType);

    /**
     * @brief Launch the monitoring of the eventSet.
     *        This eventSet will be counting from the beginning and the actual values will
     *        be computed by differences with event_start and event_stop functions.
     *
     * @param PAPIEventSetID The PAPI event set id
     */
    inline void startEventSet(int PAPIEventSetID) {
        int retVal = PAPI_start(PAPIEventSetID);
        if (retVal != PAPI_OK) {
            throwError(__FILE__, __LINE__, retVal);
        }
    }

    /**
     * @brief Get the PAPI event set ID from the user event set ID
     *
     * @param eventSetID the user event set ID
     * @return the corresponding PAPI event set id
     */
    inline int getPAPIEventSetID(int eventSetID) {
        return PEEventSets_[eventSetID];
    }

    /**
     * @brief Set the corresponding PAPI event set as launched
     * @param eventSetID the user evetn set ID
     */
    inline void setPAPIEventSetStart(int eventSetID) {
        PEEventSetLaunched_[eventSetID] = 1;
    }

    inline void configLock() {
        pthread_mutex_lock(configLock_);
    }

    inline void configUnlock() {
        pthread_mutex_unlock(configLock_);
    }
private:
    pthread_mutex_t* configLock_;
    std::vector<int> PEEventSets_;
    std::vector<int> PEEventSetLaunched_;
    long long        zeroTime_;
};

#endif //SPIDER_PAPIFYEVENTLIB_H
