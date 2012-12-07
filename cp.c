#include "ucode.c"

main(int argc, char* argv[])
{
	int fd1, fd2, n; 
	char buf[1024]; 

	printf("****************************************\n"); 
	printf("     Griffin's cp in action\n"); 
	printf("****************************************\n");

	if(argc < 3)
	{
		printf("[USAGE]: cp source destination\n"); 
		return; 
	}

	fd1 = open(argv[1], 0); 	// Open source file for READ
	fd2 = open(argv[2], 1|0100); 	// Open destination file for WRITE or CREATE	

	n = read(fd1, buf, 1024); 

	while(n)
	{
		printf("Read %d bytes from source.\n", n); 
		write(fd2, buf, n); 		// Write to destination file 
		printf("Wrote %d bytes to destination.\n", n); 
		n = read(fd1, buf, 1024); 	// Read from source file 
	}

}