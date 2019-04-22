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
#ifndef SPIDER_ARCHI_H
#define SPIDER_ARCHI_H

#include "ArchiMemUnit.h"
#include "ArchiPE.h"

class Archi {
public:

    Archi(std::uint32_t nPE, std::uint32_t nPEType, std::uint32_t nMemoryUnit);

    ~Archi();

    /* === Methods === */

    inline void addPE(PE *pe);

    inline void addMemoryUnit(MemoryUnit *memoryUnit);

    inline void activatePE(PE *pe) const;

    inline void deactivatePE(PE *pe) const;

    /* === Setters === */

    /* === Getters === */

    inline std::uint32_t getNPE() const;

    inline std::uint32_t getNLRT() const;

    inline std::uint32_t getNMemoryUnit() const;

    inline std::uint32_t getNPEType() const;

    inline std::uint32_t getNActivatedPE() const;

    inline PE *getPEFromSpiderID(std::uint32_t id) const;

    inline PE *getPEFromVirtualID(std::uint32_t id) const;

    inline PE *getPEFromHardwareID(std::uint32_t id) const;

    inline PE *getPEFromName(const std::string &name) const;

    inline PE **getPEArray() const;

    inline MemoryUnit *getMemoryUnit(std::uint32_t id) const;

private:

    std::uint32_t nPE_ = 0;
    std::uint32_t nLRT_ = 0;
    std::uint32_t nMemUnit_ = 0;
    std::uint32_t nPEType_ = 0;
    mutable std::uint32_t nActivatedPE_ = 0;

    /* === Maps between ID === */

    std::map<std::uint32_t, std::uint32_t> virt2SpiderMap_;
    std::map<std::uint32_t, std::uint32_t> hw2SpiderMap_;

    /* === Containers === */

    PE **peArray_ = nullptr;
    MemoryUnit **memoryUnitArray_ = nullptr;
};

void Archi::addPE(PE *pe) {
    if (pe->getSpiderID() > nPE_) {
        throwSpiderException("Index out of bound. PE: %"
                                     PRIu32
                                     " -- Max: %"
                                     PRIu32
                                     ".", pe->getSpiderID(), nPE_);
    }
    peArray_[pe->getSpiderID()] = pe;
    virt2SpiderMap_[pe->getVirtualID()] = pe->getSpiderID();
    hw2SpiderMap_[pe->getHardwareID()] = pe->getSpiderID();
    nLRT_ += pe->isLRT();
}

void Archi::addMemoryUnit(MemoryUnit *memoryUnit) {
    if (memoryUnit->getID() > nMemUnit_) {
        throwSpiderException("Index out of bound. PE: %"
                                     PRIu32
                                     " -- Max: %"
                                     PRIu32
                                     ".", memoryUnit->getID(), nMemUnit_);
    }
    memoryUnitArray_[memoryUnit->getID()] = memoryUnit;
}

void Archi::activatePE(PE *const pe) const {
    pe->enable();
    nActivatedPE_++;
}

void Archi::deactivatePE(PE *const pe) const {
    pe->disable();
    nActivatedPE_--;
}

std::uint32_t Archi::getNPE() const {
    return nPE_;
}

std::uint32_t Archi::getNLRT() const {
    return nLRT_;
}

std::uint32_t Archi::getNMemoryUnit() const {
    return nMemUnit_;
}

std::uint32_t Archi::getNPEType() const {
    return nPEType_;
}

std::uint32_t Archi::getNActivatedPE() const {
    return nActivatedPE_;
}

PE *Archi::getPEFromSpiderID(std::uint32_t id) const {
    return peArray_[id];
}

PE *Archi::getPEFromVirtualID(std::uint32_t id) const {
    return getPEFromSpiderID(virt2SpiderMap_.at(id));
}

PE *Archi::getPEFromHardwareID(std::uint32_t id) const {
    return getPEFromSpiderID(hw2SpiderMap_.at(id));
}

PE *Archi::getPEFromName(const std::string &name) const {
    bool found = false;
    PE *foundPE = nullptr;
    for (std::uint32_t i = 0; i < nPE_ && !found; ++i) {
        foundPE = peArray_[i];
        found |= (name == foundPE->getName());
    }
    if (found) {
        return foundPE;
    }
    return nullptr;
}

PE **Archi::getPEArray() const {
    return peArray_;
}

MemoryUnit *Archi::getMemoryUnit(std::uint32_t id) const {
    return memoryUnitArray_[id];
}

#endif //SPIDER_ARCHI_H
