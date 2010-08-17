/* dada, ipc stuff */

#include "dada_hdu.h"
#include "dada_def.h"
#include "dada_pwc_main.h"
#include "apsr_udp.h"

#include "ipcio.h"
#include "multilog.h"
#include "ascii_header.h"
#include "daemon.h"
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

/* for semaphore reading */
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

/* Number of UDP packets to be recived for a called to buffer_function */
#define NUMUDPPACKETS 2000

/* structures dmadb datatype  */
typedef struct{

  /* multilog pointers */
  multilog_t* log;
  multilog_t* stats_log;

  int verbose;           /* verbosity flag */
    
  //int fd;                /* udp socket file descriptor */
  //int port;              
  //char *socket_buffer;   /* data buffer for stioring of multiple udp packets */
  //int datasize;          /* size of *data array */
  //char *next_buffer;     /* buffer for a single udp packet */
  //char *curr_buffer;     /* buffer for a single udp packet */
  //int packet_in_buffer;  /* flag if buffer overrun occured */

  /* data buffers for the buffer function to fill */
  apsr_data_t * curr;
  apsr_data_t * next;
  apsr_data_t * temp;

  /* incoming UDP data */
  char          * interface;    // IP address for UDP data
  int             port;         // port for UDP data
  apsr_sock_t   * sock;         // UDP socket struct

  /* statistics for packets/bytes received/lost */
  stats_t       * packets;
  stats_t       * bytes;

  /* packet information */
  unsigned got_enough;
  uint64_t packets_per_buffer;
  uint64_t packet_length;
  uint64_t payload_length;

  /* thread control */
  unsigned quit_threads;

  /* timer control on packets/socket */
  uint64_t timer_count;
  uint64_t timer_max;
  int      bps;

  uint64_t next_seq;

  char zerod_char;

  uint64_t expected_sequence_no;
  uint64_t packets_late_this_sec;     //
  unsigned int expected_header_bits;  // Expected bits/sample
  unsigned int expected_nchannels;    // Expected channels 
  unsigned int expected_nbands;       // Expected number of bands
  uint64_t error_seconds;             // Number of seconds to wait for 

} udpdb_t;

void check_header(header_struct header, udpdb_t *udpdb, multilog_t *log); 
void quit(dada_pwc_main_t* pwcm);
void signal_handler(int signalValue); 
void stats_thread (void * arg);


