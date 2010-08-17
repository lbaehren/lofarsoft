/*
 * caspsr_udpNdb. Reads UDP packets and checks the header for correctness
 */


#include <sys/socket.h>
#include <math.h>
#include <pthread.h>

#include "sock.h"
#include "caspsr_udpNdb.h"
#include "dada_generator.h"

//#define _DEBUG 1
#define CASPSR_UDPNDB_PACKS_PER_XFER 1000

/* global variables */
int quit_threads = 0;

void usage()
{
  fprintf (stdout,
	   "caspsr_udpNdb [options] key1 [keyi]\n"
     " -h             print help text\n"
	   " -i iface       interface for UDP packets [default all interfaces]\n"
	   " -p port        port to open for UDP packets [default %d]\n"
     " -H file        load header from file\n"
     " -t secs        acquire for secs\n"
     " -b num         write num packets to each datablock [default %d]\n"
     " -d             run as a daemon\n"
     " -v             verbose messages\n"
     "\n"
     "key1 - keyi     existing datablocks to write to\n",
     CASPSR_DEFAULT_UDPNDB_PORT, CASPSR_UDPNDB_PACKS_PER_XFER );
}


time_t udpNdb_start_function (udpNdb_t * ctx)
{

  fprintf(stderr, "udpNdb_start_function: opening data blocks\n");

  /* connect to all the datablocks */
  if (udpNdb_open_datablocks(ctx) < 0) {
    fprintf(stderr, "udpNdb_start_function: udpNdb_open_datablocks FAILED\n");
    udpNdb_close_datablocks(ctx);
    return -1;
  }

  /* allocate required memory strucutres */
  ctx->packets = init_stats_t();
  ctx->bytes   = init_stats_t();

  /* create a zerod packet for padding the datablocks if we drop packets */
  ctx->zeroed_packet = (char *) malloc(sizeof(char) * UDP_PAYLOAD);
  char zeroed_char = 'c';
  memset(&zeroed_char, 0, sizeof(zeroed_char));
  memset(ctx->zeroed_packet, zeroed_char, UDP_PAYLOAD);

  fprintf(stderr, "udpNdb_start_function: opening socket\n");
  /* open sockets */
  ctx->fd = dada_udp_sock_in(ctx->log, ctx->interface, ctx->port, ctx->verbose);
  if (ctx->fd < 0) {
    multilog (ctx->log, LOG_ERR, "Error, Failed to create udp socket\n");
    return 0;
  }

  fprintf(stderr, "udpNdb_start_function: setting socket buffer size\n");
  /* set the socket size to 64 MB */
  dada_udp_sock_set_buffer_size (ctx->log, ctx->fd, ctx->verbose, 67108864);

  /* set the socket to non-blocking */
  sock_nonblock(ctx->fd);

  /* setup the next_seq to the initial value */
  ctx->next_seq = 0;
  ctx->n_sleeps = 0;
  ctx->ooo_packets = 0;

  fprintf(stderr, "udpNdb_start_function: returning\n");
  /* set the socket size to 64 MB */

  return 0;
}


/* 
 * open each data block and write the header 
 */
int udpNdb_open_datablocks(udpNdb_t * ctx) 
{

  uint64_t block_size = 0;
  unsigned i = 0;
  udpNdb_receiver_t * r;

  fprintf(stderr, "opening for %d receivers\n", ctx->n_receivers);

  for (i=0; i<ctx->n_receivers; i++) 
  {
    fprintf(stderr, "opening i=%d\n",i);
    r = ctx->receivers[i];

#ifdef _DEBUG    
    fprintf(stderr, "connecting to %x HDU\n", r->key);
#endif

    r->hdu = dada_hdu_create (ctx->log);

    dada_hdu_set_key(r->hdu, r->key);

    if (dada_hdu_connect (r->hdu) < 0)
    {
      multilog(ctx->log, LOG_ERR, "failed to connect to %x HDU\n", r->key);
      return -1;
    }

    /* only lock_write the first HDU */
    if (i == 0) 
    {

      if (dada_hdu_lock_write (r->hdu) < 0)
      {
        multilog(ctx->log, LOG_ERR, "failed to lock_write %x HDU\n", r->key);
        return -1;
      }

      /* ensure datablock size is a multiple of packet size */
      block_size = ipcbuf_get_bufsz ((ipcbuf_t*) r->hdu->data_block);
      if (block_size % UDP_PAYLOAD != 0) 
      {
        multilog(ctx->log, LOG_ERR, "block_size (%"PRIu64") of %x HDU was not a multiple of packet length (%d)\n", block_size, UDP_PAYLOAD);
        return -1;
      }
    }

  }

  return 0;
}     


/* 
 * Close all the datablocks 
 */
int udpNdb_close_datablocks(udpNdb_t * ctx) 
{

  unsigned i=0;
  udpNdb_receiver_t * r;
  
  for (i=0; i<ctx->n_receivers; i++)
  {
    r = ctx->receivers[i];

#ifdef _DEBUG    
    fprintf(stderr, "closing %x HDU\n", r->key);
#endif

    if (r->hdu) 
    {
      if (dada_hdu_unlock_write (r->hdu) < 0)
        multilog(ctx->log, LOG_ERR, "could not unlock_write on  %x HDU\n", r->key);

      if (dada_hdu_disconnect (r->hdu) < 0)
        multilog(ctx->log, LOG_ERR, "could not disconnect from %x HDU\n", r->key);
    }

  }

  return 0;

}


/* 
 *   Switches to the next datablock in array 
 */
int udpNdb_new_datablock(udpNdb_t * ctx)
{

  dada_hdu_t * hdu = ctx->receivers[ctx->receiver_i]->hdu;

  //  signal end of data on Data Block
  if (dada_hdu_unlock_write(hdu) < 0) 
  {
    multilog (ctx->log, LOG_ERR, "Could not unlock_write on HDU %x\n", ctx->receivers[ctx->receiver_i]->key);
    return -1;
  }
  
  // advance to the next data block
  ctx->receiver_i++;
  ctx->receiver_i = ctx->receiver_i % ctx->n_receivers;

  hdu = ctx->receivers[ctx->receiver_i]->hdu;
  if (dada_hdu_lock_write(hdu) < 0)
  {
    multilog (ctx->log, LOG_ERR, "Could not lock_write on HDU %x\n", ctx->receivers[ctx->receiver_i]->key);
    return -1;
  }

  return 0;
}


/* 
 *   Writes the header to the datablock 
 */
int udpNdb_new_header(udpNdb_t * ctx)
{

  dada_hdu_t* hdu = ctx->receivers[ctx->receiver_i]->hdu;

  char *   hdr_buf = 0;
  uint64_t hdr_size = 0;

  hdr_size = ipcbuf_get_bufsz ((ipcbuf_t*) hdu->header_block);

  if (ctx->header_size < hdr_size) {
    multilog(ctx->log, LOG_WARNING, "hdr size mismatch, block=%d, header=%"PRIu64"\n", ctx->header_size, hdr_size);
    return -1;
  }

#ifdef _DEBUG
  fprintf(stderr, "udpNdb_new_header: header_block size=%"PRIu64", \n", hdr_size);
#endif

  hdr_buf = ipcbuf_get_next_write (hdu->header_block);

  memcpy (hdr_buf, ctx->header, ctx->header_size);

#ifdef _DEBUG
  fprintf(stderr, "udpNdb_new_header: setting OBS_OFFSET to %"PRIu64"\n", ctx->obs_offset);
#endif

  if (ascii_header_set (hdr_buf, "OBS_OFFSET", "%"PRIu64, ctx->obs_offset) < 0) {
    multilog (ctx->log, LOG_ERR, "failed ascii_header_set OBS_OFFSET\n");
    return -1;
  }

  if (ipcbuf_mark_filled (hdu->header_block, ctx->header_size) < 0)  {
    multilog (ctx->log, LOG_ERR, "Could not mark filled header block\n");
    return -1;
  }

#ifdef _DEBUG
  multilog (ctx->log, LOG_INFO, "header %x marked filled\n", ctx->receivers[ctx->receiver_i]->key);
#endif

  return 0;
}


/*
 * Read data from the UDP socket and write to the ring buffers
 */
int udpNdb_main_function (udpNdb_t * ctx, int64_t total_bytes)
{

#ifdef _DEBUG 
  fprintf(stderr, "udpNdb_main_function(%"PRIi64")\n", total_bytes);
#endif

  multilog_t * log = ctx->log;

  /* datablock pointer */
  ipcio_t* ipc = 0;

  /* pointer for header decode function */
  unsigned char * arr;

  /* sequence number of the decoded packet */
  uint64_t seq_no = 0;

  /* used to decode sequence number */
  uint64_t tmp = 0;

  /* Flag for timeout */
  int timeout_ocurred = 0;

  /* flag for ignoring packets */
  unsigned ignore_packet = 0;

  /* flag for having a packet */
  unsigned have_packet = 0;

  /* data received from a recv_from call*/
  size_t got = 0;

  /* offset for packet loss calculation */
  uint64_t offset = 0;

  /* Determine the sequence number boundaries for curr and next buffers */
  int errsv;

  unsigned i = 0;

  /* choose the first datablock and set the ipc ptr */
  ctx->receiver_i = 0;
  ipc = ctx->receivers[ctx->receiver_i]->hdu->data_block;

  /* mark the first header as filled */
  if ( udpNdb_new_header (ctx) < 0 )  
  {
    multilog (ctx->log, LOG_ERR, "Failed to write header\n");
    return -1;
  }

  /* Continue to receive packets */
  while (ctx->bytes->received < total_bytes && !quit_threads) {

    if (ipc->rdwrt != 'W' && ipc->rdwrt != 'w') {
      fprintf (stderr, "ipcio_write: invalid ipcio_t (%c) [%d]\n",ipc->rdwrt, ctx->receiver_i);
      return -1;
    } 
  
    /* check if the current buffer is full */
    if (ipc->bytes == ipcbuf_get_bufsz((ipcbuf_t*)ipc)) {

      /* if it is full, but not marked as so, then mark it as full */
      if (!ipc->marked_filled) {

#ifdef _DEBUG
        fprintf (stderr, "ipcio_write buffer:%"PRIu64" mark_filled\n", ipc->buf.sync->w_buf);
#endif

        /* the buffer has been filled */
        if (ipcbuf_mark_filled ((ipcbuf_t*)ipc, ipc->bytes) < 0) {
          fprintf (stderr, "ipcio_write: error ipcbuf_mark_filled\n");
          return -1;
        }

        if (ipcio_check_pending_sod (ipc) < 0) {
          fprintf (stderr, "ipcio_write: error ipcio_check_pending_sod\n");
          return -1;
        }
      }

      ipc->curbuf = 0;
      ipc->bytes = 0;
      ipc->marked_filled = 1;

    }

    /* if we need to get the next buffer */
    if (!ipc->curbuf) {

#ifdef _DEBUG
      fprintf (stderr, "ipcio_write buffer:%"PRIu64" ipcbuf_get_next_write\n", ipc->buf.sync->w_buf);
#endif

      ipc->curbuf = ipcbuf_get_next_write ((ipcbuf_t*)ipc);

#ifdef _DEBUG
      fprintf (stderr, "ipcio_write: ipcbuf_get_next_write returns\n");
#endif

      if (!ipc->curbuf) {
        fprintf (stderr, "ipcio_write: ipcbuf_next_write failed to return a new buffer to write to\n");
        return -1;
      }

      ipc->marked_filled = 0;
      ipc->bytes = 0;
    }

#ifdef _DEBUG
    //fprintf (stderr, "ipcio_write buffer:%"PRIu64" offset:%"PRIu64"\n", ipc->buf.sync->w_buf, ipc->bytes);
#endif

    /* incredibly tight loop to try and get a packet */
    have_packet = 0; 
    while (! have_packet )
    {

      got = recvfrom (ctx->fd, ipc->curbuf + ipc->bytes, UDP_PAYLOAD, 0, NULL, NULL);

      //fprintf(stderr, "got=%d\n",got);

      /* if we received a packet as expected */
      if (got == UDP_PAYLOAD) {

        have_packet = 1;
        ignore_packet = 0;

      /* a problem ocurred, most likely no packet at the non-blocking socket */
      } else if (got == -1) {

        errsv = errno;

        if (errsv == EAGAIN) {

          /* Busy sleep for the half the time between packets 
           * at 204800 packets/sec ths is around 5/2 usec */
          //ctx->timeout.tv_sec = 0;
          //ctx->timeout.tv_usec = 1;
          //select(0, NULL, NULL, NULL, &(ctx->timeout));
          ctx->n_sleeps++;

        } else {

          multilog (log, LOG_ERR, "recvfrom failed: %s\n", strerror(errsv));
          return -1;

        }

      /* we received a packet of the WRONG size, ignore it */
      } else {

        multilog (log, LOG_ERR, "Received %d bytes, expected %d\n",
                                got, UDP_PAYLOAD);
        ignore_packet = 1;

      }
    }

    /* If we did get a packet within the timeout */
    if (have_packet) {

      /* set this pointer to the start of the buffer */
      arr = ipc->curbuf + ipc->bytes;

      /* Decode the packets apsr specific header */
      seq_no = UINT64_C (0);
      for (i = 0; i < 8; i++ )
      {
        tmp = UINT64_C (0);
        tmp = arr[8 - i - 1];
        seq_no |= (tmp << ((i & 7) << 3));
      }
      seq_no /= 2048;

      /* If we are waiting for the first packet */
      if ( (ctx->next_seq == 0) && (ctx->bytes->received == 0) ) {

        if (seq_no < 10000) {

//#ifdef _DEBUG
          fprintf(stderr,"START : received packet %"PRIu64"\n", seq_no);
//#endif
          ctx->next_seq = 0;
        } else {
          ignore_packet = 1;
        }
      }
    }


    /* If we are still waiting for the start of data */
    if (!ignore_packet) {

      if (seq_no > ctx->next_seq)
        ctx->ooo_packets += seq_no - ctx->next_seq;

      /* we are going to process the packet we have */
      have_packet = 0;

      /* expected packet */
      if (seq_no == ctx->next_seq) {

        ipc->bytes += UDP_PAYLOAD;
        ctx->packets_this_xfer++;
        ctx->obs_offset += UDP_DATA;

        ctx->packets->received++;
        ctx->bytes->received += UDP_PAYLOAD;

        ctx->next_seq++;

      /* packet is too early in the sequence */
      } else if (seq_no > ctx->next_seq) {

        offset = seq_no - ctx->next_seq;
        //multilog (log, LOG_WARNING, "Zeroing %"PRIu64" packets into datablock\n", offset);

        for (i=0; i<offset; i++) 
        {

          caspsr_encode_header(ctx->zeroed_packet, seq_no+i, 0);
          //uint64ToByteArray (seq_no+i, (size_t) 8, (unsigned char*) ctx->zeroed_packet, (int) BIG);
          ipcio_write (ipc, ctx->zeroed_packet, UDP_PAYLOAD);

          ctx->packets_this_xfer++;
          ctx->obs_offset += UDP_DATA;

          /* check for a change of data block */
          if (ctx->packets_this_xfer >= ctx->packets_per_xfer)  
          {
            fprintf(stderr, "1: ctx->packets_this_xfer[%"PRIu64"] >= ctx->packets_per_xfer[%"PRIu64"]\n", ctx->packets_this_xfer, ctx->packets_per_xfer);
            if ( udpNdb_new_datablock(ctx) < 0) 
            {
              multilog (ctx->log, LOG_ERR, "Failed to move to the next datablock\n");
              return -1;
            }

            /* update to the new datablock */
            ipc = ctx->receivers[ctx->receiver_i]->hdu->data_block;
 
            if ( udpNdb_new_header (ctx) < 0 )
            {
              multilog (ctx->log, LOG_ERR, "Failed to write new header\n");
              return -1;
            }
            ctx->packets_this_xfer = 0;
          }
        }

        ctx->packets->received += offset;
        ctx->packets->dropped += offset;
        ctx->bytes->received += offset * UDP_PAYLOAD;
        ctx->bytes->dropped += offset * UDP_PAYLOAD;

        ctx->next_seq += offset+1;

      /* packet is to late, already been zerod */
      } else {

      }

      /* check for a change of data block */
      if (ctx->packets_this_xfer >= ctx->packets_per_xfer) 
      {
        fprintf(stderr, "2: ctx->packets_this_xfer[%"PRIu64"] >= ctx->packets_per_xfer[%"PRIu64"]\n", ctx->packets_this_xfer, ctx->packets_per_xfer);
        
        if ( udpNdb_new_datablock(ctx) < 0)
        {
          multilog (ctx->log, LOG_ERR, "Failed to move to the next datablock\n");
          return -1;
        }
    
        /* update to the new datablock */
        ipc = ctx->receivers[ctx->receiver_i]->hdu->data_block;

        if ( udpNdb_new_header (ctx) < 0 )
        {
          multilog (ctx->log, LOG_ERR, "Failed to write new header\n");
          return -1;
        }

        ctx->packets_this_xfer = 0;
      }
    }
  }

  return 0;
}

/*
 * Close the udp socket and file
 */

int udpNdb_stop_function (udpNdb_t* ctx)
{

  multilog_t *log = ctx->log;

  /* get our context, contains all required params */
  if (ctx->packets->dropped && ctx->next_seq>0) {
    double percent = (double) ctx->packets->dropped / (double) ctx->next_seq;
    percent *= 100;

    multilog(log, LOG_INFO, "packets dropped %"PRIu64" / %"PRIu64 " = %8.6f %\n"
             ,ctx->packets->dropped, ctx->next_seq, percent);

  }

  close(ctx->fd);
  return 0;

}


int main (int argc, char **argv)
{

  /* DADA Logger */ 
  multilog_t* log = 0;

  /* Interface on which to listen for udp packets */
  char * interface = "any";

  /* port for UDP packets */
  int u_port = CASPSR_DEFAULT_UDPNDB_PORT;

  /* multilog output port */
  int l_port = CASPSR_DEFAULT_PWC_LOGPORT;

  /* Flag set in daemon mode */
  char daemon = 0;

  /* Flag set in verbose mode */
  char verbose = 0;

  /* number of seconds/bytes to acquire for */
  unsigned nsecs = 0;

  /* hexadecimal shared memory key */
  key_t dada_key = DADA_DEFAULT_BLOCK_KEY;

  /* actual struct with info */
  udpNdb_t udpNdb;

  /* custom header from a file, implies no controlling pwcc */
  char * header_file = NULL;

  /* Pointer to array of "read" data */
  char *src;

  /* Ignore dropped packets */
  unsigned ignore_dropped = 0;

  /* number of packets to write to each datablock */
  int packets_per_xfer = CASPSR_UDPNDB_PACKS_PER_XFER;

  int arg = 0;

  while ((arg=getopt(argc,argv,"b:dH:i:l:p:t:vh")) != -1) {
    switch (arg) {

    case 'b':
      packets_per_xfer = atoi(optarg);
      break;

    case 'd':
      daemon = 1;
      break; 
  
    case 'H':
      if (optarg) {
        header_file = optarg;
      } else {
        fprintf(stderr,"-H flag requires a file arguement\n");
        usage();
        exit(EXIT_FAILURE);
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

    case 'p':
      u_port = atoi (optarg);
      break;

    case 't':
      nsecs = atoi (optarg);
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

  int num_args = argc-optind;
  int i = 0;

  if (num_args < 1 || num_args > 16) {
    fprintf(stderr, "ERROR: must have at between 1 and 16 data blocks\n\n");
    usage();
    exit(EXIT_FAILURE);
  }

  log = multilog_open ("caspsr_udpNdb", 0);

  if (daemon)
    be_a_daemon ();
  else
    multilog_add (log, stderr);

  multilog_serve (log, l_port);

  /* allocate receiver data strutures */
  udpNdb.receivers = (udpNdb_receiver_t **) malloc(sizeof(udpNdb_receiver_t*) * num_args);
  if (!udpNdb.receivers) {
    fprintf(stderr, "Could not allocate memory\n");
    exit(EXIT_FAILURE);
  }

  /* parse datablock keys from the command line */
  fprintf(stderr, "for(i=%d, i<%d, i++)\n", optind, num_args);
  unsigned j=0;
  for (i=optind; i<optind+num_args; i++) {
    udpNdb.receivers[j] = (udpNdb_receiver_t *) malloc(sizeof(udpNdb_receiver_t));

    if (sscanf (argv[i], "%x", &(udpNdb.receivers[j]->key)) != 1) {
      fprintf(stderr, "failed to parse db key\n");
      free(udpNdb.receivers);
      exit(EXIT_FAILURE);
    }
    j++;
  }

  /* Setup context information */
  udpNdb.log = log;
  udpNdb.verbose = verbose;
  udpNdb.port = u_port;
  udpNdb.interface = strdup(interface);
  udpNdb.packets_per_xfer = packets_per_xfer;
  udpNdb.packets_this_xfer = 0;
  udpNdb.n_receivers = num_args;
  udpNdb.obs_offset = 0;

  /* allocate some space for a header */
  udpNdb.header_size = DADA_DEFAULT_HEADER_SIZE;
  udpNdb.header = (char *) malloc(sizeof(char) * udpNdb.header_size);

  fprintf(stdout,"caspsr_udpNdb: reading custom header %s\n", header_file);
  if (fileread (header_file, udpNdb.header, udpNdb.header_size) < 0)  {
    multilog (log, LOG_ERR, "could not read header from %s\n",
              header_file);
    return EXIT_FAILURE;
  }

  multilog(log, LOG_INFO, "starting stats_thread()\n");

  pthread_t stats_thread_id;
  if (pthread_create (&stats_thread_id, 0, (void *) stats_thread, (void *) &udpNdb) < 0) {
    perror ("Error creating new thread");
    return -1;
  }

  signal(SIGINT, signal_handler);

  multilog(log, LOG_INFO, "udpNdb_start_function()\n");

  time_t utc = udpNdb_start_function(&udpNdb);
  if (utc == -1 ) {
    multilog(log, LOG_ERR, "Could not run start function\n");
    return EXIT_FAILURE;
  } 


  /* -1 indicates dont stop baby */
  int64_t bytes_to_acquire = -1;
  if (nsecs) {
    bytes_to_acquire = 800 * 1000 * 1000 * (int64_t) nsecs;
    fprintf(stderr, "bytes_to_acquire = %"PRIu64" Million Bytes, nsecs=%d\n", bytes_to_acquire/1000000, nsecs);
  }

  if ( udpNdb_main_function(&udpNdb, bytes_to_acquire) < 0 ){
    fprintf(stderr, "udpNdb_main_function failed\n");
  }

  quit_threads = 1;

  /* join threads */
  void* result = 0;
  fprintf(stderr, "joining stats_thread\n");
  pthread_join (stats_thread_id, &result);

  fprintf(stderr, "udpNdb_stop_function\n");
  if ( udpNdb_stop_function(&udpNdb) != 0)
    fprintf(stderr, "Error stopping acquisition");

  return EXIT_SUCCESS;

}

/* 
 *  Thread to print simple capture statistics
 */
void stats_thread(void * arg) {

  fprintf(stderr, "stats starting\n");
  udpNdb_t * ctx = (udpNdb_t *) arg;

  uint64_t bytes_received_total = 0;
  uint64_t bytes_received_this_sec = 0;
  uint64_t bytes_dropped_total = 0;
  uint64_t bytes_dropped_this_sec = 0;
  uint64_t sleeps_total = 0;
  uint64_t sleeps_this_sec = 0;

  double mb_received_ps = 0;
  double mb_dropped_ps = 0;
  double sleeps_ps = 0;
  double avg_mb_receiver_ps = 0;
  double ps = 0;

  sleep(5);
  fprintf(stderr, "stats while starting\n");

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

    if (bytes_received_total) {
      ps++;
      avg_mb_receiver_ps = (double) ((bytes_received_total + bytes_dropped_total)/1000000) / ps;
    }
      

    fprintf(stderr,"avg=%5.2f, total=%5.2f, received=%5.2f Millon B/s\t dropped=%5.2f Million B/s, sleeps=%"PRIu64" usec, ooo packs=%"PRIu64"\n", avg_mb_receiver_ps, (mb_received_ps+mb_dropped_ps), mb_received_ps, mb_dropped_ps, sleeps_this_sec, ctx->ooo_packets);
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


