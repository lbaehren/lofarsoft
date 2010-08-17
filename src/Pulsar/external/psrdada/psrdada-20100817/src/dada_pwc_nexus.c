#include "dada_pwc_nexus.h"
#include "ascii_header.h"
#include "dada_def.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>


/* #define _DEBUG 1 */

/*! Initialize a new dada_node_t struct with default empty values */
void dada_pwc_node_init (dada_pwc_node_t* node)
{
  node_t* node_base = (node_t*) node;
  node_init (node_base);

  node -> header = 0;
  node -> header_size = 0;
  node -> state = dada_pwc_pending;
}

/*! Return pointer to a newly allocated and initialized dada_node_t struct */
node_t* dada_pwc_node_create ()
{
  dada_pwc_node_t* node = (dada_pwc_node_t*) malloc (sizeof(dada_pwc_node_t));
  assert (node != 0);
  dada_pwc_node_init (node);
  return (node_t*) node;
}

int dada_pwc_nexus_update_state (dada_pwc_nexus_t* nexus);

/*! Pointer to function that initializes a new connection with a node */
int dada_pwc_nexus_node_init (nexus_t* nexus, node_t* node)
{
  dada_pwc_nexus_t* dada_pwc_nexus = (dada_pwc_nexus_t*) nexus;
  dada_pwc_node_t* dada_pwc_node = (dada_pwc_node_t*) node;

  unsigned buffer_size = 1024;
  static char* buffer = 0;

  char* key = 0;

  if (!buffer)
    buffer = malloc (buffer_size);
  assert (buffer != 0);

#ifdef _DEBUG
  fprintf (stderr, "dada_pwc_node_init: receiving the welcome message\n");
#endif

  if (node_recv (node, buffer, buffer_size) < 0) {
    dada_pwc_node -> state = dada_pwc_undefined;
    return -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "dada_pwc_node_init: requesting state\n");
#endif

  if (node_send (node, "state") < 0) {
    dada_pwc_node -> state = dada_pwc_undefined;
    return -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "dada_pwc_node_init: receiving state\n");
#endif

  if (node_recv (node, buffer, buffer_size) < 0) {
    dada_pwc_node -> state = dada_pwc_undefined;
    return -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "dada_pwc_node_init: received '%s'\n", buffer);
#endif

  key = strtok (buffer, " \t\n\r");

  dada_pwc_node->state = dada_pwc_string_to_state (key);

  return dada_pwc_nexus_update_state (dada_pwc_nexus);
}

/*! Parse DADA PWC nexus configuration parameters from the config buffer */
int dada_pwc_nexus_parse (nexus_t* n, const char* config)
{

  /* the nexus is actually a DADA PWC nexus */
  dada_pwc_nexus_t* nexus = (dada_pwc_nexus_t*) n;

  /* the size of the header parsed from the config */
  unsigned hdr_size = 0;

  /* the status returned by sub-routines */
  int status = 0;

  /* get the heaer size */
  if (ascii_header_get (config, "HDR_SIZE", "%u", &hdr_size) < 0)
   multilog_fprintf (stderr, LOG_WARNING, "dada_pwc_nexus_parse: using default HDR_SIZE\n");
  else
    dada_pwc_set_header_size (nexus->pwc, hdr_size);

  // Get the PWCC control port from the config file
  char node_name [16];
  sprintf (node_name, "%sC_PORT", n->node_prefix);
  if (ascii_header_get (config, node_name, "%d", &(nexus->pwc->port)) < 0) {
   multilog_fprintf (stderr, LOG_WARNING, "nexus_parse: %s not specified.\n", node_name);
    nexus->pwc->port = DADA_DEFAULT_PWCC_PORT;
    if (nexus->pwc->port)
     multilog_fprintf (stderr, LOG_WARNING, "using default=%d\n", nexus->pwc->port);
    else {
     multilog_fprintf (stderr, LOG_ERR, "no default available\n");
      return -1;
    }
  }

  char logfile_dir [256];
  n->mirror->logfile_dir = NULL;

  /* If this is the mirror nexus, then setup the log file */
  if (ascii_header_get (config,"LOGFILE_DIR", "%s", &logfile_dir) < 0) {

    multilog_fprintf (stderr, LOG_WARNING, "nexus_parse: LOGFILE_DIR not specified, not logging\n");

  } else {

    // Get the PWC multilog port from the config file
    sprintf (node_name, "%s_LOGPORT", n->node_prefix);
    if (ascii_header_get (config, node_name, "%d", &(n->mirror->node_port)) < 0) {
      multilog_fprintf (stderr, LOG_WARNING, "nexus_parse: %s not specified.", node_name);
      n->mirror->node_port = DADA_DEFAULT_PWC_LOG;
      if (n->mirror->node_port)
       multilog_fprintf (stderr, LOG_WARNING, " using default=%d\n", n->mirror->node_port);
      else {
       multilog_fprintf (stderr, LOG_ERR, " no default available\n");
        return -1;
      }
    }

    // Get the PWCC multilog port from the config file
    sprintf (node_name, "%sC_LOGPORT", n->node_prefix);
    if (ascii_header_get (config, node_name, "%d", &(n->mirror->multilog_port)) < 0) {
      multilog_fprintf (stderr, LOG_WARNING, "nexus_parse: %s not specified.", node_name);
      n->mirror->multilog_port = DADA_DEFAULT_PWCC_LOGPORT;
      if (n->mirror->multilog_port)
       multilog_fprintf (stderr, LOG_WARNING, " using default=%d\n", n->mirror->multilog_port);
      else {
       multilog_fprintf (stderr, LOG_ERR, " no default available\n");
        return -1;
      }
    } 

    n->mirror->logfile_dir = malloc(strlen(logfile_dir));
    sprintf(n->mirror->logfile_dir,"%s",logfile_dir);

    struct stat st;
    stat(n->mirror->logfile_dir,&st);
                                                                                                                                                                              
    /* check the dir:  is dir     write      execute  permissions */
    if (!(S_ISDIR(st.st_mode))) {
     multilog_fprintf (stderr, LOG_WARNING, "nexus_parse: logfile directory %s did not exist\n",
                                            n->mirror->logfile_dir);
      n->mirror->logfile_dir = NULL;
      return -1;
    }
                                                                                                                                                                              
    if (!((st.st_mode & S_IWUSR) && (st.st_mode & S_IXUSR) &&
                                    (st.st_mode & S_IRUSR))) {
     multilog_fprintf (stderr, LOG_ERR, "nexus_parse: logfile directory %s was not writeable\n",
                      n->mirror->logfile_dir);
      n->logfile_dir = NULL;
      return -1;
    }
  }

  /* call the base class configuration parser */
  if (nexus_parse (n, config) < 0)
    return -1;

  return status;
}

int dada_pwc_nexus_update_state (dada_pwc_nexus_t* nexus)
{
  nexus_t* base = (nexus_t*) nexus;
  dada_pwc_node_t* node = 0;
  dada_pwc_state_t state = 0;

  unsigned inode = 0;
  unsigned nnode = 0;
  unsigned nsoft_err = 0;
  unsigned nhard_err = 0;
  unsigned nfatal_err = 0;

  /* fprintf (stderr, "dada_pwc_nexus_update_state: locking mutex\n"); */

  pthread_mutex_lock (&(base->mutex));

  /* fprintf (stderr, "dada_pwc_nexus_update_state: mutex locked\n"); */

  nnode = base->nnode;

  /* the nexus will always be in the same state as its PWC's. The only
   * exception to this is if nodes are in an error state. */
  
  for (inode = 0; inode < nnode; inode++)
  {
    node = base->nodes[inode];

    if (node->state == dada_pwc_soft_error)
      nsoft_err++;
    else if (node->state == dada_pwc_hard_error)
      nhard_err++;
    else if (node->state == dada_pwc_fatal_error)
      nfatal_err++;
    else if (inode == (nsoft_err+nhard_err+nfatal_err))
    {
      /* set state of the first non error node */
      state = node->state;
    }
    else if (state != node->state)
    {
      state = dada_pwc_undefined;
      break;
    }

    /*if (inode == 0)
      state = node->state;
    else if (state != node->state) {
      state = dada_pwc_undefined;
      break;
    }*/
  }

  /* If we don't have any clients that are non erroneous */
  if ((nsoft_err+nhard_err+nfatal_err) == nnode)
  {
    /* set state to the strongest error */
    if (nsoft_err) state = dada_pwc_soft_error;
    if (nhard_err) state = dada_pwc_hard_error;
    if (nfatal_err) state = dada_pwc_fatal_error;
  }

  pthread_mutex_unlock (&(base->mutex));

  nexus->pwc->state = state;

  return 0;
}

int dada_pwc_nexus_handle_message (void* me, unsigned inode, const char* msg)
{
  char state_string [16];
  dada_pwc_nexus_t* nexus = (dada_pwc_nexus_t*) me;
  dada_pwc_node_t* node = nexus->nexus.nodes[inode];

  char* state_change = strstr (msg, "STATE = ");

  if (state_change)
  {
    sscanf (state_change, "STATE = %s", state_string);
    node->state = dada_pwc_string_to_state (state_string);
    dada_pwc_nexus_update_state (nexus);
  }

  return 0;
}

/*! Send a unique header to each of the nodes */
int dada_pwc_nexus_cmd_config (void* context, FILE* fptr, char* args);

/*! Report on the state of each of the nodes */
int dada_pwc_nexus_cmd_state (void* context, FILE* fptr, char* args)
{
  dada_pwc_nexus_t* nexus = (dada_pwc_nexus_t*) context;
  nexus_t* base = (nexus_t*) context;
  dada_pwc_node_t* node = 0;

  unsigned inode = 0;
  unsigned nnode = 0;

  fprintf (fptr, "overall: %s\n", 
           dada_pwc_state_to_string( nexus->pwc->state ));

  pthread_mutex_lock (&(base->mutex));

  nnode = base->nnode;

  for (inode = 0; inode < nnode; inode++) {
    node = base->nodes[inode];
    fprintf (fptr, "PWC_%d: %s\n", inode,
             dada_pwc_state_to_string( node->state ));
  }

  pthread_mutex_unlock (&(base->mutex));

  return 0;
}

int dada_pwc_nexus_cmd_duration (void* context, FILE* fptr, char* args)
{
  unsigned buffer_size = 128;
  static char* buffer = 0;

  dada_pwc_nexus_t* nexus = (dada_pwc_nexus_t*) context;

  if (dada_pwc_cmd_duration (nexus->pwc, fptr, args) < 0)
    return -1;

  if (!buffer)
    buffer = malloc (buffer_size);
  assert (buffer != 0);

  snprintf (buffer, buffer_size, "duration %s", args);
  return nexus_send ((nexus_t*)nexus, buffer);
}

void dada_pwc_nexus_init (dada_pwc_nexus_t* nexus)
{
  nexus_t* nexus_base = (nexus_t*) nexus;
  nexus_init (nexus_base);

  if (nexus_base->node_prefix)
    free (nexus_base->node_prefix);
  nexus_base->node_prefix = strdup ("PWC");
  assert (nexus_base->node_prefix != 0);

  /* over-ride the nexus base class methods with dada_pwc_nexus methods */
  nexus_base->node_port = DADA_DEFAULT_PWC_PORT;
  nexus_base->node_create = &dada_pwc_node_create;
  nexus_base->node_init   = &dada_pwc_nexus_node_init;
  nexus_base->nexus_parse = &dada_pwc_nexus_parse;

  /* set up a mirror nexus for monitoring the PWC multilog messages */
  nexus_base->mirror = nexus_create ();
  nexus_base->mirror->node_port = DADA_DEFAULT_PWC_LOG;

  /* set up the monitor of the mirror nexus */
  nexus->monitor = monitor_create ();
  nexus->monitor->nexus = nexus_base->mirror;
  nexus->monitor->handle_message = &dada_pwc_nexus_handle_message;
  nexus->monitor->context = nexus;

#ifdef _DEBUG
  fprintf (stderr, "dada_pwc_nexus_init dada_pwc_create\n");
#endif

  nexus->pwc = dada_pwc_create ();

  /* do not convert times and sample counts into bytes */
  // nexus->pwc->convert_to_bytes = 0;

  /* convert time_t to local time strings */
  //nexus->convert_to_tm = localtime;
  nexus->convert_to_tm = gmtime;

  nexus->header_template = 0;

#ifdef _DEBUG
  fprintf (stderr, "dada_pwc_nexus_init command_parse_add\n");
#endif

  /* replace the header command with the config command */
  command_parse_remove (nexus->pwc->parser, "header");
  command_parse_add (nexus->pwc->parser, dada_pwc_nexus_cmd_config, nexus,
                     "config", "configure all nodes", NULL);

  /* replace the state command */
  command_parse_remove (nexus->pwc->parser, "state");
  command_parse_add (nexus->pwc->parser, dada_pwc_nexus_cmd_state, nexus,
                     "state", "get the current state of all nodes", NULL);

  /* replace the duration command */
  command_parse_remove (nexus->pwc->parser, "duration");
  command_parse_add (nexus->pwc->parser, dada_pwc_nexus_cmd_duration, nexus,
                     "duration", "set the duration of next recording", NULL);
}

/*! Create a new DADA nexus */
dada_pwc_nexus_t* dada_pwc_nexus_create ()
{
  dada_pwc_nexus_t* nexus = 0;
  nexus = (dada_pwc_nexus_t*) malloc (sizeof(dada_pwc_nexus_t));
  assert (nexus != 0);
  dada_pwc_nexus_init (nexus);
  return nexus;
}

/*! Destroy a DADA nexus */
int dada_pwc_nexus_destroy (dada_pwc_nexus_t* nexus)
{
  return nexus_destroy ((nexus_t*) nexus);
}

int dada_pwc_nexus_configure (dada_pwc_nexus_t* nexus, const char* filename)
{
  return nexus_configure ((nexus_t*) nexus, filename);
}

int dada_pwc_nexus_send (dada_pwc_nexus_t* nexus, dada_pwc_command_t command)
{
  unsigned buffer_size = 128;
  static char* buffer = 0;

  if (!buffer)
    buffer = malloc (buffer_size);
  assert (buffer != 0);

  switch (command.code) {

  case dada_pwc_clock:

    if (dada_pwc_set_state (nexus->pwc, dada_pwc_clocking, time(0)) < 0)
      return -1;

    return nexus_send ((nexus_t*)nexus, "clock");
    
  case dada_pwc_record_start:

    if (dada_pwc_set_state (nexus->pwc, dada_pwc_recording, time(0)) < 0)
      return -1;

    strftime (buffer, buffer_size, "rec_start " DADA_TIMESTR,
              nexus->convert_to_tm (&command.utc));
    return nexus_send ((nexus_t*)nexus, buffer);
    
  case dada_pwc_record_stop:

    if (dada_pwc_set_state (nexus->pwc, dada_pwc_clocking, time(0)) < 0)
      return -1;

    strftime (buffer, buffer_size, "rec_stop " DADA_TIMESTR,
              nexus->convert_to_tm (&command.utc));
    return nexus_send ((nexus_t*)nexus, buffer);
    
  case dada_pwc_start:
    
    if (dada_pwc_set_state (nexus->pwc, dada_pwc_recording, time(0)) < 0)
      return -1;

    if (!command.utc)
      return nexus_send ((nexus_t*)nexus, "start");

    strftime (buffer, buffer_size, "start " DADA_TIMESTR,
              nexus->convert_to_tm (&command.utc));
    return nexus_send ((nexus_t*)nexus, buffer);
    
  case dada_pwc_stop:

    if (dada_pwc_set_state (nexus->pwc, dada_pwc_idle, time(0)) < 0)
      return -1;

    if (!command.utc)
      return nexus_send ((nexus_t*)nexus, "stop");

    strftime (buffer, buffer_size, "stop " DADA_TIMESTR,
              nexus->convert_to_tm (&command.utc));
    return nexus_send ((nexus_t*)nexus, buffer);

  case dada_pwc_set_utc_start:
    /* Special case for PWC's who must be told their UTC_START */
    if (!command.utc)
      return -1;

    strftime (buffer, buffer_size, "set_utc_start " DADA_TIMESTR,
              nexus->convert_to_tm (&command.utc));
    return nexus_send ((nexus_t*)nexus, buffer);
          
  case dada_pwc_reset:

    return nexus_send((nexus_t*)nexus, "reset");

  }

  return -1;
}

int dada_pwc_nexus_serve (dada_pwc_nexus_t* nexus)
{
  /* the DADA PWC command */
  dada_pwc_command_t command = DADA_PWC_COMMAND_INIT;

  if (dada_pwc_serve (nexus->pwc) < 0) {
   multilog_fprintf (stderr, LOG_ERR, "dada_pwc_nexus_serve: could not start PWC server\n");
    return -1;
  }

  monitor_launch (nexus->monitor);

  while (!dada_pwc_quit (nexus->pwc))
  {
    command = dada_pwc_command_get (nexus->pwc);

    if (command.code == dada_pwc_exit)
      nexus->pwc->quit = 1;

    else if (dada_pwc_nexus_send (nexus, command) < 0)
     multilog_fprintf (stderr, LOG_ERR, "error issuing command = %d\n", command.code);
  }

  return 0;
}
