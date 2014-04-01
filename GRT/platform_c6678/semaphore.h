/*
 * semaphore.h
 *
 *  Created on: 1 mars 2014
 *      Author: Julien
 */

#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <ti/csl/tistdtypes.h>

typedef enum {
	SEMAPHORE_BARRIER = 0,
	SEMAPHORE_CIC0_BARRIER = 1,
	SEMAPHORE_CIC1_BARRIER = 2,
	SEMAPHORE_NUMBER = 3
}semaphore;

typedef enum {
	MUTEX_INTC = 0,
	MUTEX_CACHE = 1,
	MUTEX_NUMBER = 2
}mutex;

void sync_init();

void mutex_post(mutex m);
void mutex_pend(mutex m);

void semaphore_init(semaphore s, Uint32 val);
Uint32 semaphore_post(semaphore s);
Uint32 semaphore_posts(semaphore s, Uint32 nb);
Uint32 semaphore_pend(semaphore s);
Uint32 semaphore_pends(semaphore s, Uint32 nb);

#ifdef __cplusplus
}
#endif

#endif /* SEMAPHORE_H_ */
