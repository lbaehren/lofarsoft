/***************************************************************************
 *  
 *    Copyright (C) 2009 by Andrew Jameson
 *    Licensed under the Academic Free License version 2.1
 * 
 ****************************************************************************/

#ifndef __CASPSR_UDP_H
#define __CASPSR_UDP_H

/* 
 * CASPSR udp library functions 
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/types.h>

#include "dada_udp.h"
#include "caspsr_def.h"
#include "arch.h"
#include "StopWatch.h"

#define UDP_HEADER   16             // size of header/sequence number
#define UDP_DATA     8192           // obs bytes per packet
#define UDP_PAYLOAD  8208           // header + datasize
#define UDP_NPACKS   25600          // 200 MB worth
#define UDP_IFACE    "192.168.4.14" // default interface

//#define _DEBUG 1

/* a buffer that can be filled with data */
typedef struct {

  char     * buffer;  // data buffer itself
  uint64_t * ids;     // packet ids in the buffer
  uint64_t   count;   // number of packets in this buffer
  uint64_t   size;    // size in bytes of the buffer
  uint64_t   min;     // minimum seq number acceptable
  uint64_t   max;     // maximum seq number acceptable

} caspsr_data_t;

/* socket buffer for receiving udp data */
typedef struct {

  int        fd;            // FD of the socket
  char     * buffer;        // the socket buffer
  unsigned   size;          // size of the buffer
  unsigned   have_packet;   // is there a packet in the buffer
  size_t     got;           // amount of data received

} caspsr_sock_t;

typedef struct {

  multilog_t   * log;
  int            verbose;  
  unsigned       got_enough;

  /* incoming UDP socket */
  char         * interface;     // IP address of the socket (e.g. 192.168.1.100)
  int            port;          // port of the socket
  caspsr_sock_t * sock;

  stats_t      * packets;
  stats_t      * bytes;

  /* data buffers */
  caspsr_data_t * curr;
  caspsr_data_t * next;
  caspsr_data_t * temp;

  /* packet timing */
  StopWatch   * timer;
  double        timer_sleep;
  uint64_t      timer_count;
  struct        timeval timeout;

  /* xfer management */
  uint64_t xfer_bytes;   // the number of bytes we expect to receiver per xfer
  uint64_t xfer_offset;  // the offset of this receiver in the 16 PWCs
  uint64_t xfer_packets; // number of packets to receiver in an xfer
  uint64_t xfer_gap;     // the number of bytes "missing" between xfers
  uint64_t xfer_s_seq;   // the first seq number of the current xfer
  uint64_t xfer_e_seq;   // the first seq number of the current xfer
  uint64_t xfer_count;   // current xfer number
  unsigned xfer_ending;  // flag to indicate xfer will soon be ending
  unsigned sod;          // flag to indicate waiting for 1st packet


} caspsr_receiver_t;

/* read the seq_no and ch_id from a CASPSR packet */
void caspsr_decode_header (unsigned char * b, uint64_t * seq_no, uint64_t * ch_id);

/* write the seq_no and ch_ud from a CASPSR packet */
void caspsr_encode_header (char * b, uint64_t seq_no, uint64_t ch_id);

/* initialize a CASPSR sock struct */
caspsr_sock_t * caspsr_init_sock(unsigned size);

/* initialize a CASPSR data struct */
caspsr_data_t * caspsr_init_data(uint64_t size);

/* free a CASPSR sock struct */
void caspsr_free_sock(caspsr_sock_t* b);

/* free a CASPSR data struct */
void caspsr_free_data(caspsr_data_t * b);

/* set all the data values in an CASPSR buffer to zero */
void caspsr_zero_data(caspsr_data_t * b);

unsigned int caspsr_encode_data(char * b, char * s, unsigned int b_length,
                              unsigned int s_length, unsigned int s_offset);

/* initialize the caspsr_receiver */
int caspsr_receiver_init(caspsr_receiver_t * ctx, unsigned i_dest, unsigned n_dest, 
                        unsigned n_packets, unsigned n_append);

int      caspsr_receiver_dealloc(caspsr_receiver_t * ctx);
uint64_t caspsr_start_xfer(caspsr_receiver_t * ctx);
void *   caspsr_xfer(caspsr_receiver_t * ctx, uint64_t * size);
int      caspsr_stop_xfer(caspsr_receiver_t * ctx);

#endif /* __CASPSR_UDP_H */
