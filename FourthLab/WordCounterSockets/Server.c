#include<stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#define SIZE 128

typedef struct
{
	char mes[SIZE];
	int word_counter;
}MESSAGE;


int word_counter(const char* str)
{
	int word_counter = 0;
	int i = 0;
	int symbols_counter = 0;
	
	while(str[i] != '\0')
	{
		while(isalpha(str[i]) && (str[i] != '\0') )
		{
			i++;
			symbols_counter++;
		}
		if(symbols_counter != 0)
		{
			word_counter++;
		}
		symbols_counter = 0;
		
		while(!isalpha(str[i]) && (str[i] != '\0') )
		{
			i++;
		}
		if (str[i] == '\0')
		{
			break;
		}
	}
	return word_counter;

}

int main(int argc, char* argv[])
{
	int accepted_sock;
	int listener_sock;
	struct sockaddr_in addr;
	MESSAGE mes_struct;
	int bytes_read;
	
	listener_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listener_sock == -1)
	{
		printf("[SERVER]: There is an error with openning socket.\n");
		exit(-1);
	}
	printf("Socket ID: %d\n", listener_sock);
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1234);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(listener_sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		printf("[SERVER]: There is an error with binding sockets.\n");
		close(listener_sock);
		exit(-1);
	}
	printf("Listening...\n");
	int status = listen(listener_sock, 5);
	if(status != 0)
	{
		printf("[SERVER]: There is an error with listenning sockets.\n");
		close(listener_sock);
		exit(-1);
	}
	printf("Stop listenning!\n");
	
	while(1)
	{
		accepted_sock = accept(listener_sock, NULL, NULL);
		if(accepted_sock == -1)
		{
			printf("[SERVER]: There is an error with accepting sockets.\n");
			close(listener_sock);
			exit(-1);
		}
		printf("Access Socket ID: %d\n", accepted_sock);
		if (fork() == 0) //child
		{
			printf("Recieving...\n");
			close(listener_sock);
			bytes_read = recv(accepted_sock, (MESSAGE*)&mes_struct, sizeof(MESSAGE), 0);
			printf("Recieved...\n");
			
			if(bytes_read <= 0)
			{
				printf("[SERVER]: There is an error with receiving.\n");
				close(accepted_sock);
				exit(-1);
			}
			mes_struct.word_counter = word_counter(mes_struct.mes);
			printf("Sending...\n");
			if ( send(accepted_sock, (MESSAGE*)&mes_struct, bytes_read, 0) == -1 )
			{
				printf("[SERVER]: There is an error with sending message.\n");
			}
			printf("Sent.\n");
			return 0;
		}
		else
		{
        		close(accepted_sock);
        	}
    }

	return 0;
}
