/*
 * caspsr_udpthreaded. Reads UDP packets and checks the header for correctness
 */


#include <sys/socket.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#include "sock.h"
#include "caspsr_udpthreaded.h"
#include "dada_generator.h"

static pthread_mutex_t udp_sock_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t memory_mutex = PTHREAD_MUTEX_INITIALIZER;
unsigned quit_threads = 0;

void usage()
{
  fprintf (stdout,
     "caspsr_udpthreaded [options]\n"
     " -h             print help text\n"
     " -i interface   ip/interface for inc. UDP packets [default all]\n"
     " -p port        port on which to listen [default %d]\n"
     " -n nthreads    run with nthreads capturing threads [default 2]\n"
     " -v             verbose messages\n"
     " -V             very verbose messages\n", CASPSR_DEFAULT_UDPDB_PORT);
}


time_t udpthreaded_start_function (udpthreaded_t* ctx, time_t start_utc)
{

  multilog_t* log = ctx->log;

  ctx->packets = init_stats_t();
  ctx->bytes = init_stats_t();
  ctx->curr = caspsr_init_data(UDP_DATA * UDP_NPACKS);
  ctx->next = caspsr_init_data(UDP_DATA * UDP_NPACKS);

  caspsr_zero_data(ctx->curr);
  caspsr_zero_data(ctx->next);

  pthread_mutex_lock (&udp_sock_mutex);

  ctx->udpfd = dada_udp_sock_in(log, ctx->interface, ctx->port, ctx->verbose);
  if (ctx->udpfd < 0) {
    multilog (log, LOG_ERR, "Error, Failed to create udp socket\n");
    return 0;
  }

  /* set the socket size to 64 MB */
  dada_udp_sock_set_buffer_size (log, ctx->udpfd, ctx->verbose, 67108864);

  /* set the socket to non-blocking */
  sock_nonblock(ctx->udpfd);

  pthread_mutex_unlock (&udp_sock_mutex);

  /* Set the current machines name in the header block as RECV_HOST */
  char myhostname[HOST_NAME_MAX] = "unknown";;
  gethostname(myhostname,HOST_NAME_MAX); 

  /* setup the next_seq to the initial value */
  ctx->next_seq = 0;
  ctx->n_sleeps = 0;
  ctx->ooo_packs = 0;

  return 0;
}


void udpthreaded_capture_thread(void * arg)
{

  udpthreaded_t * ctx = (udpthreaded_t *) arg;

  fprintf(stderr, "udpthreaded_capture_thread starting\n");

  uint64_t seq_no = 0;
  uint64_t tmp = 0;
  unsigned have_packet = 0;
  size_t i = 0;
  size_t got = 0;
  unsigned timer = 100;
  struct timeval timeout;
  struct timespec request;
  int errsv;
  unsigned char * arr;

  // local buffer to fit 1 packet
  char * buffer = (char *) malloc(sizeof(char) * UDP_PAYLOAD);
  if (!buffer) {
    fprintf(stderr, "could not malloc buffer for packet capture\n");
    return;
  }

  arr = buffer;

  while (!quit_threads) {

    // get a packet
    while (!have_packet) {

      //pthread_mutex_lock (&udp_sock_mutex);
      got = recvfrom (ctx->udpfd, buffer, UDP_PAYLOAD, 0, NULL, NULL);
      //pthread_mutex_unlock (&udp_sock_mutex);

      if (got == UDP_PAYLOAD) {

        have_packet = 1;

      } else if (got == -1) {

        errsv = errno;
        if (errsv == EAGAIN) {
          ctx->n_sleeps++;

          /* sleep for half a micro second */
          //request.tv_sec = 0;
          //request.tv_nsec = 500;
          //nanoseconds(&request, NULL);
          //timeout.tv_sec = 0;
          //timeout.tv_usec = 1;
          //select(0, NULL, NULL, NULL, &timeout);
          //timer--;
        } else {
          multilog (ctx->log, LOG_ERR, "recvfrom failed: %s\n", strerror(errsv));
          quit_threads = 1;
        }

      } else {
        multilog (ctx->log, LOG_ERR, "Received %d bytes, expected %d\n", got, UDP_PAYLOAD);
      }

      //if (timer == 0) {
      //  multilog (ctx->log, LOG_ERR, "Timeout on receiving a packet\n");
      //  quit_threads = 1;
      //}
    } // while ! have_packet

    //timer = 100;

    if (have_packet) {

      // decode the seq no from first 8 bytes 
      seq_no = UINT64_C (0);
      for (i = 0; i < 8; i++ )
      {
        tmp = UINT64_C (0);
        tmp = arr[8 - i - 1];
        seq_no |= (tmp << ((i & 7) << 3));
      }
      seq_no /= 2048;

      // determine where this sequence number fits
      //pthread_mutex_lock (&memory_mutex);

      /* If we are waiting for the first packet */
      if ((ctx->next_seq == 0) && (ctx->curr->count == 0)) {
        fprintf(stderr,"START : received packet %"PRIu64"\n", seq_no);
        ctx->next_seq = seq_no;

        /* update the min/max sequence numbers for the receiving buffers */
        ctx->curr->min = seq_no;
        ctx->curr->max = ctx->curr->min + UDP_NPACKS;
        ctx->next->min = ctx->curr->max;
        ctx->next->max = ctx->next->min + UDP_NPACKS;

      } else {
        if (ctx->next_seq != seq_no)
          ctx->ooo_packs++;
      }

      /* Increment statistics */
      ctx->packets->received++;
      ctx->bytes->received += UDP_DATA;

      if (seq_no < ctx->curr->min) {
        multilog (ctx->log, LOG_WARNING, "Packet underflow %"PRIu64" < min (%"PRIu64")\n", seq_no, ctx->curr->min);

      } else if (seq_no <= ctx->curr->max) {
        memcpy( ctx->curr->buffer + (seq_no - ctx->curr->min) *  UDP_DATA, buffer + 16, UDP_DATA);
        ctx->curr->count++;

      } else if (seq_no <= ctx->next->max) {

        memcpy( ctx->next->buffer + (seq_no - ctx->next->min) *  UDP_DATA, buffer + 16, UDP_DATA);
        ctx->next->count++;

      } else {

        multilog (ctx->log, LOG_WARNING, "2: Not keeping up. curr %5.2f%, next %5.2f%\n",
                 ((float) ctx->curr->count / (float) UDP_NPACKS)*100,
                 ((float) ctx->next->count / (float) UDP_NPACKS)*100);

      }

      // buffer switch condition
      if ((ctx->curr->count >= UDP_NPACKS) || (ctx->next->count > (UDP_NPACKS/2))) {

        if (ctx->curr->count < UDP_NPACKS) {

          //multilog (ctx->log, LOG_WARNING, "Not keeping up. curr %5.2f%, next %5.2f%\n",
          //        ((float) ctx->curr->count / (float) UDP_NPACKS)*100,
          //         ((float) ctx->next->count / (float) UDP_NPACKS)*100);
          ctx->packets->dropped += (UDP_NPACKS - ctx->curr->count);
          ctx->bytes->dropped += (UDP_DATA * (UDP_NPACKS - ctx->curr->count));

        }

        ctx->temp = ctx->curr;
        ctx->curr = ctx->next;
        ctx->next = ctx->temp;
        ctx->temp = 0;

        /* update the counters for the next buffer */
        ctx->next->count = 0;
        ctx->next->min = ctx->curr->max;
        ctx->next->max = ctx->next->min + UDP_NPACKS;

      }

      ctx->next_seq++;
      //pthread_mutex_unlock (&memory_mutex);
      have_packet = 0;

    } // if have_packet

  } // while !quit_threads

  return;

}


/*
 * Close the udp socket and file
 */

int udpthreaded_stop_function (udpthreaded_t* ctx)
{

  /* get our context, contains all required params */
  float percent_dropped = 0;

  if (ctx->next_seq) {
    percent_dropped = (float) ((double)ctx->packets->dropped / (double)ctx->packets->received);
  }

  fprintf(stderr, "Packets dropped %"PRIu64" / %"PRIu64" = %10.8f %%\n",
          ctx->packets->dropped, ctx->next_seq, percent_dropped);
 
  pthread_mutex_lock (&udp_sock_mutex); 
  close(ctx->udpfd);
  pthread_mutex_unlock (&udp_sock_mutex);

  pthread_mutex_lock (&memory_mutex);
  fprintf(stderr, "free(ctx->curr)\n");
  caspsr_free_data(ctx->curr);
  fprintf(stderr, "free(ctx->next)\n");
  caspsr_free_data(ctx->next);
  pthread_mutex_unlock (&memory_mutex);

  return 0;

}


int main (int argc, char **argv)
{

  /* Interface on which to listen for udp packets */
  char * interface = "any";

  /* port on which to listen for incoming connections */
  int port = CASPSR_DEFAULT_UDPDB_PORT;

  /* Flag set in verbose mode */
  char verbose = 0;

  /* number of capturing threads */
  int nthreads = 2;

  int arg = 0;

  /* actual struct with info */
  udpthreaded_t udpthreaded;

  /* Pointer to array of "read" data */
  char *src;

  while ((arg=getopt(argc,argv,"i:n:p:vVh")) != -1) {
    switch (arg) {

    case 'i':
      if (optarg)
        interface = optarg;
      break;

    case 'n':
      nthreads  = atoi (optarg);
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
  unsigned i=0;

  multilog_t* log = multilog_open ("caspsr_udpthreaded", 0);
  multilog_add (log, stderr);
  multilog_serve (log, DADA_DEFAULT_PWC_LOG);

  udpthreaded.log = log;

  /* Setup context information */
  udpthreaded.verbose = verbose;
  udpthreaded.port = port;
  udpthreaded.interface = strdup(interface);
  udpthreaded.state = NOTRECORDING;

  time_t utc = udpthreaded_start_function(&udpthreaded,0);

  if (utc == -1 ) {
    fprintf(stderr,"Error: udpthreaded_start_function failed\n");
    return EXIT_FAILURE;
  }

  pthread_t stats_thread_id;
  if (pthread_create (&stats_thread_id, 0, (void *) stats_thread, (void *) &udpthreaded) < 0) {
    perror ("Error creating new thread");
    return -1;
  } 

  pthread_t * capture_thread_ids;
  capture_thread_ids = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
  for (i=0; i<nthreads; i++) {
    if (pthread_create (&(capture_thread_ids[i]), 0, (void *) udpthreaded_capture_thread, (void *) &udpthreaded) < 0) {
      perror ("Error creating new thread");
      return -1;
    } 
  }

  signal(SIGINT, signal_handler);
    
  /* sleep whilst the workers work */
  while (!quit_threads) {
    sleep(2);
  }

  /* join threads */
  void* result = 0;
  fprintf(stderr, "joining stats_thread\n");
  pthread_join (stats_thread_id, &result);

  for (i=0; i<nthreads; i++) {
    fprintf(stderr, "joining capture_thread[%d]\n",i);
    pthread_join (capture_thread_ids[i], &result);
  }

  if ( udpthreaded_stop_function(&udpthreaded) != 0)
    fprintf(stderr, "Error stopping acquisition");


  return EXIT_SUCCESS;

}

void stats_thread(void * arg) {

  fprintf(stderr, "stats starting\n");
  udpthreaded_t * ctx = (udpthreaded_t *) arg;

  uint64_t bytes_received_total = 0;
  uint64_t bytes_received_this_sec = 0;
  uint64_t bytes_dropped_total = 0;
  uint64_t bytes_dropped_this_sec = 0;
  uint64_t sleeps_total = 0;
  uint64_t sleeps_this_sec = 0;

  double mb_received_ps = 0;
  double mb_dropped_ps = 0;
  double sleeps_ps = 0;

  while (!quit_threads)
  {

    bytes_received_this_sec = ctx->bytes->received - bytes_received_total;
    bytes_dropped_this_sec  = ctx->bytes->dropped - bytes_dropped_total;
    sleeps_this_sec = ctx->n_sleeps - sleeps_total;

    bytes_received_total = ctx->bytes->received;
    bytes_dropped_total = ctx->bytes->dropped;
    sleeps_total = ctx->n_sleeps;

    mb_received_ps = (double) bytes_received_this_sec / 1000000;
    mb_dropped_ps = (double) bytes_dropped_this_sec / 1000000;

    fprintf(stderr,"total=%5.2f, received=%5.2f Millon B/s\t dropped=%5.2f Million B/s, sleeps=%"PRIu64" usec, ooo packs=%"PRIu64"\n", (mb_received_ps+mb_dropped_ps), mb_received_ps, mb_dropped_ps, sleeps_this_sec, ctx->ooo_packs);
    sleep(1);
  }

}

void signal_handler(int signalValue) {

  fprintf(stderr, "received signal %d\n", signalValue);
  if (quit_threads) {
    fprintf(stderr, "received signal %d twice, hard exit\n", signalValue);
    exit(EXIT_FAILURE);
  }
                
  quit_threads = 1;

}



