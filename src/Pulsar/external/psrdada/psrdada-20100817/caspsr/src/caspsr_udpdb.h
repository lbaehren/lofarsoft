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
#include "dada_pwc_main.h"
#include "multilog.h"
#include "ipcio.h"
#include "ascii_header.h"

#include "caspsr_def.h"
#include "caspsr_udp.h"

/* Number of UDP packets to be recived for a called to buffer_function */
#define NUMUDPPACKETS 2000
#define NOTRECORDING 0
#define RECORDING 1

typedef struct{

  multilog_t* log;
  caspsr_receiver_t * receiver;
  int verbose;                  // verbosity flag
  unsigned got_enough;          // flag for having received enough packets
  uint64_t transfer_bytes;      // total number of bytes to transfer
  uint64_t optimal_bytes;       // optimal number of bytes to transfer in 1 write
  int      error_seconds; 

} udpdb_t;

/* Re-implemented functinos from dada_pwc_main */
time_t   udpdb_start_function (dada_pwc_main_t* pwcm, time_t start_utc);
void*    udpdb_read_function (dada_pwc_main_t* pwcm, uint64_t* size);
int      udpdb_stop_function (dada_pwc_main_t* pwcm);

int      udpdb_header_valid_function (dada_pwc_main_t* pwcm);
int      udpdb_error_function (dada_pwc_main_t* pwcm);
uint64_t udpdb_start_new_xfer (dada_pwc_main_t *pwcm);


/* Utility functions */
void quit (udpdb_t* udpdb);
void signal_handler (int signalValue); 
void stats_thread(void * arg);
