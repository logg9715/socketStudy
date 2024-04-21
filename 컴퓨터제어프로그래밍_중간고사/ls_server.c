// ls_server.c
#include        <stdio.h>
#include        <netinet/in.h>
#include        <sys/socket.h>
#include        <dirent.h>

#define         PORT    9000

char    err_1[]="Directory Error";
char    rBuffer[BUFSIZ];
char	buffer[BUFSIZ] = "hello World! this is test buffer\n";

main()
{
	printf("starting...\n\n\n");
	
	int     c_socket, s_socket;
	struct  sockaddr_in s_addr, c_addr;
	int     len, length;
	int     n, i;
	char	*temp;
	
	DIR *dp;
	struct dirent *dir;

	s_socket = socket(PF_INET, SOCK_STREAM, 0);
	
	/* 문제 1. 소켓 번호 표시하기 : s_socket 값을 출력시킨다. socket()의 리턴값이다. */
	printf("1. socket is created(%d)\n", s_socket);

	memset( &s_addr, 0, sizeof(s_addr) );
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);

	if (bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) 
	{
		printf("Can not Bind\n");
		return -1;
	}
	/* 문제 2. bind()밑에 printf로 표시하기 */
	printf("2. binding is done\n");

	if(listen(s_socket, 5) == -1) 
	{
		printf("listen Fail\n");
		return -1;
	}
	/* 문제 3. listen()밑에 printf로 표시하기 */
	printf("3. socket is listening...\n");

	// [start] while
	while(1) 
	{
		len = sizeof(c_addr);
		c_socket = accept(s_socket, (struct sockaddr *)&c_addr, &len);
		/* 문제 4. accept()밑에 소켓 번호 출력, accept()의 리턴값이다. */
		printf("4. new client socket(number : %d)\n", c_socket);

		length = 0;
		temp = rBuffer;
		// [start] 버퍼 읽어들이는 부분 
		while ((n = read(c_socket, temp, 1)) > 0) 
		{
			if (*temp == '\r') continue;
			if (*temp == '\n') break;
			if (*temp == '\0') break;

			if (length == BUFSIZ) break;
			/* 문제 5. n, length, *temp 값 출력, temp는 char*이므로 *로 역참조해주고 %c를 사용한다. */
			/* n은 read()로 읽은 내용 저장하는 버퍼, read()의 버퍼크기를 1로 고정해서 무조건 1로 나옴. */
			/* length는 최종 저장될 버퍼의 길이 */
			printf("n = %d, length = %d, *temp = %c\n", n, length, *temp);
			temp++; length++;
		}
		rBuffer[length] = '\0';
		/* 문제 6. 읽어들인 rBuffer 출력 */
		printf("rBuffer : %s (length : %d)\n\n", rBuffer, length);
		// [end] 버퍼 읽어들이는 부분
		
		
		
		/* --------------------------- 여기서부터 빈칸 문제 -------------------------- */
		
		/* 문제 7. print나 ls가 입력되면 각자 명령 실행, strcmp()로 버퍼 내용을 비교 */
		// [start] if			
		if (!strcmp(rBuffer, "print"))		// case 1 : "print"
		{
			n = strlen(buffer);
			write(c_socket, buffer, n);	// write()가 응답할 소켓에 내용을 저장하는 함수
		}
		else if(!strcmp(rBuffer, "ls")) 	// case 2 : "ls"
		{
			if ((dp = opendir(".")) == NULL)
			{
				write(c_socket, err_1, strlen(err_1));
			} 
			else
			{
				while ((dir = readdir(dp)) != NULL) 
				{
					if (dir->d_ino == 0) continue;
					if (dir -> d_name[0] != '.')
					{
						write(c_socket, dir->d_name, strlen(dir->d_name));
						write(c_socket, " ", 1);
					}
				}
				closedir(dp);
			}
		}
		// [end] if
		
		close(c_socket);
	}
	// [end] while

	close(s_socket);
}
