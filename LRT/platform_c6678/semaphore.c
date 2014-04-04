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

