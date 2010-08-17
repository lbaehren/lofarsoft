/* dada, ipc stuff */

#include "dada_hdu.h"
#include "dada_def.h"
#include "bpsr_def.h"
#include "bpsr_udp.h"
#include "multilog.h"

#include "futils.h"

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

/* Number of UDP packets to be recived for a called to buffer_function */
#define NUMUDPPACKETS 2000
#define NOTRECORDING 0
#define RECORDING 1


/* structures dmadb datatype  */
typedef struct{
  multilog_t* log;
  int state;
  int verbose;           /* verbosity flag */
  int udpfd;             /* udp socket file descriptor */
  int port;              /* port to receive UDP data */
  char *socket_buffer;   /* data buffer for stioring of multiple udp packets */
  int datasize;          /* size of *data array */
  char *next_buffer;     /* buffer for a single udp packet */
  char *curr_buffer;     /* buffer for a single udp packet */
  int packet_in_buffer;  /* flag if buffer overrun occured */
  uint64_t curr_buffer_count;
  uint64_t next_buffer_count;
  uint64_t received;     /* number of bytes received */
  uint64_t expected_sequence_no;

  /* iBob specification configuration */
  char *   interface;
  uint64_t acc_len;
  uint64_t sequence_incr;
  uint64_t spectra_per_second;
  uint64_t bytes_per_second;
                                                                                
  /* Packet/byte stats */
  stats_t * packets;
  stats_t * bytes;

  uint64_t dropped_packets_to_fill;   // 
  uint64_t packets_late_this_sec;     //
  unsigned int expected_header_bits;  // Expected bits/sample
  unsigned int expected_nchannels;    // Expected channels 
  unsigned int expected_nbands;       // Expected number of bands
  float current_bandwidth;            // Bandwidth currently being received
  time_t current_time;                
  time_t prev_time; 
  uint64_t error_seconds;             // Number of seconds to wait for 
  uint64_t packet_length;                                    

  /* total number of bytes to transfer */
  uint64_t transfer_bytes;

  /* optimal number of bytes to transfer in 1 write call */
  uint64_t optimal_bytes;

  uint64_t select_sleep;

}udpheader_t;

/* Re-implemented functinos from dada_pwc */
time_t  udpheader_start_function (udpheader_t* udpheader, time_t start_utc);
void*   udpheader_read_function (udpheader_t* udpheader, uint64_t* size);
int     udpheader_stop_function (udpheader_t* udpheader);

/* Utility functions */
void quit (udpheader_t* udpheader);
void signal_handler (int signalValue); 


