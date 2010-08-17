/***************************************************************************
 *  
 *    Copyright (C) 2009 by Andrew Jameson
 *    Licensed under the Academic Free License version 2.1
 * 
 ****************************************************************************/

#ifndef __GMRT_UDP_H
#define __GMRT_UDP_H

/* 
 * GMRT udp library functions 
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/types.h>

#include "dada_udp.h"
#include "gmrt_def.h"

#define UDP_HEADER   16             // size of header/sequence number
#define UDP_DATA     8192           // obs bytes per packet
#define UDP_PAYLOAD  8208           // header + datasize
#define UDP_NPACKS   32768          // 256 MB worth
#define UDP_IFACE    "192.168.4.14" // default interface

//#define _DEBUG 1

/* a buffer that can be filled with data */
typedef struct gmrt_data {

	unsigned    n_bufs;       // number of data streams
  char     ** bufs;   		  // data buffer itself [padded for delay]
  char     ** delay_ptrs;   // ptrs to the integer delayed start of the buffer
  uint64_t *  ids;          // packet ids in the buffer
  uint64_t    count;        // number of packets in this buffer
  uint64_t    size;         // size in bytes of the buffer
  uint64_t    min;          // minimum seq number acceptable
  uint64_t    max;          // maximum seq number acceptable
	uint64_t 		packet_max_delay;
	uint64_t		samples_max_delay;

} gmrt_data_t;

/* socket buffer for receiving udp data */
typedef struct {

  int        fd;              // FD of the socket
  char     * buffer;          // the socket buffer
	int32_t  * buf_ptr;			  	// integer pointer to the buffer
  unsigned   size;            // size of the buffer
  unsigned   have_packet;     // is there a packet in the buffer
  size_t     got;             // amount of data received
	unsigned   n_buffers;       // number of buffers to deinterleave into
	char    ** buffers;			    // buffers for de-interleaving packets
	int32_t ** buf_ptrs;		  	// ptrs to sock buffers
	size_t     udpdata_per_buf;	// UDP_DATA / n_buffers

} gmrt_sock_t;

typedef struct {

  multilog_t   * log;
  int            verbose;  
  unsigned       got_enough;
  char         * interface;         // IP address of the socket (e.g. 192.168.1.100)
  int            port;              // port of the socket
  gmrt_sock_t  * sock;

  stats_t      * packets;
  stats_t      * bytes;
	
	double         total_bytes;			  // total number of bytes recorded this obseveration
	double         total_time;			  // total number of seconds recorded this observation
	double         bytes_per_sample;	// bytes in a sample
	double         bytes_per_second;  // bytes in a second of data
	double         t_samp;					  // sampling interval [usec]
	unsigned       nbit;						  // number of bits per sample
	unsigned       nchan;						  // number of channels - should be 1
	unsigned       ndim;						  // real/complex - should be 1 for real
	unsigned       npol;						  // should be 2

  /* data buffers */
  gmrt_data_t  * curr;
  gmrt_data_t  * next;
  gmrt_data_t  * temp;

  /* packet timing */
  uint64_t       timer_count;
  struct         timeval timeout;

  unsigned       sod;               // flag to indicate waiting for 1st packet
  uint64_t       busy_waits;
  unsigned       start_on_reset;

} gmrt_receiver_t;

/* read the seq_no and ch_id from a GMRT packet */
void gmrt_decode_header (unsigned char * b, uint64_t * seq_no, uint64_t * ch_id);

/* write the seq_no and ch_ud from a GMRT packet */
void gmrt_encode_header (char * b, uint64_t seq_no, uint64_t ch_id);

/* initialize a GMRT sock struct */
gmrt_sock_t * gmrt_init_sock(unsigned n_buffers, unsigned size);

/* initialize a GMRT data struct */
gmrt_data_t * gmrt_init_data(unsigned n_buffers, uint64_t size);

/* free a GMRT sock struct */
void gmrt_free_sock(gmrt_sock_t* b);

/* free a GMRT data struct */
void gmrt_free_data(gmrt_data_t * b);

/* set all the data values in an GMRT buffer to zero */
void gmrt_zero_data(gmrt_data_t * b);

unsigned int gmrt_encode_data(char * b, char * s, unsigned int b_length,
                              unsigned int s_length, unsigned int s_offset);

int 	  gmrt_receiver_init(gmrt_receiver_t * ctx, unsigned n_buffers);
int 	  gmrt_receiver_start(gmrt_receiver_t * ctx);
int     gmrt_receiver_dealloc(gmrt_receiver_t * ctx);

void *  gmrt_buffer_function(gmrt_receiver_t * ctx, uint64_t * size);
void ** gmrt_multi_buffer_function(gmrt_receiver_t * ctx, uint64_t * size);

void displayCharBits(char c);

#endif /* __GMRT_UDP_H */
