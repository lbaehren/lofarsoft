
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

#include "caspsr_udp.h"
#include "multilog.h"

typedef struct{

  multilog_t* log;

  caspsr_receiver_t * receiver;

  unsigned got_enough;      // flag for having received enough packets
  uint64_t transfer_bytes;  // total number of bytes to transfer
  uint64_t optimal_bytes;   // optimal number of bytes to transfer in 1 write

  unsigned i_dest;
  unsigned n_dest;
  unsigned n_packets;
  unsigned n_append;

} udpheader_t;

/* Re-implemented functinos from dada_pwc */
time_t udpheader_start_function (udpheader_t* udpheader, time_t start_utc);
void * udpheader_read_function (udpheader_t* udpheader, uint64_t* size);
int    udpheader_stop_function (udpheader_t* udpheader);

/* Utility functions */
void quit (udpheader_t* udpheader);
void signal_handler (int signalValue); 
void stats_thread (void * arg);
