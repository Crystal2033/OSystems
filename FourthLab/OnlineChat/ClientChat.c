#include<stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#define SIZE_IP_PORT 30
#define SIZE_MES 256
#define SIZE_NAME 20



int get_port_and_id(const char* str, char* ip_addr) // 127.0.0.1:21342
{	
	int i = 0;

	while(str[i] != ':' && str[i] != '\0')
	{
		ip_addr[i] = str[i];
		i++;
	}
	if(str[i] == '\0')
	{
		printf("There is an error with IP address.\n");
		exit(-1);
	}
	ip_addr[i] = '\0';
	return atoi(str + i + 1);
	
}
extern int errno;

typedef struct
{
	char mes[SIZE_MES];
	char name[SIZE_NAME];
}LETTER;


int main(int argc, char* argv[])
{
	char   curr_ip[SIZE_IP_PORT];
	int    my_port = -1;
	int    friend_port = -1;
	LETTER mes_struct_send;
	LETTER mes_struct_get;
	int    socket_des = -1;
	struct sockaddr_in my_addr;
	struct sockaddr_in friend_addr;
	struct sockaddr_in tmp_addr;
	char   mes_buffer[SIZE_MES];


	if (argc != 5)
	{
		printf("There is incorrect value of arguments.\n");
		exit(-1);
	}
	if ( strcmp(argv[1], "-a") || strcmp(argv[3], "-u") )
	{
		printf("There is error with your arguments. Check it out.\n");
		exit(-1);
	}
	
	
	strcpy(mes_struct_send.name, argv[4]);

	my_port = get_port_and_id(argv[2], curr_ip);
	
	socket_des = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_des == -1)
	{
		printf("[CLIENT]: There is a problem with making socket.\n");
		exit(-1);
	}
	
	struct timeval read_timeout;
	read_timeout.tv_sec = 0;
	read_timeout.tv_usec = 10;
	setsockopt(socket_des, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);
	
	
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(my_port);
	inet_pton(AF_INET, curr_ip, &my_addr.sin_addr); 
	
	if (bind(socket_des, (struct sockaddr*) &my_addr, sizeof(my_addr)) == -1)
	{
		printf("Errno = %d\n", errno);
		printf("There is an error with binding sockets.\n");
		close(socket_des);
		exit(-1);
	}
	
	printf("Please input your friend`s port.\n");
	scanf("%d", &friend_port);
	getchar();
	
	friend_addr.sin_family = AF_INET;
	friend_addr.sin_port = htons(friend_port); 
	inet_pton(AF_INET, curr_ip, &friend_addr.sin_addr);
	
	printf("---------------------------------------------------------------------\n");
	printf("You can input your messages whenever you want. Input \"quit\" for exit.\n");
	printf("---------------------------------------------------------------------\n");

		if(fork() == 0)
		{
			while(1)
			{
				mes_buffer[0] = '\0';
				fgets(mes_buffer, SIZE_MES, stdin);
				printf("Me: %s\n", mes_buffer);
				if (strcmp(mes_buffer, "quit\n") == 0 )
				{	
					strcpy(mes_struct_send.mes, "User has disconnected from the server.\n");
					if (sendto(socket_des, (LETTER*) &mes_struct_send, sizeof(mes_struct_send), 0, (struct sockaddr*) &friend_addr, sizeof(friend_addr)) == -1)
					{
						printf("There is an error with sending message.\n");
						shutdown(socket_des, SHUT_RDWR);
						close(socket_des);
						exit(-1);
					}
					break;
				}
				
				strcpy(mes_struct_send.mes, mes_buffer);
			
			
				if (sendto(socket_des, (LETTER*) &mes_struct_send, sizeof(mes_struct_send), 0, (struct sockaddr*) &friend_addr, sizeof(friend_addr)) == -1)
				{
					printf("There is an error with sending message.\n");
					shutdown(socket_des, SHUT_RDWR);
					close(socket_des);
					exit(-1);
				}
			}
			kill(getppid(),SIGTERM);
			return 0;
		}
		else
		{
		
			while(1)
			{
				int length = 0;
				if (recvfrom(socket_des, (LETTER*) &mes_struct_get, sizeof(mes_struct_get), MSG_WAITALL, (struct sockaddr*) &tmp_addr, &length) > 0 )
				{
					printf("%s: %s\n", mes_struct_get.name, mes_struct_get.mes);
				}
				
				
			}
		}
		
	shutdown(socket_des, SHUT_RDWR);
	close(socket_des);

	return 0;
}
