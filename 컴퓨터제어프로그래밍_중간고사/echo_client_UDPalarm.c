//echo_client_UDPalarm.c
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>

void sigAlarm(int);	// 인터럽트(알람)이 울리면 실행되는 함수

main(int argc, char* argv[])
{
	int     sd;
	struct  sockaddr_in s_addr;
	char    sndBuffer[BUFSIZ];
	int     n, n_send;
	int     addr_len;

	int	status;
	struct sigaction act;

	act.sa_handler = sigAlarm;	// 인터럽트가 발생하면 함수 실행
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	status = sigaction(SIGALRM, &act, 0);

	sd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&s_addr, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	s_addr.sin_port = htons(9200);

	while (1) {

		fprintf(stderr, "waiting\n");

		if ((n = read(0, sndBuffer, BUFSIZ)) > 0) 
		{

			sndBuffer[n] = '\0';
			if (!strcmp(sndBuffer, "quit\n")) break;

			printf("original  Data : %s", sndBuffer);

			if ((n_send = sendto(sd, sndBuffer, strlen(sndBuffer), 0, (struct sockaddr*)&s_addr, sizeof(s_addr))) < 0) 
			{
				fprintf(stderr, "sendto() error");
				exit(-3);
			}

			alarm(5);	// 시간이 지나면 SIGALRM 시그널을 호출함

			addr_len = sizeof(s_addr);
			fprintf(stderr, "socket recvfrom enter\n");
			if ((n = recvfrom(sd, sndBuffer, sizeof(sndBuffer), 0, NULL, NULL)) < 0) // 대기중에 시그널이 호출되면 함수가 강제종료됨
			{
				if (errno == EINTR)	// EINTR에러는 시그널로 강제 종료되었을때의 에러코드
					fprintf(stderr, "socket timeout\n");	// 결론 : 답변이 없으면 timeout이라고 표시해줌
				else
					fprintf(stderr, "recvfrom error\n");
			}
			else 
			{
				alarm(0);
				sndBuffer[n] = '\0';
				printf("echoed  Data : %s", sndBuffer);
			}
		}
	}
	close(sd);
}

void sigAlarm(int signo)
{
	fprintf(stderr, "waiting to long.....\n");
	return;
}
