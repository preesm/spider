/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2018) :
 *
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
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
#include <papi.h>
#include <string>
#include "PapifyAction.h"

PapifyAction::~PapifyAction() {
    if (outputFile_) {
        fclose(outputFile_);
    }
}

PapifyAction::PapifyAction(PapifyAction &papifyAction, long long PEId) {
    PEId_ = PEId;
    PEType_ = papifyAction.PEType_;
    actorName_ = papifyAction.actorName_;
    eventSetID_ = papifyAction.eventSetID_;
    numberOfEvents_ = papifyAction.numberOfEvents_;
    monitorTiming_ = papifyAction.monitorTiming_;
    papifyEventLib_ = papifyAction.getPapifyEventLib();

    counterValues_ = std::vector<long long int>((unsigned long) this->numberOfEvents_);
    counterValuesStart_ = std::vector<long long int>((unsigned long) this->numberOfEvents_);
    counterValuesStop_ = std::vector<long long int>((unsigned long) this->numberOfEvents_);

    // Initialize the the PAPIEventCodeSet
    PAPIEventCodeSet_ = papifyAction.PAPIEventCodeSet_;

    // Register the thread
    papifyEventLib_->configLock();
    papifyEventLib_->registerProcessingElement(PEId_);
    if (this->numberOfEvents_ > 0) {
        this->PAPIEventCodeSet_.resize((unsigned long) numberOfEvents_);
        if (!papifyEventLib_->doesEventSetExists(PEId_, eventSetID_)) {
            // 1. Create and get the PAPI event set ID
            PAPIEventSetID_ = papifyEventLib_->registerNewThread(numberOfEvents_,
                                                                 PEType_,
                                                                 PEId_,
                                                                 eventSetID_,
                                                                 PAPIEventCodeSet_);
        } else {
            // Get the PAPI event set ID
            PAPIEventSetID_ = papifyEventLib_->getPAPIEventSetID(PEId_, eventSetID_);
        }
    }
    papifyEventLib_->configUnlock();

    outputFile_ = nullptr;
}


PapifyAction::PapifyAction(const char *PEType,
                           long long PEId,
                           const char *actorName,
                           int numberOfEvents,
                           std::vector<const char *> &moniteredEventSet,
                           int eventSetID,
                           bool monitorTime,
                           PapifyEventLib *papifyEventLib) {
    PEType_ = PEType;
    PEId_ = PEId;
    actorName_ = actorName;
    eventSetID_ = eventSetID;
    numberOfEvents_ = numberOfEvents;
    monitorTiming_ = monitorTime;
    papifyEventLib_ = papifyEventLib;

    counterValues_ = std::vector<long long>((unsigned long) numberOfEvents_);
    counterValuesStart_ = std::vector<long long>((unsigned long) numberOfEvents_);
    counterValuesStop_ = std::vector<long long>((unsigned long) numberOfEvents_);

    papifyEventLib->configLock();
    papifyEventLib->registerProcessingElement(PEId_);
    if (numberOfEvents > 0) {
        this->PAPIEventCodeSet_.resize((unsigned long) numberOfEvents);
        if (!papifyEventLib->doesEventSetExists(PEId_, eventSetID)) {
            // 1. Create and get the PAPI event set ID
            PAPIEventSetID_ = papifyEventLib->PAPIEventSetInit(
                    numberOfEvents,         /* Number of events being monitored */
                    moniteredEventSet,      /* Vector of the event to be monitored */
                    eventSetID,             /* User event set ID (specified at compile time) */
                    PEType,                 /* Type of the processing element */
                    PEId_,
                    PAPIEventCodeSet_  /* PAPI event code set associated to the event monitored */);
        } else {
            // Get the PAPI event set ID
            PAPIEventSetID_ = papifyEventLib->getPAPIEventSetID(PEId_, eventSetID);
            // Initialize the the PAPIEventCodeSet
            papifyEventLib->getPAPIEventCodeSet(moniteredEventSet, this->PAPIEventCodeSet_);
        }
    }
    papifyEventLib->configUnlock();

    outputFile_ = nullptr;
}

void PapifyAction::startMonitor() {
    if (monitorTiming_) {
        // Update start timing value
        timeStart = PAPI_get_real_usec() - papifyEventLib_->getZeroTime();
    }
    // Retrieve the starting counter values
    if (numberOfEvents_ > 0) {
        // 1. Do we need to stop and start ?
        if (!papifyEventLib_->isEventSetRunning(PEId_, PAPIEventSetID_)) {
            if (papifyEventLib_->isSomeEventSetRunning(PEId_)) {
                papifyEventLib_->stopEventSetRunning(PEId_);
            }
            papifyEventLib_->startEventSet(PEId_, PAPIEventSetID_);
        }
        // 2. Let's read the value
        int retVal = PAPI_read(PAPIEventSetID_, counterValuesStart_.data());
        if (retVal != PAPI_OK) {
            PapifyEventLib::throwError(__FILE__, __LINE__, retVal);
        }
    }
}

void PapifyAction::stopMonitor() {
    if (monitorTiming_) {
        // Update start timing value
        timeStop = PAPI_get_real_usec() - papifyEventLib_->getZeroTime();
    }
    // Retrieve the stoping counter values
    if (numberOfEvents_ > 0) {
        int retVal = PAPI_read(PAPIEventSetID_, counterValuesStop_.data());
        if (retVal != PAPI_OK) {
            PapifyEventLib::throwError(__FILE__, __LINE__, retVal);
        }
        // Compute the difference between the start end value and the start value of the counters
        for (int i = 0; i < numberOfEvents_; ++i) {
            counterValues_[i] = counterValuesStop_[i] - counterValuesStart_[i];
        }
    }
}

void PapifyAction::writeEvents() {
    if (!outputFile_) {
        std::string fileName = std::string("papify-output/") +
                               std::string("LRT_") +
                               std::to_string(PEId_) +
                               std::string("__") +
                               actorName_ +
                               std::string(".csv");
        outputFile_ = fopen(fileName.c_str(), "w");
        if (!outputFile_) {
            PapifyEventLib::throwError(__FILE__, __LINE__, "failed to open output file");
        }
        fprintf(outputFile_, "PE,Actor,tinit,tend,");
        char eventName[PAPI_MAX_STR_LEN];
        for (int i = 0; i < numberOfEvents_; ++i) {
            int retVal = PAPI_event_code_to_name(PAPIEventCodeSet_[i], eventName);
            if (retVal != PAPI_OK) {
                PapifyEventLib::throwError(__FILE__, __LINE__, retVal);
            }
            fprintf(outputFile_, "%s,", eventName);
        }
        fprintf(outputFile_, "\n");
    }
    writeEvents(outputFile_);
}

void PapifyAction::writeEvents(FILE *file) {
    if (file) {
        fprintf(outputFile_, "%lld,%s,%llu,%llu", PEId_, actorName_, timeStart, timeStop);
        for (int i = 0; i < numberOfEvents_; ++i) {
            fprintf(outputFile_, ",%lld", counterValues_[i]);
        }
        fprintf(outputFile_, "\n");
    }
}
