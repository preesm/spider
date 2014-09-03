/*
 * semaphore.c
 *
 *  Created on: 1 mars 2014
 *      Author: Julien
 */

#include "semaphore.h"

#include <stdint.h>
#include <ti/csl/device/k2h/src/cslr_device.h>

#include <ti/platform/platform.h>
#include <ti/csl/csl_semAux.h>
#include <ti/csl/csl_chipAux.h>
#include <stdio.h>


void mutex_post(mutex m){
	CSL_semReleaseSemaphore(m);
}

void mutex_pend(mutex m){
	Uint8 get;
	do{
		get = CSL_semAcquireDirect(m);
	}while(!get);
}

