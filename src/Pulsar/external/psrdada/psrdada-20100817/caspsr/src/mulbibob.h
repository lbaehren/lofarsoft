/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrdada/psrdada/caspsr/src/mulbibob.h,v $
   $Revision: 1.1 $
   $Date: 2009/07/16 02:38:24 $
   $Author: ajameson $ */

#include "bibob.h"

#ifndef __DADA_MULBIBOB_H
#define __DADA_MULBIBOB_H

#include "command_parse_server.h"

/* ************************************************************************

   mulbibob_t - a struct and associated routines for creating and
   managing, as well as reading and writing to and from multiple BiBoBs
   in a multi-threaded application.

   ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {

    /*! An bibob */
    bibob_t* bibob;

    /*! for synchronization between command and monitor threads */
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_t id;

    /*! update bramdump log; otherwise just ping */
    char bramdump;

    /*! do a bramdisk */
    char bramdisk;

    /*! do a bramplot */
    char bramplot;

    /*! quit flag */
    char quit;

    /*! time of last contact with BiBoB */
    time_t alive;

    /*! lock thread used by main thread */
    pthread_t lock;

  } bibob_thread_t;


  typedef struct {

    /*! one thread for each BiBoB */
    bibob_thread_t* threads;
    unsigned nthread;

    /*! command parser and server */
    command_parse_t* parser;
    command_parse_server_t* server;

    /*! port number on which to listen for incoming connections */
    int port;

  } mulbibob_t;


  /*! allocate and initialize a new ibob_t struct */
  mulbibob_t* mulbibob_construct (unsigned nbibob);

  /*! free all resources reserved for ibob communications */
  int mulbibob_destroy (mulbibob_t* bob);

  /*! mutex lock all of the ibob interfaces */
  void mulbibob_lock (mulbibob_t* bob);

  /*! mutex unlock all of the ibob interfaces */
  void mulbibob_unlock (mulbibob_t* bob);

  /*! get the state of the instrument */
  int mulbibob_cmd_state (void* context, FILE* fptr, char* args);

  /*! set the host and port number of the specified ibob */
  int mulbibob_cmd_hostport (void* context, FILE* fptr, char* args);

  /*! set the host and port number of the specified ibob */
  int mulbibob_cmd_hostports (void* context, FILE* fptr, char* args);

  /*! set the target MAC address of the specified ibob */
  int mulbibob_cmd_mac (void* context, FILE* fptr, char* args);

  /*! set the target MAC address of the specified ibob */
  int mulbibob_cmd_macs (void* context, FILE* fptr, char* args);

  /*! set the gain of a channel and pol */
  int mulbibob_cmd_gain(void* context, FILE* fptr, char* args);

  /*! open the command connections to all of the ibobs */
  int mulbibob_cmd_open (void* context, FILE* fptr, char* args);

  /*! close the command connections to all of the ibobs */
  int mulbibob_cmd_close (void* context, FILE* fptr, char* args);

  /*! reset packet counter on next UTC second, returned */
  int mulbibob_cmd_arm (void* context, FILE* fptr, char* args);

  /*! quit */
  int mulbibob_cmd_quit (void* context, FILE* fptr, char* args);

  /*! list the devices */
  int mulbibob_cmd_listdev (void* context, FILE* fptr, char* args);

  /*! set bit levels on all ibobs*/
  int mulbibob_cmd_levels (void* context, FILE* fptr, char* args);

  /*! dump accumulated bramdumps to disk */
  int bramdisk(bibob_t* thread);

  /*! plot the accumulated accumulated bramdumps */
  int bramplot(bibob_t* thread, char * device);

  /*! reset the bram arrays */
  int bramreset(bibob_t* bibob);

#ifdef __cplusplus
	   }
#endif

#endif
