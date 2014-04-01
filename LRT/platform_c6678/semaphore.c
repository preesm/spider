/*
 * semaphore.c
 *
 *  Created on: 1 mars 2014
 *      Author: Julien
 */

#include "semaphore.h"
#include <ti/platform/platform.h>
#include <ti/csl/csl_semAux.h>
#include <ti/csl/csl_chipAux.h>
#include <stdio.h>

//#include "cache.h"

#pragma DATA_SECTION(semaphore_count, ".shared")
#pragma DATA_ALIGN(semaphore_count, 16)
Uint32 semaphore_count[SEMAPHORE_NUMBER];

void mutex_post(mutex m){
	CSL_semReleaseSemaphore(m);
}

void mutex_pend(mutex m){
	Uint8 get, owner;
	do{
		get = CSL_semAcquireDirect(m);
		owner = CSL_semGetResourceOwner(m);
		if(get && owner != CSL_chipReadDNUM()){
			printf("fail semaphore me(%d) own(%d)\n", CSL_chipReadDNUM(), owner);
		}
	}while((!get)  || (owner != CSL_chipReadDNUM()));
}

void semaphore_init(semaphore s, Uint32 val){
	semaphore_count[s] = val;
}

Uint32 semaphore_post(semaphore s){
	return semaphore_posts(s, 1);
}

Uint32 semaphore_posts(semaphore s, Uint32 nb){
	Uint32 val;
	mutex m = (mutex)(s+(semaphore)MUTEX_NUMBER);
	mutex_pend(m);
//	cache_inv(&semaphore_count, sizeof(semaphore_count));

	semaphore_count[s] += nb;
	val = semaphore_count[s];

//	cache_wb(&semaphore_count, sizeof(semaphore_count));
	mutex_post(m);

	return val;
}

Uint32 semaphore_pend(semaphore s){
	return semaphore_pends(s, 1);
}

Uint32 semaphore_pends(semaphore s, Uint32 nb){
	Uint32 val;
	mutex m = (mutex)(s+(semaphore)MUTEX_NUMBER);
	do{
//		cache_inv(&semaphore_count, sizeof(semaphore_count));
	}
	while(semaphore_count[s] < nb);

	mutex_pend(m);
//	cache_inv(&semaphore_count, sizeof(semaphore_count));
	semaphore_count[s] -= nb;
	val = semaphore_count[s];
//	cache_wb(&semaphore_count, sizeof(semaphore_count));
	mutex_post(m);

	return val;
}

