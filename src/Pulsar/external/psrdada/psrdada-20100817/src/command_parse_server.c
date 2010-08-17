#include "command_parse_server.h"
#include "sock.h"

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

// #define _DEBUG 1

/* create a new command parse server */
command_parse_server_t* 
command_parse_server_create (command_parse_t* parser)
{
  command_parse_server_t* server = 0;

  if (!parser)
    return 0;

  server = (command_parse_server_t*) malloc (sizeof(command_parse_server_t));
  assert (server != 0);

  server -> parser = parser;
  server -> welcome = 0;
  server -> prompt = strdup ("> ");
  assert (server->prompt != 0);

  server -> ok = strdup ("ok\n");
  assert (server->ok != 0);

  server -> fail = strdup ("fail\n");
  assert (server->fail != 0);

  server -> port = 0;

  server -> quit = 0;

  pthread_mutex_init(&(server->mutex), NULL);
  
  return server;
}

/* destroy a command parse server */
int command_parse_server_destroy (command_parse_server_t* server)
{
  if (!server)
    return -1;

  if (server->welcome)
    free (server->welcome);
  if (server->prompt)
    free (server->prompt);
  if (server->ok)
    free (server->ok);
  if (server->fail)
    free (server->fail);

  free (server);
  return 0;
}

typedef struct {

  /* command parse server */
  command_parse_server_t* server;

  /* I/O stream from which to read commands */
  FILE* input;

  /* I/O stream to which output is written */
  FILE* output;

} command_parse_thread_t;

/*! this thread does the actual command I/O */
static void* command_parser (void * arg)
{
  command_parse_thread_t* parser = (command_parse_thread_t*) arg;
  command_parse_server_t* server = parser->server;

  /* increased from 1024 to 4096 to allow for long headers */
  char buffer [4096];
  int ret;

#ifdef _DEBUG
  fprintf (stderr, "command_parser: server=%p\n", server);
  fprintf (stderr, "command_parser: parser=%p\n", server->parser);
  fprintf (stderr, "command_parser: input=%p\n",  parser->input);
  fprintf (stderr, "command_parser: output=%p\n", parser->output);
#endif

  assert (server != 0);
  assert (server->parser != 0);
  assert (parser->input  != 0);
  assert (parser->output != 0);

  if (server->welcome) {
#ifdef _DEBUG
    fprintf (stderr, "command_parser: print welcome %s\n", server->welcome);
#endif
    fprintf (parser->output, "%s\n", server->welcome);
  }

  while ((!feof (parser->output)) && (!server->quit)) {

    if (server->prompt)
      fprintf (parser->output, server->prompt);

    if (!fgets (buffer, 4096, parser->input) || feof (parser->input))
      break;

    ret = command_parse_output (server->parser, buffer, parser->output);

    if (ret == COMMAND_PARSE_EXIT)
      break;

    if (ret < 0 && server->fail)
      fprintf (parser->output, server->fail);

    if (ret >= 0 && server->ok)
      fprintf (parser->output, server->ok);

  }

#ifdef _DEBUG
  fprintf (stderr, "command_parser: closing\n");
#endif

  /* if the server has been asked to quit, kludgily close the socket */
  if (server->quit) 
    sock_close(server->listen_fd);

  fclose (parser->input);
  fclose (parser->output);

  free (parser);

  return 0;
}

static void* command_parse_server (void * arg)
{
  command_parse_server_t* server = (command_parse_server_t*) arg;
  command_parse_thread_t* parser;

  server->listen_fd = 0;
  int comm_fd = 0;
  int port = server->port;
  FILE* fptr = 0;

  pthread_t tmp_thread;

#ifdef _DEBUG
  fprintf (stderr, "command_parse_server: server=%p\n", server);
  fprintf (stderr, "command_parse_server: sock_create (port=%d)\n", port);
#endif

  server->listen_fd = sock_create (&port);

  if (server->listen_fd < 0 && errno == EADDRINUSE) {
    port = 0;
    server->listen_fd = sock_create (&port);
    if (server->listen_fd > -1)
      fprintf (stderr, "command_parse_server: port=%d\n", port);
  }

  if (server->listen_fd < 0)  {
    perror ("command_parse_server: Error creating socket");
    return 0;
  }

  while (!server->quit) {

#ifdef _DEBUG
    fprintf (stderr, "command_parse_server: sock_accept (fd=%d)\n", server->listen_fd);
#endif

    comm_fd = sock_accept (server->listen_fd);

    if (comm_fd < 0)  
    {
      if (!server->quit) 
      {
        perror ("command_parse_server: Error accepting connection");
        sleep(1);
      }
      else
        return 0;

    } else  {

#ifdef _DEBUG
      fprintf (stderr, "command_parse_server: connection accepted\n");
#endif

      parser = (command_parse_thread_t*) malloc (sizeof(command_parse_thread_t));
      assert (parser != 0);

      parser->server = server;

#ifdef _DEBUG
      fprintf (stderr, "command_parse_server: fdopen (fd=%d,r)\n", comm_fd);
#endif

      fptr = fdopen (comm_fd, "r");
      if (!fptr)  {
        perror ("command_parse_server: Error creating I/O stream");
        return 0;
      }

      /* do not buffer the input */
      setbuf (fptr, 0);

      parser->input = fptr;
#ifdef _DEBUG
      fprintf (stderr, "command_parse_server: input=%p\n", parser->input);
      fprintf (stderr, "command_parse_server: fdopen (fd=%d,w)\n", comm_fd);
#endif

      fptr = fdopen (comm_fd, "w");
      if (!fptr)  {
        perror ("command_parse_server: Error creating I/O stream");
        return 0;
      }

      /* do not buffer the output */
      setbuf (fptr, 0);

      parser->output = fptr;

#ifdef _DEBUG
      fprintf (stderr, "command_parse_server: output=%p\n", parser->output);
      fprintf (stderr, "command_parse_server: pthread_create command_parser\n");
#endif

      if (pthread_create (&tmp_thread, 0, command_parser, parser) < 0) {
        perror ("command_parse_serve: Error creating new thread");
        return 0;
      }

#ifdef _DEBUG
      fprintf (stderr, "command_parse_server: pthread_detach\n");
#endif

      /* thread cannot be joined; resources will be destroyed on exit */
      pthread_detach (tmp_thread);
    }

  }

  return 0;
}

/* set the welcome message */
int command_parse_server_set_welcome (command_parse_server_t* s, const char* w)
{
  if (!s)
    return -1;

  if (s->welcome)
    free (s->welcome);
  if (w) {
    s->welcome = strdup (w);
    assert (s->welcome != 0);
  }
  else
    s->welcome = 0;

  return 0;
}

/* set the prompt */
int command_parse_server_set_prompt (command_parse_server_t* s, const char* p)
{
  if (!s)
    return -1;

  if (s->prompt)
    free (s->prompt);
  if (p)  {
    s->prompt = strdup (p);
    assert (s->prompt != 0);
  }
  else
    s->prompt = 0;

  return 0;
}

int command_parse_serve (command_parse_server_t* server, int port)
{
#if 0
  sighandler_t handler = 
#endif

signal (SIGPIPE, SIG_IGN);

#if 0
  if (handler != SIG_DFL)
    signal (SIGPIPE, handler);
#endif

#ifdef _DEBUG
  fprintf (stderr, "command_parse_serve: server=%p\n", server);
#endif

  server->port = port;

  if (pthread_create (&(server->thread), 0, command_parse_server, server) < 0)
    {
      perror ("command_parse_serve: Error creating new thread");
      return -1;
    }

  return 0;
}

