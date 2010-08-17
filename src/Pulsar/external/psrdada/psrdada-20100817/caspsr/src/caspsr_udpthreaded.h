
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

#include "dada_hdu.h"
#include "caspsr_def.h"
#include "caspsr_udp.h"
#include "multilog.h"


/* Number of UDP packets to be recived for a called to buffer_function */
#define NUMUDPPACKETS 2000
#define NOTRECORDING 0
#define RECORDING 1


typedef struct{

  multilog_t* log;
  int state;
  int verbose;           /* verbosity flag */
  int udpfd;             /* udp socket file descriptor */
  int port;              /* port to receive UDP data */
  char *   interface;   /* NIC/Interface to open socket on */

  /* Packet and byte statistics */
  stats_t * packets;
  stats_t * bytes;

  /* Current and Next buffers */
  caspsr_data_t * curr;
  caspsr_data_t * next;
  caspsr_data_t * temp;

  uint64_t next_seq;       // Next packet we are expecting
  int64_t n_sleeps;        // number of times we slept
  uint64_t ooo_packs;      // number of packets out of order

} udpthreaded_t;

/* Re-implemented functinos from dada_pwc */
time_t  udpthreaded_start_function (udpthreaded_t* udpthreaded, time_t start_utc);
void    udpthreaded_capture_thread(void * arg);
int     udpthreaded_stop_function (udpthreaded_t* udpthreaded);

/* Utility functions */
void quit (udpthreaded_t* udpthreaded);
void signal_handler (int signalValue); 
void stats_thread(void * arg);
