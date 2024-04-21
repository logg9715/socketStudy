// gethostbyname.c
/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
* 올바른 서버 주소(url이나 ip)를 입력하면                                     *
*                                                                             *
* 여러가지 형태로 분석 & 출력해줌.                                            *
*                                                                             *
* 올바르지 않은 형태일 경우, 에러 반환                                        *
*                                                                             *
*                                                                             *
*                                                                             *
******************************************************************************/

#include 	<stdio.h>
#include    <netdb.h>
#include	<arpa/inet.h>
#include    <sys/socket.h>

main(int argc, char *argv[]) {
	struct in_addr addr;
	struct hostent *host;
	const char *hostName = argv[1];
	int i;

	if ( (host = gethostbyname(hostName)) == NULL) 
	{
		printf("gethostbyname() error - check network\n");
		exit(-1);
	}

	printf("official name = %s\n", host->h_name);	// e6030.a.akamaiedge.net

	i = 0;
	while( host->h_aliases[i] != NULL ) 
	{
		printf("aliases = %s\n", host->h_aliases[i++]);	// www.naver.com www.naver.com.nheos.com
	}

	printf("address type = %d\n", host->h_addrtype);	// 2
	printf("address length = %d\n", host->h_length);	// 4

	i = 0;
	while (host->h_addr_list[i] != NULL) 
	{
		memcpy(&addr.s_addr, host->h_addr_list[i], 4);
		printf("address = %s(0x%x)\n", inet_ntoa(addr), ntohl(*(long *)host->h_addr_list[i]));
		// 23.53.224.26(0x1735e01a)
		i++;
	}
}
