/*
 * bpsr_udpheader
 *
 * Simply listens on a specified port for udp packets encoded
 * in the PARSEPC format
 *
 */

#include "bpsr_udpheader.h"

#include <sys/socket.h>
#include "sock.h"
#include <math.h>

void usage()
{
  fprintf (stdout,
	   "bpsr_udpheader [options]\n"
     " -h             print help text\n"
	   " -i interface   ip/interface for inc. UDP packets [default all]\n"
	   " -p port        port on which to listen [default %d]\n"
     " -a acc_len     accumulation length of the iBob board [default %d]\n"
     " -v             verbose messages\n"
     " -V             very verbose messages\n", 
     BPSR_DEFAULT_UDPDB_PORT, BPSR_DEFAULT_ACC_LEN);
}


time_t udpheader_start_function (udpheader_t* udpheader, time_t start_utc)
{

  multilog_t* log = udpheader->log;

  /* Initialise variables */

  udpheader->select_sleep= 0;
  udpheader->packets->received = 0;
  udpheader->packets->dropped = 0;
  udpheader->packets->received_per_sec = 0;
  udpheader->packets->dropped_per_sec = 0;
                                                                                
  udpheader->bytes->received = 0;
  udpheader->bytes->dropped = 0;
  udpheader->bytes->received_per_sec = 0;
  udpheader->bytes->dropped_per_sec = 0;

  udpheader->packet_in_buffer = 0;
  udpheader->prev_time = time(0);
  udpheader->current_time = udpheader->prev_time;
  udpheader->curr_buffer_count = 0;
  udpheader->next_buffer_count = 0;
  udpheader->packets_late_this_sec = 0;


  udpheader->sequence_incr = 512 * udpheader->acc_len;
  udpheader->spectra_per_second = (BPSR_IBOB_CLOCK * 1000000 / udpheader->acc_len ) / BPSR_IBOB_NCHANNELS;
  udpheader->bytes_per_second = udpheader->spectra_per_second * BPSR_UDP_DATASIZE_BYTES;

  /* UDP SETUP */
  /* Create a udp socket */
  udpheader->udpfd = bpsr_create_udp_socket(log, udpheader->interface, udpheader->port, udpheader->verbose);
  if (udpheader->udpfd < 0) {
    multilog (log, LOG_ERR, "Error, Failed to create udp socket\n");
    return 0; // n.b. this is an error value
  }

  /* Set the current machines name in the header block as RECV_HOST */
  char myhostname[HOST_NAME_MAX] = "unknown";;
  gethostname(myhostname,HOST_NAME_MAX); 

  /* setup the data buffer for NUMUDPACKETS udp packets */
  udpheader->datasize = BPSR_UDP_PAYLOAD_BYTES * BPSR_NUM_UDP_PACKETS;
  udpheader->curr_buffer = (char *) malloc(sizeof(char) * udpheader->datasize);
  assert(udpheader->curr_buffer != 0);
  udpheader->next_buffer = (char *) malloc(sizeof(char) * udpheader->datasize);
  assert(udpheader->next_buffer != 0);

  if (udpheader->verbose == 2) 
    fprintf(stderr, "udpheader_start_function: setting internal buffers to %d bytes\n",
                    udpheader->datasize);

  /* 0 both the curr and next buffers */
  char zerodchar = 'c';
  memset(&zerodchar,0,sizeof(zerodchar));
  memset(udpheader->curr_buffer,zerodchar,udpheader->datasize);
  memset(udpheader->next_buffer,zerodchar,udpheader->datasize);

  /* Setup the socket buffer for receiveing UDP packets */
  udpheader->socket_buffer= (char *) malloc(sizeof(char) * BPSR_UDP_PAYLOAD_BYTES);
  assert(udpheader->socket_buffer != 0);

  /* setup the expected sequence no to the initial value */
  udpheader->expected_sequence_no = 0;

  /* set the packet_length to an expected value */
  udpheader->packet_length = BPSR_UDP_DATASIZE_BYTES;

  return 0;
}

void* udpheader_read_function (udpheader_t* udpheader, uint64_t* size)
{
  // A char with all bits set to 0
  char zerodchar = 'c'; 
  memset(&zerodchar,0,sizeof(zerodchar));

  /* Flag to drop out of for loop */
  int quit = 0;

  /* Flag for timeout */
  int timeout_ocurred = 0;

  /* How much data has actaully been received */
  uint64_t data_received = 0;

  /* For select polling */
  struct timeval timeout;
  fd_set *rdsp = NULL;
  fd_set readset;

  /* Switch the next and current buffers and their respective counters */
  char *tmp;
  tmp = udpheader->curr_buffer;
  udpheader->curr_buffer = udpheader->next_buffer;
  udpheader->next_buffer = tmp;
  udpheader->curr_buffer_count = udpheader->next_buffer_count;
  udpheader->next_buffer_count = 0;

  /* 0 the next buffer */
  memset(udpheader->next_buffer,zerodchar,udpheader->datasize);

  /* Determine the sequence number boundaries for curr and next buffers */
  uint64_t raw_sequence_no = 0;
  uint64_t sequence_no = 0;
  uint64_t prevnum = 0;

  /* Assume we will be able to return a full buffer */
  *size = udpheader->datasize;

  /* Continue to receive packets */
  while (!quit) {

    /* select to see if data has arrive. */
    timeout.tv_sec=0;
    timeout.tv_usec=500000;

    FD_ZERO (&readset);
    FD_SET (udpheader->udpfd, &readset);
    rdsp = &readset;

    if ( select((udpheader->udpfd+1),rdsp,NULL,NULL,&timeout) == 0 ) {

      if ((udpheader->state == RECORDING) && (udpheader->expected_sequence_no != 0)) {
        fprintf(stderr, "Warning: UDP packet timeout: no packet "
                          "received for 1 second\n");
        }
        quit = 1;
        udpheader->received = 0;
        timeout_ocurred = 1;

    } else {

      /* Timeout must have a number in it */
      if (timeout.tv_usec != 500000) {
        udpheader->select_sleep += (500000 - timeout.tv_usec);
        //fprintf(stderr, "timeout.tv_sec = %d, timeout_tv.usec = %d\n", timeout.tv_sec, timeout.tv_usec);
      }

      /* Get a packet from the socket */
      udpheader->received = recvfrom (udpheader->udpfd, udpheader->socket_buffer, BPSR_UDP_PAYLOAD_BYTES, 0, NULL, NULL);
    }

    /* If we did get a packet within the timeout, or one was in the buffer */
    if (!quit) {

      /* Decode the packets apsr specific header */
      raw_sequence_no = decode_header(udpheader->socket_buffer);
      sequence_no = raw_sequence_no / udpheader->sequence_incr;

      if (udpheader->verbose == 2) {
        fprintf(stderr, "2: %"PRIu64" [%"PRIu64"]\n", sequence_no, raw_sequence_no);
      }

      if ((sequence_no != (prevnum + 1)) && (prevnum != 0)){
        fprintf(stderr, "%"PRIu64" = > %"PRIu64", prev+1 = %"PRIu64"\n",raw_sequence_no, sequence_no, (prevnum+1));
      }
      prevnum = sequence_no;

      /* When we have received the first packet */      
      if ((udpheader->expected_sequence_no == 0) && (data_received == 0)) {

        //fprintf(stderr,"START : received packet %"PRIu64"\n", sequence_no);
        udpheader->expected_sequence_no = sequence_no;


      }

      /* Now try to slot the packet into the appropraite buffer */
      data_received += BPSR_UDP_DATASIZE_BYTES;

      /* Increment statistics */
      udpheader->packets->received++;
      udpheader->packets->received_per_sec++;
      udpheader->bytes->received += BPSR_UDP_DATASIZE_BYTES;
      udpheader->bytes->received_per_sec += BPSR_UDP_DATASIZE_BYTES;

      udpheader->curr_buffer_count++;


      /* If we have filled the current buffer, then we can stop */
      if (udpheader->curr_buffer_count == BPSR_NUM_UDP_PACKETS) {
        quit = 1;
      }

    }
  } 

  /* If the timeout ocurred, this is most likely due to end of data */
  if (timeout_ocurred) {
    *size = udpheader->curr_buffer_count * udpheader->packet_length;
    fprintf(stderr,"Warning: Suspected EOD received, returning %"PRIu64
                    " bytes\n",*size);
  }

  udpheader->expected_sequence_no += BPSR_NUM_UDP_PACKETS;
  udpheader->prev_time = udpheader->current_time;
  udpheader->current_time = time(0);
  
  if (udpheader->prev_time != udpheader->current_time) {

    //fprintf(stderr, "slept %5.2f msec, pack / sec = %"PRIu64", Bytes / sec = %"PRIu64"\n",
    //                ((float) udpheader->select_sleep) / 1000, 
    //                udpheader->packets->received_per_sec, 
    //                udpheader->bytes->received_per_sec);
    udpheader->packets->received_per_sec = 0;
    udpheader->bytes->received_per_sec = 0;

  }

  assert(udpheader->curr_buffer != 0);
  
  return (void *) udpheader->curr_buffer;

}

/*
 * Close the udp socket and file
 */

int udpheader_stop_function (udpheader_t* udpheader)
{

  /* get our context, contains all required params */
  float percent_dropped = 0;

  if (udpheader->expected_sequence_no) {
    percent_dropped = (float) ((double)udpheader->packets->dropped / (double)udpheader->expected_sequence_no);
  }

  fprintf(stderr, "Packets dropped %"PRIu64" / %"PRIu64" = %10.8f %%\n",
          udpheader->packets->dropped, udpheader->expected_sequence_no, 
          percent_dropped);
  
  close(udpheader->udpfd);
  free(udpheader->curr_buffer);
  free(udpheader->next_buffer);
  free(udpheader->socket_buffer);

  return 0;

}


int main (int argc, char **argv)
{

  /* Interface on which to listen for udp packets */
  char * interface = "any";

  /* port on which to listen for incoming connections */
  int port = BPSR_DEFAULT_UDPDB_PORT;

  /* accumulation length of the iBob board */
  int acc_len = BPSR_DEFAULT_ACC_LEN;

  /* Flag set in verbose mode */
  char verbose = 0;

  int arg = 0;

  /* actual struct with info */
  udpheader_t udpheader;

  /* Pointer to array of "read" data */
  char *src;

  while ((arg=getopt(argc,argv,"i:p:a:vVh")) != -1) {
    switch (arg) {

    case 'i':
      if (optarg)
        interface = optarg;
      break;

    case 'p':
      port = atoi (optarg);
      break;

    case 'a':
      acc_len = atoi (optarg);
      break;

    case 'v':
      verbose=1;
      break;

    case 'V':
      verbose=2;
      break;

    case 'h':
      usage();
      return 0;
      
    default:
      usage ();
      return 0;
      
    }
  }

  assert ((BPSR_UDP_DATASIZE_BYTES + BPSR_UDP_COUNTER_BYTES) == BPSR_UDP_PAYLOAD_BYTES);

  multilog_t* log = multilog_open ("bpsr_udpheader", 0);
  multilog_add (log, stderr);
  multilog_serve (log, DADA_DEFAULT_PWC_LOG);

  udpheader.log = log;

  /* Setup context information */
  udpheader.verbose = verbose;
  udpheader.port = port;
  udpheader.interface = strdup(interface);
  udpheader.acc_len = acc_len;

  /* init stats structs */
  stats_t packets = {0,0,0,0};
  stats_t bytes = {0,0,0,0};
                                                                                
  udpheader.packets = &packets;
  udpheader.bytes = &bytes;

  udpheader.dropped_packets_to_fill = 0;
  udpheader.packet_in_buffer = 0;
  udpheader.received = 0;
  udpheader.prev_time = time(0);
  udpheader.current_time = udpheader.prev_time;
  udpheader.error_seconds = 10;
  udpheader.packet_length = 0;
  udpheader.state = NOTRECORDING;
    

  time_t utc = udpheader_start_function(&udpheader,0);

  if (utc == -1 ) {
    fprintf(stderr,"Error: udpheader_start_function failed\n");
    return EXIT_FAILURE;
  }

  int quit = 0;

  while (!quit) {

    uint64_t bsize = udpheader.datasize;

   /* TODO Add a quit control to the read function */
    src = (char *) udpheader_read_function(&udpheader, &bsize);

    /* Quit if we dont get a packet for at least 1 second whilst recording */
    //if ((bsize <= 0) && (udpheader.state == RECORDING)) 
    //  quit = 1;

    if (udpheader.verbose == 2)
      fprintf(stdout,"udpheader_read_function: read %"PRIu64" bytes\n", bsize);
  }    

  if ( udpheader_stop_function(&udpheader) != 0)
    fprintf(stderr, "Error stopping acquisition");

  return EXIT_SUCCESS;

}


