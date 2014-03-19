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

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "types.h"
#include "hwQueues.h"
#include <grt_definitions.h>

#define NB_MAX_QUEUES		60
#define MAILSLOT_BASE_NAME 	"\\\\.\\mailslot\\"
#define BUFFER_SIZE		512
#define GRT_ACK_WORD	0xffffffff

static HANDLE RTQueue[MAX_SLAVES][nbQueueTypes][2];

void RTQueuesInit(UINT8 nbSlaves){
	UINT8 i;

	for(i=0; i<nbSlaves; i++){
		char tempStr[50];

//		// Creating output pipes.
//		sprintf(tempStr, "%sCtrl_Grtto%d", MAILSLOT_BASE_NAME, i);
//		RTQueue[i][RTCtrlQueue][RTOutputQueue] = CreateMailslot(tempStr,
//	        0,                             // no maximum message size
//	        MAILSLOT_WAIT_FOREVER,         // no time-out for operations
//	        (LPSECURITY_ATTRIBUTES) NULL); // default security
//
//	    if (RTQueue[i][RTCtrlQueue][RTOutputQueue] == INVALID_HANDLE_VALUE)
//	    {
//	        printf("CreateMailslot failed with %d\n", GetLastError());
//	        exit(EXIT_FAILURE);
//	    }

		// Creating input pipes.
		sprintf(tempStr, "%sCtrl_%dtoGrt", MAILSLOT_BASE_NAME, i);
		RTQueue[i][RTCtrlQueue][RTInputQueue] = CreateMailslot(tempStr,
	        0,                             // no maximum message size
	        MAILSLOT_WAIT_FOREVER,         // no time-out for operations
	        (LPSECURITY_ATTRIBUTES) NULL); // default security

	    if (RTQueue[i][RTCtrlQueue][RTInputQueue] == INVALID_HANDLE_VALUE)
	    {
	        printf("CreateMailslot failed with %d\n", GetLastError());
	        exit(EXIT_FAILURE);
	    }
	}
}


UINT32 RTQueuePush(UINT8 slaveId, RTQueueType queueType, void* data, int size){
    BOOL fSuccess = FALSE;
    int nb_bytes_written;

	fSuccess = GetMailslotInfo(
			RTQueue[slaveId][queueType][RTOutputQueue],  // mailslot handle
        (LPDWORD) NULL,               // no maximum message size
        (LPDWORD) NULL,                   // size of next message
        (LPDWORD) NULL,                    // number of messages
        (LPDWORD) NULL);              // no read time-out

	if(!fSuccess){
    	printf("GetMailslotInfo failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}

    fSuccess = WriteFile(
    				RTQueue[slaveId][queueType][RTOutputQueue],	// pipe handle
    				data,		             	// message
    				size,		             	// message length
    				(PDWORD)&nb_bytes_written,	// bytes written
    				NULL);                 		// not overlapped

    if (!fSuccess){
    	printf("WriteFile failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
    }

	return nb_bytes_written;
}


UINT32 RTQueuePush_UINT32(UINT8 slaveId, RTQueueType queueType, UINT32 data){
	return RTQueuePush(slaveId, queueType, &data, sizeof(UINT32));
}


UINT32 RTQueuePop(UINT8 slaveId, RTQueueType queueType, void* data, int size){
	BOOL fSuccess = FALSE;
	int nb_bytes_read;
	fSuccess = ReadFile(RTQueue[slaveId][queueType][RTInputQueue],
				   data,
				   size,
				   (PDWORD)&nb_bytes_read,
				   NULL);

    if (!fSuccess){
    	printf("ReadFile failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
    }

	return nb_bytes_read;
}

UINT32 RTQueuePop_UINT32(UINT8 slaveId, RTQueueType queueType){
	UINT32 data;
	RTQueuePop(slaveId, queueType, &data, sizeof(UINT32));
	return data;
}

UINT32 RTQueueNonBlockingPop(UINT8 slaveId, RTQueueType queueType, void* data, int size){
	int nb_bytes_read;
	DWORD cbMessage, cMessage;
	BOOL fSuccess;

	nb_bytes_read = 0;
	cbMessage = 0;
	cMessage = 0;

	fSuccess = GetMailslotInfo(
			RTQueue[slaveId][queueType][RTInputQueue],  // mailslot handle
        (LPDWORD) NULL,               // no maximum message size
        &cbMessage,                   // size of next message
        &cMessage,                    // number of messages
        (LPDWORD) NULL);              // no read time-out

	if(!fSuccess){
    	printf("GetMailslotInfo failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}
    if (cbMessage != MAILSLOT_NO_MESSAGE)
    {
    	nb_bytes_read = RTQueuePop(slaveId, queueType, data, size);
    }
	return nb_bytes_read;
}
