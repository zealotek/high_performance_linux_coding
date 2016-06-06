#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

int stop = 0;

void handle_term(int sig)
{
	stop = 1;
}

int main(int argc, char **argv)
{
	signal(SIGTERM, handle_term);

	char *ip = argv[1];
	int port = atoi(argv[2]);
	int backlog = atoi(argv[3]);

	int sock = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in socket_address;
	memset(&socket_address, 0, sizeof(socket_address));
	socket_address.sin_family = AF_INET;
	inet_pton( AF_INET, ip, &socket_address.sin_addr);
	socket_address.sin_port = htons(port);
	
	int ret;
	ret = bind(sock, &socket_address, sizeof(socket_address));

	ret = listen(sock, backlog);

	while(!stop)
	{
		sleep(1);
	}

	close(sock);
	return 1;
}



