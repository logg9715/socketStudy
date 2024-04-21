#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t childPid;

    childPid = fork();

    if (childPid > 0) {  // parent process
        // 부모 프로세스가 자식 프로세스의 종료 상태를 회수하지 않음
        sleep(10); // 부모 프로세스가 대기 상태에 있도록 sleep 호출
    }
    else if (childPid == 0) {  // Child process code 
        printf("Child process PID : %ld\n", (long)getpid());
        printf("Child process finished...\n");
        exit(0);
    }
    else {  // if fork is failed 
        perror("fork Fail! \n");
        return -1;
    }

    return 0;
}