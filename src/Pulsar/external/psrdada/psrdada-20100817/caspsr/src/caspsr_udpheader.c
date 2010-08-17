/***************************************************************************
 *   
 *     Copyright (C) 2009 by Andrew Jameson
 *     Licensed under the Academic Free License version 2.1
 *  
 ***************************************************************************/

/*
 * caspsr_udpheader
 *
 * Reads CASPSR udp packets but does not require a datablock 
 *
 */

#include <sys/socket.h>
#include "sock.h"
#include <math.h>
#include <pthread.h>

#include "caspsr_udpheader.h"
#include "dada_generator.h"

int quit_threads = 0;

void usage()
{
  fprintf (stdout,
   "caspsr_udpheader [options] i_dest n_dests n_packest n_append\n"
     " -h             print help text\n"
     " -i interface   ip/interface for inc. UDP packets [default all]\n"
     " -p port        port on which to listen [default %d]\n"
     " -v             verbose messages\n"
     " i_dest         the number of this destination\n"
     " n_dests        the total number of destinations\n"
     " n_packets      number of packets per xfer\n"
     " n_append       number of extra packets per xfer\n",
     CASPSR_DEFAULT_UDPDB_PORT);
}


int main (int argc, char **argv)
{

  /* Interface on which to listen for udp packets */
  char * interface = "any";

  /* port on which to listen for incoming connections */
  int port = CASPSR_DEFAULT_UDPDB_PORT;

  /* Flag set in verbose mode */
  char verbose = 0;

  /* statistics thread */
  pthread_t stats_thread_id;

  int arg = 0;

  /* actual struct with info */
  udpheader_t udpheader;

  /* struct for the receiver */
  caspsr_receiver_t receiver;

  /* Pointer to array of "read" data */
  char *src;

  /* command line arguments */
  unsigned i_dest = 0;
  unsigned n_dest = 0;
  unsigned n_packets = 0;
  unsigned n_append = 0;

  while ((arg=getopt(argc,argv,"i:p:vh")) != -1) {
    switch (arg) {

    case 'i':
      if (optarg)
        interface = optarg;
      break;

    case 'p':
      port = atoi (optarg);
      break;

    case 'v':
      verbose=1;
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

  if (argc-optind != 4) {
    fprintf(stderr, "ERROR: 4 command line arguments are required\n\n");
    usage();
    exit(EXIT_FAILURE);
  }

  if (sscanf(argv[optind], "%d", &i_dest) != 1) {
    fprintf(stderr, "ERROR: failed to parse the number of this dest\n\n");
    usage();
    exit(EXIT_FAILURE);
  }

  if (sscanf(argv[optind+1], "%d", &n_dest) != 1) {
    fprintf(stderr, "ERROR: failed to parse the number of destinations\n\n");
    usage();
    exit(EXIT_FAILURE);
  }
  
  if (sscanf(argv[optind+2], "%d", &n_packets) != 1) {
    fprintf(stderr, "ERROR: failed to parse the number of packets per xfer\n\n");
    usage();
    exit(EXIT_FAILURE);
  }
  
  if (sscanf(argv[optind+3], "%d", &n_append) != 1) {
    fprintf(stderr, "ERROR: failed to parse the number of appended packets per xfer\n\n");
    usage();
    exit(EXIT_FAILURE);
  }
  
  if (n_dest < 1 || n_dest > 16) {
    fprintf(stderr, "ERROR: n_dest [%d] must be between 1 and 16\n\n", n_dest);
    usage();
    exit(EXIT_FAILURE);
  }
  
  if (i_dest < 0 || i_dest >= n_dest) {
    fprintf(stderr, "ERROR: i_dest [%d] must be between 0 and n_dest-1\n\n", i_dest);
    usage();
    exit(EXIT_FAILURE);
  }
  
  if (n_packets < 1) {
    fprintf(stderr, "ERROR: n_packets [%d] must be > 0\n\n", n_packets);
    usage();
    exit(EXIT_FAILURE);
  }
  
  if (n_append < 0) {
    fprintf(stderr, "ERROR: n_append [%d] must be >= 0\n\n", n_append);
    usage();
    exit(EXIT_FAILURE);
  }

  multilog_t* log = multilog_open ("caspsr_udpheader", 0);
  multilog_add (log, stderr);

  udpheader.log = log;
  receiver.log = log;

  /* Setup context information */
  receiver.verbose = verbose;
  receiver.interface = strdup(interface);
  receiver.port = port;

  signal(SIGINT, signal_handler);

  /* intialize memory strucutres */   
  caspsr_receiver_init(&receiver, i_dest, n_dest, n_packets, n_append);

  multilog(log, LOG_INFO, "starting stats_thread()\n");
  if (pthread_create (&stats_thread_id, 0, (void *) stats_thread, (void *) &receiver) < 0) {
    perror ("Error creating new thread");
    return -1;
  }

  uint64_t bsize = (UDP_DATA * UDP_NPACKS);
  uint64_t first_byte = 0;

  while (!quit_threads) {

    first_byte = caspsr_start_xfer(&receiver);
    multilog(log, LOG_INFO, "receiver begins xfer on byte=%"PRIu64"\n",first_byte);

    bsize = (UDP_DATA * UDP_NPACKS);
    while (bsize == UDP_DATA * UDP_NPACKS) 
    {

      bsize = (UDP_DATA * UDP_NPACKS);
      src = (char *) caspsr_xfer(&receiver, &bsize);

      if (bsize != UDP_DATA * UDP_NPACKS) 
        multilog(log, LOG_INFO, "caspsr_xfer returned %"PRIu64" bytes, end of xfer\n", bsize);

      if (bsize == 0)  
      {
        quit_threads = 1;
        multilog(log, LOG_INFO, "caspsr_xfer return 0 bytes, end of observation\n");
      }
      
    }

    fprintf(stderr, "receiver finished xfer\n"); 

    if (caspsr_stop_xfer(&receiver) != 0) 
      fprintf(stderr, "Error stopping acquisition");
    
  }

  /* join threads */
  void* result = 0;
  fprintf(stderr, "joining stats_thread\n");
  pthread_join (stats_thread_id, &result);

  caspsr_receiver_dealloc(&receiver);

  return EXIT_SUCCESS;

}

/* 
 *  Thread to print simple capture statistics
 */
void stats_thread(void * arg) {

  caspsr_receiver_t * ctx = (caspsr_receiver_t *) arg;
  
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

    fprintf(stderr,"T=%5.2f, R=%5.2f MB/s\t D=%5.2f MB/s packets=%"PRIu64" dropped=%"PRIu64"\n", (mb_received_ps+mb_dropped_ps), mb_received_ps, mb_dropped_ps, ctx->bytes->received, ctx->bytes->dropped);
    sleep(1);
  }
}

/*
 *  Simple signal handler to exit more gracefully
 */
void signal_handler(int signalValue) {

  fprintf(stderr, "received signal %d\n", signalValue);
  if (quit_threads) {
    fprintf(stderr, "received signal %d twice, hard exit\n", signalValue);
    exit(EXIT_FAILURE);
  }

  quit_threads = 1;

}




