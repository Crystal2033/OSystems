#include "QueueServer.h"

int main(int argc, char* argv[])
{
	key_t key = ftok("QueueServer.c", 'f');
	int msid = 0;
	CLIENT_MES my_struct_mes;
	SERVER_MES ans;
	
	if ((msid = msgget(key, 0666)) == 0)
	{	
		printf("[SERVER]: Deleting existing queue.\n");
		msgctl(msid, IPC_RMID, 0);
		return -1;
	}

	if ((msid = msgget(key, 0666 | IPC_CREAT)) == -1)
	{
		printf("[SERVER]: There is an error with making message queue.\n");
		return -1;
	}

	while(1)
	{
		if (msgrcv(msid, &my_struct_mes, sizeof(my_struct_mes), 0, 0) <= 0)
		{
			continue;
		}
		
		ans.msgtype = my_struct_mes.msgtype;
		strcpy(ans.message, "Message from MESSAGE queue");
		ans.tmp_var = 1000;
		
		msgsnd(msid, &ans, sizeof(ans), 0);
	}
	
	msgctl(msid, IPC_RMID, 0);
	return 0;
}
