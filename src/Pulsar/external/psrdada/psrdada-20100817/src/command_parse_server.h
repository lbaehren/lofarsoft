#ifndef __DADA_command_parse_server_h
#define __DADA_command_parse_server_h

#include "command_parse.h"
#include <pthread.h>

typedef struct {

  /* the command parser */
  command_parse_t* parser;

  /* the welcome message presented to incoming connection */
  char* welcome;

  /* the prompt presented during interactive session */
  char* prompt;

  /* the message presented when all is good */
  char* ok;

  /* the message presented on error */
  char* fail;

  /* for multi-threaded use of the command_parse */
  pthread_mutex_t mutex;

  /* the identifier of the command_parse_server thread */
  pthread_t thread;

  /* the port on which command_parse_server is listening */
  int port;

  /* socket on which connections are made */
  int listen_fd;

  /* flag to control graceful exit of the server */
  int quit;

} command_parse_server_t;

/* create a new command parse server */
command_parse_server_t* command_parse_server_create (command_parse_t* parser);

/* destroy a command parse server */
int command_parse_server_destroy (command_parse_server_t* server);

/* set the welcome message */
int command_parse_server_set_welcome (command_parse_server_t*, const char*);

/*! Start another thread to receive incoming command socket connections */
int command_parse_serve (command_parse_server_t* server, int port);

#endif
