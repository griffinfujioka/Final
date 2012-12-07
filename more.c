#include "ucode.c"

main(int argc, char* argv[])
{
	char buf[1024], next; 
	int fd = 0; 
	int linenumber = 0, c = 0, n; 

	printf("****************************************\n"); 
	printf("     Griffin's more in action\n"); 
	printf("****************************************\n");
	if(argc = 1)		// No argument was passed to more ---> PIPE
	{
		fd = 0; 
	}
	else if(argc > 1)
	{
		fd = open(argv[1], 0);  	// Open file for read 
	} 

	n = read(fd, buf, 1024); 	// Read 1024 bytes from file into buf[]

	while(n)		// while something was read in 
	{
		// Print the entire contents of the buffer 
		while(c < n && linenumber < 24)
		{
			if(buf[c] == '\n')
				linenumber++; 	// Keep track of what line number you're on 

			putc(buf[c]); 		// Print characters to the screen 1 by 1 
			c++; 
		}

		if(c >= n) 		// You've reached the end of the contents in the buffer 
		{
			c = 0; 		// Reset c to 0 
			n = read(fd, buf,1024); 	// Do it all over again 
		}

		if(linenumber >= 24)
		{
			next = getc();
			if(next == ' ')			// Jump to next section of 24 rows 
				linenumber = 0; 
			else if(next == '\r')
				linenumber = 23; 	// Move up one row
			}
		}
	}
}