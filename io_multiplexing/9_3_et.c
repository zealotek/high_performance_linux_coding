#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/types.h>
#include<unistd.h>
#include<errno.h>
#include<assert.h>

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include<fcntl.h>
#include<sys/epoll.h>
#include<pthread.h>


int main(int argc, char ** argv)
{
	if(argc<2)
	{
		return -1;
	}

	int ret;

	//ip address & port
	const char *ip = argv[1];
	int port = atoi(argv[2]);

	struct sockaddr_in listen_addr;
	memset(&listen_addr, '\0', sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &listen_addr.sin_addr);
	listen_addr.sin_port = htons(port);

	//socket() - bind() - listen()
	int listen_fd = socket(PF_INET, SOCK_STREAM, 0);
	assert(listen_fd >= 0);

	ret = bind(listen_fd, (struct sockaddr *)&listen_addr, sizeof(listen_addr));
	assert(ret != -1);

	ret = listen(listen_fd, 5);
	assert(ret != -1);

	//epoll
	int epoll_fd = epoll_create(5);
	assert(epoll_fd != -1);

	struct epoll_event event;
	event.data.fd = listen_fd;
	event.events = EPOLLIN | EPOLLET;

	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event);

	//setnonblocking(listen_fd);
	int old_option = fcntl(listen_fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(listen_fd, F_SETFL, new_option);
	
	//loop
	struct epoll_event events[1024];
	char buf[255];
	int i;
	int sockfd;

	while(1)
	{
		printf("epoll_wait() begin\n");
		ret = epoll_wait(epoll_fd, events, sizeof(events), -1);
		printf("epoll_wait() finish\n");
		for(i=0;i<1024;i++)
		{
			sockfd = events[i].data.fd;
			if(sockfd == listen_fd)
			{
				struct sockaddr_in client_addr;
				socklen_t client_addr_length = sizeof(client_addr);

				printf("accept() begin\n");
				int conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_length);
				printf("accept() finish\n");
				//addfd(epoll_fd, conn_fd, true);

				
				struct epoll_event event;
				event.data.fd = listen_fd;
				event.events = EPOLLIN | EPOLLET;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event);
				//setnonblocking(listen_fd);
				old_option = fcntl(listen_fd,F_GETFL);
				new_option = old_option | O_NONBLOCK;
				fcntl(listen_fd, F_SETFL, new_option);
			}
			else if(events[i].events & EPOLLIN)
			{
				while(1)
				{
					memset(buf, '\0', sizeof(buf));
					printf("recv() begin\n");
					ret = recv(sockfd, buf, sizeof(buf)-1, 0);
					printf("recv() finish\n");
					if(ret < 0)
					{
						close(sockfd);
						break;
					}
					printf("data : %s\n", buf);
				}
			}
		}
	}
	return 0;
}


