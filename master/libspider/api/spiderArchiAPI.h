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
#ifndef SPIDER_SPIDERARCHIAPI_H
#define SPIDER_SPIDERARCHIAPI_H

#include <cstdint>
#include <string>

/* === Forward declarations === */

class Archi;

class PE;

class MemoryUnit;

/* === Enumerations === */

enum class SpiderPEType {
    LRT_ONLY, /*! PE is used as an LRT and does not perform any computation */
    PE_ONLY,  /*! PE is used for computation only and does not perform any job management */
    LRT_PE,   /*! PE is used as an LRT and can be used for computation */
};

enum class SpiderHWType {
    PHYS_PE,  /*! PE is instantiated in Spider and run on a core (SpiderPEType::LRT_*) */
    VIRT_PE,  /*! PE is instantiated in Spider but fully managed by an LRT (SpiderPEType::PE_ONLY) */
};

/* === Structures === */

typedef struct SpiderArchiConfig {
    std::uint32_t nPE;
    std::uint32_t nPEType;
    std::uint32_t nMemoryUnit;
} SpiderArchiConfig;

/* === Routines definitions === */

using allocateRoutine = std::uint64_t (*)(
        /* == Size to allocate == */        std::uint64_t,
        /* == Current memory used == */     std::uint64_t,
        /* == Max size of the MemUnit == */ std::uint64_t);
using deallocateRoutine = void (*)();
using receiveRoutine = char *(*)(
        /* == localMemoryUnit == */ MemoryUnit *,
        /* == localVirtAddr == */   std::uint64_t,
        /* == distMemoryUnit == */  MemoryUnit *,
        /* == distVirtAddr == */    std::uint64_t);
using sendRoutine = void (*)(
        /* == localMemoryUnit == */ MemoryUnit *,
        /* == localVirtAddr == */   std::uint64_t,
        /* == distMemoryUnit == */  MemoryUnit *,
        /* == distVirtAddr == */    std::uint64_t);
using ScheduleTimeRoutine = std::uint64_t (*)(
        /* == Number of Actors == */ std::int32_t,
        /* == Number of PEs == */    std::int32_t);

/* === API methods === */

namespace Spider {

    /* === General Archi API === */

    Archi *createArchi(SpiderArchiConfig &config);

    void setSpiderGRTVirtualID(Archi *archi, std::uint32_t id);

    /* === PE related API === */

    PE *createPE(std::uint32_t hwType,
                 std::uint32_t hwID,
                 std::uint32_t virtID,
                 std::string name = "unnamed-PE",
                 SpiderPEType spiderPEType = SpiderPEType::LRT_PE,
                 SpiderHWType spiderHWType = SpiderHWType::PHYS_PE);

    void setPESpiderPETYpe(PE *pe, SpiderPEType type);

    void setPESpiderHWTYpe(PE *pe, SpiderHWType type);

    void setPEName(PE *pe, std::string name);

    void setPEMemoryUnit(PE *pe, MemoryUnit *memoryUnit);

    void disablePE(PE *pe);

    void enablePE(PE *pe);

    /* === MemoryUnit related API === */

    MemoryUnit *createMemoryUnit(char *base, std::uint64_t size);

    void setMemoryUnitAllocateRoutine(MemoryUnit *memoryUnit, allocateRoutine routine);

    void setMemoryUnitDeallocateRoutine(MemoryUnit *memoryUnit, deallocateRoutine routine);

    void setMemoryUnitReceiveRoutine(MemoryUnit *memoryUnit, receiveRoutine routine);

    void setMemoryUnitSendRoutine(MemoryUnit *memoryUnit, sendRoutine routine);
}

#endif //SPIDER_SPIDERARCHIAPI_H
