/***************************************************************************
 *   
 *     Copyright (C) 2009 by Andrew Jameson
 *     Licensed under the Academic Free License version 2.1
 *  
 ***************************************************************************/

/*
 * gmrt_udpheader
 *
 * Reads GMRT udp packets but does not require a datablock 
 *
 */

#include <sys/socket.h>
#include "sock.h"
#include <math.h>
#include <pthread.h>

#include "gmrt_udpheader.h"
#include "dada_generator.h"

int quit_threads = 0;

void usage()
{
  fprintf (stdout,
   "gmrt_udpheader [options]\n"
     " -h             print help text\n"
     " -i interface   ip/interface for inc. UDP packets [default all]\n"
     " -p port        port on which to listen [default %d]\n"
     " -v             verbose messages\n",
     GMRT_DEFAULT_UDPDB_PORT);
}


int main (int argc, char **argv)
{

  /* Interface on which to listen for udp packets */
  char * interface = "any";

  /* port on which to listen for incoming connections */
  int port = GMRT_DEFAULT_UDPDB_PORT;

  /* Flag set in verbose mode */
  char verbose = 0;

  /* statistics thread */
  pthread_t stats_thread_id;

  int arg = 0;

  /* actual struct with info */
  udpheader_t udpheader;

  /* struct for the receiver */
  gmrt_receiver_t receiver;

  /* Pointer to array of "read" data */
  char *src;

  /* command line arguments */
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
  if (argc-optind != 0) {
    fprintf(stderr, "ERROR: 0 command line arguments are expected\n\n");
    usage();
    exit(EXIT_FAILURE);
  }

  multilog_t* log = multilog_open ("gmrt_udpheader", 0);
  multilog_add (log, stderr);

  udpheader.log = log;
  receiver.log = log;
  receiver.start_on_reset = 0;

  /* Setup context information */
  receiver.verbose = verbose;
  receiver.interface = strdup(interface);
  receiver.port = port;

  signal(SIGINT, signal_handler);

  /* intialize memory strucutres */   
  gmrt_receiver_init(&receiver, 1);

  multilog(log, LOG_INFO, "starting stats_thread()\n");
  if (pthread_create (&stats_thread_id, 0, (void *) stats_thread, (void *) &receiver) < 0) {
    perror ("Error creating new thread");
    return -1;
  }

  receiver.sod = 0;

  /* start function - opens UDP socket */
  gmrt_receiver_start(&receiver);

  /* receiving thread */
/*
  pthread_t receiving_thread_id;

  int max_pri = sched_get_priority_max(SCHED_RR);
  struct sched_param recv_parm;
  pthread_attr_t recv_attr;
  
  if (pthread_attr_init(&recv_attr) != 0) {
    fprintf(stderr, "pthread_attr_init failed: %s\n", strerror(errno));
    return 1;
  }

  if (pthread_attr_setinheritsched(&recv_attr, PTHREAD_EXPLICIT_SCHED) != 0) {
    fprintf(stderr, "pthread_attr_setinheritsched failed: %s\n", strerror(errno));
    return 1;
  }

  if (pthread_attr_setschedpolicy(&recv_attr, SCHED_RR) != 0) { 
    fprintf(stderr, "pthread_attr_setschedpolicy failed: %s\n", strerror(errno));
    return 1;
  }

  if (pthread_attr_setschedparam(&recv_attr,&recv_parm) != 0) {
    fprintf(stderr, "pthread_attr_setschedparam failed: %s\n", strerror(errno));
    return 1;
  }

  rval = pthread_create (&receiving_thread_id, &recv_attr, (void *) receiving_thread, (void *) &receiver);
  if (rval != 0) {
    multilog(log, LOG_INFO, "Error creating receiving_thread: %s\n", strerror(rval));
    return -1;
  }
*/

  uint64_t bsize = (UDP_DATA * UDP_NPACKS);
  while (!quit_threads) {

    bsize = (UDP_DATA * UDP_NPACKS);
    while (bsize == UDP_DATA * UDP_NPACKS) 
    {

      bsize = (UDP_DATA * UDP_NPACKS);
      src = (char *) gmrt_buffer_function(&receiver, &bsize);

      if (bsize != UDP_DATA * UDP_NPACKS) 
        multilog(log, LOG_INFO, "gmrt_xfer returned %"PRIu64" bytes, end of xfer\n", bsize);

      if (bsize == 0)  
      {
        quit_threads = 1;
        multilog(log, LOG_INFO, "gmrt_xfer return 0 bytes, end of observation\n");
      }
      
    }

    fprintf(stderr, "receiver finished loop\n"); 
    
  }

  /* join threads */
  void* result = 0;
  fprintf(stderr, "joining stats_thread\n");
  pthread_join (stats_thread_id, &result);

  gmrt_receiver_dealloc(&receiver);

  return EXIT_SUCCESS;

}

/* 
 *  Thread that continuously receives UDP packet stream, writing to intermediary buffers
 */
void receiving_thread(void * arg) {

   

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

    fprintf(stderr,"T=%5.2f, R=%5.2f, D=%5.2f MiB/s busy_waits=%"PRIu64" ps. total dropped=%"PRIu64"\n", (mb_received_ps+mb_dropped_ps), mb_received_ps, mb_dropped_ps, busy_waits_this_sec, ctx->bytes->dropped);
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




