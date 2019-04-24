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

    /**
     * @brief Add a PE to the architecture (does not change @refitem nPE_).
     * @param pe Pointer to the PE to add.
     */
    inline void addPE(PE *pe);

    /**
     * @brief Add a MemoryUnit to the architecture (does not change @refitem nMemoryUnit_).
     * @param memoryUnit Pointer to the MemoryUnit to add.
     */
    inline void addMemoryUnit(MemoryUnit *memoryUnit);

    /**
     * @brief Activate a PE.
     * @param pe Pointer to the PE to be activate.
     */
    inline void activatePE(PE *pe) const;

    /**
     * @brief Deactivate a PE.
     * @param pe Pointer to the PE to be deactivated.
     */
    inline void deactivatePE(PE *pe) const;

    /* === Setters === */

    /**
     * @brief Set the ID of the GRT PE.
     * @remark The method convert the virtual id to spider id for internal use.
     * @param id Virtual ID of the GRT PE (i.e S-LAM user ID)
     */
    inline void setSpiderGRTID(std::uint32_t id);

    /* === Getters === */

    /**
     * @brief Get the total number of PE (activated + deactivated).
     * @return total number of PE
     */
    inline std::uint32_t getNPE() const;

    /**
     * @brief Get the total number of LRT (not necessarly equals to @refitem getNPE()).
     * @return number of LRT
     */
    inline std::uint32_t getNLRT() const;

    /**
     * @brief Get the number of memory unit of the architecture.
     * @return
     */
    inline std::uint32_t getNMemoryUnit() const;

    /**
     * @brief Get the number of different PE type (hardware types).
     * @return number of PE types.
     */
    inline std::uint32_t getNPEType() const;

    /**
     * @brief Retrieve the number of PE(s) for a given HardwareType (@refitem PE::getHardwareType()).
     * @param HWType Hardware type.
     * @return number of PE of HardwareType HWType
     */
    inline std::uint32_t getNPEForHWType(std::uint32_t HWType) const;

    /**
     * @brief Get the number of currently activated PEs (inferior or equal to @refitem nPE_).
     * @return number of activated PEs.
     */
    inline std::uint32_t getNActivatedPE() const;

    /**
     * @brief Retrieve a PE from its Spider ID (fastest method).
     * @remark No boundaries check is perform for the id.
     * @param id Spider id of the PE to retrieve.
     * @return PE corresponding to the spider id.
     */
    inline PE *getPEFromSpiderID(std::uint32_t id) const;

    /**
     * @brief Retrieve a PE from its virtual ID.
     * @param id  Virtual ID of the PE (i.e S-LAM user id).
     * @return PE corresponding to the virtual id
     * @throws out of bound exception if virtual id does not exist
     */
    inline PE *getPEFromVirtualID(std::uint32_t id) const;

    /**
     * @brief Retrieve a PE from its hardware ID.
     * @remark This method may not be reliable in heterogeneous architecture with multiple hardware core 0 for instance.
     * @param id  Hardware ID of the PE (i.e S-LAM user id).
     * @return PE corresponding to the hardware id
     * @throws out of bound exception if hardware id does not exist
     */
    inline PE *getPEFromHardwareID(std::uint32_t id) const;

    /**
     * @brief Retrieve first PE matching with given name.
     * @param name  Name of the PE to find.
     * @return first PE with name matching given name, nullptr else.
     */
    inline PE *getPEFromName(const std::string &name) const;

    /**
     * @brief Retrieve PE array containing all PEs.
     * @remark PE array is indexed by spider id, not virtual id.
     * @return PE array.
     */
    inline PE **getPEArray() const;

    /**
     * @brief Retrieve a MemoryUnit from its id.
     * @remark MemoryUnit ids are generated in order of creation.
     * @param id ID of the memory unit to retrieve.
     * @return MemoryUnit of corresponding id.
     */
    inline MemoryUnit *getMemoryUnit(std::uint32_t id) const;

    /**
     * @brief Get spider GRT spider id.
     * @remark The id returned by this method is NOT the same as the one passed in @refitem setSpiderGRTID.
     *         It corresponds to the translated virtual -> spider id.
     * @return spider id of the GRT.
     */
    inline std::uint32_t getSpiderGRTID() const;

    /**
     * @brief Return the routine that computes the predicted time needed for scheduling.
     * @return ScheduleTimeRoutine function.
     */
    inline ScheduleTimeRoutine getScheduleTimeRoutine() const;

private:

    /* === Members === */

    std::uint32_t nPE_ = 0;
    std::uint32_t nLRT_ = 0;
    std::uint32_t nMemUnit_ = 0;
    std::uint32_t nPEType_ = 0;
    mutable std::uint32_t nActivatedPE_ = 0;
    ScheduleTimeRoutine scheduleTimeRoutine_;
    std::uint32_t spiderGRTID_ = 0;

    /* === Maps between ID === */

    std::map<std::uint32_t, std::uint32_t> virt2SpiderMap_;
    std::map<std::uint32_t, std::uint32_t> hw2SpiderMap_;

    /* === Containers === */

    PE **peArray_ = nullptr;
    MemoryUnit **memoryUnitArray_ = nullptr;
    std::uint32_t *nPEsPerPETypeArray_ = nullptr;
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
    nPEsPerPETypeArray_[pe->getHardwareType()]++;
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

void Archi::setSpiderGRTID(std::uint32_t id) {
    spiderGRTID_ = virt2SpiderMap_[id];
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

std::uint32_t Archi::getNPEForHWType(std::uint32_t HWType) const {
    return nPEsPerPETypeArray_[HWType];
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

std::uint32_t Archi::getSpiderGRTID() const {
    return spiderGRTID_;
}

ScheduleTimeRoutine Archi::getScheduleTimeRoutine() const {
    return scheduleTimeRoutine_;
}



#endif //SPIDER_ARCHI_H
