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
