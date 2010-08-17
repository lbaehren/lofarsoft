/***************************************************************************
 *  
 *    Copyright (C) 2009 by Andrew Jameson
 *    Licensed under the Academic Free License version 2.1
 * 
 ****************************************************************************/

/*
 * aspsr_udpdb : Primary Write Client for APSR Instrument
 */

#include <unistd.h>
#include <math.h>

#include "apsr_udp.h"
#include "apsr_def.h"
#include "apsr_udpdb.h"
//#include "sock.h"


void usage()
{
  fprintf (stdout,
	   "apsr_udpdb [options]\n"
     " -h             print help text\n"
     " -i interface   listen for packets on this interface [default any]\n"
	   " -p             port for UDP packets [default %d]\n"
	   " -d             run as daemon\n"
	   " -D logfile     run as daemon, redirecting output to logfile\n"
     " -m mode        1 for independent mode, 2 for controlled mode\n"
     " -k             hexidecimal shared memor key [default %x]\n"
     " -v             verbose messages\n"
     " -1             one time only mode, exit after EOD is written\n"
     " -H filename    ascii header information in file\n"
     " -l port        multilog port to write logging output to [default %d]\n"
     " -c port        port to received pwcc commands on [default %d]\n"
     " -s             launch stats thread to print to stats_log\n"
     " -S num         file size in bytes\n",APSR_DEFAULT_UDPDB_PORT,
                      DADA_DEFAULT_BLOCK_KEY, APSR_DEFAULT_PWC_LOGPORT, 
                      APSR_DEFAULT_PWC_PORT);
}

/* Initialize required data structures */
int apsr_udpdb_init(udpdb_t * ctx) 
{

  multilog_t * log = ctx->log;

  /* return 32 MB per call to buffer funnction */
  uint64_t data_size = 32*1024*1024;

  ctx->packets = init_stats_t();
  ctx->bytes   = init_stats_t();
  ctx->sock    = apsr_init_sock(UDPBUFFSIZE);
  ctx->curr    = apsr_init_data(data_size);
  ctx->next    = apsr_init_data(data_size);

  ctx->zerod_char = 'c';
  memset(&(ctx->zerod_char), 0, sizeof(ctx->zerod_char));

  assert(ctx->curr->buffer != 0);
  assert(ctx->next->buffer != 0);

  return 0;  

}


/*
 * Error function
 */
int udpdb_error_function (dada_pwc_main_t* pwcm) {

  udpdb_t *udpdb = (udpdb_t*)pwcm->context;

  /* If we are still waiting for that first packet, we don't
   * really have an error state :), but the buffer function
   * will return 0 bytes */
  if (udpdb->next_seq > 0) {
    udpdb->error_seconds--;
    multilog (pwcm->log, LOG_WARNING, "Error Function has %"PRIu64" error "
              "seconds remaining\n",udpdb->error_seconds);
  }

  if (udpdb->error_seconds <= 0) 
    return 2;
  else
    return 1;
}

/* Determine if the header is valid. Returns 1 if valid, 0 otherwise */
int udpdb_header_valid_function (dada_pwc_main_t* pwcm) {

  int utc_size = 1024;
  char utc_buffer[utc_size];
  int resolution;
  int valid = 1;

  /* Check if the UTC_START is set in the header*/
  if (ascii_header_get (pwcm->header, "UTC_START", "%s", utc_buffer) < 0) {
    valid = 0;
  }

  /* Check whether the UTC_START is set to UNKNOWN */
  if (strcmp(utc_buffer,"UNKNOWN") == 0)
    valid = 0;

  /* Check whether the RESOLUTION is known */
  if (ascii_header_get (pwcm->header, "RESOLUTION", "%d", &resolution) != 1) {
    valid = 0;
  }

  return valid;

}


time_t udpdb_start_function (dada_pwc_main_t* pwcm, time_t start_utc)
{

  udpdb_t* ctx = (udpdb_t*) pwcm->context;
  
  multilog_t* log = pwcm->log;

  apsr_zero_data(ctx->curr);
  apsr_zero_data(ctx->next);

  reset_stats_t(ctx->packets);
  reset_stats_t(ctx->bytes);

  /* open the socket for UDP data */
  ctx->sock->fd = dada_udp_sock_in(ctx->log, ctx->interface, ctx->port, ctx->verbose);
  if (ctx->sock->fd < 0)
  {
    multilog (log, LOG_ERR, "failed to create udp socket %s:%d\n", ctx->interface, ctx->port);
    return -1;
  }

  /* set the socket size to 64 MB */
  dada_udp_sock_set_buffer_size (log, ctx->sock->fd, ctx->verbose, 67108864);

  /* set the socket to non-blocking */
  sock_nonblock(ctx->sock->fd);

  /* Initialise variables */
  ctx->error_seconds = 10;
  ctx->sock->have_packet = 0;
  ctx->got_enough = 0;
  ctx->packets_late_this_sec = 0;
  
  /* Check the whether the requried header fields have been set via the 
   * control interface. This is required when receiving data from the DFB3
   * board, but not required for data transfer */
  if (ascii_header_get (pwcm->header, "NBAND", "%d", 
                        &(ctx->expected_nbands)) != 1) {
    multilog (log, LOG_WARNING, "Warning. NBAND not set in header, "
                                "Using 1 as default\n");
    ascii_header_set (pwcm->header, "NBAND", "%d", 1);
  }
                                                                                
  if (ascii_header_get (pwcm->header, "NBIT", "%d",
                        &(ctx->expected_header_bits)) != 1) {
    multilog (log, LOG_WARNING, "Warning. NBIT not set in header. "
                                "Using 8 as default.\n");
    ascii_header_set (pwcm->header, "NBIT", "%d", 8);
  }
                                                                                
  if (ascii_header_get (pwcm->header, "NCHAN", "%d",
                       &(ctx->expected_nchannels)) != 1) {
    multilog (log, LOG_WARNING, "Warning. NCHAN not set in header "
                               "Using 1 as default.\n");
    ascii_header_set (pwcm->header, "NCHAN", "%d", 2);
  }

  ctx->bps = 0;
  if (ascii_header_get (pwcm->header, "BYTES_PER_SECOND", "%d", &(ctx->bps)) != 1) {
    multilog (log, LOG_WARNING, "Warning. BYTES_PER_SECOND not set in header. "
                                "Using 64000000 as default.\n");
    ctx->bps = 64000000;
  } 

  /* Set the current machines name in the header block as RECV_HOST */
  char myhostname[HOST_NAME_MAX] = "unknown";
  gethostname(myhostname,HOST_NAME_MAX); 
  ascii_header_set (pwcm->header, "RECV_HOST", "%s", myhostname);
  
  /* setup the expected sequence no to the initial value */
  ctx->next_seq = 0;

  /* set the packet_length to an expected value */
  ctx->packet_length = UDPBUFFSIZE - UDPHEADERSIZE;
  ctx->payload_length = UDPBUFFSIZE; 

  ctx->packets_per_buffer = ctx->curr->size / ctx->packet_length;

  /* number of recvfrom calls in 1 second */
  ctx->timer_max = (ctx->bps / ctx->packet_length) / 10;

  /* Since udpdb cannot know the time sample of the first 
   * packet we always return 0 */
  
  return 0;
}

void* udpdb_buffer_function (dada_pwc_main_t* pwcm, uint64_t* size)
{
  
  /* get our context, contains all required params */
  udpdb_t* ctx = (udpdb_t*)pwcm->context;

  /* logger */
  multilog_t* log = pwcm->log;

  header_struct header = { '0', '0', 0, '0', '0', '0',"0000", 0 };

  int errsv = 0;
  int offset = 0;

  ctx->got_enough = 0;

  /* For select polling */
  struct timeval timeout;

  /* switch data buffer pointers */
  ctx->temp = ctx->curr;
  ctx->curr = ctx->next;
  ctx->next = ctx->temp;

  /* update the counters for the next buffer */
  ctx->next->count = 0;
  ctx->next->min = ctx->curr->max + 1;
  ctx->next->max = ctx->next->min + ctx->packets_per_buffer - 1;

  //if (ctx->verbose) 
  //  multilog(log, LOG_INFO, "curr[%"PRIu64" - %"PRIu64"] count=%"PRIu64",  next[%"PRIu64" - %"PRIu64"] count=%"PRIu64"\n", ctx->curr->min, ctx->curr->max, ctx->curr->count, ctx->next->min, ctx->next->max, ctx->next->count);

  /* zero the next buffer */
  apsr_zero_data(ctx->next);

  /* Assume we will be able to return a full buffer */
  *size = ctx->packets_per_buffer * ctx->packet_length;

  /* continue to receive packets until a break condition ocurrs */
  while (!ctx->got_enough) {

     /* if there is a packet in the buffer from the previous call */
    if (ctx->sock->have_packet) {

      ctx->sock->have_packet = 0;

    /* get a fresh packet */
    } else {

      ctx->sock->have_packet = 0;
      ctx->timer_count = 0;

      while (!ctx->sock->have_packet && !ctx->got_enough) {

        /* get a packet from the (non-blocking) socket */
        ctx->sock->got = recvfrom (ctx->sock->fd, ctx->sock->buffer, ctx->payload_length, 0, NULL, NULL);

        /* if we got a packet */
        if (ctx->sock->got > 0) {
          ctx->sock->have_packet = 1;

        } else if (ctx->sock->got == -1) {

          errsv = errno;

          /* if no packet at the socket */
          if (errsv == EAGAIN) {

            /* if we have waited ~0.1 seconds */
            if (ctx->timer_count > ctx->timer_max) {
              ctx->got_enough = 1;

            /* sleep for a 10 us before retrying */
            } else {
              ctx->timer_count++;
              timeout.tv_sec = 0;
              timeout.tv_usec = 10;
              select(0, NULL, NULL, NULL, &timeout);
            }

          } else {
            multilog(log, LOG_ERR, "recvfrom failed: %s\n", strerror(errsv));
            ctx->got_enough = 1;
          }
        }
      }
    }

    /* if we have received a packet withing the timeout */
    if (!ctx->got_enough && ctx->sock->have_packet) {

      decode_header(ctx->sock->buffer, &header);

      /* When we have received the first packet */
      if ((ctx->next_seq == 0) && (ctx->bytes->received == 0)) {

        if (header.sequence != 0) {
          multilog (log, LOG_WARNING, "First packet received had sequence "
                                  "number %d\n",header.sequence);

          /* If the first packet is more than 1 gigabyte into the future,
           * then something is drastically wrong */
          if ((header.sequence * ctx->sock->got) > (1024*1024*1024)) {
            multilog (log, LOG_ERR, "First packet sequence was more than "
                                      "1 GB into the future. Stopping\n");
            *size = DADA_ERROR_HARD;
          }

        } else {
          multilog (log, LOG_INFO, "First packet has been received\n");
        }

        /* If the UDP packet thinks its header is different to the prescribed
         * size, give up */
        if (header.length != UDPHEADERSIZE) {
          multilog (log, LOG_ERR, "Custom UDP header length incorrect. "
                   "Expected %d bytes, received %d bytes\n",UDPHEADERSIZE,
                   header.length);
          *size = DADA_ERROR_HARD;
          break;
        }

        /* define length of all future packets */
        ctx->packet_length = ctx->sock->got - UDPHEADERSIZE;
        ctx->payload_length = ctx->sock->got;
        ctx->timer_max = (ctx->bps / ctx->packet_length) / 10;
        if (ctx->verbose)
          multilog(log, LOG_INFO, "bps=%d, timer_max=%"PRIu64"\n", ctx->bps, ctx->timer_max);

        /* Set the number of packets a buffer can hold */
        ctx->packets_per_buffer = ctx->curr->size / ctx->packet_length;

        if (ctx->verbose) 
          multilog(log, LOG_INFO, "got=%d bytes, header=%d bytes, packet_length=%"PRIu64" bytes"
                   ", udpdb->datasize=%"PRIu64", packets_per_buffer=%"PRIu64"\n",ctx->sock->got,
                   UDPHEADERSIZE, ctx->packet_length, ctx->curr->size, ctx->packets_per_buffer);

        /* Adjust sequence numbers, now that we know packet_length*/
        ctx->curr->min = ctx->next_seq;
        ctx->curr->max = ctx->curr->min + ctx->packets_per_buffer - 1;
        ctx->next->min = ctx->curr->max + 1;
        ctx->next->max = ctx->next->min + ctx->packets_per_buffer - 1;

        //multilog(log, LOG_INFO, "curr[%"PRIu64" - %"PRIu64"] count=%"PRIu64",  next[%"PRIu64" - %"PRIu64"] count=%"PRIu64"\n", ctx->curr->min, ctx->curr->max, ctx->curr->count, ctx->next->min, ctx->next->max, ctx->next->count);

        /* set the amount a data we expect to return */
        *size = ctx->packets_per_buffer * ctx->packet_length;

        /* We define the polarization length to be the number of bytes
           in the packet, and set RESOLUTION header parameter accordingly */
        header.pollength = (unsigned int) ctx->packet_length;

        /* Set the resoultion header variable */
        if ( ascii_header_set (pwcm->header, "RESOLUTION", "%d", header.pollength) < 0) {
                multilog (log, LOG_ERR, "Could not set RESOLUTION header parameter"
                    " to %d\n", header.pollength);
          *size = DADA_ERROR_HARD;
          break;
        }


      }

      /* If the packet we received was too small, pad it */
      if (ctx->sock->got < ctx->payload_length) {

        uint64_t amount_to_pad = ctx->payload_length - ctx->sock->got;
        memset(ctx->sock->buffer + ctx->sock->got, ctx->zerod_char, amount_to_pad);
        multilog (log, LOG_WARNING, "short packet received[%d], padded %"PRIu64
                                 " bytes\n", ctx->sock->got, amount_to_pad);
      }

      /* If the packet we received was too long, warn about it */
      if (ctx->sock->got > (ctx->packet_length + UDPHEADERSIZE)) {
        multilog (log, LOG_WARNING, "Long packet received, truncated to %"PRIu64
                                 " bytes\n", ctx->packet_length);
      }

      /* Increment statistics */
      ctx->bytes->received += (ctx->sock->got - UDPHEADERSIZE);
      ctx->bytes->received_per_sec += (ctx->sock->got - UDPHEADERSIZE);
      ctx->packets->received++;
      ctx->packets->received_per_sec++;
      
      /* lets process this packet now */
      ctx->sock->have_packet = 0;

      //fprintf(stderr, "%"PRIu64"\n", header.sequence);

      if (header.sequence < ctx->curr->min) {
        fprintf(stderr, "Packet underflow %"PRIu64" < min (%"PRIu64")\n", header.sequence, ctx->curr->min);
      
      } else if (header.sequence <= ctx->curr->max) {

        offset = (header.sequence - ctx->curr->min) * ctx->packet_length;
        memcpy (ctx->curr->buffer + offset, ctx->sock->buffer + UDPHEADERSIZE, ctx->packet_length);
        ctx->curr->count++;

      } else if (header.sequence <= ctx->next->max) {

        offset = (header.sequence - ctx->next->min) * ctx->packet_length;
        memcpy (ctx->next->buffer + offset, ctx->sock->buffer + UDPHEADERSIZE, ctx->packet_length);
        ctx->next->count++;

      /* we aren't keeping up, drop this loop */
      } else {
        
        multilog(log, LOG_WARNING, "dropping packets. curr %5.2f%, next %5.2f%\n",
                 ((float) ctx->curr->count / (float) ctx->packets_per_buffer)*100,
                 ((float) ctx->next->count / (float) ctx->packets_per_buffer)*100);
        ctx->sock->have_packet = 1;
        ctx->got_enough = 1;
    
      }


      /* if we have filled the current buffer, then we can stop */
      if (ctx->curr->count >= ctx->packets_per_buffer) {
        ctx->got_enough = 1;
      }

      /* if the next buffer is 25% full, then we can stop */
      if (ctx->next->count >= (ctx->packets_per_buffer * 0.50)) {
        ctx->got_enough = 1;
        if (ctx->verbose)
          multilog(log, LOG_WARNING, "next data buffer > 50%% full\n");
      }

    }
  } 

  if (*size < 0) {
    multilog(log, LOG_WARNING, "returning size=%"PRIu64" bytes\n", *size);
    return (void *) ctx->curr->buffer;
  }

  /* check for dropped packets */
  if (ctx->timer_count < ctx->timer_max) {

    /* we have dropped some packets without a timeout ocurring */
    if (ctx->curr->count < ctx->packets_per_buffer) {
      multilog (ctx->log, LOG_WARNING, "Dropped %"PRIu64" packets\n",
               (ctx->packets_per_buffer- ctx->curr->count));
      ctx->packets->dropped += (ctx->packets_per_buffer - ctx->curr->count);
      ctx->packets->dropped_per_sec += (ctx->packets_per_buffer - ctx->curr->count);
      ctx->bytes->dropped += (ctx->packet_length * (ctx->packets_per_buffer - ctx->curr->count));
      ctx->bytes->dropped_per_sec += (ctx->packet_length * (ctx->packets_per_buffer - ctx->curr->count));
    }

    *size = ctx->packets_per_buffer * ctx->packet_length;

    ctx->next_seq += ctx->packets_per_buffer;

  /* we have had a 1 second timeout */
  } else {

    *size = ctx->curr->count * ctx->packet_length;

    /* if some data has been received, then this is the EOD */
    if (ctx->curr->count) 
      multilog(ctx->log, LOG_INFO, "suspected EOD, returning %"PRIu64" bytes\n", *size);
    else
      if (ctx->verbose)
        multilog(ctx->log, LOG_INFO, "no data received this buffer_function\n");

  }

  assert(ctx->curr->buffer != 0);
  return (void *) ctx->curr->buffer;

}


int udpdb_stop_function (dada_pwc_main_t* pwcm)
{

  /* get our context, contains all required params */
  udpdb_t* ctx= (udpdb_t*) pwcm->context;

  float percent_dropped = 0;
  if (ctx->packets->received) {
    percent_dropped = (float) ((double) ctx->packets->dropped / (double) ctx->packets->received) * 100.00;
  }

  multilog(pwcm->log, LOG_INFO, "packets dropped %"PRIu64" / %"PRIu64" = %10.8f %\n",
           ctx->packets->dropped, ctx->packets->received, percent_dropped);
  
  if (ctx->verbose) 
    fprintf(stderr,"Stopping udp transfer\n");

  /* must close socket to ensure any packets at the interface are discarded */
  close (ctx->sock->fd);

  //TODO proper messaging and return values 
  return 0;

}

/* release resources used */
int apsr_udpdb_dealloc(udpdb_t * ctx)
{
  close(ctx->sock->fd);
  apsr_free_data(ctx->curr);
  apsr_free_data(ctx->next);
  apsr_free_sock(ctx->sock);
}

int main (int argc, char **argv)
{

  /* DADA Primary Write Client main loop  */

  dada_pwc_main_t* pwcm = 0;

  /* DADA Header plus Data Unit */
  dada_hdu_t* hdu = 0;

  /* DADA Logger */
  multilog_t* log = 0;
  
  /* port on which to listen for incoming connections */
  int port = APSR_DEFAULT_UDPDB_PORT;

  /* Flag set in daemon mode */
  char daemon = 0;

  /* Flag set in verbose mode */
  char verbose = 0;

  /* listen on a specific interface */
  char * interface = "any";

  int arg = 0;

  /* mode flag */
  int mode = 0; 

  /* mode flag */
  int onetime = 0; 

  /* max length of header file */
  unsigned long fSize=800000000;

  /* size of the header buffer */
  uint64_t header_size = 0;

  /* pwcc command port */
  int c_port = APSR_DEFAULT_PWC_PORT;

  /* multilog output port */
  int l_port = APSR_DEFAULT_PWC_LOGPORT;

  /* whether to launch the stats_thread or not */
  unsigned launch_stats = 0;

  /* hexadecimal shared memory key */
  key_t dada_key = DADA_DEFAULT_BLOCK_KEY;

  /* actual struct with info */
  udpdb_t udpdb;

  /* optional logfile */
  char* log_file = 0;

  /* the filename from which the header will be read */
  char* header_file = 0;
  char* header_buf = 0;

  unsigned buffer_size = 64;
  static char* buffer = 0;
  char *src;

  while ((arg=getopt(argc,argv,"c:dD:hH:i:k:l:m:n:p:sS:v1")) != -1) {
    switch (arg) {

    case 'k':
      if (sscanf (optarg, "%x", &dada_key) != 1) {
        fprintf (stderr,"apsr_udpdb: could not parse key from %s\n",optarg);
        return -1;
      }
      break;

      
    case 'd':
      daemon = 1;
      break;

    case 'D':
      daemon = 1;
      if (optarg) {
        log_file = optarg;
      } else {
        fprintf (stderr, "Specify a log_file\n");
        usage();
      }
      break;

    case 'i':
      if (optarg)
        interface = optarg;
      break;

    case 'p':
      port = atoi (optarg);
      break;

    case 's':
      launch_stats = 1;
      break;

    case '1':
      onetime = 1;
      break;

    case 'v':
      verbose=1;
      break;

    case 'm':
#ifdef _DEBUG
      fprintf(stderr,"Mode is %s \n",optarg);
#endif
      if (optarg) {
        mode = atoi (optarg);
        if (!(mode == 1)) {
          fprintf(stderr,"Specify a valid mode\n");
          usage();
        }
      }
      break;

    case 'S':
      if (optarg){
        fSize = atoi(optarg);
        fprintf(stderr,"File size will be %lu\n",fSize);
      }
      else usage();
      break;

    case 'H':
      if (optarg) {
        header_file = optarg;
      } else {
        fprintf(stderr,"Specify a header file\n");
        usage();
      }
      break;

    case 'c':
      if (optarg) {
        c_port = atoi(optarg);
        break;
      } else {
        usage();
        return EXIT_FAILURE;
      }

    case 'l':
      if (optarg) {
        l_port = atoi(optarg);
        break;
      } else {
        usage();
        return EXIT_FAILURE;
      }

    case 'h':
      usage();
      return 0;
      
    default:
      usage ();
      return 0;
      
    }
  }

  log = multilog_open ("apsr_udpdb", 0);

  if (daemon) {
    if (log_file) {
      multilog_add (log, stderr);
      be_a_daemon_with_log (log_file);
    } else
      be_a_daemon();
  } else
    multilog_add (log, stderr);

  multilog_serve (log, l_port);
  
  if (verbose) 
    fprintf (stderr, "creating dada pwc main\n");

  pwcm = dada_pwc_main_create();

  /* Setup the global pointer to point to pwcm for the signal handler to reference */
  pwcm->log = log;
  pwcm->context = &udpdb;
  pwcm->start_function  = udpdb_start_function;
  pwcm->buffer_function = udpdb_buffer_function;
  pwcm->stop_function   = udpdb_stop_function;
  pwcm->error_function  = udpdb_error_function;
  pwcm->header_valid_function  = udpdb_header_valid_function;
  pwcm->verbose = verbose;

  /* Setup context information */
  udpdb.verbose = verbose;
  udpdb.interface = strdup(interface);
  udpdb.port = port;
  udpdb.error_seconds = 10;
  udpdb.packet_length = 0;
  udpdb.payload_length = 0;
  udpdb.stats_log = multilog_open ("apsr_udpdb_stats", 0);
  udpdb.log = log;
  udpdb.quit_threads = 0;

  /* initialize requried memory and values */
  if (verbose) multilog(pwcm->log, LOG_INFO, "calling apsr_udpdb_init\n");
  if (apsr_udpdb_init(&udpdb) != 0) {
    multilog(pwcm->log, LOG_INFO, "Failed to initialise memory structures\n");
    return -1;
  }
  if (verbose) multilog(pwcm->log, LOG_INFO, "apsr_udpdb_init returned\n");

  /* launch the stats_thread */
  pthread_t stats_thread_id = 0;
  if (launch_stats) {
    multilog(pwcm->log, LOG_INFO, "starting stats_thread()\n");
    if (pthread_create (&stats_thread_id, 0, (void *) stats_thread, (void *) &udpdb) < 0) {
      perror ("Error creating new thread");
      return -1;
    }
  }

  multilog_serve (udpdb.stats_log, APSR_DEFAULT_UDPDB_STATS);
    
  /* Connect to shared memory */
  hdu = dada_hdu_create (pwcm->log);

  /* Set the data block shared memory key */
  dada_hdu_set_key(hdu, dada_key);

  if (dada_hdu_connect (hdu) < 0)
    return EXIT_FAILURE;

  if (dada_hdu_lock_write (hdu) < 0)
    return EXIT_FAILURE;

  pwcm->data_block = hdu->data_block;
  pwcm->header_block = hdu->header_block;

  // We now need to setup the header information. for testing we can
  // accept local header info. In practise we will get this from
  // the control interface...
  if (mode == 1) {

    header_size = ipcbuf_get_bufsz (hdu->header_block);
    header_buf = ipcbuf_get_next_write (hdu->header_block);
    pwcm->header = header_buf;

    if (!header_buf)  {
      multilog (pwcm->log, LOG_ERR, "Could not get next header block\n");
      return EXIT_FAILURE;
    }

    // if header file is presented, use it. If not set command line attributes 
    if (header_file)  {

      if (verbose) 
        multilog(pwcm->log, LOG_INFO, "read header file %s\n", header_file);

      if (fileread (header_file, header_buf, header_size) < 0)  {
        multilog (pwcm->log, LOG_ERR, "Could not read header from %s\n", 
                  header_file);
        return EXIT_FAILURE;
      }

      /* get our context, contains all required params */
      udpdb_t* udpdb = (udpdb_t*)pwcm->context;

      if (ascii_header_get (header_buf, "NBAND", "%d", 
                            &(udpdb->expected_nbands)) != 1) {
        multilog (pwcm->log, LOG_WARNING, "Warning. No NBAND in header, using "
                                          "1 as default\n");
        ascii_header_set (header_buf, "NBAND", "%d", 1);
      }

      if (ascii_header_get (header_buf, "NBIT", "%d", 
                            &(udpdb->expected_header_bits)) != 1) {
        multilog (pwcm->log, LOG_WARNING, "Warning. No NBIT in header, using "
                                          "8 as default\n");
        ascii_header_set (header_buf, "NBIT", "%d", 8);
      }

      if (ascii_header_get (header_buf, "NCHAN", "%d",
                            &(udpdb->expected_nchannels)) != 1) {
        multilog (pwcm->log, LOG_WARNING, "Warning. No NCHAN in header, "
                                          "using 1 as default\n");
        ascii_header_set (header_buf, "NCHAN", "%d", 1);
      }

    } 
    else {

      if (verbose) fprintf(stderr,"Could not read header file\n");
      multilog(pwcm->log, LOG_ERR, "header file was not specified with -H\n");
      return EXIT_FAILURE;
    }

    // TODO Need to fill in the manual loop mode as we aren't being controlled
    
    if (!buffer)
      buffer = malloc (buffer_size);
    assert (buffer != 0);

    if (verbose) 
      fprintf(stderr, "calling udpdb_start_function\n");

    time_t utc = udpdb_start_function(pwcm,0);

    if (verbose) 
      fprintf(stderr, "udpdb_start_function returned\n");

    if (utc == -1 ) {
      multilog(pwcm->log, LOG_ERR, "Could not run start function\n");
    }

    strftime (buffer, buffer_size, DADA_TIMESTR, gmtime(&utc));

    /* write UTC_START to the header */
    if (ascii_header_set (header_buf, "UTC_START", "%s", buffer) < 0) {
      multilog (pwcm->log, LOG_ERR, "failed ascii_header_set UTC_START\n");
      return -1;
    }

    /* donot set header parameters anymore - acqn. doesn't start */
    if (ipcbuf_mark_filled (hdu->header_block, header_size) < 0)  {
      multilog (pwcm->log, LOG_ERR, "Could not mark filled header block\n");
      return EXIT_FAILURE;
    }

    uint64_t total_received = 0;

    while (!udpdb.quit_threads) {

      uint64_t bsize = 0;
      src = (char *) udpdb_buffer_function(pwcm, &bsize);

      total_received += bsize;

      /* write data to datablock */
      if (( ipcio_write(hdu->data_block, src, bsize) ) < bsize){
        multilog(pwcm->log, LOG_ERR, "Cannot write requested bytes to SHM\n");
        return EXIT_FAILURE;
      }

      if (total_received && bsize == 0) 
        udpdb.quit_threads = 1;

    }    

    if (udpdb.verbose) 
      multilog(log, LOG_INFO, "received total of %"PRIu64" bytes\n", total_received);

    if ( udpdb_stop_function(pwcm) != 0)
      fprintf(stderr, "Error stopping acquisition");

  /* we are controlled by PWC control interface */
  } else {

    pwcm->header = hdu->header;

    if (verbose) fprintf (stderr, "Creating dada pwc control interface\n");
    pwcm->pwc = dada_pwc_create();

    /* Set the port on which control commands may be issued */
    pwcm->pwc->port = c_port;

    if (verbose) fprintf (stderr, "Creating dada server\n");
    if (dada_pwc_serve (pwcm->pwc) < 0) {
      fprintf (stderr, "dada_udpdb: could not start server\n");
      return EXIT_FAILURE;
    }
    if (verbose) fprintf (stderr, "Entering PWC main loop\n");
    if (dada_pwc_main (pwcm) < 0) {
      fprintf (stderr, "dada_udpdb: error in PWC main loop\n");
      return EXIT_FAILURE;
    }

  }

  if (launch_stats) {
    void* result = 0;
    fprintf(stderr, "joining stats_thread\n");
    pthread_join (stats_thread_id, &result);
  }

  apsr_udpdb_dealloc(&udpdb);

  if (dada_hdu_unlock_write (hdu) < 0)
      return EXIT_FAILURE;

  if (dada_hdu_disconnect (hdu) < 0)
    return EXIT_FAILURE;

  if (mode == 1) {
    if (verbose) fprintf (stderr, "Destroying pwc\n");
    dada_pwc_destroy (pwcm->pwc);

    if (verbose) fprintf (stderr, "Destroying pwc main\n");
    dada_pwc_main_destroy (pwcm);
  }

  return EXIT_SUCCESS;

}


void check_header(header_struct header, udpdb_t *udpdb, multilog_t* log) {

  if (header.bits != udpdb->expected_header_bits) {
    multilog (log, LOG_WARNING, "Packet %"PRIu64": expected %d "
              "nbits per sample, received %d\n", header.sequence, 
              udpdb->expected_header_bits, header.bits);
  }

  if (header.channels != udpdb->expected_nchannels) {
    multilog (log, LOG_WARNING, "Packet %"PRIu64": expected %d "
              "channels, received %d\n", header.sequence,
              udpdb->expected_nchannels, header.channels);
  }
                                                                            
  if (header.bands != udpdb->expected_nbands) {
    multilog (log, LOG_WARNING, "Packet %"PRIu64": expected %d "
              "bands, received %d\n", header.sequence, udpdb->expected_nbands,
              header.bands);
  }
                                                                            
  if (header.length != UDPHEADERSIZE) {
    multilog (log, LOG_WARNING, "Packet %"PRIu64": expected a header "
              "of length %d, received %d\n", header.sequence, UDPHEADERSIZE,
              header.length);
  }
}


/* 
 *  Thread to print simple statistics to the special log port
 */
void stats_thread (void * arg) {

  udpdb_t * ctx = (udpdb_t *) arg;

  uint64_t bytes_received_total = 0;
  uint64_t bytes_received_this_sec = 0;
  uint64_t bytes_dropped_total = 0;
  uint64_t bytes_dropped_this_sec = 0;
  double   mb_received_ps = 0;
  double   mb_dropped_ps = 0;

  while (!ctx->quit_threads)
  {
    bytes_received_this_sec = ctx->bytes->received - bytes_received_total;
    bytes_dropped_this_sec  = ctx->bytes->dropped - bytes_dropped_total;

    bytes_received_total = ctx->bytes->received;
    bytes_dropped_total = ctx->bytes->dropped;

    mb_received_ps = (double) bytes_received_this_sec / 1000000;
    mb_dropped_ps = (double) bytes_dropped_this_sec / 1000000;

    multilog(ctx->log, LOG_INFO, "T=%5.2f, R=%5.2f MB/s\t D=%5.2f MB/s "
             "packets=%"PRIu64" dropped=%"PRIu64"\n", (mb_received_ps+mb_dropped_ps), 
             mb_received_ps, mb_dropped_ps, ctx->packets->received, ctx->packets->dropped);
    sleep(1);
  }

}

