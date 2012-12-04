#include "ucode.c"

main(int argc, char* argv[])
{
	char buf[1024]; 
	int fd , n;
	int c = 0, i = 0, j = 0, found = 0; 

	printf("****************************************\n"); 
	printf("     Griffin's grep in action\n"); 
	printf("****************************************\n");
	
	if(argc > 2)
		fd = open(argv[2], 0); 	// Open the file for READ
	else
	{
		printf("[USAGE]: grep pattern filename\n"); 
		fd = 0; 
		return; 
	}
		

	if(fd > 0)		// The file was opened correctly and is available for READ
	{
		//printf("fd = %d\n", fd); 
		//printf("Pattern = %s\n", argv[1]); 
		n = read(fd, buf, 1024); 	// Read 1024 bytes from file into buf[]

		while(n)
		{
			//printf("Successfully read 1024 bytes into buf[]\n"); 
			while(c < n)
			{
				if(i == 1)
				{
					i = 0; 
					// Print out everything in buf[]
					while(j < n && buf[j] != '\n')
					{
						putc(buf[j]); 		
						j++; 
					}
					printf("\n"); 
					j++; 
					c = j; 
				}
				else if (buf[c] == '\n')
				{
					c++; 
					j = c; 
				}
				else if(buf[c] == argv[1][0])		// character matches first letter of pattern
				{
					// Compare pattern against the characters following buf[c]
					//printf("We found a first letter match!\n"); 
					//printf("Length of pattern = %d\n", strlen(argv[1])); 
					found = !strncmp(argv[1],&buf[c], strlen(argv[1]));
					/*
					switch(found)
					{
						case 0: 
							printf("The pattern was not a match.\n"); 
							break; 
						case 1: 
							printf("The pattern was a match.\n"); 
							break; 
					}
					*/ 
				}

				if(found)
				{
					//printf("Found %s.\n", argv[1]); 
					found = 0; 
					while(j < n && buf[j] != '\n')
					{
						putc(buf[j]); 	// Print the line 
						j++;  
					}

					if(j >= n)
						i = 1; 
					else
					{
						printf("\n"); 
						j++; 
						c = j; 
					}
				}
				else
				{
					c++; 
				}
			}

			if(c >= n)
			{
				c = 0; 
				j = 0; 
				n = read(fd, buf, 1024); 
			}
		}
	}
	else
	{
		//printf("fd = %d\n", fd); 
		printf("ERROR: Unable to open file.\n"); 
	}
}