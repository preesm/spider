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

#ifndef PAPIFY_ACTION_H
#define PAPIFY_ACTION_H

#if (!defined(NO_DLFCN) && !defined(_BGL) && !defined(_BGP))
#include <dlfcn.h>
#endif

#include <vector>
#include <cstdio>
#include "PapifyEventLib.h"


class PapifyAction {
public:
    /**
     * @brief Initialize a PapifyAction object for monitoring a given actor.
     *
     * @param PEType            Processing Element type (architecture type of the PE)
     * @param PEId              Processing Element identifier (Core0, Core1, 0, 1, etc.)
     * @param actorName         Name of the actor to monitor
     * @param numberOfEvents    Number of events to monitor
     * @param moniteredEventSet List of events to monitor
     * @param eventSetID        ID of the event set
     * @param monitorTime       Precise if papify should also monitor time for this actor
     */
    PapifyAction(const char* PEType,
                 const char* PEId,
                 const char* actorName,
                 int numberOfEvents,
                 std::vector<char*>& moniteredEventSet,
                 int eventSetID,
                 bool monitorTime, PapifyEventLib& papifyEventLib);
    ~PapifyAction();

    /**
     * @brief start the monitoring of the events
     */
    void startMonitor(void);

    /**
     * @brief stop the monitoring of the events;
     */
    void stopMonitor(void);

    /**
     * @brief Write the events values
     */
    void writeEvents();
private:
    const char* PEId_;
    const char* PEType_;
    const char* actorName_;

    // PapifyEventLib handler
    PapifyEventLib& papifyEventLib_;

    // PAPI event code
    std::vector<int> PAPIEventCodeSet;
    // PAPI event set
    int PAPIEventSetID;
    // Number of event to be monitored
    int  numberOfEvents_;
    // Counters values associated with the events
    std::vector<long long> counterValues_;      // Values of the counters (after differentiation)
    std::vector<long long> counterValuesStart_; // Starting point
    std::vector<long long> counterValuesStop_;  // End point (required to measure events by differences)
    // ID of the event set this PapifyAction belongs
    int  eventSetID_;
    // Timing monitoring can be done on its own
    bool monitorTiming_;
    long long timeStart;
    long long timeStop;

    // The file for writing the results
    FILE* outputFile_;
};

#endif // PAPIFY_ACTION_H