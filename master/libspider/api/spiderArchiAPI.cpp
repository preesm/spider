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

#include "spiderArchiAPI.h"
#include "graphs/Archi/ArchiPE.h"


/* === PE related API === */

PE *Spider::createPE(std::uint32_t hwType,
                     std::uint32_t hwID,
                     std::uint32_t virtID,
                     std::string name,
                     SpiderPEType spiderPEType,
                     SpiderHWType spiderHWType) {
    return new PE(hwType, hwID, virtID, std::move(name), spiderPEType, spiderHWType);
}

void Spider::setPESpiderPETYpe(PE *pe, SpiderPEType type) {
    pe->setSpiderPEType(type);
}

void Spider::setPESpiderHWTYpe(PE *pe, SpiderHWType type) {
    pe->setSpiderHardwareType(type);
}

void Spider::setPEName(PE *pe, std::string name) {
    pe->setName(std::move(name));
}

void Spider::setPEMemoryUnit(PE *pe, MemoryUnit *memoryUnit) {
    pe->setMemoryUnit(memoryUnit);
}

void Spider::disablePE(PE *pe) {
    pe->disable();
}

void Spider::enablePE(PE *pe) {
    pe->enable();
}

/* === MemoryUnit related API === */

MemoryUnit *Spider::createMemoryUnit(char *base, std::uint64_t size) {
    return new MemoryUnit(base, size);
}

void Spider::setMemoryUnitAllocateRoutine(MemoryUnit *memoryUnit, allocateRoutine routine) {
    memoryUnit->setAllocateRoutine(routine);
}

void Spider::setMemoryUnitDeallocateRoutine(MemoryUnit *memoryUnit, deallocateRoutine routine) {
    memoryUnit->setDeallocateRoutine(routine);
}

void Spider::setMemoryUnitReceiveRoutine(MemoryUnit *memoryUnit, receiveRoutine routine) {
    memoryUnit->setReceiveRoutine(routine);
}

void Spider::setMemoryUnitSendRoutine(MemoryUnit *memoryUnit, sendRoutine routine) {
    memoryUnit->setSendRoutine(routine);
}
