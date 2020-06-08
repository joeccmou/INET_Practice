/*
 * inet_tcp_socket.h
 *
 *  Created on: 2019年7月3日
 *      Author: joeccmou
 */

#ifndef INET_TCP_SOCKET_H_
#define INET_TCP_SOCKET_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>


extern int tcp_init(const char* ip,int port);

extern int tcp_accept(int sfd);

extern int tcp_connect(const char* ip,int port);

extern void signalhandler(void);


#define BUFFER_SIZE 1024


#endif /* INET_TCP_SOCKET_H_ */
