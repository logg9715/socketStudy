#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <signal.h>

void sig_alrm(int);
void send_msg(void);
void handlePing(void);
unsigned short cksum_in(unsigned short *, int);
void tv_sub(struct timeval *, struct timeval *);

struct timeval *tvsend, tvrecv;	// 시간 정보 구조체

int sd;
pid_t pid;

int nsent = 0;

struct sockaddr_in sasend;
struct sockaddr_in sarecv;
int salen;

main(int argc, char *argv[])
{
        if (argc != 2)
        {
                printf("usage : ping ip_address\n");
                exit(-1);
        }

        bzero((char *)&sasend, sizeof(sasend));
        sasend.sin_family = AF_INET;					// IPv4
        sasend.sin_addr.s_addr = inet_addr(argv[1]);	// 인자로 쓴 ip주소
        salen = sizeof(sasend);							// 크기 

        pid = getpid() & 0xffff; // ICMP ID (16 bits)

        handlePing();
}

void handlePing(void)
{
        int len, hlen, icmplen;
        struct timeval tval;
        char rbuf[1500];

        fd_set readfd;
        struct iphdr *iph;
        struct icmp *icmp;

        double rtt;

        signal(SIGALRM, sig_alrm);	// 시그널 처리기 등록

        if ((sd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)	// RAW 소켓 생성
        {
                printf("socket open error\n");
                exit(-1);
        }

        sig_alrm(SIGALRM); // SIGALRM 발생시킴

        for (;;)
        {
                if ((len = recvfrom(sd, rbuf, sizeof(rbuf), 0, NULL, NULL)) < 0)	// ICMP 에코 메시지 받음
                {
                        printf("read error\n");
                        exit(-1);
                }

                iph = (struct iphdr *)rbuf;	
                hlen = iph->ihl * 4;

				/* 여기부터 나올듯 */
                if (iph->protocol != IPPROTO_ICMP)	// ICMP인지 확인
                        return;

                if (iph->saddr == sasend.sin_addr.s_addr)	// 내가 보낸 주소가 맞는지 확인
                {
                        icmp = (struct icmp *)(rbuf + hlen);
                        icmplen = len - hlen;

                        if (icmp->icmp_type == ICMP_ECHOREPLY)	// 받은 데이터가 에코 응답인지 확인
                        {
                                if (icmp->icmp_id != pid)
                                        return;

                                gettimeofday(&tvrecv, NULL);	// 현재시간 구하기 
                                tvsend = (struct timeval *)icmp->icmp_data;	// 에코 요청시간 읽기
                                tv_sub(&tvrecv, tvsend);	// 시간 비교

                                rtt = tvrecv.tv_sec * 1000.0 + tvrecv.tv_usec / 1000.0;
                                printf("%d byte from **: seq = %u,ttl=%d, rtt = % .3fms\n", icmplen, icmp->icmp_seq, iph->ttl, rtt);
                        }
                }
        }
}

// 처음에는 강제로 호출됨, 그 후에 send_msg()랑 1초알람 걸어두고 리턴함.
// 1초 뒤에 알람 울림 -> 처리기인 aig_alrm()호출, send_msg()랑 1초 알람, 리턴 -> 1초 뒤에 다리 알람 울림...
// = 1초마다 send_msg() 호출하는 루프
void sig_alrm(int signo)
{
        send_msg();

        alarm(1);
        return;
}

void send_msg(void)
{
        int len;
        struct icmp *icmp;
        char sendbuf[1500];
        int datalen = 56;

        icmp = (struct icmp *)sendbuf;

		/* 여기 에코요청 설정하는거 나올듯 */
        icmp->icmp_type = ICMP_ECHO;
		icmp->icmp_code = 0;
		icmp->icmp_id = pid;
		icmp->icmp_seq = nsent++;
		memset(icmp->icmp_data, 0xa5, datalen);
		
		gettimeofday((struct timeval *) icmp->icmp_data, NULL);	// 보낸 시간 작성
		
		len = 8 + datalen;
		icmp->icmp_cksum = 0;
		icmp->icmp_cksum = cksum_in((unsigned short *)icmp, len);
		
		// 생성했던 raw소켓에 에코 요청 데이터 넣어서 전송
		// (sd=raw소켓, sendbuf=에코요청 데이터, len=데이터길이, 0, (struct sockaddr *)&sasend=목적지, salen=목적지데이터 길이)
        sendto(sd, sendbuf, len, 0, (struct sockaddr *)&sasend, salen);
}

void tv_sub(struct timeval *out, struct timeval *in)
{
        if ((out->tv_usec -= in->tv_usec) < 0)
        {
                --out->tv_sec;
                out->tv_usec += 1000000;
        }
        out->tv_sec -= in->tv_sec;
}

unsigned short 
cksum_in(unsigned short *addr, int len)
{
        unsigned long sum = 0;
        unsigned short answer = 0;
        unsigned short *w = addr;
        int nleft = len;

        while (nleft > 1)
        {
                sum += *w++;
                if (sum & 0x80000000)
                        sum = (sum & 0xffff) + (sum >> 16);
                nleft -= 2;
        }

        if (nleft == 1)
        {
                *(unsigned char *)(&answer) = *(unsigned char *)w;
                sum += answer;
        }

        while (sum >> 16)
                sum = (sum & 0xffff) + (sum >> 16);

        return (sum == 0xffff) ? sum : ~sum;
}
