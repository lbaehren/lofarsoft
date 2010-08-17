#include "sock.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <math.h>

void usage();


void usage() {
  fprintf(stdout,
    "ibob_configurator [options] host port mac\n"
    " host        of the ibob telnet interface\n"
    " port        of the ibob telnet interace\n"
    " mac         on which the ibob interace is accessible\n"
    "\n"
    " -a len      accumulation length to set [default %d]\n"
    " -v          verbose\n"
    " -h          print help text\n",25);

}

int main (int argc, char** argv)
{

  char * hostname;
  char * mac_addr;
  int port;
  int verbose = 0;
  int n_attempts = 3;
  int arg = 0;
  int acc_len = 25;

  char macone[10];
  char mactwo[10];
  
  while ((arg=getopt(argc,argv,"a:vh")) != -1) {
    switch (arg) {

    case 'v':
      verbose = 1;
      break;

    case 'a':
      acc_len = atoi(optarg);
      break;

    case 'h':
      usage();
      return 0;

    default:
      usage();
      return 0;
    }
  }

  if ((argc - optind) != 3) {
    fprintf(stderr, "Error: host, port and mac address  must be specified\n");
    usage();
    return EXIT_FAILURE;
  } else {
    hostname = argv[optind];
    port = atoi(argv[(optind+1)]);
    mac_addr = argv[(optind+2)];
  }

  if (strlen(mac_addr) !=  12) {
    fprintf(stderr, "Mac address was not 12 characters long\n");
    return EXIT_FAILURE;
  }    

  int i=0; 
  for( i = 0; i<23; i++)
    mac_addr[i] = tolower(mac_addr[i]);

  strcpy(macone, "x00000000");
  strcpy(mactwo, "x00000000");

  strncpy(macone+5,mac_addr,4);
  strncpy(mactwo+1,mac_addr+4,8);

  //fprintf(stderr, "mac strings = %s, %s\n", macone, mactwo);

  int bytes = 0;

  char command[100];
  char *response = malloc(sizeof(char) * 10000);
  int fd = sock_open(hostname, port);

  if (fd < 0)  {
    fprintf(stderr, "Error creating socket\n");
    return -1;
  } else {
    if (verbose) fprintf(stdout, "opened socket to %s on port %d [fd %d]\n",hostname, port, fd);
  }

  sprintf(command, "regwrite reg_ip 0x0a000004\r\n");
  if (verbose) fprintf(stdout, "<- %s",command);
  if (to_ibob(fd, command)) return EXIT_FAILURE;
  bytes = from_ibob(fd, response);
  if (verbose) fprintf(stdout, "-> [%d] %s\n",bytes, response);

  sprintf(command, "regwrite reg_10GbE_destport0 4001\r\n");
  if (verbose) fprintf(stdout, "<- %s",command);
  if (to_ibob(fd, command)) return EXIT_FAILURE;
  bytes = from_ibob(fd, response);
  if (verbose) fprintf(stdout, "-> [%d] %s\n",bytes, response);

  sprintf(command, "write l xd0000000 xffffffff\r\n");
  if (verbose) fprintf(stdout, "<- %s",command);
  if (to_ibob(fd, command)) return EXIT_FAILURE;
  bytes = from_ibob(fd, response);
  if (verbose) fprintf(stdout, "-> [%d] %s\n",bytes, response);

  sprintf(command, "setb x40000000\r\n");
  if (verbose) fprintf(stdout, "<- %s",command);
  if (to_ibob(fd, command)) return EXIT_FAILURE;
  bytes = from_ibob(fd, response);
  if (verbose) fprintf(stdout, "-> [%d] %s\n",bytes, response);

  sprintf(command, "writeb l 0 x00000060\r\n");
  if (verbose) fprintf(stdout, "<- %s",command);
  if (to_ibob(fd, command)) return EXIT_FAILURE;
  bytes = from_ibob(fd, response);
  if (verbose) fprintf(stdout, "-> [%d] %s\n",bytes, response);

  sprintf(command, "writeb l 4 xdd47e301\r\n");
  if (verbose) fprintf(stdout, "<- %s",command);
  if (to_ibob(fd, command)) return EXIT_FAILURE;
  bytes = from_ibob(fd, response);
  if (verbose) fprintf(stdout, "-> [%d] %s\n",bytes, response);

  sprintf(command, "writeb l 8 x00000000\r\n");
  if (verbose) fprintf(stdout, "<- %s",command);
  if (to_ibob(fd, command)) return EXIT_FAILURE;
  bytes = from_ibob(fd, response);
  if (verbose) fprintf(stdout, "-> [%d] %s\n",bytes, response);

  sprintf(command, "writeb l 12 x0a000001\r\n");
  if (verbose) fprintf(stdout, "<- %s",command);
  if (to_ibob(fd, command)) return EXIT_FAILURE;
  bytes = from_ibob(fd, response);
  if (verbose) fprintf(stdout, "-> [%d] %s\n",bytes, response);

  sprintf(command, "writeb b x16 x0f\r\n");
  if (verbose) fprintf(stdout, "<- %s",command);
  if (to_ibob(fd, command)) return EXIT_FAILURE;
  bytes = from_ibob(fd, response);
  if (verbose) fprintf(stdout, "-> [%d] %s\n",bytes, response);

  sprintf(command, "writeb b x17 xa0\r\n");
  if (verbose) fprintf(stdout, "<- %s",command);
  if (to_ibob(fd, command)) return EXIT_FAILURE;
  bytes = from_ibob(fd, response);
  if (verbose) fprintf(stdout, "-> [%d] %s\n",bytes, response);

  sprintf(command, "writeb l x3020 %s\r\n",macone);
  if (verbose) fprintf(stdout, "<- %s",command);
  if (to_ibob(fd, command)) return EXIT_FAILURE;
  bytes = from_ibob(fd, response);
  if (verbose) fprintf(stdout, "-> [%d] %s\n",bytes, response);

  sprintf(command, "writeb l x3024 %s\r\n",mactwo);
  if (verbose) fprintf(stdout, "<- %s",command);
  if (to_ibob(fd, command)) return EXIT_FAILURE;
  bytes = from_ibob(fd, response);
  if (verbose) fprintf(stdout, "-> [%d] %s\n",bytes, response);

  sprintf(command, "writeb b x15 xff\r\n");
  if (verbose) fprintf(stdout, "<- %s",command);
  if (to_ibob(fd, command)) return EXIT_FAILURE;
  bytes = from_ibob(fd, response);
  if (verbose) fprintf(stdout, "-> [%d] %s\n",bytes, response);

  sprintf(command, "write l xd0000000 x0\r\n");
  if (verbose) fprintf(stdout, "<- %s",command);
  if (to_ibob(fd, command)) return EXIT_FAILURE;
  bytes = from_ibob(fd, response);
  if (verbose) fprintf(stdout, "-> [%d] %s\n",bytes, response);

  sprintf(command, "regwrite reg_acclen %d\r\n", (acc_len-1));
  if (verbose) fprintf(stdout, "<- %s",command);
  if (to_ibob(fd, command)) return EXIT_FAILURE;
  bytes = from_ibob(fd, response);
  if (verbose) fprintf(stdout, "-> [%d] %s\n",bytes, response);

  sprintf(command, "regwrite reg_sync_period %d\r\n", (100 * acc_len * 2048));
  if (verbose) fprintf(stdout, "<- %s",command);
  if (to_ibob(fd, command)) return EXIT_FAILURE;
  bytes = from_ibob(fd, response);
  if (verbose) fprintf(stdout, "-> [%d] %s\n",bytes, response);

  sock_close (fd);
  return 0;
}

int to_ibob(int fd, char * command) {
  int length = strlen(command) - 1;

  int bytes_written = sock_write(fd, command, strlen(command)-1);

  if (bytes_written == 0) {
    fprintf(stderr, "could not write command %s\n",command);
    sock_close(fd);
    return 1;
  }
  return 0;
}

int from_ibob(int fd, char *response) {

  int bytes_read = 0;
  int rval = 1;
  while (rval > 0) {
    rval = sock_tm_read(fd, (void *) (response + bytes_read), 4096, 0.1);
    bytes_read += rval;
  }

  int i=0;
  for (i=0; i<bytes_read; i++) {
    if (response[i] == '\r')
       response[i] = ' ';     
    if (response[i] == '\n')
       response[i] = ',';     
  }
  
  response[bytes_read] = '\0';

  return bytes_read;
}

