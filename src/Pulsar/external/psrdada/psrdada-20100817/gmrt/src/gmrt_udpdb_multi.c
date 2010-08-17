/***************************************************************************
 *  
 *    Copyright (C) 2009 by Andrew Jameson
 *    Licensed under the Academic Free License version 2.1
 * 
 ****************************************************************************/

/*
 * gmrt_udpdb_mult
 *
 * read UDP packets and demultiplex them into multiple datablocks
 */

#include <sys/socket.h>
#include <pthread.h>
#include "sock.h"
#include <math.h>

#include "gmrt_udpdb_multi.h"
#include "dada_generator.h"

#define _DEBUG 1

int quit_threads = 0;

void usage()
{
  fprintf (stdout,
      "gmrt_udpdb_multi [options] num_hdus hdu_1 ... hdu_n\n"
     " -h             print help text\n"
     " -a antsys      antsys.hdr file [default `pwd`/antsys.hdr]\n"
     " -i iface       interface for UDP packets [default all interfaces]\n"
     " -p port        port to open for UDP packets [default %d]\n"
     " -c port        port to open for PWCC commands [defalt %d]\n"
     " -l port        port to make multilog output available on [defalt %d]\n"
     " -o source      source.hdr file [default `pwd`/source.hdr]\n"
     " -n secs        manually acquire for secs\n"
     " -r             start only when packet sequence number resets\n"
     " -H file        load header from file, run without PWCC\n"
     " -d             run as a daemon\n"
     " -s sampler     sampler.hdr file [default `pwd`/sampler.hdr]\n"
     " -v             verbose messages\n\n"
     " num_hdus should correspond to the number of antennas encoded in the UDP packets\n\n",
     GMRT_DEFAULT_UDPDB_PORT, GMRT_DEFAULT_PWC_PORT, 
     GMRT_DEFAULT_PWC_LOGPORT);
}


/* 
 * gmrt_udpdb_multi_header_valid_function
 *
 * Determine if the header is valid. Returns 1 if valid, 0 otherwise 
 */
int gmrt_udpdb_multi_header_valid_function (dada_pwc_main_multi_t* pwcm) {

  unsigned utc_size = 64;
  char utc_buffer[utc_size];
  int valid = 1;

  /* Check if the UTC_START is set in the header*/
  if (ascii_header_get (pwcm->hdus[0]->header, "UTC_START", "%s", utc_buffer) < 0) {
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
 * the action dada_pwc_main_multi should take:
 *   0  everything should be ok, continue
 *   1  we are trying to recover from an error
 *   2  an unrecoverable error has ocurred
 */
int gmrt_udpdb_multi_error_function (dada_pwc_main_multi_t* pwcm) {

  gmrt_udpdb_multi_t *ctx = (gmrt_udpdb_multi_t*)pwcm->context;
  int error = 0;

  /* check if the header is valid */
  if (gmrt_udpdb_multi_header_valid_function (pwcm))
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
time_t gmrt_udpdb_multi_start_function (dada_pwc_main_multi_t* pwcm, time_t start_utc)
{

  gmrt_udpdb_multi_t *ctx = (gmrt_udpdb_multi_t*)pwcm->context;
  multilog_t* log = pwcm->log;

  uint64_t first_byte = 0;
  unsigned i = 0;
  
  for (i=0; i<pwcm->num_hdus; i++)
  {
    ascii_header_set (pwcm->hdus[i]->header, "OBS_OFFSET", "%"PRIu64, first_byte);

    /* set header param RECV_HOST */
    char myhostname[HOST_NAME_MAX] = "unknown";
    gethostname(myhostname,HOST_NAME_MAX); 
    ascii_header_set (pwcm->hdus[i]->header, "RECV_HOST", "%s", myhostname);
  }

  if (ascii_header_get (pwcm->hdus[0]->header, "TSAMP", "%lf", &(ctx->receiver->t_samp)) < 0) {
    multilog(log, LOG_ERR, "TSAMP not set in header\n");
    return -1;
  }

  if (ascii_header_get (pwcm->hdus[0]->header, "NBIT", "%d", &(ctx->receiver->nbit)) < 0) {
    multilog(log, LOG_ERR, "NBIT not set in header\n");
    return -1;
  }

  if (ascii_header_get (pwcm->hdus[0]->header, "NDIM", "%d", &(ctx->receiver->ndim)) < 0) {
    multilog(log, LOG_ERR, "NDIM not set in header\n");
    return -1;
  }

  if (ascii_header_get (pwcm->hdus[0]->header, "NCHAN", "%d", &(ctx->receiver->nchan)) < 0) {
    multilog(log, LOG_ERR, "NCHAN not set in header\n");
    return -1;
  }

  if (ascii_header_get (pwcm->hdus[0]->header, "NPOL", "%d", &(ctx->receiver->npol)) < 0) {
    multilog(log, LOG_ERR, "NPOL not set in header\n");
    return -1;
  }

  // ignore pol as this will be 2 for the testing mode...
  ctx->receiver->bytes_per_sample = ctx->receiver->npol * ctx->receiver->nbit * ctx->receiver->ndim * ctx->receiver->nchan / 8.0;
  //ctx->receiver->bytes_per_second /= ctx->receiver->npol;
  ctx->receiver->bytes_per_second = (ctx->receiver->bytes_per_sample / ctx->receiver->t_samp) * 1000000;

  ctx->receiver->bytes_per_sample = 1;
  double t_samp = 0.0025;
  ctx->receiver->bytes_per_second = (ctx->receiver->bytes_per_sample / t_samp) * 1000000;

  for (i=0; i<pwcm->num_hdus; i++)
    ascii_header_set (pwcm->hdus[i]->header, "BYTES_PER_SECOND", "%lf", ctx->receiver->bytes_per_second);

  multilog (log, LOG_INFO, "B/sample=%lf, TSAMP=%lf, B/second=%lf\n", ctx->receiver->bytes_per_sample, 
            ctx->receiver->t_samp, ctx->receiver->bytes_per_second);

  unsigned utc_size = 64;
  char utc_buffer[utc_size];

  /* Check if the UTC_START is set in the header*/
  if (ascii_header_get (pwcm->hdus[0]->header, "UTC_START", "%s", utc_buffer) < 0) {
    multilog(log, LOG_ERR, "UTC_START not set\n");
    return -1;
  }

  // convert the UTC_START time string to a time_t
  time_t utc = str2utctime (utc_buffer);
  if (utc == (time_t)-1) {
    multilog(ctx->log, LOG_WARNING, "Could not parse start time from '%s'\n", utc_buffer);
    return -1;
  }

  // set the utc_start timestamp;
  ctx->utc_start.tv_sec = utc;
  ctx->utc_start.tv_usec = 0;

  gmrt_receiver_start(ctx->receiver);

  return 0;
}


/*
 * Read data from the UDP socket and return a pointer to the data read
 * and set the size to the data read
 */
void ** gmrt_udpdb_multi_buffer_function (dada_pwc_main_multi_t* pwcm, uint64_t* size)
{
  gmrt_udpdb_multi_t *ctx = (gmrt_udpdb_multi_t*) pwcm->context;

  /* calculate the delays */
  ctx->receiver->total_time = ctx->receiver->total_bytes / ctx->receiver->bytes_per_second;

  ctx->timestamp.tv_sec = (int) ctx->receiver->total_time;
  ctx->timestamp.tv_usec = (ctx->receiver->total_time - ctx->timestamp.tv_sec) * 1000000;

  //multilog(ctx->log, LOG_INFO, "total_bytes=%lf, total_time=%lf s, timestamp.tv_sec = %d, timestamp.tv_usec = %lf\n", ctx->receiver->total_bytes, ctx->receiver->total_time, ctx->timestamp.tv_sec, ctx->timestamp.tv_usec);

  ctx->timestamp.tv_sec += ctx->utc_start.tv_sec;

  //multilog(ctx->log, LOG_INFO, "timestamp.tv_sec = %d\n", ctx->timestamp.tv_sec);

  calculate_delays(&(ctx->source), &(ctx->corr), ctx->timestamp, ctx->delays, ctx->fringes);

  // now that we have the delays, update the delay_ptrs (for "next")
  multilog(ctx->log, LOG_INFO, "stream[0] delay offset %2.16lf s -> %d bytes -> %d bytes\n",ctx->delays[0].delay_t0,
                                (int) (ctx->delays[0].delay_t0 * ctx->receiver->bytes_per_second),
                                GMRT_SAMPLE_DELAY_OFFSET - (int) (ctx->delays[0].delay_t0 * ctx->receiver->bytes_per_second)); 
  ctx->receiver->next->delay_ptrs[0] = ctx->receiver->next->bufs[0] + (GMRT_SAMPLE_DELAY_OFFSET - (int) (ctx->delays[0].delay_t0 * ctx->receiver->bytes_per_second));

  multilog(ctx->log, LOG_INFO, "stream[1] delay offset %2.16lf s -> %d bytes -> %d bytes\n",ctx->delays[1].delay_t0,
                                (int) (ctx->delays[1].delay_t0 * ctx->receiver->bytes_per_second),
                                GMRT_SAMPLE_DELAY_OFFSET - (int) (ctx->delays[1].delay_t0 * ctx->receiver->bytes_per_second));
  ctx->receiver->next->delay_ptrs[1] = ctx->receiver->next->bufs[1] + (GMRT_SAMPLE_DELAY_OFFSET - (int) (ctx->delays[1].delay_t0 * ctx->receiver->bytes_per_second));

  return gmrt_multi_buffer_function(ctx->receiver, size);

}


/*
 * Close the udp socket and file
 */

int gmrt_udpdb_multi_stop_function (dada_pwc_main_multi_t* pwcm)
{

  // close udp socket ?
  return 0;

}


int main (int argc, char **argv)
{

  /* DADA Primary Write Client main loop  */
  dada_pwc_main_multi_t* pwcm = 0;
  
  /* DADA Header plus Data Unit */
  //dada_hdu_t* hdu = 0;
  
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
  gmrt_udpdb_multi_t udpdb;

  /* the receiver */
  gmrt_receiver_t receiver;

  /* custom header from a file, implies no controlling pwcc */
  char * header_file = NULL;

  /* pwcc control flag, only disabled if a custom header is supplied */
  int pwcc_control = 1;

  /* Pointer to array of "read" data */
  char *src;

  /* thread to handle integer delay correction, updates */
  pthread_t delay_update_tid;

  /* number of hdus to connect to */
  unsigned num_hdus = 0;

  /* antsys.hdr file */
  char * antsys_hdr = "antsys.hdr";

  /* sampler.hdr file */
  char * sampler_hdr = "sampler.hdr";

  /* source.hdr file */
  char * source_hdr = "source.hdr";

  /* whether to start recording on a packet reset */
  unsigned start_on_reset = 0;

  unsigned i = 0;

  int arg = 0;

  while ((arg=getopt(argc,argv,"a:c:dH:k:i:l:n:o:p:rs:vh")) != -1) {
    switch (arg) {

    case 'a':
      if (optarg) {
        antsys_hdr = strdup(optarg);
        break;
      } else {
        usage();
        return EXIT_FAILURE;
      }

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

    case 'o':
      if (optarg) {
        source_hdr = strdup(optarg);
        break;
      } else {
        usage();
        return EXIT_FAILURE;
      }

    case 'p':
      u_port = atoi (optarg);
      break;

    case 'r':
      start_on_reset = 1;
      break;

    case 's':
      if (optarg) {
        sampler_hdr = strdup(optarg);
        break;
      } else {
        usage();
        return EXIT_FAILURE;
      }

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

  if (verbose)
    fprintf(stderr, "parsing command line args\n");

  /* parse required command line options */
  if (argc-optind < 2) {
    fprintf(stderr, "ERROR: at least 2 command line arguments are expeced\n\n");
    usage();
    exit(EXIT_FAILURE);
  }

  num_hdus = atoi(argv[optind]);

  if (argc-optind != num_hdus+1)
  {
    fprintf(stderr, "ERROR: expected %d dada keys to be specifed, found %d\n\n", num_hdus, (argc-optind)-1);
    usage();
    exit(EXIT_FAILURE);
  }

  key_t dada_keys[num_hdus];

  for (i=0; i<num_hdus; i++)
  {
    if (sscanf (argv[optind+1+i], "%x", &(dada_keys[i])) != 1)
    {
      fprintf(stderr, "ERROR: could not parse dada key %d\n", i);
      return -1;
    } 
  }

  log = multilog_open ("gmrt_udpdb", 0);

  if (daemon)
    be_a_daemon ();
  else
    multilog_add (log, stderr);

  multilog_serve (log, l_port);

  if (verbose)
    fprintf(stderr, "creating dada_pwc_main_multi\n");
  pwcm = dada_pwc_main_multi_create(num_hdus);

  pwcm->log                   = log;
  pwcm->context               = &udpdb;
  pwcm->start_function        = gmrt_udpdb_multi_start_function;
  pwcm->buffer_function       = gmrt_udpdb_multi_buffer_function;
  pwcm->stop_function         = gmrt_udpdb_multi_stop_function;
  pwcm->error_function        = gmrt_udpdb_multi_error_function;
  pwcm->header_valid_function = gmrt_udpdb_multi_header_valid_function;
  pwcm->verbose               = verbose;

  /* Setup receiver struct */
  receiver.log            = log;
  receiver.verbose        = verbose;
  receiver.interface      = strdup(interface);
  receiver.port           = u_port;
  receiver.start_on_reset = start_on_reset;
  udpdb.receiver          = &receiver;
  udpdb.log               = log;

  gmrt_receiver_init(&receiver, num_hdus);

  // read the antennasys header file
  if (read_antenna_file(antsys_hdr, udpdb.corr.antenna) < 0)
    return EXIT_FAILURE;

  // read the antenna connectivity
  if (read_antenna_connectivity(sampler_hdr, &(udpdb.corr)) < 0)
    return EXIT_FAILURE;

  // read the source header file TODO, should be integrated into start
  // function and read these values from the header...
  double BW;
  char source_name[100];
  if (read_source_file(source_hdr, &(udpdb.source), &BW, source_name ) < 0)
    return EXIT_FAILURE;

  /* connect to the shared memory block */
  for (i=0; i<num_hdus; i++)
  {
    pwcm->hdus[i] = dada_hdu_create (pwcm->log);
    dada_hdu_set_key(pwcm->hdus[i], dada_keys[i]);
    if (dada_hdu_connect (pwcm->hdus[i]) < 0)
      return EXIT_FAILURE;

    if (dada_hdu_lock_write (pwcm->hdus[i]) < 0)
      return EXIT_FAILURE;
  }

  /* we are controlled by PWC control interface */
  if (pwcc_control) {

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

    if (dada_pwc_main_multi (pwcm) < 0) {
      fprintf (stderr, "gmrt_udpdb: error in PWC main loop\n");
      return EXIT_FAILURE;
    }

    for (i=0; i<num_hdus; i++)
    {
      if (dada_hdu_unlock_write (pwcm->hdus[i]) < 0)
        return EXIT_FAILURE;

      if (dada_hdu_disconnect (pwcm->hdus[i]) < 0)
        return EXIT_FAILURE;
    }

    if (verbose) 
      fprintf (stdout, "gmrt_udpdb: destroying pwc\n");
    dada_pwc_destroy (pwcm->pwc);

    if (verbose) 
      fprintf (stdout, "gmrt_udpdb: destroying pwc main\n");
    dada_pwc_main_multi_destroy (pwcm);


  /* If a custom header argument was supplied */
  } else {

    /* statistics thread */
    pthread_t stats_thread_id;

    if (verbose)
      multilog(log, LOG_INFO, "starting stats_thread()\n");
    if (pthread_create (&stats_thread_id, 0, (void *) stats_thread, (void *) &receiver) < 0) 
    {
      perror ("Error creating new thread");
      return -1;
    }

    char *   header_buf = 0;
    uint64_t header_size = 0;
    char *   buffer = 0;
    unsigned buffer_size = 64;

    /* get the next header */
    for (i=0; i<num_hdus; i++)
    {
      header_size = ipcbuf_get_bufsz (pwcm->hdus[i]->header_block);
      pwcm->hdus[i]->header = ipcbuf_get_next_write (pwcm->hdus[i]->header_block);

      if (!pwcm->hdus[i]->header)  {
        multilog (pwcm->log, LOG_ERR, "could not get next header block\n");
        return EXIT_FAILURE;
      }
    }

    /* allocate some memory for the header */
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
  
    /* HACK just set the UTC_START to roughly now */
    time_t utc = time(0);
    strftime (buffer, buffer_size, DADA_TIMESTR, gmtime(&utc));
    if (ascii_header_set (header_buf, "UTC_START", "%s", buffer) < 0) {
      multilog (pwcm->log, LOG_ERR, "failed ascii_header_set UTC_START\n");
      return EXIT_FAILURE;
    }
    multilog(pwcm->log, LOG_INFO, "Using current time for UTC_START = %s\n", buffer);

    /* copy the header_buf to the header block, setting ANTENNA_ID */
    for (i=0; i<num_hdus; i++)
    {
      if (ascii_header_set (header_buf, "ANTENNA_ID", "%d", i) < 0) {
        multilog (pwcm->log, LOG_ERR, "failed ascii_header_set ANTENNA_ID\n");
        return EXIT_FAILURE;
      }
      memcpy(pwcm->hdus[i]->header, header_buf, header_size);
    }

    /* note that the start function will set the OBS_OFFSET & RECV_HOST */
    utc = gmrt_udpdb_multi_start_function(pwcm, 0);
    if (utc == -1 ) {
      multilog(pwcm->log, LOG_ERR, "Could not run start function\n");
      return EXIT_FAILURE;
    } 

    /* marked the header as filled */
    for (i=0; i<num_hdus; i++)
    {
      if (ipcbuf_mark_filled (pwcm->hdus[i]->header_block, header_size) < 0)  {
         multilog (pwcm->log, LOG_ERR, "Could not mark filled header block\n");
        return EXIT_FAILURE;
      }
    }

    multilog (pwcm->log, LOG_INFO, "header marked filled\n");

    int64_t bytes_to_acquire = 1;
    uint64_t first_byte = 0;

    if (nsecs) 
      bytes_to_acquire = 400 * 1000 * 1000 * (int64_t) nsecs;

    if (verbose)
      fprintf(stderr, "bytes_to_acquire = %"PRIu64", nsecs=%d\n", bytes_to_acquire, nsecs);

    uint64_t bsize = 0;

    void ** buffers = 0;

    while (bytes_to_acquire > 0) {

      bsize = (UDP_DATA * UDP_NPACKS);

      buffers = gmrt_udpdb_multi_buffer_function(pwcm, &bsize);

      /* if some data was captured */
      if (bsize) {

        /* write data to datablock */
        for (i=0; i<num_hdus; i++) 
        {
          src = (char *) buffers[i];
          if ( ipcio_write(pwcm->hdus[i]->data_block, src, bsize) < bsize ) {
            multilog(pwcm->log, LOG_ERR, "Cannot write requested bytes to SHM\n");
            bytes_to_acquire = -1;
          }
        }
        if (nsecs)
          bytes_to_acquire -= bsize;

      /* the buffer function returned 0, stop */
      } else {

        multilog(pwcm->log, LOG_INFO, "main: buffer_function returned 0 bytes\n");
        bytes_to_acquire = -1;

      }
    }

    if ( gmrt_udpdb_multi_stop_function(pwcm) != 0)
      fprintf(stderr, "Error stopping acquisition");

    for (i=0; i<num_hdus; i++)
    {

      if (dada_hdu_unlock_write (pwcm->hdus[i]) < 0)
        return EXIT_FAILURE;

      if (dada_hdu_disconnect (pwcm->hdus[i]) < 0)
        return EXIT_FAILURE;
    }

    /* join threads */
    quit_threads = 1;
    void* result = 0;
    pthread_join (stats_thread_id, &result);

  }

  gmrt_receiver_dealloc(&receiver);

  return EXIT_SUCCESS;

}

/*
 * Thread to update the delay via gmrt_delay library fns 
 */

void delay_thread(void * arg) 
{

  gmrt_udpdb_multi_t * ctx = (gmrt_udpdb_multi_t *) arg;

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
  uint64_t busy_waits_total = 0;
  uint64_t busy_waits_this_sec = 0;
  double   mb_received_ps = 0;
  double   mb_dropped_ps = 0;

  struct timespec ts;

  while (!quit_threads)
  {
    bytes_received_this_sec = ctx->bytes->received - bytes_received_total;
    bytes_dropped_this_sec  = ctx->bytes->dropped - bytes_dropped_total;
    busy_waits_this_sec     = ctx->busy_waits - busy_waits_total;

    bytes_received_total = ctx->bytes->received;
    bytes_dropped_total  = ctx->bytes->dropped;
    busy_waits_total     = ctx->busy_waits;

    mb_received_ps = (double) bytes_received_this_sec / 1000000;
    mb_dropped_ps = (double) bytes_dropped_this_sec / 1000000;

    multilog(ctx->log, LOG_INFO, "T=%.1f, R=%.1f, D=%.1f MiB/s bsleeps=%"PRIu64" dropped=%"PRIu64"\n", (mb_received_ps+mb_dropped_ps), mb_received_ps, mb_dropped_ps, busy_waits_this_sec, ctx->bytes->dropped);

    sleep(1);
  }
}
