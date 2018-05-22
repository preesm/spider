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

#include <papi.h>
#include <string>
#include "PapifyAction.h"
#include "PapifyEventLib.h"

PapifyAction::~PapifyAction() {
    if (outputFile_) {
        fclose(outputFile_);
    }
}


PapifyAction::PapifyAction(const char *PEType, const char *PEId, const char *actorName, int numberOfEvents,
                           std::vector<char *>& moniteredEventSet, int eventSetID, bool monitorTime ,
                           PapifyEventLib& papifyEventLib): papifyEventLib_(papifyEventLib) {
    this->PEType_         = PEType;
    this->PEId_           = PEId;
    this->actorName_      = actorName;
    this->eventSetID_     = eventSetID;
    this->numberOfEvents_ = numberOfEvents;
    this->monitorTiming_  = monitorTime;
    this->PAPIEventSetID  = 0; // Papify::getPAPIEventSetID(moniteredEventSet);

    this->counterValues_      = std::vector<long long int>((unsigned long)numberOfEvents);
    this->counterValuesStart_ = std::vector<long long int>((unsigned long)numberOfEvents);
    this->counterValuesStop_  = std::vector<long long int>((unsigned long)numberOfEvents);

    papifyEventLib.configLock();
    if (numberOfEvents > 0) {
        if (!papifyEventLib.isEventSetLaunched(eventSetID)) {
            // 1. Create and get the PAPI event set ID
            PAPIEventSetID = papifyEventLib.PAPIEventSetInit(numberOfEvents, moniteredEventSet, eventSetID, PEType);
            // 2. Launch the event set
            papifyEventLib.startEventSet(PAPIEventSetID);
            // 3. Set the event set as launched
            papifyEventLib.setPAPIEventSetStart(eventSetID);
        } else {
            // Get the PAPI event set ID
            PAPIEventSetID = papifyEventLib.getPAPIEventSetID(eventSetID);
        }
    }
    papifyEventLib.configUnlock();

    // TODO: fix this.
    std::string fileName= std::string("papify-output/papify_output_") + std::string(actorName) + std::string(".csv");
    outputFile_ = fopen(fileName.c_str(), "w");
    if (!outputFile_) {
        PapifyEventLib::throwError(__FILE__, __LINE__, "failed to open output file");
    }
    fprintf(outputFile_, "Actor,Action,Tstart,Tend,");
    for (int i = 0; i < numberOfEvents; ++i) {
        fprintf(outputFile_, "%s,", moniteredEventSet[i]);
    }
    fprintf(outputFile_, "\n");
}

void PapifyAction::startMonitor() {
    if (monitorTiming_) {
        // Update start timing value
        timeStart = PAPI_get_real_usec() - papifyEventLib_.getZeroTime();
    }
    // Retrieve the starting counter values
    if (numberOfEvents_ > 0) {
        int retVal = PAPI_read(PAPIEventSetID, counterValuesStart_.data());
        if (retVal != PAPI_OK) {
            //Papify::throwError();
        }
    }
}

void PapifyAction::stopMonitor() {
    if (monitorTiming_) {
        // Update start timing value
        timeStop = PAPI_get_real_usec() - papifyEventLib_.getZeroTime();
    }
    // Retrieve the stoping counter values
    if (numberOfEvents_ > 0) {
        int retVal = PAPI_read(PAPIEventSetID, counterValuesStop_.data());
        if (retVal != PAPI_OK) {
            //Papify::throwError();
        }
        // Compute the difference between the start end value and the start value of the counters
        for(int i = 0; i < numberOfEvents_; ++i) {
            counterValues_[i] = counterValuesStop_[i] - counterValuesStart_[i];
        }
    }
}

void PapifyAction::writeEvents() {
    if (outputFile_) {
        fprintf(outputFile_, "%s,%s, %llu, %llu", PEId_, actorName_, timeStart, timeStop);
        for (int i = 0; i < numberOfEvents_; ++i) {
            fprintf(outputFile_, ",%lld", counterValues_[i]);
        }
        fprintf(outputFile_, "\n");
    }
}