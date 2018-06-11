/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2013 - 2018) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2018)
 * Clément Guy <clement.guy@insa-rennes.fr> (2014)
 * Florian Arrestier <florian.arrestier@insa-rennes.fr> (2018)
 * Julien Heulot <julien.heulot@insa-rennes.fr> (2013 - 2016)
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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/mman.h>

#include "init.h"

void *Osal_cppiCsEnter(void) {
    return NULL;
}

void Osal_cppiCsExit(void *CsHandle) {
}

void *Osal_cppiMalloc(Uint32 num_bytes) {
    void *dataPtr;

    dataPtr = (void *) data_mem_base;
    data_mem_base += num_bytes;

    return dataPtr;
}

void Osal_cppiFree(void *dataPtr, Uint32 num_bytes) {
}

void Osal_cppiBeginMemAccess(void *ptr, uint32_t size) {
    msync(ptr, size, MS_INVALIDATE);
}

void Osal_cppiEndMemAccess(void *ptr, uint32_t size) {
    msync(ptr, size, MS_SYNC);
}

void *Osal_qmssCsEnter(void) {
    return NULL;
}

void Osal_qmssCsExit(void *CsHandle) {
}

void *Osal_qmssAccCsEnter(void) {
    return NULL;
}

void Osal_qmssAccCsExit(void *CsHandle) {
}

void *Osal_qmssMtCsEnter(void) {
    return NULL;
}

void Osal_qmssMtCsExit(void *CsHandle) {
    return;
}

void *Osal_qmssMalloc(Uint32 num_bytes) {
    void *ptr = malloc(num_bytes);

    /* Allocate memory.  */
    if (ptr) {
        return ptr;
    } else {
        printf("Not enough space in Osal_cppiMalloc\n");
        return ptr;
    }
}

void Osal_qmssFree(void *dataPtr, Uint32 num_bytes) {
    if (dataPtr)
        free(dataPtr);
}

void Osal_qmssBeginMemAccess(void *ptr, uint32_t size) {
    msync(ptr, size, MS_INVALIDATE);
}

void Osal_qmssEndMemAccess(void *ptr, uint32_t size) {
    msync(ptr, size, MS_SYNC);
}

void Osal_DescBeginMemAccess(void *ptr, uint32_t size) {
    msync(ptr, size, MS_INVALIDATE);
}

void Osal_DescEndMemAccess(void *ptr, uint32_t size) {
    msync(ptr, size, MS_SYNC);
}
