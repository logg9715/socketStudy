// sockpair_fork.c
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

char	data1[] = "From Parent";
char	data2[] = "From Child";

void main() {
	int	pid;
	int	sd[2], result;
	char	buf[BUFSIZ];

	result = socketpair(PF_LOCAL, SOCK_STREAM, 0, sd);

	/* 부모 프로세스에서 sd[1]을 close() 하더라도, 자식 프로세스에서
	*  참조하고 있는 이상, 해당 소켓은 사라지지 않는다. 부모에서의 sd[1]에 대한 참조만 사라진다. 
	*/
	pid = fork();
	if (pid > 0) {	// Parent Process
		close(sd[1]);
		
		read(sd[0], buf, BUFSIZ);
		printf("[parent:%d] %s\n", getpid(), buf);

		write(sd[0], data1, sizeof(data1));
		sleep(30);
		close(sd[0]);
	} else if (pid == 0) { // Child process
		close(sd[0]);

		write(sd[1], data2, sizeof(data2));

		read(sd[1], buf, BUFSIZ);
		printf("[child:%d] %s\n", getpid(), buf);
		sleep(30);
		close(sd[1]);
	} else if (pid == -1) {
		perror("fork()");
	}
}
