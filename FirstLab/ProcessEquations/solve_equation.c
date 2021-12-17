#include<stdio.h>
#include<stdlib.h>
#include<string.h>
double find_answer(const char* equation);
double find_answer(const char* equation)
{
	double first_number = 0.0, second_number = 0.0;
	double result = 0.0;
	char operation;
	char* endptr;
	//printf("%s\n", equation);
	first_number = strtod(equation, &endptr);
	while (*endptr == 0 || *endptr == ' ')
	{
		endptr++;
	}
	operation = *endptr;
	endptr++;
	while (*endptr == 0 || *endptr == ' ')
	{
		endptr++;
	}
	second_number = strtod(endptr, &endptr);
	switch (operation)
	{
	case '+':
	{
		result = first_number + second_number;
		break;
	}
	case '-':
	{
		result = first_number - second_number;
		break;
	}
	case '*':
	{
		result = first_number * second_number;
		break;
	}
	case '/':
	{
		if (second_number == 0)
		{
			printf("Division by zero error. In this case answer will be equal 0. Check better all zero answers and equations!!!\n");
			return 0;
		}
		result = first_number / second_number;
		break;
	}
	default:
	{
		printf("Error operation.\n");
		break;
	}

	}
	return result;
}

int main(int argc, char* argv[]) // equation, file_output.
{
	FILE* f_write;
		
	if (argc == 1)
	{
		printf("You have forgotten to write equation.\n");
		return -1;
	}
	
	if (argc == 3)
	{
		f_write = fopen(argv[2], "a");
		//printf("%s\n", argv[1]);
		fprintf(f_write, "%s = %f\n", argv[1], find_answer(argv[1]));
		fclose(f_write);
		return 0;
	}
	printf("%lf\n", find_answer(argv[1]));
	
	return 0;

}
