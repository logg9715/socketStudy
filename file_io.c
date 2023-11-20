#include <stdio.h>
#include <unistd.h>

#define BUFSIZE 256

int main() 
{
	int n;
	char buf[BUFSIZE];

	fprintf(stderr, "Here is file start.\n");

	//0 = 표준 입력, 1= 표준출력
	//crtl+D로 디스크립터 닫을 때까지 표준입력(콘솔)로 받은 내용을
	//표준출력(콘솔)에 출력하는 코드
	while ((n = read(0, buf, BUFSIZE)) >0)
			write(1, buf, n);

	fprintf(stderr, "Here is file end.\n");

	exit(0);
}

