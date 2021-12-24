#pragma once
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include "CentralServer.h"
extern int errno;
typedef struct ClientMessage
{
	long msgtype;
	char message[SIZE];
	
} CLIENT_MES;


typedef struct ServerMessage
{
	long msgtype;
	int tmp_var;
	char message[SIZE];
		
} SERVER_MES;


void snd_msg_to_que_serv(MESSAGE* sock_mes_struct)
{
	key_t key = ftok("QueueServer.c", 'f');
	int msid = 0;
	size_t my_pid = getpid();
	CLIENT_MES my_struct_mes;
	SERVER_MES ans;
	
	if ((msid = msgget(key, 0)) == -1)
	{
		printf("[CLIENT]: There is an error with openning message queue.\n");
		return;
	}
	

	my_struct_mes.msgtype = my_pid;
	strcpy(my_struct_mes.message, sock_mes_struct->mes);
	
	if (msgsnd(msid, &my_struct_mes, sizeof(my_struct_mes), 0) == -1)
	{
		printf("[CLIENT]: There is an error with sending message.\n");
		return;
	}
	
	while (msgrcv(msid, &ans, sizeof(ans), my_pid, 0) < 0)
	{
		//printf("waiting...\n");
	}
	
	strcpy(sock_mes_struct->mes, ans.message);
	return;
}
