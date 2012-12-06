#include <stdio.h>

int main()
{
	int input; 
	int linecount = 0; 

	while((input = getchar()) != EOF)
	{
		if(input == '\n')
		{
			linecount++; 
		}

	}
	printf("Number of lines: %d\n", linecount); 
	return linecount; 
}
