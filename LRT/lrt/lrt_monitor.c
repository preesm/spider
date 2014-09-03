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

#include <stdlib.h>
#include <platform_types.h>
#include <platform_gpio.h>
#include <platform_queue.h>
#include <platform_time.h>

#include "lrt_definitions.h"
#include "lrt_monitor.h"
#include "lrt_debug.h"

typedef struct{
	UINT32 srdagIx;
	UINT32 globalIx;
	UINT32 type;
	UINT32 pisdfVertex;
	UINT32 iter;
	UINT32 repet;

	UINT32 start;
	UINT32 end;
} taskTime;

static taskTime taskTimes[OS_MAX_TASKS_TIME];
static UINT32 nbTaskTime;

void Monitor_init(){
	nbTaskTime = 0;
}

void Monitor_startTask(){
	if(nbTaskTime>=OS_MAX_TASKS_TIME-1){
		exitWithCode(1017);
	}
	taskTimes[nbTaskTime].srdagIx		= OSTCBCur->srdagId;
	taskTimes[nbTaskTime].globalIx		= OSTCBCur->globalIx;
	taskTimes[nbTaskTime].type			= OSTCBCur->type;
	taskTimes[nbTaskTime].pisdfVertex	= OSTCBCur->pisdfVertex;
	taskTimes[nbTaskTime].iter			= OSTCBCur->iterIx;
	taskTimes[nbTaskTime].repet			= OSTCBCur->refIx;

	taskTimes[nbTaskTime].start = platform_time_getValue();
}

void Monitor_endTask(){
	taskTimes[nbTaskTime].end = platform_time_getValue();
	nbTaskTime++;
}

void Monitor_sendData(){
	platform_queue_push_UINT32(MSG_EXEC_TIMES);
	platform_queue_push_UINT32(nbTaskTime);
	platform_queue_push(taskTimes, nbTaskTime*sizeof(taskTime));
	platform_queue_push_finalize();
//	printf("nbTaskTime %d \n", nbTaskTime);
	nbTaskTime = 0;
//	exit(0);
}

