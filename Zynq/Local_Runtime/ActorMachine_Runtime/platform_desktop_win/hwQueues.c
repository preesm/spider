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

typedef enum{
	CTRL=0,
	INFO=1
} MailboxType;

typedef enum{
	INPUT=0,
	OUTPUT=1
} FifoDir;

//static HANDLE OS_QGRT[2] = {INVALID_HANDLE_VALUE};
static HANDLE OS_QGRT[2][2] = {{INVALID_HANDLE_VALUE},{INVALID_HANDLE_VALUE}};
int cpuId;

#define PIPE_BASE_NAME 	"\\\\.\\pipe\\"
#define BUFFER_SIZE		512

void OS_QInit(){
	char tempStr[50];

	// Creating input pipes.
	sprintf(tempStr, "%sCtrl_Grtto%d", PIPE_BASE_NAME, cpuId);
	OS_QGRT[CTRL][INPUT] = CreateNamedPipe(tempStr,
										   PIPE_ACCESS_INBOUND,
										   PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
										   2,
										   0,
										   BUFFER_SIZE,
										   0,
										   NULL);
	if (OS_QGRT[CTRL][INPUT] == INVALID_HANDLE_VALUE) {
		printf("CreateNamedPipe failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}

	sprintf(tempStr, "%sInfo_Grtto%d",PIPE_BASE_NAME, cpuId);
	OS_QGRT[INFO][INPUT] = CreateNamedPipe(tempStr,
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

	// Waiting for client to connect.
	if(!ConnectNamedPipe(OS_QGRT[CTRL][INPUT], NULL) && GetLastError() != ERROR_PIPE_CONNECTED){
		printf("ConnectNamedPipe failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}
	if(!ConnectNamedPipe(OS_QGRT[INFO][INPUT], NULL) && GetLastError() != ERROR_PIPE_CONNECTED){
		printf("ConnectNamedPipe failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}


	// Connecting to output pipes.
	sprintf(tempStr, "%sCtrl_%dtoGrt", PIPE_BASE_NAME, cpuId);
	OS_QGRT[CTRL][OUTPUT] = CreateFile(
								tempStr,   		// pipe name
								GENERIC_WRITE,	// write access
								0,              // no sharing
								NULL,           // default security attributes
								OPEN_EXISTING,  // opens existing pipe
								0,              // default attributes
								NULL);          // no template file
	if (OS_QGRT[CTRL][OUTPUT] == INVALID_HANDLE_VALUE) {
		printf("CreateFile failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}

	sprintf(tempStr, "%sInfo_%dtoGrt",PIPE_BASE_NAME, cpuId);
	OS_QGRT[INFO][OUTPUT] = CreateFile(
								tempStr,   		// pipe name
								GENERIC_WRITE,	// write access
								0,              // no sharing
								NULL,           // default security attributes
								OPEN_EXISTING,  // opens existing pipe
								0,              // default attributes
								NULL);          // no template file
	if (OS_QGRT[INFO][OUTPUT] == INVALID_HANDLE_VALUE) {
		printf("CreateFile failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
	}
}

/*******************/
/* Control Mailbox */
/*******************/

UINT32 OS_CtrlQPush(void* data, int size){
    BOOL fSuccess = FALSE;
    int nb_bytes_written;
    fSuccess = WriteFile(
    				OS_QGRT[CTRL][OUTPUT],		// pipe handle
    				data,		             	// message
    				size,		             	// message length
    				(PDWORD)&nb_bytes_written,	// bytes written
    				NULL);                 		// not overlapped

    if (!fSuccess){
    	printf("WriteFile failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
    }

	return nb_bytes_written;
}

UINT32 OS_CtrlQPop(void* data, int size){
	BOOL fSuccess = FALSE;
	int nb_bytes_read;
	fSuccess = ReadFile(OS_QGRT[CTRL][INPUT],
				   data,
				   size,
				   (PDWORD)&nb_bytes_read,
				   NULL);

    if (!fSuccess){
    	printf("ReadFile failed, error %ld.\n", GetLastError()); exit(EXIT_FAILURE);
    }

	return nb_bytes_read;
}

UINT32 OS_CtrlQPop_UINT32(){
	UINT32 value;
	OS_CtrlQPop(&value, sizeof(UINT32));
	return value;
}

void OS_CtrlQPush_UINT32(UINT32 value){
	UINT32 val = value;
	OS_CtrlQPush(&val, sizeof(UINT32));
}

UINT32 OS_CtrlQPop_nonBlocking(void* data, int size){
	return OS_CtrlQPop(data, size);
}

/****************/
/* Info Mailbox */
/****************/

UINT32 OS_InfoQPush(void* data, int size){
	char tempStr[50];
	sprintf(tempStr, "%sInfo_%dtoGrt", PIPE_BASE_NAME, cpuId);
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

UINT32 OS_InfoQPop(void* data, int size){
	//	int i=0;
	//	while(i < size){
			int nb_bytes_read;
			ReadFile(OS_QGRT[INFO][INPUT],
				   data,
				   size,
				   (PDWORD)&nb_bytes_read,
				   NULL);
	//		if(res>0) i+=res;
	//	}
		return nb_bytes_read;
}

UINT32 OS_InfoQPop_UINT32(){
	UINT32 value;
	OS_InfoQPop(&value, sizeof(UINT32));
	return value;
}

void OS_InfoQPush_UINT32(UINT32 value){
	UINT32 val = value;
	OS_InfoQPush(&val, sizeof(UINT32));
}

UINT32 OS_InfoQPop_nonBlocking(void* data, int size){
//	return read(OS_QGRT[INFO][IN], data, size);
	return 0;
}
