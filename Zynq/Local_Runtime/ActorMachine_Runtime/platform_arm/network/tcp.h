/*
 * tcp.h
 *
 *  Created on: Aug 12, 2013
 *      Author: julien
 */

#ifndef TCP_H_
#define TCP_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

SOCKET tcpInit(const char* server, int port);
void tcpClose(SOCKET sock);

void tcpReceive(SOCKET sock, void* data, int size);
void tcpSend(SOCKET sock, void* data, int size);

#endif /* TCP_H_ */
