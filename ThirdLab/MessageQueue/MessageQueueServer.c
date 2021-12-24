#include<stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include<math.h>
#include<stdlib.h>
#include <errno.h>
extern int errno;  
#define PI 3.1415926535

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


NUMBER* first_degree_equation(const long double* coeff_arr)
{
	long double coef_x_1, free_coeff;
	long double answer;
	NUMBER* answers = (NUMBER*)malloc(sizeof(NUMBER)*3);
	coef_x_1 = coeff_arr[0];
	free_coeff = coeff_arr[1];
	if (coef_x_1 == 0 && free_coeff != 0)
	{
		printf("SolveError. %Lf can`t be equal %Lf. \n", coef_x_1, free_coeff);
		for (int i = 0; i < 3; i++)
		{
			answers[i].real_part = 0; 
			answers[i].im_part = 0;  
		}
		return answers;
	}
	else if (coef_x_1 == 0 && free_coeff == 0)
	{
		printf("SolveError. There is nothing to solve %Lf = %Lf, it`s true.\n", coef_x_1, free_coeff);
		for (int i = 0; i < 3; i++)
		{
			answers[i].real_part = 0; 
			answers[i].im_part = 0;  
		}
		return answers;
	}
	else
	{
		if (free_coeff == 0)
		{
			answers[0].real_part = 0; 
			answers[0].im_part = 0;  
			for (int i = 0; i < 3; i++)
			{
				answers[i].real_part = 0; 
				answers[i].im_part = 0;  
			}
			return answers;
		}
		
		answer = (free_coeff * -1) / coef_x_1;
		answers[0].real_part = answer;
		answers[0].im_part = 0;
		
		for (int i = 1; i < 3; i++)
		{
			answers[i].real_part = 0; 
			answers[i].im_part = 0;  
		}
		
		return answers;
	}
}


NUMBER* second_degree_equation(const long double* coeff_arr)
{
	NUMBER answer1, answer2;
	NUMBER* answers = (NUMBER*)malloc(sizeof(NUMBER)*3);
	long double diskriminant, a, b, c;
	a = coeff_arr[0];
	b = coeff_arr[1];
	c = coeff_arr[2];
	
	if (a == 0)
	{
		long double first_degree[2];
		first_degree[0] = b;
		first_degree[1] = c;
		free(answers);
		answers = first_degree_equation(first_degree);
		return answers;
	}

	diskriminant = pow(b,2) - 4 * a * c;
	if (diskriminant >= 0)
	{
		answer1.real_part = ((-1 * b) + sqrt(diskriminant)) / (2 * a);
		answer2.real_part= ((-1 * b) - sqrt(diskriminant)) / (2 * a);

		if (answer1.real_part == answer2.real_part)
		{
			answers[0].real_part = answer1.real_part;
			answers[0].im_part = 0;
			
			for (int i = 1; i < 3; i++)
			{
				answers[i].real_part = 0; 
				answers[i].im_part = 0;  
			}
			return answers;
		}
		else
		{
			answers[0].real_part = answer1.real_part;
			answers[0].im_part = 0;
			
			answers[1].real_part = answer2.real_part;
			answers[1].im_part = 0;
			
			answers[2].real_part = 0;
			answers[2].im_part = 0;
			return answers;
		}

	}
	else
	{
		diskriminant *= -1;
		answer1.im_part = sqrt(diskriminant) / (2 * a);
		answer2.im_part = -sqrt(diskriminant) / (2 * a);
		answer1.real_part = (-1 * b) / (2 * a);
		answer2.real_part = (-1 * b) / (2 * a);
		
		answers[0].real_part = answer1.real_part;
		answers[0].im_part = answer1.im_part;
		
		answers[1].real_part = answer2.real_part;
		answers[1].im_part = answer2.im_part;
		
		answers[2].real_part = 0;
		answers[2].im_part = 0;
		
		return answers;
	}
}


NUMBER* third_degree_equation(long double* coeff_arr)
{
	long double a, b, c, d;
	a = coeff_arr[0];
	b = coeff_arr[1];
	c = coeff_arr[2];
	d = coeff_arr[3];
	NUMBER* answers = (NUMBER*)malloc(sizeof(NUMBER)*3);
	if (a != 1 && a != 0)
	{
		b /= a;
		c /= a;
		d /= a;
		a = 1;
	}

	if (a == 0)
	{
		long double second_degree[3];
		second_degree[0] = b;
		second_degree[1] = c;
		second_degree[2] = d;
		free(answers);
		answers = second_degree_equation(second_degree);
		return answers;
	}
	long double p, q, x1, x2, x3;
	p = (3 * a * c  - pow(b, 2))/ (3 * a);
	q = (2 * pow(b, 3) - 9 * a * b * c + 27 * pow(a, 2) * d) / 27 * pow(a, 3);

	long double cube_diskriminant;
	cube_diskriminant = pow(q / 2, 2) + pow(p / 3, 3);
	
	long double y1, y2, y3, phi;
	if (q < 0)
	{
		phi = atan(sqrt(cube_diskriminant * -1) / (-1 * q / 2)) * 180 / PI;
	}
	else if (q > 0)
	{
		phi = atan(sqrt(cube_diskriminant * -1) / (-1 * q / 2)) * 180 / PI + 180;
	}
	else if (q == 0)
	{
		phi = 90;
	}

	if (cube_diskriminant < 0)
	{
		y1 = 2 * sqrt((-1 * p) / 3) * cos((phi / 3) * PI / 180);
		y2 = 2 * sqrt(-1 * p / 3) * cos(((phi / 3) + (2 * 180 / 3)) * PI / 180);
		y3 = 2 * sqrt(-1 * p / 3) * cos(((phi / 3) + (4 * 180 / 3)) * PI / 180);

		x1 = y1 - b / (3 * a);
		x2 = y2 - b / (3 * a);
		x3 = y3 - b / (3 * a);
		
		answers[0].real_part = x1;
		answers[0].im_part = 0;
		
		answers[1].real_part = x2;
		answers[1].im_part = 0;
		
		answers[2].real_part = x3;
		answers[2].im_part = 0;
		
		return answers;
	}
	else if (cube_diskriminant > 0)
	{
		NUMBER y2, y3, x2, x3;
		y1 = cbrt((q / -2) + sqrt(cube_diskriminant)) + cbrt((q / -2) - sqrt(cube_diskriminant));
		y2.real_part = -0.5 * y1;
		y2.im_part = (sqrt(3) / 2) * (cbrt((-1 * q / 2) + sqrt(cube_diskriminant)) - cbrt((-1 * q / 2) - sqrt(cube_diskriminant)));
		y3.real_part = -0.5 * y1;
		y3.im_part = -1 * y2.im_part;


		x1 = y1 - b / (3 * a);
		x2.real_part = y2.real_part - b / (3 * a);
		x2.im_part = y2.im_part;
		x3.real_part = y3.real_part - b / (3 * a);
		x3.im_part = y3.im_part;
		
		answers[0].real_part = x1;
		answers[0].im_part = 0;
		
		answers[1].real_part = x2.real_part;
		answers[1].im_part = x2.im_part;
		
		answers[2].real_part = x3.real_part;
		answers[2].im_part = x3.im_part;
		
		return answers;
	}
	else if (cube_diskriminant == 0 && q != 0)
	{
		y1 = 2 * cbrt(-1 * q / 2);
		y2 = -1 * cbrt(-1 * q / 2);

		x1 = y1 - b / (3 * a);
		x2 = y2 - b / (3 * a);
		
		answers[0].real_part = x1;
		answers[0].im_part = 0;
		
		answers[1].real_part = x2;
		answers[1].im_part = 0;
		
		answers[2].real_part = 0;
		answers[2].im_part = 0;
		
		return answers;
	}
	else if (cube_diskriminant == 0 && q == 0 && p == 0)
	{
		x1 = -1 * b / (3 * a);
		
		
		answers[0].real_part = x1;
		answers[0].im_part = 0;
		
		answers[1].real_part = 0;
		answers[1].im_part = 0;
		
		answers[2].real_part = 0;
		answers[2].im_part = 0;
		return answers;
	}
	
}


int main(int argc, char* argv[])
{
	key_t key = ftok("MessageQueueClient.c", 'f');
	//key_t key = 2200;
	int msid = 0;
	CLIENT_MES my_struct_mes;
	SERVER_MES ans;
	NUMBER* answers = NULL;
	if ((msid = msgget(key, 0666 | IPC_CREAT)) == -1)
	{
		printf("[SERVER]: There is an error with making message queue.\n");
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

	while(1)
	{
		if (msgrcv(msid, &my_struct_mes, sizeof(my_struct_mes), 0, 0) <= 0)
		{
			continue;
		}
		
		ans.msgtype = my_struct_mes.msgtype;
		
		answers = third_degree_equation(my_struct_mes.coeffs);

		if (answers == NULL)
		{
			for (int i = 0; i < 3; i++)
			{
				ans.answers[i].real_part = 0;
				ans.answers[i].im_part = 0;
			}
		}
		else
		{
			for (int i = 0; i < 3; i++)
			{
				ans.answers[i] = answers[i];
			}
		}
		free(answers);
		msgsnd(msid, &ans, sizeof(ans), 0);
	}
	
	msgctl(msid, IPC_RMID, 0);
	return 0;
}
