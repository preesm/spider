/*
 * time.h
 *
 *  Created on: Aug 28, 2013
 *      Author: jheulot
 */

#ifndef TIME_H_
#define TIME_H_

#include "types.h"
#include <lrt_definitions.h>

void OS_TimeInit();

void OS_TimeReset();
void OS_TimeStart();
void OS_TimeStop();

UINT32 OS_TimeGetValue();
void lrtGetTime(LRTSYSTime *buffer);

#endif /* TIME_H_ */
