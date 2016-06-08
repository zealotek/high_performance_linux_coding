#define _GNU_SOURCE 1

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>

#include<sys/socket.h>
#include<sys/types.h>

#include<netinet/in.h>
#include<arpa/inet.h>

#include<unistd.h>
#include<poll.h>
#include<fcntl.h>

int main(int argc, char **argv)
{
	struct sockaddr_in client_addr;
	client_addr.sin_family = AF_INET;
	inet_aton(argv[1], &client_addr.sin_addr);
	client_addr.sin_port = htons(atoi(argv[2]));

	int clientfd = socket(AF_INET, SOCK_STREAM, 0);

	struct pollfd fds[2];
	fds[0].fd = 0;
	fds[0].events = POLLIN;
	fds[0].revents = 0;
	
	fds[1].fd = clientfd;
	fds[1].events = POLLIN | POLLHUP;
	fds[1].revents = 0;

	int pipefd[2];
	pipe(pipefd);

	int ret;
	char buf[255];
	while(1)
	{
		ret = poll(fds, 2, -1);
		if(ret < 0)
		{
			return 0;
		}

		if(fds[0].revents & POLLIN)
		{
			ret = splice(0, NULL, pipefd[1], NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
			ret = splice(pipefd[0], NULL, clientfd, NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
		}
		else if(fds[1].revents & POLLIN)
		{
			memset(buf,0,sizeof(buf));
			recv(fds[1].fd, buf, sizeof(buf)-1, 0);
			printf("data:%s\n", buf);
		}
		else if(fds[1].revents & POLLHUP)
		{
			printf("connect close by server!\n");
			break;
		}
	}
	close(clientfd);
}






