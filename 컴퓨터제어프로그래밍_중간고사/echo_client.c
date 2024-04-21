#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 9012
#define IPADDR "127.0.0.1"

int main(int argc, char *argv[])
{
    int c_socket;
    struct sockaddr_in c_addr;
    int len;

    char sndBuffer[BUFSIZ], rcvBuffer[BUFSIZ];

    int n;
    int n_left, n_recv;

    c_socket = socket(PF_INET, SOCK_STREAM, 0);

    memset(&c_addr, 0, sizeof(c_addr));
    c_addr.sin_addr.s_addr = inet_addr(IPADDR);
    c_addr.sin_family = AF_INET;
    c_addr.sin_port = htons(atoi(argv[1]));

    if (connect(c_socket, (struct sockaddr *)&c_addr, sizeof(c_addr)) == -1)
    {
        printf("Can not connect\n");
        close(c_socket);
        return -1;
    }

    while (1)
    {
        if ((n = read(0, sndBuffer, BUFSIZ)) > 0)
        {

            sndBuffer[n] = '\0';
            if (!strcmp(sndBuffer, "quit\n"))
                break;

            printf("original  Data : %s", sndBuffer);
            if ((n = write(c_socket, sndBuffer, strlen(sndBuffer))) < 0)
            {
                return (-1);
            }

            n_left = n;
            n_recv = 0;
            while (n_left > 0)
            {
                if ((n = read(c_socket, &rcvBuffer[n_recv], n_left)) < 0)
                {
                    return (-1);
                }

                n_left = n_left - n;
                n_recv = n_recv + n;
            }

            rcvBuffer[n_recv] = '\0';
            printf("echoed Data : %s", rcvBuffer);
			
			
        }
    }
    close(c_socket);
}
