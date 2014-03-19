/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,    *
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet             *
 *                                                                          *
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr                  *
 *                                                                          *
 * This software is a computer program whose purpose is to execute          *
 * parallel applications.                                                   *
 *                                                                          *
 * This software is governed by the CeCILL-C license under French law and   *
 * abiding by the rules of distribution of free software.  You can  use,    *
 * modify and/ or redistribute the software under the terms of the CeCILL-C *
 * license as circulated by CEA, CNRS and INRIA at the following URL        *
 * "http://www.cecill.info".                                                *
 *                                                                          *
 * As a counterpart to the access to the source code and  rights to copy,   *
 * modify and redistribute granted by the license, users are provided only  *
 * with a limited warranty  and the software's author,  the holder of the   *
 * economic rights,  and the successive licensors  have only  limited       *
 * liability.                                                               *
 *                                                                          *
 * In this respect, the user's attention is drawn to the risks associated   *
 * with loading,  using,  modifying and/or developing or reproducing the    *
 * software by the user in light of its specific status of free software,   *
 * that may mean  that it is complicated to manipulate,  and  that  also    *
 * therefore means  that it is reserved for developers  and  experienced    *
 * professionals having in-depth computer knowledge. Users are therefore    *
 * encouraged to load and test the software's suitability as regards their  *
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the    *
 * same conditions as regards security.                                     *
 *                                                                          *
 * The fact that you are presently reading this means that you have had     *
 * knowledge of the CeCILL-C license and that you accept its terms.         *
 ****************************************************************************/

#include <zynq_time.h>
//#include <print.h>
#include <types.h>
#include <string.h>
#include <gpio.h>
#include <hwQueues.h>

#include "lrt_definitions.h"
#include "lrt_monitor.h"
#include "lrt_debug.h"

static UINT32 monitorTimes[MaxMonitor];
static MonitorAction currentAction;

void initMonitor(){
	int i;
	for(i=0; i<MaxMonitor; i++) monitorTimes[i]=0;
	OS_TimeInit();
	OS_TimeStop();
	OS_TimeReset();
	initGpio();
}

void resetMonitor(){
	memset(monitorTimes, 0, sizeof(monitorTimes));
	currentAction = Default;
	OS_TimeStop();
	OS_TimeReset();
	OS_TimeStart();
}

MonitorAction switchMonitor(MonitorAction action){
	MonitorAction last = action;
	OS_TimeStop();
	if(action >= MaxMonitor) exitWithCode(1014);
	monitorTimes[currentAction] += OS_TimeGetValue();
	currentAction = action;
	if(currentAction >= Action) setLed(1); else setLed(0);
	OS_TimeReset();
	OS_TimeStart();
	return last;
}

void printResult(){
	int i=0;
	UINT32 totalTime = 0;
	UINT32 val;

	OS_TimeStop();
	monitorTimes[currentAction] += OS_TimeGetValue();

	for(i=0; i<MaxMonitor; i++){
		totalTime += monitorTimes[i];
	}

	for(i=0; i<MaxMonitor; i++){
		val = 100*monitorTimes[i];
		val /= totalTime;

		RTQueuePush_UINT32(RTInfoQueue, val);
	}

//		if(monitorTimes[i] != 0){
//			zynq_puts("\nAction "); zynq_putdec(i-Action);
//			zynq_puts(" : "); 		zynq_putdec(100*monitorTimes[i]/totalTime);
//		}
//	}
//	zynq_puts("\n\n");
}
