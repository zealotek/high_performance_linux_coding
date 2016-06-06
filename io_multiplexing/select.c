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

int main(int argc, char **argv)
{
	if(argc < 3)
	{
		printf("arg no good\n");
	}

	char *ip = argv[1];
	int port = atoi(argv[2]);

	int sock = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in socket_address;
	memset(&socket_address, 0, sizeof(socket_address));

	socket_address.sin_family = AF_INET;
	inet_pton( AF_INET, ip, &socket_address.sin_addr);
	socket_address.sin_port = htons(port);
	
	int ret;
	ret = bind(sock, &socket_address, sizeof(socket_address));

	ret = listen(sock, 3);

	struct sockaddr_in client_socket_addr;
	socklen_t client_socket_addr_length = sizeof(client_socket_addr);
	int socket_client = accept(sock, &client_socket_addr, &client_socket_addr_length);

	printf("accept a tcp link\n");

	if(socket_client < 0)
	{
		printf("error\n");
	}

	char buffer[255];
	memset(buffer, '\0', 255);
	fd_set read_fds;
	fd_set exception_fds;
	FD_ZERO(&read_fds);
	FD_ZERO(&exception_fds);
	while(1)
	{
		memset(buffer, '\0', 255);
		FD_ZERO(&read_fds);
		FD_ZERO(&exception_fds);
		FD_SET(socket_client, &read_fds);
		FD_SET(socket_client, &exception_fds);

		ret = select(socket_client + 1, &read_fds, NULL, &exception_fds, NULL);
		printf("select block?\n");
		if(ret < 0)
		{
			printf("select error\n");
		}
		if(FD_ISSET(socket_client, &read_fds))
		{
			ret = recv(socket_client, buffer, sizeof(buffer)-1, 0);
			printf("data:%s\n",buffer);
			//break;
		}
		else if(FD_ISSET(socket_client, &exception_fds))
		{
			recv(socket_client, buffer, sizeof(buffer)-1, MSG_OOB);
			printf("oob:%s\n",buffer);
			//break;
		}
	}
	
	close(socket_client);
	close(sock);
	return 1;
}



