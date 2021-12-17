#include<stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
extern int errno; 
typedef struct ClientMessage
{
	long msgtype;
	long double coeffs[4];
	
} CLIENT_MES;

typedef struct number
{
	long double real_part;
	long double im_part;
}NUMBER;

typedef struct ServerMessage
{
	long msgtype;
	NUMBER answers[3];	
}SERVER_MES;

void input_coeffs_for_cubic(long double* coeffs)
{
	printf("Please input coeffs in your cubic equation. Use dots instead of commas. Example: 23.10 .\n");
	char buffer[100];
	
	char tmp_string[50];
	int value_of_numbers = 0;
	while(1)
	{
		int i = 0;
		int j = 0;
		printf("Input coeffs for your cubic equation: \n >> ");
		fgets(buffer, 100, stdin);
		tmp_string[0] = '\0';
		value_of_numbers = 0;
		
		while(buffer[i])
		{
			if(isspace(buffer[i]))
			{
				if(strlen(tmp_string) != 0)
				{
					tmp_string[j] = '\0';
					//printf("Tm_string: %s\n", tmp_string);
					coeffs[value_of_numbers] = (long double)atof(tmp_string);
					if(coeffs[value_of_numbers] == 0.0 && tmp_string[0] != '0')
					{
						printf("Wrong input. Your number can consists only digits and '-'.\n");
						exit(1);
					}
					//printf("Number: %Lf\n", coeffs[value_of_numbers++]);
					value_of_numbers++;
					j = 0;
					tmp_string[j] = '\0';
				}
				i++;
				continue;
			}
			else
			{
				tmp_string[j++] = buffer[i++];
			}
			
		}
		if(value_of_numbers != 4)
		{
			printf("You have to input 4 numbers for cubic equation.\n");
			continue;
		}
		else
		{
			break;
		}
	}
	
	
}

int main(int argc, char* argv[])
{
	//key_t key = ftok("FileToMakeKey", 'f');
	key_t key = 2200;
	int msid = 0;
	size_t my_pid = getpid();
	CLIENT_MES my_struct_mes;
	SERVER_MES ans;
	long double coeffs[4];
	int length = 0;
	
	if ((msid = msgget(key, 0)) == -1)
	{
		printf("[CLIENT]: There is an error with openning message queue.\n");
		if (errno == ENOSPC)
		{
			printf("ENOSPC.\n");
		}
		else if (errno == EEXIST)
		{
			printf("EEXIST.\n");
		}
		else if (errno == ENOENT)
		{
			printf("ENOENT.\n");
		}
		else if (errno == ENOMEM)
		{
			printf("ENOMEM.\n");
		}
		else if (errno == EACCES)
		{
			printf("EACCES.\n");
		}
		return -1;
	}
	
	if(argc == 1)
	{
		input_coeffs_for_cubic(coeffs);
	}

	my_struct_mes.msgtype = my_pid;
	//my_struct_mes.coeffs = coeffs;
	for (int i = 0; i < 4; i++)
	{
		my_struct_mes.coeffs[i] = coeffs[i];
		//printf("%d. %Lf\n", i, my_struct_mes.coeffs[i]); 
	}
	//printf("Bytes = %ld\n", sizeof(my_struct_mes));
	//printf("ID = %d\n", msid);
	if (msgsnd(msid, &my_struct_mes, sizeof(my_struct_mes), 0) == -1)
	{
		printf("[CLIENT]: There is an error with sending message.\n");
		if (errno == EAGAIN)
		{
			printf("EAGAIN.\n");
		}
		else if (errno == EACCES)
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
		else if (errno == EINTR)
		{
			printf("EINTR.\n");
		}
		else if (errno == EINVAL)
		{
			printf("EINVAL.\n");
		}
		else if (errno == ENOMEM)
		{
			printf("ENOMEM.\n");
		}
		return -1;
	}
	
	while (msgrcv(msid, &ans, sizeof(ans), my_pid, 0) < 0)
	{
		//printf("waiting...\n");
	}
	
	for (int i = 0; i < 3; i++)
	{
		printf("X[%d] = %Lf + i*(%Lf)\n", i, ans.answers[i].real_part, ans.answers[i].im_part);
	}
	
	return 0;
}
