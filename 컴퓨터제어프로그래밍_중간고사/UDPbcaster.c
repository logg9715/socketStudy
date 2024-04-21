//UDPbcaster.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>

main(int argc, char* argv[])
{
	int     sd;
	struct  sockaddr_in s_addr;
	char    sndBuffer[BUFSIZ];
	int     n, n_send, status;

	int	on = 1;

	if (argc != 3) 
	{
		fprintf(stderr, "usage: UDPbacster broadcast_ip_addr port\n");
		exit(-1);
	}

	sd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&s_addr, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = inet_addr(argv[1]);
	s_addr.sin_port = htons(atoi(argv[2]));

	if ((status = setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on))) != 0) 
	{
		printf("setsockopt error\n");
		exit(-1);
	}

	while (1) 
	{
		fprintf(stderr, "Type broadcasting data : ");

		if ((n = read(0, sndBuffer, BUFSIZ)) > 0) 
		{

			sndBuffer[n - 1] = '\0';

			if ((n_send = sendto(sd, sndBuffer, strlen(sndBuffer), 0, (struct sockaddr*)&s_addr, sizeof(s_addr))) < 0) 
			{
				fprintf(stderr, "sendto() error");
				exit(-3);
			}

			if (!strncmp(sndBuffer, "quit", 4)) break;
		}
	}
	close(sd);
}
