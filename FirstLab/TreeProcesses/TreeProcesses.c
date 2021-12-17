#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include<time.h>
#include <locale.h>

int main(void)
{
	setlocale (LC_ALL, "");
	int pid = 0;
	int procent = 0;
	int is_already_worked = 0;
	int generate_proc_value = 0;
	int i = 0, j = 0;
	int length_of_proc_trace = 0;
	int chPID[3];
	int k = 0;
	
	int main_pid = getpid();
	char for_histo_get_data[20];
	int value_of_pids = 0;
	int max_length = 0;
	int length_pid;
	int pid_name_from_file;
	char* endptr;
	int**histogramm;
	
	
	const char* file_name_for_histo = "HistogramFilaData.txt";
	FILE* histo_out;
	FILE* histo_read;
	
	if(!(histo_out = fopen(file_name_for_histo, "w"))) // for deleting previous data.
	{
		printf("Histogramm file open error.\n");
		return -1;
	}
	fclose(histo_out);
	
	while(!is_already_worked)
	{
		is_already_worked = 0;
		srand(getpid());
		procent = rand() % 100 + 1;
		
		if(procent > 0 && procent <= 40) //End
		{
			if(!(histo_out = fopen(file_name_for_histo, "a")))
			{
				printf("Histogramm file open error.\n");
				return -1;
			}
			
			for(k = 0; k < length_of_proc_trace; k++)
			{
				if (!k)
				{
					printf("Pid: %d|   ", getpid());
					fprintf(histo_out, "%d;%d\n", getpid(), length_of_proc_trace);
				}
				if(k == length_of_proc_trace - 1)
				{
					printf("*\n");
					continue;
				}
				printf("*");
					
			
			}
			fclose(histo_out);
			break;
		}
		
		else if(procent > 40 && procent <= 75)  //+1
		{
			generate_proc_value = 1;

		}
		else if(procent > 75 && procent <= 90)  //+2
		{
			generate_proc_value = 2;
		}
		else if(procent > 90 && procent <= 100)  //+3
		{
			generate_proc_value = 3;

		}

		for (i = 0; i < generate_proc_value; i++)
		{
			switch(pid = (int)fork())
			{
				
				case -1: //error
				{
					printf("ForkError.\n");
					return -1;
				}
				
				case 0: //Child
				{ 	
					is_already_worked = 0;
					length_of_proc_trace++;
					break;
				}
				
				default: //parent
				{
					is_already_worked = 1;
					
					chPID[i] = pid;
					if(i == generate_proc_value - 1)
					{
						for (j = 0; j < generate_proc_value; j++)
						{
							waitpid(chPID[j], 0, 0);
						
						}
					}
					break;
				}
			}
			if(!pid) //for child break for and start while first.
			{
				break;
			}
		}
		
	}

	if(getpid() == main_pid)
	{
		if(!(histo_read = fopen(file_name_for_histo, "r")))
		{
			printf("File open error.");
			return -1;
		}
		
		
		while(fgets(for_histo_get_data, 20, histo_read))
		{
			value_of_pids++;
			strtol(for_histo_get_data, &endptr, 0);
			//printf("Was read pid: %d\n", pid_name_from_file);
			
			length_pid = strtol(endptr + 1, &endptr, 0);
			//printf("Was read length: %d\n", length_pid);
			if(length_pid > max_length)
			{
				max_length = length_pid;
			}
			
		}
		for(i = 0; i< value_of_pids; i++)
		{
			printf("-------");
		}
		printf("\n");
		
		fseek(histo_read, 0, SEEK_SET);
		
		
		histogramm = (int**)malloc(sizeof(int*)*(max_length+1)); // +1 because we have to have cells for pids.
		for(i = 0; i < (max_length + 1); i++)
		{
			histogramm[i] = (int*)calloc(value_of_pids, sizeof(int));
		}
		
		
		j = 0;
		endptr = NULL;
		while(fgets(for_histo_get_data, 20, histo_read))
		{
			pid_name_from_file = strtol(for_histo_get_data, &endptr, 0);
			//printf("Was read pid: %d\n", pid_name_from_file);
			
			length_pid = strtol(endptr + 1, &endptr, 0);
			//printf("Was read length: %d\n", length_pid);
			histogramm[0][j] = pid_name_from_file;
			
			for(i = 0; i < length_pid; i++)
			{
				histogramm[i+1][j] = 1;	
			}
			j++;
			
		}
		

		
		for(i = max_length; i >= 0; i--)
		{
			for(j = value_of_pids - 1; j >= 0; j--)
			{
				if(histogramm[i][j] == 1)
				{
					printf("|  %lc  |", 0x2592);
					continue;
				}
				else if(histogramm[i][j] == 0)
				{
					printf("|     |");
					continue;
				}
				else 
				{
					printf("|%d |", histogramm[i][j]);
					
				}
				
			}
			
			printf("\n");
		}
		
		
		
		for(i = 0; i < (max_length + 1); i++)
		{
			free(histogramm[i]);
		}
		free(histogramm);

		fclose(histo_read);
	}
	return 0;
}
