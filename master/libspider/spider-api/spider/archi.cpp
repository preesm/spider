/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018 - 2019)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018 - 2019)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2018)
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
#include <spider-api/user/archi.h>
#include <graphs/Archi/ArchiPE.h>
#include <graphs/Archi/Archi.h>
#include <spider.h>

/* === Default routines === */

std::uint64_t defaultInfiniteCommunicationCost(std::uint64_t) {
    return UINT64_MAX;
}


/* === General Archi API === */

Archi *Spider::createArchi(SpiderArchiConfig &config) {
    if (Spider::getArchi()) {
        throwSpiderException("Only one Archi is permitted per Spider instance.");
    }
    auto *archi = CREATE_NA(ARCHI_STACK, Archi)(
            /* == nPE == */         config.nPE,
            /* == nPEType == */     config.nPEType,
            /* == nMemoryUnit == */ config.nMemoryUnit);
    Spider::setArchi(archi);
    return archi;
}


void Spider::setSpiderGRTVirtualID(Archi *archi, std::uint32_t id) {
    archi->setSpiderGRTID(id);
}

/* === PE related API === */

PE *Spider::createPE(std::uint32_t hwType,
                     std::uint32_t hwID,
                     std::uint32_t virtID,
                     std::string name,
                     SpiderPEType spiderPEType,
                     SpiderHWType spiderHWType) {
    auto *pe = CREATE_NA(ARCHI_STACK, PE)(hwType, hwID, virtID, std::move(name), spiderPEType, spiderHWType);
    auto *archi = Spider::getArchi();
    archi->addPE(pe);
    archi->activatePE(pe);
    return pe;
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
    Spider::getArchi()->deactivatePE(pe);
}

void Spider::enablePE(PE *pe) {
    Spider::getArchi()->activatePE(pe);
}

void Spider::setReadCostRoutinePEToPE(PE *peA, PE *peB, CommunicationCostRoutine routine) {
    peA->setReadCostRoutine(peB->getSpiderID(), routine);
}

void Spider::setSendCostRoutinePEToPE(PE *peA, PE *peB, CommunicationCostRoutine routine) {
    peA->setSendCostRoutine(peB->getSpiderID(), routine);
}

void Spider::setReadCostRoutineType(std::uint32_t type, CommunicationCostRoutine routine) {
    auto *archi = Spider::getArchi();
    for (std::uint32_t i = 0; i < archi->getNPE(); ++i) {
        auto *pe = archi->getPEFromSpiderID(i);
        if (pe->getHardwareType() == type) {
            for (std::uint32_t j = i + 1; j < archi->getNPE(); ++j) {
                auto *peDist = archi->getPEFromSpiderID(j);
                if (peDist->getHardwareType() == type) {
                    peDist->setReadCostRoutine(i, routine);
                    pe->setReadCostRoutine(j, routine);
                }
            }
        }
    }
}

void Spider::setSendCostRoutineType(std::uint32_t type, CommunicationCostRoutine routine) {
    auto *archi = Spider::getArchi();
    for (std::uint32_t i = 0; i < archi->getNPE(); ++i) {
        auto *pe = archi->getPEFromSpiderID(i);
        if (pe->getHardwareType() == type) {
            for (std::uint32_t j = i + 1; j < archi->getNPE(); ++j) {
                auto *peDist = archi->getPEFromSpiderID(j);
                if (peDist->getHardwareType() == type) {
                    peDist->setSendCostRoutine(i, routine);
                    pe->setSendCostRoutine(j, routine);
                }
            }
        }
    }
}

void Spider::setReadCostRoutineTypeToType(std::uint32_t typeA, std::uint32_t typeB, CommunicationCostRoutine routine) {
    auto *archi = Spider::getArchi();
    for (std::uint32_t i = 0; i < archi->getNPE(); ++i) {
        auto *pe = archi->getPEFromSpiderID(i);
        if (pe->getHardwareType() == typeA) {
            for (std::uint32_t j = i + 1; j < archi->getNPE(); ++j) {
                auto *peDist = archi->getPEFromSpiderID(j);
                if (peDist->getHardwareType() == typeB) {
                    pe->setReadCostRoutine(j, routine);
                }
            }
        }
    }
}

void Spider::setSendCostRoutineTypeToType(std::uint32_t typeA, std::uint32_t typeB, CommunicationCostRoutine routine) {
    auto *archi = Spider::getArchi();
    for (std::uint32_t i = 0; i < archi->getNPE(); ++i) {
        auto *pe = archi->getPEFromSpiderID(i);
        if (pe->getHardwareType() == typeA) {
            for (std::uint32_t j = i + 1; j < archi->getNPE(); ++j) {
                auto *peDist = archi->getPEFromSpiderID(j);
                if (peDist->getHardwareType() == typeB) {
                    pe->setSendCostRoutine(j, routine);
                }
            }
        }
    }
}

void Spider::disableCommunicationsBetweenPEs(PE *peA, PE *peB) {
    /* == Set routines of peA == */
    peA->setReadCostRoutine(peB->getSpiderID(), defaultInfiniteCommunicationCost);
    peA->setSendCostRoutine(peB->getSpiderID(), defaultInfiniteCommunicationCost);
    /* == Set routines of peB == */
    peB->setReadCostRoutine(peA->getSpiderID(), defaultInfiniteCommunicationCost);
    peB->setSendCostRoutine(peA->getSpiderID(), defaultInfiniteCommunicationCost);
}

void Spider::disableReadSendBetweenPEs(PE *peA, PE *peB) {
    /* == Set routines of peA == */
    peA->setReadCostRoutine(peB->getSpiderID(), defaultInfiniteCommunicationCost);
    /* == Set routines of peB == */
    peB->setSendCostRoutine(peA->getSpiderID(), defaultInfiniteCommunicationCost);
}

void Spider::disableSendReadBetweenPEs(PE *peA, PE *peB) {
    /* == Set routines of peA == */
    peA->setSendCostRoutine(peB->getSpiderID(), defaultInfiniteCommunicationCost);
    /* == Set routines of peB == */
    peB->setReadCostRoutine(peA->getSpiderID(), defaultInfiniteCommunicationCost);
}

/* === MemoryUnit related API === */

MemoryUnit *Spider::createMemoryUnit(char *base, std::uint64_t size) {
    auto *memoryUnit = CREATE_NA(ARCHI_STACK, MemoryUnit)(base, size);
    auto *archi = Spider::getArchi();
    archi->addMemoryUnit(memoryUnit);
    return memoryUnit;
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





