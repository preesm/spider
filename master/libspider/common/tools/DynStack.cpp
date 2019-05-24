/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018 - 2019)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018 - 2019)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2018)
 * Yaset Oliva <yaset.oliva@insa-rennes.fr> (2013)
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
#include <cmath>
#include <tools/DynStack.h>
#include <SpiderException.h>
#include <platform.h>
#include <Logger.h>

DynStack::DynStack(const char *name) : Stack(name) {
    curUsedSize_ = 0;
    maxSize_ = 0;
    nb_ = 0;
}

DynStack::~DynStack() {
    printStat();
}

void *DynStack::alloc(int size, bool pageAligned) {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    int alignedSize = size + sizeof(std::int64_t);

    if (pageAligned) {
        alignedSize = Stack::getAlignedSize(alignedSize);
    }
    curUsedSize_ += alignedSize;
    maxSize_ = std::max(maxSize_, curUsedSize_);
    nb_++;

    void *address = operator new((size_t) alignedSize);
    if (!address) {
        throwSpiderException("failed to allocate %d bytes", alignedSize);
    }
    auto *sizeAddress = (std::int64_t *) address;
    auto *dataAddress = (void *) (sizeAddress + 1);
    *sizeAddress = alignedSize;

    return dataAddress;
}

void DynStack::freeAll() {
    if (nb_ != 0) {
        Logger::print(LOG_GENERAL, LOG_WARNING, "DynStack [%s], FreeAll called with %d remaining allocated item(s).\n",
                      getName(),
                      nb_);
    }
}

void DynStack::free(void *var) {
    std::lock_guard<std::mutex> lock(memoryMutex_);
    void *dataAddress = var;
    auto *address = (void *) (((std::int64_t *) dataAddress) - 1);
    std::int64_t size = *((std::int64_t *) address);

    maxSize_ = std::max(maxSize_, curUsedSize_);
    curUsedSize_ -= size;
    operator delete(address);
    nb_--;
}

void DynStack::printStat() {

    const char *units[4] = {"B", "KB", "MB", "GB"};

    float normalizedSize = maxSize_;
    int unitIndex = 0;
    while (normalizedSize >= 1024 && unitIndex < 3) {
        normalizedSize /= 1024.;
        unitIndex++;
    }
    Logger::print(LOG_GENERAL, LOG_INFO, "[%s]\t usage: %5.1f %s\n", getName(), normalizedSize, units[unitIndex]);

    if (nb_) {
        Logger::print(LOG_GENERAL, LOG_WARNING, "[%s]: \t%lld B still in use\n", getName(), curUsedSize_);
    }
}
