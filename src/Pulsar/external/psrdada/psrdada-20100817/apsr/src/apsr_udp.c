/***************************************************************************
 *  
 *    Copyright (C) 2009 by Andrew Jameson
 *    Licensed under the Academic Free License version 2.1
 * 
 ****************************************************************************/

#include <assert.h>
#include <errno.h>

#include "apsr_udp.h"


/* APSR sock_t structure */
apsr_sock_t * apsr_init_sock(unsigned size) 
{

  apsr_sock_t * b = (apsr_sock_t *) malloc(sizeof(apsr_sock_t));

  assert(b != NULL);

  b->fd = 0;
  b->size = size;
  b->have_packet = 0;
  b->buffer = (char *) malloc(sizeof(char) * size);
  assert(b->buffer != NULL);

  return b;

}

apsr_data_t * apsr_init_data(uint64_t size) 
{

  apsr_data_t * b = (apsr_data_t *) malloc(sizeof(apsr_data_t));

  assert(b != NULL);
  b->size = size;
  b->count = 0;
  b->min = 0;
  b->max = 0;
  b->buffer = (char *) malloc(sizeof(char) * size);
  assert(b->buffer != NULL);

  return b;

}

void apsr_free_sock(apsr_sock_t* b)
{
  b->fd = 0;
  b->size = 0;
  b->have_packet = 0;
  free(b->buffer);
}

void apsr_free_data(apsr_data_t * b)
{
  b->size = 0;
  b->count = 0;
  b->min = 0;
  b->max = 0;
  if (b->buffer) 
    free(b->buffer);
  else 
    fprintf(stderr, "free_apsr_buffer: b->buffer was unexpectedly freed\n");
}

void apsr_zero_data(apsr_data_t * b)
{
  char zerodchar = 'c';
  memset(&zerodchar,0,sizeof(zerodchar));
  memset(b->buffer, zerodchar, b->size);
}

void apsr_reset_data(apsr_data_t * b)
{
   b->count = 0;
   b->min = 0;
   b->max = 0;
}

int apsr_create_udp_socket(multilog_t* log, const char* interface, int port, int verbose) 
{
  int fd = dada_udp_sock_in(log, interface, port, verbose);
  int rval = dada_udp_sock_set_buffer_size(log, fd, verbose, 67108864);
  return fd;
}

void encode_header(char *buffer, header_struct *header) 
{

  if (header->bits == 0) header->bits = 8;

  int temp = header->sequence;
       
  buffer[7] = 0x00;
  buffer[6] = header->bits;
  buffer[5] = temp & 0xff;
  buffer[4] = (temp >> 8) & 0xff;
  buffer[3] = (temp >> 16) & 0xff;
  buffer[2] = (temp >> 24) & 0xff;
  buffer[1] = header->source;
  buffer[0] = header->length;
  buffer[15] = 0x00;
  buffer[14] = 0x00;
  buffer[13] = header->bandID[3];
  buffer[12] = header->bandID[2];
  buffer[11] = header->bandID[1];
  buffer[10] = header->bandID[0];
  buffer[9] = header->bands;
  buffer[8] = header->channels;

}

void decode_header(char *buffer, header_struct *header) 
{

  int temp;

  /* header decode */
  header->length    = buffer[0];
  header->source    = buffer[1];
  header->sequence  = buffer[2]; 
  header->sequence  <<= 24;
  temp              = buffer[3];
  header->sequence  |= ((temp << 16) & 0xff0000);
  temp              = buffer[4];
  header->sequence  |= (temp << 8) & 0xff00;
  header->sequence  |=  (buffer[5] & 0xff);
    
  // header->packetbitsum  = buffer                                                            
  // header->bits      = buffer[6];    
  // zeros	    = buffer[7];
  header->channels  = buffer[8];
  header->bands     = buffer[9];
                                                 
  header->bandID[0] = buffer[10];
  header->bandID[1] = buffer[11];
  header->bandID[2] = buffer[12];
  header->bandID[3] = buffer[13];
  // zeros          = buffer[14];
  // zeros          = buffer[14];

}

void print_header(header_struct *header) 
{
  fprintf(stderr,"length = %d\n",header->length);
  fprintf(stderr,"source = %d\n",header->source);
  fprintf(stderr,"sequence= %d\n",header->sequence);
  fprintf(stderr,"bits = %d\n",header->bits);
  fprintf(stderr,"channels = %d\n",header->channels);
  fprintf(stderr,"bands = %d\n",header->bands);
}
