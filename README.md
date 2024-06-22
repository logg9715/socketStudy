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
