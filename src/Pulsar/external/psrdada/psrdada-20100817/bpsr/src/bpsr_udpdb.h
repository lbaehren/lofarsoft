/***************************************************************************
 *  
 *    Copyright (C) 2010 by Andrew Jameson
 *    Licensed under the Academic Free License version 2.1
 * 
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <netinet/in.h>
#include <signal.h>

/* for semaphore reading */
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "dada_hdu.h"
#include "dada_def.h"
#include "dada_pwc_main.h"
#include "bpsr_udp.h"

#include "ipcio.h"
#include "multilog.h"
#include "ascii_header.h"
#include "daemon.h"
#include "futils.h"

typedef struct{
  int verbose;           /* verbosity flag */
  int fd;                /* udp socket file descriptor */
  int port;              /* port to receive UDP data */
  char *socket_buffer;   /* data buffer for stioring of multiple udp packets */
  int datasize;          /* size of *data array */
  char *next_buffer;     /* buffer for a single udp packet */
  char *curr_buffer;     /* buffer for a single udp packet */
  int packet_in_buffer;  /* flag if buffer overrun occured */
  uint64_t curr_buffer_count;
  uint64_t next_buffer_count;
  uint64_t received;     /* number of bytes received */

  /* Sequence numbers */
  uint64_t expected_sequence_no;
  uint64_t curr_sequence_no;
  uint64_t min_sequence;
  uint64_t mid_sequence;
  uint64_t max_sequence;

  /* iBob specification configuration */
  char *   interface;
  uint64_t acc_len;
  uint64_t sequence_incr;
  uint64_t spectra_per_second;
  uint64_t bytes_per_second;

  /* Packet/byte stats */
  stats_t * packets;
  stats_t * bytes;

  unsigned int expected_header_bits;  // Expected bits/sample
  unsigned int expected_nchannels;    // Expected channels 
  unsigned int expected_nbands;       // Expected number of bands
  float current_bandwidth;            // Bandwidth currently being received
  multilog_t* statslog;               // special log for statistics
  time_t current_time;                
  time_t prev_time; 
  uint64_t error_seconds;             // Number of seconds to wait for 
  int mode;
  uint64_t prev_seq;

  char * ibob_host;
  int    ibob_port;

}udpdb_t;

void quit(dada_pwc_main_t* pwcm);
void signal_handler(int signalValue); 

