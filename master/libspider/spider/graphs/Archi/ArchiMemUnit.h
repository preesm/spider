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
#ifndef SPIDER_ARCHIMEMUNIT_H
#define SPIDER_ARCHIMEMUNIT_H

#include <cstdint>
#include <cinttypes>
#include <Message.h>


inline std::uint64_t defaultAllocateRoutine(std::uint64_t size, std::uint64_t used, std::uint64_t maxSize) {
    if (used + size > maxSize) {
        throwSpiderException("Not Enough Memory. Want: %"
                                     PRIu64
                                     " -- Available: %"
                                     PRIu64
                                     "", size, maxSize - used);
    }
    used += size;
    return used;
}

inline void defaultDeallocateRoutine() {
    /* Do nothing */
}

class MemoryUnit {
public:
    /* Routines definitions */

    using allocateRoutine = std::uint64_t (*)(std::uint64_t, std::uint64_t, std::uint64_t);
    using deallocateRoutine = void (*)();
    using receiveRoutine = char *(*)(MemoryUnit *, std::uint64_t, MemoryUnit *, std::uint64_t);
    using sendRoutine = void (*)(MemoryUnit *, std::uint64_t, MemoryUnit *, std::uint64_t);

    MemoryUnit(char *base, std::uint64_t size);

    ~MemoryUnit() = default;

    /* Methods */

    inline char *virtToPhy(std::uint64_t virt) const;

    inline std::uint64_t allocateMemory(std::uint64_t size);

    inline void deallocateMemory() const;

    inline char *receiveMemory(std::uint64_t localVirtAddr, MemoryUnit *distMemoryUnit, std::uint64_t distVirtAddr);

    inline void sendMemory(std::uint64_t localVirtAddr, MemoryUnit *distMemoryUnit, std::uint64_t distVirtAddr);

    inline void resetMemoryUsage();

    /* Setters */

    inline void setAllocateRoutine(allocateRoutine routine);

    inline void setDeallocateRoutine(deallocateRoutine routine);

    inline void setReceiveRoutine(receiveRoutine routine);

    inline void setSendRoutine(sendRoutine routine);

    /* Getters */

    inline std::uint64_t size() const;

    inline std::uint64_t used() const;

    inline std::uint64_t available() const;

    inline std::uint32_t getID() const;

private:
    static std::uint32_t globalID;

    /* Core properties */

    char *base_ = nullptr;
    std::uint64_t size_ = 0;
    std::uint64_t used_ = 0;
    std::uint32_t id_ = 0;

    /* Routines */

    allocateRoutine allocateRoutine_ = defaultAllocateRoutine;
    deallocateRoutine deallocateRoutine_ = defaultDeallocateRoutine;
    receiveRoutine receiveRoutine_;
    sendRoutine sendRoutine_;
};

std::uint32_t MemoryUnit::globalID = 0;

inline char *defaultReceiveRoutine(MemoryUnit *localMemoryUnit, std::uint64_t localVirtAddr,
                                   MemoryUnit * /* distMemoryUnit */, std::uint64_t /* distVirtAddr */) {
    return localMemoryUnit->virtToPhy(localVirtAddr);
}

inline void defaultSendRoutine(MemoryUnit * /* localMemoryUnit */, std::uint64_t /* localVirtAddr */,
                               MemoryUnit * /* distMemoryUnit */, std::uint64_t  /* distVirtAddr */) {
    /* Do nothing */
}

MemoryUnit::MemoryUnit(char *base, std::uint64_t size) : base_{base},
                                                         size_{size} {
    if (!base) {
        throwSpiderException("Base address of a MemoryUnit can not be nullptr.");
    }
    id_ = MemoryUnit::globalID++;
    receiveRoutine_ = defaultReceiveRoutine;
    sendRoutine_ = defaultSendRoutine;
}

char *MemoryUnit::virtToPhy(std::uint64_t virt) const {
    return base_ + virt;
}

std::uint64_t MemoryUnit::allocateMemory(std::uint64_t size) {
    auto currentAddress = used_;
    used_ = allocateRoutine_(size, used_, size_);
    return currentAddress;
}

void MemoryUnit::deallocateMemory() const {
    deallocateRoutine_();
}

char *MemoryUnit::receiveMemory(std::uint64_t localVirtAddr, MemoryUnit *distMemoryUnit, std::uint64_t distVirtAddr) {
    return receiveRoutine_(this, localVirtAddr, distMemoryUnit, distVirtAddr);
}

void MemoryUnit::sendMemory(std::uint64_t localVirtAddr, MemoryUnit *distMemoryUnit, std::uint64_t distVirtAddr) {
    sendRoutine_(this, localVirtAddr, distMemoryUnit, distVirtAddr);
}

void MemoryUnit::setAllocateRoutine(allocateRoutine routine) {
    allocateRoutine_ = routine;
}

void MemoryUnit::setDeallocateRoutine(deallocateRoutine routine) {
    deallocateRoutine_ = routine;
}

void MemoryUnit::setReceiveRoutine(receiveRoutine routine) {
    receiveRoutine_ = routine;
}

void MemoryUnit::setSendRoutine(sendRoutine routine) {
    sendRoutine_ = routine;
}

void MemoryUnit::resetMemoryUsage() {
    used_ = 0;
}

std::uint64_t MemoryUnit::size() const {
    return size_;
}

std::uint64_t MemoryUnit::used() const {
    return used_;
}

std::uint64_t MemoryUnit::available() const {
    return size_ - used_;
}

std::uint32_t MemoryUnit::getID() const {
    return id_;
}


#endif //SPIDER_ARCHIMEMUNIT_H
