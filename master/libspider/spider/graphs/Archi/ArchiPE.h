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
#ifndef SPIDER_ARCHIPE_H
#define SPIDER_ARCHIPE_H

#include <cstdint>
#include <string>

enum class SpiderPEType {
    LRT_ONLY, /*! PE is used as an LRT and does not perform any computation */
    PE_ONLY,  /*! PE is used for computation only and does not perform any job management */
    LRT_PE,   /*! PE is used as an LRT and can be used for computation */
};

enum class SpiderHWType {
    PHYS_PE,  /*! PE is instantiated in Spider and run on a core (SpiderPEType::LRT_*) */
    VIRT_PE,  /*! PE is instantiated in Spider but fully managed by an LRT (SpiderPEType::PE_ONLY) */
};


class PE {
public:

    PE(std::uint32_t hwType,
       std::uint32_t hwID,
       std::uint32_t virtID,
       std::string name = "unnamed-PE",
       SpiderPEType spiderPEType = SpiderPEType::LRT_PE,
       SpiderHWType spiderHWType = SpiderHWType::PHYS_PE);

    ~PE() = default;

    /* Setter */

    inline void setHardwareType(std::uint32_t type);

    inline void setHardwareID(std::uint32_t id);

    inline void setVirtualID(std::uint32_t id);

    inline void setName(std::string &name);

    inline void setSpiderPEType(SpiderPEType type);

    inline void setSpiderHardwareType(SpiderHWType type);

    inline void disable();

    inline void enable();

    /* Getters */

    inline std::uint32_t getHardwareType() const;

    inline std::uint32_t getHardwareID() const;

    inline std::uint32_t getVirtualID() const;

    inline std::string getName() const;

    inline SpiderPEType getSpiderPEType() const;

    inline SpiderHWType getSpiderHardwareType() const;

    inline std::uint32_t getSpiderID() const;

    inline bool isEnabled() const;

private:
    static std::uint32_t globalID;
    /* Core properties */

    std::uint32_t hwType_ = 0;  /*! S-LAM user hardware type */
    std::uint32_t hwID_ = 0;    /*! Hardware on which PE runs (core id) */
    std::uint32_t virtID_ = 0;  /*! S-LAM user id */
    std::string name_;          /*! S-LAM user name of the PE */

    /* Spider properties */

    SpiderPEType spiderPEType_ = SpiderPEType::LRT_PE;
    SpiderHWType spiderHWType_ = SpiderHWType::PHYS_PE;
    std::uint32_t spiderID_ = 0; /*! Spider id (used internally by spider) */
    bool enabled_ = true;
};

std::uint32_t PE::globalID = 0;


PE::PE(std::uint32_t hwType,
       std::uint32_t hwID,
       std::uint32_t virtID,
       std::string name,
       SpiderPEType spiderPEType,
       SpiderHWType spiderHWType) : hwType_{hwType},
                                    hwID_{hwID},
                                    virtID_{virtID},
                                    name_{std::move(name)},
                                    spiderPEType_{spiderPEType},
                                    spiderHWType_{spiderHWType} {
    spiderID_ = PE::globalID++;
}


void PE::setHardwareType(std::uint32_t type) {
    hwType_ = type;
}

void PE::setHardwareID(std::uint32_t id) {
    hwID_ = id;
}

void PE::setVirtualID(std::uint32_t id) {
    virtID_ = id;
}

void PE::setName(std::string &name) {
    name_ = std::string(name);
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

#endif //SPIDER_ARCHIPE_H
