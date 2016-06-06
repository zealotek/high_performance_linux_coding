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

	if(argc < 5)
	{
		printf("arg error!\n");
	}

	char *server_ip = argv[1];
	int server_port = atoi(argv[2]);

	int sock = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in socket_address;
	memset(&socket_address, 0, sizeof(socket_address));

	socket_address.sin_family = AF_INET;
	inet_pton( AF_INET, server_ip, &socket_address.sin_addr);
	socket_address.sin_port = htons(server_port);
	
	int ret;
	ret = connect(sock, &socket_address, sizeof(socket_address));
	if(ret < 0)
		printf("error\n");
	else	
	{
		printf("data : %s\n",argv[3]);
		printf("oob : %s\n",argv[4]);

		send(sock, argv[4], strlen(argv[4]), MSG_OOB);
	//	send(sock, argv[3], strlen(argv[3]), 0);
	}
	close(sock);
	return 1;
}



