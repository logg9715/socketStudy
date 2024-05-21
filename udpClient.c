	//echo_client_UDP.c
	#include <stdio.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>

	main(int argc, char* argv[])
	{
		int     sd;
		struct  sockaddr_in s_addr;
		char    sndBuffer[BUFSIZ];
		int     n, n_send;
		int     addr_len;

		sd = socket(AF_INET, SOCK_DGRAM, 0);

		bzero(&s_addr, sizeof(s_addr));
		s_addr.sin_family = AF_INET;
		s_addr.sin_addr.s_addr = inet_addr(argv[1]);
		s_addr.sin_port = htons(atoi(argv[2]));

		connect(sd, (struct sockaddr*)&s_addr, sizeof(s_addr));

		while (1) 
		{
			fprintf(stderr, "waiting\n");

			if ((n = read(0, sndBuffer, BUFSIZ)) > 0) 
			{
				// 접속 종료
				sndBuffer[n] = '\0';
				if (!strcmp(sndBuffer, "quit\n")) break;
				
				// udp connect 전송
				if ((n_send = write(sd, sndBuffer, strlen(sndBuffer))) < 0) 
				{
					fprintf(stderr, "write error");
					exit(-3);
				}

				addr_len = sizeof(s_addr);
				if ((n = read(sd, sndBuffer, sizeof(sndBuffer))) < 0) 
				{
					fprintf(stderr, "read error\n");
					//exit(-3);
					continue;
				}
				sndBuffer[n] = '\0';
				
				fprintf(stderr, "echoed  Data : %s\n", sndBuffer);
			}
		}
		close(sd);
	}
