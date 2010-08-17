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
#define CASPSR_DEFAULT_UDPNDB_PORT 33108

typedef struct {

  key_t  key;        /* shared memory key containing data stream */
  dada_hdu_t* hdu;   /* DADA Header plus Data unit */

} udpNdb_receiver_t;

typedef struct {

  multilog_t* log;
  int verbose;          /* verbosity flag */
  int fd;               /* incoming UDP socket */
  int port;             /* port to receive UDP data */
  char *   interface;   /* NIC/Interface to open socket on */
  char *   header;      /* ascii header to write to all DBs */
  int      header_size; /* header size */

  /* receiving datablocks */
  unsigned n_receivers;     /* number of receivers */
  unsigned receiver_i;      /* current receiver */
  udpNdb_receiver_t ** receivers;

  /* packets per "transfer" */
  uint64_t packets_this_xfer;
  uint64_t packets_per_xfer;
  char  *  zeroed_packet;

  /* Packet and byte statistics */
  stats_t * packets;
  stats_t * bytes;

  /* socket used to receive a packet */
  caspsr_sock_t * sock;

  uint64_t obs_offset;      // current byte offset from first byte
  uint64_t next_seq;        // Next packet we are expecting
  struct   timeval timeout; 

  uint64_t n_sleeps;
  uint64_t ooo_packets;


} udpNdb_t;

time_t udpNdb_start_function (udpNdb_t * ctx);
int udpNdb_open_datablocks(udpNdb_t * ctx);
int udpNdb_close_datablocks(udpNdb_t * ctx);
int udpNdb_new_datablock(udpNdb_t * ctx);
int udpNdb_new_header(udpNdb_t * ctx);
int udpNdb_main_function (udpNdb_t * ctx, int64_t total_bytes);
int udpNdb_stop_function (udpNdb_t* ctx);


void usage();
void signal_handler (int signalValue); 
void stats_thread(void * arg);

