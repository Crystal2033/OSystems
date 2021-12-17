#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) //args: 1.FileWithEquations. 2.AnswersFile
{

	char equation_str[100];
	char* char_ptr = NULL;
	FILE* f_read_equations;
	FILE* log_file;
	int i = 0;
	int checker = 0;
	int proc_id = 0;
	FILE* f_out_file_delete_data;
	int main_pid = getpid();
	double result_of_equation = 0.0;
	int execl_status = 0;
	if (!(log_file=fopen("log.txt", "w"))) //deleting previous data.
	{
		printf("Lof file open ERROR!\n");

	}
	fclose(log_file);
	
	if (!(f_out_file_delete_data=fopen(argv[2], "w"))) //deleting previous data.
	{
		printf("Answer file open ERROR!\n");

	}
	fclose(f_out_file_delete_data);
	
	
	if (!(log_file=fopen("log.txt", "a")))
	{
		printf("Lof file open ERROR!\n");

	}
	
	if(argc == 1)
	{
		printf("There are no arguments. Please, input in args:\n");
		printf("1. First argument: file name with equations.\n");
		printf("2. Second argument: file name where will be solves on your equations in first file.\n");
		return -1;
	}
	else if(argc == 2)
	{
		printf("You have forgotten one argument. Please, input in args:\n");
		printf("1. First argument: file name with equations.\n");
		printf("2. Second argument: file name where will be solves on your equations in first file.\n");
		return -1;
	}
	else if(argc > 3)
	{
		printf("There are too many arguments. Please, input in args:\n");
		printf("1. First argument: file name with equations.\n");
		printf("2. Second argument: file name where will be solves on your equations in first file.\n");
		return -1;
	}
	
	
	
	if (!(f_read_equations = fopen(argv[1], "r")))
	{
		printf("Can`t open your file, please check your file name and try again.\n");
		return -1;
	}
	
	
	while(fgets(equation_str, 100, f_read_equations)) //Reading equations and deleting '\n'.
	{
		char_ptr = equation_str;
		for (i = 0; i < strlen(equation_str); i++)
		{
			if(*char_ptr == '\n' || *char_ptr == EOF)
			{
				equation_str[char_ptr - equation_str] = '\0';
				break;
			}
			char_ptr++;
		}
		
		if(!isalnum(equation_str[0]))  //end-of-file handling 
		{
			break;
		}
		
		switch(proc_id = (int)fork())
		{
		case -1:
		{
			fprintf(log_file, "There is error with borning process.\n");
			fclose(f_read_equations);
			return -1;
		}
		case 0:
		{
			if (!(log_file=fopen("log.txt", "a")))
			{
				printf("Lof file open ERROR!\n");

			}
			
			checker = fprintf(log_file , "I`m CHILD process, my id: %d. My parent id: %d\n", getpid(), getppid());
			
			
			execl_status = execl("./solve_equation", "solve_equation.c", equation_str, argv[2], NULL);
			if (execl_status == -1)
			{
				fprintf(log_file ,"Something went wrong with execl. PLease, check data in parametres function.\n");
			}
			fprintf(log_file, "Success!\n");
			fclose(log_file);
			break;
		}
		default:
		{
			checker = fprintf(log_file , "I`m PARENT process, my id: %d. My child id: %d\n", getpid(), proc_id);
			waitpid(proc_id, 0, 0);
			checker = fprintf(log_file, "My child has ended his work, and i have to return, my id: %d. ChildId: %d\n", getpid(), proc_id);
			break;
		}
		}
		
		fprintf(log_file, "Was read: %s\n", equation_str); 
	}
	
	fclose(f_read_equations);
	fclose(log_file);
	return 0;
}
