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
	int uid = 0, gid = 0; 

	tty = argv[1]; 

	// Close in case init opened them before 
	close(0); close(1); close(2); 

	// 1. open /dev/tty0 as 0 (READ) and 1 (WRITE) in order to display messages
	stdin  = open(tty, 0);
  	stdout = open(tty, 1);
  	stderr = open(tty, 1); 

    // determine where to run the login task
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
    				break; 	 
    			}
    		}
    		line = strtok(0, "\n"); 		// Read the next line of the file 
    	}

    	if(good)
    	{
    		tmp = strtok(temp_line, ":"); 		// tmp = username 
    		printf("username= %s\n", tmp); 
    		tmp = strtok(0, ":"); 				// tmp = password
    		printf("password= %s\n", tmp);		
    		tmp = strtok(0, ":"); 				//  tmp = gid 
    		gid = atoi(tmp); 
    		printf("gid= %d\n", gid);           // This value is garbage 
    		tmp = strtok(0, ":"); 				// tmp = uid
    		uid = atoi(tmp);                     // This value is garbage
    		printf("uid= %d\n", uid); 
    		chuid(uid, gid); 	/* Set uid and gid - hardcoded hack*/ 

    		tmp = strtok(0, ":"); 				// tmp = fullname
    		printf("fullname= %s\n", tmp); 
    		tmp = strtok(0, ":"); 				// tmp = HOMEDIR
    		printf("HOME= %s\n", tmp); 
    		chdir(tmp); 		/* cd to HOME directory */ 

    		tmp = strtok(0, ":"); 				// tmp = uid
    		exec(tmp); 			/* execute programs in user's space */ 
    	}
    	else
    	{
    		printf("login failed, try again.\n"); 
    	}

    	close(fd); 	// Close /etc/passwd file

    }
}