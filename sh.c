#include "ucode.c"

char line[64], buf[128]; 
char *name[10]; 	// Holds 10 paramaters from command line input 

Menu()
{
	printf("******** menu ********\n"); 
	printf("ls   cd   pwd   cat   more   \n"); 
	printf("cp   mv   >      >>   mkdir\n"); 
	printf("rmdir     creat  rm   chmod\n"); 
	printf("<    |    help\n"); 
	printf("**********************\n"); 
}

one(int begin, int end)
{
	int i = begin; 
	int j = end; 
	int found = 0; 

	while(i <= j && !found)
	{
		if(!strcmp(name[i], ">"))
		{
			found = 1; 
			close(1); 	
			open(name[i+1], 1|0100); 	// Open file for WRITE or CREATE
		}
		else if(!strcmp,name[i], ">>")
		{
			found = 1; 
			close(1); 
			open(name[i+1], 02000|1|0100); // Open file for APPEND, WRITE or CREATE
		}
		else if(!strcmp(name[i], "<"))
		{
			found = 1; 
			close(0); 
			open(name[i+1], 0); 	// Open file for READ 
			printf("%s\n is opened for READ.\n"); 
			getc(); 
		}
		i++; 
	}
	if(found)
		i--; 

	strcpy(buf, name[begin]); 
	begin++; 

	while(begin < i)
	{
		strcat(buf, " "); 
		strcat(buf, name[begin]); 
		begin++; 
	}

	exec(buf); 
}

multiple(int end)
{
	int j = end; 
	int isPipe = 0, pid; 
	int pipin[2]; 

	while(j >= 0 && !isPipe)
	{
		if(!strcmp(name[j], "|"))
			isPipe = 1; 

		j--; 
	}

	if(!isPipe)
		one(0, end); 
	else
	{
		pipe(pipin); 	// Create pipe 
		pid = fork(); 	// Fork a new process
		if(pid)
		{
			close(pipin[1]); 
			close(0); 
			dup2(pipin[1], 1); 	// Dupilicate file descriptor pipin[1] to 1 
			multiple(j); 
		}
	}
}

main(int argc, char* argv)
{
	int stdin, stdout, stderr; 
	int i, j, pid, cid, me, status; 
	char tmp[64]; 

	me = getpid(); 
	while(1)
	{
		printf("GPF-SH: "); 
		gets(line); 	// Read a line into line[] buffer
		if(line[0] == 0)
			continue; 

		strcpy(tmp, line);  // Copy user input into tmp 
		//printf("line = %s\n", line); 
		i = 0; 
		name[i] = strtok(tmp, " \n");  	// name[0] = command 
		while(i < 9)
		{
			i++; 
			name[i] = strtok(0, " \n"); // name[1-9] = parameters 
			//printf("name[%d] = %s\n", i, name[i] ? name[i] : ""); 
		}

		//printf("1\n"); 
		if(strcmp(name[0], "help") == 0)
		{
			Menu(); 
			continue; 
		}

		//printf("2\n"); 
		if(strcmp(name[0], "logout") == 0)
		{
			exit(0); 
		}

		//printf("3\n"); 
		if(strcmp(name[0], "cd") == 0)
		{
			//printf("4\n"); 
			chdir(name[1]); 
		}
		else if(strcmp(name[0], "pwd") == 0)
		{
			//printf("5\n"); 
			getcwd(tmp); 
			printf("%s\n", tmp); 
		}
		else
		{
			pid = fork(); 
			if(pid)
			{
				printf("parent sh waits for child to die\n"); 
				pid = wait(&status); 	
				printf("pid = %d\n"); 
			}
			else 
				multiple(i-1); 
		}
	}
}