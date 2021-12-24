#pragma once
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#define SIZE 128

#include "CentralServer.h"
typedef enum {CLIENT_GET_MEM ,CLIENT_SND_REQ, SERVER_RCV_REQ, SERVER_SND_RPY, CLIENT_RCV_RPY, RESET_MEM} STATUS;
void getsem(const int semid, struct sembuf* sops, STATUS status);

union semun
{
        int val;
        struct semid_ds *mbuf;
        ushort * array;
} arg;


void getsem(const int semid, struct sembuf* sops, STATUS status)
{
	sops[0].sem_num = 0;
	sops[1].sem_num = 1;

	printf("Set statuses.\n");
	switch(status)
	{
		case CLIENT_GET_MEM:
		{
			printf("CLIENT_GET_MEM.\n");
			sops[0].sem_op = 0;
			sops[1].sem_op = -1;
			break;
		}
		case CLIENT_SND_REQ:
		{
			printf("CLIENT_SND_REQ.\n");
			sops[0].sem_op = 1;
			sops[1].sem_op = 0;
			break;
		}
		case SERVER_RCV_REQ:
		{
			printf("SERVER_RCV_REQ.\n");
			sops[0].sem_op = -1;
			sops[1].sem_op = 0;
			break;
		}
		case SERVER_SND_RPY:
		{
			printf("SERVER_SND_RPY.\n");
			sops[0].sem_op = 1;
			sops[1].sem_op = 1;
			break;
		}
		case CLIENT_RCV_RPY:
		{
			printf("CLIENT_RCV_RPY.\n");
			sops[0].sem_op = -1;
			sops[1].sem_op = -1;
			break;
		}
		case RESET_MEM:
		{
			printf("RESET_MEM.\n");
			sops[0].sem_op = 0;
			sops[1].sem_op = 1;
			break;
		}
	}
	ushort sem_value[2];
	arg.array = sem_value;
	semctl(semid, 0, GETALL, arg);
	printf("Semaphores values: s[0]: %d, s[1]: %d\n", sem_value[0], sem_value[1]);
	printf("PID of last process to change the value of [0] semaphore:  %d\n", semctl(semid, 0, GETPID, arg));
	printf("PID of last process to change the value of [1] semaphore:  %d\n", semctl(semid, 1, GETPID, arg));
	printf("Quantity of processes waiting when [0] semaphor is increasing:  %d\n", semctl(semid, 0, GETNCNT, arg));
	printf("Quantity of processes waiting when [1] semaphor is increasing:  %d\n", semctl(semid, 1, GETNCNT, arg));
	printf("Quantity of processes waiting when [0] semaphor will become equal to 0:  %d\n", semctl(semid, 0, GETZCNT, arg));
	printf("Quantity of processes waiting when [1] semaphor will become equal to 0:  %d\n", semctl(semid, 1, GETZCNT, arg));
	printf("Making semop...\n");
	printf("Send request: %d, %d\n", sops[0].sem_op, sops[1].sem_op);
	if(semop(semid, sops, 2) == -1)
	{
		printf("There is an error with semaphor operations.\n");
	}

	return;
}


void snd_msg_to_sem_serv(MESSAGE* mes_struct)
{

	key_t key = ftok("SemaphorServer.c", 'f');

	int semid = -1;
	int shmid = -1;
	MESSAGE* shaddr = (MESSAGE*)(malloc(sizeof(MESSAGE)));
	//char* shaddr = NULL;
	char eqaution[100];
	struct sembuf sops[2];
	
	
	if ((semid = semget(key, 2, 0)) == -1) //OPEN SEMAPHORES
	{
		printf("[CLIENT]: There is error with making semaphores. Need to start SERVER first.\n");
		exit(-1);
	}
	
	if ((shmid = shmget(key, sizeof(MESSAGE), 0)) == -1) //OPEN SHARED MEMORY
	{
		printf("[CLIENT]: There is error with making shared memory. Need to start SERVER first.\n");
		exit(-1);
	}
	//printf("SHMID: %d\n", shmid);
	if ((shaddr = (MESSAGE*)shmat(shmid, NULL, 0)) == (MESSAGE*)-1) //CONNECT TO SHARED MEMORY
	{
		printf("address: %p\n", shaddr);
		printf("[SERVER]: There is error with connecting to shared memory.\n");
		printf("DELETING...\n");
		if ((semctl(semid, 0, IPC_RMID, 0)) == -1) //DELETE SEMAPHORES
		{
			printf("[SERVER]: There is error with deleting semaphores.\n");
			exit(-1);
		}
		exit(-1);
	}
	printf("Waiting...\n");
	
	printf("Hello world from SOCKET SERVER!\n");
	
	getsem(semid, sops, CLIENT_GET_MEM);
	
	strcpy(shaddr->mes, mes_struct->mes);
	printf("Sending...\n");

	getsem(semid, sops, CLIENT_SND_REQ);
	printf("Sent.\n");
	
	getsem(semid, sops, CLIENT_RCV_RPY);
	printf("Got.\n");
	strcpy(mes_struct->mes, shaddr->mes);

	getsem(semid, sops, RESET_MEM);
	
	return;

}




