#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <pthread.h>

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 10

void lt(epoll_event *, int , int , int);
int setnonblocking(int fd);
void addfd(int epollfd, int fd, bool enable_et);
//void et(epoll_event *, int , int , int);

int main(int argc, char **argv)
{
	if(argc < 3)
	{
		printf("arg no good\n");
	}

	char *ip = argv[1];
	int port = atoi(argv[2]);

	int listen_sock = socket(PF_INET, SOCK_STREAM, 0);
	assert(listen_sock >= 0);

	struct sockaddr_in socket_address;
	memset(&socket_address, 0, sizeof(socket_address));

	socket_address.sin_family = AF_INET;
	inet_pton( AF_INET, ip, &socket_address.sin_addr);
	socket_address.sin_port = htons(port);
	
	int ret;
	ret = bind(listen_sock, (struct sockaddr *) &socket_address, sizeof(socket_address));
	assert(ret != -1);

	ret = listen(listen_sock, 3);
	assert(ret != -1);

	/*
	struct sockaddr_in client_socket_addr;
	socklen_t client_socket_addr_length = sizeof(client_socket_addr);

	int socket_client = accept(listen_sock, &client_socket_addr, &client_socket_addr_length);
	
	printf("accept a tcp link\n");

	if(socket_client < 0)
	{
		printf("error\n");
	}
*/	
	//output ready event
	epoll_event events[MAX_EVENT_NUMBER];

	int epollfd = epoll_create(5);
	assert(epollfd != -1);
	
	addfd(epollfd, listen_sock, true);

	while(1)
	{
		
		printf("epoll_wait() begin\n");
		int ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
		printf("epoll_wait() finish\n");
		assert(ret >= 0);

		lt(events, ret, epollfd, listen_sock);
		//et(events, ret, epollfd, listenfd);
	}

	return 1;
}

int setnonblocking(int fd)
{
	int old_option = fcntl(fd,F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}

void addfd(int epollfd, int listen_fd, bool enable_et)
{
	epoll_event event;
	event.data.fd = listen_fd;
	event.events = EPOLLIN;
	if(enable_et)
	{
		event.events |= EPOLLET;
	}
	epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_fd, &event);
	setnonblocking(listen_fd);
}


void lt(epoll_event *events, int number, int epollfd, int listenfd)
{
	char buf[BUFFER_SIZE];
	for(int i=0;i<number;i++)
	{
		int sockfd = events[i].data.fd;
		if(sockfd == listenfd)
		{
			struct sockaddr_in client_addr;
			socklen_t client_addr_length = sizeof(client_addr);

			printf("accept() begin\n");
			int connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_addr_length);
			printf("accept() finish\n");
			addfd(epollfd, connfd, false);
		}
		else if(events[i].events & EPOLLIN)
		{
			printf("event trigger once!\n");
			memset(buf,'\0',BUFFER_SIZE);
			
			printf("recv() begin\n");
			int ret = recv(sockfd, buf, BUFFER_SIZE-1, 0);
			printf("recv() finish\n");
			printf("data : %s\n", buf);
			if(ret <= 0)
			{
				close(sockfd);
				continue;
			}
		}
		else
		{
			printf("something\n");
		}
	}
}
