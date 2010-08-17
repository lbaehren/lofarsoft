#include "dada_pwc_main.h"
#include "dada_hdu.h"
#include "dada_def.h"
#include "daemon.h"

#include <unistd.h> /* sleep */
#include <stdlib.h>
#include <assert.h>

time_t fake_start_function (dada_pwc_main_t* pwcm, time_t utc)
{
  time_t now = time(0);
  unsigned sleep_time = 0;

  if (utc > now) {
    sleep_time = utc - now;
    multilog (pwcm->log, LOG_INFO, "sleeping for %u sec\n", sleep_time);
    sleep (sleep_time);
    return utc;
  }

  return now;
}

static char* buffer = 0;
static uint64_t buffer_size = 0;

void* fake_buffer_function (dada_pwc_main_t* pwcm, uint64_t* size)
{
  uint64_t fake_buffer_size = pwcm->pwc->bytes_per_second;

  /* to simulate buffer size equal to ring buffer size
     fake_buffer_size = ipcbuf_get_bufsz ((ipcbuf_t*)pwcm->data_block);
   */

  if (buffer_size < fake_buffer_size) {
    buffer_size = fake_buffer_size;
    buffer = realloc (buffer, buffer_size);
    assert (buffer != 0);
  }

  sleep (1);

  *size = fake_buffer_size;
  return buffer;
}

int fake_stop_function (dada_pwc_main_t* pwcm)
{
  return 0;
}

int main (int argc, char** argv)
{
  /* Primary write client main loop  */
  dada_pwc_main_t* pwcm = 0;

  /* DADA Header plus Data Block */
  dada_hdu_t* hdu = 0;

  /* DADA Logger */
  multilog_t* log = 0;

  /* flags */
  int connect_hdu = 0;

  char daemon = 0; /*daemon mode */
  int verbose = 0; /* verbose mode */

  int arg = 0;
  while ((arg=getopt(argc,argv,"dhlv")) != -1)
    switch (arg) {
	    
    case 'd':
      daemon=1;
      break;

    case 'l':
      connect_hdu = 1;
      break;

    case 'v':
      verbose=1;
      break;
      
    }

  log = multilog_open ("dada_pwc_demo", daemon);
    
  /* set up for daemon usage */	  
  if (daemon)
    be_a_daemon ();
  else
    multilog_add (log, stderr);
  
  multilog_serve (log, DADA_DEFAULT_PWC_LOG);

  fprintf (stderr, "Creating dada pwc main\n");
  pwcm = dada_pwc_main_create ();

  pwcm->start_function  = fake_start_function;
  pwcm->buffer_function = fake_buffer_function;
  pwcm->stop_function   = fake_stop_function;
  pwcm->log             = log;

  if (connect_hdu) {

    /* connect to the shared memory */
    fprintf (stderr, "Connecting to shared memory\n");
    hdu = dada_hdu_create (log);
    
    if (dada_hdu_connect (hdu) < 0)
      return -1;
    
    if (dada_hdu_lock_write (hdu) < 0)
      return -1;
    
    pwcm->data_block      = hdu->data_block;
    pwcm->header_block    = hdu->header_block;

  }

  fprintf (stderr, "Creating dada pwc control interface\n");
  pwcm->pwc = dada_pwc_create ();

  if (dada_pwc_serve (pwcm->pwc) < 0) {
    fprintf (stderr, "dada_pwc_demo: could not start server\n");
    return -1;
  }

  fprintf (stderr, "Available on port %d\n", pwcm->pwc->port);

  if (dada_pwc_main (pwcm) < 0) {
    fprintf (stderr, "dada_pwc_demo: error in main loop\n");
    return -1;
  }

  if (connect_hdu)
  {
    if (dada_hdu_unlock_write (hdu) < 0)
      return -1;

    if (dada_hdu_disconnect (hdu) < 0)
      return -1;
  }

  fprintf (stderr, "Destroying pwc\n");
  dada_pwc_destroy (pwcm->pwc);

  fprintf (stderr, "Destroying pwc main\n");
  dada_pwc_main_destroy (pwcm);

  return 0;
}

