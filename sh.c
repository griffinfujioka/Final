#include "ucode.c"
#include "StringHelpers.c"

char line[64], buf[128]; 
char *name[10]; 	// Hold 10 arguments from command line input delimited by separated by " "

Menu()
{
	printf("****************************************\n");
	printf("               MENU                     \n"); 
	printf("****************************************\n"); 
	printf("* ls     cd     pwd     cat     more   *\n"); 
	printf("* cp     mv     >        >>     mkdir  *\n"); 
	printf("* rmdir         creat    rm     chmod  *\n"); 
	printf("* <      |      help                   *\n"); 
	printf("****************************************\n"); 
}

// Execute a pipe command 
ExecPipe(char *cmd1, char *cmd2)
{
	int status, pid, ppid, r, n; 
	int pd[2];
	char pipe_line[256], *s = "griffin's data from pipe"; 
	ppid = getpid(); 
	printf("parent = P%d\n", ppid); 
	pid = fork(); 
	if(pid == 0)
	{
		r = pipe(pd); 	// Create two pipes: pd[0] = READ, pd[1] = WRITE
		printf("pd[0] = %d, pd[1] = %d\n", pd[0], pd[1]); 
		// Fork a child for reading 
		pid = fork(); 
		printf("pid = P%d\n", pid); 
		if(pid)
		{
			// READER 
			printf("child P%d close pd[1] to READ from pipe.\n", getpid()); 
			close(pd[1]); 				// Close second pipe descriptor 
			n = read(pd[0], pipe_line, 256); 
			line[n] = 0; 
			printf("%s\n", line); 
		}
		else 
		{
			// WRITER
			printf("parent P%d closing pd[0]\n", getpid()); 
			close(pd[0]); 				// Close the first pipe descriptor 
			while(1)
			{
				sleep(1); 
				printf("child P%d writing pipe : %s\n", pid, s); 
				write(pd[1], s, strlen(s)); 
			}

		}
	}
	else
	{
		// Wait for child to die 
		wait(&status); 
	}

}

// Execute a redirection command 
ExecRedirect(char *execFile, char * redirect, char *file_mode)
{
	int pid, status; 

	printf("ExecRedirect(%s, %s, %x)\n", execFile, redirect, file_mode); 

	pid = fork();
	if(pid == 0)
	{
		// Setup IO redirection so output is written to file 
		close(1); 	//  Replace stdout with the file 
		if(open(redirect, file_mode) != 1)
		{
			printf("ERROR: Could not open %s for IO redirection (attempted for 0x%x).\n", redirect, file_mode); 
			exit(-1); 
		}
		exec(execFile); 
		printf("ERROR: exec %s failed.\n", execFile); 
	}
	else
	{
		wait(&status); 	// Wait for child to die 
	}
}

main(int argc, char* argv)
{
	//int stdin, stdout, stderr; 
	int i, j, pid, cid, me, status; 
	char tmp[64]; 

	me = getpid(); 
	while(1)
	{
		printf("gpf-shell:~$ "); 
		gets(line); 	// Read a line into line[] buffer
		if(line[0] == 0)
			continue; 

		strcpy(tmp, line);  // Copy user input into tmp so we can tokenize it
		
		// Tokenize the input line and put each word into name[0], name[1]... 
		i = 0; 
		name[i] = strtok(tmp, " \n");  	// name[0] = command 
		while(i < 9)
		{
			i++; 
			name[i] = strtok(0, " \n"); // name[1-9] = parameters 
		}

		// Handle special cases first: help, logout, cd and pwd 
		// cd and pwd must be handeled because they are system calls and not user programs like
		// the others we wrote for this assignment (cp, grep, cat, more, etc.)
		if(strcmp(name[0], "help") == 0)
		{
			Menu(); 
			continue; 
		}
		else if(strcmp(name[0], "logout") == 0)
		{
			exit(0); 
		}
		else if(strcmp(name[0], "cd") == 0)
		{
			chdir(name[1]); 
			getcwd(line); 
			printf("cd to %s\n", line); 
			continue; 
		}
		else if(strcmp(name[0], "pwd") == 0)
		{
			getcwd(tmp); 
			printf("%s\n", tmp); 
			continue; 
		}

		// Handle IO redirection 
		if(StringContains(line, ">") && StringContains(line, "|"))
		{
			printf("ERROR: Pipe and redirection is not supported within the same command.\n"); 
			continue; 
		}

		if(CountOccurences(line, "|") > 1)
		{
			// Multiple pipes
			printf("ERROR: Support for multiple pipes is not included in this build.\n"); 
			continue; 
		}

		if((j = GetIndex(line, "|") != -1)) 	// Pipe handling 
		{
			line[j-1] = 0; 		// First part of pipe (Ex. 'cat filename\0')
			ExecPipe(line, &line[j+2]); 	// &line[j+2] = second part of pipe (Ex. 'more\0')
			continue; 
		}
		else if((j = GetIndex(line, ">>")) != -1) 	// Append handling 
		{
			if(j == 0)
			{
				printf("[USAGE]: >> cannot be the first character in a command.\n"); 
				continue; 
			}

			line[j-1] = 0; 	// (Ex. 'cat filename\0')
			ExecRedirect(line, &line[j+3], 1|0100); 	// &line[j+3] = (Ex. 'appendFile\0') - Open file for WRITE or CREATE
			continue; 
		}
		else if((j = GetIndex(line, ">")) != -1)  		// Write contents to new file handling 
		{
			if(j == 0)
			{
				printf("[USAGE]: > cannot be the first character in a command.\n"); 
				continue; 
			}

			pid = fork(); 
			if(pid == 0)
			{
				line[j-1] = 0;  // (Ex. 'cat filename')
				printf("Forked a process to execute %s with IO to %s.\n", line, &line[j+2]); // &line[j+2] = 'newFile\0'

				// Write stdout to file 
				close(1); 		// Replace stdout with file 
				if((open(&line[j+2], 1|0100)) != 1)
				{
					printf("ERROR: Could not open %s for output redirection.\n", &line[j+2]); 
					exit(-1); 
				}

				exec(line); 
				continue; 
			}
			else
			{
				// Wait for child to die 
				wait(&status); 
				continue; 
			}
		}
		else if((j = GetIndex(line, "<")) != -1) // Ready inputs from file 
		{
			if(j == 0)
			{
				printf("[USAGE]: < cannot be the first character in a command.\n"); 
				continue; 
			}

			pid = fork(); 
			if(pid == 0)
			{
				line[j-1] = 0;  // (Ex. a.out)
				printf("Forked a process to execute %s with IO to %s.\n", line, &line[j+2]); // &line[j+2] = 'inFile\0'

				// Read from file as stdin 
				close(0); 
				if(open(&line[j+2] , 0) != 0)
				{
					printf("ERROR: Could not open %s to read as input.\n", &line[j+2]); 
					exit(-1); 
				}

				exec(line); 
				continue; 
			}
			else
			{
				// Wait for child to die 
				wait(&status); 
				continue; 
			}
		}



		// Here is where we handle all other commands besides help, logout, pwd and cd 
		pid = fork(); 	// Fork a child to exec to the command 
		if(pid)
		{
			printf("parent sh waits for child to die\n"); 
			pid = wait(&status); 	
			printf("pid = %d\n"); 
		}
		else 
		{
			printf("forked task %d to exec %s\n", getpid(), name[0]); 
			exec(line); 
			printf("exec failed.\n"); 
		}
	}
}