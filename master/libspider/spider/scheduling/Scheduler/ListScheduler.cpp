/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018 - 2019)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018 - 2019)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2018)
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
#include "ListScheduler.h"

#include <graphs/SRDAG/SRDAGGraph.h>
#include <launcher/Launcher.h>

ListScheduler::ListScheduler() {
    srdag_ = nullptr;
    schedule_ = nullptr;
    archi_ = nullptr;
    list_ = nullptr;
}

ListScheduler::~ListScheduler() = default;

static inline std::uint64_t saturateAddu64(std::uint64_t a, std::uint64_t b) {
    if (b > (UINT64_MAX - a)) {
        return UINT64_MAX;
    }
    return a + b;
}

void ListScheduler::mapVertex(SRDAGVertex *vertex) {
    Time minimumStartTime = computeMinimumStartTime(vertex);

    if (vertex->getState() == SRDAG_RUN) {
        vertex->setStartTime(minimumStartTime);
        vertex->setEndTime(minimumStartTime);
        return;
    }

    std::int32_t bestSlave = -1;
    Time bestStartTime = 0;
    Time bestEndTime = UINT64_MAX; // Very high value.
    std::uint64_t bestScheduleCost = UINT64_MAX;

    // Getting a slave for the vertex.
    for (std::uint32_t peIx = 0; peIx < archi_->getNPE(); peIx++) {
        auto *pe = archi_->getPEFromSpiderID(peIx);
        int slaveType = pe->getHardwareType();
        if (!pe->isEnabled()) {
            continue;
        }

        /* == Computing scheduling constraints and cost == */
        if (vertex->isExecutableOn(pe->getVirtualID())) {

            /* === Retrieving information needed for scheduling cost === */

            Time startTime = std::max(schedule_->getReadyTime(peIx), minimumStartTime);
            Time waitTime = startTime - schedule_->getReadyTime(peIx);
            Time execTime = vertex->executionTimeOn(slaveType);
            Time endTime = startTime + execTime;
            std::uint64_t readComCost = 0;
            for (std::int32_t i = 0; i < vertex->getNConnectedInEdge(); ++i) {
                auto *edge = vertex->getInEdge(i);
                auto *srcVertex = edge->getSrc();
                auto *distPE = archi_->getPEFromSpiderID(srcVertex->getScheduleJob()->getMappedPE());
                auto sendCost = distPE->getSendCostRoutine(pe->getSpiderID())(edge->getRate());
                auto readCost = pe->getReadCostRoutine(distPE->getSpiderID())(edge->getRate());
                readComCost = saturateAddu64(readComCost, saturateAddu64(sendCost, readCost));
            }

            /* === Computing total scheduling cost on this PE === */

            std::uint64_t scheduleCost = saturateAddu64(endTime + waitTime, readComCost);
            if (scheduleCost < bestScheduleCost) {
                bestSlave = peIx;
                bestEndTime = endTime;
                bestStartTime = startTime;
                bestScheduleCost = scheduleCost;
            }
        }
    }
    if (bestSlave < 0) {
        throwSpiderException("No slave found to execute one instance of vertex [%s].", vertex->toString());
    }
    Scheduler::addJobToSchedule(schedule_, vertex, bestSlave, &bestStartTime, &bestEndTime);
}
