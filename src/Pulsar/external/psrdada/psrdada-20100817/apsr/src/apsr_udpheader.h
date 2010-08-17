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

/* dada, ipc stuff */

#include "dada_hdu.h"
#include "dada_def.h"
#include "apsr_def.h"
#include "apsr_udp.h"
#include "multilog.h"
#include "futils.h"

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

  uint64_t packets_dropped;           // Total dropped
  uint64_t packets_dropped_last_sec;  // Total dropped in the previous second

  uint64_t packets_received;          // Total dropped
  uint64_t packets_received_last_sec; // Dropped b/w start and stop

  uint64_t bytes_received;            // Total dropped
  uint64_t bytes_received_last_sec;   // Dropped b/w start and stop

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

}udpheader_t;

/* Re-implemented functinos from dada_pwc */
time_t  udpheader_start_function (udpheader_t* udpheader, time_t start_utc);
void*   udpheader_read_function (udpheader_t* udpheader, uint64_t* size);
int     udpheader_stop_function (udpheader_t* udpheader);

/* Utility functions */
void print_udpbuffer (char * buffer, int buffersize);
void check_udpdata (char * buffer, int buffersize, int value);
int  create_udp_socket (udpheader_t* udpheader);
void quit (udpheader_t* udpheader);
void signal_handler (int signalValue); 
void my_print_header(uint64_t expected_sequence_no, header_struct *header);


