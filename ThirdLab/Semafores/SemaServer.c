#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<stdio.h>
#include<sys/shm.h>
#include<errno.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#define SIZE 256

extern int errno;

typedef struct Equ_and_res
{
	char equation_str[256];
	double result;
} EQU_AND_RES;

double find_answer(const char* equation)
{
	double first_number = 0.0, second_number = 0.0;
	double result = 0.0;
	char operation;
	char* endptr;
	//printf("%s\n", equation);
	first_number = strtod(equation, &endptr);
	while (*endptr == 0 || *endptr == ' ')
	{
		endptr++;
	}
	operation = *endptr;
	endptr++;
	while (*endptr == 0 || *endptr == ' ')
	{
		endptr++;
	}
	second_number = strtod(endptr, &endptr);
	switch (operation)
	{
	case '+':
	{
		result = first_number + second_number;
		break;
	}
	case '-':
	{
		result = first_number - second_number;
		break;
	}
	case '*':
	{
		result = first_number * second_number;
		break;
	}
	case '/':
	{
		if (second_number == 0)
		{
			printf("Division by zero error. In this case answer will be equal 0. Check better all zero answers and equations!!!\n");
			return 0;
		}
		result = first_number / second_number;
		break;
	}
	default:
	{
		printf("Error operation.\n");
		break;
	}

	}
	return result;
}

int parse_equation_str(char* equation_str)
{
	if(strlen(equation_str) == 0)
	{
		printf("You have forgotten to input an equation.\n");
		return 0;
	}
	
	char* char_ptr = NULL;
	char_ptr = equation_str;
	int i = 0;
	while(*char_ptr != '\n')
	{
		if(*char_ptr == 0)
		{
			break;
		}
		if(!(isdigit(*char_ptr) || ispunct(*char_ptr)))
		{
			if(!isspace(*char_ptr))
			{
				printf("Your equation can`t consists symbols.\n");
				return 0;
			}
		}
		if(!isspace(*char_ptr))
		{
			
			equation_str[i++] = *char_ptr;
		}
		char_ptr++;
	}
	equation_str[i] = '\0';
	return 1;

}
typedef enum {CLIENT_GET_MEM ,CLIENT_SND_REQ, SERVER_RCV_REQ, SERVER_SND_RPY, CLIENT_RCV_RPY, RESET_MEM} STATUS;

union semun{
        int val;
        struct semid_ds *mbuf;
        ushort * array;
} arg;


void getsem(const int semid, struct sembuf* sops, STATUS status)
{
	sops[0].sem_num = 0;
	sops[1].sem_num = 1;

	sops[0].sem_flg = SEM_UNDO;
	sops[1].sem_flg = SEM_UNDO;
	switch(status)
	{
		case CLIENT_GET_MEM:
		{
			sops[0].sem_op = 0;
			sops[1].sem_op = -1;
			break;
		}
		case CLIENT_SND_REQ:
		{
			sops[0].sem_op = 1;
			sops[1].sem_op = 0;
			break;
		}
		case SERVER_RCV_REQ:
		{
			sops[0].sem_op = -1;
			sops[1].sem_op = 0;
			break;
		}
		case SERVER_SND_RPY:
		{
			sops[0].sem_op = 1;
			sops[1].sem_op = 1;
			break;
		}
		case CLIENT_RCV_RPY:
		{
			sops[0].sem_op = -1;
			sops[1].sem_op = -1;
			break;
		}
		case RESET_MEM:
		{
			sops[0].sem_op = 0;
			sops[1].sem_op = 1;
			printf("Reset semaphores done! \n");
			break;
		}
	}
        //ushort sem_value[2];
        //arg.array = sem_value;
        //semctl(semid,0,GETALL,arg);
        //printf("Semaphores values: s[0]: %d, s[1]: %d\n", sem_value[0], sem_value[1]);
        //printf("PID of last process to change the value of [0] semaphore: %d\n", semctl(semid, 0, GETPID, arg));
        //printf("PID of last process to change the value of [1] semaphore:  %d\n", semctl(semid, 1, GETPID, arg));
        //printf("Quantity of processes waiting when [0] semaphor is increasing:  %d\n", semctl(semid, 0, GETNCNT, arg));
        //printf("Quantity of processes waiting when [1] semaphor is increasing:  %d\n", semctl(semid, 1, GETNCNT, arg));
        //printf("Quantity of processes waiting when [0] semaphor will become equal to 0:  %d\n", semctl(semid, 0, GETZCNT, arg));
        //printf("Quantity of processes waiting when [1] semaphor will become equal to 0:  %d\n", semctl(semid, 1, GETZCNT, arg));
	//printf("Send request: %d, %d\n", sops[0].sem_op, sops[1].sem_op);
	if(semop(semid, sops, 2) == -1)
	{
		printf("There is an error with semaphor operations.\n");
		return;
	}
	return;
}

//char* from_double_to_str(double number)
//{
//	
//
//}

int main(int argc, char* argv[])
{
	
	key_t key = 1234;
	//could be ftok
	int semid = -1;
	int shmid = -1;
	//char* shaddr;
	EQU_AND_RES* shaddr;
	char equation[256];
	
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
	
	//printf("SEMID: %d\n", semid);
	if ((shmid = shmget(key, sizeof(EQU_AND_RES), 0666)) == 0) //IF EXISTS
	{
		printf("[SERVER]: Shared memory already exists. Deleting...\n");
		if ((shmctl(shmid, IPC_RMID, NULL)) == -1) //DELETE EXISTING SHARED MEMORY
		{
			printf("[SERVER]: There is error with deleting shared memory.\n");
			return -1;
		}
	}
	//printf("SHMID: %d\n", shmid);
	
	if ((shmid = shmget(key, sizeof(EQU_AND_RES), 0666|IPC_CREAT)) == -1) //INITIALIZE SHARED MEMORY
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
	if ((shaddr = (EQU_AND_RES*)shmat(shmid, NULL, 0)) == (EQU_AND_RES*)-1) //CONNECT TO SHARED MEMORY
	{
		printf("address: %p\n", shaddr);
		if (errno == EACCES)
		{
			printf("EACCES.\n");
		}
		else if (errno == EINVAL)
		{
			printf("EINVAL.\n");
		}
		else if (errno == ENOMEM)
		{
			printf("ENOMEM.\n");
		}
		printf("[SERVER]: There is error with connecting to shared memory.\n");
		printf("DELETING...\n");
		if ((semctl(semid, 0, IPC_RMID, 0)) == -1) //DELETE SEMAPHORES
		{
			if (errno == EACCES)
			{
				printf("EACCES.\n");
			}
			else if (errno == EFAULT)
			{
				printf("EFAULT.\n");
			}
			else if (errno == EIDRM)
			{
				printf("EIDRM.\n");
			}
			else if (errno == EINVAL)
			{
				printf("EINVAL.\n");
			}
			else if (errno == EPERM)
			{
				printf("EPERM.\n");
			}
			else if (errno == ERANGE)
			{
				printf("ERANGE.\n");
			}
			printf("[SERVER]: There is error with deleting semaphores.\n");
			return -1;
		}
		return -1;
	}
	

	//strcpy(shaddr, equation);

	//printf("%s\n", shaddr);
	printf("Hello world from SERVER!\n");
	
	
	
	sops[0].sem_num = 0;
	sops[1].sem_num = 1;
	
	getsem(semid, sops, RESET_MEM);
	
	char buffer_equation[100];
	buffer_equation[0] = '\0';

	
	while(1)
	{
		getsem(semid, sops, SERVER_RCV_REQ);
		printf("Got message.\n");
		strcpy(buffer_equation, shaddr->equation_str);
		if(strcmp(buffer_equation, "\n") != 0)
		{
			parse_equation_str(buffer_equation);
			double result = find_answer(buffer_equation);
			//printf("Result: %f\n", result);
			//strcpy(shaddr->equation_str, "all is okay");
			shaddr->result = result;
		}
		buffer_equation[0] = '\0';
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
