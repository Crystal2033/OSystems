#include<stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
//#include <sys/un.h>
#include <netinet/in.h>
#include<stdlib.h>
#define SIZE 128

typedef struct
{
	char mes[SIZE];
	int word_counter;
}MESSAGE;


int main(int argc, char* argv[])
{
	MESSAGE mes_struct;
	int socket_des = -1;
	struct sockaddr_in addr;
	socket_des = socket(AF_INET, SOCK_STREAM, 0);
	printf("Socket ID: %d\n", socket_des);
	if (socket_des == -1)
	{
		printf("[CLIENT]: There is a problem with making socket.\n");
		exit(-1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(1234);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);


	if (connect(socket_des, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		printf("[CLIENT]: There is a problem with connecting socket.\n");
		if (close(socket_des) == -1)
		{
			printf("[CLIENT]: There is error with closing socket.\n");
		}
		exit(-1);
	}
	printf("Input some words: \n");
	fgets(mes_struct.mes, SIZE, stdin);
	printf("Sending...\n");
	send(socket_des, (MESSAGE*)&mes_struct, sizeof(MESSAGE), 0);
	printf("Sent.\n");
	printf("Recieving...\n");
	recv(socket_des, (MESSAGE*)&mes_struct, sizeof(MESSAGE), 0);
	printf("Recieved.\n");

	printf("Value of word(-s): %d\n", mes_struct.word_counter);

	if (shutdown(socket_des, 2) == -1)
	{

		printf("[CLIENT]: There is error with shutdowning socket.\n");
		if (close(socket_des) == -1)
		{
			printf("[CLIENT]: There is error with closing socket.\n");
		}
		exit(-1);
	}

	if (close(socket_des) == -1)
	{
		printf("[CLIENT]: There is error with closing socket.\n");
		exit(-1);
	}
	return 0;
}
