//*************************************************************************
//                      Logic of init.c 
// NOTE: this init.c creates only ONE login process on console=/dev/tty0
// YOUR init.c must also create login processes on serial ports /dev/ttyS0
// and /dev/ttyS1.. 
//************************************************************************

int pid, child, status, s0, s1;
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
  child = fork();
  
  if (child)
    parent();
  else  // fork a couple of login task 
  {
    login();    // fork a login task in the console 
    loginS0();  // fork a login task in serial port 0 
  }
    
  
  
}       

int login()
{
  exec("login /dev/tty0");
}

int loginS0() // Run the login program in serial port 0 
{
  stdin = open("/dev/ttyS0", 0); 
  stdout = open("/dev/ttyS0", 1); 
  stderr = open("/dev/ttyS0", 1); 
  printf("GRIFFIN-init: fork a login task in serial port 0\n"); 
  s0 = fork(); 

  if(s0)
    parent();
  else
    exec("login /dev/ttyS0"); 
}

int loginS1() // Run the login program in serial port 1
{
  exec("login /dev/ttyS1"); 
}
      
int parent()
{
  while(1){
    printf("KCINIT : waiting .....\n");

    pid = wait(&status);

    
    if (pid == child)
      loginS0(); 
    else
      printf("INIT : buried an orphan child %d\n", pid);
    
  }
}


// Things are getting pretty hacky around here... 
int parent0()
{
  while(1){
    printf("KCINIT : waiting .....\n");

    pid = wait(&status);

    
    if (pid == child)
      loginS0(); 
    else
        printf("INIT : buried an orphan child %d\n", pid);
    
  }
}