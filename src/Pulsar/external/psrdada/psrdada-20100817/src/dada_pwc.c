#include "dada_pwc.h"
#include "dada_def.h"

#include "ascii_header.h"
#include "tmutil.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* #define _DEBUG 1 */

/*! Parse a string and return the state */
dada_pwc_state_t dada_pwc_string_to_state (const char* key)
{
  if (!strcasecmp (key, "pending"))
    return dada_pwc_pending;
  if (!strcasecmp (key, "idle"))
    return dada_pwc_idle;
  if (!strcasecmp (key, "prepared"))
    return dada_pwc_prepared;
  if (!strcasecmp (key, "clocking"))
    return dada_pwc_clocking;
  if (!strcasecmp (key, "recording"))
    return dada_pwc_recording;
  if (!strcasecmp (key, "soft_error"))
    return dada_pwc_soft_error;
  if (!strcasecmp (key, "hard_error"))
    return dada_pwc_hard_error;
  if (!strcasecmp (key, "fatal_error"))
    return dada_pwc_fatal_error;

  return dada_pwc_undefined;
}

/*! Return the string corresponding to the state */
const char* dada_pwc_state_to_string (dada_pwc_state_t state)
{
  switch (state) {

  case dada_pwc_pending:
    return "pending";

  case dada_pwc_idle:
    return "idle";

  case dada_pwc_prepared:
    return "prepared";

  case dada_pwc_clocking:
    return "clocking";

  case dada_pwc_recording:
    return "recording";

  case dada_pwc_soft_error:
    return "soft_error";

  case dada_pwc_hard_error:
    return "hard_error";

  case dada_pwc_fatal_error:
    return "fatal_error";

  default:
    return "undefined";

  }
}

int dada_pwc_cmd_state (void* context, FILE* fptr, char* args)
{
  dada_pwc_t* primary = (dada_pwc_t*) context;
  fprintf (fptr, "%s\n", dada_pwc_state_to_string( primary->state ));
  return 0;
}

int dada_pwc_command_set_byte_count (dada_pwc_t* primary, FILE* output,
                                     dada_pwc_command_t* command)
{

  if (!primary->convert_to_bytes)
  {
#ifdef _DEBUG
    fprintf (stderr, "dada_pwc_command_set_byte_count not converting\n");
#endif
    command->byte_count = 0;
    return 0;
  }

  if (!command->utc)
  {
#ifdef _DEBUG
    fprintf (stderr, "dada_pwc_command_set_byte_count no UTC\n");
#endif
    command->byte_count = 0;
    return 0;
  }

  if (!primary->utc_start)
  {
    fprintf (output, "UTC of first time sample unknown\n");
    return -1;
  }

  if (!primary->bytes_per_second)
  {
    fprintf (output, "bytes per second not known\n");
    return -1;
  }

  if (command->utc < primary->utc_start)
  {
#ifdef _DEBUG
    fprintf (stderr, "requested UTC precedes UTC of first time sample\n");
#endif

    command->utc = 0;
    command->byte_count = 0;
    return 0;
  }

  /* the number of second between start and requested UTC of command */
  command->byte_count = command->utc - primary->utc_start;

  /* the byte count at which to execute the command */
  command->byte_count *= primary->bytes_per_second;

  /* round the byte_count to the nearest resolution boundary */
  if (primary->byte_resolution > 1)
  {
    /* round down to the start of the packet by default */
    uint64_t packets = command->byte_count / primary->byte_resolution;
    command->byte_count = packets * primary->byte_resolution;

    /* round to the end of the packet if stopping */
    if (command->code == dada_pwc_record_stop ||
	command->code == dada_pwc_stop)
      command->byte_count += primary->byte_resolution;
  }

  return 0;
}
  
/*! Set the command state */
int dada_pwc_command_set (dada_pwc_t* primary, FILE* output,
                          dada_pwc_command_t command)
{
  int ret = 0;

  if (!primary)
    return -1;
  
  pthread_mutex_lock (&(primary->mutex));

  while (primary->command.code != dada_pwc_no_command)
    pthread_cond_wait(&(primary->cond), &(primary->mutex));

  switch (command.code) {

  case dada_pwc_header:
    if (primary->state != dada_pwc_idle) {
      fprintf (output, "Cannot set header when not IDLE"
               " (state=%s)\n", dada_pwc_state_to_string (primary->state));
      ret = -1;
    }
    break;

  case dada_pwc_clock:
    if (primary->state != dada_pwc_prepared) {
      fprintf (output, "Cannot start clocking when not PREPARED"
               " (state=%s)\n", dada_pwc_state_to_string (primary->state));
      ret = -1;
    }
    break;

  case dada_pwc_record_start:
    if (primary->state != dada_pwc_clocking) {
      fprintf (output, "Cannot record start when not CLOCKING"
               " (state=%s)\n", dada_pwc_state_to_string (primary->state));
      ret = -1;
    }
    break;

  case dada_pwc_record_stop:
    if (primary->state != dada_pwc_recording) {
      fprintf (output, "Cannot record stop when not RECORDING"
               " (state=%s)\n", dada_pwc_state_to_string (primary->state));
      ret = -1;
    }
    break;

  case dada_pwc_start:
    if (primary->state != dada_pwc_prepared) {
      fprintf (output, "Cannot start when not PREPARED"
               " (state=%s)\n", dada_pwc_state_to_string (primary->state));
      ret = -1;
    }
    break;

  case dada_pwc_stop:
    // always allow stop ... WvS - 6 Oct 2008
    break;

  case dada_pwc_set_utc_start:
    if (primary->state != dada_pwc_clocking &&
        primary->state != dada_pwc_recording) {
      fprintf (output, "Cannot set_utc_start when not CLOCKING or RECORDING"
               " (state=%s)\n", dada_pwc_state_to_string (primary->state));
      ret = -1;
    }
    break;

  case dada_pwc_reset:
    if (primary->state != dada_pwc_soft_error &&
        primary->state != dada_pwc_hard_error) {
      fprintf (output, "Cannot reset when not SOFT ERROR or HARD ERROR"
               " (state=%s)\n", dada_pwc_state_to_string (primary->state));
      ret = -1;
    }
    break;

  case dada_pwc_exit:
    if (primary->state != dada_pwc_idle) {
      fprintf (output, "Cannot exit when not IDLE (state=%s)\n", 
               dada_pwc_state_to_string (primary->state));
      ret = -1;
    }
    break;
  }

  if (ret == 0) {

    primary->command = command;
    pthread_cond_signal (&(primary->cond));

  }

  pthread_mutex_unlock (&(primary->mutex));

  return ret;
}

int dada_pwc_parse_bytes_per_second (dada_pwc_t* primary,
                                     FILE* fptr, const char* header)
{
  unsigned npol;  /* number of polarizations */
  unsigned nbit;  /* nubmer of bits per sample */
  unsigned ndim;  /* number of dimensions */
  unsigned nchan;  /* number of dimensions */

  uint64_t bits_per_second = 0;
  int resolution = 1;

  double sampling_interval;

  if (ascii_header_get (header, "NPOL", "%d", &npol) < 0)
  {
    fprintf (fptr, "failed to parse NPOL - assuming 2\n");
    npol = 2;
  }

  if (ascii_header_get (header, "NBIT", "%d", &nbit) < 0)
  {
    fprintf (fptr, "failed to parse NBIT - assuming 8\n");
    nbit = 8;
  }

  if (ascii_header_get (header, "NDIM", "%d", &ndim) < 0)
  {
    fprintf (fptr, "failed to parse NDIM - assuming 1\n");
    ndim = 1;
  }

  if (ascii_header_get (header, "NCHAN", "%d", &nchan) < 0)
  {
    fprintf (fptr, "failed to parse NCHAN - assuming 1\n");
    nchan = 1;
  }

  if (ascii_header_get (header, "RESOLUTION", "%d", &resolution) < 0)
  {
    fprintf (fptr, "failed to parse RESOLUTION - assuming 1\n");
    resolution = 1;
  }

  primary->byte_resolution = resolution;

  primary->bits_per_sample = nbit * npol * ndim;

  if (ascii_header_get (header, "TSAMP", "%lf", &sampling_interval) < 0)
  {
    fprintf (fptr, "failed to parse TSAMP\n");
    primary->bytes_per_second = 0;
    return -1;
  }

  /* IMPORTANT: TSAMP is the sampling period in microseconds */
  bits_per_second = primary->bits_per_sample * nchan * ((uint64_t)(1e6/sampling_interval));

  primary->bytes_per_second = bits_per_second / 8;

  return 0;
}

int dada_pwc_cmd_header (void* context, FILE* fptr, char* args)
{
  dada_pwc_command_t command = DADA_PWC_COMMAND_INIT;
  dada_pwc_t* primary = (dada_pwc_t*) context;
  char* hdr = args;
  size_t length = 0;

  if (!args) {
    fprintf (fptr, "no header specified\n");
    return -1;
  }

  length = strlen (args);

  if (length > primary->header_size) {
    fprintf (fptr, "header too large (max %d bytes)\n", primary->header_size);
    return -1;
  }

  /* replace \ with new line */
  while ( (hdr = strchr(hdr, '\\')) != 0 )
    *hdr = '\n';

  if (dada_pwc_parse_bytes_per_second (primary, fptr, args) < 0)
    return -1;

  strcpy (primary->header, args);

  /* add a new line to the end */
  if (primary->header[length-1] != '\n' && length+1 < primary->header_size) {
    primary->header[length] = '\n';
    primary->header[length+1] = '\0';
  }

  /* set the number of bytes per second in the header */
  ascii_header_set (primary->header, "BYTES_PER_SECOND", "%"PRIu64,
                    primary->bytes_per_second);

  command.code = dada_pwc_header;
  command.header = primary->header;

  if (dada_pwc_command_set (primary, fptr, command) < 0)
    return -1;

  return 0;
}

uint64_t dada_pwc_parse_duration (dada_pwc_t* primary,
                                  FILE* fptr, const char* args)
{
  unsigned hh, mm, ss;
  uint64_t byte_count; 

  if (sscanf (args, "%u:%u:%u", &hh, &mm, &ss) == 3) {
   
    if (!primary->bytes_per_second) {
      fprintf (fptr, "bytes per second not known\n");
      return 0;
    }

    if (mm > 59) {
      fprintf (fptr, "invalid minutes = %u\n", mm);
      return 0;
    }

    if (ss > 59) {
      fprintf (fptr, "invalid seconds = %u\n", ss);
      return 0;
    }

    byte_count = hh;
    byte_count *= 3600;
    byte_count += 60 * mm;
    byte_count += ss;
    byte_count *= primary->bytes_per_second;

    return byte_count;

  }

  if (sscanf (args, "%"PRIu64"", &byte_count) == 1) {

    if (!primary->bits_per_sample) {
      fprintf (fptr, "bits per sample not known\n");
      return 0;
    }

    byte_count *= primary->bits_per_sample;
    byte_count /= 8;

    return byte_count;

  }

  fprintf (fptr, "could not parse duration from '%s'\n", args);

  return 0;
}

int dada_pwc_cmd_duration (void* context, FILE* fptr, char* args)
{
  dada_pwc_t* primary = (dada_pwc_t*) context;
  uint64_t byte_count = 0;

  int status = 0;

  if (!args) {
    fprintf (fptr, "please specify duration\n");
    return -1;
  }

  byte_count = dada_pwc_parse_duration (primary, fptr, args);

  if (!byte_count)
    return -1;

  pthread_mutex_lock (&(primary->mutex));

  while (primary->command.code != dada_pwc_no_command)
    pthread_cond_wait (&(primary->cond), &(primary->mutex));

  if (primary->state == dada_pwc_recording) {
    fprintf (stderr, "Cannot set DURATION while recording\n");
    status = -1;
  }
  else
    primary->command.byte_count = byte_count;

  pthread_mutex_unlock (&(primary->mutex));

  return status;
}


time_t dada_pwc_parse_time (FILE* fptr, char* args)  
{
  time_t utc = 0;

  if (!args)
    return utc;

  utc = str2utctime (args);
  if (utc == (time_t)-1) {
    fprintf (fptr, "Could not parse start time from '%s'\n", args);
    return -1;
  }

  return utc;
}

int dada_pwc_cmd_clock (void* context, FILE* fptr, char* args)
{
  dada_pwc_t* primary = (dada_pwc_t*) context;

  dada_pwc_command_t command = DADA_PWC_COMMAND_INIT;
  command.code = dada_pwc_clock;
  command.utc = 0;

  return dada_pwc_command_set (primary, fptr, command);
}

int dada_pwc_cmd_record_start (void* context, FILE* fptr, char* args)
{

  dada_pwc_t* primary = (dada_pwc_t*) context;

  dada_pwc_command_t command = DADA_PWC_COMMAND_INIT;
  command.code = dada_pwc_record_start;
  command.utc = dada_pwc_parse_time (fptr, args);

  if (!command.utc) {
    fprintf (fptr, "rec_start requires a valid utc argument\n");
    return -1;
  }

  if (dada_pwc_command_set_byte_count (primary, fptr, &command) < 0)
    return -1;

  return dada_pwc_command_set (primary, fptr, command);
}

int dada_pwc_cmd_record_stop (void* context, FILE* fptr, char* args)
{
  dada_pwc_t* primary = (dada_pwc_t*) context;

  dada_pwc_command_t command = DADA_PWC_COMMAND_INIT;
  command.code = dada_pwc_record_stop;
  command.utc = dada_pwc_parse_time (fptr, args);

  if (!command.utc) {
    fprintf (fptr, "rec_stop requires a valid utc argument\n");
    return -1;
  }

  if (dada_pwc_command_set_byte_count (primary, fptr, &command) < 0)
    return -1;

  return dada_pwc_command_set (primary, fptr, command);
}

int dada_pwc_cmd_start (void* context, FILE* fptr, char* args)
{
  dada_pwc_t* primary = (dada_pwc_t*) context;

  dada_pwc_command_t command = DADA_PWC_COMMAND_INIT;
  command.code = dada_pwc_start;
  command.utc = dada_pwc_parse_time (fptr, args);
  command.byte_count = primary->command.byte_count;

  return dada_pwc_command_set (primary, fptr, command);
}

int dada_pwc_cmd_stop (void* context, FILE* fptr, char* args)
{
  dada_pwc_t* primary = (dada_pwc_t*) context;

  dada_pwc_command_t command = DADA_PWC_COMMAND_INIT;
  command.code = dada_pwc_stop;
  command.utc = dada_pwc_parse_time (fptr, args);

  if (dada_pwc_command_set_byte_count (primary, fptr, &command) < 0)
    return -1;

  return dada_pwc_command_set (primary, fptr, command);
}


int dada_pwc_cmd_set_utc_start (void* context, FILE* fptr, char* args)
{
  dada_pwc_t* primary = (dada_pwc_t*) context;

  dada_pwc_command_t command = DADA_PWC_COMMAND_INIT;
  command.code = dada_pwc_set_utc_start;
  command.utc = dada_pwc_parse_time (fptr, args);

  /* Since the utc of the first time sample is now known, update the
   * primary's utc_start time to reflect this 
   * n.b. this will result in a byte_count of 0 for this command */
  primary->utc_start = command.utc;

  if (dada_pwc_command_set_byte_count (primary, fptr, &command) < 0)
    return -1;

  return dada_pwc_command_set (primary, fptr, command);
        
}

int dada_pwc_cmd_reset (void* context, FILE* fptr, char* args)
{
  dada_pwc_t* primary = (dada_pwc_t*) context;

  dada_pwc_command_t command = DADA_PWC_COMMAND_INIT;
  command.code = dada_pwc_reset;
  command.utc = 0;
                                                                                
  return dada_pwc_command_set (primary, fptr, command);

}

int dada_pwc_cmd_quit (void* context, FILE* fptr, char* args)
{
  dada_pwc_t* primary = (dada_pwc_t*) context;

  dada_pwc_command_t command = DADA_PWC_COMMAND_INIT;
  command.code = dada_pwc_exit;
  command.utc = 0;

  primary->quit = 1;

  return dada_pwc_command_set (primary, fptr, command);
                                                                                                                                                      
}


/*! Create a new DADA primary write client connection */
dada_pwc_t* dada_pwc_create ()
{
  dada_pwc_t* primary = (dada_pwc_t*) malloc (sizeof(dada_pwc_t));
  assert (primary != 0);

  primary -> state = dada_pwc_idle;
  primary -> command.code = dada_pwc_no_command;
  primary -> convert_to_bytes = 1;

  primary -> bytes_per_second = 0;
  primary -> bits_per_sample = 0;
  primary -> byte_resolution = 0;
  primary -> utc_start = 0;

  /* default header size */
  primary -> header_size = DADA_DEFAULT_HEADER_SIZE;
  primary -> header = (char *) malloc (primary->header_size);
  assert (primary->header != 0);

  /* default command port */
  primary -> port = DADA_DEFAULT_PWC_PORT;

#ifdef _DEBUG
  fprintf (stderr, "dada_pwc on port %d\n", primary->port);
#endif

  /* for multi-threaded use of primary */
  pthread_mutex_init(&(primary->mutex), NULL);
  pthread_cond_init (&(primary->cond), NULL);

  /* command parser */
  primary -> parser = command_parse_create ();

  command_parse_add (primary->parser, dada_pwc_cmd_state, primary,
                     "state", "get the current state", NULL);

  command_parse_add (primary->parser, dada_pwc_cmd_header, primary,
                     "header", "set the primary header", NULL);

  command_parse_add (primary->parser, dada_pwc_cmd_duration, primary,
                     "duration", "set the duration of next recording", NULL);

  command_parse_add (primary->parser, dada_pwc_cmd_start, primary,
                     "start", "enter the recording state", NULL);

  command_parse_add (primary->parser, dada_pwc_cmd_stop, primary,
                     "stop", "enter the idle state", NULL);

  command_parse_add (primary->parser, dada_pwc_cmd_clock, primary,
                     "clock", "enter the clocking state", NULL);

  command_parse_add (primary->parser, dada_pwc_cmd_record_start, primary,
                     "rec_start", "enter the recording state", NULL);

  command_parse_add (primary->parser, dada_pwc_cmd_record_stop, primary,
                     "rec_stop", "enter the clocking state", NULL);

  command_parse_add (primary->parser, dada_pwc_cmd_set_utc_start, primary,
                     "set_utc_start", "set the UTC_START header parameter", NULL);

  command_parse_add (primary->parser, dada_pwc_cmd_quit, primary,
                     "quit", "quit", NULL);

  command_parse_add (primary->parser, dada_pwc_cmd_reset, primary,
                     "reset", "reset error state to idle", NULL);

  primary -> server = 0;
  primary -> log = 0;
  primary -> quit = 0;

  return primary;
}

int dada_pwc_set_header_size (dada_pwc_t* primary, unsigned header_size)
{
  if (!primary)
    return -1;

  pthread_mutex_lock (&(primary->mutex));

  primary -> header_size = header_size;
  primary -> header = (char *) realloc (primary->header, header_size);
  assert (primary->header != 0);

  pthread_mutex_unlock (&(primary->mutex));

  return 0;
}


/*! */
int dada_pwc_serve (dada_pwc_t* primary)
{
  if (!primary)
    return -1;

  if (primary->server) {
    fprintf (stderr, "dada_pwc_serve: server already launched");
    return -1;
  }

  primary -> server = command_parse_server_create (primary -> parser);

  command_parse_server_set_welcome (primary -> server,
                                    "DADA primary write client command");

  /* open the command/control port */
  return command_parse_serve (primary->server, primary->port);
}

/*! Destroy a DADA primary write client connection */
int dada_pwc_destroy (dada_pwc_t* primary)
{
  return 0;
}

/*! Primary write client should exit when this is true */
int dada_pwc_quit (dada_pwc_t* primary)
{
  return primary->quit;
}

/*! Check to see if a command has arrived */
int dada_pwc_command_check (dada_pwc_t* primary)
{
  if (!primary)
    return -1;

  if (primary->command.code == dada_pwc_no_command)
    return 0;

  return 1;
}

/*! Get the next command from the connection; wait until command received */
dada_pwc_command_t dada_pwc_command_get (dada_pwc_t* primary)
{
  dada_pwc_command_t command = DADA_PWC_COMMAND_INIT;

  if (!primary) {
    command.code = -1;
    return command;
  }

  pthread_mutex_lock (&(primary->mutex));

  while (primary->command.code == dada_pwc_no_command)
    pthread_cond_wait(&(primary->cond), &(primary->mutex));

  command = primary->command;
  primary->command.code = dada_pwc_no_command;
  primary->command.byte_count = 0;

  pthread_mutex_unlock (&(primary->mutex));

  return command;
}

/*! \param new_state the state that the primary write client has entered
    \param utc the UTC time at which this state was entered */
int dada_pwc_set_state (dada_pwc_t* primary, int new_state, time_t utc)
{
  if (!primary)
    return -1;

  switch (primary->state) {

  case dada_pwc_idle:
    if (new_state != dada_pwc_prepared &&
        new_state != dada_pwc_soft_error &&
        new_state != dada_pwc_hard_error &&
        new_state != dada_pwc_fatal_error) 
    {
      fprintf (stderr, "IDLE can change only to PREPARED\n");
      return -1;
    }
    break;

  case dada_pwc_prepared:
    if (new_state != dada_pwc_idle &&
        new_state != dada_pwc_clocking &&
        new_state != dada_pwc_recording &&
        new_state != dada_pwc_soft_error &&
        new_state != dada_pwc_hard_error &&
        new_state != dada_pwc_fatal_error) 
    {
      fprintf (stderr, "PREPARED can change only to CLOCKING or RECORDING\n");
      return -1;
    }
    primary->utc_start = utc;
    break;

  case dada_pwc_clocking:
    if (new_state != dada_pwc_recording &&
        new_state != dada_pwc_idle &&
        new_state != dada_pwc_soft_error &&
        new_state != dada_pwc_hard_error &&
        new_state != dada_pwc_fatal_error) 
    {
      fprintf (stderr, "CLOCKING can change only to RECORDING or IDLE\n");
      return -1;
    }
    break;

  case dada_pwc_recording:
    if (new_state != dada_pwc_clocking &&
        new_state != dada_pwc_idle &&
        new_state != dada_pwc_soft_error &&
        new_state != dada_pwc_hard_error &&
        new_state != dada_pwc_fatal_error) 
    {
      fprintf (stderr, "RECORDING can change only to CLOCKING or IDLE\n");
      return -1;
    }
    break;

  case dada_pwc_soft_error:
    if (new_state != dada_pwc_idle) 
    {
      fprintf (stderr, "SOFT ERROR state can only change to IDLE\n");
      return -1;
    }
    break;

  case dada_pwc_hard_error:
    if (new_state != dada_pwc_idle) 
    {
      fprintf (stderr, "HARD ERROR state can only change to IDLE\n");
      return -1;
    }
    break;

  case dada_pwc_fatal_error:
    fprintf (stderr, "FATAL ERROR state cannot be changed\n");
    return -1;
    break;
    
  default:
    fprintf (stderr, "current state is UNDEFINED\n");
    return -1;

  }

  pthread_mutex_lock (&(primary->mutex));

  primary->state = new_state;

  if (primary->log)
    multilog (primary->log, LOG_INFO, 
              "STATE = %s\n", dada_pwc_state_to_string (new_state));
  
  pthread_cond_signal (&(primary->cond));
  pthread_mutex_unlock (&(primary->mutex));

  return 0;
}

const char * dada_pwc_cmd_code_string(int command_code)
{
  switch (command_code)
  {
    case dada_pwc_no_command:
      return "no_command";
                                                                                                                                                                                                                    
    case dada_pwc_header:
      return "header";
                                                                                                                                                                                                                    
    case dada_pwc_clock:
      return "clock";
                                                                                                                                                                                                                    
    case dada_pwc_record_start:
      return "record_start";
                                                                                                                                                                                                                    
    case dada_pwc_record_stop:
      return "record_stop";
                                                                                                                                                                                                                    
    case dada_pwc_start:
      return "start";
                                                                                                                                                                                                                    
    case dada_pwc_stop:
      return "stop";
                                                                                                                                                                                                                    
    case dada_pwc_set_utc_start:
      return "set_utc_start";
                                                                                                                                                                                                                    
    case dada_pwc_reset:
      return "reset";
                                                                                                                                                                                                                    
    case dada_pwc_exit:
      return "exit";
                                                                                                                                                                                                                    
    default:
      return "unknown";
  }
                                                                                                                                                                                                                    
}
