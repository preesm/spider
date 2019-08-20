/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2019)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2019)
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
#ifndef SPIDER_ARCHIPE_H
#define SPIDER_ARCHIPE_H

#include <cstdint>
#include <string>
#include <spider-api/user/archi.h>
#include "ArchiMemUnit.h"

class PE {
public:
    PE(std::uint32_t hwType,
       std::uint32_t hwID,
       std::uint32_t virtID,
       std::string name = "unnamed-PE",
       SpiderPEType spiderPEType = SpiderPEType::LRT_PE,
       SpiderHWType spiderHWType = SpiderHWType::PHYS_PE);

    ~PE();

    /* === Setters === */

    inline void setHardwareType(std::uint32_t type);

    inline void setHardwareID(std::uint32_t id);

    inline void setVirtualID(std::uint32_t id);

    inline void setName(std::string name);

    inline void setMemoryUnit(MemoryUnit *memoryUnit);

    inline void setPower(double pePower);

    inline void setSpiderPEType(SpiderPEType type);

    inline void setSpiderHardwareType(SpiderHWType type);

    inline void disable();

    inline void enable();

    inline void setReadCostRoutine(std::uint32_t distPESpiderID, CommunicationCostRoutine routine);

    inline void setSendCostRoutine(std::uint32_t distPESpiderID, CommunicationCostRoutine routine);

    /* === Getters === */

    inline std::uint32_t getHardwareType() const;

    inline std::uint32_t getHardwareID() const;

    inline std::uint32_t getVirtualID() const;

    inline std::string getName() const;

    inline MemoryUnit *getMemoryUnit() const;

    inline double getPower() const;

    inline SpiderPEType getSpiderPEType() const;

    inline SpiderHWType getSpiderHardwareType() const;

    inline std::uint32_t getSpiderID() const;

    inline bool isEnabled() const;

    inline bool isLRT() const;

    inline CommunicationCostRoutine getReadCostRoutine(std::uint32_t distPESpiderID) const;

    inline CommunicationCostRoutine getSendCostRoutine(std::uint32_t distPESpiderID) const;

private:
    static std::uint32_t globalID;

    /* === Core properties === */

    std::uint32_t hwType_ = 0;         /*! S-LAM user hardware type */
    std::uint32_t hwID_ = 0;           /*! Hardware on which PE runs (core id) */
    std::uint32_t virtID_ = 0;         /*! S-LAM user id */
    std::string name_;                 /*! S-LAM user name of the PE */
    MemoryUnit *memoryUnit_ = nullptr; /*! Memory unit attached to this PE */
    double pePower_ = 0.0;             /*! Power increment when turning on this PE */

    /* === Spider properties === */

    SpiderPEType spiderPEType_ = SpiderPEType::LRT_PE;
    SpiderHWType spiderHWType_ = SpiderHWType::PHYS_PE;
    std::uint32_t spiderID_ = 0; /*! Spider id (used internally by spider) */
    bool enabled_ = false;

    /* === read / send CommunicationCostRoutines === */

    CommunicationCostRoutine *readCostRoutineArray_;
    CommunicationCostRoutine *sendCostRoutineArray_;
};

void PE::setHardwareType(std::uint32_t type) {
    hwType_ = type;
}

void PE::setHardwareID(std::uint32_t id) {
    hwID_ = id;
}

void PE::setVirtualID(std::uint32_t id) {
    virtID_ = id;
}

void PE::setName(std::string name) {
    name_ = std::move(name);
}

void PE::setMemoryUnit(MemoryUnit *memoryUnit) {
    memoryUnit_ = memoryUnit;
}

void PE::setPower(double pePower) {
    pePower_ = pePower;
}

void PE::setSpiderPEType(SpiderPEType type) {
    spiderPEType_ = type;
}

void PE::setSpiderHardwareType(SpiderHWType type) {
    spiderHWType_ = type;
}

void PE::disable() {
    enabled_ = false;
}

void PE::enable() {
    enabled_ = true;
}

void PE::setReadCostRoutine(std::uint32_t distPESpiderID, CommunicationCostRoutine routine) {
    readCostRoutineArray_[distPESpiderID] = routine;
}

void PE::setSendCostRoutine(std::uint32_t distPESpiderID, CommunicationCostRoutine routine) {
    sendCostRoutineArray_[distPESpiderID] = routine;
}

std::uint32_t PE::getHardwareType() const {
    return hwType_;
}

std::uint32_t PE::getHardwareID() const {
    return hwID_;
}

std::uint32_t PE::getVirtualID() const {
    return virtID_;
}

std::string PE::getName() const {
    return name_;
}

MemoryUnit *PE::getMemoryUnit() const {
    return memoryUnit_;
}

double PE::getPower() const {
    return pePower_;
}

SpiderPEType PE::getSpiderPEType() const {
    return spiderPEType_;
}

SpiderHWType PE::getSpiderHardwareType() const {
    return spiderHWType_;
}

std::uint32_t PE::getSpiderID() const {
    return spiderID_;
}

bool PE::isEnabled() const {
    return enabled_;
}

bool PE::isLRT() const {
    return spiderPEType_ != SpiderPEType::PE_ONLY;
}

CommunicationCostRoutine PE::getReadCostRoutine(std::uint32_t distPESpiderID) const {
    return readCostRoutineArray_[distPESpiderID];
}

CommunicationCostRoutine PE::getSendCostRoutine(std::uint32_t distPESpiderID) const {
    return sendCostRoutineArray_[distPESpiderID];
}


#endif //SPIDER_ARCHIPE_H
