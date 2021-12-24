#include "SemaphorServer.h"
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<stdio.h>
//#include<errno.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#define SIZE 128

//extern int errno;

int main(int argc, char* argv[])
{
	key_t key = ftok("SemaphorServer.c", 'f');
	int semid = -1;
	int shmid = -1;
	//char* shaddr;
	MESSAGE* shaddr;
	//char equation[256];
	
	struct sembuf sops[2];

	
	if ((semid = semget(key, 2, 0666)) == 0) //INITIALIZE SEMAPHORES
	{
		printf("Removing existing semaphores...\n");
		if ((semctl(semid, 0, IPC_RMID, 0)) == -1) //DELETE SEMAPHORES
		{
			printf("[SERVER]: There is error with deleting shared memory.\n");
			return -1;
		}
	}
	
	
	if ((semid = semget(key, 2, 0666|IPC_CREAT)) == -1) //INITIALIZE SEMAPHORES
	{
		printf("[SERVER]: There is error with making semaphores.\n");
		return -1;
	}
	
	printf("SEMID: %d\n", semid);
	if ((shmid = shmget(key, sizeof(MESSAGE), 0666)) == 0) //IF EXISTS
	{
		printf("[SERVER]: Shared memory already exists. Deleting...\n");
		if ((shmctl(shmid, IPC_RMID, NULL)) == -1) //DELETE EXISTING SHARED MEMORY
		{
			printf("[SERVER]: There is error with deleting shared memory.\n");
			return -1;
		}
	}
	//printf("SHMID: %d\n", shmid);
	
	if ((shmid = shmget(key, sizeof(MESSAGE), 0666|IPC_CREAT)) == -1) //INITIALIZE SHARED MEMORY
	{
		printf("[SERVER]: There is error with making shared memory.\n");
		if (semctl(semid, 0, IPC_RMID, 0) == -1) //DELETE SEMAPHORES
		{	
			printf("[SERVER]: There is error with deleting semaphores.\n");
			return -1;
		}
		return -1;
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
			return -1;
		}
		return -1;
	}

	printf("Hello world from SERVER!\n");
	
	
	
	sops[0].sem_num = 0;
	sops[1].sem_num = 1;
	
	getsem(semid, sops, RESET_MEM);
	


	while(1)
	{
		getsem(semid, sops, SERVER_RCV_REQ);
		printf("Got message.\n");

		strcpy(shaddr->mes, "Hello from semaphores server");

		printf("Sending...\n");
		getsem(semid, sops, SERVER_SND_RPY);
		printf("Sent.\n");
	}
	
	
	if ((shmctl(shmid, IPC_RMID, NULL)) == -1) //DELETE SHARED MEMORY
	{
		printf("[SERVER]: There is error with deleting shared memory.\n");
		return -1;
	}
	
	if ((semctl(semid, 0, IPC_RMID, 0)) == -1) //DELETE SEMAPHORES
	{
		printf("[SERVER]: There is error with deleting shared memory.\n");
		return -1;
	}
	
	return 0;
}



