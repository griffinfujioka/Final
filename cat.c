#include "ucode.c"

main(int argc, char* argv[])
{
	int fd = 0; 
	int n; 
	char buf[1024];
	char *tmp, *input; 

	if(argc > 1)
	{
		fd = open(argv[1], 0); 	// Open file for READ
	}

	if(!fd)
	{
		input = getc(); 
		while(input != 4)	// While not Ctrl + D 
		{
			if(input == '\r')
			{
				putc("\n"); 
			}
			putc(input); 
			input = getc(); 
		}
	}
	else if(fd > 0)
	{
		n = read(fd, buf, 1024); 
		while(n)
		{
			tmp = strtok(buf, "\n"); // Read line-by-line
			while(tmp != 0)
			{
				write(1, tmp, strlen(tmp)); 
				printf("\n\r"); 
				tmp = strtok(0, "\n"); 
			}

			n = read(fd, buf, 1024); 
		}
	}
	else
	{
		printf("ERROR: An unknown error occurred. Exiting cat...\n"); 
	}


}