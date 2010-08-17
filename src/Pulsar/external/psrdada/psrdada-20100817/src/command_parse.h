#ifndef __DADA_command_parse_h
#define __DADA_command_parse_h

#include <stdio.h>

/* pointer to a function that implements a command */
typedef int (*command) (void* context, FILE* reply, char* arguments);

#define COMMAND_PARSE_EXIT 13

typedef struct {

  /* pointer to the command function */
  command cmd;

  /* additional context for the command */
  void* context;

  /* name of the command */
  char* name;

  /* help for the command */
  char* help;

  /* detailed description of the command */
  char* more;

} command_t;


typedef struct {

  /* an array of commands */
  command_t* cmds;

  /* size of the array */
  unsigned ncmd;

  /* I/O stream to be used when replying */
  FILE* reply;

} command_parse_t;


/* create a new command parser */
command_parse_t* command_parse_create ();

/* destroy a command parser */
int command_parse_destroy (command_parse_t* parser);

/* set the stream to be used when replying */
int command_parse_reply (command_parse_t* parser, FILE* fptr);

/* add a command to the list of available commands */
int command_parse_add (command_parse_t* parser, 
		       command cmd, void* context,
		       const char* command_name,
		       const char* short_help,
		       const char* long_help);

/* remove a command from the list of available commands */
int command_parse_remove (command_parse_t* parser, const char* command_name);

/* parse a command */
int command_parse (command_parse_t* parser, char* command);

/* parse a command; different output */
int command_parse_output (command_parse_t* parser, char* command, FILE* out);

#endif
