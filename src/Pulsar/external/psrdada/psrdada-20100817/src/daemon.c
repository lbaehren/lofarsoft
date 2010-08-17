#include "daemon.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PS_IOERR 1

void be_a_daemon ()
{
  pid_t pid = fork();

  if (pid < 0)
    exit(EXIT_FAILURE);
  
  if (pid > 0)
    exit(EXIT_SUCCESS);
  
  /* Create a new SID for the child process */
  if (setsid() < 0)
    exit (EXIT_FAILURE);
  
  /* Change the current working directory */
  if (chdir("/") < 0)
    exit (EXIT_FAILURE);
  
  /* Close out the standard file descriptors */
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
}


int be_a_daemon_with_log(char * logfile) {

  /* become a daemon */
  be_a_daemon();

  int fd = 0;

  if ((fd = open("/dev/null", O_RDWR)) < 0) {   /* stdin */
    return (PS_IOERR);
  }

  /* redirect STDOUT and STDERR to the logfile */
  if (logfile) {
    if ((fd = open(logfile, O_CREAT|O_WRONLY|O_APPEND, 0666)) < 0)  /* stdout */
      return(PS_IOERR);
  }

  if (dup(fd) < 0) {        /* stderr */
    return(PS_IOERR);
  }

  return 0;

}
