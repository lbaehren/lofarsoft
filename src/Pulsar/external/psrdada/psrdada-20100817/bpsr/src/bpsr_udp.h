#ifndef __BPSR_UDP_H
#define __BPSR_UDP_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dada_udp.h"


#define BPSR_UDP_COUNTER_BYTES  8          // size of header/sequence number
#define BPSR_UDP_DATASIZE_BYTES 2048       // obs bytes per packet
#define BPSR_UDP_PAYLOAD_BYTES  2056       // counter + datasize
#define BPSR_UDP_INTERFACE      "10.0.0.4" // default interface

#define BPSR_IBOB_CLOCK         400        // MHz
#define BPSR_IBOB_NCHANNELS     1024

/* Temp constants for the conversion function */
# define LITTLE 0
# define BIG 1

int      bpsr_create_udp_socket(multilog_t* log, const char* interface, int port, int verbose);
uint64_t decode_header (char *buffer);
void     encode_header (char *buffer, uint64_t counter);
void     uint64ToByteArray (uint64_t num, size_t bytes, unsigned char *arr, int type);
uint64_t byteArrayToUInt64 (unsigned char *arr, size_t bytes, int type);


/* Creates a UDP socket, and set the socket buffer to 64 MB */
int bpsr_create_udp_socket(multilog_t* log, const char* interface, int port, int verbose) {

  int fd = dada_udp_sock_in(log, interface, port, verbose);
  int rval = dada_udp_sock_set_buffer_size(log, fd, verbose, 67108864);
  return fd;
}


/* Encode the counter into the first8 bytes of the buffer array */ 
void encode_header (char *buffer, uint64_t counter) {

  uint64ToByteArray (counter, (size_t) BPSR_UDP_COUNTER_BYTES, 
                     (unsigned char*) buffer, (int) BIG);
  
}

/* Reads the first 8 bytes of the buffer array, interpreting it as
 * a 64 bit interger */
uint64_t decode_header(char *buffer) {

  uint64_t counter = byteArrayToUInt64 ((unsigned char*) buffer,
                                        (size_t) BPSR_UDP_COUNTER_BYTES,
                                        (int) BIG);
  return counter;
}

/* platform independant uint64_t to char array converter */
void uint64ToByteArray (uint64_t num, size_t bytes, unsigned char *arr, int type)  
{  
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
} 

/* platform independant char array to uint64_t converter */
uint64_t byteArrayToUInt64 (unsigned char *arr, size_t bytes, int type) 
{

  uint64_t num = UINT64_C (0);
  uint64_t tmp;

  size_t i;
  for (i = 0; i < bytes; i++ )
  {

    tmp = UINT64_C (0);
    if (type == LITTLE)
      tmp = arr[i];
    else if (type == BIG)
      tmp = arr[bytes - i - 1];

    num |= (tmp << ((i & 7) << 3));
  }

  return num;
}

#endif /* __BPSR_UDP_H */
