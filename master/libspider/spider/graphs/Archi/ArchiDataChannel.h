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
#ifndef SPIDER_ARCHIDATACHANNEL_H
#define SPIDER_ARCHIDATACHANNEL_H


#include <cstdint>

using writeRoutineType = bool (*)(void *);
using readRoutineType = bool (*)(void *);

enum class ChannelRWType {
    READ_ONLY,  /*! Owner can read only on this channel */
    WRITE_ONLY, /*! Owner can write only on this channel */
    READ_WRITE, /*! Owner can read and write on this channel */
};

inline bool defaultWriteFn(void *) {
    return true;
}

inline bool defaultReadFn(void *) {
    return true;
}

class DataChannel {
public:
    DataChannel(ChannelRWType rwType = ChannelRWType::READ_WRITE,
                writeRoutineType writeFn = defaultWriteFn,
                readRoutineType readFn = defaultReadFn);

    inline bool write(void *data) const;

    inline bool read(void *data) const;

    /* Setters */

    inline void setWriteRoutine(writeRoutineType writeFn);

    inline void setReadRoutine(readRoutineType readFn);

private:
    /* Type of the channel */
    ChannelRWType rwType_ = ChannelRWType::READ_WRITE;

    /* Communication routines */

    writeRoutineType writeFn_; /*! User specified write routine on this channel */
    readRoutineType readFn_;   /*! User specified read routine on this channel */

    /* Owner / user */

    std::uint32_t owner_; /*! Spider id of the owner of the channel */
    std::uint32_t user_;  /*! Spider id of the user of the channel */
};

bool DataChannel::write(void *data) const {
    return writeFn_(data);
}

bool DataChannel::read(void *data) const {
    return readFn_(data);
}

void DataChannel::setWriteRoutine(writeRoutineType writeFn) {
    writeFn_ = writeFn;
}

void DataChannel::setReadRoutine(readRoutineType readFn) {
    readFn_ = readFn;
}

#endif //SPIDER_ARCHIDATACHANNEL_H
