/***************************************************************************
 *  
 *    Copyright (C) 2009 by Andrew Jameson
 *    Licensed under the Academic Free License version 2.1
 * 
 ****************************************************************************/

#include <assert.h>
#include <errno.h>

#include "caspsr_udp.h"

caspsr_sock_t * caspsr_init_sock(unsigned size)
{
  caspsr_sock_t * b = (caspsr_sock_t *) malloc(sizeof(caspsr_sock_t));

  assert(b != NULL);

  b->fd = 0;
  b->size = size;
  b->have_packet = 0;
  b->buffer = (char *) malloc(sizeof(char) * size);
  assert(b->buffer != NULL);

  return b;
}

void caspsr_free_sock(caspsr_sock_t* b)
{
  b->fd = 0;
  b->size = 0;
  b->have_packet = 0;
  free(b->buffer);
}


caspsr_data_t * caspsr_init_data(uint64_t size)
{
  caspsr_data_t * b = (caspsr_data_t *) malloc(sizeof(caspsr_data_t));

  assert(b != NULL);
  b->size = size;
  b->count = 0;
  b->min = 0;
  b->max = 0;
  b->buffer = (char *) malloc(sizeof(char) * size);
  assert(b->buffer != NULL);
  b->ids = (uint64_t *) malloc(sizeof(uint64_t) * size);
  assert(b->ids != NULL);

  return b;
}

void caspsr_zero_data(caspsr_data_t * b)
{
  char zerodchar = 'c';
  memset(&zerodchar,0,sizeof(zerodchar));
  memset(b->buffer, zerodchar, b->size);
}

void caspsr_free_data(caspsr_data_t * b)
{
  b->size = 0;
  b->count = 0;
  b->min = 0;
  b->max = 0;
  if (b->buffer) 
    free(b->buffer);
  else 
    fprintf(stderr, "free_caspsr_buffer: b->buffer was unexpectedly freed\n");
  if (b->ids)
    free(b->ids);
  else 
    fprintf(stderr, "free_caspsr_buffer: b->ids was unexpectedly freed\n");
}


/* copy b_length data from s to b, subject to the s_offset, wrapping if
 * necessary and return the new s_offset */
unsigned int caspsr_encode_data(char * b, char * s, unsigned int b_length, 
                              unsigned int s_length, unsigned int s_offset) {
#ifdef _DEBUG
  fprintf(stderr, "caspsr_encode_data: b_length=%d, s_length=%d, s_offset=%d\n",
                  b_length, s_length, s_offset);
#endif

  if (s_offset + b_length > s_length) {

    unsigned part = s_length - s_offset;
    memcpy(b, s + s_offset, part);
    memcpy(b + part, s, b_length-part);
    return (b_length-part);

  } else {

    memcpy(b, s + s_offset, b_length);
    return (s_offset + b_length);

  }

}

void caspsr_decode_header (unsigned char * b, uint64_t *seq_no, uint64_t * ch_id)
{
  /*
  // *seq_no = (uint64_t) (b[0]<<56 | b[1]<<48 | b[2]<<40 | b[3]<<32 | b[4]<<24 | b[5]<<16 | b[6] << 8 | b[7]);
  *seq_no  = UINT64_C (0);
  *seq_no |= (uint64_t) b[0]<<56;
  *seq_no |= (uint64_t) b[1]<<48;
  *seq_no |= (uint64_t) b[2]<<40;
  *seq_no |= (uint64_t) b[3]<<32;
  *seq_no |= (uint64_t) (b[4]<<24 | b[5]<<16 | b[6] << 8 | b[7]);

  // *ch_id = (uint64_t) (b[8]<<56 | b[9]<<48 | b[10]<<40 | b[11]<<32 | b[12]<<24 | b[13]<<16 | b[15] << 8 | b[15]);
  *ch_id  = UINT64_C (0);
  *ch_id |= (uint64_t) b[8]<<56;
  *ch_id |= (uint64_t) b[9]<<48;
  *ch_id |= (uint64_t) b[10]<<40;
  *ch_id |= (uint64_t) b[11]<<32;
  *ch_id |= (uint64_t) b[12]<<24;
  *ch_id |= (uint64_t) (b[13]<<16 | b[15] << 8 | b[15]);
  */
  
  uint64_t tmp = 0; 
  unsigned i = 0;
  *seq_no = UINT64_C (0);
  for (i = 0; i < 8; i++ )
  {
    tmp = UINT64_C (0);
    tmp = b[8 - i - 1];
    *seq_no |= (tmp << ((i & 7) << 3));
  }

  *ch_id = UINT64_C (0);
  for (i = 0; i < 8; i++ )
  {
    tmp = UINT64_C (0);
    tmp = b[16 - i - 1];
    *ch_id |= (tmp << ((i & 7) << 3));
  }
}

void caspsr_encode_header (char *b, uint64_t seq_no, uint64_t ch_id)
{

  b[0] = (uint8_t) (seq_no>>56);
  b[1] = (uint8_t) (seq_no>>48);
  b[2] = (uint8_t) (seq_no>>40); 
  b[3] = (uint8_t) (seq_no>>32);
  b[4] = (uint8_t) (seq_no>>24);
  b[5] = (uint8_t) (seq_no>>16);
  b[6] = (uint8_t) (seq_no>>8); 
  b[7] = (uint8_t) (seq_no);

  b[8]  = (uint8_t) (ch_id>>56);
  b[9]  = (uint8_t) (ch_id>>48);
  b[10] = (uint8_t) (ch_id>>40); 
  b[11] = (uint8_t) (ch_id>>32);
  b[12] = (uint8_t) (ch_id>>24);
  b[13] = (uint8_t) (ch_id>>16);
  b[14] = (uint8_t) (ch_id>>8); 
  b[15] = (uint8_t) (ch_id);

  /*
  size_t i;
  unsigned char ch;
  for (i = 0; i < bytes; i++ )
  {
    ch = (num >> ((i & 7) << 3)) & 0xFF;
    if (type == LITTLE)
      arr[i] = ch;
    else if (type == BIG)
      arr[bytes - i - 1] = ch;
  }

  uint64ToByteArray (h->seq_no, (size_t) 8, (unsigned char*) b, (int) BIG);
  uint64ToByteArray (h->ch_id,  (size_t) 8, (unsigned char*) b+8, (int) BIG);
  */

}

/*
 *  Initiliazes data structures and parameters for the receiver 
 */
int caspsr_receiver_init(caspsr_receiver_t * ctx, unsigned i_dest, unsigned n_dest, unsigned n_packets,
                        unsigned n_append )
{

  multilog_t* log = ctx->log;

  /* intialize our data structures */
  ctx->packets = init_stats_t();
  ctx->bytes   = init_stats_t();
  ctx->sock    = caspsr_init_sock(UDP_PAYLOAD);
  ctx->curr    = caspsr_init_data(UDP_DATA * UDP_NPACKS);
  ctx->next    = caspsr_init_data(UDP_DATA * UDP_NPACKS);

  ctx->sock->fd = dada_udp_sock_in(ctx->log, ctx->interface, ctx->port, ctx->verbose);
  if (ctx->sock->fd < 0) {
    multilog (log, LOG_ERR, "failed to create udp socket %s:%d\n", ctx->interface, ctx->port);
    return 0;
  }

  /* set the socket size to 64 MB */
  dada_udp_sock_set_buffer_size (log, ctx->sock->fd, ctx->verbose, 67108864);

  /* set the socket to non-blocking */
  sock_nonblock(ctx->sock->fd);

  /* Intialize the stopwatch timer */
  RealTime_Initialise(1);
  StopWatch_Initialise(1);

  /* 10 usecs between packets */
  ctx->timer_sleep = 10;
  ctx->timer = (StopWatch *) malloc(sizeof(StopWatch));

  /* xfer spacing */
  ctx->xfer_bytes   = (n_packets + n_append) * UDP_DATA;
  ctx->xfer_offset  = i_dest * n_packets;
  ctx->xfer_gap     = n_packets * n_dest;
  ctx->xfer_packets = n_packets + n_append; 
  ctx->xfer_s_seq  = 0;
  ctx->xfer_e_seq  = 0;
  ctx->xfer_count  = 0;
  ctx->xfer_ending = 0;

  if (ctx->verbose)
    multilog(ctx->log, LOG_INFO, "xfer_bytes=%"PRIu64", xfer_offset=%"PRIu64", xfer_gap=%"PRIu64", xfer_packets=%"PRIu64"\n", ctx->xfer_bytes, ctx->xfer_offset, ctx->xfer_gap, ctx->xfer_packets);
  
}

int caspsr_receiver_dealloc(caspsr_receiver_t * ctx)
{
  close(ctx->sock->fd);
  caspsr_free_data(ctx->curr);
  caspsr_free_data(ctx->next);
  caspsr_free_sock(ctx->sock);

}

/* 
 * start an xfer, setting the expected start and end sequence numbers, incremement xfer count
 * and return the expected starting byte
 */
uint64_t caspsr_start_xfer(caspsr_receiver_t * ctx) 
{

  caspsr_zero_data(ctx->curr);
  caspsr_zero_data(ctx->next);

  /* offset based on this dest index */
  ctx->xfer_s_seq = ctx->xfer_offset;

  /* offset based on the number of xfers */
  ctx->xfer_s_seq += ctx->xfer_count * ctx->xfer_gap;
  ctx->xfer_e_seq  = ctx->xfer_s_seq + ctx->xfer_packets;

  if (ctx->verbose)
    multilog(ctx->log, LOG_INFO, "xfer: %"PRIu64", s_seq=%"PRIu64", e_seq=%"PRIu64", bytes=%"PRIu64"\n", 
                 ctx->xfer_count, ctx->xfer_s_seq, ctx->xfer_e_seq, ctx->xfer_bytes);

  ctx->sod = 0;
  ctx->xfer_count++;
  ctx->xfer_ending = 0;

  return (ctx->xfer_s_seq * UDP_DATA);

}

/* 
 * receive size packets and return a pointer to the captured data, updating size
 * with the number of packets captured
 */
void * caspsr_xfer(caspsr_receiver_t * ctx, uint64_t * size)
{

  ctx->got_enough = 0;

  /* pointer to start of socket buffer */
  unsigned char * arr;

  /* Flag for timeout */
  int timeout_ocurred = 0;

  /* How much data has actaully been received */
  uint64_t data_received = 0;

  /* sequence number of the packet */
  uint64_t seq_no = 0;

  /* ch_id of the packet */
  uint64_t ch_id = 0;

  /* for decoding sequence number */
  uint64_t tmp = 0;
  unsigned i = 0;

  /* switch the data buffer pointers */
  ctx->temp = ctx->curr;
  ctx->curr = ctx->next;
  ctx->next = ctx->temp;

  /* update the counters for the next buffer */
  ctx->next->count = 0;
  ctx->next->min = ctx->curr->max + 1;
  ctx->next->max = ctx->next->min + UDP_NPACKS -1;

  /* TODO check the performance hit of doing this */
  caspsr_zero_data(ctx->next);

  /* Determine the sequence number boundaries for curr and next buffers */
  int errsv;

  /* Assume we will be able to return a full buffer */
  *size = (UDP_DATA * UDP_NPACKS);

  /* Continue to receive packets */
  while (!ctx->got_enough) {

    /* If there is a packet in the buffer from the previous call */
    if (ctx->sock->have_packet) {

      ctx->sock->have_packet = 0;

    /* Get a packet from the socket */
    } else {

      ctx->sock->have_packet = 0;
      ctx->timer_count = 0;

      while (!ctx->sock->have_packet && !ctx->got_enough) {

        /* to control how often we check for packets, and accurate timeout */
        StopWatch_Start(ctx->timer);

        /* Get a packet from the (non-blocking) socket */
        ctx->sock->got = recvfrom (ctx->sock->fd, ctx->sock->buffer, UDP_PAYLOAD, 0, NULL, NULL);

        /* If we successfully got a packet */
        if (ctx->sock->got == UDP_PAYLOAD) {

          ctx->sock->have_packet = 1;
          StopWatch_Stop(ctx->timer);

        /* If no packet at the socket */
        } else if (ctx->sock->got == -1) {

          errsv = errno;

          if (errsv == EAGAIN) {
            /* Busy sleep for 10us */
            ctx->timer_count += (uint64_t) ctx->timer_sleep;
            StopWatch_Delay(ctx->timer, ctx->timer_sleep);

          } else {
            fprintf(stderr, "recvfrom failed: %s\n", strerror(errsv));
            ctx->got_enough = 1;
            StopWatch_Stop(ctx->timer);
          }

        } else {

          fprintf(stderr, "got %d, expected %d\n", ctx->sock->got, UDP_PAYLOAD);
          StopWatch_Stop(ctx->timer);

        }

        if (ctx->xfer_ending && ctx->timer_count >= 10000) {
          if (ctx->verbose) 
            fprintf(stderr, "xfer ending and no packet received for 10ms\n");
          ctx->got_enough = 1;
        }

        if (ctx->timer_count >= 100000) {

          /* if we are waiting the first packet of an xfer */
          if (!ctx->sod) {
            if (ctx->xfer_count <= 1) 
              ctx->timer_count = 0;
            else if (ctx->timer_count > 5000000) {
              ctx->got_enough = 1;
            } else 
              ;
          } else {
            fprintf(stderr, "no packet received in 100 ms\n");
            ctx->got_enough = 1;
          }
        }
      }
    }

    /* check that the packet is of the correct size */
    if (ctx->sock->have_packet && ctx->sock->got != UDP_PAYLOAD) {
      fprintf(stderr, "Received a packet of unexpected size\n");
      ctx->got_enough = 1;
    }

    /* If we did get a packet within the timeout */
    if (!ctx->got_enough && ctx->sock->have_packet) {

      /* Decode the packets apsr specific header */
      arr = ctx->sock->buffer;
      seq_no = UINT64_C (0);
      for (i = 0; i < 8; i++ )
      {
        tmp = UINT64_C (0);
        tmp = arr[8 - i - 1];
        seq_no |= (tmp << ((i & 7) << 3));
      }

      seq_no /= 1024;

      /* If we are waiting for the first packet */
      if (! ctx->sod) {
        ctx->sod = 1;        
        fprintf(stderr,"START : received packet %"PRIu64"\n", seq_no);

        /* update the min/max sequence numbers for the receiving buffers */
        ctx->curr->min = ctx->xfer_s_seq;
        ctx->curr->max = ctx->curr->min + UDP_NPACKS - 1;
        ctx->next->min = ctx->curr->max + 1;
        ctx->next->max = ctx->next->min + UDP_NPACKS - 1;

      } 

      /* check for nearing the end of an xfer */
      if (ctx->xfer_e_seq - seq_no < 100) {
        if ((!ctx->xfer_ending) && (ctx->verbose)) {
          fprintf(stderr, "xfer %"PRIu64" ending soon\n", ctx->xfer_count);
        } 
        ctx->xfer_ending = 1;
      }

      /* Increment statistics */
      ctx->packets->received++;
      ctx->packets->received_per_sec++;
      ctx->bytes->received += UDP_DATA;
      ctx->bytes->received_per_sec += UDP_DATA;

      /* we are going to process the packet we have */
      ctx->sock->have_packet = 0;

      if (seq_no < ctx->curr->min) {
        fprintf(stderr, "Packet underflow %"PRIu64" < min (%"PRIu64")\n",
                  seq_no, ctx->curr->min);

      } else if (seq_no <= ctx->curr->max) {
        memcpy( ctx->curr->buffer + (seq_no - ctx->curr->min) * UDP_DATA,
                ctx->sock->buffer + 16,
                UDP_DATA);
        ctx->curr->count++;

      } else if (seq_no <= ctx->next->max) {

        memcpy( ctx->next->buffer + (seq_no - ctx->next->min) * UDP_DATA,
                ctx->sock->buffer + 16,
                UDP_DATA);
        ctx->next->count++;

      } else {

        fprintf(stderr, "2: Not keeping up. curr %5.2f%, next %5.2f%\n",
                 ((float) ctx->curr->count / (float) UDP_NPACKS)*100,
                 ((float) ctx->next->count / (float) UDP_NPACKS)*100);

        ctx->sock->have_packet = 1;
        ctx->got_enough = 1;
      }

      /* If we have filled the current buffer, then we can stop */
      if (ctx->curr->count >= UDP_NPACKS)
        ctx->got_enough = 1;

      /* if the next buffer is 25% full, then we can stop */
      if (ctx->next->count >= (UDP_NPACKS * 0.50)) {
        ctx->got_enough = 1;
        if (ctx->verbose)
          fprintf(stderr, "next data buffer > 50%% full\n");
      }

    }
  }

  if (ctx->xfer_ending) {
    *size = ctx->curr->count * UDP_DATA;
    if (ctx->verbose)
      multilog(ctx->log, LOG_INFO, "EoXFER curr_count=%"PRIu64", next_count=%"PRIu64"\n",
               ctx->curr->count, ctx->next->count);

  } else {

    /* Some checks before returning */
    if (ctx->curr->count) {

      if ((ctx->curr->count < UDP_NPACKS) && !(ctx->timer_count >= 100000)) {
        multilog (ctx->log, LOG_WARNING, "Dropped %"PRIu64" packets\n",
                 (UDP_NPACKS - ctx->curr->count));

        ctx->packets->dropped += (UDP_NPACKS - ctx->curr->count);
        ctx->packets->dropped_per_sec += (UDP_NPACKS - ctx->curr->count);
        ctx->bytes->dropped += (UDP_DATA * (UDP_NPACKS - ctx->curr->count));
        ctx->bytes->dropped_per_sec += (UDP_DATA * (UDP_NPACKS - ctx->curr->count));
    
      }

    /* If the timeout ocurred, this is most likely due to end of data */
    if (ctx->timer_count >= 100000) {
      *size = ctx->curr->count * UDP_DATA;
      multilog(ctx->log, LOG_WARNING, "Suspected EOD received, returning %"PRIu64" bytes\n",*size);
    }

    } else {
      *size = 0;
    }
  }
  assert(ctx->curr->buffer != 0);

  return (void *) ctx->curr->buffer;

}

/* 
 * stop the current xfer
 */
int caspsr_stop_xfer(caspsr_receiver_t * ctx)
{
  return 0;
}
