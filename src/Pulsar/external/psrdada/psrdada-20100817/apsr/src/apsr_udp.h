#ifndef __APSR_UDP_H
#define __APSR_UDP_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dada_udp.h"

/* Maximum size of a UDP packet */
#define UDPBUFFSIZE 16384

/* Size of header component of the data packet */
#define UDPHEADERSIZE 24

/* Size of data component of the data packet */
/* 8948 == 9000 - 28 (udp hdr) - 24 (our hdr) */
#define DEFAULT_UDPDATASIZE 8948

/* header struct for UDP packet from board */ 
typedef struct {
  unsigned char length;
  unsigned char source;
  unsigned int sequence;
  unsigned char bits;
  unsigned char channels;
  unsigned char bands;
  unsigned char bandID[4];
  unsigned int pollength;
} header_struct;

/* data buffer for udpdb */
typedef struct {

  char     * buffer;  // data buffer itself
  uint64_t   count;   // number of packets in this buffer
  uint64_t   size;    // size in bytes of the buffer
  uint64_t   min;     // minimum seq number acceptable
  uint64_t   max;     // maximum seq number acceptable

} apsr_data_t;

/* socket buffer for receiving udp data */
typedef struct {

  int        fd;            // FD of the socket
  char     * buffer;        // the socket buffer
  unsigned   size;          // size of the buffer
  unsigned   have_packet;   // is there a packet in the buffer
  ssize_t     got;           // amount of data received

} apsr_sock_t;

/* initialize an APSR sock struct */
apsr_sock_t * apsr_init_sock(unsigned size);

/* initialize an APSR data struct */
apsr_data_t * apsr_init_data(uint64_t size);

/* free an APSR sock struct */
void apsr_free_sock(apsr_sock_t* b);

/* free an APSR data struct */
void apsr_free_data(apsr_data_t * b);

/* set all the data values in an APSR buffer to zero */
void apsr_zero_data(apsr_data_t * b);

/* reset the counters in an APSR buffer to zero */
void apsr_reset_data(apsr_data_t * b);

/* create a socket for UDP data */
int  apsr_create_udp_socket(multilog_t* log, const char* interface, int port, int verbose);

/* decode a PDFB3 custom packet header */
void decode_header(char *buffer, header_struct *header);

/* encode a PDFB3 custom packet header */
void encode_header(char *buffer, header_struct *header);

/* print a PDFB3 custom packet header */
void print_header(header_struct *header);

#endif /* __APSR_UDP_H */
