/***************************************************************************
 *  
 *    Copyright (C) 2009 by Andrew Jameson
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

#include "futils.h"
#include "dada_hdu.h"
#include "dada_pwc_main_multi.h"
#include "multilog.h"
#include "ipcio.h"
#include "ascii_header.h"

#include "gmrt_def.h"
#include "gmrt_udp.h"
#include "gmrt_delay_fns.h"

/* Number of UDP packets to be recived for a called to buffer_function */
#define NUMUDPPACKETS 2000
#define NOTRECORDING 0
#define RECORDING 1

typedef struct{

  multilog_t* log;
  gmrt_receiver_t * receiver;
  int verbose;                  // verbosity flag
  unsigned got_enough;          // flag for having received enough packets
  uint64_t transfer_bytes;      // total number of bytes to transfer
  uint64_t optimal_bytes;       // optimal number of bytes to transfer in 1 write
  int      error_seconds; 
	CorrType        corr;					// Correlator struct from gmrt_delay_*
	SourceParType		source;				// Source struct from gmrt_delay_*
	delay_vals_t    delays[NCHAN*NUM_ANT];	// gmrt_delay integer/fractional delay
	fringe_vals_t   fringes[NCHAN*NUM_ANT];	// gmrt_delay fringe info
	struct timeval  timestamp;		// gmrt_delay timestamp
	struct timeval  utc_start;		// timestamp of the utc_start

} gmrt_udpdb_multi_t;

/* Re-implemented functinos from dada_pwc_main_multi */
time_t   gmrt_udpdb_multi_start_function (dada_pwc_main_multi_t* pwcm, time_t start_utc);
void **  gmrt_udpdb_multi_read_function (dada_pwc_main_multi_t* pwcm, uint64_t* size);
int      gmrt_udpdb_multi_stop_function (dada_pwc_main_multi_t* pwcm);
int      gmrt_udpdb_mutli_header_valid_function (dada_pwc_main_multi_t* pwcm);
int      gmrt_udpdb_multi_error_function (dada_pwc_main_multi_t* pwcm);
uint64_t gmrt_udpdb_multi_start_new_xfer (dada_pwc_main_multi_t *pwcm);

/* Utility functions */
void quit (gmrt_udpdb_multi_t* udpdb);
void signal_handler (int signalValue); 
void stats_thread(void * arg);
void delay_thread(void * arg);
