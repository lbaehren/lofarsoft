/***************************************************************************
 *  
 *    Copyright (C) 2009 by Andrew Jameson
 *    Licensed under the Academic Free License version 2.1
 * 
 ****************************************************************************/

/*
 * gmrt_udpdb
 */

#include <sys/socket.h>
#include <pthread.h>
#include "sock.h"
#include <math.h>

#include "gmrt_udpdb.h"
#include "dada_generator.h"

#define _DEBUG 1

int quit_threads = 0;

void usage()
{
  fprintf (stdout,
     "gmrt_udpdb [options]\n"
     " -h             print help text\n"
     " -i iface       interface for UDP packets [default all interfaces]\n"
     " -p port        port to open for UDP packets [default %d]\n"
     " -c port        port to open for PWCC commands [defalt %d]\n"
     " -l port        port to make multilog output available on [defalt %d]\n"
     " -k key         hexidecimal shared memory key [default %x]\n"
     " -n secs        manually acquire for secs\n"
     " -r             start only when packet sequence number resets\n"
     " -H file        load header from file, run without PWCC\n"
     " -d             run as a daemon\n"
     " -v             verbose messages\n\n",
     GMRT_DEFAULT_UDPDB_PORT, GMRT_DEFAULT_PWC_PORT, 
     GMRT_DEFAULT_PWC_LOGPORT, DADA_DEFAULT_BLOCK_KEY);
}


/* 
 * udpdb_header_valid_function
 *
 * Determine if the header is valid. Returns 1 if valid, 0 otherwise 
 */
int udpdb_header_valid_function (dada_pwc_main_t* pwcm) {

  unsigned utc_size = 64;
  char utc_buffer[utc_size];
  int valid = 1;

  /* Check if the UTC_START is set in the header*/
  if (ascii_header_get (pwcm->header, "UTC_START", "%s", utc_buffer) < 0) {
    valid = 0;
  }

  /* Check whether the UTC_START is set to UNKNOWN */
  if (strcmp(utc_buffer,"UNKNOWN") == 0)
    valid = 0;

  return valid;
}

/*
 * udpdb_error_function
 *
 * called when the buffer_function returns 0 bytes. return value indicates 
 * the action dada_pwc_main should take:
 *   0  everything should be ok, continue
 *   1  we are trying to recover from an error
 *   2  an unrecoverable error has ocurred
 */
int udpdb_error_function (dada_pwc_main_t* pwcm) {

  udpdb_t *ctx = (udpdb_t*)pwcm->context;
  int error = 0;

  /* check if the header is valid */
  if (udpdb_header_valid_function (pwcm))
    error = 2;
  else 
    error = 0;

  return error;

}


/*
 *  udpdb_start_function
 * 
 *  start the xfer, setting the OBS_OFFSET and RECV_HOST
 */
time_t udpdb_start_function (dada_pwc_main_t* pwcm, time_t start_utc)
{
#ifdef _DEBUG 
  fprintf(stderr, "udpdb_start_function()\n");
#endif

  udpdb_t *ctx = (udpdb_t*)pwcm->context;
  multilog_t* log = pwcm->log;

  uint64_t first_byte = 0;
  ascii_header_set (pwcm->header, "OBS_OFFSET", "%"PRIu64, first_byte);

  /* set header param RECV_HOST */
  char myhostname[HOST_NAME_MAX] = "unknown";
  gethostname(myhostname,HOST_NAME_MAX); 
  ascii_header_set (pwcm->header, "RECV_HOST", "%s", myhostname);

  gmrt_receiver_start(ctx->receiver);

  return 0;
}


/*
 * Read data from the UDP socket and return a pointer to the data read
 * and set the size to the data read
 */
void* udpdb_buffer_function (dada_pwc_main_t* pwcm, uint64_t* size)
{
  udpdb_t *ctx = (udpdb_t*) pwcm->context;
  return gmrt_buffer_function(ctx->receiver, size);
}


/*
 * Close the udp socket and file
 */

int udpdb_stop_function (dada_pwc_main_t* pwcm)
{

  // close udp socket ?
  return 0;

}


int main (int argc, char **argv)
{

  /* DADA Primary Write Client main loop  */
  dada_pwc_main_t* pwcm = 0;
  
  /* DADA Header plus Data Unit */
  dada_hdu_t* hdu = 0;
  
  /* DADA Logger */ 
  multilog_t* log = 0;

  /* Interface on which to listen for udp packets */
  char * interface = "any";

  /* port for UDP packets */
  int u_port = GMRT_DEFAULT_UDPDB_PORT;

  /* pwcc command port */
  int c_port = GMRT_DEFAULT_PWC_PORT;
  
  /* multilog output port */
  int l_port = GMRT_DEFAULT_PWC_LOGPORT;

  /* Flag set in daemon mode */
  char daemon = 0;

  /* Flag set in verbose mode */
  char verbose = 0;

  /* number of seconds/bytes to acquire for */
  unsigned nsecs = 0;

  /* hexadecimal shared memory key */
  key_t dada_key = DADA_DEFAULT_BLOCK_KEY;

  /* actual struct with info */
  udpdb_t udpdb;

  /* the receiver */
  gmrt_receiver_t receiver;

  /* custom header from a file, implies no controlling pwcc */
  char * header_file = NULL;

  /* pwcc control flag, only disabled if a custom header is supplied */
  int pwcc_control = 1;

  /* Pointer to array of "read" data */
  char *src;

  /* whether to start recording on a packet reset */
  unsigned start_on_reset = 0;

  int arg = 0;

  while ((arg=getopt(argc,argv,"c:dH:k:i:l:n:p:rvh")) != -1) {
    switch (arg) {

    case 'c':
      if (optarg) {
        c_port = atoi(optarg);
        break;
      } else {
        usage();
        return EXIT_FAILURE;
      }

    case 'd':
      daemon = 1;
      break; 
  
    case 'H':
      if (optarg) {
        header_file = optarg;
        pwcc_control = 0;
      } else {
        fprintf(stderr,"-H flag requires a file arguement\n");
        usage();
      }
      break;

    case 'k':
      if (sscanf (optarg, "%x", &dada_key) != 1) {
        fprintf (stderr,"gmrt_udpdb: could not parse key from %s\n",optarg);
        return -1;
      }
      break;

    case 'i':
      if (optarg)
        interface = optarg;
      break;
    
    case 'l':
      if (optarg) {
        l_port = atoi(optarg);
        break;
      } else {
        usage();
        return EXIT_FAILURE;
      }

    case 'n':
      nsecs = atoi (optarg);
      break;

    case 'p':
      u_port = atoi (optarg);
      break;

    case 'r':
      start_on_reset = 1;
      break;

    case 'v':
      verbose = 1;
      break;

    case 'h':
      usage();
      return 0;
      
    default:
      usage ();
      return 0;
      
    }
  }


  /* parse required command line options */
  if (argc-optind != 0) {
    fprintf(stderr, "ERROR: 0 command line arguments are expeced\n\n");
    usage();
    exit(EXIT_FAILURE);
  }

  log = multilog_open ("gmrt_udpdb", 0);

  if (daemon)
    be_a_daemon ();
  else
    multilog_add (log, stderr);

  multilog_serve (log, l_port);

  pwcm = dada_pwc_main_create();

  pwcm->log                   = log;
  pwcm->context               = &udpdb;
  pwcm->start_function        = udpdb_start_function;
  pwcm->buffer_function       = udpdb_buffer_function;
  pwcm->stop_function         = udpdb_stop_function;
  pwcm->error_function        = udpdb_error_function;
  pwcm->header_valid_function = udpdb_header_valid_function;
  pwcm->verbose               = verbose;

  /* Setup receiver struct */
  receiver.log            = log;
  receiver.verbose        = verbose;
  receiver.interface      = strdup(interface);
  receiver.port           = u_port;
  receiver.start_on_reset = start_on_reset;

  udpdb.receiver          = &receiver;

  gmrt_receiver_init(&receiver, 1);

  /* connect to the shared memory block */
  hdu = dada_hdu_create (pwcm->log);
  dada_hdu_set_key(hdu, dada_key);

  if (dada_hdu_connect (hdu) < 0)
    return EXIT_FAILURE;

  if (dada_hdu_lock_write (hdu) < 0)
    return EXIT_FAILURE;

  pwcm->data_block = hdu->data_block;
  pwcm->header_block = hdu->header_block;

  /* we are controlled by PWC control interface */
  if (pwcc_control) {

    pwcm->header = hdu->header;

    if (verbose) 
      fprintf (stdout, "gmrt_udpdb: creating dada pwc control interface\n");

    pwcm->pwc = dada_pwc_create();

    pwcm->pwc->port = c_port;

    if (verbose) 
      fprintf (stdout, "gmrt_udpdb: creating dada server\n");
    if (dada_pwc_serve (pwcm->pwc) < 0) {
      fprintf (stderr, "gmrt_udpdb: could not start server\n");
      return EXIT_FAILURE;
    }

    if (verbose) 
      fprintf (stdout, "gmrt_udpdb: entering PWC main loop\n");

    if (dada_pwc_main (pwcm) < 0) {
      fprintf (stderr, "gmrt_udpdb: error in PWC main loop\n");
      return EXIT_FAILURE;
    }

    if (dada_hdu_unlock_write (hdu) < 0)
      return EXIT_FAILURE;

    if (dada_hdu_disconnect (hdu) < 0)
      return EXIT_FAILURE;

    if (verbose) 
      fprintf (stdout, "gmrt_udpdb: destroying pwc\n");
    dada_pwc_destroy (pwcm->pwc);

    if (verbose) 
      fprintf (stdout, "gmrt_udpdb: destroying pwc main\n");
    dada_pwc_main_destroy (pwcm);


  /* If a custom header argument was supplied */
  } else {

    fprintf(stdout,"gmrt_udpdb: Manual Mode\n"); 

    /* statistics thread */
    pthread_t stats_thread_id;

    multilog(log, LOG_INFO, "starting stats_thread()\n");
    if (pthread_create (&stats_thread_id, 0, (void *) stats_thread, (void *) &receiver) < 0) {
      perror ("Error creating new thread");
      return -1;
    }

    char *   header_buf = 0;
    uint64_t header_size = 0;
    char *   buffer = 0;
    unsigned buffer_size = 64;

    /* get the next header */
    header_size = ipcbuf_get_bufsz (hdu->header_block);
    multilog (pwcm->log, LOG_INFO, "header block size = %llu\n", header_size);
    pwcm->header = ipcbuf_get_next_write (hdu->header_block);

    if (!pwcm->header)  {
      multilog (pwcm->log, LOG_ERR, "could not get next header block\n");
      return EXIT_FAILURE;
    }

    /* allocate some memory for the header */
    fprintf(stdout,"gmrt_udpdb: reading custom header %s\n", header_file);
    header_buf = (char *) malloc(sizeof(char) * header_size);
    if (!header_buf) {
      multilog (pwcm->log, LOG_ERR, "could not allocate memory for header_buf\n");
      return EXIT_FAILURE;
    }

    buffer = (char *) malloc (sizeof(char) * buffer_size);
    if (!buffer) {
      multilog (pwcm->log, LOG_ERR, "could not allocate memory for buffer\n");
      return EXIT_FAILURE;
    }

    /* read the header from file */
    if (fileread (header_file, header_buf, header_size) < 0)  {
      multilog (pwcm->log, LOG_ERR, "could not read header from %s\n",
                header_file);
      return EXIT_FAILURE;
    }
  
    time_t utc = time(0);
    strftime (buffer, buffer_size, DADA_TIMESTR, gmtime(&utc));
    if (ascii_header_set (header_buf, "UTC_START", "%s", buffer) < 0) {
      multilog (pwcm->log, LOG_ERR, "failed ascii_header_set UTC_START\n");
      return EXIT_FAILURE;
    }

    /* HACK just set the UTC_START to roughly now */
    fprintf(stdout, "gmrt_udpdb: UTC_START [now] = %s\n",buffer);

    /* copy the header_buf to the header block */
    memcpy(pwcm->header, header_buf, header_size);

    if (verbose)
      fprintf(stdout, "gmrt_udpdb: running start function\n");

    /* note that the start function will set the OBS_OFFSET & RECV_HOST */
    utc = udpdb_start_function(pwcm, 0);
    if (utc == -1 ) {
      multilog(pwcm->log, LOG_ERR, "Could not run start function\n");
      return EXIT_FAILURE;
    } 

    /* marked the header as filled */
    if (ipcbuf_mark_filled (hdu->header_block, header_size) < 0)  {
      multilog (pwcm->log, LOG_ERR, "Could not mark filled header block\n");
      return EXIT_FAILURE;
    }

    multilog (pwcm->log, LOG_INFO, "header marked filled\n");

    int64_t bytes_to_acquire = 1;
    uint64_t first_byte = 0;

    if (nsecs) 
      bytes_to_acquire = 800 * 1000 * 1000 * (int64_t) nsecs;

    fprintf(stderr, "bytes_to_acquire = %"PRIu64", nsecs=%d\n", bytes_to_acquire, nsecs);

    uint64_t bsize = 0;

    while (bytes_to_acquire > 0) {

      bsize = (UDP_DATA * UDP_NPACKS);

      src = (char *) udpdb_buffer_function(pwcm, &bsize);

      /* if some data was captured */
      if (bsize) {

        /* write data to datablock */
        if ( ipcio_write(hdu->data_block, src, bsize) < bsize ) {
          multilog(pwcm->log, LOG_ERR, "Cannot write requested bytes to SHM\n");
          bytes_to_acquire = -1;
        }

        if (nsecs)
          bytes_to_acquire -= bsize;

      /* the buffer function returned 0, stop */
      } else {

        multilog(pwcm->log, LOG_INFO, "main: buffer_function returned 0 bytes\n");
        bytes_to_acquire = -1;

      }
    }

    fprintf(stderr, "udpdb_stop_function\n");
    if ( udpdb_stop_function(pwcm) != 0)
      fprintf(stderr, "Error stopping acquisition");

    fprintf(stderr, "dada_hdu_unlock_write\n");
    if (dada_hdu_unlock_write (hdu) < 0)
      return EXIT_FAILURE;

    fprintf(stderr, "dada_hdu_disconnect\n");
    if (dada_hdu_disconnect (hdu) < 0)
      return EXIT_FAILURE;

    /* join threads */
    quit_threads = 1;
    void* result = 0;
    fprintf(stderr, "joining stats_thread\n");
    pthread_join (stats_thread_id, &result);

  }

  gmrt_receiver_dealloc(&receiver);

  return EXIT_SUCCESS;

}

/* 
 *  Thread to print simple capture statistics
 */
void stats_thread(void * arg) {

  gmrt_receiver_t * ctx = (gmrt_receiver_t *) arg;

  uint64_t bytes_received_total = 0;
  uint64_t bytes_received_this_sec = 0;
  uint64_t bytes_dropped_total = 0;
  uint64_t bytes_dropped_this_sec = 0;
  double   mb_received_ps = 0;
  double   mb_dropped_ps = 0;

  struct timespec ts;

  while (!quit_threads)
  {
    bytes_received_this_sec = ctx->bytes->received - bytes_received_total;
    bytes_dropped_this_sec  = ctx->bytes->dropped - bytes_dropped_total;

    bytes_received_total = ctx->bytes->received;
    bytes_dropped_total = ctx->bytes->dropped;

    mb_received_ps = (double) bytes_received_this_sec / 1000000;
    mb_dropped_ps = (double) bytes_dropped_this_sec / 1000000;

    fprintf(stderr,"T=%5.2f, R=%5.2f MB/s\t D=%5.2f MB/s packets=%"PRIu64" dropped=%"PRIu64"\n", (mb_received_ps+mb_dropped_ps), mb_received_ps, mb_dropped_ps, ctx->packets->received, ctx->packets->dropped);
    sleep(1);
  }
}

