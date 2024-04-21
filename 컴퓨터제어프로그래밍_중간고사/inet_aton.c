// inet_aton.c
#include        <stdio.h>
#include        <arpa/inet.h>

main() {
   struct in_addr inp;
   const char *ipAddr ="192.168.0.10";
   char *addr;

   inet_aton(ipAddr, &inp);   // ip를 network의 big-endian형 32비트 16진수로 변환
   printf("ip(dotted decimal)[%s] -> ip(binary)[0x%x]\n", ipAddr, inp.s_addr);

   addr = inet_ntoa(inp);  // network의 16진수 주소를 ip주소 문자열로 변환하고 포인터 return
   printf("ip(binary)[0x%x] -> ip(dotted decimal)[%s]\n", ntohl(inp.s_addr), addr);
   // host에서 올바른 순서로 보기 위해 network형태의 inp를 ntohl()를 이용해 host의 long으로 바꾸고 %x포맷으로 16진수로 표시
}
