/*
 * HWQueues.c
 *
 *  Created on: Jun 12, 2013
 *      Author: jheulot
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "types.h"
#include "hwQueues.h"
#include <SchedulerDimensions.h>

typedef enum{
	CTRL=0,
	INFO=1
} MailboxType;

typedef enum{
	INPUT=0,
	OUTPUT=1
} FifoDir;

static HANDLE OS_QGRT[NB_MAX_CTRLQ][2][2];

#define PIPE_BASE_NAME 	"\\\\.\\pipe\\"
#define BUFFER_SIZE		512

void OS_QInit(int nbSlaves){
	int i;

	for(i=0; i<nbSlaves; i++){
		char tempStr[50];
		// Creating output pipes.
		sprintf(tempStr, "%sCtrl_%dtoGrt", PIPE_BASE_NAME, i);
		OS_QGRT[i][CTRL][INPUT] = CreateNamedPipe(
								tempStr,
								PIPE_ACCESS_INBOUND,
								PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
								2,
								0,
								BUFFER_SIZE,
								0,
								NULL);
		if (OS_QGRT[i][CTRL][INPUT] == INVALID_HANDLE_VALUE) {
			printf("CreateNamedPipe failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
		}


		sprintf(tempStr, "%sInfo_%dtoGrt", PIPE_BASE_NAME, i);
		OS_QGRT[i][INFO][INPUT] = CreateNamedPipe(
								tempStr,
								PIPE_ACCESS_INBOUND,
								PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
								2,
								0,
								BUFFER_SIZE,
								0,
								NULL);
		if (OS_QGRT[INFO][INPUT] == INVALID_HANDLE_VALUE) {
			printf("CreateNamedPipe failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
		}


		// Connecting to output pipes.
		sprintf(tempStr, "%sCtrl_Grtto%d", PIPE_BASE_NAME, i);
		OS_QGRT[i][CTRL][OUTPUT] = CreateFile(
									tempStr,   		// pipe name
									GENERIC_WRITE,	// write access
									0,              // no sharing
									NULL,           // default security attributes
									OPEN_EXISTING,  // opens existing pipe
									0,              // default attributes
									NULL);          // no template file
		if (OS_QGRT[i][CTRL][OUTPUT] == INVALID_HANDLE_VALUE) {
			printf("CreateFile failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
		}

		sprintf(tempStr, "%sInfo_Grtto%d",PIPE_BASE_NAME, i);
		OS_QGRT[i][INFO][OUTPUT] = CreateFile(
									tempStr,   		// pipe name
									GENERIC_WRITE,	// write access
									0,              // no sharing
									NULL,           // default security attributes
									OPEN_EXISTING,  // opens existing pipe
									0,              // default attributes
									NULL);          // no template file
		if (OS_QGRT[i][INFO][OUTPUT] == INVALID_HANDLE_VALUE) {
			printf("CreateFile failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
		}

		// Waiting for clients to connect.
		if(!ConnectNamedPipe(OS_QGRT[i][CTRL][INPUT], NULL) && GetLastError() != ERROR_PIPE_CONNECTED){
			printf("ConnectNamedPipe failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
		}
		if(!ConnectNamedPipe(OS_QGRT[i][INFO][INPUT], NULL) && GetLastError() != ERROR_PIPE_CONNECTED){
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
    				OS_QGRT[id][CTRL][OUTPUT],	// pipe handle
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
	fSuccess = ReadFile(OS_QGRT[id][CTRL][INPUT],
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
	return OS_CtrlQPop(id, data, size);
}

/* Info Mailbox */
UINT32 OS_InfoQPush(int id, void* data, int size){
	char tempStr[50];
	sprintf(tempStr, "%sInfo_Grtto%d", PIPE_BASE_NAME, id);
	HANDLE hPipe = CreateFile(
			tempStr,   		// pipe name
			GENERIC_WRITE,	// write access
			0,              // no sharing
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe
			0,              // default attributes
			NULL);          // no template file

    // Exit if an error occurs.
    if (hPipe == INVALID_HANDLE_VALUE){perror("Could not open pipe"); exit(EXIT_FAILURE);}

    BOOL fSuccess = FALSE;
    int nb_bytes_written;
    fSuccess = WriteFile(
	  hPipe,    			 	// pipe handle
	  data,		             	// message
	  size,		             	// message length
	  (PDWORD)&nb_bytes_written,// bytes written
	  NULL);                 	// not overlapped

    if (!fSuccess){perror("Could not write into the pipe"); exit(EXIT_FAILURE);}

	return nb_bytes_written;
}

UINT32 OS_InfoQPop(int id, void* data, int size){
	int nb_bytes_read;
	ReadFile(OS_QGRT[id][INFO][INPUT],
		   data,
		   size,
		   (PDWORD)&nb_bytes_read,
		   NULL);
//		if(res>0) i+=res;
//	}
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
//	int file = OS_QLRT[id][INFO][IN];
//
//	return read(file, (char*)data, size);
	return 0;
}
