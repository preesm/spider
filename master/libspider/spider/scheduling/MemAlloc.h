/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
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
#ifndef MEM_ALLOC_H
#define MEM_ALLOC_H

#include <platform.h>
#include <tools/List.h>
#include <tools/Stack.h>
#include <graphs/SRDAG/SRDAGVertex.h>

#include <algorithm>
#include <spider.h>

class MemAlloc {
public:
    virtual void reset() = 0;

    virtual void alloc(List<SRDAGVertex *> *listOfVertices) = 0;

    virtual int getReservedAlloc(int size) = 0;

    virtual int getMemUsed() = 0;

    MemAlloc(int start, int size) : memStart_(start), memSize_(size), memReserved_(0) {}

    MemAlloc(int start, int size, int reservedSize) : memStart_(start), memSize_(size) {
        setReservedSize(reservedSize);
    }

    inline void setReservedSize(int reservedSize);

    inline int getMemAllocSize() const;

    inline void printMemAllocSizeFormatted() const;

    virtual ~MemAlloc() {}

protected:
    int memStart_;
    int memSize_;
    int memReserved_;
};

inline int MemAlloc::getMemAllocSize() const {
    return memSize_;
}

inline void MemAlloc::printMemAllocSizeFormatted() const {
    const char* units[4] = { "B", "KB", "MB", "GB"};

    float normalizedSize = memSize_;
    int unitIndex = 0;
    while(normalizedSize >= 1024 && unitIndex < 3) {
        normalizedSize /= 1024.;
        unitIndex++;
    }
    fprintf(stderr, "%5.1f %s", normalizedSize, units[unitIndex]);
}

inline void MemAlloc::setReservedSize(int reservedSize) {
    if (reservedSize > memSize_) {
        throw std::runtime_error("Memory allocation for reserved memory superior to total allocated memory.");
    }
    memReserved_ = reservedSize;
}

#endif/*MEM_ALLOC_H*/
