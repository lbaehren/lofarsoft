/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrdada/psrdada/bpsr/src/multibob.h,v $
   $Revision: 1.8 $
   $Date: 2008/11/03 02:49:00 $
   $Author: ajameson $ */

#ifndef __DADA_MULTIBOB_H
#define __DADA_MULTIBOB_H

#include "ibob.h"
#include "command_parse_server.h"

/* ************************************************************************

   multibob_t - a struct and associated routines for creating and
   managing, as well as reading and writing to and from multiple iBoBs
   in a multi-threaded application.

   ************************************************************************ */

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {

    /*! An ibob */
    ibob_t* ibob;

    /*! for synchronization between command and monitor threads */
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_t id;

    /*! update bramdump log; otherwise just ping */
    char bramdump;

    /*! update bramdump log; otherwise just ping */
    char bramdisk;

    /*! quit flag */
    char quit;

    /*! time of last contact with iBoB */
    time_t alive;

    /*! lock thread used by main thread */
    pthread_t lock;

  } ibob_thread_t;


  typedef struct {

    /*! one thread for each iBoB */
    ibob_thread_t* threads;
    unsigned nthread;

    /*! command parser and server */
    command_parse_t* parser;
    command_parse_server_t* server;

    /*! port number on which to listen for incoming connections */
    int port;

  } multibob_t;


  /*! allocate and initialize a new ibob_t struct */
  multibob_t* multibob_construct (unsigned nibob);

  /*! free all resources reserved for ibob communications */
  int multibob_destroy (multibob_t* bob);

  /*! mutex lock all of the ibob interfaces */
  void multibob_lock (multibob_t* bob);

  /*! mutex unlock all of the ibob interfaces */
  void multibob_unlock (multibob_t* bob);

  /*! get the state of the instrument */
  int multibob_cmd_state (void* context, FILE* fptr, char* args);

  /*! set the host and port number of the specified ibob */
  int multibob_cmd_hostport (void* context, FILE* fptr, char* args);

  /*! set the host and port number of the specified ibob */
  int multibob_cmd_hostports (void* context, FILE* fptr, char* args);

  /*! set the target MAC address of the specified ibob */
  int multibob_cmd_mac (void* context, FILE* fptr, char* args);

  /*! set the target MAC address of the specified ibob */
  int multibob_cmd_macs (void* context, FILE* fptr, char* args);

  /*! open the command connections to all of the ibobs */
  int multibob_cmd_open (void* context, FILE* fptr, char* args);

  /*! close the command connections to all of the ibobs */
  int multibob_cmd_close (void* context, FILE* fptr, char* args);

  /*! reset packet counter on next UTC second, returned */
  int multibob_cmd_arm (void* context, FILE* fptr, char* args);

  /*! quit */
  int multibob_cmd_quit (void* context, FILE* fptr, char* args);

  /*! set accumultion length on all ibobs */
  int multibob_cmd_acclen (void* context, FILE* fptr, char* args);

  /*! set bit levels on all ibobs*/
  int multibob_cmd_levels (void* context, FILE* fptr, char* args);

  /*! dump accumulated bramdumps to disk */
  int bramdisk(ibob_t* thread);

#ifdef __cplusplus
	   }
#endif

#endif
