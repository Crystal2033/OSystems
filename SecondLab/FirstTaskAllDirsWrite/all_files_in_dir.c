#include<stdio.h>
#include<dirent.h>
#include<sys/types.h>
typedef struct dirent Dirent;
int is_bad_argc_status(const int argc_number)
{
	if (argc_number == 1)
	{
		printf("You have forgotten to input directory way.\n");
		return -1;
	}
	else if(argc_number > 2)
	{
		printf("There are too many arguments. PLease, input only one directory way.\n");
		return -2;
	}
	return 0;

}
int main(int argc, char* argv[])
{
	Dirent *dir_struct;
	DIR* dir_f;
	
	if(is_bad_argc_status(argc))
	{
		return -1;
	}
	
	
	if(!(dir_f = opendir(argv[1])))
	{
		printf("Directory %s doesn`t exist  or you have input wrong directory way.\n", argv[1]);
		return -3;
	}
	for (int i = 0; dir_struct = readdir(dir_f); i++)
	{
		printf("%d.Name: %s\n",i + 1,  dir_struct->d_name);
	}
	closedir(dir_f);
	return 0;

}
