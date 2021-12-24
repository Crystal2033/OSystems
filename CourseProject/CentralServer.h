#pragma once
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include<stdio.h>
#define SIZE 128
void safely_close_socket(const int socket_des);

typedef enum {SORT_FILES, GAME} CHOICE;

typedef struct
{
	char   mes[SIZE];
	CHOICE user_choice;
}MESSAGE;

void safely_close_socket(const int socket_des)
{
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
}
