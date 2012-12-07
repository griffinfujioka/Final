//*************************************************************************
//                      Logic of init.c 
// NOTE: this init.c creates only ONE login process on console=/dev/tty0
// YOUR init.c must also create login processes on serial ports /dev/ttyS0
// and /dev/ttyS1.. 
//************************************************************************

int pid, child, status, s0, s1;
int s0status, s1status; 
int s0in, s0out;  
int stdin, stdout, stderr; 

#include "ucode.c"  //<========== AS POSTED on class website


main(int argc, char *argv[])
{

  // 1. open /dev/tty0 as 0 (READ) and 1 (WRITE) in order to display messages
  stdin  = open("/dev/tty0", 0);
  stdout = open("/dev/tty0", 1);
  stderr = open("/dev/tty0", 1);  

  // 2. Now we can use printf, which calls putc(), which writes to stdout
  printf("GRIFFIN-init: fork a login task in the console\n"); 
  printf("GRIFFIN-init: fork a login task in serial port 0\n"); 
  //printf("GRIFFIN-init: fork a login task in serial port 1\n"); 

  child = fork();
  printf("init forked child process = P%d\n", child); 
  if (child)  // Fork failed, you're still parent process
  {
    loginS0();    // Fork a login task to serial port 0 
    parent();     // Wait for task to die 
  }
  else    // Successful fork 
  {
    login();    // fork a login task in the console 
  }  
}       

int login()
{
  exec("login /dev/tty0");
}

int loginS0() // Run the login program in serial port 0 
{
  stdin = open("/dev/ttyS0", 0);    // Open serial port 0 as stdin for READ
  stdout = open("/dev/ttyS0", 1);   // Open serial port 0 as stdout for WRITE 
  stderr = open("/dev/ttyS0", 1);   
  
  s0 = fork(); 

  if(s0) // Fork failed 
  {
    //loginS1(); 
    parent0();   // Wait 
  }
  else
    exec("login /dev/ttyS0"); 
}

int loginS1() // Run the login program in serial port 1
{
  stdin = open("/dev/ttyS1", 0);    // Open serial port 1 as stdin for READ  
  stdout = open("/dev/ttyS1", 1);   // Open serial port 1 as stdout for WRITE   
  stderr = open("/dev/ttyS1", 1);   
  //printf("Entered loginS1() to fork login task to serial port 1\n"); 
  
  s1 = fork(); 

  if(s1)
  {
    //printf("fork() failed to create new process to execute login task in serial port 1\n"); 
    parent1(); 
  } 
  else
  {
    //printf("forked() child P%d to execute login task in serial port 1\n", getpid()); 
    exec("login /dev/ttyS1"); 
  }
    
}
      
int parent()
{
  while(1){
    //printf("GRIFFIN-init : waiting .....\n");

    pid = wait(&status);

    
    if (pid == child)   // If the login task died
      login();          // fork a new one I
    else
      printf("GRIFFIN-init : buried an orphan child %d\n", pid);
    
  }
}


// Things are getting pretty hacky around here... 
int parent0()
{
  while(1){
    //printf("GRIFFIN-init : waiting .....\n");

    pid = wait(&status);

    
    if (pid == s0)
      loginS0(); 
    else
        printf("GRIFFIN-init : buried an orphan child %d\n", pid);
    
  }
}

int parent1()
{
  while(1){
    //printf("GRIFFIN-init : waiting .....\n");

    pid = wait(&status);

    
    if (pid == s1)
      loginS1(); 
    else
      printf("GRIFFIN-init : buried an orphan child %d\n", pid);
    
  }
}

