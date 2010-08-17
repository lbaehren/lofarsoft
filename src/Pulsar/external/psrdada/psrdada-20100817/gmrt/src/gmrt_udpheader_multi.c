/***************************************************************************
 *   
 *     Copyright (C) 2009 by Andrew Jameson
 *     Licensed under the Academic Free License version 2.1
 *  
 ***************************************************************************/

/*
 * gmrt_udpheader_multi
 *
 * Reads GMRT udp packets but does not require a datablock 
 *
 */

#include <sys/socket.h>
#include "sock.h"
#include <math.h>
#include <pthread.h>

#include "gmrt_udpheader_multi.h"
#include "dada_generator.h"

int quit_threads = 0;

void usage()
{
  fprintf (stdout,
   "gmrt_udpheader_multi [options]\n"
     " -h             print help text\n"
     " -a antsys      antsys.hdr file [default `pwd`/antsys.hdr]\n"
     " -i interface   ip/interface for inc. UDP packets [default all]\n"
     " -n streams     number of streams to interpret in the UDP packets [default 1]\n"
     " -p port        port on which to listen [default %d]\n"
     " -s sampler     sampler.hdr file [default `pwd`/sampler.hdr]\n"
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
  gmrt_udpheader_multi_t udpheader;

  /* struct for the receiver */
  gmrt_receiver_t receiver;

  /* Pointer to array of "read" data */
  char *src;

	/* antsys.hdr file */
	char * antsys_hdr = "antsys.hdr";

	/* sampler.hdr file */
	char * sampler_hdr = "sampler.hdr";

  char * source_hdr = "source.hdr";

	unsigned num_hdus = 1;

  /* command line arguments */
  while ((arg=getopt(argc,argv,"i:n:o:p:vh")) != -1) {
    switch (arg) {

    case 'a':
      if (optarg) {
        antsys_hdr = strdup(optarg);
        break;
      } else {
        usage();
        return EXIT_FAILURE;
      }

    case 'i':
      if (optarg)
        interface = optarg;
      break;

		case 'n':
			if (optarg)
				num_hdus = atoi(optarg);
			break;

    case 'p':
			if (optarg)
      	port = atoi (optarg);
      break;

    case 'v':
      verbose=1;
      break;

    case 'o':
      if (optarg) {
        source_hdr = strdup(optarg);
        break;
      } else {
        usage();
        return EXIT_FAILURE;
      }

    case 's':
      if (optarg) {
        sampler_hdr = strdup(optarg);
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

	if (num_hdus != 2) 
	{
		fprintf(stderr, "ERROR: Sorry - only support for 2 hdus atm\n");
		usage();
		exit(EXIT_FAILURE);
	}

  /* parse required command line options */
  if (argc-optind != 0) {
    fprintf(stderr, "ERROR: 0 command line arguments are expected\n\n");
    usage();
    exit(EXIT_FAILURE);
  }

  multilog_t* log = multilog_open ("gmrt_udpheader_multi", 0);
  multilog_add (log, stderr);

  udpheader.log = log;
  receiver.log = log;

  /* Setup context information */
  receiver.verbose = verbose;
  receiver.interface = strdup(interface);
  receiver.port = port;
  receiver.start_on_reset = 0;
	udpheader.receiver = &receiver;

  signal(SIGINT, signal_handler);

  /* intialize memory strucutres */   
  gmrt_receiver_init(&receiver, num_hdus);

  multilog(log, LOG_INFO, "starting stats_thread()\n");
  if (pthread_create (&stats_thread_id, 0, (void *) stats_thread, (void *) &receiver) < 0) {
    perror ("Error creating new thread");
    return -1;
  }

  receiver.sod = 0;

	/* setup some of the delay required values */
  // TODO need to get these variables from the header [hardcode for the mo]
  //udpheader.source.ra_app      = 1.498449;
  //udpheader.source.dec_app     = 0.870157;
  //udpheader.source.freq[0]     = 1190000000;
  //udpheader.source.first_lo[0] = 1340000000.000000;
  //udpheader.source.bb_lo[0]    = 70000000.000000;

	udpheader.receiver->bytes_per_sample = 1;
	udpheader.receiver->t_samp = 0.0025;
	udpheader.receiver->bytes_per_second = (udpheader.receiver->bytes_per_sample / udpheader.receiver->t_samp) * 1000000;

	gettimeofday(&(udpheader.utc_start), 0);

  /* start function - opens UDP socket */
  gmrt_receiver_start(&receiver);

  // read the antennasys header file
  if (read_antenna_file(antsys_hdr, udpheader.corr.antenna) < 0)
    return EXIT_FAILURE;

  // read the antenna connectivity
  if (read_antenna_connectivity(sampler_hdr, &(udpheader.corr)) < 0)
    return EXIT_FAILURE;

  // read the source header file TODO, should be integrated into start
  // function and read these values from the header...
  double BW;
  char source_name[100];
  if (read_source_file(source_hdr, &(udpheader.source), &BW, source_name ) < 0)
    return EXIT_FAILURE;

  uint64_t bsize = (UDP_DATA * UDP_NPACKS);
	void ** buffers;

  while (!quit_threads) {

    bsize = (UDP_DATA * UDP_NPACKS);
    while (bsize == UDP_DATA * UDP_NPACKS) 
    {

      bsize = (UDP_DATA * UDP_NPACKS);
			buffers = gmrt_udpheader_multi_buffer_function(&udpheader, &bsize);

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
 * Read data from the UDP socket and return a pointer to the data read
 * and set the size to the data read
 */
void ** gmrt_udpheader_multi_buffer_function (gmrt_udpheader_multi_t * ctx, uint64_t* size)
{

  /* calculate the delays */  
  ctx->receiver->total_time = ctx->receiver->total_bytes / ctx->receiver->bytes_per_second;
  ctx->timestamp.tv_sec = (int) ctx->receiver->total_time;
  ctx->timestamp.tv_usec = (ctx->receiver->total_time - ctx->timestamp.tv_sec) * 1000000;
  multilog(ctx->log, LOG_INFO, "total_bytes=%lf, total_time=%lf s, timestamp.tv_sec = %d, timestamp.tv_usec = %lf\n", ctx->receiver->total_bytes, ctx->receiver->total_time, ctx->timestamp.tv_sec, ctx->timestamp.tv_usec);

  ctx->timestamp.tv_sec += ctx->utc_start.tv_sec;
  multilog(ctx->log, LOG_INFO, "timestamp.tv_sec = %d\n", ctx->timestamp.tv_sec);

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




