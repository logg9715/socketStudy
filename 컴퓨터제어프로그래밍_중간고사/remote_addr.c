// remote_addr.c
#include        <stdio.h>
#include        <sys/socket.h>
#include        <netinet/in.h>

main()
{
	int     tcpSd;
	struct sockaddr_in s_addr;
	char *IPaddr = "192.168.0.12";

	if ( (tcpSd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket()");
		exit (-1);
	}

	bzero( (char *)&s_addr, sizeof(s_addr) );
	s_addr.sin_family = AF_INET;					// 프로토콜 종류
	inet_aton(IPaddr, &s_addr.sin_addr.s_addr);		// 문자열을 network 이진값으로 변환, 32비트 network순서의 바이너리
	s_addr.sin_port = htons(7);						// 포트번호, 16비트 network순서의 바이너리
	
	printf("host port binary : 0x%x\n", 7);	// 0x07 00로 저장, 읽을때는 00->07 = 0007 = 7

	printf("ip(dotted decial)=%s\n", IPaddr);
	printf("ip(binary       )=%x\n", s_addr.sin_addr.s_addr);
	printf("port no =%d\n", s_addr.sin_port);
	
	printf("port no (binary) =0x%x\n", s_addr.sin_port); // 0x0007으로 저장, 읽을때는 07->00 = 0700 = 700
	// 출력 : 0x700

	close(tcpSd);
}
