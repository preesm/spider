
/****************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet				*
 * 																			*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr					*
 * 																			*
 * This software is a computer program whose purpose is to execute			*
 * parallel applications.													*
 * 																			*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 												*
 * 																			*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 																*
 * 																			*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or *
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 									*
 * 																			*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.			*
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "types.h"
#include "hwQueues.h"

#define PIPE_BASE_NAME 	"\\\\.\\pipe\\"
#define BUFFER_SIZE		512

//static HANDLE RTQueue[2] = {INVALID_HANDLE_VALUE};
static HANDLE RTQueue[nbQueueTypes][2] = {{INVALID_HANDLE_VALUE},{INVALID_HANDLE_VALUE}};
int cpuId;

void RTQueuesInit(){
	char tempStr[50];

	// Creating input pipes.
	sprintf(tempStr, "%sCtrl_Grtto%d", PIPE_BASE_NAME, cpuId);
	RTQueue[RTCtrlQueue][RTInputQueue] = CreateNamedPipe(
										tempStr,
										PIPE_ACCESS_DUPLEX,
										PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
										2,
										0,
										BUFFER_SIZE,
										0,
										NULL);
	if (RTQueue[RTCtrlQueue][RTInputQueue] == INVALID_HANDLE_VALUE) {
		printf("CreateNamedPipe failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}

	sprintf(tempStr, "%sInfo_Grtto%d",PIPE_BASE_NAME, cpuId);
	RTQueue[RTInfoQueue][RTInputQueue] = CreateNamedPipe(
										tempStr,
										PIPE_ACCESS_DUPLEX,
										PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
										2,
										0,
										BUFFER_SIZE,
										0,
										NULL);
	if (RTQueue[RTInfoQueue][RTInputQueue] == INVALID_HANDLE_VALUE) {
		printf("CreateNamedPipe failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}

	sprintf(tempStr, "%sJob_Grtto%d",PIPE_BASE_NAME, cpuId);
	RTQueue[RTJobQueue][RTInputQueue] = CreateNamedPipe(
										tempStr,
										PIPE_ACCESS_DUPLEX,
										PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
										2,
										0,
										BUFFER_SIZE,
										0,
										NULL);
	if (RTQueue[RTJobQueue][RTInputQueue] == INVALID_HANDLE_VALUE) {
		printf("CreateNamedPipe failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}

	// Waiting for client to connect.
	if(!ConnectNamedPipe(RTQueue[RTCtrlQueue][RTInputQueue], NULL) && GetLastError() != ERROR_PIPE_CONNECTED){
		printf("ConnectNamedPipe failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}
	if(!ConnectNamedPipe(RTQueue[RTInfoQueue][RTInputQueue], NULL) && GetLastError() != ERROR_PIPE_CONNECTED){
		printf("ConnectNamedPipe failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}
	if(!ConnectNamedPipe(RTQueue[RTJobQueue][RTInputQueue], NULL) && GetLastError() != ERROR_PIPE_CONNECTED){
		printf("ConnectNamedPipe failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}


	// Connecting to output pipes.
	sprintf(tempStr, "%sCtrl_%dtoGrt", PIPE_BASE_NAME, cpuId);
	RTQueue[RTCtrlQueue][RTOutputQueue] = CreateFile(
								tempStr,   		// pipe name
								GENERIC_WRITE,	// write access
								0,              // no sharing
								NULL,           // default security attributes
								OPEN_EXISTING,  // opens existing pipe
								0,              // default attributes
								NULL);          // no template file
	if (RTQueue[RTCtrlQueue][RTOutputQueue] == INVALID_HANDLE_VALUE) {
		printf("CreateFile failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}

	sprintf(tempStr, "%sInfo_%dtoGrt",PIPE_BASE_NAME, cpuId);
	RTQueue[RTInfoQueue][RTOutputQueue] = CreateFile(
								tempStr,   		// pipe name
								GENERIC_WRITE,	// write access
								0,              // no sharing
								NULL,           // default security attributes
								OPEN_EXISTING,  // opens existing pipe
								0,              // default attributes
								NULL);          // no template file
	if (RTQueue[RTInfoQueue][RTOutputQueue] == INVALID_HANDLE_VALUE) {
		printf("CreateFile failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}

	sprintf(tempStr, "%sJob_%dtoGrt",PIPE_BASE_NAME, cpuId);
	RTQueue[RTJobQueue][RTOutputQueue] = CreateFile(
								tempStr,   		// pipe name
								GENERIC_WRITE,	// write access
								0,              // no sharing
								NULL,           // default security attributes
								OPEN_EXISTING,  // opens existing pipe
								0,              // default attributes
								NULL);          // no template file
	if (RTQueue[RTJobQueue][RTOutputQueue] == INVALID_HANDLE_VALUE) {
		printf("CreateFile failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}
}


UINT32 RTQueuePush(RTQueueType queueType, void* data, int size){
    BOOL fSuccess = FALSE;
    int nb_bytes_written;
    fSuccess = WriteFile(
    				RTQueue[queueType][RTOutputQueue],		// pipe handle
    				data,		             	// message
    				size,		             	// message length
    				(PDWORD)&nb_bytes_written,	// bytes written
    				NULL);                 		// not overlapped

    if (!fSuccess){
    	printf("WriteFile failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
    }

	return nb_bytes_written;
}

UINT32 RTQueuePush_UINT32(RTQueueType queueType, UINT32 value){
	UINT32 val = value;
	return RTQueuePush(queueType, &val, sizeof(UINT32));
}

UINT32 RTQueuePop(RTQueueType queueType, void* data, int size){
	int nb_bytes_read;
	ReadFile(RTQueue[queueType][RTInputQueue],
				   data,
				   size,
				   (PDWORD)&nb_bytes_read,
				   NULL);

	return nb_bytes_read;
}

UINT32 RTQueuePop_UINT32(RTQueueType queueType){
	UINT32 value;
	RTQueuePop(queueType, &value, sizeof(UINT32));
	return value;
}

UINT32 RTQueueNonBlockingPop(RTQueueType queueType, void* data, int size){
	int nb_bytes_read;

	// Changing the pipe to non-blocking mode.
	DWORD mode = PIPE_NOWAIT;
	BOOL fSuccess = SetNamedPipeHandleState(
										RTQueue[queueType][RTInputQueue],   // pipe handle
										&mode, 		 			// new pipe mode
										NULL,     				// don't set maximum bytes
										NULL);    				// don't set maximum time
	if(!fSuccess){
    	printf("SetNamedPipeHandleState failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}

	nb_bytes_read = RTQueuePop(queueType, data, size);

	// Resetting the pipe to blocking mode.
	mode = PIPE_WAIT;
	fSuccess = SetNamedPipeHandleState(
								RTQueue[queueType][RTInputQueue],   // pipe handle
								&mode, 		 			// new pipe mode
								NULL,     				// don't set maximum bytes
								NULL);    				// don't set maximum time
	if(!fSuccess){
    	printf("SetNamedPipeHandleState failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}

	return nb_bytes_read;
}
