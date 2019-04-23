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

#include "Archi.h"

Time defaultScheduleRoutine(std::int32_t nActor, std::int32_t /*nPE*/) {
    return (Time) 1 * nActor;
}

Archi::Archi(std::uint32_t nPE, std::uint32_t nPEType, std::uint32_t nMemoryUnit) : nPE_{nPE},
                                                                                    nMemUnit_{nMemoryUnit},
                                                                                    nPEType_{nPEType} {
    /* === Init peArray_ === */

    peArray_ = CREATE_MUL_NA(ARCHI_STACK, nPE, PE *);
    for (std::uint32_t i = 0; i < nPE_; ++i) {
        peArray_[i] = nullptr;
    }

    /* === Init memoryUnitArray_ === */

    memoryUnitArray_ = CREATE_MUL_NA(ARCHI_STACK, nMemoryUnit, MemoryUnit *);
    for (std::uint32_t i = 0; i < nMemUnit_; ++i) {
        memoryUnitArray_[i] = nullptr;
    }

    /* === Set the scheduleTimeRoutine === */

    scheduleTimeRoutine_ = defaultScheduleRoutine;
}

Archi::~Archi() {
    /* === Freeing resources of PE === */

    for (std::uint32_t i = 0; i < nPE_; ++i) {
        auto *pe = peArray_[i];
        pe->~PE();
        StackMonitor::free(ARCHI_STACK, pe);
        peArray_[i] = nullptr;
    }
    StackMonitor::free(ARCHI_STACK, peArray_);

    /* === Freeing resources of MemoryUnit === */

    for (std::uint32_t i = 0; i < nMemUnit_; ++i) {
        auto *memoryUnit = memoryUnitArray_[i];
        memoryUnit->~MemoryUnit();
        StackMonitor::free(ARCHI_STACK, memoryUnit);
        memoryUnitArray_[i] = nullptr;
    }
    StackMonitor::free(ARCHI_STACK, memoryUnitArray_);
}