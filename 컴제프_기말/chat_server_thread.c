// chat_server_thread.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

void *do_chat(void *);
void filter(char *string, char *word);

pthread_t thread;
pthread_mutex_t mutex;

#define MAX_CLIENT 10
#define CHATDATA 1024
#define MAX_NAME 20

#define INVALID_SOCK -1

char *INVALID_NAME = "\\";

void setUserNickname(int c_socket, int userIndex);
int findWhois(char *buff);
void parse_message(char* input, char* output);

struct user
{
	int userSocket;
	char userName[MAX_NAME];
};

struct user list_user[MAX_CLIENT];

char escape[] = "exit";
char greeting[] = "Welcome to chatting room\n";
char CODE200[] = "Sorry No More Connection\n";

main(int argc, char *argv[])
{
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;
	int nfds = 0;
	int i, j, n;
	fd_set read_fds;
	int res;
	
	// -------------------------- args ---------------------------------
	
	if (argc < 2)
	{
		printf("usage: %s port_number\n", argv[0]);
		exit(-1);
	}

	if (pthread_mutex_init(&mutex, NULL) != 0)
	{
		printf("Can not create mutex\n");
		return -1;
	}
	
	// -----------------------------------------------------------

	// -------------------------- socket init ---------------------------------
	s_socket = socket(PF_INET, SOCK_STREAM, 0);

	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(atoi(argv[1]));

	if (bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1)
	{
		printf("Can not Bind\n");
		return -1;
	}

	if (listen(s_socket, MAX_CLIENT) == -1)
	{
		printf("listen Fail\n");
		return -1;
	}

	/* 유저 리스트 공백으로 초기화 */
	for (i = 0; i < MAX_CLIENT; i++)
	{
		list_user[i].userSocket = INVALID_SOCK;
		strcpy(list_user[i].userName, INVALID_NAME);
	}

	// -----------------------------------------------------------

	// -------------------------- listen ---------------------------------
	
	while (1)
	{
		len = sizeof(c_addr);
		c_socket = accept(s_socket, (struct sockaddr *)&c_addr, &len);

		/* 만원 상태면 -1 반환 */
		res = pushClient(c_socket);
		if (res < 0)
		{
			// 에러 메시지 전송하고 연결 종료
			write(c_socket, CODE200, strlen(CODE200));
			close(c_socket);
		}
		else
		{
			// 정상 연결
			write(c_socket, greeting, strlen(greeting));
			pthread_create(&thread, NULL, do_chat, (void *)c_socket);
			fprintf(stderr, "[user connected] : %d, %s \n", list_user[res].userSocket, list_user[res].userName);
		}
	}
	
	// ------------------------------------------------------------------------------------------
}

// -------------------------------------client chat-------------------------------------
void *do_chat(void *arg)
{
	int c_socket = (int)arg;
	char chatData[CHATDATA];
	char parsedData[CHATDATA];
	int i, n;
	int userSock;

	while (1)
	{
		memset(chatData, 0, sizeof(chatData));
		if ((n = read(c_socket, chatData, sizeof(chatData))) > 0)
		{
			/* 단어 필터링*/
			filter(chatData, "fuck");
			
			if((userSock = findWhois(chatData)) == -1)
			/* start 브로드캐스트 */
			{
				for (i = 0; i < MAX_CLIENT; i++)
				{
					if (list_user[i].userSocket != INVALID_SOCK)
					{
						write(list_user[i].userSocket, chatData, n);
					}
				}
				
				if (strstr(chatData, escape) != NULL)
				{
					popClient(c_socket);
					break;
				}
			}
			/* end 브로드캐스트 */
			/* start 귓속말 */
			else 
			{
				parse_message(chatData, parsedData);
				write(list_user[userSock].userSocket, parsedData, n);
			}
			/* end 귓속말 */
		}
	}
}

// 귓속말 명령어 제거
void parse_message(char* input, char* output) 
{
    const char* startPattern = "/";
    const char* endPattern = "/";

    // 패턴의 시작과 끝을 찾기
    const char* start = strstr(input, startPattern);
    if (start) {
        start += strlen(startPattern);
        const char* end = strstr(start, endPattern);
        if (end) {
            // 패턴 전후의 문자열을 결합
            strncpy(output, input, start - input - 1);
            output[start - input - 1] = '\0';
            strcat(output, end + strlen(endPattern));
        } else {
            // 끝 패턴이 없으면 원래 문자열 복사
            strcpy(output, input);
        }
    } else {
        // 시작 패턴이 없으면 원래 문자열 복사
        strcpy(output, input);
    }
}
// --------------------------------------------------------------------------------------

// ------------------------------------manage client----------------------------------------------------
int pushClient(int c_socket)
{
	int i;

	for (i = 0; i < MAX_CLIENT; i++)
	{
		pthread_mutex_lock(&mutex);
		if (list_user[i].userSocket == INVALID_SOCK)	/* 비어있는 공간 찾기 */
		{
			list_user[i].userSocket = c_socket;
			setUserNickname(c_socket, i);
			pthread_mutex_unlock(&mutex);
			return i;
		}
		pthread_mutex_unlock(&mutex);
	}

	if (i == MAX_CLIENT)
		return -1;
}

void setUserNickname(int c_socket, int userIndex)
{
	char nameBuff[MAX_NAME];
	int n;
	
	n = read(c_socket, nameBuff, sizeof(nameBuff));
	strcpy(list_user[userIndex].userName, nameBuff);
}

int popClient(int s)
{
	int i;

	close(s);

	for (i = 0; i < MAX_CLIENT; i++)
	{
		pthread_mutex_lock(&mutex);
		if (s == list_user[i].userSocket)
		{
			list_user[i].userSocket = INVALID_SOCK;
			strcpy(list_user[i].userName, INVALID_NAME);
			pthread_mutex_unlock(&mutex);
			break;
		}
		pthread_mutex_unlock(&mutex);
	}

	return 0;
}
// -------------------------------------------------------------------------------

/* 버퍼에서 귓속말 유저 탐지, 없다면 -1 반환 (브로드캐스트) */
int findWhois(char *buff)
{
    char name[MAX_NAME];
	char *pos1, *pos2;
	int len;
	
	char *start = strchr(buff,']');
	start += 2;
	
	//fprintf(stderr, "@@ : %c\n", start[0]);
	
	if((pos1 = strchr(buff, '/')) == NULL)
		return -1;
	
	if((pos2 = strchr(pos1 + 1, '/')) == NULL)
		return -1;
	
	len = pos2 - pos1 - 1;
	if(len >= MAX_NAME) 
		return -1;
    
	strncpy(name, pos1 + 1, len);
	name[len] = '\0';
	
	for (int i = 0; i < MAX_CLIENT; i++)
	{
		if(strcmp(list_user[i].userName, name) == 0)
			return i;
	}
	
	return -1;
}

void filter(char *string, char *word)
{
	char *position;
	int wordLen = strlen(word);
	
	while ((position = strstr(string, word)) != NULL)
	{
		for(int i = 0; i < wordLen; i++)
		{
			*position = '*';
			position++;
		}
	}
}