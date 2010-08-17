/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrdada/psrdada/caspsr/src/bibob.h,v $
   $Revision: 1.3 $
   $Date: 2010/04/12 06:47:51 $
   $Author: straten $ */

#include "caspsr_udp.h"

#ifndef __DADA_BIBOB_H
#define __DADA_BIBOB_H

#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BIBOB_VLAN_BASE "192.168.0."
#define BIBOB_PORT 7

#define BIBOB_BRAM_CHANNELS 2048
#define BIBOB_CHARS_PER_BRAM 12

#define BIBOB_LAST_PACKET 2
#define BIBOB_NORM_PACKET 1

#define BIBOB_NBANDS 16
#define BIBOB_BRAM_ARRAYS 4

#define CASPSR_BIBOB_UDP_PAYLOAD 1024
#define CASPSR_BIBOB_UDP_HEADER 8
#define CASPSR_BIBOB_UDP_PACKET 1032

/* ************************************************************************

   bibob_t - a struct and associated routines for creating and managing,
   as well as reading and writing to and from an bibob

   ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {

    //! Unique identifier for this session/connection
    unsigned long id;

    //! sequence number of this packet (output only)
    unsigned int  seq;

    //! packet type, 1=not last, 2=last
    unsigned int  type;

  } bibob_header_t;

  typedef struct {

    long int * vals;

    float * avgs;

    unsigned count;

  } bibob_bramdump_t;


  typedef struct {

    //! Host name of bibob interface
    char* host;

    //! Port number of bibob interface
    int port;

    //! File descriptor of open socket; -1 otherwise
    int fd;

    //! UDP socket struct
    struct sockaddr_in dagram;

    //! Buffer used for bibob recv communication
    char* buffer;

    //! Unique/random identifier for CASPSR packet header
    unsigned long session_id;

    //! Size of buffer reserved for bibob communication
    unsigned buffer_size;

    //! Bit window for pol1
    unsigned pol1_bit;

    //! Bit window for pol2
    unsigned pol2_bit;

    //! Polarisation 1 scaling coefficient
    unsigned pol1_coeff;

    //! Polarisation 2 scaling coefficient
    unsigned pol2_coeff;

    //! FFT shift coefficient
    unsigned fft_shift;

    //! Bram array information 
    bibob_bramdump_t ** bram_arrays;

    float bram_max;

    unsigned n_brams;

    //! number of receiving host/ports
    unsigned num_dests;

    //! IP addresses of the receiving hosts
    char ** dest_ips;

    //! Ports of the receiving hosts
    unsigned * dest_ports;

    //! Struct for the command socket data
    caspsr_sock_t * sock;

    bibob_header_t * hdr;

  } bibob_t;


  /*! allocate and initialize a new bibob_t struct */
  bibob_t* bibob_construct ();

  /*! free all resources reserved for bibob communications */
  int bibob_destroy (bibob_t* bob);

  /*! set the host and port number of the bibob */
  int bibob_set_host (bibob_t* bob, const char* host, int port);

  /*! set the bibob number (use default IP base and port) */
  int bibob_set_number (bibob_t* bob, unsigned number);

  /*! open socket connection with currently configured bibob */
  int bibob_open (bibob_t* bob);

  /*! close socket connection with currently open bibob */
  int bibob_close (bibob_t* bob);

  /*! return true if already open */
  int bibob_is_open (bibob_t* bob);

  /*! return true if bibob is alive */
  int bibob_ping (bibob_t* bob);

  /*! reset packet counter on next UTC second, returned */
  time_t bibob_arm (bibob_t* bob);

  /*! configure the bibob */
  int bibob_configure (bibob_t* bob);

  /*! send message to bibob */
  ssize_t bibob_send (bibob_t* bob, const char* message);

  /*! receive response from bibob, writing to the buffer , return bytes read */
  ssize_t bibob_recv (bibob_t* bob);

  /*! receive and ignore the response from the bibob */
  ssize_t bibob_ignore (bibob_t* bob);

  /*! write bytes to bibob */
  ssize_t bibob_send (bibob_t* bob, const char* message);

  /*! sleep for msec milliseconds */
  void bibob_pause(int msec);

  /*! writes a bramdump into the pol?_bram arrays */
  int bibob_bramdump(bibob_t* bibob);

  /*! plot the bram informatino */
  int bibob_bramplot(bibob_t * bibob, char * device);

  /*! resets pol?_bram arrays and n_brams */
  void bibob_bramdump_reset(bibob_t* bob);

  /*! sets the levels on the bibob to optimal values */
  void * bibob_level_setter(void * context);

  /*! worker functions for bibob_level_setter */
  void extract_counts(char * bram, bibob_bramdump_t * bram_array, unsigned even);

  void calc_averages(bibob_bramdump_t * bram_array);

  void print_vals(bibob_bramdump_t * bram_array);

  int find_bit_window(long p1val, long p2val, unsigned  *gain1, unsigned* gain2);

  long calculate_max(long * array, unsigned size);

  void bibob_udp_header_encode (char * buffer, bibob_header_t* hdr);

  void bibob_udp_header_decode (char * buffer, bibob_header_t* hdr);

  bibob_bramdump_t * init_bram_array();

  int free_bram_array(bibob_bramdump_t * bram);

#ifdef __cplusplus
	   }
#endif

#endif
