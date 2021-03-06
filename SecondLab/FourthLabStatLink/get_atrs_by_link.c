#include<stdio.h>
#include<sys/types.h>
#include<fcntl.h>
#include <sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
typedef struct stat Stat;
typedef struct group Group;
typedef struct passwd Passwd;

char perm_statuses[10] = "rwxrwxrwx";

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

int main(int argc, char* argv[])
{
	Stat statistic;
	

	if(is_bad_argc_status(argc))
	{
		return -1;
	}
	
	if(symlink("get_atrs_by_link.c", argv[1]) == -1)
	{	
		printf("There is error with making symbolic link.\n");
		return -1;
	}
	
	if(lstat(argv[1], &statistic) == -1)
	{
		printf("There is an error with getting stat from symbolic link.\n");
		return -2;
	}
	
	get_full_stat(statistic);
	
	return 0;
}
