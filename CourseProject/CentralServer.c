//#include "CentralServer.h"
#include "SemaphorServer.h"
#include "QueueServer.h"

void getsem(const int semid, struct sembuf* sops, STATUS status);
void snd_msg_to_sem_serv(MESSAGE* mes_struct);
void snd_msg_to_que_serv(MESSAGE* mes_struct);


int main(int argc, char* argv[])
{
	int     accepted_sock;
	int     listener_sock;
	struct  sockaddr_in addr;
	MESSAGE mes_struct;
	int     bytes_read;

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

	if (bind(listener_sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		printf("[SERVER]: There is an error with binding sockets.\n");
		close(listener_sock);
		exit(-1);
	}
	printf("Listening...\n");
	int status = listen(listener_sock, 5);
	if (status != 0)
	{
		printf("[SERVER]: There is an error with listenning sockets.\n");
		close(listener_sock);
		exit(-1);
	}
	
	while (1)
	{
		accepted_sock = accept(listener_sock, NULL, NULL);
		if (accepted_sock == -1)
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
			if (bytes_read <= 0)
			{
				printf("[SERVER]: There is an error with receiving.\n");
				close(accepted_sock);
				exit(-1);
			}
			
			if (mes_struct.user_choice == SORT_FILES)
			{
				snd_msg_to_sem_serv(&mes_struct);
			}
			else if (mes_struct.user_choice == GAME)
			{
				snd_msg_to_que_serv(&mes_struct);
			}
			
			printf("Sending...\n");
			if (send(accepted_sock, (MESSAGE*)&mes_struct, sizeof(MESSAGE), 0) == -1)
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

