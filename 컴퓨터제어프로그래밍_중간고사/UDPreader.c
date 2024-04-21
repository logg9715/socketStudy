//UDPreader.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

main(int argc, char* argv[])
{
	int     sd;
	struct  sockaddr_in s_addr, c_addr;
	char    rcvBuffer[BUFSIZ];
	int     n, n_send;
	int     addr_len;

	int	status;
	int	on = 1;

	if (argc != 2) 
	{
		printf("usage: UDPreader port\n");
		exit(-1);
	}

	sd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&s_addr, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = INADDR_ANY;
	s_addr.sin_port = htons(atoi(argv[1]));

	if (bind(sd, (struct sockaddr*)&s_addr, sizeof(s_addr)) != 0) 
	{
		fprintf(stderr, "bind error\n");
		exit(-2);
	}

	while (1) 
	{
		addr_len = sizeof(s_addr);
		if ((n = recvfrom(sd, rcvBuffer, sizeof(rcvBuffer), 0, (struct sockaddr*)&c_addr, &addr_len)) < 0) 
		{
			fprintf(stderr, "recvfrom error");
			exit(-3);
		}
		rcvBuffer[n] = '\0';

		if (!strncmp(rcvBuffer, "quit", 4))
			break;
		
		/* 여기 수정하는거 문제로 나올 수 있을듯 */
		printf(" Broadcated  from (%x) : [%s]\n", htonl(c_addr.sin_addr.s_addr), rcvBuffer);
		//printf(" Broadcated  from (%s) : [%s]\n", inet_ntoa(c_addr.sin_addr.s_addr), rcvBuffer);
	}
	close(sd);
}
