#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>

void main()
{
	int fd;                                      
	int sd;                                     

	fd = open("/etc/services", O_RDONLY);      
	sd = socket(PF_INET, SOCK_STREAM, 0);   

	printf("fd = [%d] \n", fd);
	printf("sd = [%d] \n", sd);
}
