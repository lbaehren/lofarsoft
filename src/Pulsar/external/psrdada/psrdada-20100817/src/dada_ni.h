#ifndef __DADA_NI_H
#define __DADA_NI_H

/* ************************************************************************

   dada_ni_t - a struct and associated routines for creation and
   execution of a dada network interface

   ************************************************************************ */

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {

    /* the open socket file descriptor */
    int fd;

    /* pointer to the header of the current data transfer */
    char* header;

    /* the size of the header */
    unsigned header_size;

    /* total number of bytes of data to be transfered over network interface */
    uint64_t total_data;

    /* total number of bytes already transfered */
    uint64_t total_transfered;

    /* flag: -1=recv, 0=idle, 1=send */
    int state;

  } dada_ni_t;

  /*! Create a new DADA network interface */
  dada_ni_t* dada_ni_create (int fd);

  /*! Destroy a DADA network interface */
  void dada_ni_destroy (dada_ni_t* net);

  /*! Prepare for sending data over the DADA network interface */
  int dada_ni_open_send (dada_ni_t* net, uint64_t total_data);

  /*! Send data over the DADA network interface */
  int dada_ni_send (dada_ni_t* net, void* data, uint64_t data_size);

  /*! Prepare for receiving data over the DADA network interface */
  int dada_ni_open_recv (dada_ni_t* net);

  /*! Receive data over the DADA network interface */
  int dada_ni_recv (dada_ni_t* net, void* data, uint64_t data_size);

#ifdef __cplusplus
	   }
#endif

#endif
