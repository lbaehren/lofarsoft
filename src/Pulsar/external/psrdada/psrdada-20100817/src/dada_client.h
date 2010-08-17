#ifndef __DADA_CLIENT_H
#define __DADA_CLIENT_H

/* ************************************************************************

   dada_client_t - a struct and associated routines for creation and
   execution of a dada read or write client main loop

   ************************************************************************ */

#include "multilog.h"
#include "ipcio.h"

#ifdef __cplusplus
extern "C" {
#endif

  enum { dada_client_undefined, dada_client_reader, dada_client_writer };

  typedef struct dada_client {

    /*! The status and error logging interface */
    multilog_t* log;

    /*! The Data Block interface */
    ipcio_t* data_block;

    /*! The Header Block interface */
    ipcbuf_t* header_block;

    /*! Pointer to the function that opens the data transfer target */
    int (*open_function) (struct dada_client*);

    /*! Pointer to the function that transfers data to/from the target */
    int64_t (*io_function) (struct dada_client*, 
			    void* data, uint64_t data_size);

    /*! Pointer to the function that closes the data transfer target */
    int (*close_function) (struct dada_client*, uint64_t bytes_written);

    /*! Additional context information */
    void* context;

    /* The header to be transfered to the target */
    char* header;

    /* The size of the header */
    uint64_t header_size;

    /* When set, the header is transfered to/from the target */
    char header_transfer;

    /* The direction of data transfer */
    char direction;

    /* The open function should set the following three attributes */

    /*! The file descriptor of the data transfer target */
    int fd;

    /*! The total number of bytes to be transfered to/from the target */
    uint64_t transfer_bytes;

    /*! The optimal number of bytes to transfer to/from the target buffer */
    uint64_t optimal_bytes;

    /*! The quit flag */
    char quit;

  } dada_client_t;

  /*! Create a new DADA client main loop */
  dada_client_t* dada_client_create ();

  /*! Destroy a DADA client main loop */
  void dada_client_destroy (dada_client_t* client);

  /*! Run the DADA client read loop */
  int dada_client_read (dada_client_t* client);

  /*! Run the DADA client write loop */
  int dada_client_write (dada_client_t* client);

#ifdef __cplusplus
	   }
#endif

#endif
