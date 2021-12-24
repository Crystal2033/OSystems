#include "CentralServer.h"

int main(int argc, char* argv[])
{
	MESSAGE       user_message;
	char          user_ip[30];
	const char*   server_ip = "10.0.2.15";
	int           chosen_task;
	int           scnf_result;
	int           socket_des = -1;
	struct        sockaddr_in server_addr;


	if (argc != 2)
	{
		printf("You have forgotten to input your IP address.\n");
		exit(-1);
	}
	strcpy(user_ip, argv[1]);
	
	
	socket_des = socket(AF_INET, SOCK_STREAM, 0);
	printf("Socket ID: %d\n", socket_des);
	if (socket_des == -1)
	{
		printf("[CLIENT]: There is a problem with making socket.\n");
		exit(-1);
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(1234);
	inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
	
	if (connect(socket_des, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
	{
		printf("[CLIENT]: There is a problem with connecting socket.\n");
		if (close(socket_des) == -1)
		{
			printf("[CLIENT]: There is error with closing socket.\n");
		}
		exit(-1);
	}
	
	printf("User IP: %s\n\n", user_ip);
	printf("-----------------------------------------------------------------------------\n");
	printf("Hello dear client, select the desired option (input option`s number: 1 or 2):\n");
	printf("-----------------------------------------------------------------------------\n");
	
	printf("1. Output a list of subdirectories in the specified directory, sorting them by the number of files and a list of files, sorting them by the number of non-empty lines.\n\n");
	printf("2. Game <Guess the number>.\n\n");
	
	
	while(1)
	{
		printf(">> ");
		scnf_result = scanf("%d", &chosen_task);
		getchar();
		if (scnf_result <= 0)
		{
			printf("You have to input a number (1 or 2). Please, try again.\n");
			continue;
		}
		
		if ( !(chosen_task == 1 || chosen_task == 2) )
		{
			printf("Your chosen number is out of range of possible options. Please, try again.\n");
			continue;
		}
		break;
	}
	
	switch(chosen_task)
	{
		case 1:
		{	
			user_message.user_choice = SORT_FILES;			
			printf("Was chosen 1.\n");
			
			
			break;
		}
		case 2:
		{
			user_message.user_choice = GAME;
			printf("Was chosen 2.\n");
			break;
		}
	}
			
	strcpy(user_message.mes, "Message to server");
	printf("Sending...\n");
	if ( send(socket_des, (MESSAGE*)&user_message, sizeof(MESSAGE), 0) == -1)
	{
		printf("There is an error with sending message to socket.\n");
		safely_close_socket(socket_des);
		exit(-1);
	}
	printf("Sent.\n");
	printf("Recieving...\n");
	if ( recv(socket_des, (MESSAGE*)&user_message, sizeof(MESSAGE), 0) == -1)
	{
		printf("There is an error with receiving message to socket.\n");
		safely_close_socket(socket_des);
		exit(-1);
	}
	printf("Recieved message: %s\n", user_message.mes);
	printf("Recieved.\n");
	safely_close_socket(socket_des);
	return 0;
}


