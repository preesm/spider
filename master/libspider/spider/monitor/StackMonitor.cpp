/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2017) :
 *
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Hugo Miomandre <hugo.miomandre@insa-rennes.fr> (2017)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2015)
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
#include "StackMonitor.h"

#include <platform.h>


void StackMonitor::initStack(SpiderStack stackId, StackConfig cfg) {
    switch (cfg.type) {
        case STACK_DYNAMIC:
            Platform::get()->setStack(stackId, new DynStack(cfg.name));
            break;
        case STACK_STATIC:
            Platform::get()->setStack(stackId, new StaticStack(cfg.name, cfg.start, cfg.size));
            break;
    }
}

void StackMonitor::clean(SpiderStack id) {
    delete Platform::get()->getStack(id);
}

void StackMonitor::cleanAllStack() {
    for (int i = 0; i < STACK_COUNT; i++) {
        delete Platform::get()->getStack(i);
    }
}

void *StackMonitor::alloc(SpiderStack stackId, int size) {
    return Platform::get()->getStack(stackId)->alloc(size);
}

void StackMonitor::free(SpiderStack stackId, void *ptr) {
    return Platform::get()->getStack(stackId)->free(ptr);
}

void StackMonitor::freeAll(SpiderStack stackId) {
    return Platform::get()->getStack(stackId)->freeAll();
}

void StackMonitor::printStackStats() {
    // TODO
}

