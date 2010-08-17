#include "dada_pwc_main.h"
#include "dada_def.h"

#include "ascii_header.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

// #define _DEBUG 1

/*! Create a new DADA primary write client main loop */
dada_pwc_main_t* dada_pwc_main_create ()
{
  dada_pwc_main_t* pwcm = malloc (sizeof(dada_pwc_main_t));
  assert (pwcm != 0);

  pwcm -> pwc = 0;
  pwcm -> log = 0;

  pwcm -> data_block = 0;
  pwcm -> header_block = 0;

  pwcm -> start_function = 0;
  pwcm -> buffer_function = 0;
  pwcm -> stop_function = 0;
  pwcm -> error_function = 0;
  pwcm -> header_valid_function = 0;

  pwcm -> context = 0;
  pwcm -> header = 0;
  pwcm -> header_size = 0;
  pwcm -> verbose = 1;
  pwcm -> header_valid = 0;

  return pwcm;
}

/*! Destroy a DADA primary write client main loop */
void dada_pwc_main_destroy (dada_pwc_main_t* pwcm)
{
  free (pwcm);
}

/*! prepare for data transfer */
int dada_pwc_main_prepare (dada_pwc_main_t* pwcm);

/*! start the data transfer */
int dada_pwc_main_start_transfer (dada_pwc_main_t* pwcm);

/*! do the data transfer */
int dada_pwc_main_transfer_data (dada_pwc_main_t* pwcm);

/*! stop the data transfer */
int dada_pwc_main_stop_transfer (dada_pwc_main_t* pwcm);

/*! Run the DADA primary write client main loop */
int dada_pwc_main (dada_pwc_main_t* pwcm)
{
 
  if (!pwcm) {
    fprintf (stderr, "dada_pwc_main no main!\n");
    return -1;
  }

  if (!pwcm->pwc) {
    fprintf (stderr, "dada_pwc_main no PWC command connection\n");
    return -1;
  }

  if (!pwcm->log) {
    fprintf (stderr, "dada_pwc_main no logging facility\n");
    return -1;
  }

  if (!pwcm->start_function) {
    fprintf (stderr, "dada_pwc_main no start function\n");
    return -1;
  }

  if (!pwcm->buffer_function) {
    fprintf (stderr, "dada_pwc_main no buffer function\n");
    return -1;
  }

  if (!pwcm->stop_function) {
    fprintf (stderr, "dada_pwc_main no stop function\n");
    return -1;
  }

  if (!pwcm->pwc->log)
    pwcm->pwc->log = pwcm->log;


  /* the return value can be
   *  0:  no error
   * -1:  soft error   pwc_command will try to restart
   * -2:  hard error   fatal, but data block intact, dada clients ok
   * -3:  fatal error  fatal, data block damaged, dada clients hung
   */
  int rval = 0;

  while (!dada_pwc_quit (pwcm->pwc))
  {
    /* Enter the idle/prepared state. */
    rval = dada_pwc_main_prepare (pwcm);

    if (dada_pwc_quit (pwcm->pwc))
      break;

    if (rval < 0)
      dada_pwc_main_process_error (pwcm, rval);
    else
    {
      /* Start the data transfer. */
      rval = dada_pwc_main_start_transfer (pwcm);
      if (rval < 0) 
        dada_pwc_main_process_error (pwcm, rval);
      else {

        /* Enter the clocking/recording state. */
        rval = dada_pwc_main_transfer_data (pwcm);
        if (rval < 0) 
          dada_pwc_main_process_error (pwcm, rval); 

      }

      /* Stop the data transfer. */
      rval = dada_pwc_main_stop_transfer (pwcm);
      if (rval < 0) 
        dada_pwc_main_process_error (pwcm, rval);

    } 

    /* If we ever reach a fatal state, try to exit */
    if (pwcm->pwc->state == dada_pwc_fatal_error) 
      pwcm->pwc->quit = 1;

  }

  return rval;

}

/*! The idle and prepared states of the DADA primary write client main loop */
int dada_pwc_main_prepare (dada_pwc_main_t* pwcm)
{
  /* get next available Header Block */
  if (pwcm->header_block) {
    pwcm->header_size = ipcbuf_get_bufsz (pwcm->header_block);
    pwcm->header = ipcbuf_get_next_write (pwcm->header_block);
    if (!pwcm->header) {
      multilog (pwcm->log, LOG_ERR, "Could not get next header block\n");
      return DADA_ERROR_HARD;
    }
  }

  /* ensure that Data Block is closed */
  if (pwcm->data_block && ipcio_is_open (pwcm->data_block)
      && ipcio_close (pwcm->data_block) < 0)
  {
    multilog (pwcm->log, LOG_ERR, "Could not close Data Block\n");
    return DADA_ERROR_HARD;
  }

  while (!dada_pwc_quit (pwcm->pwc))
  {
    pwcm->command = dada_pwc_command_get (pwcm->pwc);

    if (dada_pwc_quit (pwcm->pwc))
      break;

    if (pwcm->command.code == dada_pwc_reset)
    {
      dada_pwc_set_state (pwcm->pwc, dada_pwc_idle, 0);
    }
    
    else if (pwcm->command.code == dada_pwc_header)
    {
#ifdef _DEBUG 
      multilog (pwcm->log, LOG_INFO, 
                "HEADER START\n%s\nHEADER END\n", pwcm->command.header);
#endif

      if (pwcm->header_block)
        strncpy (pwcm->header, pwcm->command.header, pwcm->header_size);

      dada_pwc_set_state (pwcm->pwc, dada_pwc_prepared, 0);
    }

    else if (pwcm->command.code == dada_pwc_clock)
    {
      /* multilog (pwcm->log, LOG_INFO, "Start clocking data\n"); */

      if (pwcm->command.byte_count) {
        multilog (pwcm->log, LOG_ERR, "dada_pwc_main_idle internal error.  "
                 "byte count specified in CLOCK command\n");
        return DADA_ERROR_SOFT;
      }

      /* Open the Data Block in clocking mode */
      if (pwcm->data_block && ipcio_open (pwcm->data_block, 'w') < 0)  {
        multilog (pwcm->log, LOG_ERR, "Could not open data block\n");
        return DADA_ERROR_HARD;
      }

      /* leave the idle state loop */
      return 0;
        
    }
    else if (pwcm->command.code == dada_pwc_start)
    {
#ifdef _DEBUG            
      multilog (pwcm->log, LOG_INFO, "Start recording data\n");
#endif

      if (pwcm->command.byte_count)
        multilog (pwcm->log, LOG_INFO,
                  "Will record %"PRIu64" bytes\n", pwcm->command.byte_count);

      /* Open the Data Block in recording mode */
      if (pwcm->data_block && ipcio_open (pwcm->data_block, 'W') < 0)  {
        multilog (pwcm->log, LOG_ERR, "Could not open data block\n");
        return DADA_ERROR_HARD;
      }
      
      /* leave the idle state loop */
      return 0;
    }
    else if (pwcm->command.code == dada_pwc_stop) 
    {
      if (pwcm->pwc->state == dada_pwc_soft_error) 
        multilog (pwcm->log, LOG_WARNING, "Resetting soft_error to idle\n");
      else
        multilog (pwcm->log, LOG_WARNING, "dada_pwc_main_prepare: Unexpected stop command\n");

      dada_pwc_set_state (pwcm->pwc, dada_pwc_idle, 0);
    }
    else
    {
      multilog (pwcm->log, LOG_ERR, "dada_pwc_main_prepare internal error = "
               "unexpected command code %s\n", 
               dada_pwc_cmd_code_string(pwcm->command.code));
      return DADA_ERROR_HARD;
    }

  }

  return 0;
}

/*! The transit to clocking/recording state */
int dada_pwc_main_start_transfer (dada_pwc_main_t* pwcm)
{
  /* If utc != 0, the start function should attempt to start the data
     transfer at the specified utc.  Otherwise, start as soon as
     possible.  Regardless, the start function should return the UTC
     of the first time sample to be transfered to the Data Block.
  */

#ifdef _DEBUG
  fprintf (stderr, "dada_pwc_main_start_transfer: call start function\n");
#endif

  time_t utc = pwcm->start_function (pwcm, pwcm->command.utc);

  unsigned buffer_size = 64;
  static char* buffer = 0;

  if (!buffer)
    buffer = malloc (buffer_size);
  assert (buffer != 0);

  if (utc < 0) {
    multilog (pwcm->log, LOG_ERR, "start_function returned invalid UTC\n");
    return DADA_ERROR_HARD;
  }

  /* If the start function, cannot provide the utc start, 0 
   * indicates that the UTC will be provided by the command
   * interface */
  if (utc == 0) 
    buffer = "UNKNOWN";
  else 
    strftime (buffer, buffer_size, DADA_TIMESTR, gmtime (&utc));

  multilog (pwcm->log, LOG_INFO, "UTC_START = %s\n", buffer);

  /* make header available on Header Block */
  if (pwcm->header_block) {

    /* write UTC_START to the header */
    if (ascii_header_set (pwcm->header, "UTC_START", "%s", buffer) < 0) {
      multilog (pwcm->log, LOG_ERR, "failed ascii_header_set UTC_START\n");
      return DADA_ERROR_SOFT;
    }

    /* Set the primary UTC_START in the pwc if we have it */
    if (utc > 0) {

      /* Set the base utc from which all commands are calculated */
      pwcm->pwc->utc_start = utc;
      multilog(pwcm->log, LOG_INFO, "Setting pwcm->pwc->utc_start = %d\n",pwcm->pwc->utc_start);

    }

    /* We can only mark the header filled if we have a start command */
    if (pwcm->command.code == dada_pwc_start) {

      /* only mark header block filled if header is valid */
      if (pwcm->header_valid_function)
        pwcm->header_valid = pwcm->header_valid_function(pwcm);
      else 
        pwcm->header_valid = 1;

      if (pwcm->header_valid) {
#ifdef _DEBUG
          multilog(pwcm->log, LOG_INFO, "dada_pwc_main_start_transfer: Marking header filled\n");
#endif
        if ( ipcbuf_mark_filled (pwcm->header_block, pwcm->header_size) < 0)  {
          multilog (pwcm->log, LOG_ERR, "Could not marked header filled or command.code != start\n");
          return DADA_ERROR_HARD;
        }
      }
    }
  } 

#ifdef _DEBUG
  fprintf (stderr, "dada_pwc_main_start_transfer: exit change state\n");
#endif

  ipcbuf_set_soclock_buf ((ipcbuf_t*) pwcm->data_block);

  if (pwcm->command.code == dada_pwc_clock)
    return dada_pwc_set_state (pwcm->pwc, dada_pwc_clocking, utc);

  else if (pwcm->command.code == dada_pwc_start) 
    return dada_pwc_set_state (pwcm->pwc, dada_pwc_recording, utc);
 

  multilog (pwcm->log, LOG_ERR, "dada_pwc_main_start_transfer"
            " internal error = invalid state\n");
  return DADA_ERROR_HARD;
}

/*! Switch from clocking to recording states */
int dada_pwc_main_record_start (dada_pwc_main_t* pwcm)
{

  /* the minimum offset at which recording may start */
  uint64_t minimum_record_start = 0;

  /* the byte to which the UTC_START corresponds */
  uint64_t utc_start_byte = 0;

  /* the byte to which command will correspond */
  uint64_t command_start_byte = 0;

  /* the next header to be written */
  char* header = 0;

  /* Find the earliest byte recording can start on. NB this may
   * have wrapped around if we have been clocking for longer than
   * the data block length... */
  minimum_record_start = ipcio_get_start_minimum (pwcm->data_block);

  /* If the first data received did not occur on buf 0, then
   * we need to know this to correctly calculate the command
   * offset */
  utc_start_byte = ipcio_get_soclock_byte(pwcm->data_block);

  // multilog(pwcm->log, LOG_INFO, "utc_start_byte = %"PRIu64"\n",utc_start_byte);

  /* The actual byte this command corresponds to */
  command_start_byte = utc_start_byte + pwcm->command.byte_count;

  // multilog (pwcm->log, LOG_INFO, "minimum_record_start=%"PRIu64"\n",
  //           minimum_record_start);

  /* If the command is scheduled to occur earlier than is possible, then
   * we must delay that command to the earliest time */
  if (command_start_byte < minimum_record_start) {
    multilog (pwcm->log, LOG_ERR, "Requested start byte=%"PRIu64
              " reset to minimum=%"PRIu64"\n", command_start_byte, 
              minimum_record_start);

    /* This is the byte the command corresponds to */
    command_start_byte = minimum_record_start;

    /* This is the offset of the command from the utc_start (OBS_OFFSET) */
    pwcm->command.byte_count = minimum_record_start - utc_start_byte;
  }

  multilog (pwcm->log, LOG_INFO, "REC_START\n");
  multilog (pwcm->log, LOG_INFO, "pwcm->command.utc = %d\n",pwcm->command.utc);
  multilog (pwcm->log, LOG_INFO, "pwcm->pwc->utc_start = %d\n",pwcm->pwc->utc_start);

  /* Special case for rec_stop/rec_start toggling */
  header = ipcbuf_get_next_write (pwcm->header_block);
  if (header != pwcm->header) {
    /* the case if there is more than one sub-block in Header Block */
    memcpy (header, pwcm->header, pwcm->header_size);
    pwcm->header = header;
  }

  /* a UTC_START may be different for multiple PWC's, but the REC_START will 
   * be uniform, so on a REC_START we will change the UTC_START in the outgoing 
   * header to be UTC_START + OBS_OFFSET, to ensure uniformity across PWCs */

  time_t utc = pwcm->command.utc;
  int buffer_size = 64;
  char buffer[buffer_size];
  strftime (buffer, buffer_size, DADA_TIMESTR, gmtime (&utc));

  multilog (pwcm->log, LOG_INFO, "dada_pwc_main_record_start: UTC_START reset to REC_START = %s\n", buffer);

  if (ascii_header_set (pwcm->header, "UTC_START", "%s", buffer) < 0) {
    multilog (pwcm->log, LOG_ERR, "fail ascii_header_set UTC_START\n");
    return DADA_ERROR_HARD;
  }

  multilog (pwcm->log, LOG_INFO, "dada_pwc_main_record_start: OBS_OFFSET = 0\n");
  if (ascii_header_set (pwcm->header, "OBS_OFFSET", "%"PRIu64, 0) < 0) {
    multilog (pwcm->log, LOG_ERR, "fail ascii_header_set OBS_OFFSET\n");
    return DADA_ERROR_HARD;
  }

  multilog (pwcm->log, LOG_INFO,"command_start_byte = %"PRIu64", command.byte_"
            "count = %"PRIu64"\n",command_start_byte,pwcm->command.byte_count);

  /* start valid data on the Data Block at the requested byte */
  if (ipcio_start (pwcm->data_block, command_start_byte) < 0)  {
    multilog (pwcm->log, LOG_ERR, "Could not start data block"
              " at %"PRIu64"\n", command_start_byte);
    return DADA_ERROR_HARD;
  }

  /* If the header has not yet been made valid */
  if (!(pwcm->header_valid)) {

    if (pwcm->header_valid_function) 
      pwcm->header_valid = pwcm->header_valid_function(pwcm);
    else
      pwcm->header_valid = 1;

    if (pwcm->header_valid) {
#ifdef _DEBUG
        multilog(pwcm->log, LOG_INFO, "dada_pwc_main_record_start: Marking header filled\n");
#endif
      if (ipcbuf_mark_filled (pwcm->header_block, pwcm->header_size) < 0)  {
        multilog (pwcm->log, LOG_ERR, "Could not mark filled header\n");
        return DADA_ERROR_HARD;
      }
    } else {
      multilog (pwcm->log, LOG_ERR, "Cannot transit from clocking to recoding "
                "if when header is invalid\n");;
      return DADA_ERROR_HARD;
    }
  }

  return 0;

}

/*! The clocking and recording states of the DADA PWC main loop */
int dada_pwc_main_transfer_data (dada_pwc_main_t* pwcm)
{
  /* total number of bytes written to the Data Block */
  uint64_t total_bytes_written = 0;

  /* the byte at which the state will change */
  uint64_t transit_byte = pwcm->command.byte_count;

  /* the number of bytes to write to the Data Block */
  uint64_t bytes_to_write = pwcm->command.byte_count;

  /* the number of bytes to be copied from buffer */
  uint64_t buf_bytes = 0;

  /* the number of bytes written to the Data Block */
  int64_t bytes_written = 0;

  /* pointer to the data buffer */
  char* buffer = 0;

  /* size of the data buffer */
  uint64_t buffer_size = 0;

  /* error state due to a 0 byte buffer function return */
  uint64_t data_transfer_error_state = 0;

  char* command_string = 0;

  /* flags and buffers for setting of UTC_START */
  uint64_t utc_start_set = 0;
  int utc_size = 1024;
  char utc_buffer[utc_size];

  /* flags for whether the header block has been cleared */

  /* Check if the UTC_START is valid */
  if (ascii_header_get (pwcm->header, "UTC_START", "%s", utc_buffer) < 0) {
    multilog (pwcm->log, LOG_ERR, "Could not read UTC_START from header\n");
    return DADA_ERROR_HARD;
  }

  if (strcmp(utc_buffer,"UNKNOWN") != 0) 
    utc_start_set = 1;

#ifdef _DEBUG
  fprintf (stderr, "dada_pwc_main_transfer_data: enter main loop\n");
#endif

  while ((!dada_pwc_quit (pwcm->pwc)) && (data_transfer_error_state <= 1)) {

#ifdef _DEBUG
    fprintf (stderr, "dada_pwc_main_transfer_data: check for command\n");
#endif

    /* check to see if a new command has been registered */
    if (dada_pwc_command_check (pwcm->pwc))  {

#ifdef _DEBUG
      fprintf (stderr, "dada_pwc_main_transfer_data: get command\n");
#endif

      pwcm->command = dada_pwc_command_get (pwcm->pwc);

      // multilog (pwcm->log, LOG_INFO, "pwcm->command.utc = %d\n",pwcm->command.utc);
      // multilog (pwcm->log, LOG_INFO, "pwcm->pwc->utc_start = %d\n",pwcm->pwc->utc_start);

      /* special case for set_utc_start */
      if (pwcm->command.code == dada_pwc_set_utc_start) {

        assert(pwcm->command.utc > 0);

        if (utc_start_set) { 
          multilog (pwcm->log, LOG_WARNING, "WARNING, UTC_START was already "
                    "set. Ignoring set_utc_start command\n");
        } else {

          strftime (utc_buffer, utc_size, DADA_TIMESTR, 
                    (struct tm*) gmtime(&(pwcm->command.utc)));

#ifdef _DEBUG
          fprintf (stderr,"dada_pwc_main_transfer_data: set UTC_START in "
                          "header to : %s\n",utc_buffer);
#endif
          multilog (pwcm->log, LOG_INFO,"UTC_START = %s\n",utc_buffer);

          // write UTC_START to the header
          if (ascii_header_set (pwcm->header,"UTC_START","%s",utc_buffer) < 0) {
            multilog (pwcm->log,LOG_ERR,"failed ascii_header_set UTC_START\n");
            return DADA_ERROR_HARD;
          }

          /* We only marked the header block as filled if we are already 
           * recording this can only happen via a "start" command. if we
           * are clocking, then the record_start command will mark the 
           * header block as filled */

#ifdef _DEBUG
          fprintf (stderr,"dada_pwc_main_transfer_data: header block filled\n");
#endif
          if (pwcm->pwc->state == dada_pwc_recording) {

            if (pwcm->header_valid_function) 
              pwcm->header_valid = pwcm->header_valid_function(pwcm);
            else
              pwcm->header_valid = 1;

            if (pwcm->header_valid) {
#ifdef _DEBUG
              multilog (pwcm->log, LOG_INFO, "dada_pwc_main_transfer_data: marking header valid\n");
#endif

              if (ipcbuf_mark_filled (pwcm->header_block,pwcm->header_size) < 0) {
                multilog (pwcm->log, LOG_ERR, "Could not mark filled header\n");
                return DADA_ERROR_HARD;
              }
            }
          }
      
          /* Set to true */
          utc_start_set = 1;
        }

        /* Signal the command control interface */
        pthread_mutex_lock (&(pwcm->pwc->mutex));
        pthread_cond_signal (&(pwcm->pwc->cond));
        pthread_mutex_unlock (&(pwcm->pwc->mutex));

      } else {

        if (pwcm->command.code == dada_pwc_record_stop)
          command_string = "recording->clocking";
        else if (pwcm->command.code == dada_pwc_record_start)
          command_string = "clocking->recording";
        else if (pwcm->command.code == dada_pwc_stop)
          command_string = "stopping";
        else {
          multilog (pwcm->log, LOG_ERR,
                    "dada_pwc_main_transfer data internal error = "
                    "unexpected command code %d\n", pwcm->command.code);
          return DADA_ERROR_HARD;
        }

        if (pwcm->command.byte_count > total_bytes_written) {
          /* command is dated, happens in the future... */
          transit_byte = pwcm->command.byte_count;
          bytes_to_write = pwcm->command.byte_count - total_bytes_written;

          multilog (pwcm->log, LOG_INFO, "%s in %"PRIu64" bytes, %"PRIu64
                    " bytes written\n", command_string, bytes_to_write, 
                    total_bytes_written);
        }

        else {
          /* command is immediate */
          multilog (pwcm->log, LOG_INFO, "%s immediately\n", command_string);
          transit_byte = total_bytes_written;
          bytes_to_write = 0;

          if (pwcm->command.byte_count &&
              pwcm->command.byte_count < total_bytes_written)
            multilog (pwcm->log, LOG_NOTICE,
                      "requested transit byte=%"PRIu64" passed\n",
                      pwcm->command.byte_count);
        }
        
      }

      if (pwcm->verbose) {
        if (pwcm->pwc->state == dada_pwc_recording)
          multilog (pwcm->log, LOG_INFO, "recording\n");
        else if (pwcm->pwc->state == dada_pwc_clocking)
          multilog (pwcm->log, LOG_INFO, "clocking\n");

        if (transit_byte) {

          if (pwcm->command.code ==  dada_pwc_record_stop)
            multilog (pwcm->log, LOG_INFO, "record stop in %"PRIu64" bytes\n",
                      bytes_to_write);
          else if (pwcm->command.code ==  dada_pwc_record_start)
            multilog (pwcm->log, LOG_INFO, "record start in %"PRIu64" bytes\n",
                      bytes_to_write);
          else if (pwcm->command.code ==  dada_pwc_record_stop)
            multilog (pwcm->log, LOG_INFO, "stop in %"PRIu64" bytes\n",
                      bytes_to_write);
          else
            ;

        }
      }
    }

    if (!transit_byte || bytes_to_write) {

#ifdef _DEBUG
  fprintf (stderr, "dada_pwc_main_transfer_data: call buffer function\n");
#endif

      /* get the next data buffer */
      buffer = pwcm->buffer_function (pwcm, &buffer_size);

      /* If the buffer_function had an error */
      if (buffer_size < 0) 
        return buffer_size;

      if ((data_transfer_error_state) && (buffer_size != 0)) {
        data_transfer_error_state = 0;
        if (total_bytes_written) 
          multilog (pwcm->log, LOG_WARNING, "pwc buffer_function "
                    "recovered from error state\n");
      }

      /* If we are experiencing an read error that we may be able to 
       * recover from, run the error_function, if it returns:
       * 0, we have recovered from error 
       * 1, we keep trying to recover
       * 2, we will not be able to recover from error, exit gracefully */

      if (buffer_size == 0) {

        if (pwcm->error_function) {
          /* If defined */
          data_transfer_error_state = pwcm->error_function(pwcm);
        } else {
          /* give up */        
          data_transfer_error_state = 2;
        }

        if (data_transfer_error_state == 2) {
          multilog (pwcm->log, LOG_ERR, "pwc buffer_function returned 0 bytes."
                                        " Stopping\n");

          /* Ensure that the ipcio_write function is called with 1 bytes
           * so that a reader may acknwowledge the SoD and subsequent EoD */
          bytes_written = ipcio_write (pwcm->data_block, buffer, 1);

          /* If we are clocking, then its not the end of the world, signal
           * a soft error */
          //if (pwcm->pwc->state == dada_pwc_clocking)
            return DADA_ERROR_SOFT;
          //else
          //  return DADA_ERROR_HARD;
        }

        if ((data_transfer_error_state == 1) && total_bytes_written) {
          multilog (pwcm->log, LOG_WARNING, "Warning: pwc buffer function "
                    "returned 0 bytes. Trying to continue\n");
          multilog (pwcm->log, LOG_WARNING, "total_bytes_written = %"PRIu64
                    ", bytes_to_write = %"PRIu64", transit_byte = %"PRIu64"\n",
                    total_bytes_written, bytes_to_write, transit_byte);
        }

      }
      
      if (!buffer) {
        multilog (pwcm->log, LOG_ERR, "buffer function error\n");
        return DADA_ERROR_HARD;   
        //TODO CHECK WHAT buffer_function actually returns...
      }

      /* Check if we need to mark the header as valid after this - not possible
       * if we are clocking */
      if ((!pwcm->header_valid) && (pwcm->pwc->state == dada_pwc_recording)) {
        pwcm->header_valid = pwcm->header_valid_function(pwcm);
      
        /* If the header is NOW valid, flag the header as filled */  
        if (pwcm->header_valid) {
#ifdef _DEBUG
            multilog (pwcm->log, LOG_INFO,"dada_pwc_main_transfer_data: marking header filled\n");
#endif
          if (ipcbuf_mark_filled (pwcm->header_block,pwcm->header_size) < 0) {
            multilog (pwcm->log, LOG_ERR, "Could not mark filled header\n");
            return DADA_ERROR_HARD;
          }
        }
      }



      /* If the transit_byte is set, do not write more than the requested
         amount of data to the Data Block */
      buf_bytes = buffer_size;
      
      if (transit_byte && buf_bytes > bytes_to_write)
        buf_bytes = bytes_to_write;
     
      /* 
      multilog (pwcm->log, LOG_INFO, "prev. buf_bytes=%"PRIu64" " 
                " curr. buf_bytes=%"PRIu64" bytes_to_write=%"PRIu64" \n",
                buffer_size,buf_bytes,bytes_to_write);
      */

      /* write the bytes to the Data Block */
      if (pwcm->data_block) {

#ifdef _DEBUG
        fprintf (stderr, "dada_pwc_main_transfer_data: write to data block"
                         " buffer=%p bytes=%"PRIu64"\n", buffer, buf_bytes);
#endif

        /* If we run out of empty buffers and don't yet have a UTC_START, 
         * fail */
        if ((!utc_start_set) && (ipcio_space_left (pwcm->data_block) < buf_bytes)) {
          multilog (pwcm->log, LOG_ERR, "Data block full and UTC_START not "
                    "set.\n");
          return DADA_ERROR_FATAL;
        }
        
        bytes_written = ipcio_write (pwcm->data_block, buffer, buf_bytes);

#ifdef _DEBUG
        fprintf (stderr, "dada_pwc_main_transfer_data: return from write\n");
#endif

        if (bytes_written < 0 || bytes_written < buf_bytes) {
          multilog (pwcm->log, LOG_ERR, "Cannot write %"PRIu64
                    " bytes to Data Block\n", buf_bytes);
          return DADA_ERROR_FATAL;
        }

      }

      total_bytes_written += buf_bytes;

      if (bytes_to_write)
        bytes_to_write -= buf_bytes;

      if (pwcm->verbose)
        multilog (pwcm->log, LOG_INFO, "Written %"PRIu64" bytes\n",
                  total_bytes_written);

    }

    if (pwcm->verbose)
      multilog (pwcm->log, LOG_INFO, "transit=%"PRIu64" total=%"PRIu64"\n",
                transit_byte, total_bytes_written);

    if (transit_byte == total_bytes_written) {

#ifdef _DEBUG
      fprintf (stderr, "dada_pwc_main_transfer_data: transit state\n");
#endif

      /* The transit_byte has been reached, it is now time to change state */
      /* reset the transit_byte */ 
      transit_byte = 0;

      if (pwcm->command.code ==  dada_pwc_record_stop) {

        multilog (pwcm->log, LOG_INFO, "record stop\n");

        /* stop valid data on the Data Block at the last byte written */
        if (pwcm->data_block && ipcio_stop (pwcm->data_block) < 0){
          multilog (pwcm->log, LOG_ERR, "Could not stop data block\n");
          return DADA_ERROR_FATAL;
        }
        
        dada_pwc_set_state (pwcm->pwc, dada_pwc_clocking, 0);
        
      }

      else if (pwcm->command.code ==  dada_pwc_record_start) {

        multilog (pwcm->log, LOG_INFO, "record start\n");

        if (dada_pwc_main_record_start (pwcm) < 0)
          return DADA_ERROR_HARD;
        
        dada_pwc_set_state (pwcm->pwc, dada_pwc_recording, 0);

      } 

      else if (pwcm->command.code == dada_pwc_stop) {
#ifdef _DEBUG
        multilog (pwcm->log, LOG_INFO, "stopping... entering idle state\n");
#endif
        return 0;
      }
      
      else if (pwcm->command.code ==  dada_pwc_set_utc_start) {

        if (total_bytes_written) {
          multilog (pwcm->log, LOG_ERR, "Error. unexpected set_utc_start\n");
          return DADA_ERROR_HARD;
        }

      } else if (pwcm->command.code == dada_pwc_clock) {

        if (total_bytes_written) {
          multilog (pwcm->log, LOG_ERR, "Error. unexpected clock command\n");
          return DADA_ERROR_HARD;
        }

      } else if (pwcm->command.code == dada_pwc_start) {

        if (total_bytes_written) {
          multilog (pwcm->log, LOG_ERR, "Error. unexpected start command\n");
          return DADA_ERROR_HARD;
        }
        
      } else  {
        /* enter the idle state */
        multilog (pwcm->log, LOG_ERR, "Error. unpected command: %d\n",
                  pwcm->command.code);
        return DADA_ERROR_HARD;

      }

      /* When changing between clocking and recording states, there
         may remain unwritten data in the buffer.  Write this data to
         the Data Block */

      /* offset the buffer pointer by the amount already written */
      buffer += buf_bytes;
      /* set the number of remaining samples */
      buf_bytes = buffer_size - buf_bytes;

      if (buf_bytes && pwcm->data_block) {

        /* If we run out of empty buffers and don't yet have a UTC_START,
         * fail */
        if ((!utc_start_set) && (ipcio_space_left(pwcm->data_block) < buf_bytes)) {
          multilog (pwcm->log, LOG_ERR, "Data block full and UTC_START not "
                    "set.\n");
          return DADA_ERROR_FATAL;
        }

        /* write the bytes to the Data Block */
        if (ipcio_write (pwcm->data_block, buffer, buf_bytes) < buf_bytes) {

          multilog (pwcm->log, LOG_ERR, "Cannot write %"PRIu64
                    " bytes to Data Block\n", buf_bytes);
          return DADA_ERROR_FATAL;

        }

        total_bytes_written += buf_bytes;

      }

    }

  }

  return 0;

}

/*! The transit to idle/prepared state */
int dada_pwc_main_stop_transfer (dada_pwc_main_t* pwcm)
{

  /* Reset the header so that it is not valid anymore */
  pwcm->header_valid = 0;
        
  if (pwcm->stop_function (pwcm) < 0)  {
    multilog (pwcm->log, LOG_ERR, "dada_pwc_main_stop_transfer"
              " stop function returned error code\n");
    return DADA_ERROR_HARD;
  }


  /* close the Data Block */
  if (pwcm->data_block && ipcio_close (pwcm->data_block) < 0)  {
    multilog (pwcm->log, LOG_ERR, "Could not close Data Block\n");
    return DADA_ERROR_FATAL;
  }

  if (pwcm->pwc->state != dada_pwc_soft_error &&
      pwcm->pwc->state != dada_pwc_hard_error &&
      pwcm->pwc->state != dada_pwc_fatal_error)  
    dada_pwc_set_state (pwcm->pwc, dada_pwc_idle, 0);

  return 0;
}

void dada_pwc_main_process_error (dada_pwc_main_t* pwcm, int rval) {

  int new_state = pwcm->pwc->state;

  switch (rval) 
  {
    case DADA_ERROR_SOFT:
      if ( (pwcm->pwc->state != dada_pwc_hard_error) && 
           (pwcm->pwc->state != dada_pwc_fatal_error) )
        new_state = dada_pwc_soft_error;
      break;

    case DADA_ERROR_HARD:
      if (pwcm->pwc->state != dada_pwc_fatal_error)
        new_state = dada_pwc_hard_error;
      break;

    case DADA_ERROR_FATAL:
      new_state = dada_pwc_fatal_error;
      break;

    default:
      multilog (pwcm->log, LOG_ERR, "Unknown error state: %d\n",rval);
      new_state = dada_pwc_fatal_error;
  }

  multilog(pwcm->log, LOG_WARNING, "PWC entering error state: %s\n",
           dada_pwc_state_to_string(new_state));

  if (dada_pwc_set_state (pwcm->pwc, new_state, 0) < 0) 
    multilog(pwcm->log, LOG_ERR, "Failed to change state from %s to %s\n",
             dada_pwc_state_to_string(pwcm->pwc->state),
             dada_pwc_state_to_string(new_state));


}
