#ifndef __DADA_PWC_H
#define __DADA_PWC_H

/* ************************************************************************

   dada_pwc_t - a struct and associated routines for creation and
   management of a dada primary write client control connection

   ************************************************************************ */

#include "command_parse_server.h"
#include "multilog.h"

#include <inttypes.h> 
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

  /*! The states in which the primary write client may be found */
  typedef enum {

    /*! undefined: the primary write client is in an undefined state */
    dada_pwc_undefined,

    /*! pending: the state of the primary write client is TBD */
    dada_pwc_pending,

    /*! idle: no data is being recorded */
    dada_pwc_idle,

    /*! prepared: no data is being recorded, header received */
    dada_pwc_prepared,

    /*! clocking: data is being clocked in over-write mode */
    dada_pwc_clocking,

    /*! recording: data is being recorded */
    dada_pwc_recording,

    /*! soft error: a soft error occurred, but the pwc can be restarted */
    dada_pwc_soft_error,

    /*! hard error: the pwc cannot be restarted, but data block is intact */
    dada_pwc_hard_error,

    /*! fatal error: a fatal error has occurred that will require the 
     * dada clients to be killed and the data block reset */
    dada_pwc_fatal_error

  } dada_pwc_state_t;

  /*! Parse a string and return the state */
  dada_pwc_state_t dada_pwc_string_to_state (const char* key);

  /*! Return the string corresponding to the state */
  const char* dada_pwc_state_to_string (dada_pwc_state_t state);

  /*! The commands that may be issued to the primary write client */
  enum {

    /*! none: no command available */
    dada_pwc_no_command,

    /*! header: configuration parameters are available */
    dada_pwc_header,

    /*! clock: enter the clocking data state */
    dada_pwc_clock,

    /*! record start: enter the recording state (from clocking state) */
    dada_pwc_record_start,

    /*! record stop: enter the clocking state (from recording state) */
    dada_pwc_record_stop,

    /*! start: enter the recording state */
    dada_pwc_start,

    /*! stop: enter the idle state */
    dada_pwc_stop,

    /*! set utc: set UTC_START, and mark header block as filled */
    dada_pwc_set_utc_start,

    /*! reset the pwc, clearing error state */
    dada_pwc_reset,

    /*! quit the pwc */
    dada_pwc_exit
  };

  typedef struct {

    /*! The command code */
    int code;

    /*! The UTC associated with the command */
    time_t utc;

    /*! The duration (in bytes) associated with the command */
    uint64_t byte_count;

    /*! The ASCII header associated with the command */
    char* header;

  } dada_pwc_command_t;

#define DADA_PWC_COMMAND_INIT {0,0,0,0}

  typedef struct {

    /*! The name of the host on which primary write client is running */
    char* host;

    /*! The port on which primary write client control is listening */
    int port;

    /*! The primary write client identifier */
    int id;

    /* flag to quit */
    int quit;

    /*! The state of the primary write client */
    dada_pwc_state_t state;

    /*! When true, convert times and sample counts to bytes */
    int convert_to_bytes;

    /*! Used to convert times to bytes */
    uint64_t bytes_per_second;

    /*! Used to convert samples to bytes */
    unsigned bits_per_sample;

    /*! The byte resolution associated with the data */
    unsigned byte_resolution;

    /*! The UTC of first time sample in ring buffer */
    time_t utc_start;

    /*! The currently queued command */
    dada_pwc_command_t command;

    /*! The ASCII header sent/received via the connection */
    char* header;

    /*! The size of the ASCII header */
    unsigned header_size;

    /*! The command parse server */ 
    command_parse_server_t* server;

    /*! The command parser */
    command_parse_t* parser;

    /*! The message logging facility */
    multilog_t* log;

    /* for multi-threaded use of the struct */
    pthread_mutex_t mutex;

    /* for multi-threaded polling */
    pthread_cond_t cond;

  } dada_pwc_t;

  /*! Create a new DADA primary write client connection */
  dada_pwc_t* dada_pwc_create ();

  /*! Destroy a DADA primary write client connection */
  int dada_pwc_destroy (dada_pwc_t* primary);

  /*! Set the size of the buffer used to store the header information */
  int dada_pwc_set_header_size (dada_pwc_t* primary, unsigned header_size);

  /*! Start the command parsing server */
  int dada_pwc_serve (dada_pwc_t* primary);

  /*! Primary write client should exit when this is true */
  int dada_pwc_quit (dada_pwc_t* primary);

  /*! Check to see if a command has arrived */
  int dada_pwc_command_check (dada_pwc_t* primary);

  /*! Get the next command from the connection; wait until command received */
  dada_pwc_command_t dada_pwc_command_get (dada_pwc_t* primary);

  /*! Acknowledge the last command received */
  int dada_pwc_set_state (dada_pwc_t* primary, int state, time_t utc);

  /*! Callback for the duration command */
  int dada_pwc_cmd_duration (void* context, FILE* fptr, char* args);

  const char * dada_pwc_cmd_code_string(int command_code);

#ifdef __cplusplus
	   }
#endif

#endif
