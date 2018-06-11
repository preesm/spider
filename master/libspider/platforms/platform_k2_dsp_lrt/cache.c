/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2014 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2014 - 2016)
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
#include "cache.h"
#include <ti/csl/csl_xmcAux.h>
#include <ti/csl/src/intc/csl_intc.h>
#include <ti/csl/src/intc/csl_intcAux.h>

#define cacheOperationCode(buffer, size, call)        \
                                                    \
    /* Disable Interrupts */                        \
    Uint32 intStatus = _disable_interrupts();        \
                                                    \
    /*  Cleanup the prefetch buffer also.*/        \
    CSL_XMC_invalidatePrefetchBuffer();            \
                                                    \
    /* Invalidate the cache. */                    \
    call(buffer, size, CACHE_FENCE_WAIT);            \
    asm (" MFENCE ");                               \
    asm (" NOP  4");                                \
    asm (" NOP  4");                                \
    asm (" NOP  4");                                \
    asm (" NOP  4");                                \
                                                    \
    /* Reenable Interrupts.*/                        \
    _restore_interrupts(intStatus);                 \


void cache_wbInvL1D(void *buffer, Uint32 size) {
    cacheOperationCode(buffer, size, CACHE_wbInvL1d);
}

void cache_wbL1D(void *buffer, Uint32 size) {
    cacheOperationCode(buffer, size, CACHE_wbL1d);
}

void cache_invL1D(void *buffer, Uint32 size) {
    cacheOperationCode(buffer, size, CACHE_invL1d);
}

void cache_wbInvL2(void *buffer, Uint32 size) {
    cacheOperationCode(buffer, size, CACHE_wbInvL2);
}

void cache_wbL2(void *buffer, Uint32 size) {
    cacheOperationCode(buffer, size, CACHE_wbL2);
}

void cache_invL2(void *buffer, Uint32 size) {
    cacheOperationCode(buffer, size, CACHE_invL2);
}


