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
#include "apsr_udp.h"
#include "apsr_def.h"
#include "dada_def.h"
#include "disk_array.h"
#include "ascii_header.h"
#include "futils.h"
#include "multilog.h"

/* Number of UDP packets to be recived for a called to buffer_function */
#define NUMUDPPACKETS 2000

#define NOTRECORDING 0
#define RECORDING 1


/* structures dmadb datatype  */
typedef struct{
  multilog_t* log;
  char* header;          /* ascii header */
  int state;
  int verbose;           /* verbosity flag */
  int udpfd;             /* udp socket file descriptor */
  int filefd;            /* output file file descriptor */
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
  uint64_t packets_dropped_this_run;  // Dropped b/w start and stop
  uint64_t packets_received;          // Total dropped
  uint64_t packets_received_this_run; // Dropped b/w start and stop
  uint64_t packets_received_last_sec; // Dropped b/w start and stop
  uint64_t bytes_received;            // Total dropped
  uint64_t bytes_received_this_run;   // Dropped b/w start and stop
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

  /* the set of disks to which data may be written */
  disk_array_t* array;

  /* current filename */
  char file_name [FILENAME_MAX];

  /* file offset from start of data, as defined by FILE_NUMBER attribute */
  unsigned file_number;

  /* total number of bytes to transfer */
  uint64_t transfer_bytes;

  /* optimal number of bytes to transfer in 1 write call */
  uint64_t optimal_bytes;

}udpdisk_t;

/* Re-implemented functinos from dada_pwc */
time_t  udpdisk_start_function (udpdisk_t* udpdisk, time_t start_utc);
void*   udpdisk_read_function (udpdisk_t* udpdisk, uint64_t* size);
int64_t udpdisk_write_function (udpdisk_t* udpdisk, void* data, uint64_t size);
int     udpdisk_stop_function (udpdisk_t* udpdisk);

/* Utility functions */
void quit (udpdisk_t* udpdisk);
void signal_handler (int signalValue); 


