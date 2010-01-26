/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "CommandParser.h"

#ifdef HAVE_READLINE
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include <stdlib.h>
#include <string.h>

using namespace std;

string CommandParser::readline ()
{
  string command;

#ifdef HAVE_READLINE
  char* cmd = 0;
  while (cmd == 0)
    cmd = ::readline (prompt.c_str());

  add_history (cmd);
  command = cmd;
  free (cmd);
#else
  cout << prompt;
  getline (cin, command);
#endif

  return command;
}

static const CommandParser* parser = 0;

void CommandParser::initialize_readline (const char* prog_name)
{
#ifdef _DEBUG
  cerr << "CommandParser::initialize_readline" << endl;
#endif

#ifdef HAVE_READLINE
  /* Allow conditional parsing of the ~/.inputrc file. */
  rl_readline_name = const_cast<char*> (prog_name);

  /* Tell the completer that we want a crack first. */
  rl_attempted_completion_function = CommandParser::completion;
#endif

  parser = this;
}

/* Attempt to complete on the contents of TEXT.  START and END
   bound the region of rl_line_buffer that contains the word to
   complete.  TEXT is the word to complete.  We can use the entire
   contents of rl_line_buffer in case we want to do some simple
   parsing.  Returnthe array of matches, or NULL if there aren't any. */
char** CommandParser::completion (const char *text, int start, int end)
{
  /* If this word is at the start of the line, then it is a command
     to complete.  Otherwise it is the name of a file in the current
     directory. */
  if (start > 0)
    return 0;

#ifdef _DEBUG
  cerr << "CommandParser::completion" << endl;
#endif

#ifdef HAVE_READLINE
  return rl_completion_matches (text, &command_generator);
#else
  return 0;
#endif
}

/* Generator function for command completion.  STATE lets us
   know whether to start from scratch; without any state
   (i.e. STATE == 0), then we start at the top of the list. */
char* CommandParser::command_generator (const char* text, int state)
{
  if (!parser)
    return 0;

  static unsigned list_index, len;

#ifdef _DEBUG
  cerr << "text='" << text << "' state=" << state << endl;
#endif

  /* If this is a new word to complete, initialize now.  This
     includes saving the length of TEXT for efficiency, and
     initializing the index variable to 0. */
  if (!state) {
    list_index = 0;
    len = strlen (text);
  }

  /* If no names matched, then return NULL. */
  char* name = 0;

  /* Return the next name which partially matches from the
     command list. */
  while (!name && list_index < parser->commands.size()) {

    if (!strncmp (parser->commands[list_index]->command.c_str(), text, len))
      name = strdup (parser->commands[list_index]->command.c_str());

    list_index++;

  }

#ifdef _DEBUG
  if (name)
    cerr << "name='" << name << "'" << endl;
  else
    cerr << "no name" << endl;
#endif

  return name;
}
