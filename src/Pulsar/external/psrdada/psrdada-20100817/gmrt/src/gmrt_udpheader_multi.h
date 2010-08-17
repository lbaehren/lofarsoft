
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

#include "gmrt_udp.h"
#include "multilog.h"
#include "gmrt_delay_fns.h"

typedef struct{

  multilog_t* log;

  gmrt_receiver_t * receiver;

  unsigned got_enough;      // flag for having received enough packets
  uint64_t transfer_bytes;  // total number of bytes to transfer
  uint64_t optimal_bytes;   // optimal number of bytes to transfer in 1 write

	CorrType        corr;         // Correlator struct from gmrt_delay_*
	SourceParType   source;       // Source struct from gmrt_delay_*
	delay_vals_t    delays[NCHAN*NUM_ANT];  // gmrt_delay integer/fractional delay
	fringe_vals_t   fringes[NCHAN*NUM_ANT]; // gmrt_delay fringe info
	struct timeval  timestamp;    // gmrt_delay timestamp
	struct timeval  utc_start;    // timestamp of the utc_start

} gmrt_udpheader_multi_t;

/* Re-implemented functinos from dada_pwc */
time_t udpheader_start_function (gmrt_udpheader_multi_t* udpheader, time_t start_utc);
void ** gmrt_udpheader_multi_buffer_function (gmrt_udpheader_multi_t * ctx, uint64_t* size);
int    udpheader_stop_function (gmrt_udpheader_multi_t* udpheader);

/* Utility functions */
void quit (gmrt_udpheader_multi_t* udpheader);
void signal_handler (int signalValue); 
void stats_thread (void * arg);
