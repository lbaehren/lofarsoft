#ifndef __DADA_PWC_NEXUS_H
#define __DADA_PWC_NEXUS_H

/* ************************************************************************

   dada_pwc_nexus_t - a struct and associated routines for creation and
   management of a dada network

   ************************************************************************ */

#include "dada_pwc.h"
#include "nexus.h"
#include "monitor.h"
#include "string_array.h"

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {

    /*! the nexus communication base class */
    node_t node;

    /*! the unique header for this primary write client */
    char* header;

    /*! the size of the header */
    unsigned header_size;

    /*! the state of the node */
    dada_pwc_state_t state;

  } dada_pwc_node_t;


  typedef struct {

    /*! the nexus communication base class */
    nexus_t nexus;

    /*! the pwc communication server */
    dada_pwc_t* pwc;

    /*! the monitor of the mirror nexus */
    monitor_t* monitor;

    /*! the template header for all primary write clients */
    char* header_template;

    /*! pointer to function used to convert between time representations */
    struct tm* (*convert_to_tm) (const time_t* timep);

  } dada_pwc_nexus_t;

  /*! Create a new DADA PWC nexus */
  dada_pwc_nexus_t* dada_pwc_nexus_create ();

  /*! Destroy a DADA PWC nexus */
  int dada_pwc_nexus_destroy (dada_pwc_nexus_t* nexus);

  /*! Read the DADA PWC nexus configuration from the specified filename */
  int dada_pwc_nexus_configure (dada_pwc_nexus_t* nexus, const char* filename);

  /*! Serve PWC */
  int dada_pwc_nexus_serve (dada_pwc_nexus_t* nexus);

#ifdef __cplusplus
	   }
#endif

#endif
