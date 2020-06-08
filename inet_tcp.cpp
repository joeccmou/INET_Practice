/*
 * inet_tcp.cpp
 *
 *  Created on: 2019年7月3日
 *      Author: joeccmou
 */

#include "inet_tcp_socket.h"
#include <pthread.h>

typedef struct
{
	int		threadid;
}ThreadArg_t;

bool	bIsConnected = 0;
char	read_buf[BUFFER_SIZE] = {0};
char	write_buf[BUFFER_SIZE] = {0};

pthread_mutex_t mutex;

void* tcp_read(void* arg)
{

	int sfd = ((ThreadArg_t*)arg)->threadid;

    ssize_t 	recv_size = 0;

	//int		    iReturn = 0;

	if(bIsConnected==0)
	{
		printf("tcp_read: no connection\r\n");
		return 0;
	}

	while(bIsConnected)
	{
		 memset(read_buf,0,BUFFER_SIZE);

		 //加锁
//		 if (pthread_mutex_lock(&mutex) != 0)
//		 {
//			perror("tcp_read: lock error! \n");
//		 }
		recv_size = recv(sfd, read_buf, BUFFER_SIZE,0); //从sfd服务端接收数据
		// 解锁
//		if (pthread_mutex_unlock(&mutex) != 0)
//		{
//			perror("tcp_read: unlock error! \n");
//		}

		if(recv_size < 0)
		{
			int errsv  = errno;
			if(EAGAIN != errno && EWOULDBLOCK != errno )
			{	// 发生error
				perror("tcp_read: recv error! \n ");
				break;
			}
		}
		else if(recv_size == 0)
		{
			// socket已正常关闭.
			printf("tcp_read: the socket is closed normally\r\n");
			break;
		}

		puts(read_buf);

		if(strcmp(read_buf,"disconnect")==0)
		{
			break;
		}
	}


	printf("tcp_read: disconnect\r\n");
	return (void*)recv_size;

}


void* tcp_write(void* arg)
{
	int sfd = ((ThreadArg_t*)arg)->threadid;

	ssize_t 	send_size = 0;
	ssize_t     send_total_size = 0;


	while(bIsConnected)
	{
		//加锁
//		if (pthread_mutex_lock(&mutex) != 0)
//		{
//			perror("tcp_write: lock error! \n");
//		}
		send_size = send(sfd, "hello", 6, 0);
		// 解锁
//		if (pthread_mutex_unlock(&mutex) != 0)
//		{
//			perror("tcp_write: unlock error! \n");
//		}


		sleep(1);
		send_total_size += send_size;
	}

	printf("tcp_write: %d Bytes\r\n", (int)send_total_size);
	return 0;

}


int main(int argc, char* argv[])
{


#if 0

	signalhandler();

	int sfd = tcp_init("192.168.0.165", 60000); //

	while (1) //用while循环表示可以与多个客户端接收和发送，但仍是阻塞模式的
	{
		int cfd = tcp_accept(sfd);

		char buf[512] = { 0 };

		if (recv(cfd, buf, sizeof(buf), 0) == -1) //从cfd客户端接收数据存于buf中
		{
			perror("recv");
			close(cfd);
			close(sfd);
			exit(-1);
		}

		puts(buf);

		if (send(cfd, "hello world", 12, 0) == -1) //从buf中取向cfd客户端发送数据
		{
			perror("send");
			close(cfd);
			close(sfd);
			exit(-1);
		}

		close(cfd);

	}

	close(sfd);


#else


		int iResult = 0;

	    int sfd = tcp_connect("192.168.0.141", 61000);
	    if(sfd<0)
	    {
	    	return sfd;
	    }

//		int flags;
//
//	    flags = fcntl(sfd, F_GETFL, 0);                       //获取文件的flags值。
//		if(flags==-1)
//		{
//			perror("tcp_connect: get socket flags failure\r\n");
//			close(sfd);
//			return flags;
//		}
//		flags |= O_NONBLOCK;
//		iResult = fcntl(sfd, F_SETFL, flags);  				 //设置成非阻塞模式；
//		if(iResult==-1)
//		{
//			perror("tcp_connect: set socket flags failure\r\n");
//			close(sfd);
//			return iResult;
//		}

	    bIsConnected = 1;	//设置连接成功标志

	    //初始化互斥锁
	    pthread_mutexattr_t attr;

	    iResult = pthread_mutexattr_init(&attr);
	    if(iResult!=0)
	    {
	    	perror("main: mutex attribute init failure\r\n ");
	    	close(sfd);
	    	return iResult;
	    }

	    iResult = pthread_mutex_init(&mutex, &attr);
	    if ( iResult != 0 )
	    {
			// 互斥锁初始化失败，关闭socket并退出
	    	perror("main: mutex init failure\r\n ");
			close(sfd);
			return iResult;
		}

	    ThreadArg_t	arg = {0};
	    pthread_t pthid_read = 0;
	    pthread_t pthid_write = 0;

	    arg.threadid = sfd;
	    iResult = pthread_create(&pthid_read, NULL, tcp_read, (void*)&arg);
	    if(iResult!=0)
	    {
	    	perror("main: pthid_read failure! \r\n");
	    	return  iResult;
	    }

	    pthread_create(&pthid_write, NULL, tcp_write, (void*)&arg);
	    if(iResult!=0)
		{
			perror("main: pthid_write failure! \r\n");
			return  iResult;
		}


	    while(bIsConnected)
	    {
	    	sleep(1);
	    }

	    pthread_mutexattr_destroy(&attr);
	    pthread_mutex_destroy(&mutex);

	    close(sfd);




#endif

	    return 0;
}
