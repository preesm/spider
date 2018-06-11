/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
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
#ifndef SHARED_MEM_ARCH_H
#define SHARED_MEM_ARCH_H

#include "Archi.h"
#include <monitor/StackMonitor.h>
#include <cstring>

class SharedMemArchi : public Archi {
public:
    SharedMemArchi(int nPE, int nPEType, int spiderPe, MappingTimeFct mapFct);

    virtual ~SharedMemArchi();

    virtual int getNPE() const;

    virtual inline const char *getPEName(int ix) const;

    virtual inline int getNPETypes() const;

    virtual inline int getPEType(int ix) const;

    virtual inline void desactivatePE(int pe);

    virtual inline void activatePE(int pe);

    virtual inline bool isActivated(int pe) const;

    virtual inline Time getTimeSend(int src, int dest, int size) const;

    virtual inline Time getTimeRecv(int src, int dest, int size) const;

    virtual inline int getSpiderPeIx() const;

    virtual inline MappingTimeFct getMappingTimeFct() const;

    inline void setPETypeSendSpeed(int type, float a, float b);

    inline void setPETypeRecvSpeed(int type, float a, float b);

    inline void setName(int pe, const char *name);

    inline void setPEType(int pe, int type);

    virtual inline int getNPEforType(int type);

private:
    int nPE_;
    int spiderPe_;
    int nPEType_;
    int *peType_;
    bool *peActive_;
    char **peName_;
    float *peTypeASend_;
    float *peTypeBSend_;
    float *peTypeARecv_;
    float *peTypeBRecv_;
    MappingTimeFct mapFct_;

    int *nPEperType_;
};

inline const char *SharedMemArchi::getPEName(int ix) const {
    return peName_[ix];
}

inline int SharedMemArchi::getNPETypes() const {
    return nPEType_;
}

inline int SharedMemArchi::getPEType(int ix) const {
    return peType_[ix];
}

inline Time SharedMemArchi::getTimeSend(int src, int dest, int size) const {
    return peTypeASend_[peType_[src]] * size + peTypeASend_[peType_[src]];
}

inline Time SharedMemArchi::getTimeRecv(int src, int dest, int size) const {
    return peTypeARecv_[peType_[dest]] * size + peTypeARecv_[peType_[dest]];
}

inline void SharedMemArchi::setPETypeSendSpeed(int type, float a, float b) {
    peTypeASend_[type] = a;
    peTypeBSend_[type] = b;
}

inline void SharedMemArchi::setPETypeRecvSpeed(int type, float a, float b) {
    peTypeARecv_[type] = a;
    peTypeBRecv_[type] = b;
}

inline void SharedMemArchi::setName(int pe, const char *name) {
    int size = strlen(name) + 1;
    char *newName = CREATE_MUL(ARCHI_STACK, size, char);
    peName_[pe] = newName;
    strcpy(newName, name);
}

inline void SharedMemArchi::setPEType(int pe, int type) {
    peType_[pe] = type;

    nPEperType_[type]++;
}


inline void SharedMemArchi::desactivatePE(int pe) {
    if (peActive_[pe]) {
        peActive_[pe] = false;
    }
}

inline void SharedMemArchi::activatePE(int pe) {
    if (!peActive_[pe]) {
        peActive_[pe] = true;
    }
}

inline bool SharedMemArchi::isActivated(int pe) const {
    return peActive_[pe];
}

inline int SharedMemArchi::getSpiderPeIx() const {
    return spiderPe_;
}

inline MappingTimeFct SharedMemArchi::getMappingTimeFct() const {
    return mapFct_;
}

inline int SharedMemArchi::getNPEforType(int type) {
    return nPEperType_[type];
}

#endif/*SHARED_MEM_ARCH_H*/
