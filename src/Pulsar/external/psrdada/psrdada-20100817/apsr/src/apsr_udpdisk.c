
#include <stdlib.h>
#include "sock.h"
#include <math.h>

#include "apsr_udpdisk.h"


void usage()
{
  fprintf (stdout,
	   "apsr_udpdisk [options] header_file\n"
     " -h             print help text\n"
     " -D path        add a disk to which data will be written\n"
	   " -p port        port on which to listen [default %d]\n"
     " -v             verbose messages\n"
     " -V             very verbose messages\n"
     " header_file    file containing ascii header\n",APSR_DEFAULT_UDPDISK_PORT);
}


time_t udpdisk_start_function (udpdisk_t* udpdisk, time_t start_utc)
{

  multilog_t* log = udpdisk->log;

  /* Initialise variables */
  udpdisk->packets_dropped_this_run = 0;
  udpdisk->packets_received_this_run = 0;
  udpdisk->bytes_received_this_run = 0;
  udpdisk->error_seconds = 10;
  udpdisk->dropped_packets_to_fill = 0;
  udpdisk->packet_in_buffer = 0;
  udpdisk->prev_time = time(0);
  udpdisk->current_time = udpdisk->prev_time;
  udpdisk->curr_buffer_count = 0;
  udpdisk->next_buffer_count = 0;
  udpdisk->packets_late_this_sec = 0;

  if (udpdisk->verbose == 2) 
    fprintf(stderr, "apsr_udpdisk: creating udp socket\n");        

  /* Create a udp socket */
  udpdisk->udpfd = apsr_create_udp_socket(log, "any", udpdisk->port, udpdisk->verbose);
  if (udpdisk->udpfd < 0) {
    multilog (log, LOG_ERR, "Failed to create udp socket\n");
    return 0; // n.b. this is an error value 
  }
          
  /* Set the current machines name in the header block as RECV_HOST */
  char myhostname[HOST_NAME_MAX] = "unknown";;
  gethostname(myhostname,HOST_NAME_MAX); 

  /* setup the data buffer for NUMUDPACKETS udp packets */
  udpdisk->datasize = 32000000;
  udpdisk->curr_buffer = (char *) malloc(sizeof(char) * udpdisk->datasize);
  assert(udpdisk->curr_buffer != 0);
  udpdisk->next_buffer = (char *) malloc(sizeof(char) * udpdisk->datasize);
  assert(udpdisk->next_buffer != 0);

  if (udpdisk->verbose == 2) 
    fprintf(stderr, "apsr_udpdisk: setting internal buffers to %"PRIu64" bytes\n",
                    udpdisk->datasize);

  /* 0 both the curr and next buffers */
  char zerodchar = 'c';
  memset(&zerodchar,0,sizeof(zerodchar));
  memset(udpdisk->curr_buffer,zerodchar,udpdisk->datasize);
  memset(udpdisk->next_buffer,zerodchar,udpdisk->datasize);

  /* Setup the socket buffer for receiveing UDP packets */
  udpdisk->socket_buffer= (char *) malloc(sizeof(char) * UDPBUFFSIZE);
  assert(udpdisk->socket_buffer != 0);

  /* setup the expected sequence no to the initial value */
  udpdisk->expected_sequence_no = 0;

  /* set the packet_length to an expected value */
  udpdisk->packet_length = 8972 - UDPHEADERSIZE;

  /* utc start, as defined by UTC_START attribute */
  char utc_start[64] = "";

  /* observation offset from the UTC_START */
  uint64_t obs_offset = 0;

  /* size of each file to be written in bytes, as determined by FILE_SIZE */
  uint64_t file_size = 0;

  /* Get the UTC_START */
  if (ascii_header_get (udpdisk->header, "UTC_START", "%s", utc_start) != 1) {
    fprintf(stderr,"Warning: Header with no UTC_START\n");
    strcpy (utc_start, "UNKNOWN");
  }

  if (udpdisk->verbose == 2) 
    fprintf(stderr, "apsr_udpdisk: UTC_START = %s\n", utc_start);


  /* Get the OBS_OFFSET */
  if (ascii_header_get (udpdisk->header, "OBS_OFFSET", "%"PRIu64, &obs_offset) != 1){
    fprintf(stderr,"Warning: Header with no OBS_OFFSET\n");
    obs_offset = 0;
  }

  if (udpdisk->verbose == 2) 
    fprintf(stderr, "apsr_udpdisk: OBS_OFFSET = %"PRIu64"\n", obs_offset);

  /* Always using the 0th filenumber */
  udpdisk->file_number = 0;

  if (udpdisk->verbose == 2) 
    fprintf(stderr, "apsr_udpdisk: FILE_NUMBER = %"PRIu64"\n", udpdisk->file_number);

  /* Set the file number to be written to the header */
  if (ascii_header_set (udpdisk->header, "FILE_NUMBER", "%u", udpdisk->file_number)<0) {
    fprintf(stderr,"Error writing FILE_NUMBER\n");
    return -1;
  }

  if (udpdisk->verbose == 2) 
    fprintf(stderr, "apsr_udpdisk: creating the file name\n");

  /* create the current file name */
  snprintf (udpdisk->file_name, FILENAME_MAX, "%s_%"PRIu64".%06u.dada",
            utc_start, obs_offset, udpdisk->file_number);

  if (udpdisk->verbose == 2) 
    fprintf(stderr, "apsr_udpdisk: file name is %s\n",udpdisk->file_name);

  /* Get the file size */
  if (ascii_header_get (udpdisk->header, "FILE_SIZE", "%"PRIu64, &file_size) != 1) {
    fprintf(stderr, "Header with no FILE_SIZE, setting to %"PRIu64"\n", 
    DADA_DEFAULT_FILESIZE);
    file_size = DADA_DEFAULT_FILESIZE;
  }

  if (udpdisk->verbose == 2) 
    fprintf(stderr, "apsr_udpdisk: file size is %"PRIu64"\n",file_size);

  uint64_t optimal_bytes;
  int fd = disk_array_open (udpdisk->array, udpdisk->file_name, file_size, 
                        &optimal_bytes, 0);

  if (fd < 0) {
    fprintf(stderr,"Error opening %s: %s\n", udpdisk->file_name, strerror (errno));
    return -1;
  }

  fprintf(stdout, "%s opened for writing %"PRIu64" bytes\n", udpdisk->file_name, 
                  file_size);

  udpdisk->filefd = fd;
  udpdisk->transfer_bytes = file_size;
  udpdisk->optimal_bytes = 512 * optimal_bytes;

  if (udpdisk->verbose) {
    fprintf(stderr,"apsr_udpdisk: transfer bytes = %"PRIu64"\n", udpdisk->transfer_bytes);
    fprintf(stderr,"apsr_udpdisk: optimal bytes = %"PRIu64"\n", udpdisk->optimal_bytes);
  }

  /* Since udpdisk cannot know the time sample of the first 
   * packet we always return 0 */
  return 0;
}

void* udpdisk_read_function (udpdisk_t* udpdisk, uint64_t* size)
{
  
  /* Packets dropped in the last second */
  uint64_t packets_dropped_this_second = 0;

  /* How many packets will fit in each curr_buffer and next_buffer */
  uint64_t buffer_capacity = udpdisk->datasize / udpdisk->packet_length;

  header_struct header = { '0', '0', 0, '0', '0', '0',"0000", 0 };

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
  tmp = udpdisk->curr_buffer;
  udpdisk->curr_buffer = udpdisk->next_buffer;
  udpdisk->next_buffer = tmp;
  udpdisk->curr_buffer_count = udpdisk->next_buffer_count;
  udpdisk->next_buffer_count = 0;

  /* 0 the next buffer */
  memset(udpdisk->next_buffer,zerodchar,udpdisk->datasize);

  /* Determine the sequence number boundaries for curr and next buffers */
  uint64_t min_sequence = udpdisk->expected_sequence_no;
  uint64_t mid_sequence = min_sequence + buffer_capacity;
  uint64_t max_sequence = mid_sequence + buffer_capacity;

  if (udpdisk->verbose == 2) 
    fprintf(stderr, "[%"PRIu64" <-> %"PRIu64" <-> %"PRIu64"]\n",min_sequence,
                    mid_sequence,max_sequence); 

  /* Assume we will be able to return a full buffer */
  *size = buffer_capacity * udpdisk->packet_length;

  /* Continue to receive packets */
  while (!quit) {

    /* If we had a packet in the socket_buffer a previous call to the 
     * buffer function*/
    if (udpdisk->packet_in_buffer) {

      udpdisk->packet_in_buffer = 0;
                                                                                
    /* Else try to get a fresh packet */
    } else {
                                                                                
      /* select to see if data has arrive. If recording, allow a 1 second
       * timeout, else 0.1 seconds to ensure buffer function is responsive */
      if (udpdisk->state == RECORDING) {
        timeout.tv_sec=1;
        timeout.tv_usec=0;
      } else {
        timeout.tv_sec=60;
        timeout.tv_usec=0;
      }

      FD_ZERO (&readset);
      FD_SET (udpdisk->udpfd, &readset);
      rdsp = &readset;

      if ( select((udpdisk->udpfd+1),rdsp,NULL,NULL,&timeout) == 0 ) {
  
        if ((udpdisk->state == RECORDING) &&
            (udpdisk->expected_sequence_no != 0)) {
          fprintf(stderr, "Warning: UDP packet timeout: no packet "
                          "received for 1 second\n");
        }
        quit = 1;
        udpdisk->received = 0;
        timeout_ocurred = 1;

      } else {

        /* Get a packet from the socket */
        udpdisk->received = recvfrom (udpdisk->udpfd, udpdisk->socket_buffer,
                                     UDPBUFFSIZE, 0, NULL, NULL);
      }
    }

    /* If we did get a packet within the timeout, or one was in the buffer */
    if (!quit) {

      /* Decode the packets apsr specific header */
      decode_header(udpdisk->socket_buffer, &header);

      /* When we have received the first packet */      
      if ((udpdisk->expected_sequence_no == 0) && (data_received == 0)) {

        udpdisk->state = RECORDING;

      	/* print_header(&header); */

        if (header.sequence != 0) {
          fprintf(stderr,"Warning: First packet received had sequence "
                         "number %d\n",header.sequence);
          /* TODO probably remove this! */
          //udpdisk->expected_sequence_no = header.sequence;
        } else {
          /* multilog (log, LOG_INFO, "First packet received\n"); */
        }

        /* If the UDP packet thinks its header is different to the prescribed
         * size, give up */
        if (header.length != UDPHEADERSIZE) {
          fprintf(stderr, "Error: Custom UDP header length incorrect. "
                   "Expected %d bytes, received %d bytes\n",UDPHEADERSIZE,
                   header.length);
          *size = DADA_ERROR_HARD;
          break;
        }

        /* define length of all future packets */
        udpdisk->packet_length = udpdisk->received - UDPHEADERSIZE;

        /* Set the number of packets a buffer can hold */
        buffer_capacity = udpdisk->datasize / udpdisk->packet_length;


        if (udpdisk->verbose == 2) 
          fprintf(stderr, "packet_length = %"PRIu64" bytes, udpdisk->datasize = %"
                          PRIu64", buffer_capacity = %"PRIu64"\n",
                          udpdisk->packet_length, udpdisk->datasize, 
                          buffer_capacity);

        /* Adjust sequence numbers, now that we know packet_length*/
        min_sequence = udpdisk->expected_sequence_no;
        mid_sequence = min_sequence + buffer_capacity;
        max_sequence = mid_sequence + buffer_capacity;

        //multilog(log, LOG_INFO, "FIRST: [%"PRIu64" <-> %"PRIu64" <-> %"PRIu64"]\n",min_sequence,mid_sequence,max_sequence); 

        /* Set the amount a data we expect to return */
        *size = buffer_capacity * udpdisk->packet_length;

      }

      /* If the packet we received was too small, pad it */
      if (udpdisk->received < (udpdisk->packet_length + UDPHEADERSIZE)) {

        uint64_t amount_to_pad = (udpdisk->packet_length + UDPHEADERSIZE) - 
                                 udpdisk->received;
        char * buffer_pos = (udpdisk->socket_buffer) + udpdisk->received;

        /* 0 the next buffer */
        memset(buffer_pos, zerodchar, amount_to_pad);

        fprintf(stderr,"Warning: Short packet received, padded %"PRIu64" bytes\n", amount_to_pad);
      }

      /* If the packet we received was too long, warn about it */
      if (udpdisk->received > (udpdisk->packet_length + UDPHEADERSIZE)) {
        fprintf(stderr,"Warning: Long packet received, truncated to %"PRIu64
                       " bytes", udpdisk->packet_length);
      }

      /* Now try to slot the packet into the appropraite buffer */
      data_received += (udpdisk->received - UDPHEADERSIZE);

      /* Increment statistics */
      udpdisk->packets_received_this_run++;
      udpdisk->packets_received++;
      udpdisk->bytes_received_this_run += (udpdisk->received - UDPHEADERSIZE);
      udpdisk->bytes_received += (udpdisk->received - UDPHEADERSIZE);
 
      /* If the packet belongs in the curr_buffer */
      if ((header.sequence >= min_sequence) && 
         (header.sequence <  mid_sequence)) {

        uint64_t buf_offset = (header.sequence - min_sequence) * 
                               udpdisk->packet_length;
  
        memcpy( (udpdisk->curr_buffer)+buf_offset, 
                (udpdisk->socket_buffer)+UDPHEADERSIZE,
                udpdisk->packet_length);

        udpdisk->curr_buffer_count++;


      } else if ((header.sequence >= mid_sequence) && 
                 (header.sequence <  max_sequence)) {

        uint64_t buf_offset = (header.sequence - mid_sequence) *
                               udpdisk->packet_length;

        memcpy( (udpdisk->curr_buffer)+buf_offset, 
                (udpdisk->socket_buffer)+UDPHEADERSIZE,
                udpdisk->packet_length);

        udpdisk->next_buffer_count++;

      /* If this packet has arrived too late, it has already missed out */
      } else if (header.sequence < min_sequence) {
 
        udpdisk->packets_late_this_sec++; 

      /* If a packet has arrived too soon, then we give up trying to fill the 
         curr_buffer and return what we do have */
      } else if (header.sequence >= max_sequence) {

        fprintf(stderr, "Warning: Packet %d arrived too soon, expecting "
                        "packets %"PRIu64" through %"PRIu64"\n",
                        header.sequence, min_sequence, max_sequence);

        udpdisk->packet_in_buffer = 1;
        quit = 1;

      } else {

        /* SHOULD NEVER HAPPEN */    
        fprintf (stderr,"ERROR header sequence number invalid\n");

      }

      /* If we have filled the current buffer, then we can stop */
      if (udpdisk->curr_buffer_count == buffer_capacity) {
        quit = 1;
      } else {
        assert(udpdisk->curr_buffer_count < buffer_capacity);
      }

      /* If the next buffer is at least half full */
      if (udpdisk->next_buffer_count > (buffer_capacity / 2)) {
        quit = 1;
      }
    }

  } 

  /* If we have received a packet during this function call */
  if (data_received) {

    /* If we have not received all the packets we expected */
    if ((udpdisk->curr_buffer_count < buffer_capacity) && (!timeout_ocurred)) {

      fprintf(stderr,"Warning: Dropped %"PRIu64" packets\n", 
              (buffer_capacity - udpdisk->curr_buffer_count));

      udpdisk->packets_dropped += buffer_capacity - udpdisk->curr_buffer_count;
      udpdisk->packets_dropped_this_run += buffer_capacity 
                                         - udpdisk->curr_buffer_count;
    }

    /* If the timeout ocurred, this is most likely due to end of data */
    if (timeout_ocurred) {
      *size = udpdisk->curr_buffer_count * udpdisk->packet_length;
      fprintf(stderr,"Warning: Suspected EOD received, returning %"PRIu64
                      " bytes\n",*size);
    }

    udpdisk->expected_sequence_no += buffer_capacity;

  } else {

    /* If we have received no data, then return a size of 0 */
    *size = 0;

  }

  udpdisk->prev_time = udpdisk->current_time;
  udpdisk->current_time = time(0);
  
  if (udpdisk->prev_time != udpdisk->current_time) {

    packets_dropped_this_second  = udpdisk->packets_dropped - 
                                   udpdisk->packets_dropped_last_sec;
    udpdisk->packets_dropped_last_sec = udpdisk->packets_dropped;

    uint64_t bytes_received_this_second = udpdisk->bytes_received -
                                   udpdisk->bytes_received_last_sec;
    udpdisk->bytes_received_last_sec = udpdisk->bytes_received;

    if (udpdisk->verbose) { 

      if (udpdisk->verbose == 2) {
     
          fprintf(stdout, "Packets: %"PRIu64" Bytes: %"PRIu64" Bytes per second: %"
                          PRIu64"\n", udpdisk->packets_received, 
                          udpdisk->bytes_received, bytes_received_this_second);
      } else {

        fprintf(stdout, "npacks dropped: %"PRIu64", npackets: %"PRIu64", "
                        "nbytes: %"PRIu64", nbytes_ps: %"PRIu64"\n",
               packets_dropped_this_second, udpdisk->packets_received, 
               udpdisk->bytes_received, bytes_received_this_second);
      }
    }


    if (udpdisk->packets_late_this_sec) {
      fprintf(stderr, "%"PRIu64" packets arrvived late\n", 
                      packets_dropped_this_second);
      udpdisk->packets_late_this_sec = 0;
    }
  
  }

  assert(udpdisk->curr_buffer != 0);
  
  return (void *) udpdisk->curr_buffer;

}

/*
 *  Write the data to disk 
 */

int64_t udpdisk_write_function (udpdisk_t* udpdisk, void* data, uint64_t size)
{
  return write (udpdisk->filefd, data, size);
}

/*
 * Close the udp socket and file
 */

int udpdisk_stop_function (udpdisk_t* udpdisk)
{

  /* get our context, contains all required params */
  int verbose = udpdisk->verbose; /* saves some typing */

  float percent_dropped = 0;
  if (udpdisk->expected_sequence_no) {
    percent_dropped = (float) ((double)udpdisk->packets_dropped / (double)udpdisk->expected_sequence_no);
  }

  fprintf(stderr, "Packets dropped %"PRIu64" / %"PRIu64" = %10.8f %\n",
          udpdisk->packets_dropped, udpdisk->expected_sequence_no, 
          percent_dropped);
  
  close(udpdisk->udpfd);
  close(udpdisk->filefd);
  free(udpdisk->curr_buffer);
  free(udpdisk->next_buffer);
  free(udpdisk->socket_buffer);

  return 0;

}


int main (int argc, char **argv)
{

  /* port on which to listen for incoming connections */
  int port = APSR_DEFAULT_UDPDISK_PORT;

  /* Flag set in daemon mode */
  char daemon = 0;

  /* Flag set in verbose mode */
  char verbose = 0;

  int arg = 0;

  /* max length of header file */
  unsigned long fSize=800000000;

  /* size of the header buffer */
  uint64_t header_size = 4096;

  /* actual struct with info */
  udpdisk_t udpdisk;

  /* the filename from which the header will be read */
  char* header_file = 0;

  unsigned buffer_size = 64;
  static char* buffer = 0;
  char *src;

  udpdisk.array = disk_array_create ();

  while ((arg=getopt(argc,argv,"D:p:vVh")) != -1) {
    switch (arg) {

    case 'D':
      if (disk_array_add (udpdisk.array, optarg) < 0) {
        fprintf (stderr, "Could not add '%s' to disk array\n", optarg);
        return EXIT_FAILURE;
      }
      break;
      
    case 'p':
      port = atoi (optarg);
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

  if ((argc - optind) != 1) {
    fprintf(stderr,"no header_file was specified\n");
    usage();
    return EXIT_FAILURE;
  }

  header_file = (char *) argv[optind];

  multilog_t* log = multilog_open ("apsr_udpdisk", 0);
  multilog_add (log, stderr);

  /* Setup context information */
  udpdisk.log = log;
  udpdisk.verbose = verbose;
  udpdisk.port = port;
  udpdisk.packets_dropped = 0;
  udpdisk.packets_dropped_this_run = 0;
  udpdisk.packets_dropped_last_sec = 0;
  udpdisk.packets_received = 0;
  udpdisk.packets_received_this_run = 0;
  udpdisk.packets_received_last_sec= 0;
  udpdisk.bytes_received = 0;
  udpdisk.bytes_received_last_sec = 0;
  udpdisk.dropped_packets_to_fill = 0;
  udpdisk.packet_in_buffer = 0;
  udpdisk.received = 0;
  udpdisk.prev_time = time(0);
  udpdisk.current_time = udpdisk.prev_time;
  udpdisk.error_seconds = 10;
  udpdisk.packet_length = 0;
  udpdisk.state = NOTRECORDING;
    
  if (verbose == 2) 
    fprintf(stderr,"apsr_udpdisk: reading header file %s\n", header_file);

  udpdisk.header = malloc(sizeof(char) * header_size);

  if (!udpdisk.header) {
    fprintf(stderr, "Error: Could not allocate %d bytes for header\n", header_size);
    return EXIT_FAILURE;
  }

  if (fileread (header_file, udpdisk.header, header_size) < 0)  {
    fprintf(stderr, "Error: Could not read header from %s\n", header_file);
    return EXIT_FAILURE;
  }

  if (verbose)
    fprintf(stderr,"apsr_udpdisk: Retrieved header information\n");

  if (verbose == 2) {
    fprintf(stderr, "========================================"
                    "========================================\n");
    fprintf(stderr, "%s",udpdisk.header);
    fprintf(stderr, "========================================"
                    "========================================\n");
  }

  if (!buffer)
    buffer = malloc (buffer_size);

  assert (buffer != 0);

  /* Get the UTC_START */
  if (ascii_header_get (udpdisk.header, "UTC_START", "%s", buffer) != 1) {

    /* If it is not set, just use now */
    time_t utc = time(0);

    strftime (buffer, buffer_size, DADA_TIMESTR, gmtime(&utc));

    if (verbose) 
      fprintf(stderr, "apsr_udpdisk: setting UTC_START to %s\n", buffer);

    if (ascii_header_set (udpdisk.header, "UTC_START", "%s", buffer) < 0) {
      fprintf(stderr,"Error: failed ascii_header_set UTC_START\n");
      return EXIT_FAILURE;
    }
  }

  time_t utc = udpdisk_start_function(&udpdisk,0);

  if (utc == -1 ) {
    fprintf(stderr,"Error: udpdisk_start_function failed\n");
    return EXIT_FAILURE;
  }

  int quit = 0;
  int header_written = 0;
  int64_t bytes_written = 0;

  while (!quit) {

    uint64_t bsize = udpdisk.datasize;

   /* TODO Add a quit control to the read function */
    src = (char *) udpdisk_read_function(&udpdisk, &bsize);

    /* Quit if we dont get a packet for at least 1 second whilst recording */
    if ((bsize <= 0) && (udpdisk.state == RECORDING)) 
      quit = 1;

    /* Write the HEADER to the file */
    if (!header_written) {
      udpdisk_write_function(&udpdisk, udpdisk.header, header_size);
      header_written = 1;
    }

    bytes_written = udpdisk_write_function(&udpdisk, src, bsize);

    if ((udpdisk.verbose == 2) && (bytes_written))
      fprintf(stdout,"Bytes transferred: UDP %"PRIu64" => FILE %"PRIu64"\n", bsize, bytes_written);

  }    

  if ( udpdisk_stop_function(&udpdisk) != 0)
    fprintf(stderr, "Error stopping acquisition");

  return EXIT_SUCCESS;

}
