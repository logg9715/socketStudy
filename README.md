# 컴퓨터제어 프로그래밍 기말 정리

## Fork()
- 호출하는 쪽(부모 프로세스), 호출 되는 쪽(자식 프로세스)
- fork시, 자식 프로세스 메모리 공간 새로 할당, 부모 프로세스의 모든 데이터 복사 
- 서로 다른 프로세스 식별자, 변수 공유 X, 파이프 통신
- int fork(); : 부모=자식프로레스 pid, 자식=0, 실패=-1
- getpid(); : 현재 프로세스 pid 가져오기

## fork 서버

### fork방식 다중 접속 서버
- 부모 프로세스 : 연결 요청 처리
- 자식 프로세스 : 연결 성공 이후의 작업 처리
```C
while (1)
{
    len = sizeof(c_addr);
    if ((connSock = accept(listenSock, (struct sockaddr *)&c_addr, &len)) < 0) { /* 에러처리 */ }
    if ((pid = fork()) < 0)    // 0. fork() 생성 & 에러처리
    {
        printf("echo server can not fork()\n");
        return -1;
    }
    else if (pid > 0)        // 1. 부모 프로세스 : 연결 닫고 accept()로 돌아감
    {
        close(connSock);
        continue;
    }
    else if (pid == 0)       // 2. 자식 프로세스 : listen() 닫고 연결된 소켓 채팅 처리 담당
    {
        printf("child creat\n");
        close(listenSock);
        do_echo(connSock);
    }
}
```

### 좀비 프로세스 처리
- 자식 프로세스의 연결이 종료되면 보고가 되지 않아서 좀비 프로세스로 남음(defunct)
- 자식 프로세스 종료시에 발생하는 SIGCHLD 시그널 감지기를 넣고 종료처리를 추가해준다.
```C
/* 시그널 처리기 등록*/ 
struct sigaction act; // sigaction 구조체 선언

act.sa_handler = sigHandler; // 시그널 처리기 동작 함수 지정
sigemptyset(&act.sa_mask);   // sa 마스크 0으로 설정
act.sa_flags = 0;            // 옵션 0

state = sigaction(SIGCHLD, &act, 0); // 시그널 처리기 등록

...

void sigHandler(int sig)
{
    int pid, status;
    pid = wait(&status); /* 부모 프로세스가 자식 프로세스 종료 상태가 올때까지 대기, 종료시 반환값 반환  */
    printf("pid[%d] terminate\n", pid);
}
```
- EINTR 에러 반환으로 무한로프 도는 것 막기
```C
if ((connSock = accept(listenSock, (struct sockaddr *)&c_addr, &len)) < 0)
{
    /* accept 도중에 SIGCHLD로 처리기 실행되고 돌아오면, EINTR코드가 accept함수 값으로 반환된다. */
    if (errno == EINTR)
        continue;
    else
        printf("accept error\n");    // 그냥 accept 에러 처리
}
```

### 시그널 종류
1. SIGUP
2. SIGINT
3. SIGQUIT
4. SIGILL
5. SIGTRAP
6. SIGABRT
7. SIGBUS
8. SIGFPE
9. SIGKILL
10. SIGUSR1
11. SIGSEGV
12. SIGUSR2
13. SIGPIPE
14. SIGALRM
15. SIGTERM
17. SIGCHLD
18. SIGCONT
19. SIGSTOP
20. SIGTSTP

## 쓰레드 서버
### thread
- 하나의 프로세스에 다수의 스레드 소유 가능
- 레지스터(연산장치) & 스택 영역 독립, 그 외의 코드 데이터 소켓 등은 공유
- 시스템 호출 등으로 블록당해도 다른 스레드가 계속 작동
- 동일 프로세스 내 스레드끼리 자원&메모리 공유

- 프로세스, 스레드, 함수 비교 <br>
![image](https://github.com/logg9715/socketStudy/assets/127168700/e714cba2-7053-4fd4-8a5f-df3cec401ad7)

- pthread_create(pthread_t, 옵션, 함수, 넘길인자); : 성공시 pthread_t에 식별자 저장하고 0 반환, 실패시 에러코드 반환
```C
int sts;
pthread_t thread_id;
void *t_return; // 넘길 함수

if ((sts = pthread_create(&thread_id, NULL, func_pthread, NULL)) != 0)    // 넘길 인자가 없으면 NULL
{
    perror("pthread create error\n");
    exit(1);
}

void *func_pthread(void *arg)
{
    // 함수 내용
}
```
- A스레드가 B스레드를 생성했는데 먼저 종료됨 -> B쓰레드가 돌고 있더라도 A와 같이 종료됨 
- pthread_join(스레드객체, 스레드 종료시 반환값); : 해당 스레드가 종료될떄까지 기다림
```C
int thr_id;
pthread_t p_thread[2];

int status;
int b = 5;
int c = 6;

thr_id = pthread_create(&p_thread[0], NULL, do_sum, (void *)&b);    // 넘길인자 b : (void *)로 캐스팅하고 b의 주소 넘김
thr_id = pthread_create(&p_thread[1], NULL, do_sum, (void *)&c);

pthread_join(p_thread[0], (void **)&status);
pthread_join(p_thread[1], (void **)&status);
```

- join 예제 (출력결과 시험에 나올수도)
```C
void *do_sum(void *data)
{
    int i;
    int sum = 0;

    int max = *((int *)data);

    for (i = 0; i < max; i++)
    {
        sum = sum + i;
        printf("%d - Add %d\n", max, i);
        sleep(1);
    }
    printf("%d - sum(%d)\n", max, sum);
}

int main()
{
    // 대충 변수선언
    thr_id = pthread_create(&p_thread[0], NULL, do_sum, (void *)&a);
    thr_id = pthread_create(&p_thread[1], NULL, do_sum, (void *)&b);
    thr_id = pthread_create(&p_thread[2], NULL, do_sum, (void *)&c);

    pthread_join(p_thread[0], (void **)&status);
    pthread_join(p_thread[1], (void **)&status);
    pthread_join(p_thread[2], (void **)&status);

    printf("programing is end\n");
    return 0;
}
/*******************************출력 내용***************************************
* 4 - Add 0                                                                   *
* 6 - Add 0                                                                   *
* 5 - Add 0                                                                   *
* 4 - Add 1                                                                   *
* 6 - Add 1                                                                   *
* 5 - Add 1                                                                   *
* 4 - Add 2                                                                   *
* 6 - Add 2                                                                   *
* 5 - Add 2                                                                   *
* 4 - Add 3                                                                   *
* 6 - Add 3                                                                   *
* 5 - Add 3                                                                   *
* 4 - sum(6)                                                                  *
* 5 - Add 4                                                                   *
* 6 - Add 4                                                                   *
* 5 - sum(10)                                                                 *
* 6 - Add 5                                                                   *
* 6 - sum(15)                                                                 *
* programing is end                                                           *
******************************************************************************/
```

- 뮤텍스 : 전역변수의 '임계 영역'문제 해결용, 여러명이 동시에 변수 접근할때 겹치는 문제 막기
```C
pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;    // 뮤텍스 초기화

pthread_mutex_lock(&mutex);    // 뮤텍스 잠그기
/* 명령 수행... */
pthread_mutex_unlock(&mutex);    // 뮤텍스 잠금 풀기
```

### 채팅서버에서 mutex가 필요한 경우
- 연결된 클라이언트 소켓을 전역변수 배열에 넣고 관리할 때 필요
- 클라이언트가 연결되면 배열에 추가되고, 접속이 끊기면 삭제한다.
- 뮤텍스가 없으면 추가&삭제에 오류가 생길수도 있고 귓속말 등이 이상한 곳으로 전송될 수도 있다.

## TCP 내부 동작
- 일련번호(Sequene Number) : 순서대로 데이터를 보내기 위한 일련번호
- 확인번호(Acknowledgement Number) : 일련번호에 대한 응답용
- 일련번호로 먼저 번호 들어옴 -> ack번호에 일련번호+1로 응답하는 구조
<br>

### 연결 준비 단계 (3-way-handshaking) : connect()
    
![화면 캡처 2024-06-22 215247](https://github.com/logg9715/socketStudy/assets/127168700/34602903-89b1-4ac5-a087-f3ddbf0b4992)

0. 서버는 bind(), listen()호출해서 LISTEN상태로 변함, accept() 호출해서 블록 상태로 클라이언트 기다림
1. SYN : 클라이언트가 SYN비트 1 패킷 전송, SYN_SEND상태가 됨.
2. SYN_ACK : 서버가 SYN비트 1 ACK비트 1 패킷 전송, SYN_RCVD상태가 됨.
3. ACK : 클라가 확인 의미로 ACK비트 1 패킷 전송, ESTABLISHED(송수신 가능)상태가 됨, 서버도 이걸 받고 ESTABLISHED상태가 됨.

### 연결 종료 단계 (4-way-handshaking) : close()
    
![image](https://github.com/logg9715/socketStudy/assets/127168700/97dcc659-2e07-4e28-91da-327ebb2abb93)

0. 클라이언트가 종료 시작한 상황
1. FIN : 클라가 FIN 패킷 전송, FIN_WAIT_1상태로 대기
2. ACK : 서버가 수신한 일련번호+1로 확인번호 쓴 ACK 패킷 전송, CLOSE_WAIT상태 들어감, 서버 작업 정리 시작
3. FIN : 서버가 모든 작업 끝내고 close()호출, 서버가 일련번호+1+1을 확인번호 쓴 FIN패킷 전송
4. ACK : 대기하던 클라가 FIN을 수신하면 FIN_WAIT_2상태로 변함, 수신한 번호+1의 확인번호 쓴 ACK 패킷 전송, 일정시간동안 TIME_WAIT로 대기 후 나중에 종료
5. 서버는 ACK를 받자마자 바로 CLOSE상태 됨.

## 소켓 옵션 변경
- getsockopt(소켓번호, 코드 모듈 타입, 읽을 옵션이름, 옵션값 저장할 인자, 옵션값의 메모리 크기); : 현재 옵션 확인, 성공시 0 실패시 -1 반환
- setsockopt(인자 같음); : 옵션 바꾸기, 성공시 0 실패시 -1 반환

### linger 추가 (SO_LINGER) : close()할때 출력 스트림에 남아있는 데이터 처리 방식 
- l_onoff = 0 : close()즉시 return, 종료가 어떻게되는지 모름
- l_onoff = 1, l_linger = 0 : 즉시 리턴, 출력 스트림에 남은 데이터 버림, 비정상 종료
- l_onoff = 1, l_linger = 0아닌 정수 : 정상종료까지 리턴하지 않음, l_linger의 시간까지 기다리고 넘으면 비정상종료 진행

```C
// linger 옵션 추가
struct linger optval;
optval.l_onoff = 1;
optval.l_linger = 10;
setsockopt(sock, SOL_SOCKET, SO_LINGER, (char *)&optval, sizeof(optval));
```

### 송수신 버퍼 길이 바꾸기 (SO_RCVBUF, SO_SNDBUF)
- 송수신 버퍼 길이를 넘은 데이터는 수신측에서 짤림
- connect()나 listen() 전에 수정해야 적용됨, accept()로 얻은 소켓에 수정해도 적용 안됨.
```
// 버퍼 길이 2배로 늘리기
int optval_;
int optlen_ = sizeof(optval_);
getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&optval_, &optlen_);

optval_ *= 2;
setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&optval_, sizeof(optval_));
```

## RAW소켓 

### RAW소켓 생성방법
- sd = socket(AF_INET, SOCK_RAW, 프로토콜 종류);
- AF_INET : IPv4 국룰 옵션
- 프로토콜 종류 : IPPROTO_ICMP(핑), IPPROTO_TCP(tcp), IPPROTO_UDP(udp)

### 무차별 모드
- NIC(Network Interface Card)는 기본적으로 자신 mac으로 온 데이터가 아니면 폐기함.
- 무차별 모드는 모든 mac에 대한 데이터를 읽을 수 있음, 부하 문제 때문에 기본적으로 꺼져있다.
> wlan0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
- sudo ifconfig 카드종류(wlan0) promisc : 무차별 모드 켤 수 있음, - 붙이면 꺼짐
> wlan0: flags=4419<UP,BROADCAST,RUNNING,PROMISC,MULTICAST>  mtu 1500 // running 뒤에 promisc옵션이 보이게 된다. 

### raw소켓으로 전송받은 데이터 읽기
```C
// read_packet.c
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

void print_ip(struct iphdr *);
void print_tcp(struct tcphdr *);

main()
{
	int sd;
	int len;

	int rx_packet_size = sizeof(struct iphdr) + sizeof(struct tcphdr);
	char *rx_packet = malloc(rx_packet_size);

	struct tcphdr *rx_tcph;
	struct iphdr *rx_iph;

	struct in_addr s_addr, d_addr;
	struct sockaddr_in local, remote;

	struct servent *serv;

	if ((sd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)	// raw소켓 생성
	{
		printf("socket open error\n");
		exit(-1);
	}

	while (1)
	{
		bzero(rx_packet, rx_packet_size);

		len = sizeof(local);
		/* rx_packet버퍼로 자료를 받아옴. */
		if (recvfrom(sd, rx_packet, rx_packet_size, 0x0, (struct sockaddr *)&local, &len) < 0)
		{
			printf("recvfrom error\n");
			exit(-2);
		}
		
		// raw소켓 정보 읽을 헤더 위치 맞추기
		rx_iph = (struct iphdr *)(rx_packet);
		rx_tcph = (struct tcphdr *)(rx_packet + rx_iph->ihl * 4);

		print_ip(rx_iph);	// ip정보 읽기, 대체로 4,5,6 나옴
		print_tcp(rx_tcph);	// 포트정보 + 패킷 ACK, SYN같은 제어비트 내용 읽기
	}

	close(sd);
}

void print_ip(struct iphdr *iph)
{
	printf("[IP  HEADER] VER: %1u HL : %2u Protocol : %3u ", iph->version, iph->ihl, iph->protocol);
	printf("SRC  IP: %x ", iph->saddr);
	printf("DEST IP: %x \n", iph->daddr);
}

void print_tcp(struct tcphdr *tcph)
{
	printf("[TCP HEADER] src port: %5u dest port : %5u  ", ntohs(tcph->source), ntohs(tcph->dest));

	(tcph->urg == 1) ? printf("U") : printf("-");
	(tcph->ack == 1) ? printf("A") : printf("-");
	(tcph->psh == 1) ? printf("P") : printf("-");
	(tcph->rst == 1) ? printf("R") : printf("-");
	(tcph->syn == 1) ? printf("S") : printf("-");
	(tcph->fin == 1) ? printf("F") : printf("-");
	printf("\n\n");
}
```
- 출력 결과
    
![image](https://github.com/logg9715/socketStudy/assets/127168700/35fa42c8-3dc7-4830-b194-b99c2ed710bb)
    
### my_ping.c 
- RAW 소켓을 통해 ICMP 에코 요청 메시지를 보내 핑체크 실행
```C
// my_ping.c

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
```

## 웹서버

### http 규격
- Request Message : Request Line, Request Header, Entity Header, CRLF(Blank Line), Body
- Response Message : Status Line, General Header, Response Header, Entity Header, CRLF(Blank Line), Body
    
- Request Line : 전송방식(get, post...) + url + http 버젼
- Request Header : 추가 정보나 클라 정보를 서버에 전송 목적, 응답 유형이나 보안 인증 등
- General Header : 전송에 필요한 일반 정보, Date, Connection, Cache-Control
- Response Header : Status 헤더 외의 추가정보
- Entity Header : Body에 대한 정보, 컨텐츠 타입이나 인코딩이나 길이 등
- Status Line : 요청에 대한 성공이나 에러 코드 (200:성공, 201:post성공) (400:요청문법오류, 404:url오류, 500:서버 프로그램 오류)

### 웹서버 코드
- 클라가 보낸 Request Message의 Request Line 데이터 추출
- strtok(*문자열, 자를문자); : 자를문자를 종료문자로 바꾸고 헤더 포인터 반환
- 문자열이 null이면, 그전에 잘랐던 문자에서 다음 자를문자를 찾고 새로운 헤더 포인터 반환
```C
// Request Line 보통 형태 : "get /index.html HTTP/1.0"

if ((n = read(c_sock, rcvBuf, BUFSIZ)) <= 0)
    web_log(ERROR, "ERROR", "can not receive data from web browser", n);

web_log(LOG, "REQUEST", rcvBuf, n);

p = strtok(rcvBuf, " ");
if (strcmp(p, "GET") && strcmp(p, "get"))	// get방식인지 확인
    web_log(ERROR, "ERROR", "Only get method can support", 0);

p = strtok(NULL, " ");
if (!strcmp(p, "/"))	// url에 /index.html없이 그냥 ip만 쳤을때(그 경우엔 url에 "/"만 있음) 인덱스 페이지로 연결해줌
    sprintf(uri, "%s/index.html", documentRoot);	// 미리 문서루트 지정한 /home/pi/어쩌구...+url로 받은 주소 /index.html 합성 -> /home/pi.../index.html
else
    sprintf(uri, "%s%s", documentRoot, p);		// 그냥 문자열 그대로 붙임

p = strtok(NULL, "\r\n "); 	// http 버젼 읽기
```
    
- web_log()에서 mutex사용 : 스레드 방식 사용시
- 다수의 스레드에서 동시에 로그파일을 열고 작성할 경우 충돌 방지
```C
void web_log(int type, char s1[], char s2[], int n)
{
	int log_fd;
	char buf[BUFSIZ];

	if (type == LOG)
	{
		sprintf(buf, "STATUS %s %s %d\n", s1, s2, n);
	}
	else if (type == ERROR)
	{
		sprintf(buf, "ERROR %s %s %d\n", s1, s2, n);
	}

	/* 여기 나올듯 */
	pthread_mutex_lock(&mutex);	// 뮤텍스를 얻어야만 파일을 열고 로그를 쓸 수 있음 
	if ((log_fd = open("web.log", O_CREAT | O_WRONLY | O_APPEND, 0644)) >= 0)
	{
		write(log_fd, buf, strlen(buf));
		close(log_fd);
	}
	pthread_mutex_unlock(&mutex);

	if (type == ERROR)
		exit(-1);
}
```


## 채팅 프로그램
- 귓속말, 필터 업그레이드
- push_client()에서 mutex쓴 이유
