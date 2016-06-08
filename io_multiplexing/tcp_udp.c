#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>

#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>

#include<unistd.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<errno.h>
#include<pthread.h>

#define MAX_EVENT_NUM 1024

void setnonblocking(int fd)
{
	int old_opt = fcntl(F_GETFL, fd);
	int new_opt = old_opt | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_opt);
}

int main(int argc, char **argv)
{
	int ret;

	struct sockaddr_in listen_addr;
	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &listen_addr.sin_addr);
	
	int tcpfd = socket(AF_INET, SOCK_STREAM, 0);
	ret = bind(tcpfd, (struct sockaddr *)&listen_addr, sizeof(listen_addr));
	ret = listen(tcpfd, 5);

	int udpfd = socket(AF_INET, SOCK_DGRAM, 0);
	ret = bind(udpfd, (struct sockaddr *)&listen_addr, sizeof(listen_addr));

	int epollfd = epoll_create(5);

	struct epoll_event event;

	event.data.fd = tcpfd;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, tcpfd, &event);
	setnonblocking(tcpfd);

	event.data.fd = udpfd;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, udpfd, &event);
	setnonblocking(udpfd);

	struct epoll_event events[MAX_EVENT_NUM];

	while(1)
	{
		int ret = epoll_wait(epollfd, events, MAX_EVENT_NUM, -1);
		int i;
		for(i=0;i<ret;i++)
		{
			int socketfd = events[i].data.fd;
			if(socketfd == tcpfd)
			{
				struct sockaddr_in client_addr;
				int client_addr_length = sizeof(client_addr);
				int connfd = accept(tcpfd, (struct sockaddr *)&client_addr, &client_addr_length);

				struct epoll_event event;
				event.data.fd = connfd;
				event.events = EPOLLIN | EPOLLET;
				epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &event);
				setnonblocking(connfd);
			}
			else if(socketfd == udpfd)
			{
				char buf[255];
				memset(buf, '\0', 255);

				struct sockaddr_in udp_client_addr;
				int udp_client_addr_length = sizeof(udp_client_addr);

				recvfrom(udpfd, buf, 254, 0, (struct sockaddr *)&udp_client_addr, &udp_client_addr_length);
				sendto(udpfd, buf, 254, 0, (struct sockaddr *)&udp_client_addr, udp_client_addr_length);

			}
			else if(events[i].events & EPOLLIN)
			{
				char buf[255];
				memset(buf, '\0', 255);
				while(1)
				{
					ret = recv(socketfd, buf, 254, 0);
					if(ret < 0)
					{
						close(socketfd);
						break;
					}
					else if(ret == 0)
					{
						close(socketfd);
					}
					else
					{
					}
				}
			}
		}
	}

	return 0;
}

