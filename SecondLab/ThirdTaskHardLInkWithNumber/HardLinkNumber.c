#include<stdio.h>
//#include<dirent.h>
#include<sys/types.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
//typedef struct dirent Dirent;
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
char* from_int_to_str(const int value)
{
	char* num_in_str = malloc(sizeof(char)*255);
	char reversed_num_in_str[255];
	int remain = value;
	int base = 10;
	int i = 0;
	int j = 0;
	int length_of_number = 0;
	if (remain == 0)
	{
		num_in_str[0] = '0';
		num_in_str[1] = '\0';
		return num_in_str;
	}
	while(remain)
	{
		reversed_num_in_str[i] = remain % 10 + '0';
		remain /= base;
		i++;
	}
	length_of_number = i;
	reversed_num_in_str[i] = '\0';
	for (j = 0; j < length_of_number; j++)
	{
		num_in_str[j] = reversed_num_in_str[length_of_number - j - 1];
	}
	num_in_str[j] = '\0';
	return num_in_str;

}
char* whats_hard_link_name(const char* current_file_name)
{

	int i = 0;
	int number_in_hl = 1;
	char* hard_link_name = malloc(sizeof(char)*256);
	char* number_str;
	while(current_file_name[i] != '.')
	{
		hard_link_name[i++] = current_file_name[i];
	}
	hard_link_name[i++] = current_file_name[i];
	int start_ext_pos = i;
	int tmp_start_ext_pos = start_ext_pos;
	int j;
	while(1) // пока не найдется нужное число.
	{
		j = 0;
		i = tmp_start_ext_pos;
		start_ext_pos = tmp_start_ext_pos;
		number_str = from_int_to_str(number_in_hl++);
		while(number_str[j])
		{
			hard_link_name[i++] = number_str[j++];
		}
		hard_link_name[i++] = '.';
		//i = start_ext_pos;
		while(current_file_name[start_ext_pos])
		{
			hard_link_name[i++] = current_file_name[start_ext_pos++];
		}
		hard_link_name[i] = '\0';
		if(access(hard_link_name, F_OK) == -1) //значит, мы нашли свободное имя жесткой ссылки.
		{
			free(number_str);
			return hard_link_name;
		}
		
	}

}

int main(int argc, char* argv[])
{
	int file_descriptor = -1;
	char* hard_link_name = NULL;
	if(is_bad_argc_status(argc))
	{
		return -1;
	}

	
	file_descriptor = open(argv[1], O_WRONLY); // |O_CREAT
	if(file_descriptor == -1)
	{
		printf("File doesn`t exist or it is not possible to open this file for writing.\n");
		return -3;
	}
	
	
	hard_link_name = whats_hard_link_name(argv[1]);
	
	//printf("Hardlink name: %s\n", hard_link_name);
	if(link(argv[1], hard_link_name) == -1)
	{
		printf("There is an error with making link of file.\n");
		return -5;
	}
	free(hard_link_name);
	if(close(file_descriptor) == -1)
	{
		printf("There is a problem with closing file. File descriptor: %d\n", file_descriptor);
		return -4;
	}

	return 0;

}
