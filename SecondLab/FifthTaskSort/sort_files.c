#include<stdio.h>
#include<sys/types.h>
#include<fcntl.h>
#include <sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include <time.h>
#include<dirent.h>
#include <pwd.h>
#include <grp.h>
#include<string.h>
#include<stdlib.h>
#include <errno.h>
extern int errno;  
char perm_statuses[10] = "rwxrwxrwx";

typedef struct stat Stat;
typedef struct dirent Dirent;
typedef struct group Group;
typedef struct passwd Passwd;

typedef struct data_file

{
	char way[256];
	time_t time;

}DATA;



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

char show_type_of_file(mode_t mode)
{
	switch(mode & S_IFMT)
	{
		case S_IFDIR: {return 'd';}
		case S_IFCHR: {return 'c';}
		case S_IFBLK: {return 'b';}
		case S_IFREG: {return '-';}
		case S_IFLNK: {return 'l';}
		case S_IFIFO: {return 'p';}
	}

}

char* get_permission(mode_t mode)
{
	char* permission_str = malloc(sizeof(char)*10);
	for (int i = 0, j= (1 << 8); i < 9; i++, j >>=1)
	{
		permission_str[i] = (mode & j) ? perm_statuses[i] : '-';
	}
	if(mode & S_ISUID)
	{
		permission_str[2] = (permission_str[2] == 'x') ? 'S' : 's';
	}
	if(mode & S_ISGID)
	{
		permission_str[5] = (permission_str[5] == 'x') ? 'G' : 'g';
	}
	if(mode & S_ISVTX)
	{
		permission_str[8] = (permission_str[8] == 'x') ? 'T' : 't';
	}
	
	return permission_str;
}
char* get_gid(gid_t gid_number)
{
	Group *gr_ptr;
	gr_ptr = getgrgid(gid_number);
	return gr_ptr->gr_name;
}

char* get_uid(uid_t uid_number)
{
	Passwd *passwd_ptr;
	passwd_ptr = getpwuid(uid_number);
	return passwd_ptr->pw_name;
}

void get_full_stat(const Stat statistic)
{

	char* print_access;
	char* gr_name, *uid_name;
	int counter = 1;
	printf("%d. File system ID: %ld\n", counter++, statistic.st_dev);
	printf("%d. Descriptor: %ld\n",counter++, statistic.st_ino);
	print_access = get_permission(statistic.st_mode);
	printf("%d. Type of file: %c\n",counter++, show_type_of_file(statistic.st_mode));
	printf("%d. Access mode: %s\n", counter++, print_access);
	free(print_access);

	printf("%d. Value of hardlinks: %ld\n",counter++, statistic.st_nlink);
	gr_name = get_gid(statistic.st_gid);
	printf("%d. GID: %s\n",counter++, gr_name);
	uid_name = get_uid(statistic.st_uid);
	printf("%d. UID: %s\n",counter++, uid_name);
	printf("%d. Size: %ld\n",counter++, statistic.st_size);
	printf("%d. Last access time: %s",counter++, ctime(&statistic.st_atime));
	printf("%d. Last modify time: %s",counter++, ctime(&statistic.st_mtime));
	printf("%d. Last access change time: %s",counter++, ctime(&statistic.st_ctime));

}


void bubble_sort(DATA* array, const int length)
{
	for (int i = 0; i < length ; i++)
	{
		for (int j = 0; j < length - i - 1; j++)
		{
			if(array[j].time > array[j + 1].time)
			{
				DATA tmp = array[j + 1];
				array[j + 1] = array[j];
				array[j] = tmp;
			}
		}
	
	}

}




int main(int argc, char* argv[])
{
	
	Dirent *dir_struct;
	DIR* dir_f;
	Stat statistic;
	char* full_path_name = malloc(sizeof(char)* 256);
	full_path_name = strcpy(full_path_name, argv[1]);
	
	DATA data_base[BUFSIZ];
	int counter = 0;
	if(is_bad_argc_status(argc))
	{
		return -1;
	}
	
	if(!(dir_f = opendir(argv[1])))
	{
		printf("Directory %s doesn`t exist or you have input wrong directory way.\n", argv[1]);
		return -3;
	}
	
	for (int i = 0; dir_struct = readdir(dir_f); i++)
	{
		counter++;
		full_path_name = strcat(full_path_name, dir_struct->d_name);
		strcpy(data_base[i].way, full_path_name);
		if(stat(full_path_name, &statistic) == -1)
		{
			printf("There is an error with getting stats from %s.\n", dir_struct->d_name);
			printf("ERROR NUMBER: %d\n", errno);
			return -1;
		}
		data_base[i].time = statistic.st_mtime;
		full_path_name = strcpy(full_path_name, argv[1]);

	}
	
	bubble_sort(data_base, counter);
	
	
	for (int i = 0; i < counter; i++)
	{
		if(stat(data_base[i].way, &statistic) == -1)
		{
			printf("%ld\n", dir_struct->d_ino);
			printf("There is an error with getting stats from %s.\n", dir_struct->d_name);
			printf("ERROR NUMBER: %d\n", errno);
			return -1;
		}
		printf("%s\n", data_base[i].way);
		get_full_stat(statistic);
		printf("-----------------------------------------------------------------------------------------\n");

	}
	free(full_path_name);
	closedir(dir_f);
	
	return 0;
}
