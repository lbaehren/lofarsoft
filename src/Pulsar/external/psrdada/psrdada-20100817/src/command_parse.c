#include "command_parse.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

// #define _DEBUG 1

int command_parse_help (void* context, FILE* fptr, char* arg)
{
  command_parse_t* parser = (command_parse_t*) context;
  unsigned icmd;
  
  fprintf (fptr, "Available commands:\n\n");

  for (icmd=0; icmd < parser->ncmd; icmd++)
    fprintf (fptr, "%s\t%s\n",
	     parser->cmds[icmd].name,
	     parser->cmds[icmd].help);

  return 0;
}

int command_parse_exit (void* context, FILE* fptr, char* arg)
{
  return COMMAND_PARSE_EXIT;
}

/* create a new command parser */
command_parse_t* command_parse_create ()
{
  command_parse_t* c = malloc (sizeof(command_parse_t));
  assert (c != 0);

  c -> cmds = 0;
  c -> ncmd = 0;
  c -> reply = stdout;

  command_parse_add (c, command_parse_help, c, "help", "print this list", 0);
  command_parse_add (c, command_parse_exit, c, "exit", "exit parser", 0);
  return c;
}

/* destroy a command parser */
int command_parse_destroy (command_parse_t* parser)
{
  unsigned icmd = 0;
  for (icmd=0; icmd<parser->ncmd; icmd++) {
    free (parser->cmds[icmd].name);
    free (parser->cmds[icmd].help);
    free (parser->cmds[icmd].more);
  }

  free (parser->cmds);
  free (parser);

  return 0;
}

/* set the stream to be used when replying */
int command_parse_reply (command_parse_t* parser, FILE* fptr)
{
  parser -> reply = fptr;
  return 0;
}

/* add a command to the list of available commands */
int command_parse_add (command_parse_t* parser, 
		       command cmd, void* context,
		       const char* command_name,
		       const char* short_help,
		       const char* long_help)
{
  if (!command_name) {
    fprintf (stderr, "command_parse_add: command name not provided\n");
    return -1;
  }

  parser->cmds = realloc (parser->cmds, (parser->ncmd+1)*sizeof(command_t));
  assert (parser->cmds != 0);

  parser->cmds[parser->ncmd].cmd = cmd;
  parser->cmds[parser->ncmd].context = context;

  parser->cmds[parser->ncmd].name = strdup(command_name);
  assert (parser->cmds[parser->ncmd].name != 0);

  if (short_help)  {
    parser->cmds[parser->ncmd].help = strdup(short_help);
    assert (parser->cmds[parser->ncmd].help != 0);
  }
  else
    parser->cmds[parser->ncmd].help = 0;

  if (long_help) {
    parser->cmds[parser->ncmd].more = strdup(long_help);
    assert (parser->cmds[parser->ncmd].more != 0);
  }
  else
    parser->cmds[parser->ncmd].more = 0;

  parser->ncmd ++;

  return 0;
}

/* remove a command from the list of available commands */
int command_parse_remove (command_parse_t* parser, const char* key)
{
  unsigned icmd = 0;

#ifdef _DEBUG
  fprintf (stderr, "command_parse_remove: '%s'\n", key);
#endif
  
  for (icmd=0; icmd < parser->ncmd; icmd++) {
    
#ifdef _DEBUG
    fprintf (stderr, "command_parse_remove: compare '%s'\n",
	     parser->cmds[icmd].name);
#endif
    
    if (strcmp (key, parser->cmds[icmd].name) == 0) {
#ifdef _DEBUG
      fprintf (stderr, "command_parse: match %d\n", icmd);
#endif
      break;
    }

  }

  if (icmd == parser->ncmd) {
    fprintf (stderr, "command_parse_remove: no command named '%s'\n", key);
    return -1;
  }

  free (parser->cmds[icmd].name);
  if (parser->cmds[icmd].help)
    free (parser->cmds[icmd].help);
  if (parser->cmds[icmd].more)
    free (parser->cmds[icmd].more);

  parser->ncmd --;

  for (; icmd < parser->ncmd; icmd++)
    parser->cmds[icmd] = parser->cmds[icmd+1];

  return 0;

}


/* parse a command */
int command_parse (command_parse_t* parser, char* command)
{
  return command_parse_output (parser, command, parser->reply);
}

/* parse a command */
int command_parse_output (command_parse_t* parser, char* cmd, FILE* out)
{
  const char* whitespace = " \r\t\n";
  unsigned icmd = 0;
  char* key = 0;

  /* skip leading whitespace */
  while (*cmd && strchr (whitespace, *cmd))
    cmd ++;

  key = strsep (&cmd, whitespace);

#ifdef _DEBUG
  fprintf (stderr, "command_parse: key '%s'\n", key);
#endif
  
  for (icmd=0; icmd < parser->ncmd; icmd++) {
    
#ifdef _DEBUG
    fprintf (stderr, "command_parse: compare '%s'\n", parser->cmds[icmd].name);
#endif
    
    if (strcmp (key, parser->cmds[icmd].name) == 0) {
#ifdef _DEBUG
      fprintf (stderr, "command_parse: match %d\n", icmd);
#endif

      /* skip leading whitespace */
      while (*cmd && strchr (whitespace, *cmd))
	cmd ++;

      /* cut trailing whitespace */
      key = strchr (cmd, '\0') -1 ;
      while (key > cmd && strchr (whitespace, *key)) {
	*key = '\0'; key --;
      }

      /* ignore null strings */
      if (! *cmd)
	cmd = 0;

      return parser->cmds[icmd].cmd (parser->cmds[icmd].context, out, cmd);
    }

  }

  return -1;
}

