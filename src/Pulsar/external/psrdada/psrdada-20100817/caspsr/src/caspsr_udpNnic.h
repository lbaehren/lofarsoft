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
#define CASPSR_DEFAULT_UDPNNIC_PORT 33108

typedef struct {

  multilog_t* log;            /* logging facility */
  char *   host;              /* destination hostname */
  int      port;              /* destination port */
  char *   buffer;            /* memory buffer */
  uint64_t size;              /* size of the memory array */
  uint64_t w_total;           /* total bytes to send in an xfer */
  uint64_t w_count;           /* total bytes written */
  uint64_t r_count;           /* total bytes read */
  int      fd;                /* outgoing file descriptor */
  struct sockaddr_in dagram;  /* UDP socket struct */

} udpNnic_receiver_t;

typedef struct {

  multilog_t* log;
  int      verbose;          /* verbosity flag */
  int      fd;               /* incoming UDP socket */
  int      port;             /* port to receive UDP data */
  char *   interface;        /* NIC/Interface to open socket on */

  /* receivers */
  unsigned n_receivers;      /* number of receivers */
  unsigned receiver_i;       /* current receiver */
  udpNnic_receiver_t ** receivers;
  unsigned i_distrib;        /* number of this distributor */
  unsigned n_distrib;        /* number of distributors */

  /* packets per "transfer" */
  uint64_t packets_this_xfer;
  uint64_t packets_per_xfer;
  uint64_t packets_to_append;
  char  *  zeroed_packet;

  /* Packet and byte statistics */
  stats_t * packets;
  stats_t * bytes;

  /* housekeeping */
  int      clamped_output_rate;
  int64_t  bytes_to_acquire;
  uint64_t ch_id;
  uint64_t next_seq;
  struct   timeval timeout; 
  uint64_t n_sleeps;
  uint64_t ooo_packets;
  uint64_t ooo_ch_ids;
  uint64_t r_bytes;
  uint64_t r_packs;
  int      send_core;
  int      recv_core;
  uint64_t send_sleeps;
  unsigned receive_only;

} udpNnic_t;

int udpNnic_initialize_receivers (udpNnic_t * ctx);
int udpNnic_dealloc_receivers(udpNnic_t * ctx);

time_t udpNnic_start_function (udpNnic_t * ctx);
int udpNnic_new_receiver(udpNnic_t * ctx);
int udpNnic_stop_function (udpNnic_t* ctx);

void * receiving_thread(void * arg);
void * sending_thread(void * arg);
void plotting_thread(void * arg);
void stats_thread(void * arg);

void usage();
void signal_handler (int signalValue); 

