/***************************************************************************
 *  
 *    Copyright (C) 2009 by Andrew Jameson
 *    Licensed under the Academic Free License version 2.1
 * 
 ****************************************************************************/

#include <assert.h>
#include <errno.h>

#include "gmrt_udp.h"

gmrt_sock_t * gmrt_init_sock(unsigned n_buffers, unsigned size)
{
  gmrt_sock_t * b = (gmrt_sock_t *) malloc(sizeof(gmrt_sock_t));

  assert(b != NULL);

  b->fd = 0;
  b->size = size;
  b->have_packet = 0;

  b->buffer = (char *) malloc(sizeof(char) * size);
  assert(b->buffer != NULL);
	
	b->buf_ptr = (int32_t *) (b->buffer + 16);

	b->n_buffers = n_buffers;
	
	b->buffers = (char **) malloc(sizeof(char *) * n_buffers);
	assert(b->buffers != NULL);

	b->buf_ptrs = (int32_t **) malloc(sizeof(int32_t *) * n_buffers);
	b->udpdata_per_buf = UDP_DATA / n_buffers;

	unsigned i = 0;
	for (i=0; i<n_buffers; i++)
	{
		b->buffers[i] = (char *) malloc(sizeof(char) * (UDP_DATA / n_buffers));
		assert(b->buffers[i] != NULL);
		b->buf_ptrs[i] = (int32_t *) b->buffers[i];
	}
  return b;
}

void gmrt_free_sock(gmrt_sock_t* b)
{
  b->fd = 0;
  b->size = 0;
  b->have_packet = 0;
  if (b->buffer)
		free(b->buffer);
	b->buffer = 0;
	if (b->buffers)
  {
    unsigned i=0;
    for (i=0; i<b->n_buffers; i++)
    {
      if (b->buffers[i])
        free(b->buffers[i]);
      b->buffers[i] = 0; 
    }
    free(b->buffers);
    free(b->buf_ptrs);
    b->buffers = 0;
  }
}


gmrt_data_t * gmrt_init_data(unsigned n_bufs, uint64_t size)
{
  gmrt_data_t * b = (gmrt_data_t *) malloc(sizeof(gmrt_data_t));

  assert(b != NULL);
  b->size = size;
  b->count = 0;
  b->min = 0;
  b->max = 0;

	b->n_bufs = n_bufs;

	b->bufs = (char **) malloc(sizeof(char *) * n_bufs);
	assert(b->bufs != NULL);

	b->delay_ptrs = (char **) malloc(sizeof(char ) * n_bufs);
	assert(b->delay_ptrs != NULL);

	unsigned i = 0;
	for (i=0; i<n_bufs; i++) 
	{
		// allocate room for the data array + padding for max delay
  	b->bufs[i] = (char *) malloc(sizeof(char) * (size + (2 * GMRT_SAMPLE_DELAY_OFFSET)));
  	assert(b->bufs[i] != NULL);

		// initially set the delay ptrs to the real buf 
		b->delay_ptrs[i] = b->bufs[i];

	}

  return b;
}

void gmrt_zero_data(gmrt_data_t * b)
{
  char zerodchar = 'c';
  memset(&zerodchar,0,sizeof(zerodchar));
	unsigned i = 0;
	for (i=0; i<b->n_bufs; i++)
 		memset(b->bufs[i], zerodchar, b->size);
}

void gmrt_free_data(gmrt_data_t * b)
{
  b->size = 0;
  b->count = 0;
  b->min = 0;
  b->max = 0;
  if (b->bufs) 
	{
		unsigned i=0;
		for (i=0; i<b->n_bufs; i++)
		{
			if (b->bufs[i])
				free(b->bufs[i]);
			b->bufs[i] = 0;
		}
    free(b->bufs);
		b->bufs = 0;
	}

	if (b->delay_ptrs)
		free(b->delay_ptrs);
	b->delay_ptrs = 0;

}


/* copy b_length data from s to b, subject to the s_offset, wrapping if
 * necessary and return the new s_offset */
unsigned int gmrt_encode_data(char * b, char * s, unsigned int b_length, 
                              unsigned int s_length, unsigned int s_offset) {
#ifdef _DEBUG
  fprintf(stderr, "gmrt_encode_data: b_length=%d, s_length=%d, s_offset=%d\n",
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

void gmrt_decode_header (unsigned char * b, uint64_t *seq_no, uint64_t * ch_id)
{
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

void gmrt_encode_header (char *b, uint64_t seq_no, uint64_t ch_id)
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

}

/*
 *  Initiliazes data structures and parameters for the receiver 
 */
int gmrt_receiver_init(gmrt_receiver_t * ctx, unsigned n_buffers)
{

  multilog_t* log = ctx->log;

  /* intialize our data structures */
  ctx->packets = init_stats_t();
  ctx->bytes   = init_stats_t();
  ctx->sock    = gmrt_init_sock(n_buffers, UDP_PAYLOAD);
  ctx->curr    = gmrt_init_data(n_buffers, UDP_DATA * UDP_NPACKS);
  ctx->next    = gmrt_init_data(n_buffers, UDP_DATA * UDP_NPACKS);

	gmrt_zero_data(ctx->curr);
	gmrt_zero_data(ctx->next);

  if (ctx->verbose) {
    multilog(log, LOG_INFO, "creating buffers of %d bytes\n", UDP_DATA * UDP_NPACKS);
    multilog(log, LOG_INFO, "opening udp socket %s:%d\n", ctx->interface, ctx->port);
  }

  ctx->busy_waits = 0;
  ctx->sod = 0;

}

/*
 *  Initiliazes data structures and parameters for the receiver 
 */
int gmrt_receiver_start(gmrt_receiver_t * ctx)
{

  multilog_t* log = ctx->log;

  ctx->sock->fd = dada_udp_sock_in(ctx->log, ctx->interface, ctx->port, ctx->verbose);
  if (ctx->sock->fd < 0) {
    multilog (log, LOG_ERR, "failed to create udp socket %s:%d\n", ctx->interface, ctx->port);
    return 0;
  }

  /* set the socket size to 512 MB */
  dada_udp_sock_set_buffer_size (log, ctx->sock->fd, ctx->verbose, 536870912);

  /* set the socket to non-blocking */
  sock_nonblock(ctx->sock->fd);

  ctx->total_bytes = 0;

  return 0;
}

int gmrt_receiver_dealloc(gmrt_receiver_t * ctx)
{
  close(ctx->sock->fd);
  gmrt_free_data(ctx->curr);
  gmrt_free_data(ctx->next);
  gmrt_free_sock(ctx->sock);

}

/* 
 * receive size packets and return a pointer to the captured data, updating size
 * with the number of packets captured
 */
void * gmrt_buffer_function(gmrt_receiver_t * ctx, uint64_t * size)
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
  gmrt_zero_data(ctx->next);

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

        /* Get a packet from the (non-blocking) socket */
        ctx->sock->got = recvfrom (ctx->sock->fd, ctx->sock->buffer, UDP_PAYLOAD, 0, NULL, NULL);

        /* If we successfully got a packet */
        if (ctx->sock->got == UDP_PAYLOAD) {

          ctx->sock->have_packet = 1;

        /* If no packet at the socket */
        } else if (ctx->sock->got == -1) {

          errsv = errno;

          if (errsv == EAGAIN) {
            /* Busy sleep for 10us */
            ctx->timer_count++;
            ctx->busy_waits++;

          } else {
            fprintf(stderr, "recvfrom failed: %s\n", strerror(errsv));
            ctx->got_enough = 1;
          }

        } else {

          fprintf(stderr, "got %d, expected %d\n", ctx->sock->got, UDP_PAYLOAD);

        }

        if (ctx->timer_count >= 100000) {

          /* if we are waiting the first packet */
          if (!ctx->sod) {
            ctx->timer_count = 0;
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

      /* if we are waiting for start of data */
      if (!ctx->sod) {
        // if we require a packet reset to start
        if (ctx->start_on_reset) 
        {  
          // if the sequence number is a "reset"
          if ((seq_no > 0) && (seq_no < 1000))
          {
            ctx->sod = 1;
            ctx->curr->min = 1;
          }
        }
        else  // we will start on the first packet we receive
        {
          ctx->sod = seq_no;
          ctx->curr->min = seq_no;
        }

        if (ctx->sod) 
        {
          ctx->sod = seq_no;
          multilog(ctx->log, LOG_INFO, "START : received packet %"PRIu64"\n", seq_no);

          // update the min/max sequence numbers for the receiving buffers
          ctx->curr->max = ctx->curr->min + UDP_NPACKS - 1;
          ctx->next->min = ctx->curr->max + 1;
          ctx->next->max = ctx->next->min + UDP_NPACKS - 1;
        }
      }

      if (ctx->sod && (seq_no != 0)) {

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
          memcpy( ctx->curr->bufs[0] + (seq_no - ctx->curr->min) * UDP_DATA,
                  ctx->sock->buffer + 16,
                  UDP_DATA);
          ctx->curr->count++;

        } else if (seq_no <= ctx->next->max) {

          memcpy( ctx->next->bufs[0] + (seq_no - ctx->next->min) * UDP_DATA,
                  ctx->sock->buffer + 16,
                  UDP_DATA);
          ctx->next->count++;

        } else {

          fprintf(stderr, "2: Not keeping up. curr %5.2f%, next %5.2f% | %"PRIu64" \n",
                   ((float) ctx->curr->count / (float) UDP_NPACKS)*100,
                   ((float) ctx->next->count / (float) UDP_NPACKS)*100, seq_no);

          ctx->sock->have_packet = 1;
          ctx->got_enough = 1;

        }

        // If we have filled the current buffer, then we can stop 
        if (ctx->curr->count >= UDP_NPACKS)
          ctx->got_enough = 1;

        // if the next buffer is 25% full, then we can stop 
        if (ctx->next->count >= (UDP_NPACKS * 0.50)) {
          ctx->got_enough = 1;
          if (ctx->verbose)
            fprintf(stderr, "next data buffer > 50%% full\n");
        }
      } // if (sod)
    } // if (!ctx->got_enough && ctx->sock->have_packet)
  } // while (!ctx->got_enough)

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
  assert(ctx->curr->bufs[0] != 0);

  return (void *) ctx->curr->bufs[0];

}

/* 
 * receive size packets and return a pointer to the captured data, updating size
 * with the number of packets captured
 */
void ** gmrt_multi_buffer_function(gmrt_receiver_t * ctx, uint64_t * size)
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

  unsigned i, j, k;

	uint64_t npacks = ctx->sock->n_buffers * UDP_NPACKS;

  /* switch the data buffer pointers */
  ctx->temp = ctx->curr;
  ctx->curr = ctx->next;
  ctx->next = ctx->temp;

  /* update the counters for the next buffer */
  ctx->next->count = 0;
  ctx->next->min = ctx->curr->max + 1;
  ctx->next->max = ctx->next->min + npacks -1;

  /* TODO check the performance hit of doing this */
  gmrt_zero_data(ctx->next);

  /* Determine the sequence number boundaries for curr and next buffers */
  int errsv;

	//int print=1;

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

        /* Get a packet from the (non-blocking) socket */
        ctx->sock->got = recvfrom (ctx->sock->fd, ctx->sock->buffer, UDP_PAYLOAD, 0, NULL, NULL);

        /* If we successfully got a packet */
        if (ctx->sock->got == UDP_PAYLOAD) {

          ctx->sock->have_packet = 1;

        /* If no packet at the socket */
        } else if (ctx->sock->got == -1) {

          errsv = errno;

          if (errsv == EAGAIN) {
            /* Busy sleep for 10us */
            ctx->timer_count++;
            ctx->busy_waits++;

          } else {
            fprintf(stderr, "recvfrom failed: %s\n", strerror(errsv));
            ctx->got_enough = 1;
          }

        } else {

          fprintf(stderr, "got %d, expected %d\n", ctx->sock->got, UDP_PAYLOAD);

        }

        if (ctx->timer_count >= 100000) {

          /* if we are waiting the first packet */
          if (!ctx->sod) {
            ctx->timer_count = 0;
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

      /* If we are waiting for the start of data */
      if (!ctx->sod) {
        if (ctx->start_on_reset)
        {
          if ((seq_no > 0) && (seq_no < 1000))
          {
            ctx->sod = 1;
            ctx->curr->min = 1;
          }
        }
        else
        {
          ctx->sod = seq_no;
          ctx->curr->min = seq_no; 
        }

        if (ctx->sod) 
        {
          ctx->sod = seq_no;        
          multilog(ctx->log, LOG_INFO, "START : received packet %"PRIu64"\n", seq_no);
          /* update the min/max sequence numbers for the receiving buffers */
          ctx->curr->max = ctx->curr->min + npacks - 1;
          ctx->next->min = ctx->curr->max + 1;
          ctx->next->max = ctx->next->min + npacks - 1;
        }
      } 

      if (ctx->sod && (seq_no != 0)) 
			{

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

			  /* if this packet belongs in the current buffer */
        } else if (seq_no <= ctx->curr->max + (2*GMRT_PACKET_DELAY_OFFSET)) {

					k = 0;
					// use tricky int32_t ptrs to copy 4 bytes at a time (note this will break 
					// if GMRT_BYTES_PER_BUFFERS is not 4 !!!
					for (i=0; i<(UDP_DATA/GMRT_BYTES_PER_BUFFER); i+=ctx->sock->n_buffers)
					{
            //for (j=0; j<ctx->sock->n_buffers; j++)
						 // ctx->sock->buf_ptrs[j][k] = ctx->sock->buf_ptr[i+j];
						 ctx->sock->buf_ptrs[0][k] = ctx->sock->buf_ptr[i];
						 ctx->sock->buf_ptrs[1][k] = ctx->sock->buf_ptr[i+1];
						k++;
					}

          // copy the de-interleave data into the curr buffer
          /*
          for (i=0; i<ctx->sock->n_buffers; i++)
          {
            memcpy( ctx->curr->bufs[i] + (seq_no - ctx->curr->min) * ctx->sock->udpdata_per_buf,
                    ctx->sock->buffers[i], ctx->sock->udpdata_per_buf);
          }*/

          memcpy( ctx->curr->bufs[0] + (seq_no - ctx->curr->min) * ctx->sock->udpdata_per_buf,
                  ctx->sock->buffers[0], ctx->sock->udpdata_per_buf);
          memcpy( ctx->curr->bufs[1] + (seq_no - ctx->curr->min) * ctx->sock->udpdata_per_buf,
                  ctx->sock->buffers[1], ctx->sock->udpdata_per_buf);

          ctx->curr->count++;

					if (seq_no > ctx->curr->max)
						ctx->next->count++;

        } else if (seq_no <= ctx->next->max) {

          // de-interleave the packet into the sockets buf_ptrs using the int32_t trick
          k = 0;
          for (i=0; i<(UDP_DATA/GMRT_BYTES_PER_BUFFER); i+=ctx->sock->n_buffers)
          {
            /*
            for (j=0; j<ctx->sock->n_buffers; j++)
						  ctx->sock->buf_ptrs[j][k] = ctx->sock->buf_ptr[i+j];
            */
						ctx->sock->buf_ptrs[j][0] = ctx->sock->buf_ptr[i];
						ctx->sock->buf_ptrs[j][1] = ctx->sock->buf_ptr[i+1];
            k++;
          }

          // copy the de-interleave data into the next buffer
          /*
          for (i=0; i<ctx->sock->n_buffers; i++)
          {
					  memcpy( ctx->next->bufs[i] + (seq_no - ctx->next->min) * ctx->sock->udpdata_per_buf,
						  	    ctx->sock->buffers[i], ctx->sock->udpdata_per_buf);
          }
          */
					memcpy( ctx->next->bufs[0] + (seq_no - ctx->next->min) * ctx->sock->udpdata_per_buf,
					 	    ctx->sock->buffers[0], ctx->sock->udpdata_per_buf);
					memcpy( ctx->next->bufs[1] + (seq_no - ctx->next->min) * ctx->sock->udpdata_per_buf,
					 	    ctx->sock->buffers[1], ctx->sock->udpdata_per_buf);

          ctx->next->count++;

        } else {

          fprintf(stderr, "2: Not keeping up. curr %5.2f%, next %5.2f% | %"PRIu64" \n",
                   ((float) ctx->curr->count / (float) npacks)*100,
                   ((float) ctx->next->count / (float) npacks)*100, seq_no);

          ctx->sock->have_packet = 1;
          ctx->got_enough = 1;

        }

        // If we have filled the current buffer (+GMRT_PACKET_DELAY_OFFSET), then we can stop 
        if (ctx->curr->count >= npacks + (2*GMRT_PACKET_DELAY_OFFSET))
          ctx->got_enough = 1;

        // if the next buffer is 25% full, then we can stop 
        if (ctx->next->count >= (npacks * 0.50)) 
				{
          ctx->got_enough = 1;
          if (ctx->verbose)
            fprintf(stderr, "next data buffer > 50%% full\n");
        }
      } // if (sod)
    } // if (!ctx->got_enough && ctx->sock->have_packet)
  } // while (!ctx->got_enough)

	// now copy the last 14 packets curr into next
	memcpy (ctx->next->bufs[0], ctx->curr->bufs[0] + ctx->curr->size, (2 * GMRT_SAMPLE_DELAY_OFFSET));
	memcpy (ctx->next->bufs[1], ctx->curr->bufs[1] + ctx->curr->size, (2 * GMRT_SAMPLE_DELAY_OFFSET));

  /* Some checks before returning */
  if (ctx->curr->count) {

    if ((ctx->curr->count < npacks+(2*GMRT_PACKET_DELAY_OFFSET)) && !(ctx->timer_count >= 100000)) {
      multilog (ctx->log, LOG_WARNING, "Dropped %"PRIu64" packets\n",
               (npacks - ctx->curr->count));

      ctx->packets->dropped += (npacks - ctx->curr->count);
      ctx->packets->dropped_per_sec += (npacks - ctx->curr->count);
      ctx->bytes->dropped += (UDP_DATA * (npacks - ctx->curr->count));
      ctx->bytes->dropped_per_sec += (UDP_DATA * (npacks - ctx->curr->count));
  
    }

    /* If the timeout ocurred, this is most likely due to end of data */
    if (ctx->timer_count >= 100000) {
      *size = ctx->curr->count * UDP_DATA;
      multilog(ctx->log, LOG_WARNING, "Suspected EOD received, returning %"PRIu64" bytes\n",*size);
    }
  } else {
    *size = 0;
  } 
	ctx->total_bytes += *size;

  assert(ctx->curr->delay_ptrs != 0);

  return (void **) ctx->curr->delay_ptrs;

}

void displayCharBits(char value) {
  char c, displayMask = 1 << 7;

  for (c=1; c<= 8; c++) {
    putchar(value & displayMask ? '1' : '0');
    value <<= 1;
  }
	fprintf(stdout, "\n");
}
