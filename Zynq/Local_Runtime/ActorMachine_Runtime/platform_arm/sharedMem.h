/*
 * sharedMem.h
 *
 *  Created on: Jun 12, 2013
 *      Author: jheulot
 */

#ifndef SHAREDMEM_H_
#define SHAREDMEM_H_

#include <stdio.h>

#include "types.h"
#include <fcntl.h>

void OS_ShMemInit();

UINT32 OS_ShMemRead(UINT32 address, void* data, UINT32 size);
UINT32 OS_ShMemWrite(UINT32 address, void* data, UINT32 size);

#endif /* SHAREDMEM_H_ */
