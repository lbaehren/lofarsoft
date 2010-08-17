#include "command_parse.h"

int echo (void* context, FILE* fptr, char* args)
{
  fprintf (fptr, "%s\n", args);
  return 0;
}

int main ()
{
  command_parse_t* parser = 0;

  fprintf (stderr, "Creating command_parse\n");
  parser = command_parse_create ();

  fprintf (stderr, "Adding echo to command_parse\n");
  command_parse_add (parser, echo, NULL, "echo", "return arguments", NULL);

  fprintf (stderr, "Sending 'echo' command to command_parse\n");
  command_parse (parser, "echo this message should be echoed");

  fprintf (stderr, "Sending 'help' command to command_parse\n");
  command_parse (parser, "help");

  fprintf (stderr, "Destroying parser\n");
  command_parse_destroy (parser);

  return 0;
}
