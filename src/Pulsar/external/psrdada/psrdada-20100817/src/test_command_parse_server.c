#include "command_parse_server.h"
#include "multilog.h"

#include <unistd.h>

typedef struct {

  int state;             /* a state code */
  float  float_value;    /* a floating point value */
  int    int_value;      /* an integer value */
  double double_value1;  /* a double-precision value */
  double double_value2;  /* a second double-precision value */

  /* means of communicating changes in state to multiple viewers */
  multilog_t* log;

} test_t;

/* values of the state variable */
#define TEST_NONE    0
#define TEST_FLOAT   1
#define TEST_DOUBLES 2
#define TEST_QUIT    3

/* this function implements the "echo" command; it ignores the context */
int echo (void* context, FILE* fptr, char* args)
{
  fprintf (fptr, "%s\n", args);
  return 0;
}

/* this function implements the "float" command */
int parse_float (void* context, FILE* fptr, char* args)
{
  test_t* test = (test_t*) context;
  float value;

  if (sscanf (args, "%f", &value) != 1) {
    fprintf (fptr, "Error could not parse float from %s\n", args);
    return -1;
  }

  multilog (test->log, LOG_INFO, "just parsed a float!\n");

  test->float_value = value;
  test->state = TEST_FLOAT;
  return 0;
}

/* this function implements the "int" command */
int parse_reset (void* context, FILE* fptr, char* args)
{
  test_t* test = (test_t*) context;

  multilog (test->log, LOG_INFO, "resetting!\n");

  test->state = TEST_NONE;
  return 0;
}

/* this function implements the "double" command */
int parse_double (void* context, FILE* fptr, char* args)
{
  test_t* test = (test_t*) context;
  double v1, v2;

  if (sscanf (args, "%lf %lf", &v1, &v2) != 2) {
    fprintf (fptr, "Error could not parse two doubles from %s\n", args);
    return -1;
  }

  multilog (test->log, LOG_INFO, "just parsed two doubles!\n");

  test->double_value1 = v1;
  test->double_value2 = v2;
  test->state = TEST_DOUBLES;
  return 0;
}
int main ()
{
  command_parse_server_t* server = 0;
  command_parse_t* parser = 0;
  test_t test;

  /* initialize the parser and test struct */

  fprintf (stderr, "Opening multilog\n");
  test.log = multilog_open ("parser", 0);
  test.state = TEST_NONE;

  fprintf (stderr, "Creating command_parse\n");
  parser = command_parse_create ();

  fprintf (stderr, "Adding echo to command_parse\n");
  command_parse_add (parser, echo, NULL,
		     "echo", "return arguments", NULL);

  fprintf (stderr, "Adding parse_float to command_parse\n");
  command_parse_add (parser, parse_float, &test, 
		     "float", "parse argument as float", NULL);

  fprintf (stderr, "Adding parse_double to command_parse\n");
  command_parse_add (parser, parse_double, &test, 
		     "double", "parse two arguments as double", NULL);

  fprintf (stderr, "Adding parse_reset to command_parse\n");
  command_parse_add (parser, parse_reset, &test,
                     "reset", "reset state", NULL);

  /* open the information port */
  multilog_serve (test.log, 2020);

  /* initialize the server */
  server = command_parse_server_create (parser);

  command_parse_server_set_welcome (server, "welcome to the test parser");

  /* open the command/control port */
  command_parse_serve (server, 2021);

  while (!test.state != TEST_QUIT) {

    switch (test.state) {
    case TEST_FLOAT:
      multilog (test.log, LOG_INFO, "float = %f\n", test.float_value);
      break;
    case TEST_DOUBLES:
      multilog (test.log, LOG_INFO, "doubles = %lf %lf\n",
		test.double_value1, test.double_value2);
      break;
    default:
      break;
    }

    sleep (1);

  }

  fprintf (stderr, "Closing log\n");
  multilog_close (test.log);

  fprintf (stderr, "Destroying parser\n");
  command_parse_destroy (parser);

  return 0;
}

