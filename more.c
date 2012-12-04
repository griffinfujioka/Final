#include "ucode.c"

main(int argc, char* argv[])
{
	char buf[1024], next; 
	int fd = 0; 
	int linenumber = 0, c = 0, n; 

	if(argc > 1)
	{
		fd = open(argv[1], 0);  	// Open file for read 
	}

	n = read(fd, buf, 1024); 	// Read 1024 bytes from file into buf[]

	while(n)
	{
		// Print the entire line 
		while(c < n && linenumber < 24)
		{
			if(buf[c] == '\n')
				linenumber++; 	// When you reach the end of a line, move to the next one

			putc(buf[c]); 
			c++; 
		}

		if(c >= n) 		// You've reached the end of whatever was just read in. 
		{
			c = 0; 		// Reset c to 0 
			n = read(fd, buf,1024); 	// Try to read 1024 bytes from file into buf[] again
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