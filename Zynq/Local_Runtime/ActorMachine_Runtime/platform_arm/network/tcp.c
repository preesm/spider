/*
 * tcp.c
 *
 *  Created on: Aug 12, 2013
 *      Author: julien
 */

#include "tcp.h"


#define CHUNCK_SIZE 8192

SOCKET tcpInit(const char* server, int port){
	SOCKET sock;
	SOCKADDR_IN sin;
	int optval;

	/* Sockets creation */
	sock = socket(AF_INET, SOCK_STREAM, 0);

	/* Configuration socket1 */
	sin.sin_addr.s_addr = inet_addr(server);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	optval = 1;
//	if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof optval) <0){
//		printf("Socket %d: ", port);
//		perror("setsockopt failed\n");
//		exit(errno);
//	}
//
//	if(setsockopt(sock, IPPROTO_TCP, TCP_QUICKACK, &optval, sizeof optval) <0){
//		printf("Socket %d: ", port);
//		perror("setsockopt failed\n");
//		exit(errno);
//	}

	optval = 700*400*3*3;
	if(setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &optval, sizeof optval) <0){
		printf("Socket %d: ", port);
		perror("setsockopt failed\n");
		exit(errno);
	}

	if(setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &optval, sizeof optval) <0){
		printf("Socket %d: ", port);
		perror("setsockopt failed\n");
		exit(errno);
	}
//
//	struct timeval timeout;
//	timeout.tv_sec = 0;
//	timeout.tv_usec = 50000;
//
//	if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
//		printf("Socket %d: ", port);
//		perror("setsockopt failed\n");
//		exit(errno);
//	}
//
//	if (setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
//		printf("Socket %d: ", port);
//		perror("setsockopt failed\n");
//		exit(errno);
//	}

	if(connect(sock, (SOCKADDR*)&sin, sizeof(sin)) != SOCKET_ERROR){
		printf("Connected to %s on port %d\n", inet_ntoa(sin.sin_addr), htons(sin.sin_port));
		return sock;
	}else{
		printf("Fail to connect to %s on port %d\n", inet_ntoa(sin.sin_addr), htons(sin.sin_port));
		return -1;
	}
}

void tcpClose(SOCKET sock){
	/* Socket1 closing */
	close(sock);
}

void tcpReceive(SOCKET sock, void* data, int size){
	int n=0, tmp;
//	while(n<size){
//		tmp = recv(sock, data+n, size, MSG_WAITALL);
//		if(tmp<0){
//			perror("recv():");
//			exit(-1);
//		}
//		n += tmp;
//		printf("Received %d/%d\n", n, size);

	for(n=0; n<size/CHUNCK_SIZE; n++){
		tmp = recv(sock, data+n*CHUNCK_SIZE, CHUNCK_SIZE, MSG_WAITALL);
		if(tmp<0){
			perror("recv():");
			exit(-1);
		}if(tmp != CHUNCK_SIZE){
			printf("/!\\ Received %d/%d\n", tmp, CHUNCK_SIZE);
		}
	}
	tmp = recv(sock, data+(size/CHUNCK_SIZE)*CHUNCK_SIZE, size-(size/CHUNCK_SIZE)*CHUNCK_SIZE, MSG_WAITALL);
}


void tcpSend(SOCKET sock, void* data, int size){
	int n=0, tmp;

	for(n=0; n<size/CHUNCK_SIZE; n++){
		tmp = send(sock, data+n*CHUNCK_SIZE, CHUNCK_SIZE, MSG_WAITALL);
		if(tmp<0){
			perror("recv():");
			exit(-1);
		}if(tmp != CHUNCK_SIZE){
			printf("/!\\ Sended %d/%d\n", tmp, CHUNCK_SIZE);
		}
	}
	tmp = send(sock, data+(size/CHUNCK_SIZE)*CHUNCK_SIZE, size-(size/CHUNCK_SIZE)*CHUNCK_SIZE, MSG_WAITALL);

//	while(n<size){
//		tmp = send(sock, data+n, size, MSG_WAITALL);
//		if(tmp<0){
//			perror("send():");
//			exit(-1);
//		}
//		n += tmp;
//		printf("sended %d/%d\n", n, size);
//	}
}

