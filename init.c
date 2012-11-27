//*************************************************************************
//                      Logic of init.c 
// NOTE: this init.c creates only ONE login process on console=/dev/tty0
// YOUR init.c must also create login processes on serial ports /dev/ttyS0
// and /dev/ttyS1.. 
//************************************************************************

int pid, child, status;
int stdin, stdout;

#include "ucode.c"  //<========== AS POSTED on class website


main(int argc, char *argv[])
{
  // 1. open /dev/tty0 as 0 (READ) and 1 (WRITE) in order to display messages
  stdin  = open("/dev/tty0", 0);
  stdout = open("/dev/tty0", 1);


  printf("GRIFFIN-init: fork a login task on console\n"); 

  // 2. Now we can use printf, which calls putc(), which writes to stdout
  //printf("KCINIT : fork a login task on console\n"); 
  child = fork();
   
  
  if (child)
    parent();
  else  // login task
    login();
  
}       

int login()
{
  exec("login /dev/tty0");
}
      
int parent()
{
  while(1){
    printf("KCINIT : waiting .....\n");

    pid = wait(&status);

    
    if (pid == child)
      login(); 
    else
        printf("INIT : buried an orphan child %d\n", pid);
    
  }
}