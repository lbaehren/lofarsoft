#ifndef __DADA_MONITOR_H
#define __DADA_MONITOR_H

/* ************************************************************************

   monitor_t - a struct and associated routines for creation and
   management of a network monitor

   ************************************************************************ */

#include "nexus.h"
#include "multilog.h"
#include "dada_def.h"

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {

    /*! The nexus to be monitored */
    nexus_t* nexus;

    /*! The multilog to which all received messages are forwarded */
    multilog_t* log;

    /*! Pointer to function that handles incoming messages */
    int (*handle_message) (void* context, unsigned node, const char* message);

    /*! Additional context passed to message handling function */
    void* context;

  } monitor_t;

  /*! Create a new monitor */
  monitor_t* monitor_create ();

  /*! Destroy a monitor */
  int monitor_destroy (monitor_t* monitor);

  /*! Start another thread to monitor messages from the nexus */
  int monitor_launch (monitor_t* monitor);

#ifdef __cplusplus
	   }
#endif

#endif
