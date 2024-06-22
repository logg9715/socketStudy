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

- pthread_join(스레드객체, 스레드 종료시 반환값); : 다른 스레드가 종료될떄까지 기다림
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

