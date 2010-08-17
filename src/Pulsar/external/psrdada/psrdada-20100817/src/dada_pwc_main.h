#ifndef __DADA_PWC_MAIN_H
#define __DADA_PWC_MAIN_H

/* ************************************************************************

   dada_pwc_main_t - a struct and associated routines for creation and
   execution of a dada primary write client main loop

   ************************************************************************ */

#include "dada_pwc.h"
#include "multilog.h"
#include "ipcio.h"

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct dada_pwc_main {

    /*! The primary write client control connection */
    dada_pwc_t* pwc;

    /*! The status and error logging interface */
    multilog_t* log;

    /*! The Data Block interface */
    ipcio_t* data_block;

    /*! The Header Block interface */
    ipcbuf_t* header_block;

    /*! Pointer to the function that starts data transfer */
    time_t (*start_function) (struct dada_pwc_main*, time_t utc);

    /*! Pointer to the function that returns a data buffer */
    void* (*buffer_function) (struct dada_pwc_main*, uint64_t* size);

    /*! Pointer to the function that stops data transfer */
    int (*stop_function) (struct dada_pwc_main*);

    /*! Pointer to the function that handles data transfer error*/
    int (*error_function) (struct dada_pwc_main*);

    /*! Pointer to the function that checks header validity */
    int (*header_valid_function) (struct dada_pwc_main*);

    /*! Additional context information */
    void* context;

    /*! The current command from the PWC control connection */
    dada_pwc_command_t command;

    /*! the unique header for this primary write client */
    char* header;

    /*! the size of the header */
    unsigned header_size;

    /*! verbosity level */
    int verbose;

    /*! flag to determine if we have a valid header */
    int header_valid;

  } dada_pwc_main_t;

  /*! Create a new DADA primary write client main loop */
  dada_pwc_main_t* dada_pwc_main_create ();

  /*! Destroy a DADA primary write client main loop */
  void dada_pwc_main_destroy (dada_pwc_main_t* pwcm);

  /*! Run the DADA primary write client main loop */
  int dada_pwc_main (dada_pwc_main_t* pwcm);

  /*! process error value, and set state accordingly */
  void dada_pwc_main_process_error(dada_pwc_main_t* pwcm, int rval); 


#ifdef __cplusplus
	   }
#endif

#endif
