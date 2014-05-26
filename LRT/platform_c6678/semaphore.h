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
	MUTEX_CACHE = 0
}mutex;

void sync_init();

void mutex_post(mutex m);
void mutex_pend(mutex m);

#ifdef __cplusplus
}
#endif

#endif /* SEMAPHORE_H_ */
