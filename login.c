/* Griffin Fujioka
login program 
*/ 
#include "ucode.c"
char *tty;

main(int argc, char *argv[]) 	// invoked by exec("login /dev/ttyxx")
{
	char buff[1024]; 
	char name[64]; 
	char password[64]; 
	char temp_line[128];
	char *line, *username, *psswrd, *tmp; 
	int fd, nlength, plength, stdin, stdout, stderr; 
	int good = 0; // good = 1 if password is valid 

	tty = argv[1]; 

	// Close in case init opened them before 
	close(0); close(1); close(2); 

	// 1. open /dev/tty0 as 0 (READ) and 1 (WRITE) in order to display messages
	stdin  = open("/dev/tty0", 0);
  	stdout = open("/dev/tty0", 1);
  	stderr = open("/dev/tty0", 1); 

  	//settty(tty); 	// store tty string in PROC.tty[] for putc()

  	printf("GF-LOGIN : open %s as stdin, stdout, stderr\n", tty); 

  	signal(2,1); 	// ignore ctrl + C interuppts (login is invincible!)

  	fd = open("/etc/passwd", 0); 	// Open password file for READ 	
    read(fd, buff, 1024);	// Read 1024 bytes from password file into buff

    while(1)
    {
    	printf("login: "); 
    	gets(name); 		// store username in name buff
    	printf("password: "); 
    	gets(password); 	// store password in password buff

    	read(fd, buff, 1024);	// Read 1024 bytes from password file into buff

    	strcat(name, ":"); 
    	strcat(password, ":"); 
    	nlength = strlen(name); 
    	plength = strlen(password); 
    	line = strtok(buff,"\n"); 	// DESTROYS buff !!! Read in a line from the /etc/passwd file
    	while(!good && line != 0)
    	{
    		strcpy(temp_line, line); 	// Copy the line into temp_line buffer

    		if(!strncmp(line, name, nlength))	// Check username by checking first nlength characters of line (i.e, the username)
    		{
    			username = strtok(line, ":"); 		// Store username in username buffer
    			psswrd = strtok(0, ":"); 			// Store password in psswrd buffer 
    			strcat(psswrd, ":"); 
    			printf("username: %s\n", username); 
    			printf("password: %s\n", psswrd); 

    			if(!strncmp(password, psswrd, plength))	// Check password
    			{
    				good = 1; 	// Valid login! 
    			}
    			else
    			{
    				printf("Invalid password.\n"); 
    				break; 	// Invalid password. 
    			}
    		}
    		line = strtok(0, "\n"); 		// Read the next line of the file 
    	}

    	if(good)
    	{
    		tmp = strtok(temp_line, ":"); 	
    		printf("tmp= %s\n", tmp); 
    		tmp = strtok(0, ":"); 	
    		printf("tmp= %s\n", tmp);
    		tmp = strtok(0, ":"); 	
    		printf("tmp= %s\n", tmp);
    		tmp = strtok(0, ":"); 	// tmp = uid
    		printf("Got it! uid is %s\n", tmp); 
    		chuid(((int)tmp)-'0',1); 	// Convert temp from char to int and set uid

    		tmp = strtok(0, ":"); 	// tmp = uid
    		tmp = strtok(0, ":"); 	// tmp = uid
    		printf("Where should I start!? Oh, HOME directory is %s\n", tmp); 
    		chdir(tmp); 		// cd to HOME directory

    		tmp = strtok(0, ":"); 	// tmp = uid
    		exec(tmp); 	// Tell where to execute the program
    	}
    	else
    	{
    		printf("login failed, try again.\n"); 
    	}

    	close(fd); 	// Close /etc/passwd file

    }
}