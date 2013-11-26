
/********************************************************************************
 * Copyright or © or Copr. IETR/INSA (2013): Julien Heulot, Yaset Oliva,	*
 * Maxime Pelcat, Jean-François Nezan, Jean-Christophe Prevotet			*
 * 										*
 * [jheulot,yoliva,mpelcat,jnezan,jprevote]@insa-rennes.fr			*
 * 										*
 * This software is a computer program whose purpose is to execute		*
 * parallel applications.							*
 * 										*
 * This software is governed by the CeCILL-C license under French law and	*
 * abiding by the rules of distribution of free software.  You can  use, 	*
 * modify and/ or redistribute the software under the terms of the CeCILL-C	*
 * license as circulated by CEA, CNRS and INRIA at the following URL		*
 * "http://www.cecill.info". 							*
 * 										*
 * As a counterpart to the access to the source code and  rights to copy,	*
 * modify and redistribute granted by the license, users are provided only	*
 * with a limited warranty  and the software's author,  the holder of the	*
 * economic rights,  and the successive licensors  have only  limited		*
 * liability. 									*
 * 										*
 * In this respect, the user's attention is drawn to the risks associated	*
 * with loading,  using,  modifying and/or developing or reproducing the	*
 * software by the user in light of its specific status of free software,	*
 * that may mean  that it is complicated to manipulate,  and  that  also	*
 * therefore means  that it is reserved for developers  and  experienced	*
 * professionals having in-depth computer knowledge. Users are therefore	*
 * encouraged to load and test the software's suitability as regards their	*
 * requirements in conditions enabling the security of their systems and/or 	*
 * data to be ensured and,  more generally, to use and operate it in the 	*
 * same conditions as regards security. 					*
 * 										*
 * The fact that you are presently reading this means that you have had		*
 * knowledge of the CeCILL-C license and that you accept its terms.		*
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "types.h"
#include "hwQueues.h"
//#include <SchedulerDimensions.h>

typedef enum{
	CTRL=0,
	INFO=1
} MailboxType;

typedef enum{
	FIFO_DIR_INPUT	= 0,
	FIFO_DIR_OUTPUT	= 1
} FifoDir;

static HANDLE OS_QGRT[NB_MAX_CTRLQ][2][2];

#define PIPE_BASE_NAME 	"\\\\.\\pipe\\"
#define BUFFER_SIZE		512

void OS_QInit(int nbSlaves){
	int i;

	for(i=0; i<nbSlaves; i++){
		char tempStr[50];
		// Creating input pipes.
		sprintf(tempStr, "%sCtrl_%dtoGrt", PIPE_BASE_NAME, i);
		OS_QGRT[i][CTRL][FIFO_DIR_INPUT] = CreateNamedPipe(
								tempStr,
								PIPE_ACCESS_DUPLEX,
								PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
								2,
								0,
								BUFFER_SIZE,
								0,
								NULL);
		if (OS_QGRT[i][CTRL][FIFO_DIR_INPUT] == INVALID_HANDLE_VALUE) {
			printf("CreateNamedPipe failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
		}


		sprintf(tempStr, "%sInfo_%dtoGrt", PIPE_BASE_NAME, i);
		OS_QGRT[i][INFO][FIFO_DIR_INPUT] = CreateNamedPipe(
								tempStr,
								PIPE_ACCESS_DUPLEX,
								PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
								2,
								0,
								BUFFER_SIZE,
								0,
								NULL);
		if (OS_QGRT[INFO][FIFO_DIR_INPUT] == INVALID_HANDLE_VALUE) {
			printf("CreateNamedPipe failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
		}


		// Connecting to output pipes.
		sprintf(tempStr, "%sCtrl_Grtto%d", PIPE_BASE_NAME, i);
		OS_QGRT[i][CTRL][FIFO_DIR_OUTPUT] = CreateFile(
									tempStr,   		// pipe name
									GENERIC_WRITE,	// write access
									0,              // no sharing
									NULL,           // default security attributes
									OPEN_EXISTING,  // opens existing pipe
									0,              // default attributes
									NULL);          // no template file
		if (OS_QGRT[i][CTRL][FIFO_DIR_OUTPUT] == INVALID_HANDLE_VALUE) {
			printf("CreateFile failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
		}

		sprintf(tempStr, "%sInfo_Grtto%d",PIPE_BASE_NAME, i);
		OS_QGRT[i][INFO][FIFO_DIR_OUTPUT] = CreateFile(
									tempStr,   		// pipe name
									GENERIC_WRITE,	// write access
									0,              // no sharing
									NULL,           // default security attributes
									OPEN_EXISTING,  // opens existing pipe
									0,              // default attributes
									NULL);          // no template file
		if (OS_QGRT[i][INFO][FIFO_DIR_OUTPUT] == INVALID_HANDLE_VALUE) {
			printf("CreateFile failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
		}

		// Waiting for clients to connect.
		if(!ConnectNamedPipe(OS_QGRT[i][CTRL][FIFO_DIR_INPUT], NULL) && GetLastError() != ERROR_PIPE_CONNECTED){
			printf("ConnectNamedPipe failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
		}
		if(!ConnectNamedPipe(OS_QGRT[i][INFO][FIFO_DIR_INPUT], NULL) && GetLastError() != ERROR_PIPE_CONNECTED){
			printf("ConnectNamedPipe failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
		}
	}

//	UINT32 c;
//	for(i=0; i<nbSlaves; i++){
//		while(OS_CtrlQPop_nonBlocking(i, &c, sizeof(UINT32))==sizeof(UINT32));
//		while(OS_InfoQPop_nonBlocking(i, &c, sizeof(UINT32))==sizeof(UINT32));
//	}
}

/* Ctrl Mailbox */
UINT32 OS_CtrlQPush(int id, void* data, int size){
    BOOL fSuccess = FALSE;
    int nb_bytes_written;
    fSuccess = WriteFile(
    				OS_QGRT[id][CTRL][FIFO_DIR_OUTPUT],	// pipe handle
    				data,		             	// message
    				size,		             	// message length
    				(PDWORD)&nb_bytes_written,	// bytes written
    				NULL);                 		// not overlapped

    if (!fSuccess){
    	printf("WriteFile failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
    }

	return nb_bytes_written;
}

UINT32 OS_CtrlQPop(int id, void* data, int size){
	BOOL fSuccess = FALSE;
	int nb_bytes_read;
	fSuccess = ReadFile(OS_QGRT[id][CTRL][FIFO_DIR_INPUT],
				   data,
				   size,
				   (PDWORD)&nb_bytes_read,
				   NULL);

    if (!fSuccess){
    	printf("ReadFile failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
    }

	return nb_bytes_read;
}

UINT32 OS_CtrlQPushInt(int id, unsigned int data){
	return OS_CtrlQPush(id, &data, sizeof(unsigned int));
}


UINT32 OS_CtrlQPopInt(int id){
	UINT32 data;
	OS_CtrlQPop(id, &data, sizeof(UINT32));
	return data;
}

UINT32 OS_CtrlQPop_nonBlocking(int id, void* data, int size){
	// Changing the pipe to non-blocking mode.
	DWORD mode = PIPE_NOWAIT;
	BOOL fSuccess = SetNamedPipeHandleState(
										OS_QGRT[id][CTRL][FIFO_DIR_INPUT],   // pipe handle
										&mode, 			 			// new pipe mode
										NULL,     					// don't set maximum bytes
										NULL);    					// don't set maximum time
	if(!fSuccess){
    	printf("SetNamedPipeHandleState failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}

	int nb_bytes_read = OS_CtrlQPop(id, data, size);

	// Resetting the pipe to blocking mode.
	mode = PIPE_WAIT;
	fSuccess = SetNamedPipeHandleState(
								OS_QGRT[id][CTRL][FIFO_DIR_INPUT],   // pipe handle
								&mode, 			 			// new pipe mode
								NULL,     					// don't set maximum bytes
								NULL);    					// don't set maximum time
	if(!fSuccess){
    	printf("SetNamedPipeHandleState failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}

	return nb_bytes_read;
}

/* Info Mailbox */
UINT32 OS_InfoQPush(int id, void* data, int size){
    BOOL fSuccess = FALSE;
    int nb_bytes_written;
    fSuccess = WriteFile(
    				OS_QGRT[id][INFO][FIFO_DIR_OUTPUT],	// pipe handle
    				data,		             	// message
    				size,		             	// message length
    				(PDWORD)&nb_bytes_written,	// bytes written
    				NULL);                 		// not overlapped

    if (!fSuccess){
    	printf("WriteFile failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
    }

	return nb_bytes_written;
}

UINT32 OS_InfoQPop(int id, void* data, int size){
	BOOL fSuccess = FALSE;
	int nb_bytes_read;
	fSuccess = ReadFile(OS_QGRT[id][INFO][FIFO_DIR_INPUT],
				   data,
				   size,
				   (PDWORD)&nb_bytes_read,
				   NULL);

    if (!fSuccess){
    	printf("ReadFile failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
    }

	return nb_bytes_read;
}

UINT32 OS_InfoQPushInt(int id, unsigned int data){
	return OS_InfoQPush(id, &data, sizeof(unsigned int));
}


UINT32 OS_InfoQPopInt(int id){
	UINT32 data;
	OS_InfoQPop(id, &data, sizeof(UINT32));
	return data;
}

UINT32 OS_InfoQPop_nonBlocking(int id, void* data, int size){
	// Changing the pipe to non-blocking mode.
	DWORD mode = PIPE_NOWAIT;
	BOOL fSuccess = SetNamedPipeHandleState(
										OS_QGRT[id][INFO][FIFO_DIR_INPUT],   // pipe handle
										&mode, 			 			// new pipe mode
										NULL,     					// don't set maximum bytes
										NULL);    					// don't set maximum time
	if(!fSuccess){
    	printf("SetNamedPipeHandleState failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}

	int nb_bytes_read = OS_CtrlQPop(id, data, size);

	// Resetting the pipe to blocking mode.
	mode = PIPE_WAIT;
	fSuccess = SetNamedPipeHandleState(
								OS_QGRT[id][INFO][FIFO_DIR_INPUT],   // pipe handle
								&mode, 			 			// new pipe mode
								NULL,     					// don't set maximum bytes
								NULL);    					// don't set maximum time
	if(!fSuccess){
    	printf("SetNamedPipeHandleState failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}

	return nb_bytes_read;
	return 0;
}
