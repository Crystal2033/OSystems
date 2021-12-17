#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/shm.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
//#define SIZE 256
typedef enum {CLIENT_GET_MEM ,CLIENT_SND_REQ, SERVER_RCV_REQ, SERVER_SND_RPY, CLIENT_RCV_RPY, RESET_MEM} STATUS;
enum {CLIENT, SERVER};
union semun{
	int val;
	struct semid_ds *mbuf;
	ushort * array;
} arg;

void getsem(const int semid, struct sembuf* sops, STATUS status)
{
	sops[0].sem_num = 0;
	sops[1].sem_num = 1;
	switch(status)
	{
		case CLIENT_GET_MEM:
		{
			sops[CLIENT].sem_op = 0;
			sops[SERVER].sem_op = -1;
			break;
		}
		case CLIENT_SND_REQ:
		{
			sops[CLIENT].sem_op = 1;
			sops[SERVER].sem_op = 0;
			break;
		}
		case SERVER_RCV_REQ:
		{
			sops[CLIENT].sem_op = -1;
			sops[SERVER].sem_op = 0;
			break;
		}
		case SERVER_SND_RPY:
		{
			sops[CLIENT].sem_op = 1;
			sops[SERVER].sem_op = 1;
			break;
		}
		case CLIENT_RCV_RPY:
		{
			sops[CLIENT].sem_op = -1;
			sops[SERVER].sem_op = -1;
			break;
		}
		case RESET_MEM:
		{
			sops[CLIENT].sem_op = 0;
			sops[SERVER].sem_op = 1;
			break;
		}
	}
	//ushort sem_value[2];
	//arg.array = sem_value;
	//semctl(semid, 0, GETALL, arg);
	//printf("Semaphores values: s[0]: %d, s[1]: %d\n", sem_value[0], sem_value[1]);
	//printf("PID of last process to change the value of [0] semaphore:  %d\n", semctl(semid, 0, GETPID, arg));
	//printf("PID of last process to change the value of [1] semaphore:  %d\n", semctl(semid, 1, GETPID, arg));
	//printf("Quantity of processes waiting when [0] semaphor is increasing:  %d\n", semctl(semid, 0, GETNCNT, arg));
	//printf("Quantity of processes waiting when [1] semaphor is increasing:  %d\n", semctl(semid, 1, GETNCNT, arg));
	//printf("Quantity of processes waiting when [0] semaphor will become equal to 0:  %d\n", semctl(semid, 0, GETZCNT, arg));
	//printf("Quantity of processes waiting when [1] semaphor will become equal to 0:  %d\n", semctl(semid, 1, GETZCNT, arg));
	//printf("Making semop...\n");
	//printf("Send request: %d, %d\n", sops[0].sem_op, sops[1].sem_op);
	if(semop(semid, sops, 2) == -1)
	{
		printf("There is an error with semaphor operations.\n");
	}
	//else
	//{printf("OK!!!\n");}
	return;
}

typedef struct Equ_and_res
{
	char equation_str[256];
	double result;
} EQU_AND_RES;

int main(int argc, char* argv[])
{
	key_t key = 1234;
	//could be ftok
	int semid = -1;
	int shmid = -1;
	EQU_AND_RES* shaddr = (EQU_AND_RES*)(malloc(sizeof(EQU_AND_RES)));
	//char* shaddr = NULL;
	char eqaution[100];
	struct sembuf sops[2];
	
	
	if ((semid = semget(key, 2, 0)) == -1) //OPEN SEMAPHORES
	{
		printf("[CLIENT]: There is error with making semaphores. Need to start SERVER first.\n");
		return -1;
	}
	
	if ((shmid = shmget(key, sizeof(EQU_AND_RES), 0)) == -1) //OPEN SHARED MEMORY
	{
		printf("[CLIENT]: There is error with making shared memory. Need to start SERVER first.\n");
		return -1;
	}
	//printf("SHMID: %d\n", shmid);
	if ((shaddr = (EQU_AND_RES*)shmat(shmid, NULL, 0)) == (EQU_AND_RES*)-1) //CONNECT TO SHARED MEMORY
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
	printf("Waiting...\n");
	getsem(semid, sops, CLIENT_GET_MEM);
	
	
	
	
	printf("Hello world from CLIENT!\n");
	char exit_str[50];
	char* endptr;
	while(strcmp(exit_str, "\n") != 0)
	{
		printf("Input an expression to solve: \n");
		exit_str[0] = '\0';
		fgets(exit_str, 50, stdin);
		strcpy(shaddr->equation_str, exit_str);
		printf("Sending...\n");
		
		getsem(semid, sops, CLIENT_SND_REQ);
		printf("Sent.\n");
		
		getsem(semid, sops, CLIENT_RCV_RPY);
		printf("Got.\n");
		if(strcmp(exit_str, "\n") != 0)
		{
			printf("Result: %f\n", shaddr->result);
		}
		
		//printf("Result of strcmp: %d, string: %s\n", strcmp(exit_str, "exit"), exit_str);
	}
	getsem(semid, sops, RESET_MEM);
	exit_str[0] = '\0';
	
	
	return 0;
}
