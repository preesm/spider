/**
 * Copyright or © or Copr. IETR/INSA - Rennes (2019) :
 *
 * Antoine Morvan <antoine.morvan@insa-rennes.fr> (2019)
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
/**
 * 
 * Custom semaphore wrapper:
 * uses POSIX Semaphores on Windows / Linux
 * uses dispatch Semaphores on MacOSX
 * 
 * Strongly inspired from https://stackoverflow.com/questions/27736618/why-are-sem-init-sem-getvalue-sem-destroy-deprecated-on-mac-os-x-and-w
 * 
 * Note: dispatch is not async safe: https://github.com/adrienverge/openfortivpn/issues/105
 * 
 **/

#ifndef _SPIDER_SEMAPHORES_H
#define _SPIDER_SEMAPHORES_H

#ifdef __APPLE__
// MacOSX does not implement semaphores
// use dispatch instead

#include <dispatch/dispatch.h>

typedef struct spider_sem_t {
    dispatch_semaphore_t    sem;
} spider_sem;

static inline void spider_sem_init(spider_sem *s, unsigned int maxvalue) {
    dispatch_semaphore_t *sem = &(s->sem);
    *sem = dispatch_semaphore_create(maxvalue);
}

static inline long spider_sem_wait(spider_sem *s) {
    return dispatch_semaphore_wait(s->sem, DISPATCH_TIME_FOREVER);
}

static inline long spider_sem_trywait(spider_sem *s) {
    return dispatch_semaphore_wait(s->sem, DISPATCH_TIME_NOW);
}

static inline void spider_sem_post(spider_sem *s) {
    dispatch_semaphore_signal(s->sem);
}

static inline void spider_sem_destroy(spider_sem *s) {
    dispatch_release(s->sem);
}

#else
// windows / linux
// use POSIX semaphores
#include <semaphore.h>

// semaphore.h includes _ptw32.h that redefines types int64_t and uint64_t on Visual Studio,
// making compilation error with the IDE's own declaration of said types
#ifdef _MSC_VER
#ifdef int64_t
#undef int64_t
#endif

#ifdef uint64_t
#undef uint64_t
#endif
#endif

typedef struct spider_sem_t {
    sem_t                   sem;
} spider_sem;

static inline void spider_sem_init(spider_sem *s, unsigned int maxvalue) {
    sem_init(&s->sem, 0, maxvalue);
}

static inline long spider_sem_wait(spider_sem *s) {
    return (long) sem_wait(&s->sem);
}

static inline long spider_sem_trywait(spider_sem *s) {
    return (long) sem_trywait(&s->sem);
}

static inline void spider_sem_post(spider_sem *s) {
    sem_post(&s->sem);
}

static inline void spider_sem_destroy(spider_sem *s) {
    sem_destroy(&s->sem);
}

// end linux/windows
#endif


#endif
