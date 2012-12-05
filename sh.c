#include "ucode.c"
#include "StringHelpers.c"

char line[64], buf[128]; 
char *name[10]; 	// Hold 10 arguments from command line input delimited by ' '

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

PipeRedirect(char *cmd1, char *cmd2)
{
	int status; 
	int pd[2]; 
	if(0 == fork())
	{
		pipe(pd); 	// Create the pipe

		// Fork a child for reading 
		if(fork())
		{
			printf("child P%d closing pd[1]\n", getpid()); 
			close(pd[1]); 
			close(0); 	// Replace stdin with the pipe reader 
			dup(pd[0]); 
			exec(cmd2); 	// Execute the command 
			printf("%s failed to execute.\n", cmd2); 
		}
		else 
		{
			printf("parent P%d closing pd[0]\n", getpid()); 
			close(pd[0]); 
			close(1); 	// Replace stdout with the pipe writer 
			dup(pd[1]); 
			exec(cmd1); 
			printf("%s failed to execute.\n", cmd1); 

		}
	}
	else
	{
		// We are the shell process and must wait for child to die 
		wait(&status); 
	}

}

ExecRedirect(char *execFile, char * redirect, char *file_mode)
{
	int pid, status; 

	printf("ExecRedirect(%s, %s, 0x%x)\n", execFile, redirect, file_mode); 
	sleep(5); 

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
			//printf("4\n"); 
			chdir(name[1]); 
			getcwd(line); 
			printf("cd to %s\n", line); 
			continue; 
		}
		else if(strcmp(name[0], "pwd") == 0)
		{
			//printf("5\n"); 
			getcwd(tmp); 
			printf("%s\n", tmp); 
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

		if((j = GetIndex(line, "|") != -1))
		{
			line[j-1] = 0; 
			PipeRedirect(line, &line[j+2]); 
			continue; 
		}
		else if((j = GetIndex(line, ">>")) != -1)
		{
			if(j == 0)
			{
				printf("ERROR: >> cannot be the first character in a command.\n"); 
				continue; 
			}

			line[j-1] = 0; 
			ExecRedirect(line, &line[j+3], 1|0100); 	// Open file for WRITE or CREATE
			continue; 
		}
		else if((j = GetIndex(line, ">")) != -1)
		{
			if(j == 0)
			{
				printf("ERROR: > cannot be the first character in a command.\n"); 
				continue; 
			}

			pid = fork(); 
			if(pid == 0)
			{
				line[j-1] = 0; 
				printf("Forked a process to execute %s with IO to %s.\n", line, &line[j+2]); 

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
		else if((j = GetIndex(line, "<")) != -1)
		{
			if(j == 0)
			{
				printf("ERROR: < cannot be the first character in a command.\n"); 
				continue; 
			}

			pid = fork(); 
			if(pid == 0)
			{
				line[j-1] = 0; 
				printf("Forked a process to execute %s with IO to %s.\n", line, &line[j+2]); 

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





		pid = fork(); 
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