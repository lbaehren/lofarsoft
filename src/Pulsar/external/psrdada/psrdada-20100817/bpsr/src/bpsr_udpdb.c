/***************************************************************************
 *  
 *    Copyright (C) 2010 by Andrew Jameson
 *    Licensed under the Academic Free License version 2.1
 * 
 ****************************************************************************/

/***************************************************************************
 *
 * bpsr_udpdb
 * 
 * Primary Write Client for BPSR backend
 *
 ****************************************************************************/

// #define _DEBUG 1
//
#include <math.h>

#include "bpsr_def.h"
#include "bpsr_udpdb.h"

void usage()
{
  fprintf (stdout,
	   "bpsr_udpdb [options]\n"
     " -h             print help text\n"
     " -i interface   ip/interface for inc. UDP packets [default all]\n"
	   " -p port        port on which to listen [default %d]\n"
	   " -d             run as daemon\n"
	   " -k             hexidecimal shared memor key [default %x]\n"
     " -m mode        1 for independent mode, 2 for controlled mode\n"
     " -v             verbose messages\n"
     " -1             one time only mode, exit after EOD is written\n"
     " -H filename    ascii header information in file\n"
     " -S num         file size in bytes\n"
     " -l port        multilog port to write logging output to [default %d]\n"
     " -c port        port to received pwcc commands on [default %d]\n",
     BPSR_DEFAULT_UDPDB_PORT, DADA_DEFAULT_BLOCK_KEY,
     BPSR_DEFAULT_PWC_LOGPORT, BPSR_DEFAULT_PWC_PORT);
}


/* Determine if the header is valid. Returns 1 if valid, 0 otherwise */
int udpdb_header_valid_function (dada_pwc_main_t* pwcm) {

  int utc_size = 64;
  char utc_buffer[utc_size];
  int valid = 1;

  /* Check if the UTC_START is set in the header*/
  if (ascii_header_get (pwcm->header, "UTC_START", "%s", utc_buffer) < 0) {
    valid = 0;
  }

  /* Check whether the UTC_START is set to UNKNOWN */
  if (strcmp(utc_buffer,"UNKNOWN") == 0)
    valid = 0;

#ifdef _DEBUG
  multilog(pwcm->log, LOG_INFO, "Checking if header is valid: %d\n", valid);
#endif

  return valid;

}

/*
 * Error function
 */
int udpdb_error_function (dada_pwc_main_t* pwcm) {

  udpdb_t *udpdb = (udpdb_t*)pwcm->context;

  /* If UTC_START has been received, the buffer function 
   * should be returning data */
  if (udpdb_header_valid_function (pwcm) == 1) {

    udpdb->error_seconds--;
    //multilog (pwcm->log, LOG_WARNING, "Error Function has %"PRIu64" error "
    //          "seconds remaining\n",udpdb->error_seconds);

    if (udpdb->error_seconds <= 0)
      return 2;
    else
      return 1;

  } else {
    return 0;
  }

}



time_t udpdb_start_function (dada_pwc_main_t* pwcm, time_t start_utc)
{

  /* get our context, contains all required params */
  udpdb_t* udpdb = (udpdb_t*)pwcm->context;
  
  multilog_t* log = pwcm->log;

  /* Initialise variables */
  udpdb->packets->received = 0;
  udpdb->packets->dropped = 0;
  udpdb->packets->received_per_sec = 0;
  udpdb->packets->dropped_per_sec = 0;

  udpdb->bytes->received = 0;
  udpdb->bytes->dropped = 0;
  udpdb->bytes->received_per_sec = 0;
  udpdb->bytes->dropped_per_sec = 0;

  udpdb->error_seconds = 5;
  udpdb->packet_in_buffer = 0;
  udpdb->prev_time = time(0);
  udpdb->current_time = udpdb->prev_time;
  udpdb->curr_buffer_count = 0;
  udpdb->next_buffer_count = 0;
  
  /* Create a udp socket */
  udpdb->fd = bpsr_create_udp_socket(log, udpdb->interface, udpdb->port, udpdb->verbose);
  if (udpdb->fd < 0) {
    multilog (log, LOG_ERR, "Error, Failed to create udp socket\n");
    return 0; // n.b. this is an error value 
  }

  /* Get the accumulation length from the header */
  if (ascii_header_get (pwcm->header, "ACC_LEN", "%d", &(udpdb->acc_len)) != 1) {
    multilog (log, LOG_WARNING, "Warning. ACC_LEN not set in header, Using "
              "%d as default\n", BPSR_DEFAULT_ACC_LEN);
    ascii_header_set (pwcm->header, "ACC_LEN", "%d", BPSR_DEFAULT_ACC_LEN);
    udpdb->acc_len = BPSR_DEFAULT_ACC_LEN;
  }

  udpdb->sequence_incr = 512 * udpdb->acc_len;
  udpdb->spectra_per_second = (BPSR_IBOB_CLOCK * 1000000 / udpdb->acc_len ) / BPSR_IBOB_NCHANNELS;
  udpdb->bytes_per_second = udpdb->spectra_per_second * BPSR_UDP_DATASIZE_BYTES;

  /* Check the whether the requried header fields have been set via the 
   * control interface. This is required when receiving data from the DFB3
   * board, but not required for data transfer */

  /* Set the current machines name in the header block as RECV_HOST */
  char myhostname[HOST_NAME_MAX] = "unknown";;
  gethostname(myhostname,HOST_NAME_MAX); 
  ascii_header_set (pwcm->header, "RECV_HOST", "%s", myhostname);
  
  /* setup the data buffer to be 32 MB */
  udpdb->datasize = BPSR_UDP_DATASIZE_BYTES * BPSR_NUM_UDP_PACKETS;
  udpdb->curr_buffer = (char *) malloc(sizeof(char) * udpdb->datasize);
  assert(udpdb->curr_buffer != 0);
  udpdb->next_buffer = (char *) malloc(sizeof(char) * udpdb->datasize);
  assert(udpdb->next_buffer != 0);

  /* 0 both the curr and next buffers */
  char zerodchar = 'c';
  memset(&zerodchar,0,sizeof(zerodchar));
  memset(udpdb->curr_buffer,zerodchar,udpdb->datasize);
  memset(udpdb->next_buffer,zerodchar,udpdb->datasize);

  /* Setup the socket buffer for receiveing UDP packets */
  udpdb->socket_buffer= (char *) malloc(sizeof(char) * BPSR_UDP_PAYLOAD_BYTES);
  assert(udpdb->socket_buffer != 0);

  /* setup the expected sequence no to the initial value */
  udpdb->expected_sequence_no = 0;
  udpdb->prev_seq = 0;

  time_t utc = 0;
  return utc;
}

void* udpdb_buffer_function (dada_pwc_main_t* pwcm, uint64_t* size)
{
  
  /* get our context, contains all required params */
  udpdb_t* udpdb = (udpdb_t*)pwcm->context;

  /* logger */
  multilog_t* log = pwcm->log;

  // A char with all bits set to 0
   char zerodchar = 'c'; 
   memset(&zerodchar,0,sizeof(zerodchar));

  /* Flag to drop out of for loop */
  int quit = 0;

  /* Flag for timeout */
  int timeout_ocurred = 0;

  /* How much data has actaully been received */
  uint64_t data_received = 0;

  /* Sequence number of current packet */
  int ignore_packet = 0;

  /* For select polling */
  struct timeval timeout;
  fd_set *rdsp = NULL;
  fd_set readset;

  uint64_t prevnum = udpdb->curr_sequence_no;

  /* Switch the next and current buffers and their respective counters */
  char *tmp;
  tmp = udpdb->curr_buffer;
  udpdb->curr_buffer = udpdb->next_buffer;
  udpdb->next_buffer = tmp;
  udpdb->curr_buffer_count = udpdb->next_buffer_count;
  udpdb->next_buffer_count = 0;

  /* 0 the next buffer */
  // memset(udpdb->next_buffer, zerodchar, udpdb->datasize);

  /* Determine the sequence number boundaries for curr and next buffers */
  udpdb->min_sequence = udpdb->expected_sequence_no;
  udpdb->mid_sequence = udpdb->min_sequence + BPSR_NUM_UDP_PACKETS;
  udpdb->max_sequence = udpdb->mid_sequence + BPSR_NUM_UDP_PACKETS;

  //multilog(log, LOG_INFO, "[%"PRIu64" <-> %"PRIu64" <-> %"PRIu64"]\n",
  //         udpdb->min_sequence,udpdb->mid_sequence,udpdb->max_sequence);

  /* Assume we will be able to return a full buffer */
  *size = udpdb->datasize;
  int64_t max_ignore = udpdb->datasize;

  //multilog (log, LOG_INFO, "Before while\n");

  /* Continue to receive packets */
  while (!quit) {

    /* If we had a packet in the socket)buffer a previous call to the 
     * buffer function*/
    if (udpdb->packet_in_buffer) {

      udpdb->packet_in_buffer = 0;
                                                                                
    /* Else try to get a fresh packet */
    } else {
      
      /* 1.0 second timeout for select() */            
      timeout.tv_sec=0;
      timeout.tv_usec=1000000;

      FD_ZERO (&readset);
      FD_SET (udpdb->fd, &readset);
      rdsp = &readset;

      if ( select((udpdb->fd+1),rdsp,NULL,NULL,&timeout) == 0 ) {
    
        if (pwcm->pwc->state == dada_pwc_recording)
        {
          multilog (log, LOG_WARNING, "UDP packet timeout: no packet "
                                      "received for 1 second\n");
        }
        quit = 1;
        udpdb->received = 0;
        timeout_ocurred = 1;

      } else {

        /* Get a packet from the socket */
        udpdb->received = recvfrom (udpdb->fd, udpdb->socket_buffer, BPSR_UDP_PAYLOAD_BYTES, 0, NULL, NULL);
        ignore_packet = 0;

      }
    }

    //multilog(log, LOG_INFO, "After packet\n");

    /* If we did get a packet within the timeout, or one was in the buffer */
    if (!quit) {

      /* Decode the packets apsr specific header */
      udpdb->curr_sequence_no = decode_header(udpdb->socket_buffer) / udpdb->sequence_incr;

      /* If we are waiting for the "first" packet */
      if ((udpdb->expected_sequence_no == 0) && (data_received == 0)) {

#ifdef _DEBUG
        if ((udpdb->curr_sequence_no < (udpdb->prev_seq - 10)) && (udpdb->prev_seq != 0)) {
          multilog(log, LOG_INFO, "packet num reset from %"PRIu64" to %"PRIu64"\n", udpdb->prev_seq, udpdb->curr_sequence_no);
        }
#endif
        udpdb->prev_seq = udpdb->curr_sequence_no;
 
        /* Accept a "restart" which occurs within a 5 second window */ 
        if (udpdb->curr_sequence_no < (udpdb->spectra_per_second*5)) {

#ifdef _DEBUG
          multilog (log, LOG_INFO, "Start detected on packet %"PRIu64"\n",udpdb->curr_sequence_no);
#endif

          /* Increment the buffer counts with the missed packets */
          if (udpdb->curr_sequence_no < udpdb->mid_sequence)
            udpdb->curr_buffer_count += udpdb->curr_sequence_no;
          else {
            udpdb->curr_buffer_count += BPSR_NUM_UDP_PACKETS;
            udpdb->next_buffer_count += udpdb->curr_sequence_no - udpdb->mid_sequence;
          }

        } else {

          ignore_packet = 1;
        }

        if (udpdb->received != BPSR_UDP_PAYLOAD_BYTES) {
          multilog (log, LOG_ERR, "UDP packet size was incorrect (%"PRIu64" != %d)\n", udpdb->received, BPSR_UDP_PAYLOAD_BYTES);
          *size = DADA_ERROR_HARD;
          break;
        }
      }

      /* If we are still waiting dor the start of data */
      if (ignore_packet) {

        max_ignore -= BPSR_UDP_PAYLOAD_BYTES;
        if (max_ignore < 0) 
          quit = 1;

      } else {

        /* If the packet we received was too small, pad it */
        if (udpdb->received < BPSR_UDP_PAYLOAD_BYTES) {
        
          uint64_t amount_to_pad = BPSR_UDP_PAYLOAD_BYTES - udpdb->received;
          char * buffer_pos = (udpdb->socket_buffer) + udpdb->received;
 
          /* 0 the next buffer */
          memset(buffer_pos, zerodchar, amount_to_pad);

          multilog (log, LOG_WARNING, "Short packet received, padded %"PRIu64
                                      " bytes\n", amount_to_pad);
        } 

        /* If the packet we received was too long, warn about it */
        if (udpdb->received > BPSR_UDP_PAYLOAD_BYTES) {

          multilog (log, LOG_WARNING, "Long packet received, truncated to %"PRIu64
                                      " bytes\n", BPSR_UDP_DATASIZE_BYTES);
        }

        /* Now try to slot the packet into the appropraite buffer */
        data_received += BPSR_UDP_DATASIZE_BYTES;

        /* Increment statistics */
        udpdb->packets->received++;
        udpdb->packets->received_per_sec++;
        udpdb->bytes->received += BPSR_UDP_DATASIZE_BYTES;
        udpdb->bytes->received_per_sec += BPSR_UDP_DATASIZE_BYTES;

        /* If the packet belongs in the curr_buffer */
        if ((udpdb->curr_sequence_no >= udpdb->min_sequence) && 
            (udpdb->curr_sequence_no <  udpdb->mid_sequence)) {

          uint64_t buf_offset = (udpdb->curr_sequence_no - udpdb->min_sequence) * BPSR_UDP_DATASIZE_BYTES;

          memcpy( (udpdb->curr_buffer)+buf_offset, 
                  (udpdb->socket_buffer)+BPSR_UDP_COUNTER_BYTES,
                  BPSR_UDP_DATASIZE_BYTES);

          udpdb->curr_buffer_count++;

        } else if ((udpdb->curr_sequence_no >= udpdb->mid_sequence) && 
                   (udpdb->curr_sequence_no <  udpdb->max_sequence)) {

          uint64_t buf_offset = (udpdb->curr_sequence_no - udpdb->mid_sequence) * BPSR_UDP_DATASIZE_BYTES;

          memcpy( (udpdb->curr_buffer)+buf_offset, 
                  (udpdb->socket_buffer)+BPSR_UDP_COUNTER_BYTES,
                  BPSR_UDP_DATASIZE_BYTES);

          udpdb->next_buffer_count++;

        /* If this packet has arrived too late, it has already missed out */
        } else if (udpdb->curr_sequence_no < udpdb->min_sequence) {
 
          multilog (log, LOG_WARNING, "Packet arrived too soon, %"PRIu64" < %"PRIu64"\n",
                    udpdb->curr_sequence_no, udpdb->min_sequence);

        /* If a packet has arrived too soon, then we give up trying to fill the 
           curr_buffer and return what we do have */
        } else if (udpdb->curr_sequence_no >= udpdb->max_sequence) {

          float curr_percent = ((float) udpdb->curr_buffer_count / (float) BPSR_NUM_UDP_PACKETS)*100;
          float next_percent = ((float) udpdb->next_buffer_count / (float) BPSR_NUM_UDP_PACKETS)*100;
          //multilog (log, LOG_WARNING, "%"PRIu64" > %"PRIu64"\n",udpdb->curr_sequence_no,udpdb->max_sequence);
  
          multilog (log, LOG_WARNING, "Not keeping up. curr_buffer %5.2f%, next_buffer %5.2f%\n",
                                      curr_percent, next_percent);

          udpdb->packet_in_buffer = 1;
          quit = 1;

        } else {

          fprintf (stderr,"Sequence number invalid\n");

        }


        /* If we have filled the current buffer, then we can stop */
        if (udpdb->curr_buffer_count == BPSR_NUM_UDP_PACKETS) {
          quit = 1;
        } else {
          assert(udpdb->curr_buffer_count < BPSR_NUM_UDP_PACKETS);
        }

        /* If the next buffer is at least half full */
        if (udpdb->next_buffer_count > (BPSR_NUM_UDP_PACKETS / 2)) {
          float curr_percent = ((float) udpdb->curr_buffer_count / (float) BPSR_NUM_UDP_PACKETS)*100;
          float next_percent = ((float) udpdb->next_buffer_count / (float) BPSR_NUM_UDP_PACKETS)*100;

          multilog(log, LOG_WARNING, "Bailing curr_buf %5.2f%, next_buffer %5.2f%\n",curr_percent,next_percent);
          quit = 1;
        }
      }
    } 
  }

   /*multilog (log, LOG_INFO, "curr: %"PRIu64", next: %"PRIu64", capacity: %"PRIu64"\n",udpdb->curr_buffer_count, udpdb->next_buffer_count, BPSR_NUM_UDP_PACKETS); */

  /* If we have received a packet during this function call */
  if (data_received) {

    /* If we have not received all the packets we expected */
    if ((udpdb->curr_buffer_count < BPSR_NUM_UDP_PACKETS) && (!timeout_ocurred)) {

      multilog (log, LOG_WARNING, "Dropped %"PRIu64" packets\n",
               (BPSR_NUM_UDP_PACKETS - udpdb->curr_buffer_count));

      udpdb->packets->dropped += (BPSR_NUM_UDP_PACKETS - udpdb->curr_buffer_count);
      udpdb->packets->dropped_per_sec += (BPSR_NUM_UDP_PACKETS - udpdb->curr_buffer_count);
      udpdb->bytes->dropped += (BPSR_UDP_DATASIZE_BYTES * (BPSR_NUM_UDP_PACKETS - udpdb->curr_buffer_count));
      udpdb->bytes->dropped_per_sec += (BPSR_UDP_DATASIZE_BYTES * (BPSR_NUM_UDP_PACKETS - udpdb->curr_buffer_count));

    }

    /* If the timeout ocurred, this is most likely due to end of data */
    if (timeout_ocurred) {
      *size = udpdb->curr_buffer_count * BPSR_UDP_DATASIZE_BYTES;
      multilog (log, LOG_WARNING, "Suspected EOD received, returning "
                     "%"PRIu64" bytes\n",*size);
    }

    udpdb->expected_sequence_no += BPSR_NUM_UDP_PACKETS;

  } else {

    /* If we have received no data, then return a size of 0 */
    *size = 0;

  }

  udpdb->prev_time = udpdb->current_time;
  udpdb->current_time = time(0);
  
  if (udpdb->prev_time != udpdb->current_time) {

    if (udpdb->verbose) {

      if (ignore_packet) {

        multilog(log, LOG_INFO, "Ignoring out of range sequence no: %"PRIu64" > %"PRIu64"\n",udpdb->curr_sequence_no, (10 * udpdb->spectra_per_second));

      } else {

        multilog(log, LOG_INFO, "Packets [%"PRIu64",%"PRIu64"] [%"PRIu64",%"PRIu64"]\n",
                 udpdb->packets->dropped_per_sec,udpdb->packets->dropped,
                 udpdb->packets->received_per_sec,udpdb->packets->received);

        multilog(log, LOG_INFO, "Bytes   [%"PRIu64",%"PRIu64"] [%"PRIu64",%"PRIu64"]\n",
                 udpdb->bytes->dropped_per_sec,udpdb->bytes->dropped,
                 udpdb->bytes->received_per_sec,udpdb->bytes->received);
      }

    } else {
      /*
      multilog(log, LOG_INFO, "Packet loss %5.3f % / %5.3f %, Data loss %5.3f %\n", 
               (100 * ((float) udpdb->packets->dropped / (float) udpdb->packets->received)), 
               (100 * ((float) udpdb->packets->dropped_per_sec / (float) udpdb->packets->received_per_sec)), 
               (100 * ((float) udpdb->bytes->dropped / (float) udpdb->bytes->received)));
       */
    }

    udpdb->packets->received_per_sec = 0;
    udpdb->packets->dropped_per_sec = 0;
    udpdb->bytes->received_per_sec = 0;
    udpdb->bytes->dropped_per_sec = 0;

  }

  assert(udpdb->curr_buffer != 0);
  
  return (void *) udpdb->curr_buffer;

}


int udpdb_stop_function (dada_pwc_main_t* pwcm)
{

  /* get our context, contains all required params */
  udpdb_t* udpdb = (udpdb_t*)pwcm->context;
  int verbose = udpdb->verbose; /* saves some typing */

  float percent_dropped = 0;
  if (udpdb->expected_sequence_no) {
    percent_dropped = (float) ((double)udpdb->packets->dropped / (double)udpdb->expected_sequence_no) * 100;
  }

  if (udpdb->packets->dropped) 
  {
    multilog(pwcm->log, LOG_INFO, "packets dropped %"PRIu64" / %"PRIu64
             " = %8.6f %\n", udpdb->packets->dropped, 
             udpdb->expected_sequence_no, percent_dropped);
  }
  
  if (verbose) 
    fprintf(stderr,"Stopping udp transfer\n");

  close(udpdb->fd);
  free(udpdb->curr_buffer);
  free(udpdb->next_buffer);
  free(udpdb->socket_buffer);

  //TODO proper messaging and return values 
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

  /* port on which to listen for udp packets */
  int port = BPSR_DEFAULT_UDPDB_PORT;

  /* pwcc command port */
  int c_port = BPSR_DEFAULT_PWC_PORT;

  /* multilog output port */
  int l_port = BPSR_DEFAULT_PWC_LOGPORT;

  /* Flag set in daemon mode */
  char daemon = 0;

  /* Flag set in verbose mode */
  char verbose = 0;

  int arg = 0;

  /* mode flag */
  int mode = 0; 

  /* mode flag */
  int onetime = 0; 

  /* max length of header file */
  unsigned long fSize=800000000;

  /* size of the header buffer */
  uint64_t header_size = 0;

  /* hexadecimal shared memory key */
  key_t dada_key = DADA_DEFAULT_BLOCK_KEY;


  /* actual struct with info */
  udpdb_t udpdb;

  /* the filename from which the header will be read */
  char* header_file = 0;
  char* header_buf = 0;

  unsigned buffer_size = 64;
  static char* buffer = 0;
  char *src;

  while ((arg=getopt(argc,argv,"k:di:p:vm:S:H:n:1hc:l:")) != -1) {
    switch (arg) {

    case 'k':
      if (sscanf (optarg, "%x", &dada_key) != 1) {
        fprintf (stderr,"bpsr_udpdb: could not parse key from %s\n",optarg);
        return -1;
      }
      break;
      
    case 'd':
      daemon = 1;
      break;

    case 'i':
      if (optarg)
        interface = optarg;
      break;

    case 'p':
      port = atoi (optarg);
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

  log = multilog_open ("bpsr_udpdb", 0);

  if (daemon) 
    be_a_daemon ();
  else
    multilog_add (log, stderr);

  multilog_serve (log, l_port);
  
  if (verbose) 
    fprintf (stderr, "Creating dada pwc main\n");

  pwcm = dada_pwc_main_create();

  /* Setup the global pointer to point to pwcm for the signal handler to
   * reference */

  pwcm->log = log;
  pwcm->context = &udpdb;
  pwcm->start_function  = udpdb_start_function;
  pwcm->buffer_function = udpdb_buffer_function;
  pwcm->stop_function   = udpdb_stop_function;
  pwcm->error_function  = udpdb_error_function;
  pwcm->header_valid_function  = udpdb_header_valid_function;
  pwcm->verbose = verbose;

  /* init iBob config */
  udpdb.verbose = verbose;
  udpdb.interface = strdup(interface);
  udpdb.port = port;
  udpdb.acc_len = BPSR_DEFAULT_ACC_LEN;   // 25
  udpdb.mode = mode;
  udpdb.ibob_host = NULL;
  udpdb.ibob_port = 0;

  /* init stats structs */
  stats_t packets = {0,0,0,0};
  stats_t bytes = {0,0,0,0};

  udpdb.packets = &packets;
  udpdb.bytes = &bytes;

  udpdb.packet_in_buffer = 0;
  udpdb.received = 0;
  udpdb.prev_time = time(0);
  udpdb.current_time = udpdb.prev_time;
  udpdb.error_seconds = 5;
  //udpdb.statslog = multilog_open ("bpsr_udpdb_stats", 0);
  //multilog_serve (udpdb.statslog, BPSR_DEFAULT_UDPDB_STATS);
    
  /* Connect to shared memory */
  hdu = dada_hdu_create (pwcm->log);

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
    multilog (pwcm->log, LOG_INFO, "header block size = %llu\n", header_size);
    header_buf = ipcbuf_get_next_write (hdu->header_block);
    pwcm->header = header_buf;

    if (!header_buf)  {
      multilog (pwcm->log, LOG_ERR, "Could not get next header block\n");
      return EXIT_FAILURE;
    }

    // if header file is presented, use it. If not set command line attributes 
    if (header_file)  {

      if (verbose) fprintf(stderr,"read header file %s\n", header_file);
      if (fileread (header_file, header_buf, header_size) < 0)  {
        multilog (pwcm->log, LOG_ERR, "Could not read header from %s\n", 
                  header_file);
        return EXIT_FAILURE;
      }

      if (verbose) fprintf(stderr,"Retrieved header information\n");
      
    } else {

      if (verbose) fprintf(stderr,"Could not read header file\n");
      multilog(pwcm->log, LOG_ERR, "header file was not specified with -H\n");
      return EXIT_FAILURE;
    }

    // TODO Need to fill in the manual loop mode as we aren't being controlled
    
    if (!buffer)
      buffer = malloc (buffer_size);
    assert (buffer != 0);

    if (verbose) 
      fprintf(stderr, "running start function\n");
    time_t utc = udpdb_start_function(pwcm,0);

    if (utc == -1 ) {
      multilog(pwcm->log, LOG_ERR, "Could not run start function\n");
    } else 

    strftime (buffer, buffer_size, DADA_TIMESTR, gmtime(&utc));

    /* write UTC_START to the header */
    if (ascii_header_set (header_buf, "UTC_START", "%s", buffer) < 0) {
      multilog (pwcm->log, LOG_ERR, "failed ascii_header_set UTC_START\n");
      return -1;
    }

    multilog (pwcm->log, LOG_INFO, "UTC_START %s written to header\n", buffer);

    /* donot set header parameters anymore - acqn. doesn't start */
    if (ipcbuf_mark_filled (hdu->header_block, header_size) < 0)  {
      multilog (pwcm->log, LOG_ERR, "Could not mark filled header block\n");
      return EXIT_FAILURE;
    }

    multilog (pwcm->log, LOG_INFO, "header marked filled\n");

    while (1) {

      uint64_t bsize = udpdb.datasize;
      src = (char *) udpdb_buffer_function(pwcm, &bsize);

      /* write data to datablock */
      if (( ipcio_write(hdu->data_block, src, bsize) ) < bsize){
        multilog(pwcm->log, LOG_ERR, "Cannot write requested bytes to SHM\n");
        return EXIT_FAILURE;
      }
    }    

    if ( udpdb_stop_function(pwcm) != 0)
      fprintf(stderr, "Error stopping acquisition");

    if (dada_hdu_unlock_write (hdu) < 0)
      return EXIT_FAILURE;

    if (dada_hdu_disconnect (hdu) < 0)
      return EXIT_FAILURE;

  }
    /* we are controlled by PWC control interface */
  else {

    pwcm->header = hdu->header;

    if (verbose) fprintf (stderr, "Creating dada pwc control interface\n");
    pwcm->pwc = dada_pwc_create();

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

    if (dada_hdu_unlock_write (hdu) < 0)
      return EXIT_FAILURE;
  
    if (dada_hdu_disconnect (hdu) < 0)
      return EXIT_FAILURE;

    if (verbose) fprintf (stderr, "Destroying pwc\n");
    dada_pwc_destroy (pwcm->pwc);

    if (verbose) fprintf (stderr, "Destroying pwc main\n");
    dada_pwc_main_destroy (pwcm);

  }

  return EXIT_SUCCESS;

}


